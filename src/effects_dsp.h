/* Scratch Edition v1.0.2 R2: independently developed stereo, diffuse reverb
   and cross-fed filtered Echo. No external library/allocations on audio thread.
   Inspired by general controls described in Ableton's public documentation;
   this is NOT Ableton DSP and cannot be advertised as an exact clone. */
#ifndef APO_EFFECTS_DSP_H
#define APO_EFFECTS_DSP_H
#define APO_FX_ECHO_CAP 262144u
#define APO_FX_COMB_CAP 65536u
#define APO_FX_ALLPASS_CAP 4096u
#define APO_FX_PRE_CAP 65536u
#define APO_FX_COMBS 8u
#define APO_FX_DIFFUSERS 4u
#define APO_FX_PRESET_COUNT 8
/* Retain exact disk format from v1.0.2: no settings migration or corruption. */
typedef struct {
 int preset,echoOn,reverbOn,echoMs,echoFeedback,echoMix,room,damping,reverbMix;
} ApoFxConfig;
typedef struct {
 float echo[2][APO_FX_ECHO_CAP];
 float comb[APO_FX_COMBS][APO_FX_COMB_CAP],combFilter[APO_FX_COMBS];
 float diffusion[APO_FX_DIFFUSERS][APO_FX_ALLPASS_CAP];
 float pre[APO_FX_PRE_CAP];
 float echoLP[2],echoHP[2],preLP,preHP;
 float fbGain[APO_FX_COMBS],fbGainTarget[APO_FX_COMBS];
 float echoFeedbackSmooth,echoMixSmooth,reverbMixSmooth,dampingSmooth;
 unsigned smoothReady,echoLive,revLive;
 unsigned echoAt,combAt[APO_FX_COMBS],diffAt[APO_FX_DIFFUSERS],preAt;
 unsigned tailFrames,modFrame,echoRampLeft,lastRate;
 int lastRoom;
 float echoMsCurrent,echoMsFrom,echoMsTo;
} ApoFxState;
static int apo_fx_clamp(int x,int lo,int hi){return x<lo?lo:(x>hi?hi:x);}
static float apo_fx_bound(float v,float x){return v>x?x:(v< -x?-x:v);}
static float apo_fx_tiny(float v){return v>-1.0e-18f&&v<1.0e-18f?0.f:v;}
static ApoFxConfig apo_fx_preset(int n){
 /* Room maps primarily to a spacious decay/diffusion character. Short-room
    reflections intentionally receive little level even in the meme preset. */
 ApoFxConfig c={0,0,0,300,45,0,75,30,0};c.preset=n;
 switch(n){
 case 1:c.echoOn=1;c.reverbOn=1;c.echoMs=300;c.echoFeedback=56;c.echoMix=68;c.room=79;c.damping=40;c.reverbMix=39;break;
 case 2:c.echoOn=1;c.echoMs=115;c.echoFeedback=20;c.echoMix=53;break;
 case 3:c.reverbOn=1;c.room=83;c.damping=37;c.reverbMix=68;break;
 case 4:c.echoOn=1;c.reverbOn=1;c.echoMs=430;c.echoFeedback=53;c.echoMix=42;c.room=96;c.damping=62;c.reverbMix=72;break;
 case 5:c.reverbOn=1;c.room=26;c.damping=9;c.reverbMix=40;break;
 case 6:c.echoOn=1;c.reverbOn=1;c.echoMs=360;c.echoFeedback=39;c.echoMix=32;c.room=91;c.damping=53;c.reverbMix=58;break;
 case 7:c.echoOn=1;c.reverbOn=1;c.echoMs=300;c.echoFeedback=45;c.echoMix=36;c.room=76;c.damping=35;c.reverbMix=40;break;
 default:c.preset=0;break;
 }
 return c;
}
/* Linear fractional interpolation for a click-free dual-head time change. */
static float apo_fx_echo_tap(const ApoFxState*s,int chan,float frames){
 if(frames<1.f)frames=1.f;if(frames>(float)(APO_FX_ECHO_CAP-3u))frames=(float)(APO_FX_ECHO_CAP-3u);
 unsigned n=(unsigned)frames;float frac=frames-(float)n;
 unsigned a=(s->echoAt+APO_FX_ECHO_CAP-n)&(APO_FX_ECHO_CAP-1u);
 unsigned b=(a+APO_FX_ECHO_CAP-1u)&(APO_FX_ECHO_CAP-1u);
 return s->echo[chan][a]*(1.f-frac)+s->echo[chan][b]*frac;
}
static void apo_fx_update_decay(ApoFxState*s,int room,unsigned rate){
 if(s->lastRate==rate&&s->lastRoom==room)return;
 int firstInit=(s->lastRate==0);
 s->lastRate=rate;s->lastRoom=room;
 /* Smooth long-tail T60 approximation, with a wide range of concert-hall
    decay times. Gains are updated only when settings or host rate changes. */
 float rt60=1.4f+((float)room/100.f)*7.2f;
 static const unsigned times[8]={2843u,3307u,3701u,4099u,4483u,5021u,5437u,6029u};
 for(unsigned i=0;i<8;i++){
  /* Scale the feedback decay by the actual delay length at 48 kHz.
     Only tail decay changes with the user control, never a delay read head. */
  float seconds=(float)times[i]/48000.f;
  float x=6.9f*seconds/rt60;
  s->fbGainTarget[i]=1.f/(1.f+x+.5f*x*x);
  if(s->fbGainTarget[i]>.992f)s->fbGainTarget[i]=.992f;
  if(firstInit)s->fbGain[i]=s->fbGainTarget[i];
 }
}
/* Clear a retired tank once; do not leave old samples frozen in its ring. */
static void apo_fx_clear_echo(ApoFxState*s){
 for(unsigned ch=0;ch<2;ch++){for(unsigned i=0;i<APO_FX_ECHO_CAP;i++)s->echo[ch][i]=0.f;s->echoLP[ch]=s->echoHP[ch]=0.f;}
 s->echoAt=s->echoRampLeft=0;s->echoMsCurrent=s->echoMsFrom=s->echoMsTo=0.f;s->echoLive=0;s->echoMixSmooth=0.f;
}
static void apo_fx_clear_rev(ApoFxState*s){
 for(unsigned i=0;i<APO_FX_COMBS;i++){for(unsigned j=0;j<APO_FX_COMB_CAP;j++)s->comb[i][j]=0.f;s->combAt[i]=0;s->combFilter[i]=0.f;s->fbGain[i]=s->fbGainTarget[i]=0.f;}
 for(unsigned i=0;i<APO_FX_DIFFUSERS;i++){for(unsigned j=0;j<APO_FX_ALLPASS_CAP;j++)s->diffusion[i][j]=0.f;s->diffAt[i]=0;}
 for(unsigned i=0;i<APO_FX_PRE_CAP;i++)s->pre[i]=0.f;
 s->preAt=s->modFrame=s->lastRate=0;s->preLP=s->preHP=0.f;s->revLive=0;s->reverbMixSmooth=0.f;
}
static int apo_fx_should_process(const ApoFxState*s,const ApoFxConfig*c,float dry){
 return s->tailFrames||s->echoLive||s->revLive||((dry!=0.f)&&((c->echoOn&&c->echoMix>0)||(c->reverbOn&&c->reverbMix>0)));
}
/* True unity-magnitude allpass, with the feedback taken from its OUTPUT. */
static float apo_fx_diffuse(float x,float *memory){float old=*memory;float y=old-.70f*x;*memory=apo_fx_tiny(x+.70f*y);return y;}
/* Two channels are returned directly to Equalizer APO (and local monitor).
   Echo ping-pongs left/right, reverberant tail is decorrelated for width. */
static void apo_fx_process_stereo(ApoFxState*s,const ApoFxConfig*c,unsigned rate,float dry,float*left,float*right){
 if(!left||!right)return;
 *left=dry;*right=dry;
 if(!s||!c)return;
 if(rate<8000u||rate>384000u)rate=48000u;
 /* Keep a wet processor active until its mix has ramped fully to zero.
    Otherwise moving the mini-bar to 0 would hard-cut an audible tail. */
 float targetFeedback=(float)apo_fx_clamp(c->echoFeedback,0,78)*.01f;
 float targetEchoMix=c->echoOn?(float)apo_fx_clamp(c->echoMix,0,100)*.01f:0.f;
 float targetRevMix=c->reverbOn?(float)apo_fx_clamp(c->reverbMix,0,100)*.01f:0.f;
 float targetDamp=(float)apo_fx_clamp(c->damping,0,100)*.01f;
 if(!s->smoothReady){
  s->echoFeedbackSmooth=targetFeedback;s->echoMixSmooth=targetEchoMix;
  s->reverbMixSmooth=targetRevMix;s->dampingSmooth=targetDamp;s->smoothReady=1;
 }
 float smoothStep=52.f/(float)rate;
 s->echoFeedbackSmooth+=(targetFeedback-s->echoFeedbackSmooth)*smoothStep;
 s->echoMixSmooth+=(targetEchoMix-s->echoMixSmooth)*smoothStep;
 s->reverbMixSmooth+=(targetRevMix-s->reverbMixSmooth)*smoothStep;
 s->dampingSmooth+=(targetDamp-s->dampingSmooth)*smoothStep;
 int echo=targetEchoMix>0.f||s->echoMixSmooth>.0001f;
 int rev=targetRevMix>0.f||s->reverbMixSmooth>.0001f;
 if(!echo&&s->echoLive)apo_fx_clear_echo(s);
 if(!rev&&s->revLive)apo_fx_clear_rev(s);
 if(!echo&&!rev){s->tailFrames=0;s->smoothReady=0;return;}
 float absDry=dry<0.f?-dry:dry;
 if(absDry>1.e-7f&&(targetEchoMix>0.f||targetRevMix>0.f))s->tailFrames=rate*12u;
 else if(s->tailFrames)s->tailFrames--;
 if(!s->tailFrames&&absDry<1.e-10f){
  if(s->echoLive)apo_fx_clear_echo(s);if(s->revLive)apo_fx_clear_rev(s);s->smoothReady=0;return;
 }
 float wetEchoL=0.f,wetEchoR=0.f;
 if(echo){
  s->echoLive=1;
  int wanted=apo_fx_clamp(c->echoMs,50,650);
  if(s->echoMsTo<1.f){s->echoMsTo=(float)wanted;s->echoMsFrom=(float)wanted;s->echoMsCurrent=(float)wanted;}
  if((int)s->echoMsTo!=wanted){
   s->echoMsFrom=s->echoMsCurrent;s->echoMsTo=(float)wanted;
   s->echoRampLeft=rate/25u;if(!s->echoRampLeft)s->echoRampLeft=1;
  }
  float a=s->echoMsFrom*(float)rate*.001f,b=s->echoMsTo*(float)rate*.001f;
  wetEchoL=apo_fx_echo_tap(s,0,b);wetEchoR=apo_fx_echo_tap(s,1,b);
  if(s->echoRampLeft){
   float k=1.f-(float)s->echoRampLeft/((float)rate/25.f);if(k<0)k=0;if(k>1)k=1;
   wetEchoL=apo_fx_echo_tap(s,0,a)*(1.f-k)+wetEchoL*k;
   wetEchoR=apo_fx_echo_tap(s,1,a)*(1.f-k)+wetEchoR*k;
   s->echoMsCurrent=s->echoMsFrom*(1.f-k)+s->echoMsTo*k;
   if(!--s->echoRampLeft)s->echoMsCurrent=s->echoMsTo;
  }
  /* Band-limited feedback: each repeat becomes softer, avoiding the small
     metallic-room quality of reverb taps mistakenly used as an echo. */
  float lp=(float)rate/48000.f;lp=.62f/lp;if(lp>.7f)lp=.7f;
  float hp=.006f/((float)rate/48000.f);
  s->echoLP[0]=apo_fx_tiny(s->echoLP[0]+lp*(wetEchoL-s->echoLP[0]));
  s->echoLP[1]=apo_fx_tiny(s->echoLP[1]+lp*(wetEchoR-s->echoLP[1]));
  s->echoHP[0]=apo_fx_tiny(s->echoHP[0]+hp*(s->echoLP[0]-s->echoHP[0]));
  s->echoHP[1]=apo_fx_tiny(s->echoHP[1]+hp*(s->echoLP[1]-s->echoHP[1]));
  float filteredL=s->echoLP[0]-s->echoHP[0],filteredR=s->echoLP[1]-s->echoHP[1];
  float fb=s->echoFeedbackSmooth;
  /* Original note enters L; delay from L enters R and vice versa. */
  s->echo[0][s->echoAt]=apo_fx_bound(dry+fb*filteredR,2.f);
  s->echo[1][s->echoAt]=apo_fx_bound(fb*filteredL,2.f);
  s->echoAt=(s->echoAt+1u)&(APO_FX_ECHO_CAP-1u);
 }
 float wetL=0.f,wetR=0.f;
 if(rev){
  s->revLive=1;
  int room=apo_fx_clamp(c->room,0,100);
  apo_fx_update_decay(s,room,rate);
  float damp=s->dampingSmooth;
  float dryIn=dry+.12f*s->echoMixSmooth*(wetEchoL+wetEchoR);
  /* Gentle peak headroom for the tank, unity for ordinary input. */
  float peak=dryIn<0.f?-dryIn:dryIn;if(peak>1.f)dryIn/=1.f+.5f*(peak-1.f);
  /* Remove low-frequency buildup before the diffuse tank. */
  float hp=.023f*(48000.f/(float)rate);
  float lp=.46f*(48000.f/(float)rate);
  if(lp>.88f)lp=.88f;
  s->preHP=apo_fx_tiny(s->preHP+hp*(dryIn-s->preHP));
  s->preLP=apo_fx_tiny(s->preLP+lp*((dryIn-s->preHP)-s->preLP));
  unsigned preMask=APO_FX_PRE_CAP-1u;
  s->pre[s->preAt]=s->preLP;
  unsigned predelay=(unsigned)(.042f*(float)rate);
  if(predelay>APO_FX_PRE_CAP-1u)predelay=APO_FX_PRE_CAP-1u;
  unsigned delayAt=(s->preAt+APO_FX_PRE_CAP-predelay)&preMask;
  float diffuse=s->pre[delayAt];s->preAt=(s->preAt+1u)&preMask;
  /* High diffusion before entering the 8-channel FDN. No prominent early
     reflections: intentionally favours a lush, open hall over a small room. */
  static const unsigned apTimes[4]={347u,491u,677u,863u};
  for(unsigned i=0;i<APO_FX_DIFFUSERS;i++){
   unsigned len=(unsigned)((unsigned long long)apTimes[i]*rate/48000u);
   if(len>APO_FX_ALLPASS_CAP-2u)len=APO_FX_ALLPASS_CAP-2u;
   if(len<2u)len=2u;
   unsigned at=s->diffAt[i];if(at>=len)at=0;
   float y=apo_fx_diffuse(diffuse,&s->diffusion[i][at]);
   s->diffAt[i]=(at+1u<len)?at+1u:0u;
   diffuse=y;
  }
  static const unsigned bases[8]={2843u,3307u,3701u,4099u,4483u,5021u,5437u,6029u};
  float mixed[8],raw[8];
  for(unsigned i=0;i<8;i++){
   unsigned len=(unsigned)(((unsigned long long)bases[i]*rate)/48000u);
   if(len>=APO_FX_COMB_CAP-96u)len=APO_FX_COMB_CAP-96u-(7u-i)*32u;
   if(len<9u)len=9u;
   unsigned w=s->combAt[i];
   unsigned period=rate/2u+(i+1u)*997u;
   if(period<32u)period=32u;
   unsigned ph=(s->modFrame+i*7193u)%period;
   float tri=(ph<period/2u?(float)ph/(float)(period/2u):(float)(period-ph)/(float)(period-period/2u));
   tri=tri*tri*(3.f-2.f*tri);
   float mod=(tri*2.f-1.f)*(float)rate/48000.f*6.5f;
   float read=(float)len+mod;
   if(read>(float)(APO_FX_COMB_CAP-3u))read=(float)(APO_FX_COMB_CAP-3u);
   if(read<1.f)read=1.f;
   unsigned n=(unsigned)read;float frac=read-(float)n;
   unsigned a=(w+APO_FX_COMB_CAP-n)&(APO_FX_COMB_CAP-1u);
   unsigned b=(a+APO_FX_COMB_CAP-1u)&(APO_FX_COMB_CAP-1u);
   raw[i]=s->comb[i][a]*(1.f-frac)+s->comb[i][b]*frac;
   float cutoff=(.48f-.40f*damp)*(48000.f/(float)rate);
   if(cutoff>.80f)cutoff=.80f;
   s->combFilter[i]=apo_fx_tiny(s->combFilter[i]+cutoff*(raw[i]-s->combFilter[i]));
   mixed[i]=s->combFilter[i];
  }
  for(unsigned span=1u;span<8u;span<<=1u){
   for(unsigned a=0;a<8;a+=span*2u)for(unsigned b=0;b<span;b++){
    float x=mixed[a+b],y=mixed[a+b+span];mixed[a+b]=x+y;mixed[a+b+span]=x-y;
   }
  }
  static const int leftSign[8]={1,1,-1,-1,1,1,-1,-1};
  static const int rightSign[8]={1,-1,1,-1,-1,1,-1,1};
  for(unsigned i=0;i<8;i++){
   /* A change to Decay no longer instantly changes the feedback gain
      of eight active delays (the previous source of occasional clicks). */
   s->fbGain[i]+=(s->fbGainTarget[i]-s->fbGain[i])*smoothStep;
   unsigned at=s->combAt[i];
   s->comb[i][at]=apo_fx_tiny(diffuse*.30f+mixed[i]*.35355339f*s->fbGain[i]);
   s->combAt[i]=(at+1u)&(APO_FX_COMB_CAP-1u);
   wetL+=(float)leftSign[i]*raw[i];wetR+=(float)rightSign[i]*raw[i];
  }
  s->modFrame++;
  /* Decorrelated diffuse tail. Early reflections kept nearly inaudible. */
  wetL=wetL*.36f+diffuse*.008f;
  wetR=wetR*.36f-diffuse*.008f;
 }
 float eMix=s->echoMixSmooth;
 float rMix=s->reverbMixSmooth;
 *left=apo_fx_tiny(dry+wetEchoL*eMix+wetL*rMix);
 *right=apo_fx_tiny(dry+wetEchoR*eMix+wetR*rMix);
}
/* Compatibility for offline mono tests; the actual VST uses stereo output. */
static float apo_fx_process(ApoFxState*s,const ApoFxConfig*c,unsigned rate,float dry){
 float l=0,r=0;apo_fx_process_stereo(s,c,rate,dry,&l,&r);return l;
}
#endif
