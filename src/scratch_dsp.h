#ifndef APO_SCRATCH_DSP_H
#define APO_SCRATCH_DSP_H
/* Scratch Edition pitch-preserving overlap-add scrub.
   Input grains ALWAYS play at 1.0x source sample speed (signed for reverse).
   The cursor controls where grains originate, not their within-grain pitch.
   Optional waveform-similarity alignment reduces comb filtering at seams.
   Header-only, freestanding C, deterministic and allocation-free per sample.
*/
typedef struct {
 double phase[2];        /* 0..1 through each fixed-duration output grain */
 double origin[2];       /* input sample position at grain start */
 int direction[2];       /* signed, locked for each grain */
 int lastDirection;
 double grainHostFrames;
 double inputPerHost;
 double lastInputCursor;
 float lastOutput;
 int initialized;
} ApoScratchStretch;

static double scratch_abs_d(double x){return x<0?-x:x;}
static double scratch_clamp_d(double x,double low,double high){return x<low?low:x>high?high:x;}
/* Weight is smooth at the boundaries. We normalize the sum explicitly.
   This polynomial raised-cosine substitute needs no C math runtime. */
static double scratch_win(double t){if(t<=0.0||t>=1.0)return 0.0;double v=t*(1.0-t);return v*v;}
static float scratch_read_source(const float*s,unsigned long long count,double at){
 if(!s||!count)return 0;
 if(at<0)at=0;if(at>(double)(count-1))at=(double)(count-1);
 unsigned long long i=(unsigned long long)at;
 unsigned long long i0=i?i-1:i,i2=i+1<count?i+1:i,i3=i2+1<count?i2+1:i2;
 double t=at-(double)i,tt=t*t,ttt=tt*t;
 double a=s[i0],b=s[i],c=s[i2],d=s[i3];
 return (float)(0.5*((2*b)+(-a+c)*t+(2*a-5*b+4*c-d)*tt+(-a+3*b-3*c+d)*ttt));
}
static void scratch_stretch_init(ApoScratchStretch*v,double cursor,unsigned srcRate,unsigned outputRate){
 if(!srcRate)srcRate=48000;if(!outputRate)outputRate=48000;
 v->grainHostFrames=(double)outputRate*0.096; /* 96 ms, clearer voice freeze */
 if(v->grainHostFrames<192)v->grainHostFrames=192;
 v->inputPerHost=(double)srcRate/(double)outputRate;
 v->phase[0]=0.0;v->phase[1]=0.5;
 v->direction[0]=v->direction[1]=v->lastDirection=1;
 v->origin[0]=cursor-v->grainHostFrames*v->inputPerHost*0.5;
 v->origin[1]=v->origin[0]-v->grainHostFrames*v->inputPerHost*0.5;
 v->lastInputCursor=cursor;v->lastOutput=0;
 v->initialized=1;
}
/* Optional bounded correlation search for a smooth junction.
   At a 48k input, a ±7ms search costs < 10k samples per hop (~48ms).
   Correlation uses native source samples and never resamples by cursor velocity. */
static double scratch_align_grain(const ApoScratchStretch*v,int oldGrain,int newDirection,
        double target,const float*src,unsigned long long count,unsigned sr){
 if(count<16||!src||!sr)return target;
 double inputGrain=v->grainHostFrames*v->inputPerHost;
 double previous=v->origin[oldGrain]+(double)v->direction[oldGrain]*v->phase[oldGrain]*inputGrain;
 /* Large instantaneous seek: prioritize following the mouse. */
 if(scratch_abs_d(previous-target)>inputGrain*1.8)return target;
 int radius=(int)((double)sr*0.007);
 if(radius>700)radius=700;if(radius<2)radius=2;
 int stride=radius/28;if(stride<1)stride=1;
 double best=target,scoreBest=-10000000.0;
 double half=inputGrain*0.5,measureStep=half/60.0;
 if(measureStep<0.5)measureStep=0.5;
 for(int shift=-radius;shift<=radius;shift+=stride){
  double anchor=target+(double)shift;
  /* Prioritize mouse accuracy when similarity is approximately tied. */
  double cross=0,powA=0,powB=0;
  for(int j=0;j<60;j++){
   double hop=(double)j*measureStep;
   float a=scratch_read_source(src,count,previous+(double)v->direction[oldGrain]*hop);
   float b=scratch_read_source(src,count,anchor+(double)newDirection*hop);
   cross+=(double)a*(double)b;powA+=(double)a*a;powB+=(double)b*b;
  }
  double norm=(powA+powB)>1e-7?(2.0*cross/(powA+powB)):-0.5;
  double score=norm-0.02*scratch_abs_d((double)shift)/(double)radius;
  if(score>scoreBest){scoreBest=score;best=anchor;}
 }
 return scoreBest>0.08?best:target;
}
/* rate: cursor speed in x1 units (e.g. 0.5 or -2.0), 0 on hold.
   cursor: user's absolute cursor position in SOURCE sample frames.
   This processor never changes within-grain playback rate, preserving pitch.
*/
static float scratch_stretch_render(ApoScratchStretch*v,const float*src,unsigned long long count,
                 unsigned srcRate,unsigned outputRate,double cursor,double rate){
 if(!src||!count)return 0.0f;
 if(!v->initialized||v->grainHostFrames<1.0)scratch_stretch_init(v,cursor,srcRate,outputRate);
 if(rate>0.05)v->lastDirection=1;else if(rate< -0.05)v->lastDirection=-1;
 /* Start of the next slice is anchored near cursor; each slice plays at the
    original sample rate so faster/slower pointer movement does NOT detune it. */
 double inputGrain=v->grainHostFrames*v->inputPerHost;
 double sum=0,wsum=0;
 for(int k=0;k<2;k++){
  double p=v->phase[k];double weight=scratch_win(p);
  double inputPos=v->origin[k]+(double)v->direction[k]*p*inputGrain;
  sum+=(double)scratch_read_source(src,count,inputPos)*weight;
  wsum+=weight;
 }
 float sample=wsum>1e-9?(float)(sum/wsum):v->lastOutput;
 v->lastOutput=sample;
 double incr=1.0/v->grainHostFrames;
 v->phase[0]+=incr;v->phase[1]+=incr;
 for(int k=0;k<2;k++)if(v->phase[k]>=1.0){
  v->phase[k]-=1.0;
  int direction=v->lastDirection;
  double target=cursor-(double)direction*inputGrain*0.5;
  int other=1-k;
  double matched=scratch_align_grain(v,other,direction,target,src,count,srcRate);
  v->origin[k]=scratch_clamp_d(matched,-inputGrain*0.9,(double)count+inputGrain*0.9);
  v->direction[k]=direction;
 }
 v->lastInputCursor=cursor;
 return sample;
}
#endif
