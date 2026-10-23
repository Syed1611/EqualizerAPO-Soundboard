#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "src/scratch_dsp.h"
#define SR 48000
#define SOURCE_FRAMES (SR*5)
#define PI 3.1415926535897932384626433832795
int main(void){
 float*src=(float*)calloc(SOURCE_FRAMES,sizeof(float));
 if(!src)return 1;
 for(int i=0;i<SOURCE_FRAMES;i++){
  double t=(double)i/SR;
  src[i]=(float)(0.31*sin(2*PI*220*t)+0.14*sin(2*PI*440*t+0.2)+0.08*sin(2*PI*770*t+0.7));
 }
 ApoScratchStretch*st=(ApoScratchStretch*)calloc(1,sizeof(*st));if(!st)return 2;
 scratch_stretch_init(st,2.0*SR,SR,SR);
 unsigned long long heldSample=0,afterMove=0;
 double energy=0,prev=0,maxJump=0,peak=0;
 FILE*f=fopen("spectral_test.raw","wb");if(!f)return 3;
 clock_t before=clock();
 for(int i=0;i<SR*5;i++){
  double cursor=2.0*SR,speed=0,ratio=1;
  if(i>=SR*2&&i<SR*3){cursor=2.0*SR+(double)(i-SR*2)*0.65;speed=.65;}
  else if(i>=SR*3)cursor=2.65*SR;
  if(i>=SR*4)ratio=1.5;
  float x=scratch_stretch_render(st,src,SOURCE_FRAMES,SR,SR,cursor,speed,ratio);
  if(!isfinite(x)){fprintf(stderr,"NaN at %d\n",i);return 4;}
  if(fabs((double)x)>peak)peak=fabs((double)x);
  if(i&&fabs((double)x-prev)>maxJump)maxJump=fabs((double)x-prev);
  prev=x;energy+=(double)x*x;fwrite(&x,4,1,f);
  if(i==SR)heldSample=st->spec.outputFrames;
  if(i==SR*3+SR/2)afterMove=st->spec.outputFrames;
  if(i>SR&&i<SR*2&&st->spec.outputFrames<=heldSample)return 5;
 }
 fclose(f);
 double sec=(double)(clock()-before)/CLOCKS_PER_SEC;
 printf("spectral hold test rms %.5f max_jump %.5f peak %.5f cpu_time %.3f sec holdSamples=%llu relockSamples=%llu fullSpectrumBins=%u\n",sqrt(energy/(SR*5)),maxJump,peak,sec,heldSample,afterMove,APO_SPECTRAL_HALF+1);
 if(!(energy>100.0)||!(maxJump<.6)||!(peak<.98)||!heldSample||!afterMove)return 6;
 free(st);free(src);return 0;
}
