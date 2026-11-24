/* R15 waveform-coherent scrub: a single stereo-aware, arbitrary-position
   waveform overlap/add stream. No spectral reconstruction or mixed engines.
   The analysis search is bounded about the pointer, and pitch is set only
   by the local read increment. Historical engines remain for regression tests. */
#ifndef APO_R15_SCRUB_H
#define APO_R15_SCRUB_H
static float r15_read(const float*x,unsigned long long n,double p,double step,int hq){
 if(!x||!n||p<0.||p>(double)(n-1))return 0.f;
 return hq?r7_pcm(x,n,p,scratch_abs_d(step)):scratch_read_source(x,n,p);
}
#define R15_MATCH 64
static double r15_score(const float*x,const float*y,unsigned long long n,double start,double step,const float*a,const float*b,double stride,double ref2){
 double dot=0.,e=0.;for(unsigned j=0;j<R15_MATCH;j++){
  double pos=start+(double)j*stride*step;
  double u=r15_read(x,n,pos,step,0),v=r15_read(y,n,pos,step,0);
  dot+=a[j]*u+b[j]*v;e+=u*u+v*v;
 }
 if(ref2<1e-10||e<1e-10)return -1.;
 /* Normalized waveform similarity with modest level consistency penalty. */
 return dot/r7_sqrt(ref2*e)-.10*scratch_abs_d(e-ref2)/(e+ref2);
}
static double r15_align(const float*x,const float*y,unsigned long long n,double old,double oldStep,double wanted,double step,unsigned hop,unsigned sr){
 float a[R15_MATCH],b[R15_MATCH];double ref2=0.,stride=(double)hop/R15_MATCH;
 for(unsigned j=0;j<R15_MATCH;j++){double pos=old+(double)j*stride*oldStep;a[j]=r15_read(x,n,pos,oldStep,0);b[j]=r15_read(y,n,pos,oldStep,0);ref2+=(double)a[j]*a[j]+(double)b[j]*b[j];}
 if(ref2<1e-10)return wanted;
 /* Prefer direct continuation when already coherent; exact 1x transport
    is a single PCM path. Search +/- 14 ms to cover low voice periods. */
 double direct=r15_score(x,y,n,wanted,step,a,b,stride,ref2);
 if(direct>.996)return wanted;
 int radius=(int)(sr*.014),coarse=(int)(sr/12000);if(coarse<1)coarse=1;if(radius<coarse)radius=coarse;
 double best=direct,offset=0.;
 for(int k=-radius;k<=radius;k+=coarse){double z=(double)k/radius;double score=r15_score(x,y,n,wanted+k,step,a,b,stride,ref2)-.035*z*z;if(score>best){best=score;offset=k;}}
 double coarseBest=offset;
 for(double k=coarseBest-coarse;k<=coarseBest+coarse;k+=.5){if(k < -radius||k>radius)continue;double z=k/radius;double score=r15_score(x,y,n,wanted+k,step,a,b,stride,ref2)-.035*z*z;if(score>best){best=score;offset=k;}}
 /* Avoid phase-searching unrelated transients/noise into a distant syllable. */
 return best>.35?wanted+offset:wanted;
}
static float r15_render_pair(ApoScratchStretch*v,const float*x,const float*y,unsigned long long n,unsigned sr,unsigned host,double cursor,double speed,double pitch,float*side){
 if(side)*side=0;if(!x||!n)return 0;if(!sr)sr=48000;if(!host)host=48000;
 cursor=scratch_clamp_d(cursor,0.,(double)(n-1));pitch=scratch_clamp_d(pitch,.5,2.);
 R15Wave*w=&v->wave;double base=(double)sr/host;
 if(!v->initialized||!w->ready||w->sr!=sr||w->host!=host||w->frames!=n){
  w->ready=1;w->sr=sr;w->host=host;w->frames=n;w->hop=host/80;if(w->hop<8)w->hop=8;
  w->phase=0;w->pitch=pitch;w->direction=speed<-.8?-1:1;w->stepA=w->stepB=base*pitch*w->direction;
  w->a=w->b=cursor;w->level=1.;v->initialized=1;v->spec.quality=9;v->spec.r9Ready=1;v->spec.sampleRate=host;v->spec.outputFrames=0;
 }
 w->pitch+=(pitch-w->pitch)/(host*.012+1.);
 if(w->phase>=w->hop){
  w->phase=0;w->a=w->b;w->stepA=w->stepB;
  /* Slow backwards motion traverses the sequence of local forward grains.
     Fast reverse uses reversed PCM; the overlap handles direction changes. */
  if(speed<-.8)w->direction=-1;else if(speed>-.55)w->direction=1;
  w->stepB=base*w->pitch*w->direction;
  double advance=scratch_clamp_d(speed*base*w->hop,-(double)sr*.015,(double)sr*.015);
  double wanted=cursor+advance-w->stepB*w->hop;
  if(scratch_abs_d(speed)<.04)wanted=cursor-w->stepB*w->hop;
  wanted=scratch_clamp_d(wanted,0.,(double)(n-1));
  w->b=r15_align(x,y,n,w->a,w->stepA,wanted,w->stepB,w->hop,sr);
  /* Prevent extrapolation past the source; candidate starts stay local. */
  w->b=scratch_clamp_d(w->b,0.,(double)(n-1));
 }
 /* Complementary raised-cosine-like cubic weights: unity for identical
    grains, zero-slope boundaries; shared stereo timing, no phase vocoder. */
 double t=(double)w->phase/w->hop,m=t*t*(3.-2.*t);
 float u=r15_read(x,n,w->a,w->stepA,1),z=r15_read(x,n,w->b,w->stepB,1);
 float out=(float)(u+(z-u)*m);
 if(side&&y){float a=r15_read(y,n,w->a,w->stepA,1),b=r15_read(y,n,w->b,w->stepB,1);*side=(float)(a+(b-a)*m);}
 w->a+=w->stepA;w->b+=w->stepB;w->phase++;v->spec.outputFrames++;v->spec.analysisCursor=cursor;
 return out;
}
#endif
