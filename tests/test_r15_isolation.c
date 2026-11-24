#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../src/controller.c"
static Client a,b;static ApoFxState fa[PAD_COUNT],fb[PAD_COUNT];static Sample sm;static float data[48000];
static void ck(int x,const char*s){if(!x){fprintf(stderr,"FAIL %s\n",s);exit(1);}puts(s);}
int main(){for(int i=0;i<48000;i++)data[i]=.10f*sin(i*.048);sm.refs=100;sm.frames=48000;sm.sampleRate=48000;sm.mono=data;
 for(int i=0;i<2;i++){g_pads[i].sample=&sm;g_pads[i].selEnd=1;g_pads[i].volumeHalfPct=i?800:200;}
 g_fxSettings[0]=apo_fx_preset(1);g_fxSettings[1]=apo_fx_preset(0);a.sampleRate=b.sampleRate=48000;a.fx=fa;b.fx=fb;g_r13Link=1;g_monitorVolumeHalfPct=300;
 select_pad(0);add_voice(&a,0);add_voice(&b,0);double worst=0;for(int i=0;i<240;i++){select_pad(0);generate_audio(&a,a.audio,256);select_pad(1);if(i==10){fx_load_preset(1,2);g_pads[1].pitchHalf=12;}generate_audio(&b,b.audio,256);for(int j=0;j<512;j++){double d=fabs(a.audio[j]-b.audio[j])+fabs(a.monitorAudio[j]-b.monitorAudio[j]);if(d>worst)worst=d;}}
 printf("Maximum sum of microphone and monitor difference including effect tails: %.9f\n",worst);ck(worst<1e-7,"Selecting/editing another pad cannot change playback or effect tails");ck(g_monitorVolumeHalfPct==300&&r15_monitor_ui_half()==800,"Selecting pad changes linked display, not global gain");
 g_fxSettings[0]=g_fxSettings[1]=apo_fx_preset(0);g_pads[1].pitchHalf=0;g_pads[0].volumeHalfPct=200;g_pads[1].volumeHalfPct=400;memset(&a,0,sizeof(a));a.sampleRate=48000;add_voice(&a,0);add_voice(&a,1);generate_audio(&a,a.audio,256);double e=0;for(int j=0;j<512;j++){e+=fabs(a.audio[j]-a.monitorAudio[j]);}ck(e<1e-6,"Two linked pads have equal independent monitor and microphone gains");
 select_pad(1);r13_apply_volume(1,600);ck(g_pads[0].volumeHalfPct==200&&g_pads[1].volumeHalfPct==400,"Unapproved edit caps only selected pad");g_r10VolumeDecision=1;r13_apply_volume(1,600);ck(g_pads[0].volumeHalfPct==200&&g_pads[1].volumeHalfPct==600,"Linked volume edit changes only selected pad");g_r13Link=0;ck(r15_monitor_ui_half()==300,"Unlink restores independent monitor master");
 r14_font_defaults();ck(g_r14Sizes[1]==10,"Pad names and hotkeys share 10 pt default");ck(r14_unpack_fonts(0x14264233u)&&g_r14Sizes[1]==10,"Valid older font preference loads");return 0;}
