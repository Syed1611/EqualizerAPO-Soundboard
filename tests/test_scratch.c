#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "src/scratch_dsp.h"
#define SR 48000
static const double PI=3.14159265358979323846;
static int synthetic(double rate,double ratio,const char*name){
 int srcN=SR*5,n=SR*2;
 float*src=calloc(srcN,sizeof(float)),*out=calloc(n,sizeof(float));if(!src||!out)return 1;
 for(int i=0;i<srcN;i++)src[i]=(float)(0.54*sin(2*PI*220*i/SR)+0.16*sin(2*PI*440*i/SR));
 ApoScratchStretch st={0};double at=SR*2.0; scratch_stretch_init(&st,at,SR,SR);
 double rms=0,maxJump=0,prev=0;
 for(int i=0;i<n;i++){
  at+=rate;if(at<SR)at=SR;if(at>srcN-SR)at=srcN-SR;
  out[i]=scratch_stretch_render(&st,src,srcN,SR,SR,at,rate,ratio);
  if(!isfinite(out[i]))return 2;
  rms+=(double)out[i]*out[i];
  if(i>0&&fabs((double)out[i]-prev)>maxJump)maxJump=fabs((double)out[i]-prev);
  prev=out[i];
 }
 char fname[250];snprintf(fname,sizeof(fname),"test_%s.raw",name);
 FILE*f=fopen(fname,"wb");if(!f)return 3;fwrite(out,sizeof(float),n,f);fclose(f);
 printf("%s: rms %.4f max_jump %.4f\n",name,sqrt(rms/n),maxJump);
 free(src);free(out);return 0;
}
static int voice(double rate,double ratio,const char*name){
 FILE*f=fopen("voice_test_input.raw","rb");if(!f){puts("voice test skipped (optional voice_test_input.raw absent)");return 0;}
 fseek(f,0,SEEK_END);long bytes=ftell(f);fseek(f,0,SEEK_SET);int srcN=(int)(bytes/4);float*src=malloc(bytes);if(!src)return 11;
 if(fread(src,4,srcN,f)!=(size_t)srcN)return 12;fclose(f);
 int n=2*SR;float*out=calloc(n,sizeof(float));if(!out)return 13;
 ApoScratchStretch st={0};double at=SR*1.03; scratch_stretch_init(&st,at,SR,SR); double rms=0;
 for(int i=0;i<n;i++){
  at+=rate;if(at<SR/4)at=SR/4;if(at>(double)srcN-1000)at=(double)srcN-1000;
  out[i]=scratch_stretch_render(&st,src,srcN,SR,SR,at,rate,ratio);
  if(!isfinite(out[i]))return 14;
  rms+=out[i]*out[i];
 }
 char fname[250];snprintf(fname,sizeof(fname),"voice_%s.raw",name);f=fopen(fname,"wb");if(!f)return 15;
 fwrite(out,sizeof(float),n,f);fclose(f);printf("voice_%s rms %.4f\n",name,sqrt(rms/n));free(out);free(src);return 0;
}
int main(void){
 int err=0;
 err|=synthetic(0.0,1.0,"held_pitch0");
 err|=synthetic(0.5,1.0,"slow_pitch0");
 err|=synthetic(2.0,1.0,"fast_pitch0");
 err|=synthetic(-1.0,1.0,"reverse_pitch0");
 err|=synthetic(0.0,2.0,"held_plus12");
 err|=synthetic(0.0,0.5,"held_minus12");
 err|=synthetic(2.0,2.0,"fast_plus12");
 err|=voice(0.0,1.0,"held_pitch0");
 err|=voice(1.8,1.0,"fast_pitch0");
 err|=voice(0.0,1.5,"held_plus7");
 return err;
}
