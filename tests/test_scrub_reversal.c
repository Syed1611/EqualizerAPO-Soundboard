/* v1.0.1: ensure direct-PCM scratching fades toward spectral at a direction change. */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "src/scratch_dsp.h"
#define SR 48000u
#define PI 3.1415926535897932384626433832795
int main(void){
 float *src=calloc(SR*4,sizeof(*src));ApoScratchStretch *d=calloc(1,sizeof(*d));
 if(!src||!d)return 1;
 for(unsigned i=0;i<SR*4;i++)src[i]=(float)(.4*sin(PI*2.*220.*i/SR));
 double cursor=SR;
 scratch_stretch_init(d,cursor,SR,SR);
 for(int i=0;i<10000;i++){
  cursor+=1.0;
  float y=scratch_stretch_render(d,src,SR*4,SR,SR,cursor,1.0,1.0);
  if(!isfinite(y)||fabsf(y)>1.5)return 2;
 }
 double original=d->spec.directMix;
 if(original<.20){fprintf(stderr,"FAIL: direct source mode never engaged: %.4f\n",original);return 3;}
 unsigned flagged=0;double maxStep=0,previous=0;
 for(int i=0;i<2000;i++){
  cursor-=1.0;
  float y=scratch_stretch_render(d,src,SR*4,SR,SR,cursor,-1.0,1.0);
  if(!isfinite(y)||fabsf(y)>1.5)return 4;
  if(d->spec.reversalFrames>0)flagged++;
  if(i){double step=fabs(y-previous);if(step>maxStep)maxStep=step;}
  previous=y;
 }
 if(!flagged){fprintf(stderr,"FAIL: reversal fade never activated\n");return 5;}
 if(maxStep>.8){fprintf(stderr,"FAIL: sharp sample discontinuity %.4f\n",maxStep);return 6;}
 printf("PASS: reverse fade active (%u frames observed), direct mix before %.3f, peak output step %.3f\n",flagged,original,maxStep);
 free(d);free(src);return 0;
}
