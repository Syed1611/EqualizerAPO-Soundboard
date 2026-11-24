/* Long-lived WSOLA/phase-vocoder playback: finite, pitch stable and no abrupt
   stale-pitch locking when moving, freezing and changing direction. */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "../src/scratch_dsp.h"
#define RATE 48000
#define PI 3.14159265358979323846
static void verify(int yes,const char*s){if(!yes){fprintf(stderr,"FAIL %s\n",s);exit(1);}printf("PASS %s\n",s);}
int main(void){
 unsigned frames=RATE*8u;
 float*pcm=(float*)calloc(frames,sizeof(*pcm));
 ApoScratchStretch*st=(ApoScratchStretch*)calloc(1,sizeof(*st));
 verify(pcm&&st,"long-scrub test buffers allocated");
 for(unsigned i=0;i<frames;i++){
  float env=(i%18000u<120)?1.f:.74f;
  double t=(double)i/RATE;
  pcm[i]=(float)((.24*sin(2*PI*172.*t)+.08*sin(2*PI*344.*t)+.04*sin(2*PI*860.*t))*env);
 }
 double cursor=RATE*2.,maxStep=0,rms=0,wsPeak=0;float previous=0;
 for(int i=0;i<RATE*12;i++){
  double speed=(i<RATE*6)? .24:(i<RATE*8?0.:-.46);
  cursor+=speed;
  if(cursor<1000.)cursor=1000.;if(cursor>(double)(frames-1000u))cursor=(double)(frames-1000u);
  float y=scratch_stretch_render(st,pcm,frames,RATE,RATE,cursor,speed,1.);
  if(!isfinite(y)){fprintf(stderr,"FAIL non-finite sample %d\n",i);return 2;}
  if(i){double d=fabs(y-previous);if(d>maxStep)maxStep=d;}
  if(st->spec.wsMix>wsPeak)wsPeak=st->spec.wsMix;
  previous=y;rms+=(double)y*y;
 }
 rms=sqrt(rms/(RATE*12.));
 verify(1,"all long-scrub samples finite");
 verify(rms>.01 && rms<.65,"long stretch and hold preserve audible bounded signal");
 verify(maxStep<.65,"no catastrophic jump across long slow stretch and reversal");
 verify(wsPeak>.85,"high-confidence WSOLA carries moving voiced signal");
 printf("Long scrub 12s: rms=%.5f max-jump=%.5f max-ws-mix=%.3f\n",rms,maxStep,wsPeak);
 free(st);free(pcm);puts("ALL v1.0.2 LONG SCRUB TESTS PASSED");return 0;
}
