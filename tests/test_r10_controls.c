#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../src/controller.c"
static Client c; static Sample sample; static float pcm[48000];
static int prompts=0,reply=IDYES;
static void check(int ok,const char*s){if(!ok){fprintf(stderr,"FAIL %s\n",s);exit(1);}puts(s);}
static WL_I32 WL_WINAPI ask(WL_HWND h,const WL_WCHAR*a,const WL_WCHAR*b,WL_UINT flags){(void)h;(void)a;(void)b;(void)flags;prompts++;return reply;}
static WL_DWORD WL_WINAPI noenv(const WL_WCHAR*n,WL_WCHAR*b,WL_DWORD l){(void)n;(void)b;(void)l;return 0;}
static WL_HWND WL_WINAPI cap(WL_HWND h){return h;}
static WL_BOOL WL_WINAPI release(void){return 1;}
static WL_LPARAM xy(int x,int y){return (WL_LPARAM)((unsigned)x|((unsigned)y<<16));}
int main(void){g_api.MessageBoxW=ask;g_api.GetEnvironmentVariableW=noenv;g_api.SetCapture=cap;g_api.ReleaseCapture=release;
 g_selected=0;g_volX=0;g_volW=401;set_volume_from_x(400,0);check(prompts==1&&g_pads[0].volumeHalfPct==800,"Yes unlocks 400 percent");set_volume_from_x(100,0);set_volume_from_x(400,0);check(prompts==1,"Yes not repeated after returning below threshold");WL_U32 opts=r7_options();r7_read_options(0);r7_read_options(opts);check(g_r10VolumeDecision==1,"Yes saved in options");
 r7_read_options(0);reply=7;set_volume_from_x(400,0);check(prompts==2&&g_pads[0].volumeHalfPct==400,"No caps at 200 percent");opts=r7_options();r7_read_options(opts);set_volume_from_x(400,0);check(prompts==3&&g_pads[0].volumeHalfPct==400,"No can be reconsidered across load");g_r10VolumeDecision=1;r10_set_pad_volume(0,800);check(g_pads[0].volume==4.f,"Mixer gain reaches four");
 g_waveMode=1;g_activeTab=0;g_waveX=10;g_waveY=10;g_waveW=101;g_waveH=30;g_fxDragging=-1;for(int i=0;i<48000;i++)pcm[i]=.1*sin(i*.06);sample.mono=pcm;sample.frames=48000;sample.sampleRate=48000;sample.refs=100;g_pads[0].sample=&sample;g_pads[0].selEnd=1;g_pads[0].volumeHalfPct=200;c.sampleRate=48000;add_voice(&c,0);g_scrubState[0]=2;generate_audio(&c,c.audio,256);
 wndproc(0,WM_LBUTTONDOWN,0,xy(35,20));check(g_scratchHold&&g_scrubState[0]==3,"Single press engages hold");wndproc(0,WM_LBUTTONUP,0,xy(35,20));check(!g_scratchHold&&g_scrubState[0]==1,"Original R2 release pauses");wndproc(0,WM_LBUTTONDBLCLK,0,xy(60,20));check(g_scratchHold&&g_scrubState[0]==3,"Double click second press is ordinary hold");wndproc(0,WM_LBUTTONUP,0,xy(60,20));generate_audio(&c,c.audio,256);check(g_scrubState[0]==1&&fabs(c.voices[0].pos-24000)<2,"Rapid clicks before mixer pause at newest cursor");
 g_scrubState[0]=1;wndproc(0,WM_LBUTTONDOWN,0,xy(40,20));wndproc(0,WM_LBUTTONUP,0,xy(40,20));generate_audio(&c,c.audio,256);check(g_scrubState[0]==1&&fabs(c.voices[0].pos-14400)<2,"Paused gesture remains paused at cursor");
 g_scrubState[0]=2;wndproc(0,WM_LBUTTONDOWN,0,xy(45,20));wndproc(0,WM_CAPTURECHANGED,0,0);check(!g_scratchHold&&g_scrubState[0]==1,"Capture loss pauses at cursor");
 r7_read_options(0);check(r10_quality()==3,"Single R2 engine");g_r10Ultra=1;opts=r7_options();r7_read_options(opts);check(r10_quality()==3,"Obsolete mode ignored");ApoScratchStretch*v=calloc(1,sizeof(*v));double e=0;for(int i=0;i<8192;i++){float q=r9_render(v,pcm,48000,48000,48000,10000+i*.5,.5,1,5);if(!isfinite(q))check(0,"Ultra finite");e+=q*q;}check(e>0.01&&e<1000,"Ultra finite bounded moving output");free(v);puts("PASS R10 controls");return 0;}
