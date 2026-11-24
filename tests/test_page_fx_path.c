/* Regression: Page 2 always exposes IDs 17-32; effects reach actual mixer. */
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "../src/controller.c"
static Client c;
static ApoFxState fx[PAD_COUNT];
static float impulse[4096];
static Sample sample;
static void check(int v,const char*s){if(!v){fprintf(stderr,"FAIL %s\n",s);exit(1);}printf("PASS %s\n",s);}
int main(void){
 for(int page=0;page<2;page++){
  int seen=0;
  for(int p=0;p<PAD_COUNT;p++){
   int want=(p>=page*LEGACY_PAD_COUNT && p<(page+1)*LEGACY_PAD_COUNT);
   int got=page_shows_pad(page,p);
   check(got==want,"button-to-page mapping agrees with pad index");
   seen+=got;
  }
  check(seen==16,"exactly 16 visible pad windows per page");
 }
 impulse[0]=.45f;sample.mono=impulse;sample.frames=4096;sample.sampleRate=48000;sample.refs=100;
 g_selected=17;g_padPage=1;
 g_pads[17].sample=&sample;g_pads[17].selStart=0;g_pads[17].selEnd=1;
 g_pads[17].volumeHalfPct=200;g_pads[17].pitchHalf=0;
 g_fxSettings[17]=apo_fx_preset(1);g_fxSerial[17]=5;
 c.sampleRate=48000;c.fx=fx;c.fxSeen[17]=5;
 g_triggerSerial[17]=1;
 float nearEcho=0,preEcho=0;int tailAfterClip=0;
 for(int start=0;start<20000;start+=256){
  int active=generate_audio(&c,c.audio,256);
  check(active,"echo tail continues through 20,000 frame playback");
  if(start==14336)nearEcho=c.audio[(14400-start)*2];
  if(start==8192)preEcho=c.audio[0];
  if(start==4096)tailAfterClip=(c.fx[17].tailFrames>0);
 }
 check(tailAfterClip,"pad17 echo/reverb continues after dry clip ends");
 check(nearEcho>.13f,"Meme Echo repeats real pad 17 audio after 300ms");
 check(fabsf(preEcho)<.2f,"early tail decays before main echo");
 check(g_activeVoices[17]==0,"source playback ended while echo tail continued");
 g_stopSerial=1;
 check(!generate_audio(&c,c.audio,256),"Stop All clears effect tails in the mixer");
 puts("ALL LIVE MIXER/EFFECT ROUTING CHECKS PASSED");return 0;
}
