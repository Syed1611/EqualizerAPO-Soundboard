#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "../src/controller.c"
static WL_U64 timeNow=1000;
static WL_U64 WL_WINAPI fake_time(void){return timeNow;}
static Voice v;
static r19_ApoScratchStretch ref;
static float pcm[48000];
static void ck(int ok,const char*msg){if(!ok){fprintf(stderr,"FAIL %s\n",msg);exit(1);}puts(msg);}
int main(void){
 Sample s={0};s.frames=48000;s.sampleRate=48000;s.mono=pcm;v.sample=&s;v.targetStep=1.;pGetTickCount64=fake_time;
 for(int i=0;i<48000;i++)pcm[i]=.18*sin(i*.0287979)+.09*sin(i*.073)*sin(i*.00021);
 g_r17Silent=0;double maxerr=0.;
 for(int i=0;i<16000;i++){
  timeNow=1000+i/48;g_scratchMoveMs=(i<6000||i>=10000)?timeNow:1124;g_scratchRateMilli=i<6000?80:-250;
  g_scratchPosPpm=400000+(i<6000?i:i<10000?6000:16000-i);
  double cursor=g_scratchPosPpm*.000001*48000.;double speed=timeNow-g_scratchMoveMs<55?g_scratchRateMilli*.001:0.;
  float expected=r19_scratch_stretch_render(&ref,pcm,48000,48000,48000,cursor,speed,1.)*.76f;
  float actual=scratch_granular(&v,48000,timeNow,0);double diff=fabs(actual-expected);if(diff>maxerr)maxerr=diff;
 }
 printf("Active mono render maximum difference vs directly included original R2 = %.9g\n",maxerr);ck(maxerr==0.,"Original R2 core invoked without numerical differences");
 g_r17Silent=1;v.r17Gain=1.;timeNow=2000;r18_motion_event(timeNow,225,0);ck(g_r18MotionUntil==2090,"Worst-case movement deadline is capped at 90 ms");
 timeNow=2091;for(int i=0;i<577;i++)scratch_granular(&v,48000,timeNow,0);ck(v.r17Gain==0.,"Stationary gate reaches exact zero within fade");
 puts("PASS R19 exact core and bounded stop");return 0;
}
