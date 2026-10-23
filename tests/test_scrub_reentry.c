/* Regression for v0.9.9's negative-weight, last-output-feedback buzz.
   Includes the real controller transport and DSP, not a toy copy. */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../src/controller.c"

static Client client;
static Sample samples[2];
static float wave[2][48000];
static float out[128*2];
static void require(int ok,const char*msg){
 if(!ok){fprintf(stderr,"FAIL: %s\n",msg);exit(1);}
 printf("PASS: %s\n",msg);
}
static void tick_audio(unsigned blocks){
 for(unsigned b=0;b<blocks;b++){
  generate_audio(&client,out,128);
  for(unsigned k=0;k<128*2;k++)require(isfinite(out[k]),"output must be finite");
 }
}
int main(void){
 client.sampleRate=48000;
 for(int p=0;p<2;p++){
  for(int j=0;j<48000;j++){
   wave[p][j]=(float)(.24*sin((double)j*(p+1)*.027)+.025*sin((double)j*.141));
  }
  samples[p].mono=wave[p];samples[p].frames=48000;
  samples[p].sampleRate=48000;samples[p].refs=100;
  g_pads[p].sample=&samples[p];g_pads[p].selStart=0;
  g_pads[p].selEnd=1;g_pads[p].volumeHalfPct=200;
 }
 add_voice(&client,0);add_voice(&client,1);
 for(int b=0;b<7;b++)generate_audio(&client,out,128);
 double otherPos=client.voices[1].pos;
 require(otherPos>0,"independent second pad initially playing");

 /* Reenter from playback, then from held pause, then from resumed playback. */
 for(int cycle=0;cycle<9;cycle++){
  WL_I32 ppm=100000+cycle*75000;
  double anchor=(double)ppm*.000001*48000.;
  g_scratchPad=0;g_scratchPosPpm=ppm;g_scrubPosPpm[0]=ppm;
  g_scrubHasCursor[0]=1;g_scratchHold=1;g_scrubState[0]=3;
  generate_audio(&client,out,128);
  require(client.voices[0].scratch,"click enters scratch");
  require(fabs(client.voices[0].pos-anchor)<1.0,"click immediately reanchors scratch position");
  /* First entry frame MUST be decoded PCM at the click: no stale lastOutput.
     Prior v0.9.9 yields ~0 or a large negative-weight blend here. */
  float reference=cubic_array_sample(wave[0],48000,anchor);
  /* Compare against isolated selected pad by subtracting the second voice. */
  double p1=client.voices[1].pos-128.;
  float isolated=out[0]-cubic_sample(wave[1],48000,0,48000,p1);
  require(fabsf(isolated-reference)<.015f,"first scratch-entry sample is original PCM at click");
  float peak=0.;
  for(int b=0;b<16;b++){
   generate_audio(&client,out,128);
   for(int j=0;j<128;j++){
    float a=fabsf(out[j*2]);
    if(a>peak)peak=a;
    if(!isfinite(a)){fprintf(stderr,"non-finite output\n");return 1;}
   }
  }
  require(peak<.80f,"entry-fade output peak remains safely bounded");
  g_scrubPosPpm[0]=ppm;g_scratchHold=0;g_scrubState[0]=1;
  for(int b=0;b<4;b++)generate_audio(&client,out,128);
  require(fabs(client.voices[0].pos-anchor)<1.,"release freezes cursor position");
  if(cycle%3==2){g_scrubState[0]=2;generate_audio(&client,out,128);
    require(client.voices[0].pos>anchor,"Play resumes from parked cursor");}
 }
 require(client.voices[1].pos>otherPos,"other playing pad continues through all drags");
 /* Also cover the no-active-voice path: a new voice starts under Scrub. */
 static Client fresh;
 fresh.sampleRate=48000;
 g_scratchPad=0;g_scratchHold=1;g_scrubState[0]=3;
 g_scratchPosPpm=570000;g_scrubPosPpm[0]=570000;
 require(add_voice(&fresh,0),"new voice opens directly in Scrub");
 generate_audio(&fresh,out,128);
 double clicked=.570000*48000.;
 require(fabs(fresh.voices[0].pos-clicked)<1.,"new voice remains anchored at click");
 require(fabsf(out[0]-cubic_array_sample(wave[0],48000,clicked))<.015f,
         "new-voice entry uses decoded PCM instead of an old output buffer");
 require(fresh.voices[0].scratchAttack>=(48000/30)-128-1,
         "new-voice attack uses the full 33ms fade");
 puts("ALL REPEATED SCRUB ENTRY TESTS PASSED");
 return 0;
}
