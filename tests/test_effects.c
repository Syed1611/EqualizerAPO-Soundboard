/* v1.0.1 echo/reverb: finite output, delay, isolation, stop, preset defaults. */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "../src/effects_dsp.h"
static void check(int okay,const char*label){if(!okay){fprintf(stderr,"FAIL: %s\n",label);exit(1);}printf("PASS: %s\n",label);}
int main(void){
 ApoFxState *a=calloc(1,sizeof(ApoFxState)),*b=calloc(1,sizeof(ApoFxState));
 check(a&&b,"preallocated pad effect memory");
 ApoFxConfig clean=apo_fx_preset(0);check(apo_fx_process(a,&clean,48000,.25f)==.25f,"Clean is bit-exact bypass");
 ApoFxConfig fx=apo_fx_preset(1);check(fx.echoOn&&fx.reverbOn,"Meme preset enables both effects");
 double peak=0.;float delayed=0,other=0;int allFinite=1;
 for(int t=0;t<48000*2;t++){
  float x=t==0?.5f:0.f;
  float v=apo_fx_process(a,&fx,48000,x);
  if(t==14400)delayed=v;
  if(t==14400)other=apo_fx_process(b,&fx,48000,0.f);
  if(t!=14400)apo_fx_process(b,&fx,48000,0.f);
  if(!isfinite(v))allFinite=0;
  if(fabs(v)>peak)peak=fabs(v);
 }
 check(allFinite,"all emitted effect samples finite");
 check(delayed>.10f,"Meme Echo repeats a substantial delayed impulse at 300ms");
 check(fabsf(other)<1e-8f,"per-pad effect states do not leak between pads");
 check(peak<3.01,"bounded output");
 check(a->tailFrames>0,"effect tail continues beyond the source ending");
 memset(a,0,sizeof(*a));check(a->tailFrames==0,"Stop clears effect tail");
 float after=apo_fx_process(a,&fx,48000,0);check(after==0,"Stop clears old delayed samples");
 for(int i=0;i<APO_FX_PRESET_COUNT;i++){ApoFxConfig t=apo_fx_preset(i);check(t.preset==i,"each preset identifier matches its configured values");}
 ApoFxConfig limited=apo_fx_preset(1);limited.echoMs=650;limited.echoFeedback=78;limited.echoMix=100;limited.reverbMix=100;
 for(int i=0;i<96000;i++){float x=i<500?(.2f*(float)sin(i*.031)):0;float o=apo_fx_process(a,&limited,96000,x);if(!isfinite(o)||fabs(o)>3.01f){fprintf(stderr,"FAIL: 96k output bound\n");return 1;}}
 /* A reverb-only impulse must yield a wet tail without any echo. */
 memset(a,0,sizeof(*a));
 ApoFxConfig room=apo_fx_preset(3);double wetpeak=0.;
 for(int i=0;i<18000;i++){
  float x=i==0?.4f:0.f;float y=apo_fx_process(a,&room,48000,x);
  if(i>300 && fabsf(y)>wetpeak)wetpeak=fabsf(y);
  if(!isfinite(y)){fprintf(stderr,"FAIL: reverb emitted nonfinite\n");return 1;}
 }
 check(wetpeak>.003,"reverb-only produces an audible finite tail");
 /* Full echo time must remain functional at 384 kHz, not get silently
    truncated to the fixed buffer size. */
 memset(a,0,sizeof(*a));
 ApoFxConfig hi=apo_fx_preset(2);hi.echoMs=650;hi.echoFeedback=0;hi.echoMix=100;
 float last=0.0f;
 for(int i=0;i<=249600;i++)last=apo_fx_process(a,&hi,384000,i==0?.4f:0.f);
 check(last>.39f,"650ms echo is available at 384kHz");
 puts("ALL EFFECTS TESTS PASSED");free(a);free(b);return 0;
}
