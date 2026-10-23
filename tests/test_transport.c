#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../src/controller.c"
static Client c;
static Sample sm[2];
static float pcm[2][48000];
static void assert_transport(int yes,const char*name){if(!yes){printf("FAIL %s\n",name);exit(1);}printf("PASS %s\n",name);}
int main(){
 c.sampleRate=48000;
 for(int p=0;p<2;p++){
  for(int i=0;i<48000;i++)pcm[p][i]=.1f*(float)sin((double)i*.02*(p+1));
  sm[p].mono=pcm[p];sm[p].frames=48000;sm[p].sampleRate=48000;sm[p].refs=100;
  g_pads[p].sample=&sm[p];g_pads[p].selStart=0;g_pads[p].selEnd=1;g_pads[p].volumeHalfPct=200;
 }
 g_playPosPpm[0]=g_playPosPpm[1]=-1;
 add_voice(&c,0);add_voice(&c,1);
 float output[256*2];
 for(int i=0;i<25;i++)generate_audio(&c,output,256);
 double p0=c.voices[0].pos,p1=c.voices[1].pos;
 assert_transport(p0>0&&p1>0,"both pads initially play");
 g_scrubState[0]=1;
 for(int i=0;i<7;i++)generate_audio(&c,output,256);
 assert_transport(c.voices[0].pos==p0,"pause retains selected pad source frame");
 assert_transport(c.voices[1].pos>p1,"other pad continues during selected pause");
 g_scrubState[0]=2;
 generate_audio(&c,output,256);
 assert_transport(c.voices[0].pos>p0,"Play resumes selected pad");
 g_scratchPad=0;g_scratchHold=1;g_scrubState[0]=3;g_scratchPosPpm=300000;g_scrubPosPpm[0]=300000;
 generate_audio(&c,output,256);
 assert_transport(c.voices[0].scratch&&fabs(c.voices[0].pos-14400)<2,"scrub takes over at mouse position");
 g_scrubPosPpm[0]=750000;g_scratchPosPpm=750000;g_scratchHold=0;g_scrubState[0]=1;
 generate_audio(&c,output,256);
 double pa=c.voices[0].pos;
 assert_transport(fabs(pa-36000)<2,"release pauses at exact cursor");
 generate_audio(&c,output,256);
 assert_transport(c.voices[0].pos==pa,"held release stays paused");
 g_scrubState[0]=2;
 generate_audio(&c,output,256);
 assert_transport(c.voices[0].pos>pa,"Play resumes from frozen position");
 assert_transport(c.voices[1].pos>p1,"other pad unaffected by scrub");
 puts("ALL TRANSPORT CHECKS PASSED");
 return 0;
}
