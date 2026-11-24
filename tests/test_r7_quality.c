#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "src/scratch_dsp.h"
#define PI 3.14159265358979323846
static void check(int ok,const char *s){if(!ok){fprintf(stderr,"FAIL %s\n",s);exit(1);}}
static double tone_amp(const float*y,int n,int sr,double freq){double a=0,b=0;for(int i=0;i<n;i++){double w=2*PI*freq*i/sr;a+=y[i]*cos(w);b+=y[i]*sin(w);}return 2*sqrt(a*a+b*b)/n;}
static double run(int sr,int host,double freq,int quality,double *hz){int n=sr*2,m=host/3;float*x=malloc(n*4),*y=malloc(m*4);ApoScratchStretch*d=calloc(1,sizeof(*d));check(x&&y&&d,"allocation");for(int i=0;i<n;i++)x[i]=.3f*sin(2*PI*freq*i/sr);scratch_stretch_init(d,sr,sr,host);d->spec.quality=quality;for(int i=0;i<m+host/4;i++){float z=scratch_stretch_render(d,x,n,sr,host,sr,0,1);check(isfinite(z),"finite render");if(i>=host/4)y[i-host/4]=z;}double amp=tone_amp(y,m,host,freq);int crossings=0;for(int i=1;i<m;i++)if(y[i-1]<0&&y[i]>=0)crossings++;*hz=(double)crossings*host/m;free(x);free(y);free(d);return amp;}
int main(void){int rates[]={44100,48000,88200,96000,192000};for(int i=0;i<5;i++){double hz=0,amp=run(44100,rates[i],220,1,&hz);printf("44100 -> %d: %.2f Hz amplitude %.4f\n",rates[i],hz,amp);check(fabs(hz-220)<5&&amp>.15,"HQ held pitch and level");}
 double hz1,hz2;double old=run(44100,48000,6000,0,&hz1),now=run(44100,48000,6000,1,&hz2);printf("6 kHz held tone, 44.1 -> 48 kHz: legacy %.5f, HQ %.5f (input .30000)\n",old,now);check(now>old&&now>.15,"HQ retains more high-frequency tone amplitude");
 float pcm[4800];for(int i=0;i<4800;i++)pcm[i]=sin(2*PI*18000*i/48000);double e=0;for(int i=100;i<2300;i++){double z=r7_pcm(pcm,4800,2.*i,2.);e+=z*z;}printf("18 kHz -> 24 kHz anti-alias RMS %.7f\n",sqrt(e/2200));check(sqrt(e/2200)<.01,"cached sinc rejects alias");
 double maxerr=0;for(int i=0;i<3600;i++){double a=(i-1800)*PI/1800.;double b=scratch_atan2_hq(sin(a),cos(a));double err=fabs(scratch_wrap_angle(b-a));if(err>maxerr)maxerr=err;}printf("HQ phase max error %.3g rad\n",maxerr);check(maxerr<1e-7,"phase accuracy");puts("PASS R7 quality");return 0;}
