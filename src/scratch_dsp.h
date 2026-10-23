/* Scratch Edition v0.9.7 - full spectrum phase-coherent overlap/add.
   Retains ALL frequency bins, including the broadband/transient components
   lost by the old 112-partial oscillator bank. No short time-domain loops.
   The output phases stay continuous when a source cursor is stationary.
   Freestanding Win64, no CRT or external runtime dependency.
*/
#ifndef APO_SCRATCH_DSP_H
#define APO_SCRATCH_DSP_H
#define APO_SPECTRAL_FFT 2048u
#define APO_SPECTRAL_HOP 256u
#define APO_SPECTRAL_HALF 1024u
#define APO_SPECTRAL_RING 8192u
#define APO_SPECTRAL_PI 3.1415926535897932384626433832795
#define APO_SPECTRAL_TWO_PI 6.283185307179586476925286766559

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
 unsigned long long outputFrames,generatedFrames;
 double readPosition;
 double currentPitch;
 unsigned sampleRate,sourceRate;
 unsigned initialized,phaseValid;
 unsigned randomState;
 unsigned transientCount;
 double priorAnalysisCursor,observedMotion,steadyMotion;
 double directMix, dryReadPos, directRate;
} ApoSpectralFreeze;
typedef struct {
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
static void scratch_ifft(double*re,double*im){
 for(unsigned j=0;j<APO_SPECTRAL_FFT;j++)im[j]=-im[j];
 scratch_fft(re,im);
 for(unsigned j=0;j<APO_SPECTRAL_FFT;j++){re[j]/=(double)APO_SPECTRAL_FFT;im[j]/=-(double)APO_SPECTRAL_FFT;}
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
 v->initialized=1;v->freezeValid=0;v->wasStationary=1;
 v->freezeCursor=cursor;v->lastAnalysisCursor=cursor;
 v->lastAnalysisFrame=0;
 ApoSpectralFreeze*sp=&v->spec;
 sp->sampleRate=outputRate;sp->sourceRate=sourceRate;
 sp->outputFrames=0;sp->generatedFrames=0;sp->readPosition=0.;sp->phaseValid=0;sp->randomState=0x6a339c17u;
 sp->currentPitch=1.;sp->initialized=1;sp->transientCount=0;
 sp->priorAnalysisCursor=cursor;sp->observedMotion=0;sp->steadyMotion=0;sp->directMix=0.;
 sp->dryReadPos=cursor-(double)APO_SPECTRAL_FFT*.5;sp->directRate=1.;
 for(unsigned i=0;i<APO_SPECTRAL_RING;i++){sp->ring[i]=0;sp->rendered[i]=0;}
 for(unsigned k=0;k<=APO_SPECTRAL_HALF;k++){
  sp->synthPhase[k]=0;sp->srcPhase[k]=0;
  sp->olderMag[k]=0;sp->frequency[k]=APO_SPECTRAL_TWO_PI*(double)k/(double)APO_SPECTRAL_FFT;
 }
 for(unsigned i=0;i<APO_SPECTRAL_FFT;i++){
  double sn,cs;scratch_sincos(APO_SPECTRAL_TWO_PI*(double)i/(double)APO_SPECTRAL_FFT,&sn,&cs);
  sp->fftWindow[i]=.5-.5*cs;
 }
}
/* Captures a full spectrum around a source location. A second window one
   source hop later estimates the natural instantaneous frequency; source
   motion does not affect fundamental pitch. Both windows use decoded PCM. */
static void scratch_analyze(ApoSpectralFreeze*sp,const float*src,unsigned long long count,double cursor){
 double center=cursor-(double)APO_SPECTRAL_FFT*.5;
 for(unsigned pass=0;pass<2;pass++){
  double base=center+(double)(pass*APO_SPECTRAL_HOP);
  for(unsigned j=0;j<APO_SPECTRAL_FFT;j++){
   double at=base+(double)j;
   double v=at<0||at>(double)(count-1)?0.0:(double)scratch_read_source(src,count,at);
   sp->re[j]=v*sp->fftWindow[j];sp->im[j]=0.0;
  }
  scratch_fft(sp->re,sp->im);
  if(pass==0){
   for(unsigned j=0;j<=APO_SPECTRAL_HALF;j++){
    sp->analysisRe[j]=sp->re[j];sp->analysisIm[j]=sp->im[j];
    double r=sp->re[j],im=sp->im[j];
    sp->mag[j]=scratch_sqrt(r*r+im*im);
    sp->phase[j]=scratch_atan2(im,r);
   }
  }
 }
 for(unsigned k=0;k<=APO_SPECTRAL_HALF;k++){
  double r=sp->re[k],im=sp->im[k];
  double after=scratch_atan2(im,r);
  double centerOmega=APO_SPECTRAL_TWO_PI*(double)k/(double)APO_SPECTRAL_FFT;
  double delta=scratch_wrap_angle(after-sp->phase[k]-centerOmega*(double)APO_SPECTRAL_HOP);
  double omega=centerOmega+delta/(double)APO_SPECTRAL_HOP;
  sp->frequency[k]=scratch_clamp_d(omega*(double)sp->sourceRate/(double)sp->sampleRate,0.,APO_SPECTRAL_PI);
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
 int onset=sp->phaseValid&&total>1e-5&&positive/total>.44;
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
  double target=(double)k*(double)sp->sourceRate/(double)sp->sampleRate;
  unsigned bin=(unsigned)target;
  double frac=target-(double)bin;
  if(bin>0&&bin<half){sp->re[bin]+=strength*(1.-frac)*cs;sp->im[bin]+=strength*(1.-frac)*sn;}
  if(bin+1<half){sp->re[bin+1]+=strength*frac*cs;sp->im[bin+1]+=strength*frac*sn;}
 }
 /* Retain DC/edge frequencies only when safe. */
 sp->re[0]=sp->analysisRe[0];sp->im[0]=0.;
 sp->re[half]=0.;sp->im[half]=0.;
 for(unsigned k=1;k<half;k++){sp->re[n-k]=sp->re[k];sp->im[n-k]=-sp->im[k];}
 scratch_ifft(sp->re,sp->im);
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
static float scratch_stretch_render(ApoScratchStretch*v,const float*src,unsigned long long count,
                                    unsigned sr,unsigned hostRate,double cursor,double cursorSpeed,double pitchRatio){
 if(!src||!count)return 0.0f;
 if(!sr)sr=48000;if(!hostRate)hostRate=48000;
 if(!v->initialized)scratch_stretch_init(v,cursor,sr,hostRate);
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
 unsigned long long needed=first+2ull;
 while(sp->generatedFrames<=needed){
  unsigned long long t=sp->generatedFrames;
  if((t&(APO_SPECTRAL_HOP-1u))==0){
   double motion=(cursor-sp->priorAnalysisCursor)/((double)APO_SPECTRAL_HOP*sourceStep);
   if(!sp->phaseValid)motion=0.;
   motion=scratch_clamp_d(motion,-7.,7.);
   sp->observedMotion=sp->observedMotion*.78+motion*.22;
   double vMouse=scratch_clamp_d(cursorSpeed,-7.,7.);
   sp->steadyMotion=(scratch_abs_d(vMouse)>.10)?(.38*sp->observedMotion+.62*vMouse):sp->observedMotion;
   sp->priorAnalysisCursor=cursor;
   scratch_synthesis_frame(v,src,count,cursor);
  }
  unsigned slot=(unsigned)(t&(APO_SPECTRAL_RING-1u));
  sp->rendered[slot]=sp->ring[slot];sp->ring[slot]=0.0f;
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
 sp->readPosition+=sp->currentPitch;
 unsigned long long t=sp->outputFrames;
 /* Lossless decoded PCM on sustained approximately 1x forward/reverse
    movement, only when the user Pitch setting is at neutral. Never use
    source resampling to implement slow motion: that changes pitch. */
 double rate=sp->steadyMotion;
 int directReady=(scratch_abs_d(scratch_abs_d(rate)-1.)<.13&&scratch_abs_d(sp->currentPitch-1.)<.022);
 double mixStep=1./((double)hostRate*.029);
 if(directReady){sp->directMix+=mixStep;if(sp->directMix>1.)sp->directMix=1.;}
 else{sp->directMix-=mixStep;if(sp->directMix<0.)sp->directMix=0.;}
 double wanted=cursor-(double)APO_SPECTRAL_FFT*.5;
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
 float raw=scratch_read_source(src,count,sp->dryReadPos);
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
