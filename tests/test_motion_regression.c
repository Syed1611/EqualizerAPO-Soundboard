#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "src/scratch_dsp.h"
#define SR 48000
#define PI 3.1415926535897932384626433832795
static double rms(const float*s,int a,int b){double v=0;for(int i=a;i<b;i++)v+=(double)s[i]*s[i];return sqrt(v/(double)(b-a));}
static double mean_frequency(const float*s,int n,int start){
 /* A zero-crossing counter for isolated calibration tones. */
 int edges=0;for(int i=start+1;i<start+n;i++)if(s[i-1]<=0&&s[i]>0)edges++;
 return (double)edges*SR/(double)n;
}
int main(void){
 const int inputN=SR*4, outputN=SR*6;
 float*in=calloc(inputN,sizeof(float)),*out=calloc(outputN,sizeof(float));
 ApoScratchStretch*dsp=calloc(1,sizeof(*dsp));
 if(!in||!out||!dsp)return 1;
 for(int i=0;i<inputN;i++){
  double t=(double)i/SR;
  double f=(i<(int)(1.8*SR))?180.0:440.0;
  in[i]=(float)(.43*sin(2*PI*f*t));
 }
 scratch_stretch_init(dsp,0.70*SR,SR,SR);
 double largestJump=0,peak=0,energy=0;
 for(int i=0;i<outputN;i++){
  double cursor,ratio=1.0;
  if(i<SR)cursor=.70*SR;
  else if(i<2*SR)cursor=.70*SR+2.0*(double)(i-SR);
  else if(i<3*SR)cursor=2.70*SR;
  else if(i<4*SR)cursor=2.70*SR-2.0*(double)(i-3*SR);
  else if(i<5*SR)cursor=.70*SR;
  else{cursor=.70*SR;ratio=1.5;}
  /* Mouse direction is deliberately derived only from cursor values. */
  float x=scratch_stretch_render(dsp,in,inputN,SR,SR,cursor,0.0,ratio);
  if(!isfinite(x))return 2;
  out[i]=x;
  double a=fabs(x);if(a>peak)peak=a;energy+=x*x;
  if(i>0){double jump=fabs(out[i]-out[i-1]);if(jump>largestJump)largestJump=jump;}
 }
 FILE*f=fopen("motion_regression.raw","wb");if(f){fwrite(out,sizeof(float),outputN,f);fclose(f);}
 double early=mean_frequency(out,SR/2,SR/4),middle=mean_frequency(out,SR/2,2*SR+SR/4);
 double reverse=mean_frequency(out,SR/6,3*SR+SR/6);
 double final=mean_frequency(out,SR/2,5*SR+SR/4);
 printf("moving spectral trace: early=%.1fHz late=%.1fHz reverse=%.1fHz pitched=%.1fHz jump=%.3f peak=%.3f rms=%.3f\n",early,middle,reverse,final,largestJump,peak,sqrt(energy/outputN));
 if(fabs(early-180)>4||fabs(middle-440)>10||fabs(reverse-440)>25||fabs(final-270)>7)return 3;
 if(largestJump>.24||peak>=.98)return 4;
 if(rms(out,0,SR/2)<.12||rms(out,2*SR,2*SR+SR/2)<.12)return 5;
 /* Previous architecture had seam alignment and a moving 78ms grain.
    Their state fields no longer exist in ApoScratchStretch. */
 free(out);free(in);free(dsp);return 0;
}
