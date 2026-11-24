#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../src/controller.c"
static float pcm[48000];
static void check(int ok,const char*s){if(!ok){fprintf(stderr,"FAIL %s\n",s);exit(1);}puts(s);}
static double hold(int q){ApoScratchStretch*v=calloc(1,sizeof(*v));double e=0;for(int i=0;i<24000;i++){float y=r9_render(v,pcm,48000,48000,48000,24000,0,1,q);if(!isfinite(y))exit(2);if(i>=12000)e+=y*y;}free(v);return sqrt(e/12000);}
int main(){r7_read_options(0);check(g_r11Font==3&&g_r11Size==11&&r10_quality()==3,"R10 default migration");g_r11Font=7;g_r11Size=14;g_r11Local=1;WL_U32 opts=r7_options();r7_read_options(opts);check(g_r11Font==7&&g_r11Size==14&&r10_quality()==3,"Appearance persisted; one engine");check(r11_fade_at(0,1,0)==0&&r11_fade_at(0,1,150)==.5&&r11_fade_at(0,1,300)==1&&r11_fade_at(.5,0,300)==0,"300ms fade endpoints and reversal");
 for(int i=0;i<48000;i++)pcm[i]=fabs(i-24000)>400&&fabs(i-24000)<850?.3*sin(i*.16):0;double old=hold(4),local=hold(6);printf("Neighbor-only RMS original %.8f local %.8f\n",old,local);check(local<old*.8,"Localized mode reduces neighboring-only energy");
 for(int i=0;i<48000;i++)pcm[i]=.2*sin(2*3.141592653589793*220*i/48000);old=hold(4);local=hold(6);printf("Stable 220Hz RMS original %.8f local %.8f\n",old,local);check(local>old*.7&&local<old*1.4,"Stable tone level retained");return 0;}
