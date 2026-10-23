#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "src/scratch_dsp.h"
#define SR 48000
#define N (SR*4)
int main(void){
 float *src=malloc(N*sizeof(float));if(!src)return 1;
 for(int i=0;i<N;i++){
  double t=(double)i/SR;
  src[i]=(float)(0.37*sin(2*3.14159265358979323846*(174*t+3*t*t))
       +0.13*sin(2*3.14159265358979323846*(337*t+0.6*t*t)));
 }
 ApoScratchStretch *st=calloc(1,sizeof(*st));if(!st)return 2;
 scratch_stretch_init(st,SR,SR,SR);
 unsigned long long lastCount=0,finalCount=0;
 double firstCursor=0,lastCursor=0,rms=0,maxJump=0,prev=0;
 for(int i=0;i<SR*4;i++){
  double cursor=SR,speed=0,pitch=1.0;
  if(i>=SR&&i<SR*2){cursor=SR+(i-SR)*0.45;speed=0.45;}
  else if(i>=SR*2)cursor=SR*1.45;
  if(i>=SR*3)pitch=1.5;
  float f=scratch_stretch_render(st,src,N,SR,SR,cursor,speed,pitch);
  if(!isfinite(f))return 3;
  rms+=f*f;
  double jump=fabs((double)f-prev);if(jump>maxJump)maxJump=jump;prev=f;
  if(i==SR/3){firstCursor=st->freezeCursor;lastCount=st->spec.outputFrames;}
  if(i==SR-1){if(st->spec.outputFrames<=lastCount+SR/2||st->freezeCursor!=firstCursor)return 4;}
  if(i==SR*2+SR/3){lastCursor=st->freezeCursor;lastCount=st->spec.outputFrames;}
  if(i==SR*3-1){if(st->spec.outputFrames<=lastCount+SR/2||st->freezeCursor!=lastCursor)return 5;}
  if(i==SR*4-1)finalCount=st->spec.outputFrames;
 }
 rms=sqrt(rms/(SR*4));
 printf("spectral freeze hold->move->hold: RMS=%.5f peak_jitter=%.5f synthesis_count=%llu firstCursor=%f secondCursor=%f\n",rms,maxJump,finalCount,firstCursor,lastCursor);
 if(!(lastCursor>firstCursor+SR*.4)||!(rms>.05)||!(maxJump<.6)||!(finalCount>SR))return 6;
 free(st);free(src);return 0;
}
