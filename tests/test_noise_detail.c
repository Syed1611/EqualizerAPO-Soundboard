#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "src/scratch_dsp.h"
#define SR 48000
int main(){
 int n=SR*4,m=SR*2;
 float*s=calloc(n,sizeof(float));ApoScratchStretch*v=calloc(1,sizeof(*v));if(!s||!v)return 1;
 unsigned state=0x123456u;
 for(int i=0;i<n;i++){
  state^=state<<13;state^=state>>17;state^=state<<5;
  float z=(float)((double)(state&0xffff)/32768.0-1.0);
  s[i]=(float)(.25*sin(2*3.141592653589793*190*i/SR)+.12*z);
 }
 scratch_stretch_init(v,SR,SR,SR);
 double energy=0,peak=0,jump=0;float prev=0;
 for(int i=0;i<m;i++){
  double cursor=SR+(i<SR/2?i*.67:(i<SR?SR*.335:(i<SR*3/2?SR*.335-(i-SR)*.8:-SR*.065)));
  float z=scratch_stretch_render(v,s,n,SR,SR,cursor,0,1);
  if(!isfinite(z))return 2;
  energy+=z*z;
  if(fabs(z)>peak)peak=fabs(z);
  if(i&&fabs(z-prev)>jump)jump=fabs(z-prev);
  prev=z;
 }
 double rms=sqrt(energy/m);
 printf("tonal+broadband moving scratch rms=%.4f peak=%.4f largestJump=%.4f\n",rms,peak,jump);
 free(v);free(s);
 return rms<.03||peak>=.98||jump>.48?3:0;
}
