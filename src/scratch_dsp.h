/* Scratch Edition v0.9.6: one continuous tracked spectral oscillator engine.
   Both stationary and moving cursors synthesize coherent phases. Moving
   updates frequency/amplitude targets from source FFT snapshots, rather than
   playing 78 ms grains with old/new segment crossfades. The live Pitch knob
   transposes independently of cursor speed. Experimental voice resynthesis.
*/
#ifndef APO_SCRATCH_DSP_H
#define APO_SCRATCH_DSP_H
#define APO_SPECTRAL_FFT 4096u
#define APO_SPECTRAL_PARTIALS 192u
#define APO_SPECTRAL_TRACKED 112u
#define APO_SPECTRAL_PI 3.1415926535897932384626433832795
#define APO_SPECTRAL_TWO_PI 6.283185307179586476925286766559

typedef struct {
 double frequencyHz, targetHz;
 double amplitude, targetAmplitude;
 double phaseCos,phaseSin;
 double stepCos,stepSin;
 unsigned matched;
} ApoSpectralPeak;
typedef struct {
 /* Allocated within the heap-owned client voices, never on the worker stack. */
 double workRe[APO_SPECTRAL_FFT],workIm[APO_SPECTRAL_FFT];
 double workPrevRe[APO_SPECTRAL_FFT/2+1],workPrevIm[APO_SPECTRAL_FFT/2+1];
 double workMag[APO_SPECTRAL_FFT/2+1];
 unsigned tempCandidates[APO_SPECTRAL_TRACKED*2];
 double tempFreq[APO_SPECTRAL_TRACKED], tempAmp[APO_SPECTRAL_TRACKED];
 double tempPhaseRe[APO_SPECTRAL_TRACKED], tempPhaseIm[APO_SPECTRAL_TRACKED];
 ApoSpectralPeak peaks[APO_SPECTRAL_PARTIALS];
 unsigned peakCount;
 unsigned outputRate,sourceRate,pitchUpdateFrames;
 unsigned long long outputFrames;
 double currentPitch, noiseAmplitude,noiseState;
 double gain;
 unsigned randomState;
 int valid;
} ApoSpectralFreeze;
typedef struct {
 /* One continuous phase-coherent oscillator bank for BOTH motion and hold.
    Source cursor motion updates timbre only; it never plays old/new source
    fragments or crossfades between independent audio grains. */
 double freezeCursor, lastAnalysisCursor;
 unsigned long long lastAnalysisFrame;
 int initialized,freezeValid,wasStationary;
 ApoSpectralFreeze spec;
} ApoScratchStretch;
static double scratch_abs_d(double x){return x<0?-x:x;}
static double scratch_clamp_d(double x,double lo,double hi){return x<lo?lo:(x>hi?hi:x);}
static float scratch_read_source(const float*s,unsigned long long count,double at){
 if(!s||!count)return 0.0f;
 if(at<=0.0)return s[0];
 if(at>=(double)(count-1))return s[count-1];
 unsigned long long i=(unsigned long long)at;
 unsigned long long i0=i?i-1:i,i2=i+1<count?i+1:i,i3=i2+1<count?i2+1:i2;
 double t=at-(double)i,t2=t*t,t3=t2*t;
 double a=s[i0],b=s[i],c=s[i2],d=s[i3];
 return (float)(0.5*((2.0*b)+(-a+c)*t+(2.0*a-5.0*b+4.0*c-d)*t2+(-a+3.0*b-3.0*c+d)*t3));
}
static void scratch_stretch_init(ApoScratchStretch*v,double cursor,unsigned sourceRate,unsigned outputRate){
 if(!sourceRate)sourceRate=48000;if(!outputRate)outputRate=48000;
 v->initialized=1;v->freezeValid=0;v->wasStationary=1;
 v->freezeCursor=cursor;v->lastAnalysisCursor=cursor;
 v->lastAnalysisFrame=0;
 v->spec.valid=0;v->spec.peakCount=0;v->spec.outputFrames=0;
 v->spec.outputRate=outputRate;v->spec.sourceRate=sourceRate;
 v->spec.currentPitch=1.0;v->spec.gain=1.0;
}
/* Sine/cosine polynomial evaluation on a reduced interval; no runtime CRT.
   Errors remain below 3e-6 in the argument range used by the FFT. */
static void scratch_sincos(double a,double*sn,double*cs){
 while(a>APO_SPECTRAL_PI)a-=APO_SPECTRAL_TWO_PI;
 while(a< -APO_SPECTRAL_PI)a+=APO_SPECTRAL_TWO_PI;
 double cosSign=1.0;
 if(a>APO_SPECTRAL_PI*.5){a=APO_SPECTRAL_PI-a;cosSign=-1.0;}
 else if(a< -APO_SPECTRAL_PI*.5){a=-APO_SPECTRAL_PI-a;cosSign=-1.0;}
 double q=a*a;
 *sn=a*(1.0+q*(-1.0/6.0+q*(1.0/120.0+q*(-1.0/5040.0+q*(1.0/362880.0-q/39916800.0)))));
 *cs=cosSign*(1.0+q*(-0.5+q*(1.0/24.0+q*(-1.0/720.0+q*(1.0/40320.0-q/3628800.0)))));
}
static double scratch_sqrt(double x){
 if(!(x>0.0))return 0.0;
 double y=x>=1.0?x:1.0;
 for(int i=0;i<18;i++)y=0.5*(y+x/y);
 return y;
}
static double scratch_atan2(double y,double x){
 const double q=APO_SPECTRAL_PI*0.25;
 double ay=scratch_abs_d(y)+1e-30;
 double r,angle;
 if(x>=0.0){r=(x-ay)/(x+ay);angle=q+(0.1963*r*r-0.9817)*r;}
 else{r=(x+ay)/(ay-x);angle=3.0*q+(0.1963*r*r-0.9817)*r;}
 return y<0.0?-angle:angle;
}
static double scratch_wrap_angle(double a){
 while(a>APO_SPECTRAL_PI)a-=APO_SPECTRAL_TWO_PI;
 while(a< -APO_SPECTRAL_PI)a+=APO_SPECTRAL_TWO_PI;
 return a;
}
/* Radix-2 complex FFT: forward (- sign). */
static void scratch_fft(double*re,double*im){
 const unsigned n=APO_SPECTRAL_FFT;
 for(unsigned i=1,j=0;i<n;i++){
  unsigned bit=n>>1;
  while(j&bit){j^=bit;bit>>=1;}j^=bit;
  if(i<j){double t=re[i];re[i]=re[j];re[j]=t;t=im[i];im[i]=im[j];im[j]=t;}
 }
 for(unsigned len=2;len<=n;len<<=1){
  double sn,cs;scratch_sincos(-APO_SPECTRAL_TWO_PI/(double)len,&sn,&cs);
  for(unsigned at=0;at<n;at+=len){
   double wr=1.0,wi=0.0;
   for(unsigned j=0;j<len/2;j++){
    unsigned a=at+j,b=a+len/2;
    double tr=wr*re[b]-wi*im[b],ti=wr*im[b]+wi*re[b];
    re[b]=re[a]-tr;im[b]=im[a]-ti;
    re[a]+=tr;im[a]+=ti;
    double nr=wr*cs-wi*sn;wi=wr*sn+wi*cs;wr=nr;
   }
  }
 }
}
static unsigned scratch_random(unsigned*x){
 unsigned n=*x;n^=n<<13;n^=n>>17;n^=n<<5;*x=n;return n;
}
/* Continuous spectral tracking. The analysis cursor is NEVER the starting
   position of an audio grain: there is no time-domain source playback in
   scratch mode. Analyses supply smoothly tracked amplitudes/frequencies;
   every output sample is generated from continuously evolving phases. */
static void scratch_spectral_capture(ApoScratchStretch*v,const float*src,unsigned long long count,
                                    unsigned sr,unsigned hostRate,double cursor){
 if(!src||count==0)return;
 const unsigned n=APO_SPECTRAL_FFT, half=n/2, hop=n/8;
 ApoSpectralFreeze*sp=&v->spec;
 double*re=sp->workRe,*im=sp->workIm,*prevRe=sp->workPrevRe,*prevIm=sp->workPrevIm,*mag=sp->workMag;
 /* The two staggered source windows only ESTIMATE one spectral snapshot.
    We never play or crossfade the audio within those windows. */
 double start=cursor-(double)(n/2+hop/2);
 double sourceEnergy=0.0;
 for(unsigned pass=0;pass<2;pass++){
  double at=start+(double)pass*(double)hop;
  for(unsigned j=0;j<n;j++){
   double st,ct;scratch_sincos(APO_SPECTRAL_TWO_PI*(double)j/(double)(n-1),&st,&ct);
   double hann=0.5-0.5*ct;
   double ix=at+(double)j;
   double z=(ix>=0.0&&ix<(double)count)?(double)scratch_read_source(src,count,ix):0.0;
   re[j]=z*hann;im[j]=0.0;
   if(pass==1)sourceEnergy+=z*z*hann;
  }
  scratch_fft(re,im);
  if(!pass)for(unsigned j=0;j<=half;j++){prevRe[j]=re[j];prevIm[j]=im[j];}
 }
 for(unsigned k=0;k<=half;k++)mag[k]=scratch_sqrt(re[k]*re[k]+im[k]*im[k]);
 unsigned *candidates=sp->tempCandidates,nc=0;
 double maxMag=0.0;
 for(unsigned k=2;k<half-2;k++)if(mag[k]>maxMag)maxMag=mag[k];
 for(unsigned k=2;k<half-2;k++){
  double m=mag[k];
  if(m<=mag[k-1]||m<mag[k+1]||m<maxMag*0.0016)continue;
  unsigned at=0;
  while(at<nc&&mag[candidates[at]]>=m)at++;
  if(at>=APO_SPECTRAL_TRACKED*2)continue;
  if(nc<APO_SPECTRAL_TRACKED*2)nc++;
  for(unsigned j=nc-1;j>at;j--)candidates[j]=candidates[j-1];
  candidates[at]=k;
 }
 /* Use at most 112 spectral peaks to leave 80 slots for simultaneous
    fade-outs. Tracking is one-to-one: an existing phase is never reset by
    an update to the same acoustic partial. */
 double *freq=sp->tempFreq,*amp=sp->tempAmp;
 double *phaseRe=sp->tempPhaseRe,*phaseIm=sp->tempPhaseIm;
 unsigned found=0;
 for(unsigned ix=0;ix<nc&&found<APO_SPECTRAL_TRACKED;ix++){
  unsigned k=candidates[ix];
  double diff=scratch_atan2(im[k]*prevRe[k]-re[k]*prevIm[k],
                            re[k]*prevRe[k]+im[k]*prevIm[k]);
  double expected=APO_SPECTRAL_TWO_PI*(double)k*(double)hop/(double)n;
  double bin=(double)k+scratch_wrap_angle(diff-expected)*(double)n/(APO_SPECTRAL_TWO_PI*(double)hop);
  if(bin<(double)k-1.4||bin>(double)k+1.4)bin=(double)k;
  double hz=bin*(double)sr/(double)n;
  if(hz<24.0||hz>(double)hostRate*.47)continue;
  int duplicate=0;
  for(unsigned q=0;q<found;q++)if(scratch_abs_d(freq[q]-hz)<(double)sr/(double)n*1.7){duplicate=1;break;}
  if(duplicate)continue;
  freq[found]=hz;
  amp[found]=4.0*mag[k]/(double)n;
  double phase=scratch_atan2(-im[k],re[k]);
  phase+=APO_SPECTRAL_TWO_PI*bin*((double)half-(double)hop/2.0)/(double)n;
  scratch_sincos(phase,&phaseIm[found],&phaseRe[found]);
  found++;
 }
 double estimatedEnergy=0.0;
 for(unsigned i=0;i<found;i++)estimatedEnergy+=amp[i]*amp[i]*.5;
 double sourceRms=scratch_sqrt(sourceEnergy/(double)n*2.0);
 double estimatedRms=scratch_sqrt(estimatedEnergy);
 /* Conservative output target avoids transient peaks overwhelming the mic. */
 double scale=estimatedRms>1e-8?sourceRms/estimatedRms:0.0;
 if(scale>2.0)scale=2.0;
 if(sourceRms>.42)scale*=.42/sourceRms;
 for(unsigned i=0;i<found;i++)amp[i]*=scale;
 if(!sp->valid){
  sp->peakCount=0;sp->outputFrames=0;sp->pitchUpdateFrames=0;
  sp->outputRate=hostRate;sp->sourceRate=sr;
  sp->currentPitch=1.0;sp->gain=1.0;
  sp->randomState=(unsigned)((unsigned long long)cursor)^0x6859ac31u;
  sp->noiseAmplitude=0.0;sp->noiseState=0.0;
  sp->valid=1;
 }
 for(unsigned i=0;i<sp->peakCount;i++){
  sp->peaks[i].matched=0;
  sp->peaks[i].targetAmplitude=0.0;
 }
 for(unsigned j=0;j<found;j++){
  int best=-1;double bestScore=1e30;
  for(unsigned i=0;i<sp->peakCount;i++){
   ApoSpectralPeak*pk=&sp->peaks[i];
   if(pk->matched||pk->amplitude<0.000015)continue;
   double d=scratch_abs_d(pk->targetHz-freq[j]);
   double allowed=16.0+freq[j]*.055;
   if(d<allowed&&d/allowed<bestScore){bestScore=d/allowed;best=(int)i;}
  }
  if(best<0){
   /* Prefer a completely faded or previously unused oscillator, rather
      than stealing a loud one (which would cause a phase discontinuity). */
   for(unsigned i=0;i<sp->peakCount;i++){
    if(!sp->peaks[i].matched&&sp->peaks[i].amplitude<0.000012){best=(int)i;break;}
   }
   if(best<0&&sp->peakCount<APO_SPECTRAL_PARTIALS)best=(int)sp->peakCount++;
   if(best<0)continue;
   ApoSpectralPeak*pk=&sp->peaks[best];
   pk->amplitude=0.0;pk->frequencyHz=freq[j];
   pk->phaseCos=phaseRe[j];pk->phaseSin=phaseIm[j];
   pk->stepSin=0.0;pk->stepCos=1.0;
  }
  ApoSpectralPeak*pk=&sp->peaks[best];
  pk->targetHz=freq[j];pk->targetAmplitude=amp[j];pk->matched=1;
 }
 /* Dry consonants/noise are difficult to hold spectrally. Keep a subdued
    deterministic residual so formants dominate, without broadband hiss. */
 sp->noiseAmplitude=sourceRms*.006;
 v->lastAnalysisCursor=cursor;v->lastAnalysisFrame=sp->outputFrames;
 v->freezeCursor=cursor;v->freezeValid=1;
}
static float scratch_spectral_render(ApoScratchStretch*v,double pitchRatio){
 ApoSpectralFreeze*sp=&v->spec;
 if(!sp->valid)return 0.0f;
 pitchRatio=scratch_clamp_d(pitchRatio,0.25,4.0);
 double rate=(double)(sp->outputRate?sp->outputRate:48000);
 sp->currentPitch+=(pitchRatio-sp->currentPitch)/(rate*.012);
 int refresh=sp->pitchUpdateFrames==0;
 if(refresh)sp->pitchUpdateFrames=32;
 sp->pitchUpdateFrames--;
 double out=0.0;
 for(unsigned k=0;k<sp->peakCount;k++){
  ApoSpectralPeak*pk=&sp->peaks[k];
  /* Slow enough to avoid zipper noise, quick enough for cursor interaction. */
  pk->amplitude+=(pk->targetAmplitude-pk->amplitude)/(rate*.013);
  pk->frequencyHz+=(pk->targetHz-pk->frequencyHz)/(rate*.016);
  if(refresh){
   double d=APO_SPECTRAL_TWO_PI*pk->frequencyHz*sp->currentPitch/rate;
   scratch_sincos(d,&pk->stepSin,&pk->stepCos);
  }
  out+=pk->amplitude*pk->phaseCos;
  double co=pk->phaseCos*pk->stepCos-pk->phaseSin*pk->stepSin;
  double si=pk->phaseCos*pk->stepSin+pk->phaseSin*pk->stepCos;
  pk->phaseCos=co;pk->phaseSin=si;
  if((sp->outputFrames&1023ull)==1023ull){
   double norm=1.0/scratch_sqrt(co*co+si*si+1e-18);
   pk->phaseCos*=norm;pk->phaseSin*=norm;
  }
 }
 unsigned rand=scratch_random(&sp->randomState);
 double white=(double)(rand&0xffffffu)/8388608.0-1.0;
 sp->noiseState=sp->noiseState*.87+white*.13;
 out+=sp->noiseAmplitude*(white*.35+sp->noiseState*.65);
 sp->outputFrames++;
 /* Soft output safety net, NOT the primary mixing gain management. */
 double a=scratch_abs_d(out);
 if(a>.86){double extra=a-.86;double y=.86+.12*extra/(.12+extra);out=out<0?-y:y;}
 return (float)out;
}
/* Motion and hold share exactly one oscillator state. Source position alone
   chooses fresh timbral snapshots, never neighboring playback fragments.
   A stopped cursor never retriggers analysis or loop/reverse synthesis. */
static float scratch_stretch_render(ApoScratchStretch*v,const float*src,unsigned long long count,
                 unsigned sr,unsigned hostRate,double cursor,double cursorSpeed,double pitchRatio){
 (void)cursorSpeed;
 if(!src||!count)return 0.0f;
 if(!sr)sr=48000;if(!hostRate)hostRate=48000;
 if(!v->initialized)scratch_stretch_init(v,cursor,sr,hostRate);
 cursor=scratch_clamp_d(cursor,0.0,(double)(count-1));
 if(!v->spec.valid){scratch_spectral_capture(v,src,count,sr,hostRate,cursor);}
 else{
  double diff=scratch_abs_d(cursor-v->lastAnalysisCursor);
  unsigned long long since=v->spec.outputFrames-v->lastAnalysisFrame;
  /* Every ~8ms at most, and only following an actual pixel/cursor change.
     Holding does not interpolate ±1 audio frame or switch to a new mode. */
  unsigned long long minHop=(unsigned long long)(hostRate/125u);
  if(minHop<64)minHop=64;
  if(diff>(double)sr*.00022&&since>=minHop)
   scratch_spectral_capture(v,src,count,sr,hostRate,cursor);
 }
 v->wasStationary=scratch_abs_d(cursor-v->lastAnalysisCursor)<(double)sr*.00022;
 return scratch_spectral_render(v,pitchRatio);
}
#endif
