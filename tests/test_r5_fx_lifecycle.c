/* Test actual controller mixer fade, idle retirement, stale-buffer prevention. */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../src/controller.c"
static Client c;static ApoFxState fx[PAD_COUNT];static Sample sample;static float wave[48000];
static void check(int ok,const char*s){if(!ok){fprintf(stderr,"FAIL %s\n",s);exit(1);}puts(s);}
int main(void){
 int p=18;for(int i=0;i<48000;i++)wave[i]=.3f*sinf(2*3.14159265f*220*i/48000.f);
 sample.mono=wave;sample.frames=48000;sample.sampleRate=48000;sample.refs=100;g_pads[p].sample=&sample;g_pads[p].selEnd=1;g_pads[p].volumeHalfPct=200;
 ApoFxConfig cfg=apo_fx_preset(1);g_fxSettings[p]=cfg;c.sampleRate=48000;c.fx=fx;g_triggerSerial[p]=1;
 for(int i=0;i<220;i++)generate_audio(&c,c.audio,256);
 check(fx[p].echoLive&&fx[p].revLive&&fx[p].tailFrames,"Both effect tanks remain active after source ends");
 cfg.echoMix=0;cfg.reverbMix=0;fx_apply_config(p,&cfg);
 check(generate_audio(&c,c.audio,256)&&fx[p].echoMixSmooth>0&&fx[p].reverbMixSmooth>0,"Controller actually processes the fade to zero");
 for(int i=0;i<50;i++)generate_audio(&c,c.audio,256);
 check(!fx[p].echoLive&&!fx[p].revLive&&!fx[p].tailFrames,"Both effects retire; no indefinite active stream");
 check(!generate_audio(&c,c.audio,256),"Mixer returns idle after fade");
 for(unsigned i=0;i<APO_FX_ECHO_CAP;i++)if(fx[p].echo[0][i]!=0||fx[p].echo[1][i]!=0)return 2;
 for(unsigned i=0;i<8;i++)for(unsigned j=0;j<APO_FX_COMB_CAP;j++)if(fx[p].comb[i][j]!=0)return 3;
 check(1,"Retired delay buffers have no stale samples");
 cfg=apo_fx_preset(1);fx_apply_config(p,&cfg);check(!generate_audio(&c,c.audio,256),"Re-enable does not resurrect stale tails");
 g_triggerSerial[p]++;for(int i=0;i<80;i++)generate_audio(&c,c.audio,256);
 cfg.echoOn=0;fx_apply_config(p,&cfg);generate_audio(&c,c.audio,256);check(fx[p].echoLive&&fx[p].echoMixSmooth>0,"Toggle off fades rather than clearing abruptly");
 for(int i=0;i<50;i++)generate_audio(&c,c.audio,256);
 check(!fx[p].echoLive&&fx[p].revLive,"Echo retires independently of active reverb");
 g_stopSerial++;check(!generate_audio(&c,c.audio,256),"Stop All remains immediate");return 0;
}
