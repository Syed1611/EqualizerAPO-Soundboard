#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "src/scratch_dsp.h"
#define SR 48000
int main(){
 int n=SR*5, outN=SR*2;float*in=calloc(n,sizeof(float)),*out=calloc(outN,sizeof(float));ApoScratchStretch*st=calloc(1,sizeof(*st));
 for(int i=0;i<n;i++){
  double t=(double)i/SR;
  in[i]=(float)(.28*sin(2*3.141592653589793*(140*t+70*t*t))+.14*sin(2*3.141592653589793*(460*t-30*t*t))+.07*sin(2*3.141592653589793*3400*t));
 }
 double center=SR*1.0;
 for(int i=0;i<outN;i++){
  out[i]=scratch_stretch_render(st,in,n,SR,SR,center+i,1.0,1.0);
 }
 FILE*f=fopen("fidelity.raw","wb");fwrite(out,sizeof(float),outN,f);fclose(f);
 double bestErr=1e100;int bestOffset=0;
 for(int off=-1500;off<1500;off++){
  double err=0,energy=0;for(int i=SR/2;i<outN;i++){
   int j=(int)center+i+off;if(j<0||j>=n)continue;double d=out[i]-in[j];err+=d*d;energy+=(double)in[j]*in[j];
  }
  double ratio=err/(energy+1e-20);if(ratio<bestErr){bestErr=ratio;bestOffset=off;}
 }
 printf("fidelity 1x error ratio=%.5f (SNR=%.2fdB) bestOffset=%d peak=",bestErr,-10*log10(bestErr),bestOffset);
 double peak=0;for(int i=0;i<outN;i++)if(fabs(out[i])>peak)peak=fabs(out[i]);printf("%.3f\n",peak);
 free(out);free(in);free(st);return bestErr<.001?0:6;
}
