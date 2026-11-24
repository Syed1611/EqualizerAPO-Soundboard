/* Compare whole controller render at unity gain with effects off. */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifdef ORIGINAL_R2
#include "reference/r2_controller/controller.c"
#else
#include "../src/controller.c"
#endif
static Client client;static Sample sample;static float pcm[96000],block[480];
static WL_U64 now=1000;
static WL_U64 WL_WINAPI clock_fake(void){return now;}
static void* WL_WINAPI heap(void){return (void*)1;}
static void* WL_WINAPI alloc(void*h,WL_DWORD f,WL_SIZE_T n){(void)h;return f&8?calloc(1,n):malloc(n);}
static WL_BOOL WL_WINAPI freem(void*h,WL_DWORD f,void*p){(void)h;(void)f;free(p);return 1;}
static WL_BOOL WL_WINAPI release(void){return 1;}
static void render(void){if(!generate_audio(&client,block,240))for(int i=0;i<480;i++)block[i]=0;fwrite(block,sizeof(float),480,stdout);now+=5;}
int main(int argc,char**argv){int native=argc>1?atoi(argv[1]):48000;int pitch=argc>2?atoi(argv[2]):0;int takeover=argc>3?atoi(argv[3]):0;g_api.GetProcessHeap=heap;g_api.HeapAlloc=alloc;g_api.HeapFree=freem;g_api.ReleaseCapture=release;pGetTickCount64=clock_fake;g_hostSampleRate=48000;client.sampleRate=48000;g_waveX=10;g_waveW=1001;g_waveMode=1;sample.sampleRate=native;sample.frames=native*2;sample.mono=pcm;sample.refs=100;for(int i=0;i<native*2;i++){double t=(double)i/native;pcm[i]=(float)(.2*sin(t*2*3.141592653589793*133)+.08*sin(t*2*3.141592653589793*267)*sin(t*41)+.04*sin(t*2*3.141592653589793*847));}
#ifdef ORIGINAL_R2
 if(native!=48000){float*p=0;WL_U64 n=0;if(resample_buffer(pcm,sample.frames,native,48000,&p,&n,0)!=1)return 2;sample.mono=p;sample.frames=n;sample.sampleRate=48000;}
#else
 r20_prepare_pcm(&sample,48000);if(native!=48000&&!r20_find_pcm(&sample,48000))return 2;g_r17Silent=0;
#endif
 g_pads[0].sample=&sample;g_pads[0].selEnd=1;g_pads[0].volumeHalfPct=200;g_pads[0].pitchHalf=pitch;
 if(takeover){if(!add_voice(&client,0))return 3;for(int i=0;i<10;i++){generate_audio(&client,block,240);now+=5;}}
 g_waveDragging=2;g_scratchPad=0;
 scratch_update_position(310,1);g_scratchHold=1;g_scrubState[0]=3;if(!takeover&&!add_voice(&client,0))return 3;
 for(int i=0;i<100;i++){if(i<25&&i%3==0)scratch_update_position(310+i*2,0);if(i==20){scratch_update_position(350,0);scratch_update_position(351,0);}if(i>=55&&i<80&&i%4==0)scratch_update_position(365-(i-55)*3,0);if(i==65)g_pads[0].pitchHalf=-7;render();}
 scratch_update_position(291,0);end_scratch();if(g_scrubState[0]!=1)return 4;for(int i=0;i<10;i++)render();scrub_toggle_selected();if(g_scrubState[0]!=2)return 5;for(int i=0;i<15;i++)render();return 0;}
