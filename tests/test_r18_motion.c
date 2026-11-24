#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "../src/controller.c"
static Client cl;static Sample sm;static float pcm[48000],out[240*2];static WL_U64 nowMs=1000;
static WL_U64 WL_WINAPI clock_test(void){return nowMs;}
static void ck(int c,const char*s){if(!c){fprintf(stderr,"FAIL: %s\n",s);exit(1);}puts(s);}
static void block(void){generate_audio(&cl,out,240);nowMs+=5;}
int main(void){pGetTickCount64=clock_test;for(int i=0;i<48000;i++)pcm[i]=.25f*sin(i*.0287979);sm.frames=48000;sm.sampleRate=48000;sm.refs=100;sm.mono=pcm;g_pads[0].sample=&sm;g_pads[0].selEnd=1;g_pads[0].volumeHalfPct=200;cl.sampleRate=48000;g_scratchPad=0;g_scrubState[0]=3;g_r17Silent=1;
 int x=g_waveX+300;scratch_update_position(x,1);g_scratchHold=1;ck(add_voice(&cl,0),"Voice created");for(int i=0;i<20;i++)block();ck(cl.voices[0].r17Gain==0.,"Click without motion remains silent");
 scratch_update_position(++x,0);scratch_update_position(++x,0);ck(g_scratchRateMilli==0&&r18_is_moving(nowMs,g_r18MotionUntil),"Original R2 velocity can be zero at same tick; separate gate stays active");
 const int gaps[]={25,40,45,50,35,35,50,45,40,40};int bad=0;for(int j=0;j<10;j++){scratch_update_position(++x,0);if(j==3)scratch_update_position(--x,0);for(int i=0;i<gaps[j]/5;i++){block();if(j>0&&cl.voices[0].r17Gain<.999)bad++;}}ck(!bad,"No gate drop over tested irregular slow drag and reversal");
 WL_U64 deadline=g_r18MotionUntil;for(int i=0;i<25;i++){scratch_update_position(x,0);block();}ck(deadline==g_r18MotionUntil,"Duplicate coordinates do not extend activity");ck(cl.voices[0].r17Gain==0.,"True stop reaches zero after grace and fade");
 scratch_update_position(--x,0);for(int i=0;i<4;i++)block();ck(cl.voices[0].r17Gain==1.,"Motion resumes full gain");
 g_scrubState[0]=2;g_scratchHold=0;block();ck(cl.voices[0].lastTransportState==2,"Release restores playback state");
 puts("PASS R18 actual cursor-update path with simulated irregular clock");return 0;}
