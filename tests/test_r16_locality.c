#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../src/controller.c"
#undef APO_R15_SCRUB_H
#undef R15_MATCH
#define r15_read r15_original_read
#define r15_score r15_original_score
#define r15_align r15_original_align
#define r15_render_pair r15_original_render_pair
#include "reference/r15_scrub_original.h"
#undef r15_read
#undef r15_score
#undef r15_align
#undef r15_render_pair
#undef APO_R15_SCRUB_H
#undef R15_MATCH
#define r15_read r17_reference_read
#define r15_score r17_reference_score
#define r15_align r17_reference_align
#define r15_render_pair r17_reference_render_pair
#include "reference/r17_wave.h"
#undef r15_read
#undef r15_score
#undef r15_align
#undef r15_render_pair
#define r15_render_pair r17_reference_render_pair
static float x[96000],y[96000];
static const double pi=3.141592653589793;
static void ck(int c,const char*s){if(!c){fprintf(stderr,"FAIL %s\n",s);exit(1);}puts(s);}
typedef struct {double dist,outer,amp,rms;} Result;
static Result hold(int orig,const float *src,const float *side,int n,int sr,double cursor,double freq,double pitch){
 ApoScratchStretch*v=calloc(1,sizeof(*v));Result r={0};double a=0,b=0,e=0;int count=12000;
 for(int i=0;i<24000;i++){float st=0;float q=orig?r15_original_render_pair(v,src,side,n,sr,48000,cursor,0,pitch,&st):r15_render_pair(v,src,side,n,sr,48000,cursor,0,pitch,&st);if(!isfinite(q)||!isfinite(st))exit(2);if(i<12000)continue;R15Wave*w=&v->wave;double t=(double)(w->phase-1)/w->hop,m=t*t*(3-2*t),da=fabs(w->a-w->stepA-cursor)*1000/sr,db=fabs(w->b-w->stepB-cursor)*1000/sr;r.dist+=(1-m)*da+m*db;r.outer+=(1-m)*(da>12.5)+m*(db>12.5);a+=q*cos(2*pi*freq*pitch*i/48000);b+=q*sin(2*pi*freq*pitch*i/48000);e+=q*q;}
 free(v);r.dist/=count;r.outer/=count;r.amp=2*hypot(a,b)/count;r.rms=sqrt(e/count);return r;
}
int main(){double ds0=0,ds1=0,os0=0,os1=0;double frequencies[]={65,82.4,110,140,220,440,880};for(int j=0;j<7;j++){double f=frequencies[j];for(int i=0;i<96000;i++){x[i]=.2*sin(2*pi*f*i/48000);y[i]=.5*x[i];}for(int p=0;p<3;p++){double pitch=p==0?.5:p==1?1:2;Result a=hold(1,x,y,96000,48000,48000,f,pitch),b=hold(0,x,y,96000,48000,48000,f,pitch);printf("tone %.1f pitch %.1f distance %.3f -> %.3f ms; outside %.4f -> %.4f; amplitude %.4f -> %.4f\n",f,pitch,a.dist,b.dist,a.outer,b.outer,a.amp,b.amp);ds0+=a.dist;ds1+=b.dist;os0+=a.outer;os1+=b.outer;ck(b.amp>=a.amp*.96-.002,"Hold tone amplitude preserved against baseline");}}
 for(int i=0;i<96000;i++){double t=i/48000.;x[i]=.09*sin(2*pi*110*t)+.04*sin(2*pi*220*t)+.03*sin(2*pi*330*t)+.03*sin(2*pi*164.81*t);y[i]=.08*sin(2*pi*110*t+.3)+.04*sin(2*pi*329.63*t);}
 Result a=hold(1,x,y,96000,48000,48000,110,1),b=hold(0,x,y,96000,48000,48000,110,1);printf("Polyphonic stereo distance %.3f -> %.3f ms, outside %.4f -> %.4f, RMS %.4f -> %.4f\n",a.dist,b.dist,a.outer,b.outer,a.rms,b.rms);ck(b.rms>a.rms*.85&&b.rms<a.rms*1.15,"Polyphonic RMS stays within 15% of baseline");
 FILE*f=fopen("tests/reference/synthetic_speech_44100.f32","rb");ck(f!=0,"Speech fixture available");fseek(f,0,SEEK_END);long n=ftell(f)/4;rewind(f);float *pcm=malloc(n*4);ck(fread(pcm,4,n,f)==(size_t)n,"Speech fixture loaded");fclose(f);double s0=0,s1=0,o0=0,o1=0;for(int j=1;j<=12;j++){double cursor=n*(double)j/13.;a=hold(1,pcm,0,n,44100,cursor,0,1);b=hold(0,pcm,0,n,44100,cursor,0,1);printf("speech position %d: distance %.3f -> %.3f ms outside %.4f -> %.4f RMS %.4f -> %.4f\n",j,a.dist,b.dist,a.outer,b.outer,a.rms,b.rms);s0+=a.dist;s1+=b.dist;o0+=a.outer;o1+=b.outer;}
 printf("Tone averages: distance %.3f -> %.3f ms; outside %.4f -> %.4f\n",ds0/21,ds1/21,os0/21,os1/21);printf("Speech averages: distance %.3f -> %.3f ms; outside %.4f -> %.4f\n",s0/12,s1/12,o0/12,o1/12);
 ck(ds1<ds0&&os1<os0&&s1<s0&&o1<o0,"Weighted source footprint improves on aggregate tone and speech fixtures");
 for(int j=0;j<5;j++){double rates[]={.025,.08,.5,-.5,-1.},speed=rates[j];ApoScratchStretch*u=calloc(1,sizeof(*u)),*v=calloc(1,sizeof(*v));for(int i=0;i<24000;i++){double cursor=n*.4+speed*i*44100./48000.;float a=r15_original_render_pair(u,pcm,0,n,44100,48000,cursor,speed,1,0),b=r15_render_pair(v,pcm,0,n,44100,48000,cursor,speed,1,0);if(memcmp(&a,&b,sizeof(a)))exit(4);}free(u);free(v);}ck(1,"Moving speech output bit-identical to R15 across five rates");ApoScratchStretch*v=calloc(1,sizeof(*v));double cursor=n*.4,previous=0.,maxDelta=0.;int seenHold=0,seenMotion=0;for(int i=0;i<48000;i++){double speed=i<12000?.25:i<24000?0:i<36000?-.5:0;cursor+=speed*44100./48000.;float q=r15_render_pair(v,pcm,0,n,44100,48000,cursor,speed,1,0);if(!isfinite(q)||fabs(q)>1.5)exit(5);if(i){double d=fabs(q-previous);if(d>maxDelta)maxDelta=d;}previous=q;if(i>=13000&&i<24000&&v->wave.hop==300)seenHold=1;if(i>=25000&&i<36000&&v->wave.hop==600)seenMotion=1;}free(v);printf("Motion-hold-motion maximum adjacent sample delta %.6f\n",maxDelta);ck(seenHold&&seenMotion,"Stationary/moving hop changes at grain boundaries");free(pcm);return 0;
}
