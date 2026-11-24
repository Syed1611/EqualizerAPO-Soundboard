/* v1.0.1: 32-pad config, both waveform transport modes, and tail-after-source. */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../src/controller.c"
static Client client;
static ApoFxState fxState[PAD_COUNT];
static Sample voiceSample;
static float wave[4096];
static void insist(int ok,const char*msg){if(!ok){fprintf(stderr,"FAIL: %s\n",msg);exit(1);}printf("PASS: %s\n",msg);}
int main(void){
 insist(PAD_COUNT==32 && LEGACY_PAD_COUNT==16,"32 pads, original 16-pad layout preserved");
 insist(sizeof(((ConfigV11Data*)0)->base.base.base.pads)/sizeof(PadDiskV2)==16,"v11 settings still use 16 disk pads");
 insist(sizeof(((ConfigV12Data*)0)->extraPads)/sizeof(PadDiskV2)==16,"v12 adds 16 new disk pads");
 insist(sizeof(((ConfigV12Data*)0)->effects)/sizeof(ApoFxConfig)==32,"v12 retains per-pad effects");
 insist(sizeof(ConfigV12Data)>sizeof(ConfigV11Data),"v12 settings cannot overwrite old v11 format");
 for(int i=0;i<4096;i++)wave[i]=.15f*(float)sin(i*.042);
 voiceSample.mono=wave;voiceSample.frames=4096;voiceSample.sampleRate=48000;voiceSample.refs=100;
 g_pads[19].sample=&voiceSample;g_pads[19].selStart=0;g_pads[19].selEnd=1;g_pads[19].volumeHalfPct=200;g_selected=19;g_padPage=1;
 g_capturePad=-1;g_hotkeysSuspended=0;g_activeTab=0;
 g_waveMode=0;g_scrubState[19]=0;g_scrubHasCursor[19]=0;
 scrub_toggle_selected();insist(g_scrubState[19]==2,"Select Range: Space/Play starts pad 20");
 scrub_toggle_selected();insist(g_scrubState[19]==1,"Select Range: Space/Play pauses pad 20");
 scrub_toggle_selected();insist(g_scrubState[19]==2,"Select Range: next Space resumes pad 20");
 g_waveMode=1;g_scrubState[19]=1;
 scrub_toggle_selected();insist(g_scrubState[19]==2,"Scrub: Space/Play resumes");
 scrub_toggle_selected();insist(g_scrubState[19]==1,"Scrub: Space/Play pauses");
 g_scrubState[19]=0;g_pads[0].volumeHalfPct=200;g_fxSettings[0]=apo_fx_preset(1);g_fxSerial[0]=5;
 memset(&client,0,sizeof(client));client.sampleRate=48000;client.fx=fxState;client.fxSeen[0]=5;
 fxState[0].tailFrames=50000;fxState[0].echo[0][APO_FX_ECHO_CAP-14400]=.5f;
 float out[256];int active=generate_audio(&client,out,128);
 insist(active==1,"echo tail keeps VST output alive after source ends");
 insist(out[0]>.25f,"tail is audibly present after source ended");
 int allFinite=1;for(int i=0;i<256;i++)if(!isfinite(out[i]))allFinite=0;insist(allFinite,"effect tail is finite");
 __atomic_add_fetch(&g_stopSerial,1,__ATOMIC_SEQ_CST);
 active=generate_audio(&client,out,128);
 insist(active==0,"Stop All clears echo/reverb tails immediately");
 puts("ALL v1.0.1 INTEGRATION TESTS PASSED");return 0;
}
