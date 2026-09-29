#include "winlite.h"

#define VST_MAGIC ((WL_I32)0x56737450) /* 'VstP' */
#define FOURCC(a,b,c,d) ((WL_I32)(((WL_U32)(a)<<24)|((WL_U32)(b)<<16)|((WL_U32)(c)<<8)|(WL_U32)(d)))
#define PLUGIN_ID FOURCC('A','P','S','B')
#define EFF_HAS_EDITOR (1<<0)
#define EFF_CAN_REPLACING (1<<4)
#define WS_CHILD 0x40000000u
#define WS_VISIBLE 0x10000000u
#define SS_LEFT 0x00000000u
#define SW_SHOWNORMAL 1
#define PIPE_CHUNK 512u
#define RING_FRAMES 4096u
#define RING_MASK (RING_FRAMES-1u)
#define TARGET_FILL 1024u
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
typedef struct { WL_U32 magic,version,sampleRate,channels; } PipeHello;
typedef struct { WL_U32 magic,frames; } PipeRequest;
typedef struct { WL_U32 magic,frames,channels; } PipeAudio;
#pragma pack(pop)
#define M_HELLO 0x31425341u
#define M_REQ   0x51525341u
#define M_AUDIO 0x44525341u
static const WL_WCHAR PIPE_NAME[]={ '\\','\\','.','\\','p','i','p','e','\\','A','P','O','S','o','u','n','d','b','o','a','r','d','_','v','1',0 };

typedef struct {
 AEffect effect;
 audioMasterCallback host;
 WL_API api;
 volatile WL_I32 apiReady;
 volatile WL_I32 running;
 WL_HANDLE thread;
 volatile WL_HANDLE pipe;
 volatile WL_U32 sampleRate;
 volatile WL_U32 rpos,wpos;
 float ring[RING_FRAMES*2];
 float params[3];
 VstRect editorRect;
 WL_HWND editorChild;
 WL_API editorApi;
 WL_I32 editorReady;
} Plugin;


static WL_HMODULE g_dll_module=0;
static const WL_WCHAR CONTROLLER_EXE[]={ 'A','P','O','S','o','u','n','d','b','o','a','r','d','C','o','n','t','r','o','l','l','e','r','.','e','x','e',0 };
static const WL_WCHAR STATIC_CLASS[]={ 'S','T','A','T','I','C',0 };
static const WL_WCHAR PANEL_TEXT[]={ 'A','P','O',' ','S','o','u','n','d','b','o','a','r','d',' ','i','s',' ','l','o','a','d','e','d','.', '\r','\n','O','p','e','n',' ','P','a','n','e','l',' ','l','a','u','n','c','h','e','s',' ','t','h','e',' ','c','o','n','t','r','o','l','l','e','r',' ','f','o','r',' ','p','a','d','s',' ','a','n','d',' ','g','l','o','b','a','l',' ','h','o','t','k','e','y','s','.', '\r','\n','Y','o','u',' ','m','a','y',' ','c','l','o','s','e',' ','t','h','i','s',' ','s','m','a','l','l',' ','p','a','n','e','l',';',' ','t','h','e',' ','c','o','n','t','r','o','l','l','e','r',' ','k','e','e','p','s',' ','r','u','n','n','i','n','g','.',0 };
static const WL_WCHAR OPEN_VERB[]={ 'o','p','e','n',0 };

static void launch_controller(Plugin*p){
 WL_API*a=&p->editorApi;if(!p->editorReady||!a->ShellExecuteW||!g_dll_module)return;
 WL_WCHAR path[520];WL_DWORD n=a->GetModuleFileNameW(g_dll_module,path,520);if(!n||n>=519)return;
 WL_DWORD slash=0;for(WL_DWORD i=0;i<n;i++)if(path[i]=='\\'||path[i]=='/')slash=i+1;
 if(!slash)return;path[slash]=0;wl_wcat(path,CONTROLLER_EXE,520);
 a->ShellExecuteW(0,OPEN_VERB,path,0,0,SW_SHOWNORMAL);
}

static Plugin* P(AEffect* e){return (Plugin*)e->object;}
static float clampf(float x,float a,float b){return x<a?a:(x>b?b:x);}
static void acpy(char* d,const char*s,WL_U32 cap){WL_U32 i=0;if(!d||!cap)return;while(i+1<cap&&s&&s[i]){d[i]=s[i];i++;}d[i]=0;}
static void utoa3(char* d,int v){ if(v<0)v=0;if(v>999)v=999; if(v>=100){d[0]=(char)('0'+v/100);d[1]=(char)('0'+(v/10)%10);d[2]=(char)('0'+v%10);d[3]=0;} else if(v>=10){d[0]=(char)('0'+v/10);d[1]=(char)('0'+v%10);d[2]=0;} else {d[0]=(char)('0'+v);d[1]=0;} }
static int exact_write(Plugin*p,WL_HANDLE h,const void*buf,WL_U32 bytes){const WL_U8*s=(const WL_U8*)buf;WL_U32 done=0;while(done<bytes&&p->running){WL_DWORD n=0;if(!p->api.WriteFile(h,s+done,bytes-done,&n,0)||n==0)return 0;done+=n;}return done==bytes;}
static int exact_read(Plugin*p,WL_HANDLE h,void*buf,WL_U32 bytes){WL_U8*d=(WL_U8*)buf;WL_U32 done=0;while(done<bytes&&p->running){WL_DWORD n=0;if(!p->api.ReadFile(h,d+done,bytes-done,&n,0)||n==0)return 0;done+=n;}return done==bytes;}
static WL_DWORD WL_CALLBACK pipe_thread(void* ctx){Plugin*p=(Plugin*)ctx;float* temp=(float*)p->api.HeapAlloc(p->api.GetProcessHeap(),0,PIPE_CHUNK*2*sizeof(float));if(!temp){p->running=0;return 0;}
 while(p->running){
  if(!p->api.WaitNamedPipeW(PIPE_NAME,250)){p->api.Sleep(100);continue;}
  WL_HANDLE h=p->api.CreateFileW(PIPE_NAME,GENERIC_READ|GENERIC_WRITE,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);if(h==WL_INVALID_HANDLE_VALUE){p->api.Sleep(100);continue;}p->pipe=h;
  PipeHello hello={M_HELLO,1,p->sampleRate?p->sampleRate:48000,2}; if(!exact_write(p,h,&hello,sizeof(hello))){p->api.CloseHandle(h);p->pipe=0;continue;}
  while(p->running){WL_U32 r=__atomic_load_n(&p->rpos,__ATOMIC_ACQUIRE),w=__atomic_load_n(&p->wpos,__ATOMIC_ACQUIRE);WL_U32 avail=w-r;if(avail>=TARGET_FILL){p->api.Sleep(2);continue;}WL_U32 freef=RING_FRAMES-avail;if(freef<PIPE_CHUNK){p->api.Sleep(1);continue;}
   PipeRequest rq={M_REQ,PIPE_CHUNK}; if(!exact_write(p,h,&rq,sizeof(rq)))break;PipeAudio ah;if(!exact_read(p,h,&ah,sizeof(ah)))break;if(ah.magic!=M_AUDIO||ah.frames==0||ah.frames>PIPE_CHUNK||(ah.channels!=1&&ah.channels!=2))break;WL_U32 vals=ah.frames*ah.channels;if(!exact_read(p,h,temp,vals*sizeof(float)))break;
   w=__atomic_load_n(&p->wpos,__ATOMIC_RELAXED);for(WL_U32 i=0;i<ah.frames;i++){WL_U32 idx=(w+i)&RING_MASK;float l=temp[i*ah.channels],rr=ah.channels==2?temp[i*2+1]:l;p->ring[idx*2]=l;p->ring[idx*2+1]=rr;}__atomic_store_n(&p->wpos,w+ah.frames,__ATOMIC_RELEASE);
  }
  p->api.CloseHandle(h);p->pipe=0;__atomic_store_n(&p->rpos,0,__ATOMIC_RELEASE);__atomic_store_n(&p->wpos,0,__ATOMIC_RELEASE);p->api.Sleep(50);
 }
 p->api.HeapFree(p->api.GetProcessHeap(),0,temp);return 0;}
static int ensure_api(Plugin*p){if(p->apiReady)return 1;if(!wl_init_kernel(&p->api))return 0;p->apiReady=1;return 1;}
static void start_stream(Plugin*p){if(!ensure_api(p))return;WL_I32 expected=0;if(!__atomic_compare_exchange_n(&p->running,&expected,1,0,__ATOMIC_ACQ_REL,__ATOMIC_ACQUIRE))return;__atomic_store_n(&p->rpos,0,__ATOMIC_RELEASE);__atomic_store_n(&p->wpos,0,__ATOMIC_RELEASE);p->thread=p->api.CreateThread(0,0,pipe_thread,p,0,0);if(!p->thread)p->running=0;}
static void stop_stream(Plugin*p){if(!p->apiReady)return;if(!__atomic_exchange_n(&p->running,0,__ATOMIC_ACQ_REL))return;WL_HANDLE h=(WL_HANDLE)p->pipe;if(h)p->api.CancelIoEx(h,0);if(p->thread){p->api.WaitForSingleObject(p->thread,1500);p->api.CloseHandle(p->thread);p->thread=0;}p->pipe=0;}

static void WL_CALLBACK process_replacing(AEffect*e,float**in,float**out,WL_I32 frames){Plugin*p=P(e);float mic=p->params[1]*2.0f,board=p->params[0]*2.0f;int limit=p->params[2]>=0.5f;WL_U32 r=__atomic_load_n(&p->rpos,__ATOMIC_RELAXED),w=__atomic_load_n(&p->wpos,__ATOMIC_ACQUIRE);WL_U32 avail=w-r;
 for(WL_I32 i=0;i<frames;i++){float s0=0,s1=0;if((WL_U32)i<avail){WL_U32 idx=(r+(WL_U32)i)&RING_MASK;s0=p->ring[idx*2];s1=p->ring[idx*2+1];}float a=(in&&in[0]?in[0][i]:0)*mic+s0*board;float b=(in&&in[1]?in[1][i]:0)*mic+s1*board;if(limit){a=clampf(a,-1,1);b=clampf(b,-1,1);}if(out&&out[0])out[0][i]=a;if(out&&out[1])out[1][i]=b;}
 WL_U32 used=(WL_U32)frames<avail?(WL_U32)frames:avail;__atomic_store_n(&p->rpos,r+used,__ATOMIC_RELEASE);
}
static void WL_CALLBACK process_accum(AEffect*e,float**in,float**out,WL_I32 frames){process_replacing(e,in,out,frames);}
static void WL_CALLBACK set_param(AEffect*e,WL_I32 index,float v){Plugin*p=P(e);if(index>=0&&index<3)p->params[index]=clampf(v,0,1);}
static float WL_CALLBACK get_param(AEffect*e,WL_I32 index){Plugin*p=P(e);return (index>=0&&index<3)?p->params[index]:0;}
static WL_IPTR WL_CALLBACK dispatch(AEffect*e,WL_I32 op,WL_I32 index,WL_IPTR value,void*ptr,float opt){Plugin*p=P(e);switch(op){case effOpen:return 0;case effClose:{if(p->editorChild&&p->editorReady&&p->editorApi.DestroyWindow){p->editorApi.DestroyWindow(p->editorChild);p->editorChild=0;}stop_stream(p);if(ensure_api(p))p->api.HeapFree(p->api.GetProcessHeap(),0,p);return 0;}case effSetSampleRate:p->sampleRate=(WL_U32)(opt>8000?opt:48000);return 0;case effSetBlockSize:return 0;case effMainsChanged:if(value)start_stream(p);else stop_stream(p);return 0;case effEditGetRect:if(ptr){*(VstRect**)ptr=&p->editorRect;return 1;}return 0;case effEditOpen:{if(!ptr)return 0;if(!p->editorReady){if(!wl_init_gui(&p->editorApi))return 0;p->editorReady=1;}if(p->editorChild){p->editorApi.DestroyWindow(p->editorChild);p->editorChild=0;}p->editorChild=p->editorApi.CreateWindowExW(0,STATIC_CLASS,PANEL_TEXT,WS_CHILD|WS_VISIBLE|SS_LEFT,12,12,496,80,(WL_HWND)ptr,0,(WL_HINSTANCE)p->editorApi.GetModuleHandleW(0),0);launch_controller(p);return 1;}case effEditClose:if(p->editorChild&&p->editorReady&&p->editorApi.DestroyWindow){p->editorApi.DestroyWindow(p->editorChild);p->editorChild=0;}return 1;case effEditIdle:return 1;case effStartProcess:start_stream(p);return 1;case effStopProcess:stop_stream(p);return 1;case effGetParamName:if(ptr){acpy((char*)ptr,index==0?"Board":index==1?"Mic":index==2?"Limiter":"",32);}return 1;case effGetParamLabel:if(ptr)acpy((char*)ptr,index<2?"%":"",16);return 1;case effGetParamDisplay:if(ptr){if(index<2)utoa3((char*)ptr,(int)(p->params[index]*200.0f+0.5f));else acpy((char*)ptr,p->params[2]>=0.5f?"On":"Off",16);}return 1;case effGetEffectName:if(ptr)acpy((char*)ptr,"APO Soundboard",64);return 1;case effGetVendorString:if(ptr)acpy((char*)ptr,"OpenAI Build",64);return 1;case effGetProductString:if(ptr)acpy((char*)ptr,"APO Soundboard",64);return 1;case effGetVendorVersion:return 60000;case effGetVstVersion:return 2400;case effGetPlugCategory:return 1;case effCanDo:return 0;default:return 0;}}

__declspec(dllexport) AEffect* WL_CALLBACK VSTPluginMain(audioMasterCallback host){WL_API api;if(!wl_init_kernel(&api))return 0;Plugin*p=(Plugin*)api.HeapAlloc(api.GetProcessHeap(),0,sizeof(Plugin));if(!p)return 0;memset(p,0,sizeof(*p));p->api=api;p->apiReady=1;p->host=host;p->sampleRate=48000;p->params[0]=0.5f;p->params[1]=0.5f;p->params[2]=1.0f;p->editorRect.top=0;p->editorRect.left=0;p->editorRect.bottom=104;p->editorRect.right=520;AEffect*e=&p->effect;e->magic=VST_MAGIC;e->dispatcher=dispatch;e->process=process_accum;e->setParameter=set_param;e->getParameter=get_param;e->numPrograms=1;e->numParams=3;e->numInputs=2;e->numOutputs=2;e->flags=EFF_HAS_EDITOR|EFF_CAN_REPLACING;e->object=p;e->uniqueID=PLUGIN_ID;e->version=60000;e->processReplacing=process_replacing;return e;}
WL_BOOL WL_CALLBACK DllMain(void*h,WL_DWORD reason,void*reserved){(void)reserved;if(reason==1)g_dll_module=(WL_HMODULE)h;return WL_TRUE;}