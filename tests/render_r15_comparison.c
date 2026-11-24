/* Offline comparison generator. Uses the bundled synthetic (eSpeak) voice.
   Output is raw little-endian float32 at 48 kHz mono, not a listening test. */
#include <stdio.h>
#include <stdlib.h>
#include "../src/controller.c"
int main(){FILE*f=fopen("tests/reference/synthetic_speech_44100.f32","rb");if(!f)return 1;fseek(f,0,SEEK_END);long n=ftell(f)/4;rewind(f);float*x=malloc((size_t)n*4);if(!x||fread(x,4,n,f)!=(size_t)n)return 2;fclose(f);
 const unsigned sr=44100,host=48000;const double start=sr*1.0;
 for(int q=8;q<=9;q++){ApoScratchStretch*v=calloc(1,sizeof(*v));char path[64];sprintf(path,"validation/r%d-025x.raw",q==8?14:15);f=fopen(path,"wb");if(!f||!v)return 3;for(int i=0;i<host*6;i++){float z=r9_render(v,x,n,sr,host,start+.25*i*sr/host,.25,1.,q);fwrite(&z,4,1,f);}fclose(f);free(v);}f=fopen("validation/source-1x.raw","wb");if(!f)return 4;for(int i=0;i<host*3/2;i++){float z=r7_pcm(x,n,start+(double)i*sr/host,(double)sr/host);fwrite(&z,4,1,f);}fclose(f);free(x);return 0;}
