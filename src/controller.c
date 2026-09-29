#include "winlite.h"

/* APO Soundboard Controller v0.4
   v0.2 architecture retained: standalone controller + VST audio bridge.
   Custom GDI UI, pad selection/double-click playback, volume/pitch knobs,
   waveform region playback, Media Foundation decoding and minimize-to-tray. */

#define PAD_COUNT 16
#define MAX_PATH_W 520
#define MAX_VOICES 32
#define WAVE_BINS 1024
#define HOTKEY_BASE 1000

#define WM_CREATE 0x0001u
#define WM_DESTROY 0x0002u
#define WM_SIZE 0x0005u
#define WM_PAINT 0x000Fu
#define WM_CLOSE 0x0010u
#define WM_ERASEBKGND 0x0014u
#define WM_KEYDOWN 0x0100u
#define WM_SYSKEYDOWN 0x0104u
#define WM_HOTKEY 0x0312u
#define WM_MOUSEMOVE 0x0200u
#define WM_LBUTTONDOWN 0x0201u
#define WM_LBUTTONUP 0x0202u
#define WM_LBUTTONDBLCLK 0x0203u
#define WM_MOUSEWHEEL 0x020Au
#define WM_APP 0x8000u
#define WM_TRAY (WM_APP+21u)
#define WM_RESTOREAPP (WM_APP+22u)

#define CS_VREDRAW 0x0001u
#define CS_HREDRAW 0x0002u
#define CS_DBLCLKS 0x0008u
#define WS_OVERLAPPEDWINDOW 0x00CF0000u
#define SW_HIDE 0
#define SW_SHOW 5
#define SW_RESTORE 9
#define SIZE_MINIMIZED 1
#define COLOR_WINDOW 5

#define DT_LEFT 0x0000u
#define DT_CENTER 0x0001u
#define DT_RIGHT 0x0002u
#define DT_VCENTER 0x0004u
#define DT_SINGLELINE 0x0020u
#define DT_END_ELLIPSIS 0x00008000u
#define DT_NOPREFIX 0x00000800u
#define TRANSPARENT 1
#define PS_SOLID 0

#define MOD_ALT 0x0001u
#define MOD_CONTROL 0x0002u
#define MOD_SHIFT 0x0004u
#define MOD_WIN 0x0008u
#define MOD_NOREPEAT 0x4000u
#define VK_SHIFT 0x10u
#define VK_CONTROL 0x11u
#define VK_MENU 0x12u
#define VK_ESCAPE 0x1Bu
#define VK_SPACE 0x20u
#define VK_LWIN 0x5Bu
#define VK_RWIN 0x5Cu

#define OFN_NOCHANGEDIR 0x00000008u
#define OFN_PATHMUSTEXIST 0x00000800u
#define OFN_FILEMUSTEXIST 0x00001000u
#define OFN_EXPLORER 0x00080000u
#define GENERIC_READ 0x80000000u
#define GENERIC_WRITE 0x40000000u
#define OPEN_EXISTING 3u
#define CREATE_ALWAYS 2u
#define FILE_ATTRIBUTE_NORMAL 0x80u
#define PIPE_ACCESS_DUPLEX 0x00000003u
#define PIPE_TYPE_MESSAGE 0x00000004u
#define PIPE_READMODE_MESSAGE 0x00000002u
#define PIPE_WAIT 0x00000000u
#define PIPE_REJECT_REMOTE_CLIENTS 0x00000008u
#define ERROR_PIPE_CONNECTED 535u
#define SDDL_REVISION_1 1u

#define NIM_ADD 0u
#define NIM_DELETE 2u
#define NIF_MESSAGE 0x1u
#define NIF_ICON 0x2u
#define NIF_TIP 0x4u
#define IDI_APPLICATION 32512u

#define LOWORD_(x) ((WL_U16)((WL_UPTR)(x)&0xffffu))
#define HIWORD_(x) ((WL_U16)(((WL_UPTR)(x)>>16)&0xffffu))
#define GET_X_LPARAM(lp) ((WL_I32)(WL_I16)LOWORD_(lp))
#define GET_Y_LPARAM(lp) ((WL_I32)(WL_I16)HIWORD_(lp))
#define RGB_(r,g,b) ((WL_DWORD)(((WL_U8)(r))|((WL_U16)((WL_U8)(g))<<8)|(((WL_U32)(WL_U8)(b))<<16)))

#pragma pack(push,1)
typedef struct { WL_U32 magic,version,sampleRate,channels; } PipeHello;
typedef struct { WL_U32 magic,frames; } PipeRequest;
typedef struct { WL_U32 magic,frames,channels; } PipeAudio;
#pragma pack(pop)
#define M_HELLO 0x31425341u
#define M_REQ   0x51525341u
#define M_AUDIO 0x44525341u
static const WL_WCHAR PIPE_NAME[]={ '\\','\\','.','\\','p','i','p','e','\\','A','P','O','S','o','u','n','d','b','o','a','r','d','_','v','1',0 };

static WL_API g_api;
static WL_HWND g_main=0;
static volatile WL_I32 g_running=1;
static volatile WL_I32 g_connections=0;
static volatile WL_I32 g_padLock=0;
static volatile WL_I64 g_triggerSerial[PAD_COUNT];
static volatile WL_I64 g_stopSerial=0;
static WL_I32 g_selected=0;
static WL_I32 g_capturePad=-1;
static WL_I32 g_dragMode=0; /* 1 volume, 2 pitch, 3 waveform */
static WL_I32 g_dragStartY=0;
static float g_dragStartVolume=1.0f;
static WL_I32 g_dragStartPitch=0;
static WL_I32 g_waveAnchorX=0;
static WL_I32 g_waveMoved=0;
static WL_I32 g_trayShown=0;
static WL_HANDLE g_mutex=0;

static WL_HFONT g_fontTitle=0,g_fontNormal=0,g_fontSmall=0,g_fontTiny=0;
static WL_HBRUSH g_brBg=0,g_brPanel=0,g_brPad=0,g_brPadLoaded=0,g_brSelected=0,g_brAccent=0,g_brWave=0,g_brWaveSel=0,g_brKnob=0,g_brButton=0,g_brButtonHot=0;
static WL_HPEN g_penBorder=0,g_penAccent=0,g_penWave=0,g_penWaveSel=0,g_penKnob=0;
static WL_HICON g_icon=0;

static const WL_WCHAR CLASS_NAME[]={'A','P','O','S','o','u','n','d','b','o','a','r','d','C','t','r','l','V','0','4',0};
static const WL_WCHAR WINDOW_TITLE[]={'A','P','O',' ','S','o','u','n','d','b','o','a','r','d',' ','v','0','.','4',0};
static const WL_WCHAR FONT_NAME[]={'S','e','g','o','e',' ','U','I',0};

/* ---------------- basic helpers ---------------- */
static void wadd_ascii(WL_WCHAR*d,WL_SIZE_T cap,const char*s){WL_SIZE_T n=wl_wlen(d),i=0;while(n+1<cap&&s&&s[i])d[n++]=(WL_U8)s[i++];d[n]=0;}
static void wadd_num(WL_WCHAR*d,WL_SIZE_T cap,WL_I64 v){WL_WCHAR t[32];int n=0;if(v==0)t[n++]='0';else{if(v<0){wl_wcat(d,(const WL_WCHAR[]){'-',0},cap);v=-v;}while(v&&n<31){t[n++]=(WL_WCHAR)('0'+v%10);v/=10;}}while(n--&&wl_wlen(d)+1<cap){WL_SIZE_T q=wl_wlen(d);d[q]=t[n];d[q+1]=0;}}
static void wset_ascii(WL_WCHAR*d,WL_SIZE_T cap,const char*s){if(cap)d[0]=0;wadd_ascii(d,cap,s);}
static const WL_WCHAR* base_name(const WL_WCHAR*p){const WL_WCHAR*b=p;if(!p)return p;for(const WL_WCHAR*s=p;*s;s++)if(*s=='\\'||*s=='/')b=s+1;return b;}
static float fclamp(float x,float a,float b){return x<a?a:(x>b?b:x);}
static WL_I32 iclamp(WL_I32 x,WL_I32 a,WL_I32 b){return x<a?a:(x>b?b:x);}
static void spin_lock(void){while(1){WL_I32 e=0;if(__atomic_compare_exchange_n(&g_padLock,&e,1,0,__ATOMIC_ACQ_REL,__ATOMIC_ACQUIRE))return;g_api.Sleep(0);}}
static void spin_unlock(void){__atomic_store_n(&g_padLock,0,__ATOMIC_RELEASE);}
static int pt_in(WL_RECT r,int x,int y){return x>=r.left&&x<r.right&&y>=r.top&&y<r.bottom;}
static void fill(WL_HDC dc,WL_RECT r,WL_HBRUSH b){g_api.FillRect(dc,&r,b);}
static void line(WL_HDC dc,int x1,int y1,int x2,int y2,WL_HPEN p){WL_HGDIOBJ old=g_api.SelectObject(dc,(WL_HGDIOBJ)p);g_api.MoveToEx(dc,x1,y1,0);g_api.LineTo(dc,x2,y2);g_api.SelectObject(dc,old);}
static void frame(WL_HDC dc,WL_RECT r,WL_HPEN p){line(dc,r.left,r.top,r.right-1,r.top,p);line(dc,r.right-1,r.top,r.right-1,r.bottom-1,p);line(dc,r.right-1,r.bottom-1,r.left,r.bottom-1,p);line(dc,r.left,r.bottom-1,r.left,r.top,p);}
static WL_U16 rd16(const WL_U8*p){return (WL_U16)(p[0]|((WL_U16)p[1]<<8));}
static WL_U32 rd32(const WL_U8*p){return (WL_U32)p[0]|((WL_U32)p[1]<<8)|((WL_U32)p[2]<<16)|((WL_U32)p[3]<<24);}
static WL_I32 rd24s(const WL_U8*p){WL_I32 v=(WL_I32)((WL_U32)p[0]|((WL_U32)p[1]<<8)|((WL_U32)p[2]<<16));if(v&0x00800000)v|=(WL_I32)0xff000000;return v;}
static int fourcc(const WL_U8*p,const char*s){return p[0]==(WL_U8)s[0]&&p[1]==(WL_U8)s[1]&&p[2]==(WL_U8)s[2]&&p[3]==(WL_U8)s[3];}

/* ---------------- sample/config ---------------- */
typedef struct {
 WL_WCHAR path[MAX_PATH_W];
 float* mono;
 WL_U64 frames;
 WL_U32 sampleRate;
 float volume;
 WL_I32 pitch;
 WL_U64 selStart,selEnd;
 WL_U32 vk,mods;
 float peaks[WAVE_BINS];
} Pad;
static Pad g_pads[PAD_COUNT];

typedef struct { WL_U32 magic,version; } ConfigHeader;
typedef struct { WL_WCHAR path[260]; WL_U32 vk,mods; float volume; } PadDiskV1;
typedef struct { WL_WCHAR path[MAX_PATH_W]; WL_U32 vk,mods; float volume; WL_I32 pitch; float trimStart,trimEnd; } PadDiskV2;
#define CFG_MAGIC 0x31424653u
static const WL_WCHAR ENV_LOCALAPPDATA[]={'L','O','C','A','L','A','P','P','D','A','T','A',0};
static const WL_WCHAR APP_DIR_SUFFIX[]={'\\','A','P','O','S','o','u','n','d','b','o','a','r','d',0};
static const WL_WCHAR CFG_SUFFIX_V04[]={'\\','c','o','n','f','i','g','_','v','0','4','.','b','i','n',0};
static const WL_WCHAR CFG_SUFFIX_OLD[]={'\\','c','o','n','f','i','g','.','b','i','n',0};
static int config_path(WL_WCHAR*out,WL_SIZE_T cap,int old){WL_WCHAR dir[700];WL_DWORD n=g_api.GetEnvironmentVariableW(ENV_LOCALAPPDATA,dir,650);if(!n||n>=650)return 0;wl_wcat(dir,APP_DIR_SUFFIX,700);g_api.CreateDirectoryW(dir,0);wl_wcpy(out,dir,cap);wl_wcat(out,old?CFG_SUFFIX_OLD:CFG_SUFFIX_V04,cap);return 1;}

/* ---------------- Media Foundation decoder ---------------- */
typedef WL_I32 (WL_WINAPI *PFN_CoInitializeEx)(void*,WL_DWORD);
typedef void (WL_WINAPI *PFN_CoUninitialize)(void);
typedef WL_I32 (WL_WINAPI *PFN_MFStartup)(WL_U32,WL_DWORD);
typedef WL_I32 (WL_WINAPI *PFN_MFShutdown)(void);
typedef WL_I32 (WL_WINAPI *PFN_MFCreateMediaType)(void**);
typedef WL_I32 (WL_WINAPI *PFN_MFCreateSourceReaderFromURL)(const WL_WCHAR*,void*,void**);
typedef WL_U32 (WL_WINAPI *PFN_ComRelease)(void*);
typedef WL_I32 (WL_WINAPI *PFN_AttrGetU32)(void*,const WL_GUID*,WL_U32*);
typedef WL_I32 (WL_WINAPI *PFN_AttrSetU32)(void*,const WL_GUID*,WL_U32);
typedef WL_I32 (WL_WINAPI *PFN_AttrSetGuid)(void*,const WL_GUID*,const WL_GUID*);
typedef WL_I32 (WL_WINAPI *PFN_SRGetNative)(void*,WL_DWORD,WL_DWORD,void**);
typedef WL_I32 (WL_WINAPI *PFN_SRSetType)(void*,WL_DWORD,WL_DWORD*,void*);
typedef WL_I32 (WL_WINAPI *PFN_SRRead)(void*,WL_DWORD,WL_DWORD,WL_DWORD*,WL_DWORD*,WL_I64*,void**);
typedef WL_I32 (WL_WINAPI *PFN_SampleContig)(void*,void**);
typedef WL_I32 (WL_WINAPI *PFN_BufferLock)(void*,WL_U8**,WL_DWORD*,WL_DWORD*);
typedef WL_I32 (WL_WINAPI *PFN_BufferUnlock)(void*);

typedef struct {
 void* ole32; void* mfplat; void* mfreadwrite;
 PFN_CoInitializeEx CoInitializeEx; PFN_CoUninitialize CoUninitialize;
 PFN_MFStartup MFStartup; PFN_MFShutdown MFShutdown; PFN_MFCreateMediaType MFCreateMediaType;
 PFN_MFCreateSourceReaderFromURL MFCreateSourceReaderFromURL;
 int ready,coInited;
} MediaAPI;
static MediaAPI g_media;
static const WL_WCHAR OLE32_NAME[]={'o','l','e','3','2','.','d','l','l',0};
static const WL_WCHAR MFPLAT_NAME[]={'m','f','p','l','a','t','.','d','l','l',0};
static const WL_WCHAR MFREAD_NAME[]={'m','f','r','e','a','d','w','r','i','t','e','.','d','l','l',0};

static const WL_GUID G_MAJOR={0x48eba18e,0xf8c9,0x4687,{0xbf,0x11,0x0a,0x74,0xc9,0xf9,0x6a,0x8f}};
static const WL_GUID G_SUBTYPE={0xf7e34c9a,0x42e8,0x4714,{0xb7,0x4b,0xcb,0x29,0xd7,0x2c,0x35,0xe5}};
static const WL_GUID G_INDEPENDENT={0xc9173739,0x5e56,0x461c,{0xb7,0x13,0x46,0xfb,0x99,0x5c,0xb9,0x5f}};
static const WL_GUID G_AUDIO={0x73647561,0x0000,0x0010,{0x80,0x00,0x00,0xaa,0x00,0x38,0x9b,0x71}};
static const WL_GUID G_FLOAT={0x00000003,0x0000,0x0010,{0x80,0x00,0x00,0xaa,0x00,0x38,0x9b,0x71}};
static const WL_GUID G_PCM={0x00000001,0x0000,0x0010,{0x80,0x00,0x00,0xaa,0x00,0x38,0x9b,0x71}};
static const WL_GUID G_CHANNELS={0x37e48bf5,0x645e,0x4c5b,{0x89,0xde,0xad,0xa9,0xe2,0x9b,0x69,0x6a}};
static const WL_GUID G_RATE={0x5faeeae7,0x0290,0x4c31,{0x9e,0x8a,0xc5,0x34,0xf6,0x8d,0x9d,0xba}};
static const WL_GUID G_AVG={0x1aab75c8,0xcfef,0x451c,{0xab,0x95,0xac,0x03,0x4b,0x8e,0x17,0x31}};
static const WL_GUID G_BLOCK={0x322de230,0x9eeb,0x43bd,{0xab,0x7a,0xff,0x41,0x22,0x51,0x54,0x1d}};
static const WL_GUID G_BITS={0xf2deb57f,0x40fa,0x4764,{0xaa,0x33,0xed,0x4f,0x2d,0x1f,0xf6,0x69}};

static void* vmethod(void*obj,int idx){return obj?(*(void***)obj)[idx]:0;}
static void com_release(void*o){if(o)((PFN_ComRelease)vmethod(o,2))(o);}
static int media_init(void){memset(&g_media,0,sizeof(g_media));g_media.ole32=g_api.LoadLibraryW(OLE32_NAME);g_media.mfplat=g_api.LoadLibraryW(MFPLAT_NAME);g_media.mfreadwrite=g_api.LoadLibraryW(MFREAD_NAME);if(!g_media.ole32||!g_media.mfplat||!g_media.mfreadwrite)return 0;g_media.CoInitializeEx=(PFN_CoInitializeEx)wl_get_proc(g_media.ole32,"CoInitializeEx");g_media.CoUninitialize=(PFN_CoUninitialize)wl_get_proc(g_media.ole32,"CoUninitialize");g_media.MFStartup=(PFN_MFStartup)wl_get_proc(g_media.mfplat,"MFStartup");g_media.MFShutdown=(PFN_MFShutdown)wl_get_proc(g_media.mfplat,"MFShutdown");g_media.MFCreateMediaType=(PFN_MFCreateMediaType)wl_get_proc(g_media.mfplat,"MFCreateMediaType");g_media.MFCreateSourceReaderFromURL=(PFN_MFCreateSourceReaderFromURL)wl_get_proc(g_media.mfreadwrite,"MFCreateSourceReaderFromURL");if(!g_media.CoInitializeEx||!g_media.CoUninitialize||!g_media.MFStartup||!g_media.MFShutdown||!g_media.MFCreateMediaType||!g_media.MFCreateSourceReaderFromURL)return 0;WL_I32 hr=g_media.CoInitializeEx(0,2);if(hr>=0)g_media.coInited=1;hr=g_media.MFStartup(0x00020070u,0);if(hr<0){if(g_media.coInited)g_media.CoUninitialize();g_media.coInited=0;return 0;}g_media.ready=1;return 1;}
static void media_shutdown(void){if(g_media.ready)g_media.MFShutdown();if(g_media.coInited)g_media.CoUninitialize();g_media.ready=0;g_media.coInited=0;}

static int grow_audio(float**buf,WL_U64*cap,WL_U64 need){if(need>*cap){WL_U64 nc=*cap?*cap:65536;while(nc<need){if(nc>100000000ull/2){nc=need;break;}nc*=2;}if(nc>100000000ull)return 0;void*h=g_api.GetProcessHeap();void*n=*buf?g_api.HeapReAlloc(h,0,*buf,(WL_SIZE_T)nc*sizeof(float)):g_api.HeapAlloc(h,0,(WL_SIZE_T)nc*sizeof(float));if(!n)return 0;*buf=(float*)n;*cap=nc;}return 1;}

static int decode_media_foundation(const WL_WCHAR*path,float**out,WL_U64*framesOut,WL_U32*rateOut){
 if(!g_media.ready)return 0;void*reader=0;void*native=0;void*type=0;float*mono=0;WL_U64 count=0,cap=0;WL_U32 channels=0,rate=0;int floatMode=1;int ok=0;
 if(g_media.MFCreateSourceReaderFromURL(path,0,&reader)<0||!reader)goto done;
 if(((PFN_SRGetNative)vmethod(reader,5))(reader,0xfffffffdu,0,&native)<0||!native)goto done;
 if(((PFN_AttrGetU32)vmethod(native,7))(native,&G_CHANNELS,&channels)<0||channels<1||channels>32)goto done;
 if(((PFN_AttrGetU32)vmethod(native,7))(native,&G_RATE,&rate)<0||rate<8000||rate>384000)goto done;
 if(g_media.MFCreateMediaType(&type)<0||!type)goto done;
 if(((PFN_AttrSetGuid)vmethod(type,24))(type,&G_MAJOR,&G_AUDIO)<0)goto done;
 if(((PFN_AttrSetGuid)vmethod(type,24))(type,&G_SUBTYPE,&G_FLOAT)<0)goto done;
 if(((PFN_AttrSetU32)vmethod(type,21))(type,&G_CHANNELS,channels)<0)goto done;
 if(((PFN_AttrSetU32)vmethod(type,21))(type,&G_RATE,rate)<0)goto done;
 ((PFN_AttrSetU32)vmethod(type,21))(type,&G_BITS,32);
 ((PFN_AttrSetU32)vmethod(type,21))(type,&G_BLOCK,channels*4);
 ((PFN_AttrSetU32)vmethod(type,21))(type,&G_AVG,rate*channels*4);
 ((PFN_AttrSetU32)vmethod(type,21))(type,&G_INDEPENDENT,1);
 if(((PFN_SRSetType)vmethod(reader,7))(reader,0xfffffffdu,0,type)<0){
  com_release(type);type=0;floatMode=0;if(g_media.MFCreateMediaType(&type)<0||!type)goto done;
  if(((PFN_AttrSetGuid)vmethod(type,24))(type,&G_MAJOR,&G_AUDIO)<0)goto done;
  if(((PFN_AttrSetGuid)vmethod(type,24))(type,&G_SUBTYPE,&G_PCM)<0)goto done;
  if(((PFN_AttrSetU32)vmethod(type,21))(type,&G_CHANNELS,channels)<0)goto done;
  if(((PFN_AttrSetU32)vmethod(type,21))(type,&G_RATE,rate)<0)goto done;
  ((PFN_AttrSetU32)vmethod(type,21))(type,&G_BITS,16);
  ((PFN_AttrSetU32)vmethod(type,21))(type,&G_BLOCK,channels*2);
  ((PFN_AttrSetU32)vmethod(type,21))(type,&G_AVG,rate*channels*2);
  ((PFN_AttrSetU32)vmethod(type,21))(type,&G_INDEPENDENT,1);
  if(((PFN_SRSetType)vmethod(reader,7))(reader,0xfffffffdu,0,type)<0)goto done;
 }
 com_release(type);type=0;com_release(native);native=0;
 for(;;){WL_DWORD actual=0,flags=0;WL_I64 ts=0;void*sample=0;WL_I32 hr=((PFN_SRRead)vmethod(reader,9))(reader,0xfffffffdu,0,&actual,&flags,&ts,&sample);if(hr<0){if(sample)com_release(sample);goto done;}if(sample){void*buffer=0;if(((PFN_SampleContig)vmethod(sample,41))(sample,&buffer)>=0&&buffer){WL_U8*data=0;WL_DWORD maxLen=0,curLen=0;if(((PFN_BufferLock)vmethod(buffer,3))(buffer,&data,&maxLen,&curLen)>=0&&data){WL_U32 bps=floatMode?4:2;WL_U32 block=channels*bps;WL_U64 fr=block?curLen/block:0;if(fr&&count+fr<=100000000ull&&grow_audio(&mono,&cap,count+fr)){for(WL_U64 f=0;f<fr;f++){float sum=0;WL_U32 used=channels<2?channels:2;for(WL_U32 c=0;c<used;c++){const WL_U8*s=data+f*block+c*bps;float v;if(floatMode){union{WL_U32 u;float f;}q;q.u=rd32(s);v=q.f;}else{WL_I16 q=(WL_I16)rd16(s);v=(float)q/32768.0f;}sum+=v;}mono[count+f]=used?sum/(float)used:0;}count+=fr;}((PFN_BufferUnlock)vmethod(buffer,4))(buffer);}com_release(buffer);}com_release(sample);}if(flags&0x2u)break;}
 if(!count)goto done;*out=mono;*framesOut=count;*rateOut=rate;mono=0;ok=1;
done:if(type)com_release(type);if(native)com_release(native);if(reader)com_release(reader);if(mono)g_api.HeapFree(g_api.GetProcessHeap(),0,mono);return ok;
}

static int decode_pcm_wav(const WL_WCHAR*path,float**out,WL_U64*framesOut,WL_U32*rateOut){
 WL_HANDLE h=g_api.CreateFileW(path,GENERIC_READ,1,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);if(h==WL_INVALID_HANDLE_VALUE)return 0;WL_I64 size=0;if(!g_api.GetFileSizeEx(h,&size)||size<44||size>(WL_I64)(512*1024*1024)){g_api.CloseHandle(h);return 0;}WL_U8*bytes=(WL_U8*)g_api.HeapAlloc(g_api.GetProcessHeap(),0,(WL_SIZE_T)size);if(!bytes){g_api.CloseHandle(h);return 0;}WL_DWORD total=0;while(total<(WL_U32)size){WL_DWORD n=0;if(!g_api.ReadFile(h,bytes+total,(WL_DWORD)size-total,&n,0)||!n)break;total+=n;}g_api.CloseHandle(h);if(total!=(WL_U32)size||!fourcc(bytes,"RIFF")||!fourcc(bytes+8,"WAVE")){g_api.HeapFree(g_api.GetProcessHeap(),0,bytes);return 0;}
 WL_U16 tag=0,ch=0,bits=0,block=0;WL_U32 sr=0;WL_U8*data=0;WL_U32 dataBytes=0;WL_U32 pos=12;while(pos+8<=(WL_U32)size){WL_U8*c=bytes+pos;WL_U32 cs=rd32(c+4),body=pos+8;if(body+cs>(WL_U32)size)break;if(fourcc(c,"fmt ")&&cs>=16){tag=rd16(bytes+body);ch=rd16(bytes+body+2);sr=rd32(bytes+body+4);block=rd16(bytes+body+12);bits=rd16(bytes+body+14);if(tag==0xfffe&&cs>=40)tag=rd16(bytes+body+24);}else if(fourcc(c,"data")){data=bytes+body;dataBytes=cs;}pos=body+cs+(cs&1u);}WL_U32 bps=(bits+7)/8;if(!data||!ch||!sr||!block||!bits||!(tag==1||tag==3)||!bps||block<(WL_U32)ch*bps||((tag==1)&&!(bits==8||bits==16||bits==24||bits==32))||((tag==3)&&bits!=32)){g_api.HeapFree(g_api.GetProcessHeap(),0,bytes);return 0;}WL_U64 frames=dataBytes/block;if(!frames||frames>100000000ull){g_api.HeapFree(g_api.GetProcessHeap(),0,bytes);return 0;}float*mono=(float*)g_api.HeapAlloc(g_api.GetProcessHeap(),0,(WL_SIZE_T)frames*sizeof(float));if(!mono){g_api.HeapFree(g_api.GetProcessHeap(),0,bytes);return 0;}for(WL_U64 f=0;f<frames;f++){float sum=0;WL_U32 used=ch<2?ch:2;for(WL_U32 c=0;c<used;c++){const WL_U8*s=data+f*block+c*bps;float v=0;if(tag==3&&bits==32){union{WL_U32 u;float f;}q;q.u=rd32(s);v=q.f;}else if(bits==8)v=((int)s[0]-128)/128.0f;else if(bits==16){WL_I16 q=(WL_I16)rd16(s);v=(float)q/32768.0f;}else if(bits==24)v=(float)rd24s(s)/8388608.0f;else if(bits==32){WL_I32 q=(WL_I32)rd32(s);v=(float)q/2147483648.0f;}sum+=v;}mono[f]=used?sum/(float)used:0;}g_api.HeapFree(g_api.GetProcessHeap(),0,bytes);*out=mono;*framesOut=frames;*rateOut=sr;return 1;
}

static int load_audio_into_pad(int idx,const WL_WCHAR*path,float trimStart,float trimEnd){float*mono=0;WL_U64 frames=0;WL_U32 sr=0;if(!decode_pcm_wav(path,&mono,&frames,&sr)&&!decode_media_foundation(path,&mono,&frames,&sr))return 0;if(trimStart<0||trimStart>=1)trimStart=0;if(trimEnd<=0||trimEnd>1)trimEnd=1;if(trimEnd<=trimStart){trimStart=0;trimEnd=1;}WL_U64 ss=(WL_U64)((double)frames*trimStart),ee=(WL_U64)((double)frames*trimEnd);if(ee>frames)ee=frames;if(ee<=ss){ss=0;ee=frames;}spin_lock();float*old=g_pads[idx].mono;g_pads[idx].mono=mono;g_pads[idx].frames=frames;g_pads[idx].sampleRate=sr;g_pads[idx].selStart=ss;g_pads[idx].selEnd=ee;wl_wcpy(g_pads[idx].path,path,MAX_PATH_W);for(int b=0;b<WAVE_BINS;b++){WL_U64 a=(WL_U64)((double)b*frames/WAVE_BINS),z=(WL_U64)((double)(b+1)*frames/WAVE_BINS);if(z<=a)z=a+1;if(z>frames)z=frames;float pk=0;WL_U64 span=z-a,step=span>256?span/256:1;for(WL_U64 f=a;f<z;f+=step){float q=mono[f];if(q<0)q=-q;if(q>pk)pk=q;}g_pads[idx].peaks[b]=pk>1?1:pk;}spin_unlock();__atomic_add_fetch(&g_stopSerial,1,__ATOMIC_SEQ_CST);if(old)g_api.HeapFree(g_api.GetProcessHeap(),0,old);return 1;}

static void save_config(void){WL_WCHAR path[700];if(!config_path(path,700,0))return;WL_HANDLE h=g_api.CreateFileW(path,GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);if(h==WL_INVALID_HANDLE_VALUE)return;ConfigHeader ch={CFG_MAGIC,2};WL_DWORD wr=0;g_api.WriteFile(h,&ch,sizeof(ch),&wr,0);for(int i=0;i<PAD_COUNT;i++){PadDiskV2 d;memset(&d,0,sizeof(d));wl_wcpy(d.path,g_pads[i].path,MAX_PATH_W);d.vk=g_pads[i].vk;d.mods=g_pads[i].mods;d.volume=g_pads[i].volume;d.pitch=g_pads[i].pitch;if(g_pads[i].frames){d.trimStart=(float)((double)g_pads[i].selStart/(double)g_pads[i].frames);d.trimEnd=(float)((double)g_pads[i].selEnd/(double)g_pads[i].frames);}else{d.trimStart=0;d.trimEnd=1;}g_api.WriteFile(h,&d,sizeof(d),&wr,0);}g_api.CloseHandle(h);}

static void load_config(void){for(int i=0;i<PAD_COUNT;i++){g_pads[i].volume=1.0f;g_pads[i].pitch=0;}WL_WCHAR path[700];if(!config_path(path,700,0))return;WL_HANDLE h=g_api.CreateFileW(path,GENERIC_READ,1,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);if(h==WL_INVALID_HANDLE_VALUE){if(!config_path(path,700,1))return;h=g_api.CreateFileW(path,GENERIC_READ,1,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);if(h==WL_INVALID_HANDLE_VALUE)return;}ConfigHeader ch;WL_DWORD n=0;if(!g_api.ReadFile(h,&ch,sizeof(ch),&n,0)||n!=sizeof(ch)||ch.magic!=CFG_MAGIC||(ch.version!=1&&ch.version!=2)){g_api.CloseHandle(h);return;}if(ch.version==1){for(int i=0;i<PAD_COUNT;i++){PadDiskV1 d;memset(&d,0,sizeof(d));if(!g_api.ReadFile(h,&d,sizeof(d),&n,0)||n!=sizeof(d))break;g_pads[i].vk=d.vk;g_pads[i].mods=d.mods;g_pads[i].volume=fclamp(d.volume,0,2);if(d.path[0]){WL_WCHAR full[MAX_PATH_W];full[0]=0;wl_wcpy(full,d.path,260);load_audio_into_pad(i,full,0,1);}}}else{for(int i=0;i<PAD_COUNT;i++){PadDiskV2 d;memset(&d,0,sizeof(d));if(!g_api.ReadFile(h,&d,sizeof(d),&n,0)||n!=sizeof(d))break;g_pads[i].vk=d.vk;g_pads[i].mods=d.mods;g_pads[i].volume=fclamp(d.volume,0,2);g_pads[i].pitch=iclamp(d.pitch,-24,24);if(d.path[0])load_audio_into_pad(i,d.path,d.trimStart,d.trimEnd);}}g_api.CloseHandle(h);}

/* ---------------- hotkeys / text ---------------- */
static void key_name(WL_U32 vk,WL_WCHAR*out,WL_SIZE_T cap){out[0]=0;if((vk>='0'&&vk<='9')||(vk>='A'&&vk<='Z')){out[0]=(WL_WCHAR)vk;out[1]=0;return;}if(vk>=0x70&&vk<=0x87){wadd_ascii(out,cap,"F");wadd_num(out,cap,(WL_I64)(vk-0x6f));return;}if(vk>=0x60&&vk<=0x69){wadd_ascii(out,cap,"Num ");wadd_num(out,cap,(WL_I64)(vk-0x60));return;}WL_UINT sc=g_api.MapVirtualKeyW(vk,0);WL_I32 lp=(WL_I32)(sc<<16);if(!g_api.GetKeyNameTextW(lp,out,(WL_I32)cap))wadd_num(out,cap,vk);}
static void hotkey_text(int i,WL_WCHAR*out,WL_SIZE_T cap){out[0]=0;WL_U32 m=g_pads[i].mods,v=g_pads[i].vk;if(!v){wadd_ascii(out,cap,"No hotkey");return;}if(m&MOD_CONTROL)wadd_ascii(out,cap,"Ctrl+");if(m&MOD_ALT)wadd_ascii(out,cap,"Alt+");if(m&MOD_SHIFT)wadd_ascii(out,cap,"Shift+");if(m&MOD_WIN)wadd_ascii(out,cap,"Win+");WL_WCHAR k[64];key_name(v,k,64);wl_wcat(out,k,cap);}
static int register_pad_hotkey(int i){g_api.UnregisterHotKey(g_main,HOTKEY_BASE+i);if(!g_pads[i].vk)return 1;return g_api.RegisterHotKey(g_main,HOTKEY_BASE+i,g_pads[i].mods|MOD_NOREPEAT,g_pads[i].vk)?1:0;}
static void trigger_pad(int i){if(i<0||i>=PAD_COUNT||!g_pads[i].mono)return;__atomic_add_fetch(&g_triggerSerial[i],1,__ATOMIC_SEQ_CST);}
static void stop_all(void){__atomic_add_fetch(&g_stopSerial,1,__ATOMIC_SEQ_CST);}
static void invalidate(void){if(g_main)g_api.InvalidateRect(g_main,0,WL_FALSE);}

static int choose_file_for_pad(int i){static const WL_WCHAR FILTER[]={ 'A','u','d','i','o',' ','f','i','l','e','s',0,'*','.','w','a','v',';','*','.','m','p','3',';','*','.','f','l','a','c',';','*','.','m','4','a',';','*','.','a','a','c',';','*','.','w','m','a',';','*','.','o','g','g',';','*','.','o','p','u','s',0,'W','A','V',' ','f','i','l','e','s',0,'*','.','w','a','v',0,'A','l','l',' ','f','i','l','e','s',0,'*','.','*',0,0};WL_WCHAR file[MAX_PATH_W];file[0]=0;WL_OPENFILENAMEW ofn;memset(&ofn,0,sizeof(ofn));ofn.lStructSize=sizeof(ofn);ofn.hwndOwner=g_main;ofn.lpstrFilter=FILTER;ofn.lpstrFile=file;ofn.nMaxFile=MAX_PATH_W;ofn.Flags=OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_EXPLORER|OFN_NOCHANGEDIR;if(!g_api.GetOpenFileNameW(&ofn))return 0;if(!load_audio_into_pad(i,file,0,1)){g_api.MessageBoxW(g_main,(const WL_WCHAR[]){'T','h','a','t',' ','a','u','d','i','o',' ','f','i','l','e',' ','c','o','u','l','d',' ','n','o','t',' ','b','e',' ','d','e','c','o','d','e','d','.',0},WINDOW_TITLE,0x10);return 0;}save_config();invalidate();return 1;}

/* ---------------- layout / painting ---------------- */
typedef struct {WL_RECT header,pads[PAD_COUNT],inspect,wave,footer,loadBtn,playBtn,hotkeyBtn,clearBtn,stopBtn,volKnob,pitchKnob;} Layout;
static void get_layout(Layout*l){WL_RECT c;g_api.GetClientRect(g_main,&c);int W=c.right-c.left,H=c.bottom-c.top;memset(l,0,sizeof(*l));l->header=(WL_RECT){0,0,W,50};int padX=18,padY=64,padW=(W>760?(W*64/100):W-36),padH=314;int gap=8,cw=(padW-gap*3)/4,ch=(padH-gap*3)/4;for(int i=0;i<PAD_COUNT;i++){int col=i%4,row=i/4;int x=padX+col*(cw+gap),y=padY+row*(ch+gap);l->pads[i]=(WL_RECT){x,y,x+cw,y+ch};}int ix=padX+padW+18,ir=W-18;if(ir-ix<280){ix=W-310;if(ix<padX+padW)ix=padX+padW+8;}l->inspect=(WL_RECT){ix,padY,ir,padY+padH};int kTop=padY+96;l->volKnob=(WL_RECT){ix+22,kTop,ix+122,kTop+100};l->pitchKnob=(WL_RECT){ix+142,kTop,ix+242,kTop+100};int by=padY+220;l->loadBtn=(WL_RECT){ix+14,by,ix+105,by+34};l->playBtn=(WL_RECT){ix+112,by,ix+196,by+34};l->stopBtn=(WL_RECT){ix+203,by,ir-14,by+34};l->hotkeyBtn=(WL_RECT){ix+14,by+43,ix+145,by+77};l->clearBtn=(WL_RECT){ix+152,by+43,ir-14,by+77};int waveY=padY+padH+18;int footerH=42;l->wave=(WL_RECT){18,waveY,W-18,H-footerH-12};l->footer=(WL_RECT){18,H-footerH,W-18,H-6};}

static void draw_text(WL_HDC dc,const WL_WCHAR*t,WL_RECT r,WL_HFONT f,WL_DWORD color,WL_UINT flags){WL_HGDIOBJ old=0;if(f)old=g_api.SelectObject(dc,(WL_HGDIOBJ)f);g_api.SetTextColor(dc,color);g_api.SetBkMode(dc,TRANSPARENT);g_api.DrawTextW(dc,t,-1,&r,flags|DT_NOPREFIX);if(old)g_api.SelectObject(dc,old);}
static void draw_button(WL_HDC dc,WL_RECT r,const char*txt){fill(dc,r,g_brButton);frame(dc,r,g_penBorder);WL_WCHAR w[80];wset_ascii(w,80,txt);draw_text(dc,w,r,g_fontSmall,RGB_(225,228,230),DT_CENTER|DT_VCENTER|DT_SINGLELINE);}
static const WL_I32 knobPts[17][2]={{-21,21},{-26,14},{-29,6},{-30,-3},{-28,-11},{-23,-19},{-17,-25},{-9,-29},{0,-30},{9,-29},{17,-25},{23,-19},{28,-11},{30,-3},{29,6},{26,14},{21,21}};
static void draw_knob(WL_HDC dc,WL_RECT r,int kind){int cx=(r.left+r.right)/2,cy=r.top+43;WL_HGDIOBJ oldP=g_api.SelectObject(dc,(WL_HGDIOBJ)g_penKnob),oldB=g_api.SelectObject(dc,(WL_HGDIOBJ)g_brKnob);g_api.Ellipse(dc,cx-37,cy-37,cx+37,cy+37);g_api.SelectObject(dc,oldB);g_api.SelectObject(dc,oldP);int idx;if(kind==1)idx=iclamp((int)(g_pads[g_selected].volume*8.0f+0.5f),0,16);else idx=iclamp((g_pads[g_selected].pitch+24)*16/48,0,16);line(dc,cx,cy,cx+knobPts[idx][0],cy+knobPts[idx][1],g_penAccent);WL_WCHAR t[64];t[0]=0;if(kind==1){wadd_ascii(t,64,"VOLUME  ");wadd_num(t,64,(WL_I64)(g_pads[g_selected].volume*100.0f+0.5f));wadd_ascii(t,64,"%");}else{wadd_ascii(t,64,"PITCH  ");if(g_pads[g_selected].pitch>0)wadd_ascii(t,64,"+");wadd_num(t,64,g_pads[g_selected].pitch);wadd_ascii(t,64," st");}WL_RECT tr={r.left,r.bottom-23,r.right,r.bottom};draw_text(dc,t,tr,g_fontSmall,RGB_(218,221,224),DT_CENTER|DT_SINGLELINE);}

static void draw_pad(WL_HDC dc,int i,WL_RECT r){Pad*p=&g_pads[i];WL_HBRUSH br=i==g_selected?g_brSelected:(p->mono?g_brPadLoaded:g_brPad);fill(dc,r,br);frame(dc,r,i==g_selected?g_penAccent:g_penBorder);WL_WCHAR num[20];num[0]=0;wadd_num(num,20,i+1);WL_RECT nr={r.left+8,r.top+5,r.left+38,r.top+25};draw_text(dc,num,nr,g_fontTiny,i==g_selected?RGB_(247,174,92):RGB_(142,149,155),DT_LEFT|DT_SINGLELINE);WL_WCHAR name[300];if(p->path[0])wl_wcpy(name,base_name(p->path),300);else wset_ascii(name,300,"Empty pad");WL_RECT rr={r.left+9,r.top+26,r.right-9,r.bottom-24};draw_text(dc,name,rr,g_fontNormal,p->mono?RGB_(238,240,241):RGB_(125,130,135),DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);WL_WCHAR hk[100];hotkey_text(i,hk,100);WL_RECT hr={r.left+9,r.bottom-22,r.right-9,r.bottom-4};draw_text(dc,hk,hr,g_fontTiny,RGB_(132,139,145),DT_LEFT|DT_SINGLELINE|DT_END_ELLIPSIS);}

static void draw_waveform(WL_HDC dc,WL_RECT r){fill(dc,r,g_brWave);frame(dc,r,g_penBorder);Pad*p=&g_pads[g_selected];WL_RECT title={r.left+10,r.top+7,r.right-10,r.top+31};WL_WCHAR wt[220];wt[0]=0;wadd_ascii(wt,220,"WAVEFORM / PLAY REGION");if(p->mono&&p->frames){wadd_ascii(wt,220,"     drag to select  |  double-click to reset");}draw_text(dc,wt,title,g_fontSmall,RGB_(181,186,190),DT_LEFT|DT_SINGLELINE);int left=r.left+12,right=r.right-12,top=r.top+38,bottom=r.bottom-12,w=right-left,h=bottom-top,mid=(top+bottom)/2;if(!p->mono||!p->frames||w<=2||h<=2){WL_RECT er={left,top,right,bottom};WL_WCHAR e[120];wset_ascii(e,120,"Load an audio file to see its waveform");draw_text(dc,e,er,g_fontNormal,RGB_(108,114,120),DT_CENTER|DT_VCENTER|DT_SINGLELINE);return;}int sx=left+(int)((double)p->selStart*(double)w/(double)p->frames),ex=left+(int)((double)p->selEnd*(double)w/(double)p->frames);if(ex<=sx)ex=sx+1;WL_RECT sel={sx,top,ex,bottom};fill(dc,sel,g_brWaveSel);line(dc,left,mid,right,mid,g_penBorder);for(int x=0;x<w;x++){int b=(int)((WL_I64)x*WAVE_BINS/w);if(b<0)b=0;if(b>=WAVE_BINS)b=WAVE_BINS-1;float peak=p->peaks[b];int amp=(int)(peak*(h/2-2));line(dc,left+x,mid-amp,left+x,mid+amp,(left+x>=sx&&left+x<ex)?g_penWaveSel:g_penWave);}line(dc,sx,top,sx,bottom,g_penAccent);line(dc,ex-1,top,ex-1,bottom,g_penAccent);WL_U64 startMs=p->sampleRate?1000ull*p->selStart/p->sampleRate:0,endMs=p->sampleRate?1000ull*p->selEnd/p->sampleRate:0,totalMs=p->sampleRate?1000ull*p->frames/p->sampleRate:0;WL_WCHAR info[160];info[0]=0;wadd_ascii(info,160,"Selection: ");wadd_num(info,160,startMs);wadd_ascii(info,160," ms - ");wadd_num(info,160,endMs);wadd_ascii(info,160," ms   /   ");wadd_num(info,160,totalMs);wadd_ascii(info,160," ms total");WL_RECT ir={right-360,r.top+7,right,r.top+31};draw_text(dc,info,ir,g_fontTiny,RGB_(139,145,150),DT_RIGHT|DT_SINGLELINE);}

static void paint_ui(WL_HDC dc){Layout l;get_layout(&l);WL_RECT c;g_api.GetClientRect(g_main,&c);fill(dc,c,g_brBg);fill(dc,l.header,g_brPanel);WL_RECT title={18,9,520,43};draw_text(dc,(const WL_WCHAR[]){'A','P','O',' ','S','O','U','N','D','B','O','A','R','D',0},title,g_fontTitle,RGB_(238,240,242),DT_LEFT|DT_VCENTER|DT_SINGLELINE);WL_WCHAR conn[140];conn[0]=0;wadd_ascii(conn,140,"VST AUDIO  ");wadd_num(conn,140,__atomic_load_n(&g_connections,__ATOMIC_RELAXED));wadd_ascii(conn,140,__atomic_load_n(&g_connections,__ATOMIC_RELAXED)?" connected":" waiting");WL_RECT cr={c.right-300,12,c.right-18,40};draw_text(dc,conn,cr,g_fontSmall,__atomic_load_n(&g_connections,__ATOMIC_RELAXED)?RGB_(121,201,145):RGB_(167,154,120),DT_RIGHT|DT_VCENTER|DT_SINGLELINE);for(int i=0;i<PAD_COUNT;i++)draw_pad(dc,i,l.pads[i]);fill(dc,l.inspect,g_brPanel);frame(dc,l.inspect,g_penBorder);WL_WCHAR sel[80];sel[0]=0;wadd_ascii(sel,80,"PAD ");wadd_num(sel,80,g_selected+1);WL_RECT sr={l.inspect.left+14,l.inspect.top+12,l.inspect.right-14,l.inspect.top+38};draw_text(dc,sel,sr,g_fontTitle,RGB_(242,174,96),DT_LEFT|DT_SINGLELINE);WL_WCHAR nm[300];if(g_pads[g_selected].path[0])wl_wcpy(nm,base_name(g_pads[g_selected].path),300);else wset_ascii(nm,300,"No sample loaded");WL_RECT nr={l.inspect.left+14,l.inspect.top+47,l.inspect.right-14,l.inspect.top+78};draw_text(dc,nm,nr,g_fontNormal,RGB_(219,222,224),DT_LEFT|DT_SINGLELINE|DT_END_ELLIPSIS);draw_knob(dc,l.volKnob,1);draw_knob(dc,l.pitchKnob,2);draw_button(dc,l.loadBtn,"LOAD");draw_button(dc,l.playBtn,"PLAY");draw_button(dc,l.stopBtn,"STOP ALL");draw_button(dc,l.hotkeyBtn,g_capturePad==g_selected?"PRESS HOTKEY...":"SET HOTKEY");draw_button(dc,l.clearBtn,"CLEAR HOTKEY");WL_WCHAR hk[120];hotkey_text(g_selected,hk,120);WL_RECT hkr={l.inspect.left+14,l.inspect.bottom-34,l.inspect.right-14,l.inspect.bottom-10};draw_text(dc,hk,hkr,g_fontSmall,RGB_(144,150,156),DT_LEFT|DT_SINGLELINE|DT_END_ELLIPSIS);draw_waveform(dc,l.wave);fill(dc,l.footer,g_brPanel);WL_WCHAR ft[380];ft[0]=0;if(g_capturePad>=0)wset_ascii(ft,380,"Press a key combination. Esc cancels.");else wset_ascii(ft,380,"Single-click selects  |  Double-click pad plays  |  Drag knobs  |  Drag waveform to choose the played region  |  Space plays selected pad");draw_text(dc,ft,l.footer,g_fontSmall,RGB_(154,160,166),DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);}

/* ---------------- audio pipe ---------------- */
static const double pitchRatio[49]={0.250000000,0.264865774,0.280615512,0.297301779,0.314980262,0.333709964,0.353553391,0.374576769,0.396850263,0.420448208,0.445449359,0.471937156,0.500000000,0.529731547,0.561231024,0.594603558,0.629960525,0.667419927,0.707106781,0.749153538,0.793700526,0.840896415,0.890898718,0.943874313,1.000000000,1.059463094,1.122462048,1.189207115,1.259921050,1.334839854,1.414213562,1.498307077,1.587401052,1.681792831,1.781797436,1.887748625,2.000000000,2.118926189,2.244924097,2.378414230,2.519842100,2.669679708,2.828427125,2.996614154,3.174802104,3.363585661,3.563594873,3.775497251,4.000000000};
typedef struct {WL_I32 active,pad;double pos;} Voice;
typedef struct {WL_HANDLE pipe;WL_U32 sampleRate;WL_I64 seen[PAD_COUNT],seenStop;Voice voices[MAX_VOICES];} Client;
static int pipe_read_exact(WL_HANDLE h,void*buf,WL_U32 bytes){WL_U8*d=(WL_U8*)buf;WL_U32 done=0;while(done<bytes&&g_running){WL_DWORD n=0;if(!g_api.ReadFile(h,d+done,bytes-done,&n,0)||!n)return 0;done+=n;}return done==bytes;}
static int pipe_write_exact(WL_HANDLE h,const void*buf,WL_U32 bytes){const WL_U8*s=(const WL_U8*)buf;WL_U32 done=0;while(done<bytes&&g_running){WL_DWORD n=0;if(!g_api.WriteFile(h,s+done,bytes-done,&n,0)||!n)return 0;done+=n;}return done==bytes;}
static void add_voice(Client*c,int pad){Pad*p=&g_pads[pad];if(!p->mono||!p->frames||p->selEnd<=p->selStart)return;for(int v=0;v<MAX_VOICES;v++)if(!c->voices[v].active){c->voices[v].active=1;c->voices[v].pad=pad;c->voices[v].pos=(double)p->selStart;return;}c->voices[0].active=1;c->voices[0].pad=pad;c->voices[0].pos=(double)p->selStart;}
static void sync_triggers(Client*c){WL_I64 stop=__atomic_load_n(&g_stopSerial,__ATOMIC_ACQUIRE);if(stop!=c->seenStop){for(int v=0;v<MAX_VOICES;v++)c->voices[v].active=0;c->seenStop=stop;}for(int i=0;i<PAD_COUNT;i++){WL_I64 now=__atomic_load_n(&g_triggerSerial[i],__ATOMIC_ACQUIRE),delta=now-c->seen[i];if(delta>8)delta=8;while(delta-->0)add_voice(c,i);c->seen[i]=now;}}
static void generate_audio(Client*c,float*out,WL_U32 frames){spin_lock();sync_triggers(c);for(WL_U32 f=0;f<frames;f++){float sum=0;for(int v=0;v<MAX_VOICES;v++){Voice*vo=&c->voices[v];if(!vo->active)continue;Pad*p=&g_pads[vo->pad];if(!p->mono||!p->frames||!p->sampleRate||p->selEnd<=p->selStart){vo->active=0;continue;}double pos=vo->pos;if(pos<(double)p->selStart)pos=(double)p->selStart;WL_U64 i0=(WL_U64)pos;if(i0>=p->selEnd||i0>=p->frames){vo->active=0;continue;}WL_U64 i1=i0+1<p->frames?i0+1:i0;float frac=(float)(pos-(double)i0);float s=p->mono[i0]+(p->mono[i1]-p->mono[i0])*frac;sum+=s*p->volume;double step=(double)p->sampleRate/(double)c->sampleRate*pitchRatio[iclamp(p->pitch,-24,24)+24];pos+=step;vo->pos=pos;if(pos>=(double)p->selEnd)vo->active=0;}sum=fclamp(sum,-4.0f,4.0f);out[f*2]=sum;out[f*2+1]=sum;}spin_unlock();}
static WL_DWORD WL_CALLBACK server_thread(void*ctx){(void)ctx;void*sd=0;WL_SECURITY_ATTRIBUTES sa;memset(&sa,0,sizeof(sa));sa.nLength=sizeof(sa);static const WL_WCHAR SDDL[]={ 'D',':','(','A',';',';','G','A',';',';',';','W','D',')',0};if(g_api.ConvertStringSecurityDescriptorToSecurityDescriptorW(SDDL,SDDL_REVISION_1,&sd,0))sa.lpSecurityDescriptor=sd;float*audio=(float*)g_api.HeapAlloc(g_api.GetProcessHeap(),0,2048*2*sizeof(float));if(!audio){if(sd)g_api.LocalFree(sd);return 0;}while(g_running){WL_HANDLE p=g_api.CreateNamedPipeW(PIPE_NAME,PIPE_ACCESS_DUPLEX,PIPE_TYPE_MESSAGE|PIPE_READMODE_MESSAGE|PIPE_WAIT|PIPE_REJECT_REMOTE_CLIENTS,1,16384,4096,0,sd?&sa:0);if(p==WL_INVALID_HANDLE_VALUE){g_api.Sleep(250);continue;}WL_BOOL ok=g_api.ConnectNamedPipe(p,0);if(!ok&&g_api.GetLastError()!=ERROR_PIPE_CONNECTED){g_api.CloseHandle(p);if(g_running)g_api.Sleep(50);continue;}Client c;memset(&c,0,sizeof(c));c.pipe=p;PipeHello hi;if(!pipe_read_exact(p,&hi,sizeof(hi))||hi.magic!=M_HELLO||hi.version!=1||hi.sampleRate<8000||hi.sampleRate>384000){g_api.DisconnectNamedPipe(p);g_api.CloseHandle(p);continue;}c.sampleRate=hi.sampleRate;for(int i=0;i<PAD_COUNT;i++)c.seen[i]=__atomic_load_n(&g_triggerSerial[i],__ATOMIC_ACQUIRE);c.seenStop=__atomic_load_n(&g_stopSerial,__ATOMIC_ACQUIRE);__atomic_store_n(&g_connections,1,__ATOMIC_RELEASE);invalidate();while(g_running){PipeRequest rq;if(!pipe_read_exact(p,&rq,sizeof(rq)))break;if(rq.magic!=M_REQ||!rq.frames||rq.frames>2048)break;generate_audio(&c,audio,rq.frames);PipeAudio ah={M_AUDIO,rq.frames,2};if(!pipe_write_exact(p,&ah,sizeof(ah))||!pipe_write_exact(p,audio,rq.frames*2*sizeof(float)))break;}__atomic_store_n(&g_connections,0,__ATOMIC_RELEASE);invalidate();g_api.DisconnectNamedPipe(p);g_api.CloseHandle(p);if(g_running)g_api.Sleep(30);}g_api.HeapFree(g_api.GetProcessHeap(),0,audio);if(sd)g_api.LocalFree(sd);return 0;}

/* ---------------- tray / resource lifecycle ---------------- */
static void add_tray(void){if(g_trayShown||!g_main||!g_api.Shell_NotifyIconW)return;WL_NOTIFYICONDATAW n;memset(&n,0,sizeof(n));n.cbSize=sizeof(n);n.hWnd=g_main;n.uID=1;n.uFlags=NIF_MESSAGE|NIF_ICON|NIF_TIP;n.uCallbackMessage=WM_TRAY;n.hIcon=g_icon;wl_wcpy(n.szTip,(const WL_WCHAR[]){'A','P','O',' ','S','o','u','n','d','b','o','a','r','d',' ','v','0','.','4',' ','-',' ','d','o','u','b','l','e','-','c','l','i','c','k',' ','t','o',' ','r','e','s','t','o','r','e',0},128);if(g_api.Shell_NotifyIconW(NIM_ADD,&n))g_trayShown=1;}
static void remove_tray(void){if(!g_trayShown)return;WL_NOTIFYICONDATAW n;memset(&n,0,sizeof(n));n.cbSize=sizeof(n);n.hWnd=g_main;n.uID=1;g_api.Shell_NotifyIconW(NIM_DELETE,&n);g_trayShown=0;}
static void restore_window(void){remove_tray();g_api.ShowWindow(g_main,SW_RESTORE);g_api.SetForegroundWindow(g_main);}
static void create_gdi(void){g_fontTitle=g_api.CreateFontW(-22,0,0,0,600,0,0,0,1,0,0,5,0,FONT_NAME);g_fontNormal=g_api.CreateFontW(-16,0,0,0,500,0,0,0,1,0,0,5,0,FONT_NAME);g_fontSmall=g_api.CreateFontW(-14,0,0,0,400,0,0,0,1,0,0,5,0,FONT_NAME);g_fontTiny=g_api.CreateFontW(-12,0,0,0,400,0,0,0,1,0,0,5,0,FONT_NAME);g_brBg=g_api.CreateSolidBrush(RGB_(22,24,26));g_brPanel=g_api.CreateSolidBrush(RGB_(30,33,36));g_brPad=g_api.CreateSolidBrush(RGB_(36,39,42));g_brPadLoaded=g_api.CreateSolidBrush(RGB_(42,45,48));g_brSelected=g_api.CreateSolidBrush(RGB_(55,48,42));g_brAccent=g_api.CreateSolidBrush(RGB_(235,149,74));g_brWave=g_api.CreateSolidBrush(RGB_(18,20,22));g_brWaveSel=g_api.CreateSolidBrush(RGB_(48,40,34));g_brKnob=g_api.CreateSolidBrush(RGB_(45,48,51));g_brButton=g_api.CreateSolidBrush(RGB_(43,46,49));g_brButtonHot=g_api.CreateSolidBrush(RGB_(56,50,43));g_penBorder=g_api.CreatePen(PS_SOLID,1,RGB_(58,62,66));g_penAccent=g_api.CreatePen(PS_SOLID,2,RGB_(241,157,81));g_penWave=g_api.CreatePen(PS_SOLID,1,RGB_(91,101,109));g_penWaveSel=g_api.CreatePen(PS_SOLID,1,RGB_(228,145,73));g_penKnob=g_api.CreatePen(PS_SOLID,2,RGB_(82,88,93));g_icon=g_api.LoadIconW(0,(const WL_WCHAR*)(WL_UPTR)IDI_APPLICATION);}
static void destroy_gdi(void){WL_HGDIOBJ objs[]={g_fontTitle,g_fontNormal,g_fontSmall,g_fontTiny,g_brBg,g_brPanel,g_brPad,g_brPadLoaded,g_brSelected,g_brAccent,g_brWave,g_brWaveSel,g_brKnob,g_brButton,g_brButtonHot,g_penBorder,g_penAccent,g_penWave,g_penWaveSel,g_penKnob};for(unsigned i=0;i<sizeof(objs)/sizeof(objs[0]);i++)if(objs[i])g_api.DeleteObject(objs[i]);}

/* ---------------- input ---------------- */
static void waveform_set_from_x(Layout*l,int x,int final){Pad*p=&g_pads[g_selected];if(!p->mono||!p->frames)return;int left=l->wave.left+12,right=l->wave.right-12,w=right-left;if(w<=0)return;int ax=iclamp(g_waveAnchorX,left,right),bx=iclamp(x,left,right);if(!final&&bx!=ax)g_waveMoved=1;if(!g_waveMoved&&!final)return;WL_U64 a=(WL_U64)((double)(ax-left)*p->frames/w),b=(WL_U64)((double)(bx-left)*p->frames/w);WL_U64 s=a<b?a:b,e=a<b?b:a;if(e>p->frames)e=p->frames;if(e<=s)e=s+1;if(e>p->frames){e=p->frames;if(s>=e)s=e?e-1:0;}p->selStart=s;p->selEnd=e;invalidate();if(final)save_config();}
static void handle_knob_move(int y){if(g_dragMode==1){float v=g_dragStartVolume+(float)(g_dragStartY-y)/90.0f;g_pads[g_selected].volume=fclamp(v,0,2);}else if(g_dragMode==2){int p=g_dragStartPitch+(g_dragStartY-y)/3;g_pads[g_selected].pitch=iclamp(p,-24,24);}invalidate();}

static WL_LRESULT WL_CALLBACK wndproc(WL_HWND hwnd,WL_UINT msg,WL_WPARAM wp,WL_LPARAM lp){
 if(msg==WM_CREATE){g_main=hwnd;create_gdi();for(int i=0;i<PAD_COUNT;i++)if(g_pads[i].vk)register_pad_hotkey(i);return 0;}
 if(msg==WM_ERASEBKGND)return 1;
 if(msg==WM_PAINT){WL_PAINTSTRUCT ps;WL_HDC dc=g_api.BeginPaint(hwnd,&ps);if(dc)paint_ui(dc);g_api.EndPaint(hwnd,&ps);return 0;}
 if(msg==WM_SIZE&&wp==SIZE_MINIMIZED){add_tray();g_api.ShowWindow(hwnd,SW_HIDE);return 0;}
 if(msg==WM_TRAY){if((WL_UINT)lp==WM_LBUTTONDBLCLK)restore_window();return 0;}
 if(msg==WM_RESTOREAPP){restore_window();return 0;}
 if(msg==WM_LBUTTONDOWN){int x=GET_X_LPARAM(lp),y=GET_Y_LPARAM(lp);Layout l;get_layout(&l);for(int i=0;i<PAD_COUNT;i++)if(pt_in(l.pads[i],x,y)){g_selected=i;g_capturePad=-1;invalidate();return 0;}if(pt_in(l.volKnob,x,y)){g_dragMode=1;g_dragStartY=y;g_dragStartVolume=g_pads[g_selected].volume;g_api.SetCapture(hwnd);return 0;}if(pt_in(l.pitchKnob,x,y)){g_dragMode=2;g_dragStartY=y;g_dragStartPitch=g_pads[g_selected].pitch;g_api.SetCapture(hwnd);return 0;}if(pt_in(l.wave,x,y)&&g_pads[g_selected].mono){g_dragMode=3;g_waveAnchorX=x;g_waveMoved=0;g_api.SetCapture(hwnd);return 0;}if(pt_in(l.loadBtn,x,y)){choose_file_for_pad(g_selected);return 0;}if(pt_in(l.playBtn,x,y)){trigger_pad(g_selected);return 0;}if(pt_in(l.stopBtn,x,y)){stop_all();invalidate();return 0;}if(pt_in(l.hotkeyBtn,x,y)){g_capturePad=g_selected;g_api.SetFocus(hwnd);invalidate();return 0;}if(pt_in(l.clearBtn,x,y)){g_api.UnregisterHotKey(hwnd,HOTKEY_BASE+g_selected);g_pads[g_selected].vk=0;g_pads[g_selected].mods=0;save_config();invalidate();return 0;}return 0;}
 if(msg==WM_MOUSEMOVE&&g_dragMode){int y=GET_Y_LPARAM(lp);if(g_dragMode==1||g_dragMode==2)handle_knob_move(y);else if(g_dragMode==3){Layout l;get_layout(&l);waveform_set_from_x(&l,GET_X_LPARAM(lp),0);}return 0;}
 if(msg==WM_LBUTTONUP&&g_dragMode){if(g_dragMode==1||g_dragMode==2)save_config();else if(g_dragMode==3){Layout l;get_layout(&l);waveform_set_from_x(&l,GET_X_LPARAM(lp),1);}g_dragMode=0;g_api.ReleaseCapture();invalidate();return 0;}
 if(msg==WM_LBUTTONDBLCLK){int x=GET_X_LPARAM(lp),y=GET_Y_LPARAM(lp);Layout l;get_layout(&l);for(int i=0;i<PAD_COUNT;i++)if(pt_in(l.pads[i],x,y)){g_selected=i;if(g_pads[i].mono)trigger_pad(i);else choose_file_for_pad(i);invalidate();return 0;}if(pt_in(l.wave,x,y)&&g_pads[g_selected].mono){g_pads[g_selected].selStart=0;g_pads[g_selected].selEnd=g_pads[g_selected].frames;save_config();invalidate();return 0;}if(pt_in(l.volKnob,x,y)){g_pads[g_selected].volume=1.0f;save_config();invalidate();return 0;}if(pt_in(l.pitchKnob,x,y)){g_pads[g_selected].pitch=0;save_config();invalidate();return 0;}return 0;}
 if(msg==WM_MOUSEWHEEL){WL_POINT pt={(WL_I32)(WL_I16)LOWORD_(lp),(WL_I32)(WL_I16)HIWORD_(lp)};g_api.ScreenToClient(hwnd,&pt);Layout l;get_layout(&l);WL_I32 delta=(WL_I16)HIWORD_(wp);if(pt_in(l.volKnob,pt.x,pt.y)){g_pads[g_selected].volume=fclamp(g_pads[g_selected].volume+(delta>0?0.05f:-0.05f),0,2);save_config();invalidate();return 0;}if(pt_in(l.pitchKnob,pt.x,pt.y)){g_pads[g_selected].pitch=iclamp(g_pads[g_selected].pitch+(delta>0?1:-1),-24,24);save_config();invalidate();return 0;}}
 if((msg==WM_KEYDOWN||msg==WM_SYSKEYDOWN)&&g_capturePad>=0){WL_U32 vk=(WL_U32)wp;if(vk==VK_ESCAPE){g_capturePad=-1;invalidate();return 0;}if(vk==VK_SHIFT||vk==VK_CONTROL||vk==VK_MENU||vk==VK_LWIN||vk==VK_RWIN)return 0;WL_U32 mods=0;if(g_api.GetKeyState(VK_CONTROL)<0)mods|=MOD_CONTROL;if(g_api.GetKeyState(VK_MENU)<0)mods|=MOD_ALT;if(g_api.GetKeyState(VK_SHIFT)<0)mods|=MOD_SHIFT;if(g_api.GetKeyState(VK_LWIN)<0||g_api.GetKeyState(VK_RWIN)<0)mods|=MOD_WIN;int pad=g_capturePad;g_capturePad=-1;g_api.UnregisterHotKey(hwnd,HOTKEY_BASE+pad);WL_U32 ov=g_pads[pad].vk,om=g_pads[pad].mods;g_pads[pad].vk=vk;g_pads[pad].mods=mods;if(!register_pad_hotkey(pad)){g_pads[pad].vk=ov;g_pads[pad].mods=om;register_pad_hotkey(pad);g_api.MessageBoxW(hwnd,(const WL_WCHAR[]){'T','h','a','t',' ','h','o','t','k','e','y',' ','i','s',' ','a','l','r','e','a','d','y',' ','i','n',' ','u','s','e','.',0},WINDOW_TITLE,0x30);}else save_config();invalidate();return 0;}
 if(msg==WM_KEYDOWN&&g_capturePad<0&&wp==VK_SPACE){trigger_pad(g_selected);return 0;}
 if(msg==WM_HOTKEY){int id=(int)wp;if(id>=HOTKEY_BASE&&id<HOTKEY_BASE+PAD_COUNT){trigger_pad(id-HOTKEY_BASE);return 0;}}
 if(msg==WM_CLOSE){g_api.DestroyWindow(hwnd);return 0;}
 if(msg==WM_DESTROY){g_running=0;remove_tray();for(int i=0;i<PAD_COUNT;i++)g_api.UnregisterHotKey(hwnd,HOTKEY_BASE+i);for(int i=0;i<PAD_COUNT;i++)if(g_pads[i].mono){g_api.HeapFree(g_api.GetProcessHeap(),0,g_pads[i].mono);g_pads[i].mono=0;}media_shutdown();destroy_gdi();if(g_mutex){g_api.CloseHandle(g_mutex);g_mutex=0;}g_api.PostQuitMessage(0);return 0;}
 return g_api.DefWindowProcW(hwnd,msg,wp,lp);
}

void WL_CALLBACK entry(void){
 if(!wl_init_gui(&g_api))return;
 static const WL_WCHAR MUTEX_NAME[]={'L','o','c','a','l','\\','A','P','O','S','o','u','n','d','b','o','a','r','d','C','o','n','t','r','o','l','l','e','r','_','v','0','4',0};
 g_mutex=g_api.CreateMutexW(0,WL_TRUE,MUTEX_NAME);if(g_mutex&&g_api.GetLastError()==183){WL_HWND w=g_api.FindWindowW(CLASS_NAME,0);if(w)g_api.PostMessageW(w,WM_RESTOREAPP,0,0);g_api.CloseHandle(g_mutex);g_mutex=0;g_api.ExitProcess(0);}
 media_init();load_config();
 WL_HANDLE srv=g_api.CreateThread(0,0,server_thread,0,0,0);if(srv)g_api.CloseHandle(srv);
 WL_WNDCLASSEXW wc;memset(&wc,0,sizeof(wc));wc.cbSize=sizeof(wc);wc.style=CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS;wc.lpfnWndProc=wndproc;wc.hInstance=(WL_HINSTANCE)g_api.GetModuleHandleW(0);wc.hbrBackground=(WL_HBRUSH)(WL_UPTR)(COLOR_WINDOW+1);wc.lpszClassName=CLASS_NAME;wc.hIcon=g_api.LoadIconW(0,(const WL_WCHAR*)(WL_UPTR)IDI_APPLICATION);wc.hIconSm=wc.hIcon;if(!g_api.RegisterClassExW(&wc))g_api.ExitProcess(2);
 WL_HWND w=g_api.CreateWindowExW(0,CLASS_NAME,WINDOW_TITLE,WS_OVERLAPPEDWINDOW,80,60,1060,760,0,0,wc.hInstance,0);if(!w)g_api.ExitProcess(3);g_api.ShowWindow(w,SW_SHOW);g_api.UpdateWindow(w);WL_MSG m;while(g_api.GetMessageW(&m,0,0,0)>0){g_api.TranslateMessage(&m);g_api.DispatchMessageW(&m);}g_api.ExitProcess(0);
}