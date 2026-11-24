/* Standalone native render of original R2 and R21 with identical traces. */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifdef ORIGINAL_R2
#include "../src/r19_r2_original.h"
#else
#include "../src/r21_r2_pitch.h"
#endif
int main(int argc,char**argv){
 if(argc!=4)return 1;
 double pitch=pow(2.,atof(argv[1])/12.),speed=atof(argv[2]);int samples=atoi(argv[3]);
 unsigned n=240000;float*x=malloc(n*sizeof(float));ApoScratchStretch*v=calloc(1,sizeof(*v));if(!x||!v)return 2;
 for(unsigned i=0;i<n;i++){double t=i/48000.;x[i]=(float)(.25*sin(2*APO_SPECTRAL_PI*4000*t)+.25*sin(2*APO_SPECTRAL_PI*16000*t));}
 for(int i=0;i<samples;i++){float z=scratch_stretch_render(v,x,n,48000,48000,48000.+speed*i,speed,pitch);if(!isfinite(z))return 3;fwrite(&z,4,1,stdout);}free(x);free(v);return 0;
}
