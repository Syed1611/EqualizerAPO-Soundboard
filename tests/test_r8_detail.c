/* Moving multitone regression: compare the R7 hybrid and R8 detail modes. */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "src/scratch_dsp.h"
#define PI 3.14159265358979323846
static const double f[3]={220,1320,6000},amp[3]={.22,.12,.08};
static int run(int sr,int hr,double speed,int quality,double *out){
 int n=sr*6,len=hr;float*x=calloc(n,sizeof(*x));ApoScratchStretch*d=calloc(1,sizeof(*d));if(!x||!d)return 0;
 for(int i=0;i<n;i++)for(int k=0;k<3;k++)x[i]+=amp[k]*sin(2*PI*f[k]*i/sr);
 double cursor=sr*3.,a[3]={0},b[3]={0};scratch_stretch_init(d,cursor,sr,hr);d->spec.quality=quality;
 for(int i=0;i<hr/3+len;i++){cursor+=speed*sr/hr;float y=scratch_stretch_render(d,x,n,sr,hr,cursor,speed,1.);if(!isfinite(y))return 0;if(i>=hr/3)for(int k=0;k<3;k++){double w=2*PI*f[k]*(i-hr/3)/hr;a[k]+=y*cos(w);b[k]+=y*sin(w);}}
 printf("%d -> %d speed %+.2f mode %d:",sr,hr,speed,quality);for(int k=0;k<3;k++){out[k]=2*hypot(a[k],b[k])/len;printf(" %.5f",out[k]);}printf(" WS %.3f\n",d->spec.wsMix);int ok=quality!=2||d->spec.wsMix==0.;free(x);free(d);return ok;
}
int main(void){double a[3],b[3];if(!run(48000,48000,.5,1,a))return 1;
 const double speeds[]={0,.15,.5,-.5};const int rates[]={48000,44100};for(int r=0;r<2;r++)for(int j=0;j<4;j++){if(!run(rates[r],48000,speeds[j],2,b))return 2;for(int k=0;k<3;k++)if(fabs(b[k]/amp[k]-1.)>.08)return 3;}puts("PASS R8 moving spectral detail");return 0;}
