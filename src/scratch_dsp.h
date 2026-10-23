#ifndef APO_SCRATCH_DSP_H
#define APO_SCRATCH_DSP_H
/* Scratch Edition v0.9.5: spectral freeze (no short audio loops).
   A pair of windowed FFT analyses identifies spectral peaks and refines their
   instantaneous frequencies. The held sound is synthesized by continuous,
   phase-preserving oscillators plus a small noise residual. No recordings are
   replayed or wrapped during hold. Scrub motion retains the correlation-based
   sample playback used in v0.9.4, crossfading on hold/move transitions.
   Fully freestanding: no CRT, dynamic allocations or external DSP libraries.
*/
#define APO_SPECTRAL_FFT 4096u
#define APO_SPECTRAL_PARTIALS 120u
#define APO_SPECTRAL_PI 3.1415926535897932384626433832795
#define APO_SPECTRAL_TWO_PI 6.283185307179586476925286766559

typedef struct {
 double frequencyHz;
 double amplitude;
 double phaseCos,phaseSin;
 double stepCos,stepSin;
} ApoSpectralPeak;
typedef struct {
 /* Working spectrum stored with each heap-allocated client voice: large
    FFT scratch buffers must not overflow the Windows DSP thread stack. */
 double workRe[APO_SPECTRAL_FFT],workIm[APO_SPECTRAL_FFT];
 double workPrevRe[APO_SPECTRAL_FFT/2+1],workPrevIm[APO_SPECTRAL_FFT/2+1];
 double workMag[APO_SPECTRAL_FFT/2+1];
 ApoSpectralPeak peaks[APO_SPECTRAL_PARTIALS];
 unsigned peakCount;
 unsigned outputRate,sourceRate;
 unsigned pitchUpdateFrames;
 unsigned long long outputFrames;
 double currentPitch;
 double targetRms;
 double noiseAmplitude,noiseState,noisePrevious;
 unsigned randomState;
 int valid;
} ApoSpectralFreeze;
typedef struct {
 double position, incomingPosition, cursorAtStart;
 double step, frameInGrain;
 double grainFrames, fadeFrames;
 float lastOutput;
 double freezeCursor,freezeMix;
 int freezeValid,wasStationary;
 int direction, incomingDirection, crossfade, initialized;
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
 /* ~78 ms segments and ~11 ms seam, independent of source pitch. */
 v->grainFrames=(double)outputRate*0.078;
 v->fadeFrames=(double)outputRate*0.011;
 if(v->grainFrames<120)v->grainFrames=120;
 if(v->fadeFrames<16)v->fadeFrames=16;
 if(v->fadeFrames>v->grainFrames*0.25)v->fadeFrames=v->grainFrames*0.25;
 v->step=(double)sourceRate/(double)outputRate;
 v->position=cursor-v->grainFrames*v->step*0.42;
 v->incomingPosition=v->position;
 v->cursorAtStart=cursor;
 v->frameInGrain=0;
 v->direction=v->incomingDirection=1;
 v->crossfade=0;v->lastOutput=0.0f;v->freezeCursor=cursor;v->freezeMix=0;v->freezeValid=0;v->wasStationary=0;v->spec.valid=0;v->spec.peakCount=0;v->initialized=1;
}
/* Compare the outgoing seam to potential incoming segments, using a bounded
   normalized cross-correlation search. This is a WSOLA-inspired matching step
   rather than phasing two full length independent grains together. */
static double scratch_align_seam(const ApoScratchStretch*v,const float*src,unsigned long long count,
                                 double desired,int direction,unsigned srcRate){
 if(!src||count<64)return desired;
 int rad=(int)((double)srcRate*0.008);
 if(rad<3)rad=3;if(rad>1000)rad=1000;
 int stride=rad/24;if(stride<1)stride=1;
 double step=v->step;
 double fadeStep=v->fadeFrames/47.0;
 double best=desired,bestScore=-1e9;
 double powerOld=0.0;
 float old[48];
 for(int j=0;j<48;j++){
  old[j]=scratch_read_source(src,count,v->position+(double)v->direction*step*(double)j*fadeStep);
  powerOld+=(double)old[j]*old[j];
 }
 if(powerOld<1e-8)return desired;
 for(int shift=-rad;shift<=rad;shift+=stride){
  double candidate=desired+(double)shift;
  double cross=0,energy=0;
  for(int j=0;j<48;j++){
   double next=candidate+(double)direction*step*(double)j*fadeStep;
   float y=scratch_read_source(src,count,next);
   cross+=(double)y*old[j];energy+=(double)y*y;
  }
  /* Use 2*cross/(A+B) instead of cosine to penalize severe volume mismatch. */
  double score=2.0*cross/(powerOld+energy+1e-10)-0.012*scratch_abs_d((double)shift)/(double)rad;
  if(score>bestScore){bestScore=score;best=candidate;}
 }
 return bestScore>0.10?best:desired;
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
/* Analyze around the requested cursor exactly ONCE per distinct held position.
   True frequencies come from phase changes between two staggered FFT windows.
   Peak amplitudes are RMS-normalized to the source. */
static void scratch_spectral_lock(ApoScratchStretch*v,const float*src,unsigned long long count,
                                  unsigned sr,unsigned hostRate,double cursor){
 if(!src||!count)return;
 const unsigned n=APO_SPECTRAL_FFT,half=n/2,hop=n/8;
 ApoSpectralFreeze*sp=&v->spec;
 double*re=sp->workRe,*im=sp->workIm;
 double*prevRe=sp->workPrevRe,*prevIm=sp->workPrevIm;
 double*mag=sp->workMag;
 double start=cursor-(double)(n/2+hop/2);
 double sourceEnergy=0.0;
 for(int pass=0;pass<2;pass++){
  double windowStart=start+pass*(double)hop;
  for(unsigned j=0;j<n;j++){
   double st,ct;scratch_sincos(APO_SPECTRAL_TWO_PI*(double)j/(double)(n-1),&st,&ct);
   double hann=0.5-0.5*ct;
   double z=(double)scratch_read_source(src,count,windowStart+(double)j);
   re[j]=z*hann;im[j]=0;
   if(pass==1)sourceEnergy+=z*z*hann;
  }
  scratch_fft(re,im);
  if(pass==0)for(unsigned i=0;i<=half;i++){prevRe[i]=re[i];prevIm[i]=im[i];}
 }
 for(unsigned k=0;k<=half;k++)mag[k]=scratch_sqrt(re[k]*re[k]+im[k]*im[k]);
 sp->peakCount=0;sp->outputFrames=0;sp->pitchUpdateFrames=0;sp->sourceRate=sr;sp->outputRate=hostRate;
 sp->currentPitch=1.0;sp->noiseState=0.0;sp->noisePrevious=0.0;
 sp->randomState=(unsigned)((unsigned long long)cursor)^0x7f4a7c15u;
 if(!sp->randomState)sp->randomState=0xab04e155u;
 double maxMag=0.0;
 for(unsigned k=2;k<half-2;k++)if(mag[k]>maxMag)maxMag=mag[k];
 /* Select strongest peaks while avoiding duplicate bins from one FFT lobe. */
 unsigned candidates[APO_SPECTRAL_PARTIALS*2],candidateCount=0;
 for(unsigned k=2;k<half-2;k++){
  double m=mag[k];
  if(m<maxMag*.0018||m<=mag[k-1]||m<mag[k+1])continue;
  unsigned at=0;
  while(at<candidateCount&&mag[candidates[at]]>=m)at++;
  if(at>=APO_SPECTRAL_PARTIALS*2)continue;
  if(candidateCount<APO_SPECTRAL_PARTIALS*2)candidateCount++;
  for(unsigned j=candidateCount-1;j>at;j--)candidates[j]=candidates[j-1];
  candidates[at]=k;
 }
 for(unsigned ix=0;ix<candidateCount&&sp->peakCount<APO_SPECTRAL_PARTIALS;ix++){
  unsigned k=candidates[ix];
  int duplicate=0;
  for(unsigned t=0;t<sp->peakCount;t++){
   double f=sp->peaks[t].frequencyHz*(double)n/(double)sr;
   if(f>(double)k-2.0&&f<(double)k+2.0){duplicate=1;break;}
  }
  if(duplicate)continue;

  ApoSpectralPeak*pk=&sp->peaks[sp->peakCount++];
  double dotRe=re[k]*prevRe[k]+im[k]*prevIm[k];
  double dotIm=im[k]*prevRe[k]-re[k]*prevIm[k];
  double diff=scratch_atan2(dotIm,dotRe);
  double expected=APO_SPECTRAL_TWO_PI*(double)k*(double)hop/(double)n;
  double corr=scratch_wrap_angle(diff-expected);
  double bin=(double)k+corr*(double)n/(APO_SPECTRAL_TWO_PI*(double)hop);
  /* Sub-bin phase differences of noise peaks can be unstable; clamp. */
  if(bin<(double)k-1.4||bin>(double)k+1.4)bin=(double)k;
  if(bin<0.01)bin=0.01;
  pk->frequencyHz=bin*(double)sr/(double)n;
  pk->amplitude=4.0*mag[k]/(double)n;
  double phase=scratch_atan2(-im[k],re[k]);
  phase+=APO_SPECTRAL_TWO_PI*bin*((double)half-(double)hop/2.0)/(double)n;
  scratch_sincos(phase,&pk->phaseSin,&pk->phaseCos);
  pk->stepCos=1.0;pk->stepSin=0.0;
 }
 double activeEnergy=0.0;
 for(unsigned i=0;i<sp->peakCount;i++)activeEnergy+=sp->peaks[i].amplitude*sp->peaks[i].amplitude*0.5;
 double rms=scratch_sqrt(sourceEnergy/(double)n*2.0);
 double estimated=scratch_sqrt(activeEnergy);
 /* Ensure exact numeric headroom without a nonlinear distortion effect. */
 double scale=estimated>1e-8?rms/estimated:0.0;
 if(scale>2.4)scale=2.4;
 if(scale<0)scale=0;
 if(rms>0.58)scale*=0.58/rms;
 for(unsigned i=0;i<sp->peakCount;i++)sp->peaks[i].amplitude*=scale;
 sp->targetRms=rms>0.58?0.58:rms;
 /* Small shaped noise floor so consonants never become pure metallic tones. */
 sp->noiseAmplitude=(rms>0.58?0.58:rms)*0.035;
 sp->valid=1;
 v->freezeCursor=cursor;v->freezeValid=1;
}
static float scratch_spectral_render(ApoScratchStretch*v,double pitchRatio){
 ApoSpectralFreeze*sp=&v->spec;
 if(!sp->valid)return 0.0f;
 if(!(pitchRatio>0.0))pitchRatio=1.0;
 pitchRatio=scratch_clamp_d(pitchRatio,0.25,4.0);
 double follow=1.0/((double)(sp->outputRate?sp->outputRate:48000)*0.012);
 sp->currentPitch+=(pitchRatio-sp->currentPitch)*follow;
 if(sp->pitchUpdateFrames==0){
  /* Refresh oscillator increments every 32 frames; preserve each phase. */
  double inv=APO_SPECTRAL_TWO_PI*sp->currentPitch/(double)(sp->outputRate?sp->outputRate:48000);
  for(unsigned k=0;k<sp->peakCount;k++){
   ApoSpectralPeak*pk=&sp->peaks[k];
   double d=pk->frequencyHz*inv;
   scratch_sincos(d,&pk->stepSin,&pk->stepCos);
  }
  sp->pitchUpdateFrames=32;
 }
 sp->pitchUpdateFrames--;
 double sample=0.0;
 for(unsigned k=0;k<sp->peakCount;k++){
  ApoSpectralPeak*pk=&sp->peaks[k];
  sample+=pk->amplitude*pk->phaseCos;
  double co=pk->phaseCos*pk->stepCos-pk->phaseSin*pk->stepSin;
  double si=pk->phaseCos*pk->stepSin+pk->phaseSin*pk->stepCos;
  pk->phaseCos=co;pk->phaseSin=si;
  if((sp->outputFrames&1023ull)==1023ull){
   double norm=1.0/scratch_sqrt(co*co+si*si+1e-18);
   pk->phaseCos*=norm;pk->phaseSin*=norm;
  }
 }
 /* The residual is intentionally independent of fragment seams or loop indices. */
 unsigned rand=scratch_random(&sp->randomState);
 double noise=((double)(rand&0xffffffu)/8388608.0-1.0);
 sp->noiseState=0.74*sp->noiseState+0.26*noise;
 sample+=sp->noiseAmplitude*(noise*.55+sp->noiseState*.45);
 sp->outputFrames++;
 /* Smooth 0.82-soft-knee ceiling: unlike the previous hard clamp, the
    value and first derivative are continuous at the knee. */
 double absSample=scratch_abs_d(sample);
 if(absSample>0.82){
  double above=absSample-0.82;
  double y=0.82+0.17*above/(0.17+above);
  sample=sample<0.0?-y:y;
 }
 return (float)sample;
}
/* pitchRatio=2^(semitones/12), independent from the cursor speed. A pitch
   change is smoothed within ~12ms even while the mouse is held stationary. */
static float scratch_stretch_render(ApoScratchStretch*v,const float*src,unsigned long long count,
                  unsigned srcRate,unsigned hostRate,double cursor,double cursorSpeed,double pitchRatio){
 if(!src||!count)return 0.0f;
 if(!srcRate)srcRate=48000;if(!hostRate)hostRate=48000;
 if(!v->initialized||v->grainFrames<1.0)scratch_stretch_init(v,cursor,srcRate,hostRate);
 pitchRatio=scratch_clamp_d(pitchRatio,0.25,4.0);
 double targetStep=(double)srcRate/(double)hostRate*pitchRatio;
 double smooth=1.0/((double)hostRate*0.012);
 if(smooth>1.0)smooth=1.0;
 v->step+=(targetStep-v->step)*smooth;
 /* Cursor speed 0 is a genuine, locked sound freeze rather than 0.1x
    jitter between different snippets. Note: this doesn't change pitch. */
 int stationary=scratch_abs_d(cursorSpeed)<0.045;
 /* On first movement after a freeze, re-anchor to the CURRENT pointer.
    The locked-loop output fades out while the new direct segment fades in. */
 if(!stationary&&v->wasStationary){
  v->position=scratch_clamp_d(cursor,0.0,(double)(count-1));
  v->cursorAtStart=cursor;
  v->direction=cursorSpeed<0.0?-1:1;
  v->frameInGrain=0.0;
  v->crossfade=0;
 }
 v->wasStationary=stationary;
 if(stationary){
  if(!v->freezeValid||scratch_abs_d(cursor-v->freezeCursor)>(double)srcRate*.030)
   scratch_spectral_lock(v,src,count,srcRate,hostRate,cursor);
 }else if(v->freezeMix<=0.0)v->freezeValid=0;
 double blendRate=1.0/((double)hostRate*0.011);
 if(blendRate>1.0)blendRate=1.0;
 if(stationary){v->freezeMix+=blendRate;if(v->freezeMix>1.0)v->freezeMix=1.0;}
 else {v->freezeMix-=blendRate;if(v->freezeMix<0.0)v->freezeMix=0.0;}
 float frozen=0.0f;
 if(v->freezeMix>0.0&&v->freezeValid)frozen=scratch_spectral_render(v,pitchRatio);
 if(stationary&&v->freezeMix>=1.0){v->lastOutput=frozen;return frozen;}
 int desiredDir=cursorSpeed< -0.045?-1:(cursorSpeed>0.045?1:v->direction);
 double span=v->grainFrames*v->step;
 int mustSwitch=(desiredDir!=v->direction);
 /* Long-distance seeks become responsive without constantly re-anchoring
    small mouse movements and causing phasey/choppy audio. */
 if(scratch_abs_d(cursor-v->cursorAtStart)>span*1.15&&v->frameInGrain>v->fadeFrames*1.5)mustSwitch=1;
 if(!v->crossfade&&(mustSwitch||v->frameInGrain>=v->grainFrames-v->fadeFrames)){
  int direction=desiredDir;
  double desired=cursor-(double)direction*span*0.42;
  desired=scratch_clamp_d(desired,0.0,(double)(count-1));
  v->incomingPosition=scratch_align_seam(v,src,count,desired,direction,srcRate);
  v->incomingDirection=direction;
  v->crossfade=1;v->frameInGrain=0.0;
 }
 float sample=scratch_read_source(src,count,v->position);
 if(v->crossfade){
  float next=scratch_read_source(src,count,v->incomingPosition);
  double t=scratch_clamp_d(v->frameInGrain/v->fadeFrames,0.0,1.0);
  double blend=t*t*(3.0-2.0*t);
  sample=(float)((double)sample*(1.0-blend)+(double)next*blend);
  v->incomingPosition+=(double)v->incomingDirection*v->step;
 }
 v->position+=(double)v->direction*v->step;
 v->frameInGrain+=1.0;
 if(v->crossfade&&v->frameInGrain>=v->fadeFrames){
  v->position=v->incomingPosition;
  v->direction=v->incomingDirection;
  v->cursorAtStart=cursor;
  v->crossfade=0;
  v->frameInGrain=v->fadeFrames;/* new grain has already rendered its first part */
 }
 if(v->freezeMix>0.0&&v->freezeValid){
  double f=v->freezeMix;
  f=f*f*(3.0-2.0*f);
  sample=(float)((double)sample*(1.0-f)+(double)frozen*f);
 }
 if(!stationary&&v->freezeMix<=0.0)v->freezeValid=0;
 v->lastOutput=sample;
 return sample;
}
#endif
