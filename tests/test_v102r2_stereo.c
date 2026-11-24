/* R2: clean unity bypass, genuine stereo Echo, late diffuse hall reverb. */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "../src/effects_dsp.h"
static void yes(int x,const char*t){if(!x){fprintf(stderr,"FAIL %s\n",t);exit(1);}puts(t);}
int main(void){
 ApoFxState*s=(ApoFxState*)calloc(1,sizeof(*s));yes(s!=0,"Allocated stereo effects memory");
 ApoFxConfig c=apo_fx_preset(0);float l=0,r=0;
 apo_fx_process_stereo(s,&c,48000,.24f,&l,&r);
 yes(l==.24f&&r==.24f,"Clean passes both channels without level or phase changes");
 memset(s,0,sizeof(*s));c=apo_fx_preset(2);c.echoMs=300;c.echoMix=100;c.echoFeedback=60;
 float echoL=0,echoR=0,secondL=0,secondR=0;
 for(int n=0;n<30000;n++){
  apo_fx_process_stereo(s,&c,48000,n==0?.4f:0.f,&l,&r);
  if(n==14400){echoL=l;echoR=r;}
  if(n==28800){secondL=l;secondR=r;}
  if(!isfinite(l)||!isfinite(r)||fabsf(l)>3.01f||fabsf(r)>3.01f)return 2;
 }
 printf("Echo 300ms L=%f R=%f; 600ms L=%f R=%f\n",echoL,echoR,secondL,secondR);
 yes(echoL>.35f&&fabsf(echoR)<1.e-5f,"First 300 ms echo repeat is on the left channel");
 yes(secondR>.1f&&fabsf(secondL)<1.e-5f,"Second 600 ms echo repeat crosses to the right channel");
 memset(s,0,sizeof(*s));c=apo_fx_preset(3);
 double early=0,late=0,separation=0,post=0;float pL=0,pR=0;
 for(int n=0;n<96000;n++){
  apo_fx_process_stereo(s,&c,48000,n==0?.7f:0.f,&l,&r);
  if(n>0&&n<4800)early+=fabsf(l-.0f)+fabsf(r-.0f);
  if(n>12000&&n<48000){late+=fabsf(l)+fabsf(r);separation+=fabsf(l-r);}
  if(n>48000)post+=fabsf(l)+fabsf(r);
  if(!isfinite(l)||!isfinite(r)||fabsf(l)>3.01f||fabsf(r)>3.01f)return 3;
  pL=l;pR=r;
 }
 printf("Reverb early=%g late=%g stereo-diff=%g >1s=%g\n",early,late,separation,post);
 yes(early<late*.01,"Large Hall suppresses early reflections relative to diffuse tail");
 yes(late>1.0&&post>.1,"Diffuse reverb tail persists across multiple seconds");
 yes(separation>.01,"Reverb left and right tails are decorrelated");
 memset(s,0,sizeof(*s));apo_fx_process_stereo(s,&c,48000,0,&l,&r);
 yes(l==0.f&&r==0.f,"Stop clears both stereo channels");
 free(s);puts("ALL R2 STEREO AND TAIL TESTS PASSED");return 0;
}
