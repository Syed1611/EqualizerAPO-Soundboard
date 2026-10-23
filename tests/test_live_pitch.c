#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "src/scratch_dsp.h"
int main(void){
 const int rate=48000,n=rate*4;
 float*in=(float*)malloc((size_t)n*sizeof(float));if(!in)return 1;
 for(int i=0;i<n;i++)in[i]=0.55f*(float)sin(2.0*3.141592653589793*220.0*i/(double)rate);
 ApoScratchStretch dsp={0};scratch_stretch_init(&dsp,rate*2,rate,rate);
 FILE*f=fopen("live_pitch.raw","wb");if(!f)return 2;
 double peak=0;int clip=0;
 for(int i=0;i<rate*3;i++){
  double p=i<rate?1.0:(i<rate*2?1.5:0.75);
  float v=scratch_stretch_render(&dsp,in,n,rate,rate,rate*2,0,p);
  if(!isfinite(v))return 3;
  if(fabs((double)v)>peak)peak=fabs((double)v);
  if(fabs((double)v)>1.0)clip++;
  fwrite(&v,4,1,f);
 }
 printf("live pitch test peak=%0.4f clipped=%d\n",peak,clip);
 fclose(f);free(in);return 0;
}
