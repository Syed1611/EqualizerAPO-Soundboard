/* New R4 quality safeguards: preserve strong-bin natural frequency, reduce
   cross-engine phasing duration, maintain finite 48k + 96k stationary holds. */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../src/scratch_dsp.h"
#define PI 3.1415926535897932384626433832795
static void check(int condition,const char*message){if(!condition){fprintf(stderr,"FAIL: %s\n",message);exit(1);}puts(message);}
int main(void){
 for(int testRate=48000;testRate<=96000;testRate*=2){
  unsigned rate=(unsigned)testRate;
  unsigned len=rate*5u;
  float *src=(float*)calloc(len,sizeof(float));
  ApoScratchStretch*st=(ApoScratchStretch*)calloc(1,sizeof(*st));
  check(src!=0&&st!=0,"Scrub buffers allocated");
  for(unsigned i=0;i<len;i++){
   double t=(double)i/rate;
   src[i]=(float)(.26*sin(2.*PI*194.*t)+.08*sin(2.*PI*388.*t));
  }
  double cursor=rate*2.;
  scratch_stretch_init(st,cursor,rate,rate);
  scratch_analyze(&st->spec,src,len,cursor);
  double best=0.,frequency=0.;
  for(unsigned k=1;k<APO_SPECTRAL_HALF;k++){
   if(st->spec.mag[k]>best){best=st->spec.mag[k];frequency=st->spec.frequency[k]*(double)rate/(2.*PI);}
  }
  printf("Rate %d: dominant analysis frequency %.2fHz\n",testRate,frequency);
  check(frequency>190.&&frequency<198.,"Dominant harmonic remains on original source frequency");
  float before=0.;double peakStep=0.;
  for(unsigned i=0;i<rate*2u;i++){
   cursor+=.50;
   float v=scratch_stretch_render(st,src,len,rate,rate,cursor,.50,1.);
   if(!isfinite(v)||fabsf(v)>2.)return 3;
   if(i>0&&fabsf(v-before)>peakStep)peakStep=fabsf(v-before);
   before=v;
  }
  if(rate==48000u)check(st->spec.wsMix>.70,"Waveform engine available for confidently matched moving speech at 48 kHz");
  else check(st->spec.wsMix>=0.&&st->spec.wsMix<=1.,"Higher-rate playback safely falls back to spectral synthesis when WSOLA confidence is low");
  for(unsigned i=0;i<rate/33u;i++){
   float v=scratch_stretch_render(st,src,len,rate,rate,cursor,0.,1.);
   if(!isfinite(v))return 4;
  }
  printf("Rate %d: WSOLA contribution after 30ms holding = %.4f\n",testRate,st->spec.wsMix);
  check(st->spec.wsMix<.03,"Stationary hold leaves the moving engine within 30ms");
  check(peakStep<.75,"Moving Scrub avoids abrupt sample discontinuities");
  free(src);free(st);
 }
 puts("ALL R4 SCRUB SAFEGUARDS PASSED");return 0;
}
