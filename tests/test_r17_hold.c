#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "../src/controller.c"
static Client cl;static Sample sm;static float pcm[48000],out[256*2];static WL_U64 nowMs=1000;
static WL_U64 WL_WINAPI clock_test(void){return nowMs;}
static void ck(int c,const char*s){if(!c){fprintf(stderr,"FAIL: %s\n",s);exit(1);}puts(s);}
static double peak(void){double a=0.;for(int i=0;i<512;i++)if(fabs(out[i])>a)a=fabs(out[i]);return a;}
static void run(int blocks){for(int b=0;b<blocks;b++){generate_audio(&cl,out,256);nowMs+=5;}}
int main(void){
 ck(ID_R17_HOLD!=ID_SCRUB_PLAY&&ID_R17_HOLD!=ID_SCRUB_STOP,"Toggle command distinct from playback");
 r7_read_options(0x80028010u);ck(!g_r17Silent,"Existing config migrates to Continuous");g_r17Silent=1;WL_U32 options=r7_options();g_r17Silent=0;r7_read_options(options);ck(g_r17Silent,"Silent preference round trip");g_r17Silent=0;r7_read_options(r7_options());ck(!g_r17Silent,"Continuous preference round trip");
 for(int gap=1;gap<=75;gap++)ck(r18_is_moving(1000+gap,1000+r18_grace(gap)),"Sparse motion event interval remains active");ck(!r18_is_moving(1091,1000+r18_grace(225)),"True stationary hold after capped grace");ck(!r18_is_moving(1000,0),"Mouse down without motion is not moving");
 double gain=1;for(int i=0;i<577;i++)gain=r17_gate(gain,0,48000);ck(gain==0.,"12ms fade ends at exact zero");for(int i=0;i<385;i++)gain=r17_gate(gain,1,48000);ck(gain==1.,"8ms attack reaches unity");
 pGetTickCount64=clock_test;for(int i=0;i<48000;i++)pcm[i]=.25f*sin(i*.0287979);sm.frames=48000;sm.sampleRate=48000;sm.refs=100;sm.mono=pcm;g_pads[0].sample=&sm;g_pads[0].selEnd=1;g_pads[0].volumeHalfPct=200;cl.sampleRate=48000;g_scratchHold=1;g_scratchPad=0;g_scrubState[0]=3;g_scratchPosPpm=500000;g_scratchMoveMs=nowMs;g_scratchRateMilli=0;g_r17Silent=1;ck(add_voice(&cl,0),"Voice created in Silent hold");run(10);ck(peak()==0.,"Silent mouse down and hold produce exact digital zero, including entry fade");
 g_scratchMoveMs=nowMs;g_scratchRateMilli=25;g_r17MotionGap=120;r18_motion_event(nowMs,120,0);run(8);ck(peak()>.02,"Slow movement produces audible signal");run(50);ck(peak()==0.,"Silent hold fades to exact zero after motion");g_scratchMoveMs=nowMs;g_scratchRateMilli=-25;r18_motion_event(nowMs,120,0);run(8);ck(peak()>.02,"Reverse motion resumes sound");run(50);ck(peak()==0.,"Second stop is silent");g_scratchHold=0;g_scrubState[0]=2;run(1);ck(cl.voices[0].pos>24000.,"Release resumes previous playback");
 g_scrubState[0]=3;g_scratchHold=1;g_scratchRateMilli=0;g_r17Silent=0;run(20);ck(peak()>.02,"Continuous hold still audible");
 puts("PASS R17 config, motion gate, stereo-shared gain, transport");return 0;
}
