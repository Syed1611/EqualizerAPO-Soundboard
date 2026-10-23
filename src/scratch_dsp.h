#ifndef APO_SCRATCH_DSP_H
#define APO_SCRATCH_DSP_H
/* Scratch Edition v0.9.4: pitch-aware movement plus a stable locked freeze loop.
   Speed/direction are controlled by the waveform cursor (which decides where
   the next source segment begins); pitch is set independently by the per-pad
   live Pitch/Auto Sweep slider (which sets the source read increment).
   Most samples are read ONCE without overlaying dissimilar voices. Only the
   short overlap at each slice transition is crossfaded. Freestanding, no
   allocations and no CRT/math library required in the VST/controller.
*/
typedef struct {
 double position, incomingPosition, cursorAtStart;
 double step, frameInGrain;
 double grainFrames, fadeFrames;
 float lastOutput;
 /* Locked freeze: a fixed, waveform-matched forward-reading source loop.
    Stationary playback does not reseek or run waveform matching each cycle. */
 double freezeStart,freezeLength,freezeFade,freezePhase,freezeCursor,freezeMix;
 int freezeValid,wasStationary;
 int direction, incomingDirection, crossfade, initialized;
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
 v->crossfade=0;v->lastOutput=0.0f;v->freezeStart=0;v->freezeLength=0;v->freezeFade=0;v->freezePhase=0;v->freezeCursor=cursor;v->freezeMix=0;v->freezeValid=0;v->wasStationary=0;v->initialized=1;
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
/* Select one stable loop by matching the waveform at its start and end.
   It is deliberately LOCKED while stationary: the old v0.9.3 processor
   repeatedly re-searched every ~78ms, sounding like a backward-forward echo.
   All loop points are in SOURCE frames; output pitch is still controlled only
   by the per-pad manual pitch/Auto Sweep source step. */
static void scratch_freeze_lock(ApoScratchStretch*v,const float*src,unsigned long long count,
                                unsigned srcRate,double cursor){
 if(!count)return;
 double max=(double)(count-1);
 double start=cursor-(double)srcRate*0.073;
 if(start<0.0)start=0.0;
 if(start>max)start=max;
 /* Around 72ms of forward source content. Never repeatedly bounce across
    the cursor. Tight seams are selected by a small waveform similarity scan. */
 int minL=(int)((double)srcRate*0.045),maxL=(int)((double)srcRate*0.092);
 if(minL<24)minL=24;
 if(maxL<minL+1)maxL=minL+1;
 if(start+(double)maxL+128.0>max){
  start=max-(double)maxL-128.0;
  if(start<0.0)start=0.0;
 }
 if(start+(double)maxL+128.0>max)maxL=(int)(max-start-128.0);
 if(maxL<minL){minL=maxL;if(minL<8)minL=8;}
 if(maxL<minL){maxL=minL;start=0.0;}
 const int points=48;
 double firstEnergy=0;
 float a[48];
 for(int k=0;k<points;k++){
  double at=start+(double)k*4.0;
  a[k]=scratch_read_source(src,count,at);firstEnergy+=(double)a[k]*a[k];
 }
 double bestScore=-1e20;
 int bestL=minL;
 int stride=4;
 for(int l=minL;l<=maxL;l+=stride){
  double dot=0,energy=0;
  for(int j=0;j<points;j++){
   double t=(double)scratch_read_source(src,count,start+(double)l+(double)j*4.0);
   dot+=(double)a[j]*t;energy+=t*t;
  }
  /* Favor phase continuity, not a loop that repeatedly phase-hunts. */
  double score=2.0*dot/(firstEnergy+energy+1e-7);
  score-=0.006*scratch_abs_d((double)l-(double)srcRate*.073)/((double)srcRate*.073+1.0);
  if(score>bestScore){bestScore=score;bestL=l;}
 }
 /* Refine to single-sample precision around the winner. */
 for(int l=bestL-stride;l<=bestL+stride;l++){
  if(l<minL||l>maxL)continue;
  double dot=0,energy=0;
  for(int j=0;j<points;j++){
   double t=(double)scratch_read_source(src,count,start+(double)l+(double)j*4.0);
   dot+=(double)a[j]*t;energy+=t*t;
  }
  double score=2.0*dot/(firstEnergy+energy+1e-7);
  score-=0.006*scratch_abs_d((double)l-(double)srcRate*.073)/((double)srcRate*.073+1.0);
  if(score>bestScore){bestScore=score;bestL=l;}
 }
 v->freezeStart=start;
 /* bestL is the phase-matched repeat interval. The extra fade frames
    are outside the repeated interval; otherwise each wrap changes pitch. */
 v->freezeFade=(double)srcRate*(bestScore>0.85?0.0025:0.0050);
 v->freezeLength=(double)bestL+v->freezeFade;
 if(v->freezeFade<6.0)v->freezeFade=6.0;
 if(v->freezeFade>v->freezeLength*.24)v->freezeFade=v->freezeLength*.24;
 double ahead=cursor-start;
 if(ahead<0.0)ahead=0.0;
 if(ahead>=v->freezeLength)ahead=v->freezeLength-v->freezeFade;
 v->freezePhase=ahead;
 v->freezeCursor=cursor;
 v->freezeValid=1;
}
static float scratch_freeze_render(ApoScratchStretch*v,const float*src,unsigned long long count){
 if(!v->freezeValid||v->freezeLength<8.0)return 0.0f;
 double phase=v->freezePhase;
 double from=v->freezeStart+phase;
 float out=scratch_read_source(src,count,from);
 const double blendStart=v->freezeLength-v->freezeFade;
 if(phase>=blendStart){
  double t=scratch_clamp_d((phase-blendStart)/v->freezeFade,0.0,1.0);
  double blend=t*t*(3.0-2.0*t);
  float next=scratch_read_source(src,count,v->freezeStart+(phase-blendStart));
  out=(float)((double)out*(1.0-blend)+(double)next*blend);
 }
 phase+=v->step;
 while(phase>=v->freezeLength)phase-=v->freezeLength-v->freezeFade;
 v->freezePhase=phase;
 return out;
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
   scratch_freeze_lock(v,src,count,srcRate,cursor);
 }else if(v->freezeMix<=0.0)v->freezeValid=0;
 double blendRate=1.0/((double)hostRate*0.011);
 if(blendRate>1.0)blendRate=1.0;
 if(stationary){v->freezeMix+=blendRate;if(v->freezeMix>1.0)v->freezeMix=1.0;}
 else {v->freezeMix-=blendRate;if(v->freezeMix<0.0)v->freezeMix=0.0;}
 float frozen=0.0f;
 if(v->freezeMix>0.0&&v->freezeValid)frozen=scratch_freeze_render(v,src,count);
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
