/* Hybrid WSOLA path engages on voiced movement and retreats on stationary holds. */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "src/scratch_dsp.h"
#define SR 48000
#define PI 3.1415926535897932384626433832795
static void check(int ok,const char*what){if(!ok){fprintf(stderr,"FAIL %s\n",what);exit(1);}printf("PASS %s\n",what);}
int main(void){
 float*sound=(float*)calloc(SR*4,sizeof(float));
 ApoScratchStretch*p=(ApoScratchStretch*)calloc(1,sizeof(*p));
 check(sound&&p,"hybrid buffers allocated");
 for(int i=0;i<SR*4;i++){
  double t=(double)i/SR;
  double vowel=.24*sin(2*PI*165*t)+.10*sin(2*PI*330*t)+.045*sin(2*PI*660*t);
  double transient=(i%7200<60)?.15*sin(2*PI*3200*t):0;
  sound[i]=(float)(vowel+transient);
 }
 double cursor=SR,pre=0,maxjump=0,total=0,wsPeak=0;
 scratch_stretch_init(p,cursor,SR,SR);
 for(int i=0;i<50000;i++){
  cursor+=.65;
  float v=scratch_stretch_render(p,sound,SR*4,SR,SR,cursor,.65,1.);
  if(i>10){double step=fabs((double)v-pre);if(step>maxjump)maxjump=step;}
  pre=v;total+=fabsf(v);
  if(p->spec.wsMix>wsPeak)wsPeak=p->spec.wsMix;
  if(!isfinite(v)||fabsf(v)>1.5f)return 2;
 }
 check(wsPeak>.28,"hybrid waveform-similarity path engages during moving Scrub");
 check(total/50000.>.015,"moving hybrid output remains audible");
 check(maxjump<.85,"moving hybrid output has bounded sample discontinuity");
 for(int i=0;i<15000;i++){
  float v=scratch_stretch_render(p,sound,SR*4,SR,SR,cursor,0.,1.);
  if(!isfinite(v))return 3;
 }
 check(p->spec.wsMix<.002,"stationary hold returns to original spectral-only engine");
 printf("Hybrid WS peak mix %.3f, peak jump %.3f, final hold mix %.5f\n",wsPeak,maxjump,p->spec.wsMix);
 free(p);free(sound);return 0;
}
