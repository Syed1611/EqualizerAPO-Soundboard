#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ApoScratchStretch ref_ApoScratchStretch
#define ApoSpectralFreeze ref_ApoSpectralFreeze
#define scratch_abs_d ref_scratch_abs_d
#define scratch_analyze ref_scratch_analyze
#define scratch_atan2 ref_scratch_atan2
#define scratch_clamp_d ref_scratch_clamp_d
#define scratch_fft ref_scratch_fft
#define scratch_ifft ref_scratch_ifft
#define scratch_read_source ref_scratch_read_source
#define scratch_sincos ref_scratch_sincos
#define scratch_sqrt ref_scratch_sqrt
#define scratch_stretch_init ref_scratch_stretch_init
#define scratch_stretch_render ref_scratch_stretch_render
#define scratch_synthesis_frame ref_scratch_synthesis_frame
#define scratch_wrap_angle ref_scratch_wrap_angle
#define scratch_ws_synthesis_frame ref_scratch_ws_synthesis_frame
#include "reference/r2_scratch_dsp.h"
#undef APO_SCRATCH_DSP_H
#undef ApoScratchStretch
#undef ApoSpectralFreeze
#undef scratch_abs_d
#undef scratch_analyze
#undef scratch_atan2
#undef scratch_clamp_d
#undef scratch_fft
#undef scratch_ifft
#undef scratch_read_source
#undef scratch_sincos
#undef scratch_sqrt
#undef scratch_stretch_init
#undef scratch_stretch_render
#undef scratch_synthesis_frame
#undef scratch_wrap_angle
#undef scratch_ws_synthesis_frame
#include "src/scratch_dsp.h"
#include "src/r9_r2.h"
#define PI 3.14159265358979323846
static void check(int ok,const char *s){if(!ok){fprintf(stderr,"FAIL: %s\n",s);exit(1);}}
static void compare(int sr,int host,const float*x,int n){
 int m=(int)(((unsigned long long)n*host+sr/2)/sr);float *grid=malloc(m*sizeof(float));ApoScratchStretch*v=calloc(1,sizeof(*v)),*hq=calloc(1,sizeof(*hq));ref_ApoScratchStretch*ref=calloc(1,sizeof(*ref));check(grid&&v&&hq&&ref,"alloc");
 ApoSpectralFreeze prep={0};prep.r9ActualFrames=n;prep.r9ActualRate=sr;prep.sampleRate=host;for(int i=0;i<m;i++)grid[i]=sr==host?x[i]:r9_grid_sample(&prep,x,i);
 double max=0.,energy[2]={0},delta[2]={0},last[2]={0};double pos=n*.5;int used[2]={0};
 for(int i=0;i<host;i++){int seg=i/(host/5);double speed=seg==0?.5:seg==1?.12:seg==2?0:seg==3?-.5:1.;double pitch=seg==4?1.25:1.;pos+=speed*sr/host;double at=sr==host?pos:pos*(double)m/n;
 float a=ref_scratch_stretch_render(ref,grid,m,host,host,at,speed,pitch);float b=r9_render(v,x,n,sr,host,pos,speed,pitch,3);float c=r9_render(hq,x,n,sr,host,pos,speed,pitch,4);check(isfinite(a)&&isfinite(b)&&isfinite(c),"finite output");double d=fabs(a-b);if(d>max)max=d;energy[0]+=b*b;energy[1]+=c*c;delta[0]+=(b-last[0])*(b-last[0]);delta[1]+=(c-last[1])*(c-last[1]);last[0]=b;last[1]=c;used[0]+=v->spec.wsMix>.9;used[1]+=hq->spec.wsMix>.9;}
 printf("%d -> %d original max error %.9g; RMS original %.5f enhanced %.5f; WSOLA samples %d / %d; derivative RMS %.5f / %.5f\n",sr,host,max,sqrt(energy[0]/host),sqrt(energy[1]/host),used[0],used[1],sqrt(delta[0]/host),sqrt(delta[1]/host));check(max<1e-6,"Original R2 reference equivalence");check(energy[1]>energy[0]*.25&&energy[1]<energy[0]*4,"enhanced bounded energy");
 float q=r9_render(hq,x,n,sr,96000,pos,0,1,4);check(isfinite(q)&&hq->spec.sampleRate==96000&&hq->spec.outputFrames==1,"live host-rate reset");free(grid);free(v);free(hq);free(ref);
}
int main(void){int n=144000;float*x=malloc(n*4);check(x!=0,"source allocation");for(int i=0;i<n;i++){double t=i/48000.;x[i]=.23*sin(2*PI*220*t)+.10*sin(2*PI*1320*t)+.06*sin(2*PI*6000*t);}
 compare(48000,48000,x,n);compare(44100,48000,x,n);
 /* Source-view alias rejection before the hybrid synthesizer. */
 for(int i=0;i<n;i++)x[i]=.5*sin(2*PI*18000*i/48000.);
 ApoSpectralFreeze *sp=calloc(1,sizeof(*sp));check(sp!=0,"alias state");sp->r9ActualFrames=n;sp->r9ActualRate=48000;sp->sampleRate=24000;double e[2]={0};for(int i=128;i<12000;i++){for(int k=0;k<2;k++){sp->quality=3+k;float q=r9_source(sp,x,n/2,i,1);e[k]+=q*q;}}printf("Source view alias RMS original %.6f enhanced %.6f\n",sqrt(e[0]/11872),sqrt(e[1]/11872));check(e[1]<e[0]*.001,"Enhanced source alias rejection");free(sp);free(x);
 FILE*f=fopen("tests/reference/synthetic_speech_44100.f32","rb");if(f){fseek(f,0,SEEK_END);n=ftell(f)/4;rewind(f);x=malloc(n*4);check(x&&fread(x,4,n,f)==(size_t)n,"speech fixture");fclose(f);compare(44100,48000,x,n);free(x);}else puts("SKIP optional synthesized speech fixture");puts("PASS R9 R2 equivalence and enhanced movement");return 0;}
