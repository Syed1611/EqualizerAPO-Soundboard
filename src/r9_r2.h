/* R2 OpenPanel-derived engine. Quality 3 = Original R2, 4 = Enhanced R2.
   Virtual host-rate PCM grid preserves the R2 preparation without allocations. */
#ifndef APO_R9_R2_H
#define APO_R9_R2_H
static float r9_grid_sample(const ApoSpectralFreeze*sp,const float*src,unsigned long long i){
 unsigned long long n=sp->r9ActualFrames; double at=(double)i*((double)sp->r9ActualRate/sp->sampleRate);
 if(at>(double)(n-1))at=(double)(n-1);unsigned long long j=(unsigned long long)at,j0=j?j-1:j,j2=j+1<n?j+1:j,j3=j2+1<n?j2+1:j2;
 float t=(float)(at-j),t2=t*t,t3=t2*t,a=src[j0],b=src[j],c=src[j2],d=src[j3];
 float x=.5f*((2.f*b)+(-a+c)*t+(2.f*a-5.f*b+4.f*c-d)*t2+(-a+3.f*b-3.f*c+d)*t3);return x>8.f?8.f:x< -8.f?-8.f:x;
}
static float r9_source(const ApoSpectralFreeze*sp,const float*src,unsigned long long count,double at,int synthesis){
 if(!src||!count)return 0; if(at<0)at=0;if(at>(double)(count-1))at=(double)(count-1);
 if(sp->quality>=4&&synthesis){double step=(double)sp->r9ActualRate/sp->sampleRate;return r7_pcm(src,sp->r9ActualFrames,at*step,step);}
 if(sp->r9ActualRate==sp->sampleRate)return scratch_read_source(src,count,at);
 unsigned long long i=(unsigned long long)at,i0=i?i-1:i,i2=i+1<count?i+1:i,i3=i2+1<count?i2+1:i2;
 double t=at-i,t2=t*t,t3=t2*t,a=r9_grid_sample(sp,src,i0),b=r9_grid_sample(sp,src,i),c=r9_grid_sample(sp,src,i2),d=r9_grid_sample(sp,src,i3);
 return (float)(.5*((2.*b)+(-a+c)*t+(2.*a-5.*b+4.*c-d)*t2+(-a+3.*b-3.*c+d)*t3));
}
static void r9_r2_stretch_init(ApoScratchStretch*v,double cursor,unsigned sourceRate,unsigned outputRate){
 if(!sourceRate)sourceRate=48000;if(!outputRate)outputRate=48000;
 v->initialized=1;v->freezeValid=0;v->wasStationary=1;
 v->freezeCursor=cursor;v->lastAnalysisCursor=cursor;
 v->lastAnalysisFrame=0;
 ApoSpectralFreeze*sp=&v->spec;
 sp->sampleRate=outputRate;sp->sourceRate=sourceRate;
 sp->outputFrames=0;sp->generatedFrames=0;sp->readPosition=0.;sp->phaseValid=0;sp->randomState=0x6a339c17u;
 sp->currentPitch=1.;sp->initialized=1;sp->transientCount=0;sp->reversalFrames=0;sp->reversalSign=0;
 sp->priorAnalysisCursor=cursor;sp->observedMotion=0;sp->steadyMotion=0;sp->directMix=0.;
 sp->wsMix=0.;sp->wsConfidence=.45;sp->wsLastShift=0.;sp->wsLastScore=0.;
 sp->analysisCursor=cursor;sp->trackingMotion=0.;sp->analysisCursorValid=0;
 sp->dryReadPos=cursor-(double)APO_SPECTRAL_FFT*.5;sp->directRate=1.;
 for(unsigned i=0;i<APO_SPECTRAL_RING;i++){sp->ring[i]=0;sp->rendered[i]=0;
  sp->wsRing[i]=0;sp->wsRendered[i]=0;sp->r14Weight[i]=0;}
 sp->r14Transient=0;
 for(unsigned k=0;k<=APO_SPECTRAL_HALF;k++){
  sp->synthPhase[k]=0;sp->srcPhase[k]=0;
  sp->olderMag[k]=0;sp->frequency[k]=APO_SPECTRAL_TWO_PI*(double)k/(double)APO_SPECTRAL_FFT;
  sp->freqSmooth[k]=sp->frequency[k];
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
static void r9_r2_analyze(ApoSpectralFreeze*sp,const float*src,unsigned long long count,double cursor){
 double center=cursor-(double)APO_SPECTRAL_FFT*.5;
 int local=sp->quality>=6&&scratch_abs_d(sp->steadyMotion)<.035;
 int maxHold=sp->quality>=7&&local;unsigned phaseHop=maxHold?64u:APO_SPECTRAL_HOP;
 for(unsigned pass=0;pass<2;pass++){
  double base=center+(double)(pass*phaseHop);
  for(unsigned j=0;j<APO_SPECTRAL_FFT;j++){
   double at=base+(double)j;
   double v=at<0||at>(double)(count-1)?0.0:(double)r9_source(sp,src,count,at,1);
   double win=sp->fftWindow[j];if(maxHold){win*=win;win*=win*1.523809523809524;}else if(local)win*=win*1.333333333333333;sp->re[j]=v*win;sp->im[j]=0.0;
  }
  if(sp->quality>=4)r7_fft(sp->re,sp->im);else scratch_fft(sp->re,sp->im);
  if(sp->quality==8&&!local&&sp->r14Transient){
   /* Short analysis (effective 1024 sample support, zero padded to the
      existing FFT grid) contributes only to upper bands on transients.
      The long window retains pitch/bass resolution and phase reference. */
   for(unsigned k=0;k<=APO_SPECTRAL_HALF;k++){sp->r14LongRe[k]=sp->re[k];sp->r14LongIm[k]=sp->im[k];}
   for(unsigned j=0;j<APO_SPECTRAL_FFT;j++){double at=base+j;double win=0.;if(j>=512&&j<1536)win=2.*sp->fftWindow[(j-512)*2];sp->re[j]=(at<0||at>(double)(count-1)?0.:r9_source(sp,src,count,at,1))*win;sp->im[j]=0.;}
   r7_fft(sp->re,sp->im);
   for(unsigned k=0;k<=APO_SPECTRAL_HALF;k++){double hz=(double)k*sp->sampleRate/APO_SPECTRAL_FFT;double mix=.65*scratch_clamp_d((hz-1600.)/2200.,0.,1.);sp->re[k]=sp->r14LongRe[k]*(1.-mix)+sp->re[k]*mix;sp->im[k]=sp->r14LongIm[k]*(1.-mix)+sp->im[k]*mix;}
  }
  if(pass==0){
   for(unsigned j=0;j<=APO_SPECTRAL_HALF;j++){
    sp->analysisRe[j]=sp->re[j];sp->analysisIm[j]=sp->im[j];
    double r=sp->re[j],im=sp->im[j];
    sp->mag[j]=(sp->quality>=4?r7_sqrt(r*r+im*im):scratch_sqrt(r*r+im*im));
    sp->phase[j]=(sp->quality>=4?scratch_atan2_hq(im,r):scratch_atan2(im,r));
   }
  }
 }
 for(unsigned k=0;k<=APO_SPECTRAL_HALF;k++){
  double r=sp->re[k],im=sp->im[k];
  double after=(sp->quality>=4?scratch_atan2_hq(im,r):scratch_atan2(im,r));
  double centerOmega=APO_SPECTRAL_TWO_PI*(double)k/(double)APO_SPECTRAL_FFT;
  double delta=scratch_wrap_angle(after-sp->phase[k]-centerOmega*(double)phaseHop);
  double omega=centerOmega+delta/(double)phaseHop;
  double measured=scratch_clamp_d(omega*(double)sp->sourceRate/(double)sp->sampleRate,0.,APO_SPECTRAL_PI);
  /* Harmonic tracks are noisy in low-energy bins. Smooth only when the
     bin is weak; loud speech transients and sustained tonal peaks stay
     responsive to the source. */
  double strength=sp->mag[k];
  double weight=strength<.00008? .16 : .57;
  double filt=sp->freqSmooth[k]+(measured-sp->freqSmooth[k])*weight;
  sp->freqSmooth[k]=filt;
  sp->frequency[k]=filt;
 }
}
/* Synthesize all 1025 bins instead of selected oscillator peaks. Identity
   phase locking at local magnitude maxima keeps nearby bins together and
   reduces the reverberant/underwater phase-vocoder sound. */
static void r9_r2_synthesis_frame(ApoScratchStretch*v,const float*src,unsigned long long count,
                                    double cursor){
 ApoSpectralFreeze*sp=&v->spec;
 r9_r2_analyze(sp,src,count,cursor);
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
 if(onset)sp->transientCount++;if(sp->quality==8)sp->r14Transient=onset;
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
  unsigned lock=nearest;
  if(sp->quality==8){for(unsigned j=k+1;j<half-1&&j<=k+3;j++)if(sp->mag[j]>=sp->mag[j-1]&&sp->mag[j]>=sp->mag[j+1]){if(lock==0||k-lock>j-k)lock=j;break;}}
  if(lock>0&&(lock>k?lock-k:k-lock)<=3&&sp->mag[lock]>strength*1.5&&sp->directMix<0.9){
   phase=sp->synthPhase[lock]+scratch_wrap_angle(sp->phase[k]-sp->phase[lock]);
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
 if(sp->quality>=4){for(unsigned j=0;j<n;j++)sp->im[j]=-sp->im[j];r7_fft(sp->re,sp->im);for(unsigned j=0;j<n;j++){sp->re[j]/=n;sp->im[j]/=-(double)n;}}else scratch_ifft(sp->re,sp->im);
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
static void r9_r2_ws_synthesis_frame(ApoSpectralFreeze*sp,const float*src,
                                       unsigned long long count,double cursor,
                                       unsigned long long t){
 const unsigned mask=APO_SPECTRAL_RING-1u;
 double step=(double)sp->sourceRate/(double)sp->sampleRate;
 unsigned grain=sp->quality==8&&sp->r14Transient?512u:APO_WS_GRAIN;
 unsigned long long start=t+(APO_SPECTRAL_FFT-grain)/2u;
 double target=cursor-(double)(grain/2u)*step;
 double best=-3.,bestShift=0.;
 if(t>=APO_SPECTRAL_HOP){
  /* 2-sample search gives much less quantization jitter than the previous
     4-sample grid. Use multiple overlap positions for a voiced match. */
  for(int shift=-128;shift<=128;shift+=(sp->quality>=4?1:2)){
   double dot=0.,old2=1.e-10,new2=1.e-10;
   double offset=(double)shift*step;
   for(unsigned j=24;j<grain/2u-12u;j+=7u){
    float prior=sp->wsRing[(unsigned)((start+j)&mask)];if(sp->quality==8){float w=sp->r14Weight[(unsigned)((start+j)&mask)];if(w>.025f)prior/=w;else continue;}
    float cand=r9_source(sp,src,count,target+offset+(double)j*step,0);
    dot+=(double)prior*cand;old2+=(double)prior*prior;new2+=(double)cand*cand;
   }
   double score=dot/(sp->quality>=4?r7_sqrt(old2*new2):scratch_sqrt(old2*new2));
   /* Avoid switching between harmonic periods on every 256-frame hop. */
   score-=.035*scratch_abs_d((double)shift-sp->wsLastShift)/128.;
   score-=.012*scratch_abs_d((double)shift)/128.;
   if(score>best){best=score;bestShift=(double)shift;}
  }
  if(sp->quality>=5&&(sp->quality!=8||(best>.45&&best<.85))){double center=bestShift,scoreBest=-1e30;for(int k=-4;k<=4;k++){double shift=center+k*.25;if(shift<-128||shift>128)continue;double dot=0,old2=1e-10,new2=1e-10;for(unsigned j=24;j<grain/2u-12u;j+=3u){float prior=sp->wsRing[(unsigned)((start+j)&mask)];if(sp->quality==8){float w=sp->r14Weight[(unsigned)((start+j)&mask)];if(w>.025f)prior/=w;else continue;}float cand=r9_source(sp,src,count,target+(shift+j)*step,1);dot+=(double)prior*cand;old2+=(double)prior*prior;new2+=(double)cand*cand;}double score=dot/r7_sqrt(old2*new2)-.035*scratch_abs_d(shift-sp->wsLastShift)/128.-.012*scratch_abs_d(shift)/128.;if(score>scoreBest){scoreBest=score;bestShift=shift;}}best=scoreBest;}
  /* Large unexpected jumps imply low similarity and should use spectral
     processing instead of stitching two unrelated speech segments. */
  double jump=scratch_abs_d(bestShift-sp->wsLastShift);
  double confidence=scratch_clamp_d((best+.08)*1.2,0.,1.);
  if(jump>90.&&best<.73)confidence*=.65;
  sp->wsConfidence=sp->wsConfidence*.76+confidence*.24;
 }else sp->wsConfidence=.35;
 sp->wsLastShift=bestShift;sp->wsLastScore=best;
 double base=target+bestShift*step;
 for(unsigned j=0;j<grain;j++){
  unsigned ix=(unsigned)((start+j)&mask);float window=sp->wsWindow[j*(APO_WS_GRAIN/grain)];
  sp->wsRing[ix]+=r9_source(sp,src,count,base+(double)j*step,1)*window;
  if(sp->quality==8)sp->r14Weight[ix]+=window;
 }
}

static float r9_r2_stretch_render(ApoScratchStretch*v,const float*src,unsigned long long count,
                                    unsigned sr,unsigned hostRate,double cursor,double cursorSpeed,double pitchRatio){
 if(!src||!count)return 0.0f;
 if(!sr)sr=48000;if(!hostRate)hostRate=48000;
 if(!v->initialized)r9_r2_stretch_init(v,cursor,sr,hostRate);
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
 unsigned long long needed=first+(sp->quality>=4?24ull:2ull);
 while(sp->generatedFrames<=needed){
  unsigned long long t=sp->generatedFrames;
  if((t&(APO_SPECTRAL_HOP-1u))==0){
   double motion=(cursor-sp->priorAnalysisCursor)/((double)APO_SPECTRAL_HOP*sourceStep);
   if(!sp->phaseValid)motion=0.;
   motion=scratch_clamp_d(motion,-7.,7.);
   sp->observedMotion=sp->observedMotion*.78+motion*.22;
   double vMouse=scratch_clamp_d(cursorSpeed,-7.,7.);
   sp->steadyMotion=(scratch_abs_d(vMouse)>(sp->quality==8?.004:.10))?(.38*sp->observedMotion+.62*vMouse):sp->observedMotion;
   if(sp->quality>=6&&scratch_abs_d(vMouse)<.001&&scratch_abs_d(motion)<.001){sp->observedMotion=0.;sp->steadyMotion=0.;}
   sp->priorAnalysisCursor=cursor;
   /* GUI pointer coordinates update less frequently than 256-frame
      analysis hops. Predict intermediate source positions instead of
      re-analyzing the exact same frame twice, which sounded like flutter
      during long slow stretches. Re-anchor gently to actual cursor data. */
   if(!sp->analysisCursorValid){sp->analysisCursor=cursor;sp->analysisCursorValid=1;}
   double vel=sp->steadyMotion;
   double maximum=(double)APO_SPECTRAL_HOP*sourceStep;
   if(scratch_abs_d(vel)<.035){
    if(sp->quality>=6)sp->analysisCursor=cursor;else sp->analysisCursor+=(cursor-sp->analysisCursor)*.72;
    if(scratch_abs_d(cursor-sp->analysisCursor)<.5)sp->analysisCursor=cursor;
   }else{
    double predicted=sp->analysisCursor+vel*maximum;
    double error=cursor-predicted;
    /* Don't follow stale UI coordinates backward on every audio hop. */
    double correction=scratch_clamp_d(error,-maximum*.20,maximum*.20);
    sp->analysisCursor=predicted+correction;
    /* R13: a queued audio block can run many hops before another UI event.
       Bound extrapolation to at most two hops so the engine cannot wander
       into neighboring speech while the pointer coordinate is stale. */
    if(sp->quality>=7){double lead=maximum*(sp->quality==8?3.:2.);sp->analysisCursor=scratch_clamp_d(sp->analysisCursor,cursor-lead,cursor+lead);}
   }
   sp->analysisCursor=scratch_clamp_d(sp->analysisCursor,0.,(double)(count-1));
   r9_r2_synthesis_frame(v,src,count,sp->analysisCursor);
   r9_r2_ws_synthesis_frame(sp,src,count,sp->analysisCursor,t);
  }
  unsigned slot=(unsigned)(t&(APO_SPECTRAL_RING-1u));
  sp->rendered[slot]=sp->ring[slot];sp->ring[slot]=0.0f;
  sp->wsRendered[slot]=sp->quality==8?(sp->r14Weight[slot]>.05f?sp->wsRing[slot]/sp->r14Weight[slot]:0.f):sp->wsRing[slot]*.5f;sp->wsRing[slot]=0.0f;sp->r14Weight[slot]=0.f;
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
 if(sp->quality>=4){processed=r7_ring_pcm(sp->rendered,first,frac,sp->currentPitch);ws=r7_ring_pcm(sp->wsRendered,first,frac,sp->currentPitch);}
 sp->readPosition+=sp->currentPitch;
 unsigned long long t=sp->outputFrames;
 /* Lossless decoded PCM on sustained approximately 1x forward/reverse
    movement, only when the user Pitch setting is at neutral. Never use
    source resampling to implement slow motion: that changes pitch. */
 double rate=sp->steadyMotion;
 double speed=scratch_abs_d(rate);
 double wsTarget=0.;
 if(speed>(sp->quality==8?.065:.12)&&speed<2.25&&sp->outputFrames>=APO_SPECTRAL_FFT){
  /* Avoid permanent FFT+WSOLA comb filtering: use waveform-domain output
     entirely when speech matching is coherent. Spectral synthesis takes
     over for poor matches, extreme speeds and stationary holding. */
  if(sp->wsConfidence>(sp->quality==8?.72:.60))wsTarget=1.;
  else if(sp->wsConfidence<(sp->quality==8?.48:.42))wsTarget=0.;
  else wsTarget=sp->wsMix>.5?1.:0.;
 }
 double wsStep=1./((double)hostRate*(sp->quality>=7&&speed<.035?.012:sp->quality==8?.020:sp->quality>=4?.030:.055));
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
 float raw=r9_source(sp,src,count,sp->dryReadPos,1);
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

#include "r15_scrub.h"
static float r9_render(ApoScratchStretch*v,const float*src,unsigned long long count,unsigned sr,unsigned host,double cursor,double speed,double pitch,unsigned quality){
 if(quality==9)return r15_render_pair(v,src,0,count,sr,host,cursor,speed,pitch,0);
 if(!src||!count)return 0;if(!sr)sr=48000;if(!host)host=48000;
 unsigned long long n=(count*(unsigned long long)host+sr/2)/sr;if(!n)n=1;
 double at=sr==host?cursor:cursor*(double)n/(double)count;
 if(!v->initialized||!v->spec.r9Ready||v->spec.sampleRate!=host||v->spec.r9ActualRate!=sr||v->spec.r9ActualFrames!=count||v->spec.quality!=quality){r9_r2_stretch_init(v,at,host,host);v->spec.quality=quality;v->spec.r9Ready=1;v->spec.r9ActualRate=sr;v->spec.r9ActualFrames=count;}
 return r9_r2_stretch_render(v,src,n,host,host,at,speed,pitch);
}
/* R2 dry entry/release reads the same virtual host grid as its scrub. */
static float r9_legacy_dry(const ApoScratchStretch*v,const float*src,unsigned long long count,double pos){
 const ApoSpectralFreeze*sp=&v->spec;
 if(!sp->r9Ready||!count||!sp->r9ActualRate||!sp->sampleRate)return scratch_read_source(src,count,pos);
 unsigned long long n=(count*(unsigned long long)sp->sampleRate+sp->r9ActualRate/2)/sp->r9ActualRate;if(!n)n=1;
 double at=sp->r9ActualRate==sp->sampleRate?pos:pos*(double)n/count;
 return r9_source(sp,src,n,at,0);
}
#endif
