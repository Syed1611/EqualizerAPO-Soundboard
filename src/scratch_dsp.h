/* Scratch Edition v1.0.1 experimental - hybrid WSOLA + full-spectrum FFT.
   During moving Scrub, a pitch-preserving waveform-similarity overlap/add
   stream can carry speech transients alongside spectral synthesis. At rest,
   the original full-spectrum phase-locked spectral hold is retained.
   No external runtime or CRT required. */
#ifndef APO_SCRATCH_DSP_H
#define APO_SCRATCH_DSP_H
#define APO_SPECTRAL_FFT 2048u
#define APO_SPECTRAL_HOP 256u
#define APO_SPECTRAL_HALF 1024u
#define APO_SPECTRAL_RING 8192u
#define APO_WS_GRAIN 1024u
#define APO_WS_LOOKAHEAD ((APO_SPECTRAL_FFT-APO_WS_GRAIN)/2u)
#define APO_SPECTRAL_PI 3.1415926535897932384626433832795
#define APO_SPECTRAL_TWO_PI 6.283185307179586476925286766559
#include "r7_dsp_tables.h"

typedef struct {
 double re[APO_SPECTRAL_FFT],im[APO_SPECTRAL_FFT];
 double analysisRe[APO_SPECTRAL_FFT],analysisIm[APO_SPECTRAL_FFT];
 double mag[APO_SPECTRAL_HALF+1],phase[APO_SPECTRAL_HALF+1];
 double olderMag[APO_SPECTRAL_HALF+1];
 double synthPhase[APO_SPECTRAL_HALF+1];
 double srcPhase[APO_SPECTRAL_HALF+1];
 double frequency[APO_SPECTRAL_HALF+1];
 double fftWindow[APO_SPECTRAL_FFT];
 float ring[APO_SPECTRAL_RING];
 float rendered[APO_SPECTRAL_RING];
 float wsRing[APO_SPECTRAL_RING],wsRendered[APO_SPECTRAL_RING];
 float wsWindow[APO_WS_GRAIN];
 float r14Weight[APO_SPECTRAL_RING];
 double r14LongRe[APO_SPECTRAL_HALF+1],r14LongIm[APO_SPECTRAL_HALF+1];
 int r14Transient;
 double wsMix,wsConfidence,wsLastShift,wsLastScore;
 double analysisCursor,trackingMotion;
 int analysisCursorValid;
 double spectralFloor;
 double freqSmooth[APO_SPECTRAL_HALF+1];
 unsigned r9Ready,r9ActualRate;
 unsigned long long r9ActualFrames;
 unsigned long long outputFrames,generatedFrames;
 double readPosition;
 double currentPitch;
 unsigned sampleRate,sourceRate;
 unsigned initialized,phaseValid;
 unsigned quality; /* 0 legacy hybrid, 1 R7 HQ hybrid, 2 R8 spectral detail */
 unsigned randomState;
 unsigned transientCount;
 unsigned reversalFrames; int reversalSign;
 double priorAnalysisCursor,observedMotion,steadyMotion;
 double directMix, dryReadPos, directRate;
} ApoSpectralFreeze;
typedef struct { unsigned ready,sr,host,hop,phase; unsigned long long frames; int direction; double a,b,stepA,stepB,pitch,level; } R15Wave;
typedef struct {
 R15Wave wave;
 double freezeCursor,lastAnalysisCursor;
 unsigned long long lastAnalysisFrame;
 int initialized,freezeValid,wasStationary;
 ApoSpectralFreeze spec;
} ApoScratchStretch;
static double scratch_abs_d(double x){return x<0?-x:x;}
static double scratch_clamp_d(double x,double lo,double hi){return x<lo?lo:(x>hi?hi:x);}
static double scratch_wrap_angle(double x){while(x>APO_SPECTRAL_PI)x-=APO_SPECTRAL_TWO_PI;while(x< -APO_SPECTRAL_PI)x+=APO_SPECTRAL_TWO_PI;return x;}
static void scratch_sincos(double a,double*sn,double*cs){
 while(a>APO_SPECTRAL_PI)a-=APO_SPECTRAL_TWO_PI;
 while(a< -APO_SPECTRAL_PI)a+=APO_SPECTRAL_TWO_PI;
 double sign=1.;
 if(a>APO_SPECTRAL_PI*.5){a=APO_SPECTRAL_PI-a;sign=-1.;}
 else if(a< -APO_SPECTRAL_PI*.5){a=-APO_SPECTRAL_PI-a;sign=-1.;}
 double q=a*a;
 *sn=a*(1.+q*(-1./6.+q*(1./120.+q*(-1./5040.+q*(1./362880.-q/39916800.)))));
 *cs=sign*(1.+q*(-.5+q*(1./24.+q*(-1./720.+q*(1./40320.-q/3628800.)))));
}
static double scratch_sqrt(double x){
 if(!(x>0))return 0.;double y=x>=1?x:1.;
 for(int i=0;i<18;i++)y=.5*(y+x/y);return y;
}
static double scratch_atan2(double y,double x){
 /* Low-error atan2 approximation; no CRT dependency. */
 const double q=APO_SPECTRAL_PI*.25;
 double ay=scratch_abs_d(y)+1e-30,r,a;
 if(x>=0){r=(x-ay)/(x+ay);a=q+(.1963*r*r-.9817)*r;}
 else{r=(x+ay)/(ay-x);a=3.*q+(.1963*r*r-.9817)*r;}
 return y<0?-a:a;
}
/* Argument reduction keeps |z| <= tan(pi/8); alternating series error < 2e-10. */
static double scratch_atan2_hq(double y,double x){
 double ax=scratch_abs_d(x),ay=scratch_abs_d(y);if(ax+ay<1e-30)return 0.;
 double z=ax>=ay?ay/ax:ax/ay, offset=0.;
 if(z>.414213562373095){z=(z-1.)/(z+1.);offset=APO_SPECTRAL_PI*.25;}
 double zz=z*z, term=z, a=z;
 for(int i=1;i<=10;i++){term*=-zz;a+=term/(2*i+1);}
 a+=offset;if(ay>ax)a=APO_SPECTRAL_PI*.5-a;if(x<0)a=APO_SPECTRAL_PI-a;return y<0?-a:a;
}
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
   double wr=1.,wi=0.;
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
static void r7_fft(double*re,double*im){
 const unsigned n=APO_SPECTRAL_FFT;
 for(unsigned i=1,j=0;i<n;i++){unsigned bit=n>>1;while(j&bit){j^=bit;bit>>=1;}j^=bit;if(i<j){double a=re[i],b=im[i];re[i]=re[j];im[i]=im[j];re[j]=a;im[j]=b;}}
 for(unsigned len=2;len<=n;len<<=1){unsigned stride=n/len;for(unsigned at=0;at<n;at+=len)for(unsigned j=0;j<len/2;j++){const double*w=r7_twiddle[j*stride];unsigned a=at+j,b=a+len/2;double tr=w[0]*re[b]-w[1]*im[b],ti=w[0]*im[b]+w[1]*re[b];re[b]=re[a]-tr;im[b]=im[a]-ti;re[a]+=tr;im[a]+=ti;}}
}
static double r7_sqrt(double x){if(x<=0.)return 0.;
#if defined(__x86_64__) || defined(_M_X64)
 double y;__asm__("sqrtsd %1, %0":"=x"(y):"x"(x));return y;
#else
 return scratch_sqrt(x);
#endif
}
static void scratch_ifft(double*re,double*im){
 for(unsigned j=0;j<APO_SPECTRAL_FFT;j++)im[j]=-im[j];
 scratch_fft(re,im);
 for(unsigned j=0;j<APO_SPECTRAL_FFT;j++){re[j]/=(double)APO_SPECTRAL_FFT;im[j]/=-(double)APO_SPECTRAL_FFT;}
}
/* Table cutoff is rounded down to avoid aliasing on fractional downsampling. */
static unsigned r7_band(double step){if(step<=1.)return R7_BANDS-1;int n=(int)(R7_BANDS/step);if(n<1)n=1;if(n>R7_BANDS)n=R7_BANDS;return (unsigned)(n-1);}
static float r7_pcm(const float*s,unsigned long long count,double at,double step){
 if(!s||!count)return 0.;if(at<0.)at=0.;if(at>(double)(count-1))at=(double)(count-1);
 unsigned long long first=(unsigned long long)at;double frac=at-first;if(step<=1.&&frac<1e-10)return s[first];
 double phase=frac*R7_PHASES;unsigned ph=(unsigned)phase;if(ph>=R7_PHASES)ph=R7_PHASES-1;double mix=phase-ph;
 const float*a=r7_sinc[r7_band(step)][ph],*b=a+R7_TAPS;double sum=0.;
 for(int j=0;j<R7_TAPS;j++){long long ix=(long long)first+j-(R7_TAPS/2-1);if(ix<0)ix=0;if((unsigned long long)ix>=count)ix=(long long)(count-1);sum+=s[ix]*(a[j]+(b[j]-a[j])*mix);}return (float)sum;
}
static float r7_ring_pcm(const float*s,unsigned long long first,double frac,double step){
 if(step<=1.&&frac<1e-10)return s[first&(APO_SPECTRAL_RING-1u)];double phase=frac*R7_PHASES;unsigned ph=(unsigned)phase;if(ph>=R7_PHASES)ph=R7_PHASES-1;double mix=phase-ph;
 const float*a=r7_sinc[r7_band(step)][ph],*b=a+R7_TAPS;double sum=0.;
 for(int j=0;j<R7_TAPS;j++){long long ix=(long long)first+j-(R7_TAPS/2-1);if(ix<0)ix=0;sum+=s[(unsigned long long)ix&(APO_SPECTRAL_RING-1u)]*(a[j]+(b[j]-a[j])*mix);}return (float)sum;
}
static float scratch_read_source(const float*s,unsigned long long count,double at){
 if(!s||!count)return 0.0f;
 if(at<=0.0)return s[0];
 if(at>=(double)(count-1))return s[count-1];
 unsigned long long i=(unsigned long long)at;
 unsigned long long i0=i?i-1:i,i2=i+1<count?i+1:i,i3=i2+1<count?i2+1:i2;
 double t=at-(double)i,t2=t*t,t3=t2*t;
 double a=s[i0],b=s[i],c=s[i2],d=s[i3];
 return (float)(.5*((2.*b)+(-a+c)*t+(2.*a-5.*b+4.*c-d)*t2+(-a+3.*b-3.*c+d)*t3));
}
static void scratch_stretch_init(ApoScratchStretch*v,double cursor,unsigned sourceRate,unsigned outputRate){
 if(!sourceRate)sourceRate=48000;if(!outputRate)outputRate=48000;
 v->wave.ready=0;v->initialized=1;v->freezeValid=0;v->wasStationary=1;
 v->freezeCursor=cursor;v->lastAnalysisCursor=cursor;
 v->lastAnalysisFrame=0;
 ApoSpectralFreeze*sp=&v->spec;
 sp->sampleRate=outputRate;sp->sourceRate=sourceRate;sp->quality=0;sp->r9Ready=0;
 sp->outputFrames=0;sp->generatedFrames=0;sp->readPosition=0.;sp->phaseValid=0;sp->randomState=0x6a339c17u;
 sp->currentPitch=1.;sp->initialized=1;sp->transientCount=0;sp->reversalFrames=0;sp->reversalSign=0;
 sp->priorAnalysisCursor=cursor;sp->observedMotion=0;sp->steadyMotion=0;sp->directMix=0.;
 sp->wsMix=0.;sp->wsConfidence=.45;sp->wsLastShift=0.;sp->wsLastScore=0.;
 sp->analysisCursor=cursor;sp->trackingMotion=0.;sp->analysisCursorValid=0;
 sp->dryReadPos=cursor-(double)APO_SPECTRAL_FFT*.5;sp->directRate=1.;
 for(unsigned i=0;i<APO_SPECTRAL_RING;i++){sp->ring[i]=0;sp->rendered[i]=0;
  sp->wsRing[i]=0;sp->wsRendered[i]=0;}
 for(unsigned k=0;k<=APO_SPECTRAL_HALF;k++){
  sp->synthPhase[k]=0;sp->srcPhase[k]=0;
  sp->olderMag[k]=0;sp->frequency[k]=APO_SPECTRAL_TWO_PI*(double)k/(double)APO_SPECTRAL_FFT;

 }
 for(unsigned i=0;i<APO_SPECTRAL_FFT;i++){
  double sn,cs;scratch_sincos(APO_SPECTRAL_TWO_PI*(double)i/(double)APO_SPECTRAL_FFT,&sn,&cs);
  sp->fftWindow[i]=.5-.5*cs;
 }
 for(unsigned i=0;i<APO_WS_GRAIN;i++){
  double sn,cs;scratch_sincos(APO_SPECTRAL_TWO_PI*(double)i/(double)APO_WS_GRAIN,&sn,&cs);
  sp->wsWindow[i]=(float)(.5-.5*cs);
 }
}
/* Captures a full spectrum around a source location. A second window one
   source hop later estimates the natural instantaneous frequency; source
   motion does not affect fundamental pitch. Both windows use decoded PCM. */
static void scratch_analyze(ApoSpectralFreeze*sp,const float*src,unsigned long long count,double cursor){
 double analysisStep=sp->quality?(double)sp->sourceRate/sp->sampleRate:1.;
 double rateScale=sp->quality?1.:(double)sp->sourceRate/sp->sampleRate;
 double center=cursor-(double)APO_SPECTRAL_FFT*.5*analysisStep;
 for(unsigned pass=0;pass<2;pass++){
  double base=center+(double)(pass*APO_SPECTRAL_HOP)*analysisStep;
  for(unsigned j=0;j<APO_SPECTRAL_FFT;j++){
   double at=base+(double)j*analysisStep;
   double v=at<0||at>(double)(count-1)?0.0:(double)(sp->quality?r7_pcm(src,count,at,analysisStep):scratch_read_source(src,count,at));
   sp->re[j]=v*sp->fftWindow[j];sp->im[j]=0.0;
  }
  if(sp->quality)r7_fft(sp->re,sp->im);else scratch_fft(sp->re,sp->im);
  if(pass==0){
   for(unsigned j=0;j<=APO_SPECTRAL_HALF;j++){
    sp->analysisRe[j]=sp->re[j];sp->analysisIm[j]=sp->im[j];
    double r=sp->re[j],im=sp->im[j];
    sp->mag[j]=sp->quality?r7_sqrt(r*r+im*im):scratch_sqrt(r*r+im*im);
    sp->phase[j]=sp->quality?scratch_atan2_hq(im,r):scratch_atan2(im,r);
   }
  }
 }
 /* Suppress unstable phase derivatives in near-silent frequency bins.
    R3 smoothed every measured frequency at 57% each hop; that changes
    harmonic trajectories and can create a slow, metallic chorusing during
    long vocal stretches. Preserve true measured harmonic frequency in
    audible bins, and use the FFT-bin center only for the noise floor. */
 double peak=0.;
 for(unsigned k=0;k<=APO_SPECTRAL_HALF;k++)if(sp->mag[k]>peak)peak=sp->mag[k];
 sp->spectralFloor=peak*.00035;
 for(unsigned k=0;k<=APO_SPECTRAL_HALF;k++){
  double base=APO_SPECTRAL_TWO_PI*(double)k/(double)APO_SPECTRAL_FFT;
  if(sp->mag[k]<sp->spectralFloor){
   sp->frequency[k]=base*rateScale;
   continue;
  }
  double after=sp->quality?scratch_atan2_hq(sp->im[k],sp->re[k]):scratch_atan2(sp->im[k],sp->re[k]);
  double delta=scratch_wrap_angle(after-sp->phase[k]-base*(double)APO_SPECTRAL_HOP);
  double omega=base+delta/(double)APO_SPECTRAL_HOP;
  sp->frequency[k]=scratch_clamp_d(omega*rateScale,0.,APO_SPECTRAL_PI);
 }
}
/* Synthesize all 1025 bins instead of selected oscillator peaks. Identity
   phase locking at local magnitude maxima keeps nearby bins together and
   reduces the reverberant/underwater phase-vocoder sound. */
static void scratch_synthesis_frame(ApoScratchStretch*v,const float*src,unsigned long long count,
                                    double cursor){
 ApoSpectralFreeze*sp=&v->spec;
 scratch_analyze(sp,src,count,cursor);
 const unsigned half=APO_SPECTRAL_HALF,hop=APO_SPECTRAL_HOP,n=APO_SPECTRAL_FFT;
 /* Dynamic transients: re-align phase on sharp, broadband onsets. No
    blending of neighboring decoded segments, only overlapping FFT frames. */
 double positive=0.,total=0.;
 if(sp->phaseValid){
  for(unsigned k=2;k<half;k++){
   double m=sp->mag[k],old=sp->olderMag[k];
   if(m>old)positive+=m-old;
   total+=m;
  }
 }
 /* More responsive spectral-flux threshold captures brief speech consonants. */
 int onset=sp->phaseValid&&total>1e-5&&positive/total>.34;
 if(onset)sp->transientCount++;
 /* Track natural instantaneous frequency, independent of how quickly the
    user moves the pointer; Pitch acts independently and is smoothed. */
 if(!sp->phaseValid){
  for(unsigned k=0;k<=half;k++)sp->synthPhase[k]=sp->phase[k];
  sp->phaseValid=1;
 }else{
  for(unsigned k=0;k<=half;k++){
   double d=sp->frequency[k]*(double)hop;
   sp->synthPhase[k]=scratch_wrap_angle(sp->synthPhase[k]+d);
  }
 }
 for(unsigned k=0;k<=half;k++)sp->olderMag[k]=sp->mag[k];
 /* Real FFT construction. Keep complex per-bin fine structure: speech
    noise, breath and consonants are no longer reduced to 112 peaks. */
 for(unsigned k=0;k<n;k++){sp->re[k]=0.;sp->im[k]=0.;}
 unsigned nearest=0;
 for(unsigned k=1;k<half;k++){
  if(k>1&&k<half-1&&sp->mag[k]>=sp->mag[k-1]&&sp->mag[k]>=sp->mag[k+1])nearest=k;
  double strength=sp->mag[k];
  if(strength<1e-12)continue;
  double phase=sp->synthPhase[k];
  /* Preserve inter-bin phase of bins that belong to one vocal harmonic. */
  if(nearest>0&&k-nearest<=3&&sp->mag[nearest]>strength*1.5&&sp->directMix<0.9){
   phase=sp->synthPhase[nearest]+scratch_wrap_angle(sp->phase[k]-sp->phase[nearest]);
  }
  if(onset){ /* short transient emphasis via phase realignment */
   phase=sp->phase[k];sp->synthPhase[k]=phase;
  }
  double sn,cs;scratch_sincos(phase,&sn,&cs);
  double target=sp->quality?(double)k:(double)k*(double)sp->sourceRate/(double)sp->sampleRate;
  unsigned bin=(unsigned)target;
  double frac=target-(double)bin;
  if(bin>0&&bin<half){sp->re[bin]+=strength*(1.-frac)*cs;sp->im[bin]+=strength*(1.-frac)*sn;}
  if(bin+1<half){sp->re[bin+1]+=strength*frac*cs;sp->im[bin+1]+=strength*frac*sn;}
 }
 /* Retain DC/edge frequencies only when safe. */
 sp->re[0]=sp->analysisRe[0];sp->im[0]=0.;
 sp->re[half]=0.;sp->im[half]=0.;
 for(unsigned k=1;k<half;k++){sp->re[n-k]=sp->re[k];sp->im[n-k]=-sp->im[k];}
 if(sp->quality){for(unsigned j=0;j<n;j++)sp->im[j]=-sp->im[j];r7_fft(sp->re,sp->im);for(unsigned j=0;j<n;j++){sp->re[j]/=n;sp->im[j]/=-(double)n;}}else scratch_ifft(sp->re,sp->im);
 unsigned long long start=sp->generatedFrames;
 for(unsigned j=0;j<n;j++){
  /* Hann analysis and Hann synthesis, overlap 8x -> sum of squared
     Hann windows is exactly 3.0 in steady state. */
  double sample=sp->re[j]*sp->fftWindow[j]*(1.0/3.0);
  unsigned ix=(unsigned)((start+j)&(APO_SPECTRAL_RING-1u));
  sp->ring[ix]+=(float)sample;
 }
 v->lastAnalysisCursor=cursor;v->lastAnalysisFrame=start;
 v->freezeCursor=cursor;v->freezeValid=1;
}
/* Phase-aware waveform-similarity overlap/add. Grains are 1024 samples
   (21 ms at 48 kHz) and start every 256 frames. Four Hann windows overlap;
   the rendered stream is normalized by 2.0. Grain alignment is searched
   around the current pointer, with a continuity penalty against jumping
   between plausible but phase-mismatched pitch periods. */
static void scratch_ws_synthesis_frame(ApoSpectralFreeze*sp,const float*src,
                                       unsigned long long count,double cursor,
                                       unsigned long long t){
 const unsigned mask=APO_SPECTRAL_RING-1u;
 double step=(double)sp->sourceRate/(double)sp->sampleRate;
 unsigned long long start=t+APO_WS_LOOKAHEAD;
 double target=cursor-(double)(APO_WS_GRAIN/2u)*step;
 double best=-3.,bestShift=0.;
 if(t>=APO_SPECTRAL_HOP){
  /* 2-sample search gives much less quantization jitter than the previous
     4-sample grid. Use multiple overlap positions for a voiced match. */
  for(int shift=-128;shift<=128;shift+=2){
   double dot=0.,old2=1.e-10,new2=1.e-10;
   double offset=(double)shift*step;
   for(unsigned j=24;j<APO_WS_GRAIN/2u-12u;j+=7u){
    float prior=sp->wsRing[(unsigned)((start+j)&mask)];
    float cand=scratch_read_source(src,count,target+offset+(double)j*step);
    dot+=(double)prior*cand;old2+=(double)prior*prior;new2+=(double)cand*cand;
   }
   double score=dot/(sp->quality?r7_sqrt(old2*new2):scratch_sqrt(old2*new2));
   /* Avoid switching between harmonic periods on every 256-frame hop. */
   score-=.085*scratch_abs_d((double)shift-sp->wsLastShift)/128.;
   score-=.018*scratch_abs_d((double)shift)/128.;
   if(score>best){best=score;bestShift=(double)shift;}
  }
  /* Large unexpected jumps imply low similarity and should use spectral
     processing instead of stitching two unrelated speech segments. */
  double jump=scratch_abs_d(bestShift-sp->wsLastShift);
  double confidence=scratch_clamp_d((best+.08)*1.2,0.,1.);
  if(jump>72.&&best<.82)confidence*=.50;
  sp->wsConfidence=sp->wsConfidence*.76+confidence*.24;
 }else sp->wsConfidence=.35;
 sp->wsLastShift=bestShift;sp->wsLastScore=best;
 double base=target+bestShift*step;
 for(unsigned j=0;j<APO_WS_GRAIN;j++){
  unsigned ix=(unsigned)((start+j)&mask);
  sp->wsRing[ix]+=(sp->quality?r7_pcm(src,count,base+(double)j*step,step):scratch_read_source(src,count,base+(double)j*step))*sp->wsWindow[j];
 }
}

static float scratch_stretch_render(ApoScratchStretch*v,const float*src,unsigned long long count,
                                    unsigned sr,unsigned hostRate,double cursor,double cursorSpeed,double pitchRatio){
 if(!src||!count)return 0.0f;
 if(!sr)sr=48000;if(!hostRate)hostRate=48000;
 if(!v->initialized)scratch_stretch_init(v,cursor,sr,hostRate);
 else if(v->spec.sampleRate!=hostRate||v->spec.sourceRate!=sr){unsigned quality=v->spec.quality;scratch_stretch_init(v,cursor,sr,hostRate);v->spec.quality=quality;}
 ApoSpectralFreeze*sp=&v->spec;
 cursor=scratch_clamp_d(cursor,0.0,(double)(count-1));
 pitchRatio=scratch_clamp_d(pitchRatio,.5,2.);
 /* Pitch smoothing prevents zipper noise and tracks the knob live. */
 sp->currentPitch+=(pitchRatio-sp->currentPitch)/((double)hostRate*.013);
 const double sourceStep=(double)sr/(double)hostRate;
 /* A continuous full-spectrum stream is synthesized at native pitch.
    Resample this completed stream for the independent live Pitch control.
    This avoids bin reallocation/comb cancellation at nonzero pitch. */
 double sampleAt=sp->readPosition;
 unsigned long long first=(unsigned long long)sampleAt;
 unsigned long long needed=first+(sp->quality?24ull:2ull);
 while(sp->generatedFrames<=needed){
  unsigned long long t=sp->generatedFrames;
  if((t&(APO_SPECTRAL_HOP-1u))==0){
   double motion=(cursor-sp->priorAnalysisCursor)/((double)APO_SPECTRAL_HOP*sourceStep);
   if(!sp->phaseValid)motion=0.;
   motion=scratch_clamp_d(motion,-7.,7.);
   double vMouse=scratch_clamp_d(cursorSpeed,-7.,7.);
   if(scratch_abs_d(vMouse)<.08&&scratch_abs_d(motion)<.018){
    /* An actually stationary pointer is a spectral freeze, not an
       exponentially decaying phantom movement. R3 kept selecting WSOLA
       for extra analysis hops, repeating grains after the cursor stopped. */
    sp->observedMotion=0.;sp->steadyMotion=0.;
   }else{
    sp->observedMotion=sp->observedMotion*.78+motion*.22;
    sp->steadyMotion=(scratch_abs_d(vMouse)>.10)?(.38*sp->observedMotion+.62*vMouse):sp->observedMotion;
   }
   sp->priorAnalysisCursor=cursor;
   /* GUI pointer coordinates update less frequently than 256-frame
      analysis hops. Predict intermediate source positions instead of
      re-analyzing the exact same frame twice, which sounded like flutter
      during long slow stretches. Re-anchor gently to actual cursor data. */
   if(!sp->analysisCursorValid){sp->analysisCursor=cursor;sp->analysisCursorValid=1;}
   double vel=sp->steadyMotion;
   double maximum=(double)APO_SPECTRAL_HOP*sourceStep;
   if(scratch_abs_d(vel)<.035){
    sp->analysisCursor+=(cursor-sp->analysisCursor)*.72;
    if(scratch_abs_d(cursor-sp->analysisCursor)<.5)sp->analysisCursor=cursor;
   }else{
    double predicted=sp->analysisCursor+vel*maximum;
    double error=cursor-predicted;
    /* Don't follow stale UI coordinates backward on every audio hop. */
    double correction=scratch_clamp_d(error,-maximum*.20,maximum*.20);
    sp->analysisCursor=predicted+correction;
   }
   sp->analysisCursor=scratch_clamp_d(sp->analysisCursor,0.,(double)(count-1));
   scratch_synthesis_frame(v,src,count,sp->analysisCursor);
   /* R8: no uncorrelated grain overlap in detail mode. Keep the complete
      phase-locked spectrum during motion, as in v1.0.1. Legacy modes retain
      their original WSOLA path and stationary-hold behavior. */
   if(sp->quality!=2) scratch_ws_synthesis_frame(sp,src,count,sp->analysisCursor,t);
  }
  unsigned slot=(unsigned)(t&(APO_SPECTRAL_RING-1u));
  sp->rendered[slot]=sp->ring[slot];sp->ring[slot]=0.0f;
  sp->wsRendered[slot]=sp->wsRing[slot]*.5f;sp->wsRing[slot]=0.0f;
  sp->generatedFrames=t+1;
 }
 /* Cubic interpolation for fractional resampling. Exactly unity at
    1.0x, including frequencies near Nyquist. */
 double frac=sampleAt-(double)first;
 float p0=sp->rendered[(unsigned)((first?first-1:first)&(APO_SPECTRAL_RING-1u))];
 float p1=sp->rendered[(unsigned)(first&(APO_SPECTRAL_RING-1u))];
 float p2=sp->rendered[(unsigned)((first+1ull)&(APO_SPECTRAL_RING-1u))];
 float p3=sp->rendered[(unsigned)((first+2ull)&(APO_SPECTRAL_RING-1u))];
 double f2=frac*frac,f3=f2*frac;
 float processed=(float)(.5*((2.*p1)+(-p0+p2)*frac+(2.*p0-5.*p1+4.*p2-p3)*f2+(-p0+3.*p1-3.*p2+p3)*f3));
 /* WSOLA and FFT share the same output clock. Cubic interpolation applies
    equally to the waveform stream, so live Pitch remains independent of motion. */
 float w0=sp->wsRendered[(unsigned)((first?first-1:first)&(APO_SPECTRAL_RING-1u))];
 float w1=sp->wsRendered[(unsigned)(first&(APO_SPECTRAL_RING-1u))];
 float w2=sp->wsRendered[(unsigned)((first+1ull)&(APO_SPECTRAL_RING-1u))];
 float w3=sp->wsRendered[(unsigned)((first+2ull)&(APO_SPECTRAL_RING-1u))];
 float ws=(float)(.5*((2.*w1)+(-w0+w2)*frac+(2.*w0-5.*w1+4.*w2-w3)*f2+(-w0+3.*w1-3.*w2+w3)*f3));
 if(sp->quality){processed=r7_ring_pcm(sp->rendered,first,frac,sp->currentPitch);if(sp->quality!=2)ws=r7_ring_pcm(sp->wsRendered,first,frac,sp->currentPitch);}
 sp->readPosition+=sp->currentPitch;
 unsigned long long t=sp->outputFrames;
 /* Lossless decoded PCM on sustained approximately 1x forward/reverse
    movement, only when the user Pitch setting is at neutral. Never use
    source resampling to implement slow motion: that changes pitch. */
 double rate=sp->steadyMotion;
 double speed=scratch_abs_d(rate);
 double wsTarget=0.;
 if(sp->quality!=2&&speed>.12&&speed<2.25&&sp->outputFrames>=APO_SPECTRAL_FFT){
  /* Avoid permanent FFT+WSOLA comb filtering: use waveform-domain output
     entirely when speech matching is coherent. Spectral synthesis takes
     over for poor matches, extreme speeds and stationary holding. */
  if(sp->wsConfidence>.72)wsTarget=1.;
  else if(sp->wsConfidence<.48)wsTarget=0.;
  else wsTarget=sp->wsMix>.5?1.:0.;
 }
 /* R3's 55 ms overlap of the spectral and WSOLA signals produced audible
    phase-comb coloration. Shorten transitions to 18 ms; keep hysteresis
    so it does not flutter between engines during sustained movement. */
 double wsStep=1./((double)hostRate*.018);
 if(sp->wsMix<wsTarget){sp->wsMix+=wsStep;if(sp->wsMix>wsTarget)sp->wsMix=wsTarget;}
 else{sp->wsMix-=wsStep;if(sp->wsMix<wsTarget)sp->wsMix=wsTarget;}
 processed=(float)((double)processed*(1.-sp->wsMix)+(double)ws*sp->wsMix);
 int directReady=(scratch_abs_d(scratch_abs_d(rate)-1.)<.13&&scratch_abs_d(sp->currentPitch-1.)<.022);
 double mixStep=1./((double)hostRate*.029);
 int direction=rate<-.2?-1:(rate>.2?1:0);
 /* Reversing a fast drag used to invert direct-PCM reads immediately. First
    fade the direct path back to the phase-coherent spectral stream. */
 if(direction&&sp->reversalSign&&direction!=sp->reversalSign&&sp->directMix>.015){
  sp->reversalFrames=hostRate/48u+1u; /* ~21ms reversal crossfade */
 }
 if(direction)sp->reversalSign=direction;
 if(sp->reversalFrames){
  sp->directMix-=1.0/(double)(hostRate/48u+1u);
  if(sp->directMix<0.)sp->directMix=0.;
  sp->reversalFrames--;
 }else if(directReady){sp->directMix+=mixStep;if(sp->directMix>1.)sp->directMix=1.;}
 else{sp->directMix-=mixStep;if(sp->directMix<0.)sp->directMix=0.;}
 double wanted=cursor-(double)APO_SPECTRAL_FFT*.5*(sp->quality?sourceStep:1.);
 if(sp->directMix<.00001){sp->dryReadPos=wanted;sp->directRate=rate<0?-1.:1.;}
 else{
  double sign=rate<0?-1.:1.;
  /* Any major waveform jump returns to spectral mode; don't abruptly
     teleport a direct PCM read cursor during an audible segment. */
  double err=wanted-sp->dryReadPos;
  if(scratch_abs_d(err)>(double)sr*.045){sp->directMix-=mixStep*4.;if(sp->directMix<0.)sp->directMix=0.;}
  sp->directRate=sign;
  /* Sample FIRST, then advance: the synthesized frame and the direct
     signal refer to the same output instant, avoiding one-sample flams. */
 }
 float raw=sp->quality?r7_pcm(src,count,sp->dryReadPos,sourceStep):scratch_read_source(src,count,sp->dryReadPos);
 if(sp->directMix>.00001){
  double sign=sp->directRate;
  double error=wanted-sp->dryReadPos;
  double correction=scratch_clamp_d(error/((double)hostRate*.30),-.018,.018);
  sp->dryReadPos+=sourceStep*(sign+correction);
 }
 float out=(float)((double)processed*(1.-sp->directMix)+(double)raw*sp->directMix);
 sp->outputFrames=t+1;
 /* Output gain and final microphone-plus-soundboard limiting stay outside
    this processor. No hidden soft saturation is added here. */
 if(out>1.5f)out=1.5f;else if(out< -1.5f)out=-1.5f;
 return out;
}
#endif
