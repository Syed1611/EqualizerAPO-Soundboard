#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "src/scratch_dsp.h"
static double freq(const float*s,int sr,int a,int n){int cross=0;for(int i=a+1;i<a+n;i++)if(s[i-1]<0&&s[i]>=0)cross++;return (double)cross*sr/n;}
int main(){
 int source=44100,host=48000,n=source*3,m=host*2;
 float*in=calloc(n,sizeof(float)),*out=calloc(m,sizeof(float));ApoScratchStretch*st=calloc(1,sizeof(*st));if(!in||!out||!st)return 1;
 for(int i=0;i<n;i++)in[i]=(float)(.33*sin(2*3.141592653589793*220.0*i/(double)source));
 scratch_stretch_init(st,source*1.5,source,host);
 for(int i=0;i<m;i++){out[i]=scratch_stretch_render(st,in,n,source,host,source*1.5,0.,1.0);if(!isfinite(out[i]))return 2;}
 double hz=freq(out,host,host/2,host);printf("44100 to 48000 held tone = %.2fHz\n",hz);
 free(in);free(out);free(st);
 return fabs(hz-220)>4?3:0;
}
