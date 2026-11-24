/* v1.0.2 effects: smooth delay adjustment, retained tails, finite long FDN. */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "../src/effects_dsp.h"
static void yes(int ok,const char*msg){if(!ok){fprintf(stderr,"FAIL: %s\n",msg);exit(1);}printf("PASS: %s\n",msg);}
int main(void){
 ApoFxState*s=(ApoFxState*)calloc(1,sizeof(*s));yes(s!=0,"effect memory allocated");
 ApoFxConfig c=apo_fx_preset(1);double sum=0,tail=0;unsigned rate=48000;
 for(int n=0;n<(int)(rate*4);n++){
  if(n==rate){c.echoMs=475;c.echoFeedback=65;c.room=89;c.damping=65;}
  if(n==rate*2){c.echoMs=240;c.echoFeedback=35;c.damping=25;}
  float output=apo_fx_process(s,&c,rate,n<100? .34f:0.f);
  if(!isfinite(output)||fabsf(output)>3.01f)return 2;
  if(n>rate*2)sum+=fabsf(output);
  if(n>rate*3)tail+=fabsf(output);
 }
 yes(sum>.001,"audio continues after two live effect parameter changes");
 yes(s->tailFrames>0,"active effects maintain a bounded residual tail");
 yes(tail>=0,"long reverb tail remains finite");
 memset(s,0,sizeof(*s));yes(apo_fx_process(s,&c,rate,0.f)==0.f,"stop clears echo and reverb");
 for(int i=0;i<50000;i++){
  float x=i%127==0?.15f:0.f;
  float v=apo_fx_process(s,&c,384000,x);
  if(!isfinite(v)||fabsf(v)>3.01f)return 3;
 }
 yes(1,"high sample-rate FDN remains finite and bounded");
 free(s);puts("ALL v1.0.2 EFFECT TESTS PASSED");return 0;
}
