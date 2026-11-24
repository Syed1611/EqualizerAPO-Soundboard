#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "../src/effects_dsp.h"
static void check(int ok,const char*s){if(!ok){fprintf(stderr,"FAIL %s\n",s);exit(1);}puts(s);}
int main(void){
 float mem=0;double e=0;for(int i=0;i<1024;i++){float y=apo_fx_diffuse(i==0?1.f:0.f,&mem);e+=(double)y*y;}
 printf("Diffuser impulse energy %.9f (unity expected)\n",e);check(fabs(e-1)<1e-6,"Correct allpass preserves impulse energy");
 ApoFxState*s=calloc(1,sizeof(*s));check(s!=0,"Allocated tank");
 unsigned rates[]={44100,48000,96000};for(int j=0;j<3;j++){
 unsigned rate=rates[j];memset(s,0,sizeof(*s));ApoFxConfig c=apo_fx_preset(3);c.room=100;c.damping=0;c.reverbMix=100;float max=0,wet=0,l,r;
 for(unsigned i=0;i<rate*4;i++){float d=i<rate*2?1.4f*sinf(2*3.14159265f*197*i/rate)+.4f*sinf(2*3.14159265f*503*i/rate):0.f;apo_fx_process_stereo(s,&c,rate,d,&l,&r);if(!isfinite(l)||!isfinite(r))return 2;float pk=fmaxf(fabsf(l),fabsf(r));if(pk>max)max=pk;if(i>=rate*2&&pk>wet)wet=pk;}
 printf("%u Hz hot-input output peak %.4f; tail peak %.4f\n",rate,max,wet);check(max<4&&wet>.001f&&wet<1.5f,"Hot reverb input remains finite with bounded nonzero tail");
 }
 free(s);return 0;
}
