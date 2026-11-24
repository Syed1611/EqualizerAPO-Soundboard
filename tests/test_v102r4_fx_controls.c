/* New R4 Echo/Reverb parameter smoothing checks; existing tests cover
   stereo timing, stop behavior, finite output, and full sample-rate bounds. */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "../src/effects_dsp.h"
static void check(int state,const char*msg){if(!state){fprintf(stderr,"FAIL: %s\n",msg);exit(1);}puts(msg);}
int main(void){
 ApoFxState*s=(ApoFxState*)calloc(1,sizeof(*s));check(s!=0,"Effects state allocated");
 ApoFxConfig echo=apo_fx_preset(2);echo.echoMs=200;echo.echoFeedback=0;echo.echoMix=100;
 float l=0,r=0;for(int t=0;t<48000;t++)apo_fx_process_stereo(s,&echo,48000,.2f,&l,&r);
 float before=l;
 echo.echoMix=0;
 apo_fx_process_stereo(s,&echo,48000,.2f,&l,&r);
 printf("Echo wet mix: before=%f firstAfter=%f\n",before,l);
 check(fabsf(l-before)<.05f,"Reducing Echo Mix to zero does not hard-cut the first sample");
 for(int i=0;i<2400;i++){apo_fx_process_stereo(s,&echo,48000,.2f,&l,&r);if(!isfinite(l)||!isfinite(r))return 2;}
 check(fabsf(l-.2f)<.04f,"Echo reaches dry level after smoothing");
 memset(s,0,sizeof(*s));ApoFxConfig rev=apo_fx_preset(3);
 for(int i=0;i<12000;i++)apo_fx_process_stereo(s,&rev,48000,i==0?.7f:0,&l,&r);
 float prior=s->fbGain[3];rev.room=95;apo_fx_process_stereo(s,&rev,48000,0,&l,&r);
 check(s->fbGain[3]>prior && s->fbGain[3]<s->fbGainTarget[3],"Reverb Decay gain ramps instead of jumping");
 rev.damping=85;apo_fx_process_stereo(s,&rev,48000,0,&l,&r);
 check(s->dampingSmooth<.85f,"Damping mini-bar changes smoothly");
 rev.reverbMix=0;apo_fx_process_stereo(s,&rev,48000,0,&l,&r);
 check(s->reverbMixSmooth>0.f,"Reverb tail is not hard-cut when mix reaches zero");
 free(s);
 puts("ALL R4 EFFECT PARAMETER TESTS PASSED");return 0;
}
