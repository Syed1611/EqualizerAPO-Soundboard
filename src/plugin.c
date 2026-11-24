#include "winlite.h"

#define VST_MAGIC ((WL_I32)0x56737450) /* 'VstP' */
#define FOURCC(a,b,c,d) ((WL_I32)(((WL_U32)(a)<<24)|((WL_U32)(b)<<16)|((WL_U32)(c)<<8)|(WL_U32)(d)))
#define PLUGIN_ID FOURCC('A','P','S','X')
#define EFF_HAS_EDITOR (1<<0)
#define EFF_CAN_REPLACING (1<<4)
#define WS_CHILD 0x40000000u
#define WS_VISIBLE 0x10000000u
#define SS_LEFT 0x00000000u
#define SW_SHOWNORMAL 1
#define PIPE_CHUNK_MAX 1024u
#define PROTOCOL_VERSION 3u
#define BUILD_VERSION 110000u
#define PIPE_AUDIO_IDLE 1u
#define RING_FRAMES 4096u
#define RING_MASK (RING_FRAMES-1u)
#define TARGET_BASE 1024u
#define TARGET_MAX 2048u
#define TARGET_STEP 256u
#define INFINITE 0xffffffffu
#define WAIT_TIMEOUT 258u
#define GENERIC_READ 0x80000000u
#define GENERIC_WRITE 0x40000000u
#define OPEN_EXISTING 3u
#define FILE_ATTRIBUTE_NORMAL 0x80u

/* VST2 ABI subset. */
typedef struct AEffect AEffect;
typedef WL_IPTR (WL_CALLBACK *audioMasterCallback)(AEffect*,WL_I32,WL_I32,WL_IPTR,void*,float);
typedef WL_IPTR (WL_CALLBACK *AEffectDispatcherProc)(AEffect*,WL_I32,WL_I32,WL_IPTR,void*,float);
typedef void (WL_CALLBACK *AEffectProcessProc)(AEffect*,float**,float**,WL_I32);
typedef void (WL_CALLBACK *AEffectSetParameterProc)(AEffect*,WL_I32,float);
typedef float (WL_CALLBACK *AEffectGetParameterProc)(AEffect*,WL_I32);
typedef void (WL_CALLBACK *AEffectProcessDoubleProc)(AEffect*,double**,double**,WL_I32);
struct AEffect {
 WL_I32 magic; AEffectDispatcherProc dispatcher; AEffectProcessProc process; AEffectSetParameterProc setParameter; AEffectGetParameterProc getParameter;
 WL_I32 numPrograms,numParams,numInputs,numOutputs,flags; void* resvd1; void* resvd2; WL_I32 initialDelay,realQualities,offQualities; float ioRatio; void* object; void* user; WL_I32 uniqueID,version; AEffectProcessProc processReplacing; AEffectProcessDoubleProc processDoubleReplacing; char future[56];
};

enum { effOpen=0,effClose=1,effGetParamLabel=6,effGetParamDisplay=7,effGetParamName=8,effSetSampleRate=10,effSetBlockSize=11,effMainsChanged=12,effEditGetRect=13,effEditOpen=14,effEditClose=15,effEditIdle=19,effGetPlugCategory=35,effGetEffectName=45,effGetVendorString=47,effGetProductString=48,effGetVendorVersion=49,effCanDo=51,effGetVstVersion=58,effStartProcess=71,effStopProcess=72 };
typedef struct { WL_I16 top,left,bottom,right; } VstRect;

#pragma pack(push,1)
typedef struct { WL_U32 magic,version,build,sampleRate,channels; } PipeHello;
typedef struct { WL_U32 magic,version,build,flags; } PipeAck;
typedef struct { WL_U32 magic,frames,sampleRate,blockSize,ringFill,targetFill,underruns,clips,peakMilli; } PipeRequest;
typedef struct { WL_U32 magic,frames,channels,flags; } PipeAudio;
#pragma pack(pop)
#define M_HELLO 0x31425341u
#define M_REQ   0x51525341u
#define M_AUDIO 0x44525341u
#define M_ACK   0x4b435341u
static const WL_WCHAR PIPE_NAME[]={ '\\','\\','.','\\','p','i','p','e','\\','A','P','O','S','o','u','n','d','b','o','a','r','d','S','c','r','a','t','c','h','_','v','1','2','s',0 };

typedef struct {
 AEffect effect;
 audioMasterCallback host;
 WL_API api;
 volatile WL_I32 apiReady;
 volatile WL_I32 running;
 WL_HANDLE thread;
 volatile WL_HANDLE pipe;
 volatile WL_U32 sampleRate;
 volatile WL_U32 rateEpoch,ringEpoch;
 volatile WL_U32 blockSize;
 volatile WL_U32 rpos,wpos;
 volatile WL_I32 ringResetLock;
 volatile WL_U32 targetFill;
 volatile WL_U32 underruns;
 volatile WL_U32 clipCount;
 volatile WL_U32 peakMilli;
 volatile WL_U64 stableFrames;
 volatile WL_I32 streamReady;
 volatile WL_I32 sourceIdle;
 volatile WL_I32 boardIdle;
 volatile WL_I32 protocolReady;
 float ring[RING_FRAMES*2];
 float params[4];
 float limiterGain;
 VstRect editorRect;
 WL_API editorApi;
 WL_I32 editorReady;
} Plugin;


static WL_HMODULE g_dll_module=0;
static const WL_WCHAR CONTROLLER_EXE[]={'A','P','O','S','o','u','n','d','b','o','a','r','d','S','c','r','a','t','c','h','C','o','n','t','r','o','l','l','e','r','.','e','x','e',0};
/* The VST editor is a launch action, not a second UI. Equalizer APO still
   supplies its own editor host window; the plugin creates no child panel. */
static const WL_WCHAR CONTROLLER_CLASS[]={'A','P','O','S','o','u','n','d','b','o','a','r','d','S','c','r','a','t','c','h','C','t','r','l','R','2','0',0};

/* Win32 API declarations for explicit same-user, non-elevating launch. */
typedef struct {
 WL_DWORD cb;WL_WCHAR *lpReserved,*lpDesktop,*lpTitle;
 WL_DWORD dwX,dwY,dwXSize,dwYSize,dwXCountChars,dwYCountChars,dwFillAttribute,dwFlags;
 WL_WORD wShowWindow,cbReserved2;WL_U8*lpReserved2;
 WL_HANDLE hStdInput,hStdOutput,hStdError;
} APO_STARTUPINFOW;
typedef struct { WL_HANDLE hProcess,hThread;WL_DWORD dwProcessId,dwThreadId; } APO_PROCESS_INFORMATION;
typedef WL_BOOL (WL_WINAPI *APO_CreateProcessW)(const WL_WCHAR*,WL_WCHAR*,void*,void*,WL_BOOL,WL_DWORD,void*,const WL_WCHAR*,APO_STARTUPINFOW*,APO_PROCESS_INFORMATION*);
typedef WL_HWND (WL_WINAPI *APO_FindWindowW)(const WL_WCHAR*,const WL_WCHAR*);
typedef WL_BOOL (WL_WINAPI *APO_SetForegroundWindow)(WL_HWND);
typedef WL_HWND (WL_WINAPI *APO_GetAncestor)(WL_HWND,WL_UINT);
typedef WL_HWND (WL_WINAPI *APO_GetWindow)(WL_HWND,WL_UINT);
typedef WL_IPTR (WL_WINAPI *APO_GetWindowLongPtrW)(WL_HWND,WL_I32);
#define APO_SW_RESTORE 9
#define APO_GA_ROOT 2u
#define APO_GW_OWNER 4u
#define APO_GWL_STYLE (-16)
#define APO_WS_POPUP 0x80000000u
#define APO_WM_CLOSE 0x0010u

_Static_assert(sizeof(APO_STARTUPINFOW)==104, "STARTUPINFOW x64 ABI");
_Static_assert(sizeof(APO_PROCESS_INFORMATION)==24, "PROCESS_INFORMATION x64 ABI");

/* Open Panel needs only kernel32 + a few user32 actions. The R3 editor
   initialized common dialogs, GDI, Advapi32 and Shell32 even though no VST
   child UI exists. Avoid loading those modules on every host startup path. */
static int apo_init_editor_api(WL_API*a){
 if(!wl_init_kernel(a))return 0;
 a->user32=a->LoadLibraryW(wl_user32_name);
 if(!a->user32)return 0;
 WL_RESOLVE(a,ShowWindow,a->user32);
 WL_RESOLVE(a,PostMessageW,a->user32);
 WL_RESOLVE(a,MessageBoxW,a->user32);
 WL_RESOLVE(a,GetWindowTextW,a->user32);
 return 1;
}

/* Return 1 for either a newly started controller or an existing instance.
   This intentionally never invokes ShellExecute 'runas' or the UAC broker. */
static int launch_controller(Plugin*p){
 WL_API*a=&p->editorApi;
 if(!p->editorReady||!g_dll_module)return 0;
 APO_FindWindowW find=(APO_FindWindowW)wl_get_proc(a->user32,"FindWindowW");
 APO_SetForegroundWindow focus=(APO_SetForegroundWindow)wl_get_proc(a->user32,"SetForegroundWindow");
 if(find){
  WL_HWND existing=find(CONTROLLER_CLASS,0);
  if(existing){a->ShowWindow(existing,APO_SW_RESTORE);if(focus)focus(existing);return 1;}
 }
 /* Startup occurs on the editor thread, not on the audio callback.
    Use a heap path buffer so long Unicode locations do not introduce an
    MSVC __chkstk runtime dependency in our freestanding VST DLL. */
 WL_WCHAR* storage=(WL_WCHAR*)a->HeapAlloc(a->GetProcessHeap(),0,4096u*sizeof(WL_WCHAR));
 if(!storage)return 0;
 WL_WCHAR* path=storage,*folder=storage+2048;
 WL_DWORD n=a->GetModuleFileNameW(g_dll_module,path,2048);
 if(!n||n>=2047){a->HeapFree(a->GetProcessHeap(),0,storage);return 0;}
 WL_DWORD slash=0;for(WL_DWORD i=0;i<n;i++)if(path[i]=='\\'||path[i]=='/')slash=i+1;
 if(!slash||slash+wl_wlen(CONTROLLER_EXE)+1>=2048){a->HeapFree(a->GetProcessHeap(),0,storage);return 0;}
 path[slash]=0;wl_wcpy(folder,path,2048);
 wl_wcat(path,CONTROLLER_EXE,2048);
 APO_CreateProcessW create=(APO_CreateProcessW)wl_get_proc(a->kernel32,"CreateProcessW");
 if(!create){a->HeapFree(a->GetProcessHeap(),0,storage);return 0;}
 APO_STARTUPINFOW si;APO_PROCESS_INFORMATION pi;
 memset(&si,0,sizeof(si));memset(&pi,0,sizeof(pi));si.cb=sizeof(si);
 int started=create(path,0,0,0,WL_FALSE,0,0,folder,&si,&pi)?1:0;
 a->HeapFree(a->GetProcessHeap(),0,storage);
 if(!started)return 0;
 if(pi.hThread)a->CloseHandle(pi.hThread);
 if(pi.hProcess)a->CloseHandle(pi.hProcess);
 return 1;
}

/* Stock Equalizer APO constructs a Qt dialog in on_openPanelButton_clicked
   before it calls effEditOpen. A VST cannot prevent that creation. For a
   truly zero-dialog handler, see the optional scoped host source patch.
   Close the wrapper only when Windows confirms
   it is the exact plugin-titled standalone owned popup, never when embedded in
   the main Equalizer APO editor or another host-owned top-level window. This
   request is queued so it is not issued synchronously during effEditOpen. */
static void dismiss_standalone_host_panel(Plugin*p,WL_HWND panel){
 if(!panel||!p->editorReady)return;
 WL_API*a=&p->editorApi;
 APO_GetAncestor ancestor=(APO_GetAncestor)wl_get_proc(a->user32,"GetAncestor");
 APO_GetWindow getwin=(APO_GetWindow)wl_get_proc(a->user32,"GetWindow");
 APO_GetWindowLongPtrW getstyle=(APO_GetWindowLongPtrW)wl_get_proc(a->user32,"GetWindowLongPtrW");
 if(!ancestor||!getwin||!getstyle)return;
 WL_HWND root=ancestor(panel,APO_GA_ROOT);
 if(!root)return; /* never close an editor with no owned popup */
 if(!getwin(root,APO_GW_OWNER))return; /* not a host-owned popup */
 WL_UPTR style=(WL_UPTR)getstyle(root,APO_GWL_STYLE);
 if(!(style&APO_WS_POPUP)||(style&WS_CHILD))return;
 /* The stock Equalizer APO dialog is titled from the VST effect name.
    Refuse to close any other dialog, even if Windows describes it as a
    Qt-owned popup. An unknown dialog remains the host's responsibility. */
 static const WL_WCHAR expected[]={'A','P','O',' ','S','o','u','n','d','b','o','a','r','d',' ','S','c','r','a','t','c','h',0};
 WL_WCHAR title[64];title[0]=0;
 if(!a->GetWindowTextW||a->GetWindowTextW(root,title,64)!=(WL_I32)wl_wlen(expected))return;
 for(unsigned i=0;expected[i];i++)if(title[i]!=expected[i])return;
 a->PostMessageW(root,APO_WM_CLOSE,0,0);
}

static Plugin* P(AEffect* e){return (Plugin*)e->object;}
static float clampf(float x,float a,float b){return x<a?a:(x>b?b:x);}
static void acpy(char* d,const char*s,WL_U32 cap){WL_U32 i=0;if(!d||!cap)return;while(i+1<cap&&s&&s[i]){d[i]=s[i];i++;}d[i]=0;}
static void utoa3(char* d,int v){ if(v<0)v=0;if(v>999)v=999; if(v>=100){d[0]=(char)('0'+v/100);d[1]=(char)('0'+(v/10)%10);d[2]=(char)('0'+v%10);d[3]=0;} else if(v>=10){d[0]=(char)('0'+v/10);d[1]=(char)('0'+v%10);d[2]=0;} else {d[0]=(char)('0'+v);d[1]=0;} }
static int exact_write(Plugin*p,WL_HANDLE h,const void*buf,WL_U32 bytes){const WL_U8*s=(const WL_U8*)buf;WL_U32 done=0;while(done<bytes&&p->running){WL_DWORD n=0;if(!p->api.WriteFile(h,s+done,bytes-done,&n,0)||n==0)return 0;done+=n;}return done==bytes;}
static int exact_read(Plugin*p,WL_HANDLE h,void*buf,WL_U32 bytes){WL_U8*d=(WL_U8*)buf;WL_U32 done=0;while(done<bytes&&p->running){WL_DWORD n=0;if(!p->api.ReadFile(h,d+done,bytes-done,&n,0)||n==0)return 0;done+=n;}return done==bytes;}
static WL_U32 r7_target_base(Plugin*p){WL_U32 sr=p->sampleRate?p->sampleRate:48000,b=p->blockSize;WL_U32 n=(sr+99)/100;if(b>TARGET_MAX/2)n=TARGET_MAX;else if(b&&b*2>n)n=b*2;if(n<256)n=256;if(n>TARGET_MAX)n=TARGET_MAX;return n;}
static WL_U32 choose_request_frames(Plugin*p){
 WL_U32 b=__atomic_load_n(&p->blockSize,__ATOMIC_RELAXED);
 WL_U32 n=b<=64?128u:(b<=128?256u:(b<=256?512u:1024u));
 if(n>PIPE_CHUNK_MAX)n=PIPE_CHUNK_MAX;
 if(n<64)n=64;
 return n;
}
/* The producer may reset only while the audio consumer is outside its block.
   The real-time callback never waits; during a reset it passes mic through. */
static void reset_ring_idle(Plugin*p,int idle){
 while(__atomic_exchange_n(&p->ringResetLock,1,__ATOMIC_ACQUIRE))p->api.Sleep(0);
 __atomic_store_n(&p->rpos,0,__ATOMIC_RELEASE);
 __atomic_store_n(&p->wpos,0,__ATOMIC_RELEASE);
 __atomic_store_n(&p->sourceIdle,idle,__ATOMIC_RELEASE);
 __atomic_store_n(&p->boardIdle,idle,__ATOMIC_RELEASE);
 __atomic_store_n(&p->streamReady,idle?1:0,__ATOMIC_RELEASE);
 __atomic_store_n(&p->ringResetLock,0,__ATOMIC_RELEASE);
}
static WL_DWORD WL_CALLBACK pipe_thread(void* ctx){
 Plugin*p=(Plugin*)ctx;
 float* temp=(float*)p->api.HeapAlloc(p->api.GetProcessHeap(),0,PIPE_CHUNK_MAX*2*sizeof(float));
 if(!temp){p->running=0;return 0;}
 while(p->running){
  if(!p->api.WaitNamedPipeW(PIPE_NAME,250)){p->api.Sleep(100);continue;}
  WL_HANDLE h=p->api.CreateFileW(PIPE_NAME,GENERIC_READ|GENERIC_WRITE,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
  if(h==WL_INVALID_HANDLE_VALUE){p->api.Sleep(100);continue;}
  p->pipe=h;
  reset_ring_idle(p,0);
  __atomic_store_n(&p->targetFill,r7_target_base(p),__ATOMIC_RELEASE);
  __atomic_store_n(&p->protocolReady,0,__ATOMIC_RELEASE);
  PipeHello hello={M_HELLO,PROTOCOL_VERSION,BUILD_VERSION,p->sampleRate?p->sampleRate:48000,2};
  if(!exact_write(p,h,&hello,sizeof(hello))){p->api.CloseHandle(h);p->pipe=0;continue;}
  PipeAck ack;
  if(!exact_read(p,h,&ack,sizeof(ack))||ack.magic!=M_ACK||ack.version!=PROTOCOL_VERSION||ack.build!=BUILD_VERSION){p->api.CloseHandle(h);p->pipe=0;p->api.Sleep(250);continue;}
  __atomic_store_n(&p->protocolReady,1,__ATOMIC_RELEASE);
  WL_U32 idlePolls=0,epoch=__atomic_load_n(&p->rateEpoch,__ATOMIC_ACQUIRE);__atomic_store_n(&p->ringEpoch,epoch,__ATOMIC_RELEASE);
  while(p->running){
   WL_U32 nextEpoch=__atomic_load_n(&p->rateEpoch,__ATOMIC_ACQUIRE);if(nextEpoch!=epoch){reset_ring_idle(p,0);epoch=nextEpoch;__atomic_store_n(&p->ringEpoch,epoch,__ATOMIC_RELEASE);idlePolls=0;__atomic_store_n(&p->stableFrames,0,__ATOMIC_RELEASE);__atomic_store_n(&p->targetFill,r7_target_base(p),__ATOMIC_RELEASE);}
   WL_U32 r=__atomic_load_n(&p->rpos,__ATOMIC_ACQUIRE),w=__atomic_load_n(&p->wpos,__ATOMIC_ACQUIRE),avail=w-r;
   WL_U32 target=__atomic_load_n(&p->targetFill,__ATOMIC_RELAXED);
   int srcIdle=__atomic_load_n(&p->sourceIdle,__ATOMIC_ACQUIRE);
   if(srcIdle&&avail==0){__atomic_store_n(&p->boardIdle,1,__ATOMIC_RELEASE);__atomic_store_n(&p->streamReady,1,__ATOMIC_RELEASE);p->api.Sleep(idlePolls<125?2:4);}
   else if(avail>=target){p->api.Sleep(2);continue;}
   WL_U32 req=choose_request_frames(p),freef=RING_FRAMES-avail;
   if(freef<req){if(freef<64){p->api.Sleep(1);continue;}req=freef;}
   WL_U32 peak=__atomic_exchange_n(&p->peakMilli,0,__ATOMIC_ACQ_REL);
   PipeRequest rq={M_REQ,req,__atomic_load_n(&p->sampleRate,__ATOMIC_ACQUIRE),p->blockSize,avail,target,__atomic_load_n(&p->underruns,__ATOMIC_RELAXED),__atomic_load_n(&p->clipCount,__ATOMIC_RELAXED),peak};
   if(!exact_write(p,h,&rq,sizeof(rq)))break;
   PipeAudio ah;if(!exact_read(p,h,&ah,sizeof(ah)))break;
   if(ah.magic!=M_AUDIO||ah.frames>req||(ah.channels!=1&&ah.channels!=2))break;
   if((ah.flags&PIPE_AUDIO_IDLE)&&ah.frames==0){idlePolls++;__atomic_store_n(&p->sourceIdle,1,__ATOMIC_RELEASE);if(avail==0)__atomic_store_n(&p->boardIdle,1,__ATOMIC_RELEASE);continue;}
   if(ah.frames==0)break;
   WL_U32 vals=ah.frames*ah.channels;if(!exact_read(p,h,temp,vals*sizeof(float)))break;
   if(epoch!=__atomic_load_n(&p->rateEpoch,__ATOMIC_ACQUIRE))continue;
   idlePolls=0;__atomic_store_n(&p->sourceIdle,0,__ATOMIC_RELEASE);__atomic_store_n(&p->boardIdle,0,__ATOMIC_RELEASE);
   w=__atomic_load_n(&p->wpos,__ATOMIC_RELAXED);
   for(WL_U32 i=0;i<ah.frames;i++){WL_U32 idx=(w+i)&RING_MASK;float l=temp[i*ah.channels],rr=ah.channels==2?temp[i*2+1]:l;p->ring[idx*2]=l;p->ring[idx*2+1]=rr;}
   __atomic_store_n(&p->wpos,w+ah.frames,__ATOMIC_RELEASE);__atomic_store_n(&p->streamReady,1,__ATOMIC_RELEASE);
  }
  p->api.CloseHandle(h);p->pipe=0;__atomic_store_n(&p->protocolReady,0,__ATOMIC_RELEASE);reset_ring_idle(p,1);__atomic_store_n(&p->targetFill,TARGET_BASE,__ATOMIC_RELEASE);__atomic_store_n(&p->stableFrames,0,__ATOMIC_RELEASE);p->api.Sleep(50);
 }
 p->api.HeapFree(p->api.GetProcessHeap(),0,temp);return 0;
}
static int ensure_api(Plugin*p){if(p->apiReady)return 1;if(!wl_init_kernel(&p->api))return 0;p->apiReady=1;return 1;}
static void start_stream(Plugin*p){if(!ensure_api(p))return;
 /* A timed-out previous shutdown must not permit another worker to overwrite
    its thread handle or race against the same VST instance. */
 if(p->thread){WL_DWORD wr=p->api.WaitForSingleObject(p->thread,0);if(wr!=0)return;p->api.CloseHandle(p->thread);p->thread=0;}
 WL_I32 expected=0;if(!__atomic_compare_exchange_n(&p->running,&expected,1,0,__ATOMIC_ACQ_REL,__ATOMIC_ACQUIRE))return;reset_ring_idle(p,0);__atomic_store_n(&p->targetFill,TARGET_BASE,__ATOMIC_RELEASE);__atomic_store_n(&p->stableFrames,0,__ATOMIC_RELEASE);__atomic_store_n(&p->streamReady,0,__ATOMIC_RELEASE);__atomic_store_n(&p->sourceIdle,0,__ATOMIC_RELEASE);__atomic_store_n(&p->boardIdle,0,__ATOMIC_RELEASE);__atomic_store_n(&p->protocolReady,0,__ATOMIC_RELEASE);p->thread=p->api.CreateThread(0,0,pipe_thread,p,0,0);if(!p->thread)p->running=0;}
static int stop_stream(Plugin*p){if(!p->apiReady)return 1;WL_I32 wasRunning=__atomic_exchange_n(&p->running,0,__ATOMIC_ACQ_REL);WL_HANDLE h=(WL_HANDLE)p->pipe;if(wasRunning&&h)p->api.CancelIoEx(h,0);if(p->thread){WL_DWORD wr=p->api.WaitForSingleObject(p->thread,5000);if(wr==WAIT_TIMEOUT)return 0;p->api.CloseHandle(p->thread);p->thread=0;}p->pipe=0;return 1;}

static void peak_update(Plugin*p,WL_U32 peak){WL_U32 old=__atomic_load_n(&p->peakMilli,__ATOMIC_RELAXED);while(peak>old&&!__atomic_compare_exchange_n(&p->peakMilli,&old,peak,1,__ATOMIC_RELEASE,__ATOMIC_RELAXED)){} }
/* One linear stereo gain envelope protects the FINAL mic+board mix.
   This avoids cascading nonlinear saturators on the scratch signal. The gain
   releases gradually after a loud transient; it attacks immediately so peak
   excursions cannot hard clip in the host. */
static float r6_limiter_gain(Plugin*p,float magnitude,float release){
 float needed=magnitude>0.94f?0.94f/magnitude:1.0f;
 if(needed<p->limiterGain)p->limiterGain=needed;
 else{
  p->limiterGain+=(needed-p->limiterGain)*release;
 }
 if(p->limiterGain>1.0f)p->limiterGain=1.0f;
 if(p->limiterGain<0.0f)p->limiterGain=0.0f;
 return p->limiterGain;
}
static float output_limiter_gain(Plugin*p,float magnitude){return r6_limiter_gain(p,magnitude,1.0f/(float)((p->sampleRate?p->sampleRate:48000)*0.11f));}
static float flush_tiny(float x){return (x>-1.0e-20f&&x<1.0e-20f)?0.0f:x;}
static void WL_CALLBACK process_replacing(AEffect*e,float**in,float**out,WL_I32 frames){
 Plugin*p=P(e);if(frames<=0)return;float mic=p->params[1]*2.0f,board=p->params[0]*2.0f;int limit=p->params[2]>=0.5f;
 int locked=!__atomic_exchange_n(&p->ringResetLock,1,__ATOMIC_ACQUIRE);
 WL_U32 r=__atomic_load_n(&p->rpos,__ATOMIC_RELAXED),w=__atomic_load_n(&p->wpos,__ATOMIC_ACQUIRE),avail=w-r,peak=0,clipped=0;
 int idle=!locked||__atomic_load_n(&p->ringEpoch,__ATOMIC_ACQUIRE)!=__atomic_load_n(&p->rateEpoch,__ATOMIC_ACQUIRE)||__atomic_load_n(&p->boardIdle,__ATOMIC_ACQUIRE);if(avail>RING_FRAMES){avail=0;idle=1;}
 if(!idle&&__atomic_load_n(&p->streamReady,__ATOMIC_ACQUIRE)&&frames>0){
  if(avail<(WL_U32)frames){__atomic_add_fetch(&p->underruns,1,__ATOMIC_RELAXED);WL_U32 target=__atomic_load_n(&p->targetFill,__ATOMIC_RELAXED);if(target<TARGET_MAX){target+=TARGET_STEP;if(target>TARGET_MAX)target=TARGET_MAX;__atomic_store_n(&p->targetFill,target,__ATOMIC_RELAXED);}__atomic_store_n(&p->stableFrames,0,__ATOMIC_RELAXED);}
  else{WL_U64 stable=__atomic_add_fetch(&p->stableFrames,(WL_U64)frames,__ATOMIC_RELAXED);WL_U64 threshold=(WL_U64)(p->sampleRate?p->sampleRate:48000)*10ull;if(stable>=threshold){WL_U32 target=__atomic_load_n(&p->targetFill,__ATOMIC_RELAXED);WL_U32 base=r7_target_base(p);if(target!=base){target=target>base+TARGET_STEP?target-TARGET_STEP:base;__atomic_store_n(&p->targetFill,target,__ATOMIC_RELAXED);}__atomic_store_n(&p->stableFrames,0,__ATOMIC_RELAXED);}}
 }
 /* Optional block-peak envelope anticipates peaks in the current host block
    without delaying either microphone or soundboard. Original mode is default. */
 float blockPeak=0,release=1.0f/(float)((p->sampleRate?p->sampleRate:48000)*0.11f);int blockMode=limit&&p->params[3]>=.5f;
 if(blockMode){for(WL_I32 i=0;i<frames;i++){float a=(in&&in[0]?in[0][i]:0)*mic,b=(in&&in[1]?in[1][i]:0)*mic;if(!idle&&(WL_U32)i<avail){WL_U32 k=(r+(WL_U32)i)&RING_MASK;a+=p->ring[k*2]*board;b+=p->ring[k*2+1]*board;}float x=a<0?-a:a,y=b<0?-b:b;if(x>blockPeak)blockPeak=x;if(y>blockPeak)blockPeak=y;}}
 for(WL_I32 i=0;i<frames;i++){
  float s0=0,s1=0;if(!idle&&(WL_U32)i<avail){WL_U32 idx=(r+(WL_U32)i)&RING_MASK;s0=p->ring[idx*2];s1=p->ring[idx*2+1];}
  float a=flush_tiny((in&&in[0]?in[0][i]:0)*mic+s0*board),b=flush_tiny((in&&in[1]?in[1][i]:0)*mic+s1*board);
  float aa=a<0?-a:a,bb=b<0?-b:b,m=aa>bb?aa:bb;WL_U32 pm=(WL_U32)(m*1000.0f+0.5f);if(pm>peak)peak=pm;if(m>1.0f)clipped++;
  if(limit){float gain=r6_limiter_gain(p,blockMode?blockPeak:m,release);a*=gain;b*=gain;}else p->limiterGain=1.0f;if(out&&out[0])out[0][i]=a;if(out&&out[1])out[1][i]=b;
 }
 if(peak>9999u)peak=9999u;peak_update(p,peak);if(clipped)__atomic_add_fetch(&p->clipCount,clipped,__ATOMIC_RELAXED);
 if(locked){WL_U32 used=!idle?((WL_U32)frames<avail?(WL_U32)frames:avail):0;__atomic_store_n(&p->rpos,r+used,__ATOMIC_RELEASE);
 if(__atomic_load_n(&p->sourceIdle,__ATOMIC_ACQUIRE)&&r+used==w)__atomic_store_n(&p->boardIdle,1,__ATOMIC_RELEASE);__atomic_store_n(&p->ringResetLock,0,__ATOMIC_RELEASE);}
}
static void WL_CALLBACK process_accum(AEffect*e,float**in,float**out,WL_I32 frames){process_replacing(e,in,out,frames);}
static void WL_CALLBACK set_param(AEffect*e,WL_I32 index,float v){Plugin*p=P(e);if(index>=0&&index<4)p->params[index]=clampf(v,0,1);}
static float WL_CALLBACK get_param(AEffect*e,WL_I32 index){Plugin*p=P(e);return (index>=0&&index<4)?p->params[index]:0;}
static WL_IPTR WL_CALLBACK dispatch(AEffect*e,WL_I32 op,WL_I32 index,WL_IPTR value,void*ptr,float opt){Plugin*p=P(e);switch(op){case effOpen:return 0;case effClose:{if(stop_stream(p)){if(ensure_api(p))p->api.HeapFree(p->api.GetProcessHeap(),0,p);}return 0;}case effSetSampleRate:{WL_U32 rate=(WL_U32)(opt>=8000&&opt<=384000?opt:48000);if(__atomic_exchange_n(&p->sampleRate,rate,__ATOMIC_ACQ_REL)!=rate)__atomic_add_fetch(&p->rateEpoch,1,__ATOMIC_ACQ_REL);return 0;}case effSetBlockSize:p->blockSize=value>0?(WL_U32)value:0;return 0;case effMainsChanged:if(value)start_stream(p);else (void)stop_stream(p);return 0;case effEditGetRect:if(ptr){*(VstRect**)ptr=&p->editorRect;return 1;}return 0;case effEditOpen:{
 /* R8 direct-host contract: NULL parent is intentional, with no editor wrapper. */
 if(!p->editorReady){if(!apo_init_editor_api(&p->editorApi))return 0;p->editorReady=1;}
 if(!launch_controller(p)){
  /* Failure is uncommon and must not silently make the Open Panel button
     appear broken. This error dialog is only shown when launch fails. */
  p->editorApi.MessageBoxW((WL_HWND)ptr,(const WL_WCHAR[]){'C','o','u','l','d',' ','n','o','t',' ','s','t','a','r','t',' ','t','h','e',' ','s','o','u','n','d','b','o','a','r','d','.',13,10,'C','h','e','c','k',' ','t','h','e',' ','c','o','n','t','r','o','l','l','e','r',' ','E','X','E',' ','a','n','d',' ','W','i','n','d','o','w','s',' ','p','e','r','m','i','s','s','i','o','n','s','.',0},(const WL_WCHAR[]){'A','P','O',' ','S','o','u','n','d','b','o','a','r','d',0},0x10);
  return 0;
 }
 if(ptr) dismiss_standalone_host_panel(p,(WL_HWND)ptr);
 return 1;
}case effEditClose:return 1;case effEditIdle:return 1;case effStartProcess:start_stream(p);return 1;case effStopProcess:(void)stop_stream(p);return 1;case effGetParamName:if(ptr){acpy((char*)ptr,index==0?"Board":index==1?"Mic":index==2?"Limiter":index==3?"PeakMode":"",32);}return 1;case effGetParamLabel:if(ptr)acpy((char*)ptr,index<2?"%":"",16);return 1;case effGetParamDisplay:if(ptr){if(index<2)utoa3((char*)ptr,(int)(p->params[index]*200.0f+0.5f));else if(index==2)acpy((char*)ptr,p->params[2]>=0.5f?"On":"Off",16);else if(index==3)acpy((char*)ptr,p->params[3]>=0.5f?"Block":"Classic",16);}return 1;case effGetEffectName:if(ptr)acpy((char*)ptr,"APO Soundboard Scratch",64);return 1;case effGetVendorString:if(ptr)acpy((char*)ptr,"Syed1611",64);return 1;case effGetProductString:if(ptr)acpy((char*)ptr,"APO Soundboard Scratch",64);return 1;case effGetVendorVersion:return BUILD_VERSION;case effGetVstVersion:return 2400;case effGetPlugCategory:return 1;case effCanDo:return 0;default:return 0;}}

__declspec(dllexport) AEffect* WL_CALLBACK VSTPluginMain(audioMasterCallback host){WL_API api;if(!wl_init_kernel(&api))return 0;Plugin*p=(Plugin*)api.HeapAlloc(api.GetProcessHeap(),0,sizeof(Plugin));if(!p)return 0;memset(p,0,sizeof(*p));p->api=api;p->apiReady=1;p->host=host;p->sampleRate=48000;p->blockSize=0;p->targetFill=TARGET_BASE;p->params[0]=0.5f;p->params[1]=0.5f;p->params[2]=1.0f;p->limiterGain=1.0f;p->editorRect.top=0;p->editorRect.left=0;p->editorRect.bottom=1;p->editorRect.right=1;AEffect*e=&p->effect;e->magic=VST_MAGIC;e->dispatcher=dispatch;e->process=process_accum;e->setParameter=set_param;e->getParameter=get_param;e->numPrograms=1;e->numParams=4;e->numInputs=2;e->numOutputs=2;e->flags=EFF_HAS_EDITOR|EFF_CAN_REPLACING;e->object=p;e->uniqueID=PLUGIN_ID;e->version=BUILD_VERSION;e->processReplacing=process_replacing;return e;}
WL_BOOL WL_CALLBACK DllMain(void*h,WL_DWORD reason,void*reserved){(void)reserved;if(reason==1)g_dll_module=(WL_HMODULE)h;return WL_TRUE;}
