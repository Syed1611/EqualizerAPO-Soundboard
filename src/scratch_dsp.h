#ifndef APO_SCRATCH_DSP_H
#define APO_SCRATCH_DSP_H
/* Scratch Edition v0.9.3: single-grain waveform-synchronized scrubbing.
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
 v->crossfade=0;v->lastOutput=0.0f;v->initialized=1;
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
 v->lastOutput=sample;
 return sample;
}
#endif
