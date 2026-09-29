#include "winlite.h"

#define PAD_COUNT 16
#define MAX_PATH_W 260
#define MAX_VOICES 32
#define HOTKEY_BASE 1000
#define ID_PAD_BASE 100
#define ID_LOAD 300
#define ID_HOTKEY 301
#define ID_CLEAR_HOTKEY 302
#define ID_VOL_DOWN 303
#define ID_VOL_UP 304
#define ID_STOP_ALL 305
#define ID_ABOUT 306
#define ID_PITCH_DOWN 307
#define ID_PITCH_UP 308
#define ID_FULL_RANGE 309

#define WM_CREATE 0x0001u
#define WM_DESTROY 0x0002u
#define WM_COMMAND 0x0111u
#define WM_KEYDOWN 0x0100u
#define WM_SYSKEYDOWN 0x0104u
#define WM_HOTKEY 0x0312u
#define WM_TIMER 0x0113u
#define WM_SETFONT 0x0030u
#define WM_PAINT 0x000Fu
#define WM_MOUSEMOVE 0x0200u
#define WM_LBUTTONDOWN 0x0201u
#define WM_LBUTTONUP 0x0202u
#define WM_LBUTTONDBLCLK 0x0203u
#define MK_LBUTTON 0x0001u
#define CS_DBLCLKS 0x0008u
#define BN_CLICKED 0
#define BN_DOUBLECLICKED 5
#define BS_NOTIFY 0x00004000u
#define WS_OVERLAPPEDWINDOW 0x00CF0000u
#define WS_CHILD 0x40000000u
#define WS_VISIBLE 0x10000000u
#define BS_PUSHBUTTON 0x00000000u
#define BS_MULTILINE 0x00002000u
#define SS_LEFT 0x00000000u
#define SW_SHOW 5
#define COLOR_WINDOW 5
#define DEFAULT_GUI_FONT 17
#define MOD_ALT 0x0001u
#define MOD_CONTROL 0x0002u
#define MOD_SHIFT 0x0004u
#define MOD_WIN 0x0008u
#define MOD_NOREPEAT 0x4000u
#define VK_SHIFT 0x10u
#define VK_CONTROL 0x11u
#define VK_MENU 0x12u
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
#define PIPE_UNLIMITED_INSTANCES 255u
#define ERROR_PIPE_CONNECTED 535u
#define SDDL_REVISION_1 1u
#define WAVE_PEAKS 512
#define MAX_PAD_SAMPLE_BYTES (128ull*1024ull*1024ull)
#define MAX_TOTAL_SAMPLE_BYTES (512ull*1024ull*1024ull)
#define MAX_PAD_FRAMES (MAX_PAD_SAMPLE_BYTES/sizeof(float))
#define WM_APP 0x8000u
#define WM_CONN_UPDATE (WM_APP+1u)
#define DIAG_TIMER_ID 77u
#define MOVEFILE_REPLACE_EXISTING 0x00000001u
#define MOVEFILE_WRITE_THROUGH 0x00000008u
#define TH32CS_SNAPTHREAD 0x00000004u
#define GR_GDIOBJECTS 0u
#define GR_USEROBJECTS 1u
#define ALL_PROCESSOR_GROUPS 0xffffu
#define WAVE_X 15
#define WAVE_Y 438
#define WAVE_W 710
#define WAVE_H 92
#define VOL_X 135
#define VOL_Y 610
#define VOL_W 220
#define VOL_H 18
#define PITCH_X 400
#define PITCH_Y 610
#define PITCH_W 220
#define PITCH_H 18
#define RGB_(r,g,b) ((WL_DWORD)((r)|((g)<<8)|((b)<<16)))

#define LOWORD_(x) ((WL_U16)((WL_UPTR)(x)&0xffffu))
#define HIWORD_(x) ((WL_U16)(((WL_UPTR)(x)>>16)&0xffffu))

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
static WL_HWND g_main=0,g_padButtons[PAD_COUNT],g_selectedText=0,g_statusText=0,g_connText=0,g_diagText=0;
static WL_HFONT g_font=0;
static WL_HBRUSH g_waveBg=0,g_waveSel=0,g_waveBorder=0;
static WL_HPEN g_wavePen=0,g_waveMidPen=0;
static volatile WL_I32 g_running=1;
static volatile WL_I32 g_connections=0;
static volatile WL_I32 g_padLock=0;
static WL_U64 g_totalSampleBytes=0;
static volatile WL_I64 g_triggerSerial[PAD_COUNT];
static volatile WL_I64 g_stopSerial=0;
static WL_I32 g_selected=0;
static WL_I32 g_capturePad=-1;
static WL_I32 g_waveDragging=0;
static float g_waveAnchor=0.0f;
static WL_I32 g_controlDragging=0; /* 1=volume, 2=pitch */
static WL_I32 g_hotkeyState[PAD_COUNT]; /* 0=none, 1=registered, -1=conflict/rejected */
static WL_I32 g_backupState=0; /* 0=none, 1=ready, 2=restored, 3=error */
static WL_I32 g_loadedFromBackup=0;
static WL_U64 g_diagStartTick=0,g_diagLastTick=0,g_diagLastCpu=0;
static WL_I32 g_diagCpuPct=0;

static const WL_WCHAR CLASS_NAME[]={'A','P','O','S','o','u','n','d','b','o','a','r','d','C','t','r','l',0};
static const WL_WCHAR WINDOW_TITLE[]={'A','P','O',' ','S','o','u','n','d','b','o','a','r','d',' ','C','o','n','t','r','o','l','l','e','r',' ','v','0','.','6','.','2',0};
static const WL_WCHAR BTN_CLASS[]={'B','U','T','T','O','N',0};
static const WL_WCHAR STATIC_CLASS[]={'S','T','A','T','I','C',0};


typedef struct { WL_DWORD lo,hi; } DIAG_FILETIME;
typedef struct { WL_DWORD dwSize,cntUsage,th32ThreadID,th32OwnerProcessID; WL_I32 tpBasePri,tpDeltaPri; WL_DWORD dwFlags; } DIAG_THREADENTRY32;
typedef struct { WL_DWORD cb,PageFaultCount; WL_SIZE_T PeakWorkingSetSize,WorkingSetSize,QuotaPeakPagedPoolUsage,QuotaPagedPoolUsage,QuotaPeakNonPagedPoolUsage,QuotaNonPagedPoolUsage,PagefileUsage,PeakPagefileUsage; } DIAG_PMC;
typedef WL_UPTR (WL_WINAPI *PFN_SetTimer)(WL_HWND,WL_UPTR,WL_UINT,void*);
typedef WL_BOOL (WL_WINAPI *PFN_KillTimer)(WL_HWND,WL_UPTR);
typedef WL_HANDLE (WL_WINAPI *PFN_GetCurrentProcess)(void);
typedef WL_DWORD (WL_WINAPI *PFN_GetCurrentProcessId)(void);
typedef WL_BOOL (WL_WINAPI *PFN_GetProcessHandleCount)(WL_HANDLE,WL_DWORD*);
typedef WL_BOOL (WL_WINAPI *PFN_GetProcessTimes)(WL_HANDLE,DIAG_FILETIME*,DIAG_FILETIME*,DIAG_FILETIME*,DIAG_FILETIME*);
typedef WL_U64 (WL_WINAPI *PFN_GetTickCount64)(void);
typedef WL_DWORD (WL_WINAPI *PFN_GetActiveProcessorCount)(WL_WORD);
typedef WL_BOOL (WL_WINAPI *PFN_K32GetProcessMemoryInfo)(WL_HANDLE,DIAG_PMC*,WL_DWORD);
typedef WL_DWORD (WL_WINAPI *PFN_GetGuiResources)(WL_HANDLE,WL_DWORD);
typedef WL_HANDLE (WL_WINAPI *PFN_CreateToolhelp32Snapshot)(WL_DWORD,WL_DWORD);
typedef WL_BOOL (WL_WINAPI *PFN_Thread32First)(WL_HANDLE,DIAG_THREADENTRY32*);
typedef WL_BOOL (WL_WINAPI *PFN_Thread32Next)(WL_HANDLE,DIAG_THREADENTRY32*);
typedef WL_BOOL (WL_WINAPI *PFN_CopyFileW)(const WL_WCHAR*,const WL_WCHAR*,WL_BOOL);
typedef WL_BOOL (WL_WINAPI *PFN_MoveFileExW)(const WL_WCHAR*,const WL_WCHAR*,WL_DWORD);
typedef WL_BOOL (WL_WINAPI *PFN_DeleteFileW)(const WL_WCHAR*);
typedef WL_BOOL (WL_WINAPI *PFN_FlushFileBuffers)(WL_HANDLE);
static PFN_SetTimer pSetTimer=0; static PFN_KillTimer pKillTimer=0; static PFN_GetCurrentProcess pGetCurrentProcess=0; static PFN_GetCurrentProcessId pGetCurrentProcessId=0;
static PFN_GetProcessHandleCount pGetProcessHandleCount=0; static PFN_GetProcessTimes pGetProcessTimes=0; static PFN_GetTickCount64 pGetTickCount64=0; static PFN_GetActiveProcessorCount pGetActiveProcessorCount=0;
static PFN_K32GetProcessMemoryInfo pK32GetProcessMemoryInfo=0; static PFN_GetGuiResources pGetGuiResources=0; static PFN_CreateToolhelp32Snapshot pCreateToolhelp32Snapshot=0; static PFN_Thread32First pThread32First=0; static PFN_Thread32Next pThread32Next=0;
static PFN_CopyFileW pCopyFileW=0; static PFN_MoveFileExW pMoveFileExW=0; static PFN_DeleteFileW pDeleteFileW=0; static PFN_FlushFileBuffers pFlushFileBuffers=0;
static void init_optional_apis(void){
 pSetTimer=(PFN_SetTimer)wl_get_proc(g_api.user32,"SetTimer"); pKillTimer=(PFN_KillTimer)wl_get_proc(g_api.user32,"KillTimer"); pGetGuiResources=(PFN_GetGuiResources)wl_get_proc(g_api.user32,"GetGuiResources");
 pGetCurrentProcess=(PFN_GetCurrentProcess)wl_get_proc(g_api.kernel32,"GetCurrentProcess"); pGetCurrentProcessId=(PFN_GetCurrentProcessId)wl_get_proc(g_api.kernel32,"GetCurrentProcessId"); pGetProcessHandleCount=(PFN_GetProcessHandleCount)wl_get_proc(g_api.kernel32,"GetProcessHandleCount");
 pGetProcessTimes=(PFN_GetProcessTimes)wl_get_proc(g_api.kernel32,"GetProcessTimes"); pGetTickCount64=(PFN_GetTickCount64)wl_get_proc(g_api.kernel32,"GetTickCount64"); pGetActiveProcessorCount=(PFN_GetActiveProcessorCount)wl_get_proc(g_api.kernel32,"GetActiveProcessorCount");
 pK32GetProcessMemoryInfo=(PFN_K32GetProcessMemoryInfo)wl_get_proc(g_api.kernel32,"K32GetProcessMemoryInfo"); pCreateToolhelp32Snapshot=(PFN_CreateToolhelp32Snapshot)wl_get_proc(g_api.kernel32,"CreateToolhelp32Snapshot"); pThread32First=(PFN_Thread32First)wl_get_proc(g_api.kernel32,"Thread32First"); pThread32Next=(PFN_Thread32Next)wl_get_proc(g_api.kernel32,"Thread32Next");
 pCopyFileW=(PFN_CopyFileW)wl_get_proc(g_api.kernel32,"CopyFileW"); pMoveFileExW=(PFN_MoveFileExW)wl_get_proc(g_api.kernel32,"MoveFileExW"); pDeleteFileW=(PFN_DeleteFileW)wl_get_proc(g_api.kernel32,"DeleteFileW"); pFlushFileBuffers=(PFN_FlushFileBuffers)wl_get_proc(g_api.kernel32,"FlushFileBuffers");
 if(pGetTickCount64)g_diagStartTick=pGetTickCount64();
}

/* Helpers. */
static WL_WCHAR* wend(WL_WCHAR*s){while(*s)s++;return s;}
static void wadd_ascii(WL_WCHAR*d,WL_SIZE_T cap,const char*s){WL_SIZE_T n=wl_wlen(d),i=0;while(n+1<cap&&s&&s[i])d[n++]=(WL_U8)s[i++];d[n]=0;}
static void wadd_num(WL_WCHAR*d,WL_SIZE_T cap,int v){WL_WCHAR t[16];int n=0;if(v==0)t[n++]='0';else{if(v<0){wl_wcat(d,(const WL_WCHAR[]){'-',0},cap);v=-v;}while(v&&n<15){t[n++]=(WL_WCHAR)('0'+v%10);v/=10;}}while(n--&&wl_wlen(d)+1<cap){WL_SIZE_T q=wl_wlen(d);d[q]=t[n];d[q+1]=0;}}
static void wset_ascii(WL_WCHAR*d,WL_SIZE_T cap,const char*s){if(cap)d[0]=0;wadd_ascii(d,cap,s);}
static const WL_WCHAR* base_name(const WL_WCHAR* p){const WL_WCHAR*b=p;if(!p)return p;for(const WL_WCHAR*s=p;*s;s++)if(*s=='\\'||*s=='/')b=s+1;return b;}
static void spin_lock(void){while(1){WL_I32 expect=0;if(__atomic_compare_exchange_n(&g_padLock,&expect,1,0,__ATOMIC_ACQ_REL,__ATOMIC_ACQUIRE))return;g_api.Sleep(0);}}
static void spin_unlock(void){__atomic_store_n(&g_padLock,0,__ATOMIC_RELEASE);}
static float fclamp(float x,float a,float b){return x<a?a:(x>b?b:x);}
static WL_U16 rd16(const WL_U8*p){return (WL_U16)(p[0]|((WL_U16)p[1]<<8));}
static WL_U32 rd32(const WL_U8*p){return (WL_U32)p[0]|((WL_U32)p[1]<<8)|((WL_U32)p[2]<<16)|((WL_U32)p[3]<<24);}
static WL_I32 rd24s(const WL_U8*p){WL_I32 v=(WL_I32)((WL_U32)p[0]|((WL_U32)p[1]<<8)|((WL_U32)p[2]<<16));if(v&0x00800000)v|=(WL_I32)0xff000000;return v;}
static int fourcc(const WL_U8*p,const char*s){return p[0]==(WL_U8)s[0]&&p[1]==(WL_U8)s[1]&&p[2]==(WL_U8)s[2]&&p[3]==(WL_U8)s[3];}

static void wadd_u64(WL_WCHAR*d,WL_SIZE_T cap,WL_U64 v){WL_WCHAR t[32];int n=0;if(!v)t[n++]='0';else while(v&&n<31){t[n++]=(WL_WCHAR)('0'+(v%10));v/=10;}while(n--&&wl_wlen(d)+1<cap){WL_SIZE_T q=wl_wlen(d);d[q]=t[n];d[q+1]=0;}}
static void wadd_2(WL_WCHAR*d,WL_SIZE_T cap,int v){if(v<10)wadd_ascii(d,cap,"0");wadd_num(d,cap,v);}
static WL_U32 crc32_bytes(const WL_U8*p,WL_SIZE_T n){WL_U32 c=0xffffffffu;while(n--){c^=*p++;for(int k=0;k<8;k++)c=(c>>1)^((c&1)?0xedb88320u:0u);}return c^0xffffffffu;}

/* Sample storage + decoding. */
typedef struct {
 WL_WCHAR path[MAX_PATH_W];
 float* mono;
 WL_U64 frames;
 WL_U32 sampleRate;
 float volume;
 WL_I32 pitch;
 float selStart,selEnd;
 float peaks[WAVE_PEAKS];
 WL_U32 vk,mods;
} Pad;
static Pad g_pads[PAD_COUNT];

static const double PITCH_RATIO[49]={
 0.250000000000,0.264865773590,0.280615512077,0.297301778751,0.314980262474,0.333709963543,0.353553390593,
 0.374576769219,0.396850262992,0.420448207627,0.445449359070,0.471937156341,0.500000000000,0.529731547180,
 0.561231024155,0.594603557501,0.629960524947,0.667419927085,0.707106781187,0.749153538438,0.793700525984,
 0.840896415254,0.890898718140,0.943874312682,1.000000000000,1.059463094359,1.122462048309,1.189207115003,
 1.259921049895,1.334839854170,1.414213562373,1.498307076877,1.587401051968,1.681792830507,1.781797436281,
 1.887748625363,2.000000000000,2.118926188719,2.244924096619,2.378414230005,2.519842099790,2.669679708340,
 2.828427124746,2.996614153753,3.174802103936,3.363585661015,3.563594872561,3.775497250727,4.000000000000
};
static double pitch_ratio(WL_I32 semi){if(semi<-24)semi=-24;if(semi>24)semi=24;return PITCH_RATIO[semi+24];}

typedef struct { WL_U32 magic,version; } ConfigHeader;
typedef struct { WL_WCHAR path[MAX_PATH_W]; WL_U32 vk,mods; float volume; } PadDiskV1;
typedef struct { WL_WCHAR path[MAX_PATH_W]; WL_U32 vk,mods; float volume; WL_I32 pitch; float selStart,selEnd; } PadDiskV2;
#define CFG_MAGIC 0x31424653u
static const WL_WCHAR ENV_LOCALAPPDATA[]={'L','O','C','A','L','A','P','P','D','A','T','A',0};
static const WL_WCHAR APP_DIR_SUFFIX[]={92,'A','P','O','S','o','u','n','d','b','o','a','r','d',0};
static const WL_WCHAR CFG_OLD_SUFFIX[]={92,'c','o','n','f','i','g','.','b','i','n',0};
static const WL_WCHAR CFG_NEW_SUFFIX[]={92,'c','o','n','f','i','g','_','v','0','5','.','b','i','n',0};
static const WL_WCHAR CFG_BAK_EXT[]={'.','b','a','k',0};
static const WL_WCHAR CFG_TMP_EXT[]={'.','t','m','p',0};
static int config_path(WL_WCHAR*out,WL_SIZE_T cap,int old){WL_WCHAR dir[512];WL_DWORD n=g_api.GetEnvironmentVariableW(ENV_LOCALAPPDATA,dir,500);if(!n||n>=500)return 0;wl_wcat(dir,APP_DIR_SUFFIX,512);g_api.CreateDirectoryW(dir,0);wl_wcpy(out,dir,cap);wl_wcat(out,old?CFG_OLD_SUFFIX:CFG_NEW_SUFFIX,cap);return 1;}
static int cfg_file_exists(const WL_WCHAR*path){WL_HANDLE h=g_api.CreateFileW(path,GENERIC_READ,1,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);if(h==WL_INVALID_HANDLE_VALUE)return 0;g_api.CloseHandle(h);return 1;}
static int cfg_write_exact(WL_HANDLE h,const void*p,WL_DWORD bytes){WL_DWORD wr=0;return g_api.WriteFile(h,p,bytes,&wr,0)&&wr==bytes;}
static int cfg_write_file(const WL_WCHAR*path){WL_SIZE_T diskBytes=(WL_SIZE_T)sizeof(PadDiskV2)*PAD_COUNT;PadDiskV2*disk=(PadDiskV2*)g_api.HeapAlloc(g_api.GetProcessHeap(),0,diskBytes);if(!disk)return 0;for(int i=0;i<PAD_COUNT;i++){memset(&disk[i],0,sizeof(disk[i]));spin_lock();wl_wcpy(disk[i].path,g_pads[i].path,MAX_PATH_W);disk[i].vk=g_pads[i].vk;disk[i].mods=g_pads[i].mods;disk[i].volume=g_pads[i].volume;disk[i].pitch=g_pads[i].pitch;disk[i].selStart=g_pads[i].selStart;disk[i].selEnd=g_pads[i].selEnd;spin_unlock();}ConfigHeader ch={CFG_MAGIC,3};WL_U32 crc=crc32_bytes((const WL_U8*)disk,diskBytes);WL_HANDLE h=g_api.CreateFileW(path,GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);if(h==WL_INVALID_HANDLE_VALUE){g_api.HeapFree(g_api.GetProcessHeap(),0,disk);return 0;}int ok=cfg_write_exact(h,&ch,sizeof(ch))&&cfg_write_exact(h,disk,(WL_DWORD)diskBytes)&&cfg_write_exact(h,&crc,sizeof(crc));if(ok&&pFlushFileBuffers)ok=pFlushFileBuffers(h)?1:0;g_api.CloseHandle(h);g_api.HeapFree(g_api.GetProcessHeap(),0,disk);return ok;}
static void save_config(void){WL_WCHAR path[600],tmp[610],bak[610];if(!config_path(path,600,0))return;wl_wcpy(tmp,path,610);wl_wcat(tmp,CFG_TMP_EXT,610);wl_wcpy(bak,path,610);wl_wcat(bak,CFG_BAK_EXT,610);if(!cfg_write_file(tmp)){g_backupState=3;if(pDeleteFileW)pDeleteFileW(tmp);return;}if(cfg_file_exists(path)&&!g_loadedFromBackup&&pCopyFileW)pCopyFileW(path,bak,WL_FALSE);int replaced=0;if(pMoveFileExW)replaced=pMoveFileExW(tmp,path,MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)?1:0;if(!replaced){if(pDeleteFileW)pDeleteFileW(tmp);g_backupState=3;return;}g_loadedFromBackup=0;if(!cfg_file_exists(bak)&&pCopyFileW)pCopyFileW(path,bak,WL_FALSE);g_backupState=cfg_file_exists(bak)?1:0;}

static void compute_peaks(const float*mono,WL_U64 frames,float*out){for(int i=0;i<WAVE_PEAKS;i++)out[i]=0;if(!mono||!frames)return;for(WL_U64 f=0;f<frames;f++){WL_U64 bi=(f*(WL_U64)WAVE_PEAKS)/frames;if(bi>=WAVE_PEAKS)bi=WAVE_PEAKS-1;float v=mono[f];if(v<0)v=-v;if(v>out[bi])out[bi]=v;}for(int i=0;i<WAVE_PEAKS;i++)if(out[i]>1.0f)out[i]=1.0f;}
static int commit_sample(int idx,const WL_WCHAR*path,float*mono,WL_U64 frames,WL_U32 sr){if(!mono||!frames||!sr)return 0;WL_U64 newBytes=frames*sizeof(float);if(newBytes>MAX_PAD_SAMPLE_BYTES){g_api.HeapFree(g_api.GetProcessHeap(),0,mono);return 0;}float peaks[WAVE_PEAKS];compute_peaks(mono,frames,peaks);spin_lock();float*old=g_pads[idx].mono;WL_U64 oldBytes=old?g_pads[idx].frames*sizeof(float):0;if(g_totalSampleBytes-oldBytes+newBytes>MAX_TOTAL_SAMPLE_BYTES){spin_unlock();g_api.HeapFree(g_api.GetProcessHeap(),0,mono);return 0;}g_totalSampleBytes=g_totalSampleBytes-oldBytes+newBytes;g_pads[idx].mono=mono;g_pads[idx].frames=frames;g_pads[idx].sampleRate=sr;g_pads[idx].selStart=0.0f;g_pads[idx].selEnd=1.0f;memcpy(g_pads[idx].peaks,peaks,sizeof(peaks));wl_wcpy(g_pads[idx].path,path,MAX_PATH_W);spin_unlock();if(old)g_api.HeapFree(g_api.GetProcessHeap(),0,old);return 1;}

static int decode_wav(const WL_WCHAR*path,float**out,WL_U64*outFrames,WL_U32*outRate){*out=0;*outFrames=0;*outRate=0;WL_HANDLE h=g_api.CreateFileW(path,GENERIC_READ,1,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);if(h==WL_INVALID_HANDLE_VALUE)return 0;WL_I64 size=0;if(!g_api.GetFileSizeEx(h,&size)||size<44||size>(WL_I64)(512*1024*1024)){g_api.CloseHandle(h);return 0;}WL_U8*bytes=(WL_U8*)g_api.HeapAlloc(g_api.GetProcessHeap(),0,(WL_SIZE_T)size);if(!bytes){g_api.CloseHandle(h);return 0;}WL_DWORD total=0;while(total<(WL_U32)size){WL_DWORD n=0;if(!g_api.ReadFile(h,bytes+total,(WL_DWORD)size-total,&n,0)||!n)break;total+=n;}g_api.CloseHandle(h);if(total!=(WL_U32)size||!fourcc(bytes,"RIFF")||!fourcc(bytes+8,"WAVE")){g_api.HeapFree(g_api.GetProcessHeap(),0,bytes);return 0;}
 WL_U16 tag=0,ch=0,bits=0,block=0;WL_U32 sr=0;WL_U8*data=0;WL_U32 dataBytes=0;WL_U32 pos=12;while(pos+8<=(WL_U32)size){WL_U8*c=bytes+pos;WL_U32 cs=rd32(c+4);WL_U32 body=pos+8;if(body+cs>(WL_U32)size)break;if(fourcc(c,"fmt ")&&cs>=16){tag=rd16(bytes+body);ch=rd16(bytes+body+2);sr=rd32(bytes+body+4);block=rd16(bytes+body+12);bits=rd16(bytes+body+14);if(tag==0xfffe&&cs>=40)tag=rd16(bytes+body+24);}else if(fourcc(c,"data")){data=bytes+body;dataBytes=cs;}pos=body+cs+(cs&1u);}
 WL_U32 bps=(bits+7)/8;if(!data||!ch||!sr||!block||!bits||!(tag==1||tag==3)||!bps||block<(WL_U32)ch*bps||((tag==1)&&!(bits==8||bits==16||bits==24||bits==32))||((tag==3)&&bits!=32)){g_api.HeapFree(g_api.GetProcessHeap(),0,bytes);return 0;}WL_U64 frames=dataBytes/block;if(!frames||frames>MAX_PAD_FRAMES){g_api.HeapFree(g_api.GetProcessHeap(),0,bytes);return 0;}float*mono=(float*)g_api.HeapAlloc(g_api.GetProcessHeap(),0,(WL_SIZE_T)frames*sizeof(float));if(!mono){g_api.HeapFree(g_api.GetProcessHeap(),0,bytes);return 0;}
 for(WL_U64 f=0;f<frames;f++){float sum=0;int used=0;for(WL_U16 c=0;c<ch;c++){const WL_U8*q=data+f*block+c*bps;float v=0;if(tag==3&&bits==32){union{WL_U32 u;float f;}u;u.u=rd32(q);v=u.f;}else if(tag==1&&bits==8)v=((int)q[0]-128)/128.0f;else if(tag==1&&bits==16){WL_I16 z=(WL_I16)rd16(q);v=(float)z/32768.0f;}else if(tag==1&&bits==24)v=(float)rd24s(q)/8388608.0f;else if(tag==1&&bits==32){WL_I32 z=(WL_I32)rd32(q);v=(float)z/2147483648.0f;}else{g_api.HeapFree(g_api.GetProcessHeap(),0,mono);g_api.HeapFree(g_api.GetProcessHeap(),0,bytes);return 0;}if(c<2){sum+=v;used++;}}mono[f]=used?sum/(float)used:0;}
 g_api.HeapFree(g_api.GetProcessHeap(),0,bytes);*out=mono;*outFrames=frames;*outRate=sr;return 1;}

/* Windows Media Foundation decoder: MP3, AAC/M4A, WMA, FLAC and other installed MF codecs. */
typedef struct { WL_U32 Data1; WL_U16 Data2,Data3; WL_U8 Data4[8]; } MF_GUID;
typedef struct { void** lpVtbl; } MF_OBJ;
typedef WL_I32 (WL_WINAPI *PFN_MFStartup)(WL_U32,WL_DWORD);
typedef WL_I32 (WL_WINAPI *PFN_MFShutdown)(void);
typedef WL_I32 (WL_WINAPI *PFN_MFCreateMediaType)(MF_OBJ**);
typedef WL_I32 (WL_WINAPI *PFN_MFCreateSourceReaderFromURL)(const WL_WCHAR*,MF_OBJ*,MF_OBJ**);
typedef WL_I32 (WL_WINAPI *PFN_CoInitializeEx)(void*,WL_DWORD);
typedef void (WL_WINAPI *PFN_CoUninitialize)(void);
static void* g_mfplat=0,*g_mfrw=0,*g_ole32=0;static PFN_MFStartup g_MFStartup=0;static PFN_MFShutdown g_MFShutdown=0;static PFN_MFCreateMediaType g_MFCreateMediaType=0;static PFN_MFCreateSourceReaderFromURL g_MFCreateReader=0;static PFN_CoInitializeEx g_CoInitializeEx=0;static PFN_CoUninitialize g_CoUninitialize=0;static int g_mfState=0,g_mfStarted=0,g_comInitialized=0;
static const MF_GUID G_MT_MAJOR={0x48eba18e,0xf8c9,0x4687,{0xbf,0x11,0x0a,0x74,0xc9,0xf9,0x6a,0x8f}};
static const MF_GUID G_MT_SUBTYPE={0xf7e34c9a,0x42e8,0x4714,{0xb7,0x4b,0xcb,0x29,0xd7,0x2c,0x35,0xe5}};
static const MF_GUID G_MT_CH={0x37e48bf5,0x645e,0x4c5b,{0x89,0xde,0xad,0xa9,0xe2,0x9b,0x69,0x6a}};
static const MF_GUID G_MT_SR={0x5faeeae7,0x0290,0x4c31,{0x9e,0x8a,0xc5,0x34,0xf6,0x8d,0x9d,0xba}};
static const MF_GUID G_AUDIO={0x73647561,0x0000,0x0010,{0x80,0x00,0x00,0xaa,0x00,0x38,0x9b,0x71}};
static const MF_GUID G_FLOAT={0x00000003,0x0000,0x0010,{0x80,0x00,0x00,0xaa,0x00,0x38,0x9b,0x71}};
static const WL_WCHAR MFPLAT_DLL[]={'m','f','p','l','a','t','.','d','l','l',0};
static const WL_WCHAR MFRW_DLL[]={'m','f','r','e','a','d','w','r','i','t','e','.','d','l','l',0};
static const WL_WCHAR OLE32_DLL[]={'o','l','e','3','2','.','d','l','l',0};
static void mf_shutdown(void){if(g_mfStarted&&g_MFShutdown)g_MFShutdown();g_mfStarted=0;if(g_comInitialized&&g_CoUninitialize)g_CoUninitialize();g_comInitialized=0;if(g_mfrw&&g_api.FreeLibrary)g_api.FreeLibrary((WL_HMODULE)g_mfrw);if(g_mfplat&&g_api.FreeLibrary)g_api.FreeLibrary((WL_HMODULE)g_mfplat);if(g_ole32&&g_api.FreeLibrary)g_api.FreeLibrary((WL_HMODULE)g_ole32);g_mfrw=g_mfplat=g_ole32=0;g_mfState=0;}
static int mf_init(void){if(g_mfState)return g_mfState>0;g_mfState=-1;g_mfplat=g_api.LoadLibraryW(MFPLAT_DLL);g_mfrw=g_api.LoadLibraryW(MFRW_DLL);g_ole32=g_api.LoadLibraryW(OLE32_DLL);if(!g_mfplat||!g_mfrw){mf_shutdown();g_mfState=-1;return 0;}g_MFStartup=(PFN_MFStartup)wl_get_proc(g_mfplat,"MFStartup");g_MFShutdown=(PFN_MFShutdown)wl_get_proc(g_mfplat,"MFShutdown");g_MFCreateMediaType=(PFN_MFCreateMediaType)wl_get_proc(g_mfplat,"MFCreateMediaType");g_MFCreateReader=(PFN_MFCreateSourceReaderFromURL)wl_get_proc(g_mfrw,"MFCreateSourceReaderFromURL");if(g_ole32){g_CoInitializeEx=(PFN_CoInitializeEx)wl_get_proc(g_ole32,"CoInitializeEx");g_CoUninitialize=(PFN_CoUninitialize)wl_get_proc(g_ole32,"CoUninitialize");}if(!g_MFStartup||!g_MFShutdown||!g_MFCreateMediaType||!g_MFCreateReader){mf_shutdown();g_mfState=-1;return 0;}if(g_CoInitializeEx){WL_I32 chr=g_CoInitializeEx(0,0);if(chr>=0)g_comInitialized=1;}if(g_MFStartup(0x00020070u,0)<0){mf_shutdown();g_mfState=-1;return 0;}g_mfStarted=1;g_mfState=1;return 1;}
static WL_U32 mf_release(MF_OBJ*o){if(!o)return 0;typedef WL_U32(WL_WINAPI*F)(MF_OBJ*);return ((F)o->lpVtbl[2])(o);}
static WL_I32 mf_set_guid(MF_OBJ*o,const MF_GUID*k,const MF_GUID*v){typedef WL_I32(WL_WINAPI*F)(MF_OBJ*,const MF_GUID*,const MF_GUID*);return ((F)o->lpVtbl[24])(o,k,v);}
static WL_I32 mf_get_u32(MF_OBJ*o,const MF_GUID*k,WL_U32*v){typedef WL_I32(WL_WINAPI*F)(MF_OBJ*,const MF_GUID*,WL_U32*);return ((F)o->lpVtbl[7])(o,k,v);}
static WL_I32 reader_set_type(MF_OBJ*r,WL_U32 stream,MF_OBJ*t){typedef WL_I32(WL_WINAPI*F)(MF_OBJ*,WL_U32,WL_DWORD*,MF_OBJ*);return ((F)r->lpVtbl[7])(r,stream,0,t);}
static WL_I32 reader_get_type(MF_OBJ*r,WL_U32 stream,MF_OBJ**t){typedef WL_I32(WL_WINAPI*F)(MF_OBJ*,WL_U32,MF_OBJ**);return ((F)r->lpVtbl[6])(r,stream,t);}
static WL_I32 reader_read(MF_OBJ*r,WL_U32 stream,WL_U32*flags,MF_OBJ**sample){typedef WL_I32(WL_WINAPI*F)(MF_OBJ*,WL_U32,WL_DWORD,WL_U32*,WL_U32*,WL_I64*,MF_OBJ**);WL_U32 actual=0;WL_I64 ts=0;return ((F)r->lpVtbl[9])(r,stream,0,&actual,flags,&ts,sample);}
static WL_I32 sample_buffer(MF_OBJ*s,MF_OBJ**b){typedef WL_I32(WL_WINAPI*F)(MF_OBJ*,MF_OBJ**);return ((F)s->lpVtbl[41])(s,b);}
static WL_I32 buffer_lock(MF_OBJ*b,WL_U8**p,WL_U32*cur){typedef WL_I32(WL_WINAPI*F)(MF_OBJ*,WL_U8**,WL_U32*,WL_U32*);WL_U32 max=0;return ((F)b->lpVtbl[3])(b,p,&max,cur);}
static void buffer_unlock(MF_OBJ*b){typedef WL_I32(WL_WINAPI*F)(MF_OBJ*);((F)b->lpVtbl[4])(b);}
static int decode_mf(const WL_WCHAR*path,float**out,WL_U64*outFrames,WL_U32*outRate){*out=0;*outFrames=0;*outRate=0;if(!mf_init())return 0;MF_OBJ*r=0,*mt=0,*cur=0;if(g_MFCreateReader(path,0,&r)<0||!r)return 0;if(g_MFCreateMediaType(&mt)<0||!mt){mf_release(r);return 0;}if(mf_set_guid(mt,&G_MT_MAJOR,&G_AUDIO)<0||mf_set_guid(mt,&G_MT_SUBTYPE,&G_FLOAT)<0||reader_set_type(r,0xfffffffdu,mt)<0){mf_release(mt);mf_release(r);return 0;}mf_release(mt);mt=0;if(reader_get_type(r,0xfffffffdu,&cur)<0||!cur){mf_release(r);return 0;}WL_U32 sr=0,ch=0;if(mf_get_u32(cur,&G_MT_SR,&sr)<0||mf_get_u32(cur,&G_MT_CH,&ch)<0||!sr||!ch||ch>32){mf_release(cur);mf_release(r);return 0;}mf_release(cur);cur=0;
 WL_U64 cap=262144,count=0;float*mono=(float*)g_api.HeapAlloc(g_api.GetProcessHeap(),0,(WL_SIZE_T)cap*sizeof(float));if(!mono){mf_release(r);return 0;}int ok=1;for(;;){WL_U32 flags=0;MF_OBJ*samp=0;WL_I32 hr=reader_read(r,0xfffffffdu,&flags,&samp);if(hr<0){ok=0;break;}if(samp){MF_OBJ*buf=0;if(sample_buffer(samp,&buf)<0||!buf){mf_release(samp);ok=0;break;}WL_U8*data=0;WL_U32 bytes=0;if(buffer_lock(buf,&data,&bytes)<0||!data){mf_release(buf);mf_release(samp);ok=0;break;}WL_U64 fr=bytes/((WL_U64)ch*4u);if(count+fr>MAX_PAD_FRAMES){buffer_unlock(buf);mf_release(buf);mf_release(samp);ok=0;break;}if(count+fr>cap){WL_U64 nc=cap;while(nc<count+fr)nc*=2;if(nc>MAX_PAD_FRAMES)nc=MAX_PAD_FRAMES;float*n=(float*)g_api.HeapAlloc(g_api.GetProcessHeap(),0,(WL_SIZE_T)nc*sizeof(float));if(!n){buffer_unlock(buf);mf_release(buf);mf_release(samp);ok=0;break;}memcpy(n,mono,(WL_SIZE_T)count*sizeof(float));g_api.HeapFree(g_api.GetProcessHeap(),0,mono);mono=n;cap=nc;}float*f=(float*)data;for(WL_U64 i=0;i<fr;i++){float sum=0;WL_U32 used=ch<2?ch:2;for(WL_U32 c=0;c<used;c++)sum+=f[i*ch+c];mono[count+i]=sum/(float)used;}count+=fr;buffer_unlock(buf);mf_release(buf);mf_release(samp);}if(flags&0x2u)break;}mf_release(r);if(!ok||!count){g_api.HeapFree(g_api.GetProcessHeap(),0,mono);return 0;}*out=mono;*outFrames=count;*outRate=sr;return 1;}

static int load_audio_into_pad(int idx,const WL_WCHAR*path){float*mono=0;WL_U64 frames=0;WL_U32 sr=0;if(!decode_wav(path,&mono,&frames,&sr)){if(!decode_mf(path,&mono,&frames,&sr))return 0;}return commit_sample(idx,path,mono,frames,sr);}

static int apply_disk_config(PadDiskV2*disk){for(int i=0;i<PAD_COUNT;i++){PadDiskV2*d=&disk[i];g_pads[i].vk=d->vk;g_pads[i].mods=d->mods;g_pads[i].volume=fclamp(d->volume,0,2);g_pads[i].pitch=d->pitch<-24?-24:(d->pitch>24?24:d->pitch);g_pads[i].selStart=fclamp(d->selStart,0,1);g_pads[i].selEnd=fclamp(d->selEnd,0,1);if(g_pads[i].selEnd<=g_pads[i].selStart){g_pads[i].selStart=0;g_pads[i].selEnd=1;}if(d->path[0]){float a=g_pads[i].selStart,b=g_pads[i].selEnd;if(load_audio_into_pad(i,d->path)){g_pads[i].selStart=a;g_pads[i].selEnd=b;}}}return 1;}
static int load_new_config_file(const WL_WCHAR*path){WL_HANDLE h=g_api.CreateFileW(path,GENERIC_READ,1,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);if(h==WL_INVALID_HANDLE_VALUE)return 0;WL_SIZE_T diskBytes=(WL_SIZE_T)sizeof(PadDiskV2)*PAD_COUNT;PadDiskV2*disk=(PadDiskV2*)g_api.HeapAlloc(g_api.GetProcessHeap(),0,diskBytes);if(!disk){g_api.CloseHandle(h);return 0;}ConfigHeader ch;WL_DWORD n=0;int ok=0;if(g_api.ReadFile(h,&ch,sizeof(ch),&n,0)&&n==sizeof(ch)&&ch.magic==CFG_MAGIC&&(ch.version==2||ch.version==3)){if(g_api.ReadFile(h,disk,(WL_DWORD)diskBytes,&n,0)&&n==(WL_DWORD)diskBytes){ok=1;if(ch.version==3){WL_U32 stored=0;if(!g_api.ReadFile(h,&stored,sizeof(stored),&n,0)||n!=sizeof(stored)||stored!=crc32_bytes((const WL_U8*)disk,diskBytes))ok=0;}}}g_api.CloseHandle(h);if(ok)apply_disk_config(disk);g_api.HeapFree(g_api.GetProcessHeap(),0,disk);return ok;}
static void load_config(void){for(int i=0;i<PAD_COUNT;i++){g_pads[i].volume=1.0f;g_pads[i].pitch=0;g_pads[i].selStart=0.0f;g_pads[i].selEnd=1.0f;}WL_WCHAR path[600],bak[610];if(config_path(path,600,0)){wl_wcpy(bak,path,610);wl_wcat(bak,CFG_BAK_EXT,610);if(load_new_config_file(path)){if(!cfg_file_exists(bak)&&pCopyFileW)pCopyFileW(path,bak,WL_TRUE);g_backupState=cfg_file_exists(bak)?1:0;return;}if(load_new_config_file(bak)){g_loadedFromBackup=1;g_backupState=2;return;}}if(!config_path(path,600,1))return;WL_HANDLE h=g_api.CreateFileW(path,GENERIC_READ,1,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);if(h==WL_INVALID_HANDLE_VALUE)return;ConfigHeader ch;WL_DWORD n=0;if(!g_api.ReadFile(h,&ch,sizeof(ch),&n,0)||n!=sizeof(ch)||ch.magic!=CFG_MAGIC||ch.version!=1){g_api.CloseHandle(h);return;}for(int i=0;i<PAD_COUNT;i++){PadDiskV1 d;memset(&d,0,sizeof(d));if(!g_api.ReadFile(h,&d,sizeof(d),&n,0)||n!=sizeof(d))break;g_pads[i].vk=d.vk;g_pads[i].mods=d.mods;g_pads[i].volume=fclamp(d.volume,0,2);if(d.path[0])load_audio_into_pad(i,d.path);}g_api.CloseHandle(h);save_config();}


/* Hotkey/UI text. */
static void key_name(WL_U32 vk,WL_WCHAR*out,WL_SIZE_T cap){out[0]=0;if((vk>='0'&&vk<='9')||(vk>='A'&&vk<='Z')){out[0]=(WL_WCHAR)vk;out[1]=0;return;}if(vk>=0x70&&vk<=0x87){wl_wcat(out,(const WL_WCHAR[]){'F',0},cap);wadd_num(out,cap,(int)(vk-0x6f));return;}if(vk>=0x60&&vk<=0x69){wl_wcat(out,(const WL_WCHAR[]){'N','u','m',' ',0},cap);wadd_num(out,cap,(int)(vk-0x60));return;}WL_UINT sc=g_api.MapVirtualKeyW(vk,0);WL_I32 lp=(WL_I32)(sc<<16);if(!g_api.GetKeyNameTextW(lp,out,(WL_I32)cap))wadd_num(out,cap,(int)vk);}
static void hotkey_text(int i,WL_WCHAR*out,WL_SIZE_T cap){out[0]=0;WL_U32 m=g_pads[i].mods,v=g_pads[i].vk;if(!v){wadd_ascii(out,cap,"None");return;}if(m&MOD_CONTROL)wadd_ascii(out,cap,"Ctrl+");if(m&MOD_ALT)wadd_ascii(out,cap,"Alt+");if(m&MOD_SHIFT)wadd_ascii(out,cap,"Shift+");if(m&MOD_WIN)wadd_ascii(out,cap,"Win+");WL_WCHAR k[64];key_name(v,k,64);wl_wcat(out,k,cap);if(g_hotkeyState[i]<0)wadd_ascii(out,cap," [CONFLICT]");}
static void update_pad_button(int i){if(!g_padButtons[i])return;WL_WCHAR t[512];t[0]=0;wadd_ascii(t,512,"Pad ");wadd_num(t,512,i+1);wl_wcat(t,(const WL_WCHAR[]){'\r','\n',0},512);if(g_pads[i].path[0]){const WL_WCHAR*b=base_name(g_pads[i].path);WL_SIZE_T len=wl_wlen(b);if(len>26)b+=len-26;wl_wcat(t,b,512);}else wadd_ascii(t,512,"(empty - click to load)");wl_wcat(t,(const WL_WCHAR[]){'\r','\n',0},512);WL_WCHAR hk[100];hotkey_text(i,hk,100);wl_wcat(t,hk,512);g_api.SetWindowTextW(g_padButtons[i],t);}
static void update_selected_text(void){if(!g_selectedText)return;WL_WCHAR t[420];t[0]=0;spin_lock();Pad*p=&g_pads[g_selected];float vol=p->volume;WL_I32 pitch=p->pitch;float a=p->selStart,b=p->selEnd;spin_unlock();wadd_ascii(t,420,"Selected pad: ");wadd_num(t,420,g_selected+1);wadd_ascii(t,420,"    Volume: ");wadd_num(t,420,(int)(vol*100.0f+0.5f));wadd_ascii(t,420,"%    Pitch: ");if(pitch>=0)wadd_ascii(t,420,"+");wadd_num(t,420,pitch);wadd_ascii(t,420," st    Range: ");wadd_num(t,420,(int)(a*100.0f+0.5f));wadd_ascii(t,420,"-");wadd_num(t,420,(int)(b*100.0f+0.5f));wadd_ascii(t,420,"%    Hotkey: ");WL_WCHAR hk[100];hotkey_text(g_selected,hk,100);wl_wcat(t,hk,420);g_api.SetWindowTextW(g_selectedText,t);}
static void set_status_ascii(const char*s){if(!g_statusText)return;WL_WCHAR t[300];wset_ascii(t,300,s);g_api.SetWindowTextW(g_statusText,t);}
static void update_conn_text(void){if(!g_connText)return;WL_WCHAR t[128];t[0]=0;wadd_ascii(t,128,"VST audio connections: ");wadd_num(t,128,__atomic_load_n(&g_connections,__ATOMIC_RELAXED));g_api.SetWindowTextW(g_connText,t);}
static int find_internal_hotkey_conflict(int pad,WL_U32 vk,WL_U32 mods){if(!vk)return -1;for(int i=0;i<PAD_COUNT;i++)if(i!=pad&&g_pads[i].vk==vk&&g_pads[i].mods==mods)return i;return -1;}
static int register_pad_hotkey(int i){g_api.UnregisterHotKey(g_main,HOTKEY_BASE+i);g_hotkeyState[i]=0;if(!g_pads[i].vk)return 1;if(find_internal_hotkey_conflict(i,g_pads[i].vk,g_pads[i].mods)>=0){g_hotkeyState[i]=-1;return 0;}if(g_api.RegisterHotKey(g_main,HOTKEY_BASE+i,g_pads[i].mods|MOD_NOREPEAT,g_pads[i].vk)){g_hotkeyState[i]=1;return 1;}g_hotkeyState[i]=-1;return 0;}
static void trigger_pad(int i){if(i<0||i>=PAD_COUNT||!g_pads[i].mono)return;__atomic_add_fetch(&g_triggerSerial[i],1,__ATOMIC_SEQ_CST);}

static WL_U64 diag_ft64(DIAG_FILETIME f){return ((WL_U64)f.hi<<32)|f.lo;}
static int diag_thread_count(void){if(!pCreateToolhelp32Snapshot||!pThread32First||!pThread32Next||!pGetCurrentProcessId)return -1;WL_HANDLE s=pCreateToolhelp32Snapshot(TH32CS_SNAPTHREAD,0);if(s==WL_INVALID_HANDLE_VALUE)return -1;DIAG_THREADENTRY32 te;memset(&te,0,sizeof(te));te.dwSize=sizeof(te);WL_DWORD pid=pGetCurrentProcessId();int c=0;if(pThread32First(s,&te)){do{if(te.th32OwnerProcessID==pid)c++;te.dwSize=sizeof(te);}while(pThread32Next(s,&te));}g_api.CloseHandle(s);return c;}
static int diag_hotkey_conflicts(void){int c=0;for(int i=0;i<PAD_COUNT;i++)if(g_hotkeyState[i]<0)c++;return c;}
static void update_diagnostics(void){if(!g_diagText)return;WL_WCHAR t[620];t[0]=0;WL_HANDLE proc=pGetCurrentProcess?pGetCurrentProcess():0;int ram=-1,handles=-1,gdi=-1,user=-1,threads=diag_thread_count();if(proc&&pK32GetProcessMemoryInfo){DIAG_PMC pm;memset(&pm,0,sizeof(pm));pm.cb=sizeof(pm);if(pK32GetProcessMemoryInfo(proc,&pm,sizeof(pm)))ram=(int)(pm.WorkingSetSize/(1024u*1024u));}if(proc&&pGetProcessHandleCount){WL_DWORD h=0;if(pGetProcessHandleCount(proc,&h))handles=(int)h;}if(proc&&pGetGuiResources){gdi=(int)pGetGuiResources(proc,GR_GDIOBJECTS);user=(int)pGetGuiResources(proc,GR_USEROBJECTS);}if(proc&&pGetProcessTimes&&pGetTickCount64){DIAG_FILETIME a,b,k,u;if(pGetProcessTimes(proc,&a,&b,&k,&u)){WL_U64 now=pGetTickCount64(),cpu=diag_ft64(k)+diag_ft64(u);if(g_diagLastTick&&now>g_diagLastTick){WL_U64 wall=now-g_diagLastTick,diff=cpu>=g_diagLastCpu?cpu-g_diagLastCpu:0;WL_DWORD cores=pGetActiveProcessorCount?pGetActiveProcessorCount((WL_WORD)ALL_PROCESSOR_GROUPS):1;if(!cores)cores=1;WL_U64 den=wall*100ull*(WL_U64)cores;if(den)g_diagCpuPct=(WL_I32)(diff/den);if(g_diagCpuPct>999)g_diagCpuPct=999;}g_diagLastTick=now;g_diagLastCpu=cpu;}}
 wadd_ascii(t,620,"Diagnostics: RAM ");if(ram>=0)wadd_num(t,620,ram);else wadd_ascii(t,620,"?");wadd_ascii(t,620," MB | Samples ");wadd_u64(t,620,g_totalSampleBytes/(1024ull*1024ull));wadd_ascii(t,620," MB | CPU ");wadd_num(t,620,g_diagCpuPct);wadd_ascii(t,620,"% | Handles ");if(handles>=0)wadd_num(t,620,handles);else wadd_ascii(t,620,"?");wadd_ascii(t,620," | Threads ");if(threads>=0)wadd_num(t,620,threads);else wadd_ascii(t,620,"?");wadd_ascii(t,620,"\r\nGDI ");if(gdi>=0)wadd_num(t,620,gdi);else wadd_ascii(t,620,"?");wadd_ascii(t,620," | USER ");if(user>=0)wadd_num(t,620,user);else wadd_ascii(t,620,"?");wadd_ascii(t,620," | VST ");wadd_num(t,620,__atomic_load_n(&g_connections,__ATOMIC_RELAXED));wadd_ascii(t,620," | Hotkey conflicts ");wadd_num(t,620,diag_hotkey_conflicts());wadd_ascii(t,620," | Backup ");if(g_backupState==1)wadd_ascii(t,620,"Ready");else if(g_backupState==2)wadd_ascii(t,620,"Restored");else if(g_backupState==3)wadd_ascii(t,620,"ERROR");else wadd_ascii(t,620,"None");wadd_ascii(t,620," | Uptime ");WL_U64 secs=(pGetTickCount64&&g_diagStartTick)?(pGetTickCount64()-g_diagStartTick)/1000ull:0;int hh=(int)(secs/3600ull),mm=(int)((secs/60ull)%60ull),ss=(int)(secs%60ull);wadd_num(t,620,hh);wadd_ascii(t,620,":");wadd_2(t,620,mm);wadd_ascii(t,620,":");wadd_2(t,620,ss);g_api.SetWindowTextW(g_diagText,t);}

static int choose_file_for_pad(int i){static const WL_WCHAR FILTER[]={ 'A','u','d','i','o',' ','f','i','l','e','s',' ','(','W','A','V',',',' ','M','P','3',',',' ','F','L','A','C',',',' ','M','4','A',',',' ','A','A','C',',',' ','W','M','A',')',0,'*','.','w','a','v',';','*','.','m','p','3',';','*','.','f','l','a','c',';','*','.','m','4','a',';','*','.','a','a','c',';','*','.','w','m','a',0,'W','A','V',' ','f','i','l','e','s',0,'*','.','w','a','v',0,'A','l','l',' ','f','i','l','e','s',0,'*','.','*',0,0};WL_WCHAR file[MAX_PATH_W];file[0]=0;WL_OPENFILENAMEW ofn;memset(&ofn,0,sizeof(ofn));ofn.lStructSize=sizeof(ofn);ofn.hwndOwner=g_main;ofn.lpstrFilter=FILTER;ofn.lpstrFile=file;ofn.nMaxFile=MAX_PATH_W;ofn.Flags=OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_EXPLORER|OFN_NOCHANGEDIR;if(!g_api.GetOpenFileNameW(&ofn))return 0;if(!load_audio_into_pad(i,file)){g_api.MessageBoxW(g_main,(const WL_WCHAR[]){'T','h','a','t',' ','a','u','d','i','o',' ','f','i','l','e',' ','c','o','u','l','d',' ','n','o','t',' ','b','e',' ','d','e','c','o','d','e','d','.',13,10,'W','A','V',' ','i','s',' ','b','u','i','l','t','-','i','n',';',' ','M','P','3',',',' ','A','A','C','/','M','4','A',',',' ','W','M','A',' ','a','n','d',' ','F','L','A','C',' ','u','s','e',' ','W','i','n','d','o','w','s',' ','M','e','d','i','a',' ','F','o','u','n','d','a','t','i','o','n','.',0},WINDOW_TITLE,0x10);return 0;}save_config();update_pad_button(i);update_selected_text();if(g_main)g_api.InvalidateRect(g_main,0,WL_FALSE);set_status_ascii("Sample loaded. Drag across the waveform to choose the playback range.");return 1;}

/* Pipe audio server. */
typedef struct { WL_HANDLE pipe; WL_U32 sampleRate; WL_I64 seen[PAD_COUNT]; WL_I64 seenStop; struct {WL_I32 active,pad;double pos,end,step;} voices[MAX_VOICES]; } Client;
static int pipe_read_exact(WL_HANDLE h,void*buf,WL_U32 bytes){WL_U8*d=(WL_U8*)buf;WL_U32 done=0;while(done<bytes&&g_running){WL_DWORD n=0;if(!g_api.ReadFile(h,d+done,bytes-done,&n,0)||!n)return 0;done+=n;}return done==bytes;}
static int pipe_write_exact(WL_HANDLE h,const void*buf,WL_U32 bytes){const WL_U8*s=(const WL_U8*)buf;WL_U32 done=0;while(done<bytes&&g_running){WL_DWORD n=0;if(!g_api.WriteFile(h,s+done,bytes-done,&n,0)||!n)return 0;done+=n;}return done==bytes;}
static void add_voice(Client*c,int pad){double start=0,end=0,step=1;spin_lock();Pad*p=&g_pads[pad];if(p->mono&&p->frames&&p->sampleRate){float a=fclamp(p->selStart,0,1),b=fclamp(p->selEnd,0,1);if(b<=a){a=0;b=1;}start=(double)a*(double)p->frames;end=(double)b*(double)p->frames;if(end>p->frames)end=(double)p->frames;step=((double)p->sampleRate/(double)(c->sampleRate?c->sampleRate:48000))*pitch_ratio(p->pitch);}spin_unlock();if(end<=start+0.5)return;for(int v=0;v<MAX_VOICES;v++)if(!c->voices[v].active){c->voices[v].active=1;c->voices[v].pad=pad;c->voices[v].pos=start;c->voices[v].end=end;c->voices[v].step=step;return;}c->voices[0].active=1;c->voices[0].pad=pad;c->voices[0].pos=start;c->voices[0].end=end;c->voices[0].step=step;}
static void sync_triggers(Client*c){WL_I64 stop=__atomic_load_n(&g_stopSerial,__ATOMIC_ACQUIRE);if(stop!=c->seenStop){for(int v=0;v<MAX_VOICES;v++)c->voices[v].active=0;c->seenStop=stop;}for(int i=0;i<PAD_COUNT;i++){WL_I64 now=__atomic_load_n(&g_triggerSerial[i],__ATOMIC_ACQUIRE),delta=now-c->seen[i];if(delta>8)delta=8;while(delta-->0)add_voice(c,i);c->seen[i]=now;}}
static void generate_audio(Client*c,float*out,WL_U32 frames){sync_triggers(c);spin_lock();for(WL_U32 f=0;f<frames;f++){float sum=0;for(int v=0;v<MAX_VOICES;v++){if(!c->voices[v].active)continue;Pad*p=&g_pads[c->voices[v].pad];if(!p->mono||!p->frames){c->voices[v].active=0;continue;}double pos=c->voices[v].pos;if(pos>=c->voices[v].end||pos>=(double)p->frames){c->voices[v].active=0;continue;}WL_U64 i0=(WL_U64)pos;WL_U64 i1=i0+1<p->frames?i0+1:i0;float frac=(float)(pos-(double)i0);float q=p->mono[i0]+(p->mono[i1]-p->mono[i0])*frac;sum+=q*p->volume;pos+=c->voices[v].step;c->voices[v].pos=pos;if(pos>=c->voices[v].end)c->voices[v].active=0;}sum=fclamp(sum,-4.0f,4.0f);out[f*2]=sum;out[f*2+1]=sum;}spin_unlock();}
static WL_DWORD WL_CALLBACK client_thread(void* ctx){Client*c=(Client*)ctx;__atomic_add_fetch(&g_connections,1,__ATOMIC_SEQ_CST);if(g_main)g_api.PostMessageW(g_main,WM_CONN_UPDATE,0,0);PipeHello hi;if(!pipe_read_exact(c->pipe,&hi,sizeof(hi))||hi.magic!=M_HELLO||hi.version!=1||hi.sampleRate<8000||hi.sampleRate>384000){goto done;}c->sampleRate=hi.sampleRate;for(int i=0;i<PAD_COUNT;i++)c->seen[i]=__atomic_load_n(&g_triggerSerial[i],__ATOMIC_ACQUIRE);c->seenStop=__atomic_load_n(&g_stopSerial,__ATOMIC_ACQUIRE);float*audio=(float*)g_api.HeapAlloc(g_api.GetProcessHeap(),0,2048*2*sizeof(float));if(!audio)goto done;while(g_running){PipeRequest rq;if(!pipe_read_exact(c->pipe,&rq,sizeof(rq)))break;if(rq.magic!=M_REQ||rq.frames==0||rq.frames>2048)break;generate_audio(c,audio,rq.frames);PipeAudio ah={M_AUDIO,rq.frames,2};if(!pipe_write_exact(c->pipe,&ah,sizeof(ah))||!pipe_write_exact(c->pipe,audio,rq.frames*2*sizeof(float)))break;}g_api.HeapFree(g_api.GetProcessHeap(),0,audio);
done:g_api.DisconnectNamedPipe(c->pipe);g_api.CloseHandle(c->pipe);g_api.HeapFree(g_api.GetProcessHeap(),0,c);__atomic_sub_fetch(&g_connections,1,__ATOMIC_SEQ_CST);if(g_main)g_api.PostMessageW(g_main,WM_CONN_UPDATE,0,0);return 0;}
static WL_DWORD WL_CALLBACK server_thread(void* ctx){(void)ctx;void*sd=0;WL_SECURITY_ATTRIBUTES sa;memset(&sa,0,sizeof(sa));sa.nLength=sizeof(sa);static const WL_WCHAR SDDL[]={ 'D',':','(','A',';',';','G','A',';',';',';','W','D',')',0};if(g_api.ConvertStringSecurityDescriptorToSecurityDescriptorW(SDDL,SDDL_REVISION_1,&sd,0)){sa.lpSecurityDescriptor=sd;}while(g_running){WL_HANDLE p=g_api.CreateNamedPipeW(PIPE_NAME,PIPE_ACCESS_DUPLEX,PIPE_TYPE_MESSAGE|PIPE_READMODE_MESSAGE|PIPE_WAIT|PIPE_REJECT_REMOTE_CLIENTS,PIPE_UNLIMITED_INSTANCES,16384,4096,0,sd?&sa:0);if(p==WL_INVALID_HANDLE_VALUE){g_api.Sleep(500);continue;}WL_BOOL ok=g_api.ConnectNamedPipe(p,0);if(!ok&&g_api.GetLastError()!=ERROR_PIPE_CONNECTED){g_api.CloseHandle(p);g_api.Sleep(50);continue;}Client*c=(Client*)g_api.HeapAlloc(g_api.GetProcessHeap(),0,sizeof(Client));if(!c){g_api.CloseHandle(p);continue;}memset(c,0,sizeof(*c));c->pipe=p;WL_HANDLE th=g_api.CreateThread(0,0,client_thread,c,0,0);if(th)g_api.CloseHandle(th);else{g_api.CloseHandle(p);g_api.HeapFree(g_api.GetProcessHeap(),0,c);}}if(sd)g_api.LocalFree(sd);return 0;}


static void invalidate_wave(void){if(g_main){WL_RECT r={WAVE_X,WAVE_Y,WAVE_X+WAVE_W,WAVE_Y+WAVE_H};g_api.InvalidateRect(g_main,&r,WL_FALSE);}}
static int in_wave(int x,int y){return x>=WAVE_X&&x<WAVE_X+WAVE_W&&y>=WAVE_Y&&y<WAVE_Y+WAVE_H;}
static float wave_frac(int x){float f=(float)(x-WAVE_X)/(float)(WAVE_W-1);return fclamp(f,0,1);}
static void apply_wave_drag(float cur,int finish){float a=g_waveAnchor,b=cur;if(a>b){float t=a;a=b;b=t;}if(finish&&b-a<1.0f/(float)WAVE_W){b=a+1.0f/(float)WAVE_W;if(b>1){b=1;a=b-1.0f/(float)WAVE_W;}}spin_lock();if(g_pads[g_selected].mono){g_pads[g_selected].selStart=fclamp(a,0,1);g_pads[g_selected].selEnd=fclamp(b,0,1);}spin_unlock();update_selected_text();invalidate_wave();if(finish)save_config();}
static void reset_wave_range(void){spin_lock();if(g_pads[g_selected].mono){g_pads[g_selected].selStart=0;g_pads[g_selected].selEnd=1;}spin_unlock();save_config();update_selected_text();invalidate_wave();set_status_ascii("Playback range reset to the full sample.");}
static void draw_waveform(WL_HDC dc){WL_RECT r={WAVE_X,WAVE_Y,WAVE_X+WAVE_W,WAVE_Y+WAVE_H};g_api.FillRect(dc,&r,g_waveBg);float peaks[WAVE_PEAKS],a=0,b=1;int loaded=0;spin_lock();Pad*p=&g_pads[g_selected];loaded=p->mono&&p->frames;memcpy(peaks,p->peaks,sizeof(peaks));a=p->selStart;b=p->selEnd;spin_unlock();if(loaded){int sx=WAVE_X+(int)(a*(WAVE_W-1)),ex=WAVE_X+(int)(b*(WAVE_W-1));if(ex<sx){int t=sx;sx=ex;ex=t;}WL_RECT sr={sx,WAVE_Y+1,ex+1,WAVE_Y+WAVE_H-1};g_api.FillRect(dc,&sr,g_waveSel);WL_HGDIOBJ old=g_api.SelectObject(dc,(WL_HGDIOBJ)g_waveMidPen);int cy=WAVE_Y+WAVE_H/2;g_api.MoveToEx(dc,WAVE_X+1,cy,0);g_api.LineTo(dc,WAVE_X+WAVE_W-1,cy);g_api.SelectObject(dc,(WL_HGDIOBJ)g_wavePen);for(int x=1;x<WAVE_W-1;x++){int bi=(x*WAVE_PEAKS)/WAVE_W;if(bi>=WAVE_PEAKS)bi=WAVE_PEAKS-1;int amp=(int)(peaks[bi]*(float)(WAVE_H/2-4));g_api.MoveToEx(dc,WAVE_X+x,cy-amp,0);g_api.LineTo(dc,WAVE_X+x,cy+amp+1);}g_api.SelectObject(dc,old);}WL_RECT top={WAVE_X,WAVE_Y,WAVE_X+WAVE_W,WAVE_Y+1},bot={WAVE_X,WAVE_Y+WAVE_H-1,WAVE_X+WAVE_W,WAVE_Y+WAVE_H},le={WAVE_X,WAVE_Y,WAVE_X+1,WAVE_Y+WAVE_H},ri={WAVE_X+WAVE_W-1,WAVE_Y,WAVE_X+WAVE_W,WAVE_Y+WAVE_H};g_api.FillRect(dc,&top,g_waveBorder);g_api.FillRect(dc,&bot,g_waveBorder);g_api.FillRect(dc,&le,g_waveBorder);g_api.FillRect(dc,&ri,g_waveBorder);}


static void invalidate_controls(void){
 if(!g_main)return;
 WL_RECT a={VOL_X-4,VOL_Y-4,VOL_X+VOL_W+4,VOL_Y+VOL_H+4};
 WL_RECT b={PITCH_X-4,PITCH_Y-4,PITCH_X+PITCH_W+4,PITCH_Y+PITCH_H+4};
 g_api.InvalidateRect(g_main,&a,WL_FALSE);g_api.InvalidateRect(g_main,&b,WL_FALSE);
}
static int in_volume_control(int x,int y){return x>=VOL_X&&x<VOL_X+VOL_W&&y>=VOL_Y&&y<VOL_Y+VOL_H;}
static int in_pitch_control(int x,int y){return x>=PITCH_X&&x<PITCH_X+PITCH_W&&y>=PITCH_Y&&y<PITCH_Y+PITCH_H;}
static float control_frac(int x,int ox,int ow){return fclamp((float)(x-ox)/(float)(ow-1),0,1);}
static void set_volume_from_x(int x,int finish){float v=control_frac(x,VOL_X,VOL_W)*2.0f;spin_lock();g_pads[g_selected].volume=fclamp(v,0,2);spin_unlock();update_selected_text();invalidate_controls();if(finish)save_config();}
static void set_pitch_from_x(int x,int finish){float f=control_frac(x,PITCH_X,PITCH_W);WL_I32 p=(WL_I32)(f*48.0f+0.5f)-24;if(p<-24)p=-24;if(p>24)p=24;spin_lock();g_pads[g_selected].pitch=p;spin_unlock();update_selected_text();invalidate_controls();if(finish)save_config();}
static void reset_volume(void){spin_lock();g_pads[g_selected].volume=1.0f;spin_unlock();save_config();update_selected_text();invalidate_controls();set_status_ascii("Volume reset to 100%.");}
static void reset_pitch(void){spin_lock();g_pads[g_selected].pitch=0;spin_unlock();save_config();update_selected_text();invalidate_controls();set_status_ascii("Pitch reset to 0 semitones.");}
static void draw_control_bar(WL_HDC dc,int x,int y,int w,int h,int pos,int centerMode){
 WL_RECT r={x,y,x+w,y+h};g_api.FillRect(dc,&r,g_waveBg);
 WL_RECT top={x,y,x+w,y+1},bot={x,y+h-1,x+w,y+h},le={x,y,x+1,y+h},ri={x+w-1,y,x+w,y+h};
 g_api.FillRect(dc,&top,g_waveBorder);g_api.FillRect(dc,&bot,g_waveBorder);g_api.FillRect(dc,&le,g_waveBorder);g_api.FillRect(dc,&ri,g_waveBorder);
 if(pos<x+1)pos=x+1;if(pos>x+w-2)pos=x+w-2;
 if(centerMode){int c=x+w/2;WL_RECT mid={c,y+2,c+1,y+h-2};g_api.FillRect(dc,&mid,g_waveBorder);WL_RECT fill;if(pos<c){fill.left=pos;fill.right=c;}else{fill.left=c;fill.right=pos;}fill.top=y+3;fill.bottom=y+h-3;if(fill.right>fill.left)g_api.FillRect(dc,&fill,g_waveSel);}else{WL_RECT fill={x+1,y+3,pos,y+h-3};if(fill.right>fill.left)g_api.FillRect(dc,&fill,g_waveSel);}
 WL_RECT knob={pos-2,y+1,pos+3,y+h-1};g_api.FillRect(dc,&knob,g_waveBorder);
}
static void draw_controls(WL_HDC dc){float vol;WL_I32 pitch;spin_lock();vol=g_pads[g_selected].volume;pitch=g_pads[g_selected].pitch;spin_unlock();int vp=VOL_X+(int)(fclamp(vol,0,2)*0.5f*(VOL_W-1));int pp=PITCH_X+(int)(((float)(pitch+24)/48.0f)*(PITCH_W-1));draw_control_bar(dc,VOL_X,VOL_Y,VOL_W,VOL_H,vp,0);draw_control_bar(dc,PITCH_X,PITCH_Y,PITCH_W,PITCH_H,pp,1);}

static void create_child(WL_HWND* out,const WL_WCHAR* cls,const WL_WCHAR* text,WL_DWORD style,int x,int y,int w,int h,int id){*out=g_api.CreateWindowExW(0,cls,text,WS_CHILD|WS_VISIBLE|style,x,y,w,h,g_main,(void*)(WL_UPTR)id,(WL_HINSTANCE)g_api.GetModuleHandleW(0),0);if(*out&&g_font)g_api.SendMessageW(*out,WM_SETFONT,(WL_WPARAM)g_font,1);}
static WL_LRESULT WL_CALLBACK wndproc(WL_HWND hwnd,WL_UINT msg,WL_WPARAM wp,WL_LPARAM lp){
 if(msg==WM_CREATE){g_main=hwnd;g_font=(WL_HFONT)g_api.GetStockObject(DEFAULT_GUI_FONT);g_waveBg=g_api.CreateSolidBrush(RGB_(255,255,255));g_waveSel=g_api.CreateSolidBrush(RGB_(205,225,248));g_waveBorder=g_api.CreateSolidBrush(RGB_(105,105,105));g_wavePen=g_api.CreatePen(0,1,RGB_(25,25,25));g_waveMidPen=g_api.CreatePen(0,1,RGB_(180,180,180));for(int i=0;i<PAD_COUNT;i++){int col=i%4,row=i/4;create_child(&g_padButtons[i],BTN_CLASS,(const WL_WCHAR[]){0},BS_PUSHBUTTON|BS_MULTILINE|BS_NOTIFY,15+col*180,15+row*90,170,80,ID_PAD_BASE+i);update_pad_button(i);}create_child(&g_selectedText,STATIC_CLASS,(const WL_WCHAR[]){0},SS_LEFT,15,385,710,32,500);WL_HWND label,b;create_child(&label,STATIC_CLASS,(const WL_WCHAR[]){'W','a','v','e','f','o','r','m',':',' ','d','r','a','g',' ','t','o',' ','s','e','l','e','c','t',' ','w','h','a','t',' ','p','l','a','y','s','.',' ','D','o','u','b','l','e','-','c','l','i','c','k',' ','f','o','r',' ','f','u','l','l',' ','s','a','m','p','l','e','.',0},SS_LEFT,15,418,710,18,503);create_child(&b,BTN_CLASS,(const WL_WCHAR[]){'L','o','a','d',' ','/',' ','R','e','p','l','a','c','e',' ','A','u','d','i','o',0},BS_PUSHBUTTON,15,542,165,34,ID_LOAD);create_child(&b,BTN_CLASS,(const WL_WCHAR[]){'S','e','t',' ','H','o','t','k','e','y',0},BS_PUSHBUTTON,190,542,110,34,ID_HOTKEY);create_child(&b,BTN_CLASS,(const WL_WCHAR[]){'C','l','e','a','r',' ','H','o','t','k','e','y',0},BS_PUSHBUTTON,310,542,120,34,ID_CLEAR_HOTKEY);create_child(&b,BTN_CLASS,(const WL_WCHAR[]){'F','u','l','l',' ','R','a','n','g','e',0},BS_PUSHBUTTON,440,542,105,34,ID_FULL_RANGE);create_child(&b,BTN_CLASS,(const WL_WCHAR[]){'S','t','o','p',' ','A','l','l',0},BS_PUSHBUTTON,555,542,95,34,ID_STOP_ALL);create_child(&label,STATIC_CLASS,(const WL_WCHAR[]){'V','o','l','u','m','e',' ','(','d','r','a','g',';',' ','d','o','u','b','l','e','-','c','l','i','c','k',' ','r','e','s','e','t',')',0},SS_LEFT,135,584,220,18,504);create_child(&label,STATIC_CLASS,(const WL_WCHAR[]){'P','i','t','c','h',' ','(','d','r','a','g',';',' ','d','o','u','b','l','e','-','c','l','i','c','k',' ','r','e','s','e','t',')',0},SS_LEFT,400,584,220,18,505);create_child(&g_connText,STATIC_CLASS,(const WL_WCHAR[]){0},SS_LEFT,15,640,300,22,501);create_child(&g_statusText,STATIC_CLASS,(const WL_WCHAR[]){0},SS_LEFT,15,669,710,44,502);create_child(&g_diagText,STATIC_CLASS,(const WL_WCHAR[]){0},SS_LEFT,15,718,710,40,506);for(int i=0;i<PAD_COUNT;i++)if(g_pads[i].vk)register_pad_hotkey(i);for(int i=0;i<PAD_COUNT;i++)update_pad_button(i);update_selected_text();update_conn_text();update_diagnostics();if(pSetTimer)pSetTimer(hwnd,DIAG_TIMER_ID,1000,0);if(diag_hotkey_conflicts()>0){g_api.MessageBoxW(g_main,(const WL_WCHAR[]){'O','n','e',' ','o','r',' ','m','o','r','e',' ','s','a','v','e','d',' ','h','o','t','k','e','y','s',' ','c','o','u','l','d',' ','n','o','t',' ','b','e',' ','r','e','g','i','s','t','e','r','e','d','.',13,10,'C','o','n','f','l','i','c','t','e','d',' ','p','a','d','s',' ','a','r','e',' ','m','a','r','k','e','d',' ','[','C','O','N','F','L','I','C','T',']','.',0},WINDOW_TITLE,0x30);set_status_ascii("Hotkey conflict detected. Change the pad marked [CONFLICT].");}else if(g_backupState==2)set_status_ascii("Config recovery: the backup was loaded because the main config was invalid.");else set_status_ascii("Ready. Single-click selects; double-click plays. Drag Volume/Pitch; double-click either bar to reset.");return 0;}
 if(msg==WM_CONN_UPDATE){update_conn_text();update_diagnostics();return 0;}
 if(msg==WM_TIMER&&wp==DIAG_TIMER_ID){update_diagnostics();return 0;}
 if(msg==WM_PAINT){WL_PAINTSTRUCT ps;WL_HDC dc=g_api.BeginPaint(hwnd,&ps);if(dc){draw_waveform(dc);draw_controls(dc);}g_api.EndPaint(hwnd,&ps);return 0;}
 if(msg==WM_LBUTTONDOWN){int x=(WL_I16)LOWORD_(lp),y=(WL_I16)HIWORD_(lp);if(in_wave(x,y)&&g_pads[g_selected].mono){g_waveDragging=1;g_waveAnchor=wave_frac(x);g_api.SetCapture(hwnd);apply_wave_drag(g_waveAnchor,0);return 0;}if(in_volume_control(x,y)){g_controlDragging=1;g_api.SetCapture(hwnd);set_volume_from_x(x,0);return 0;}if(in_pitch_control(x,y)){g_controlDragging=2;g_api.SetCapture(hwnd);set_pitch_from_x(x,0);return 0;}}
 if(msg==WM_MOUSEMOVE&&(wp&MK_LBUTTON)){int x=(WL_I16)LOWORD_(lp);if(g_waveDragging){apply_wave_drag(wave_frac(x),0);return 0;}if(g_controlDragging==1){set_volume_from_x(x,0);return 0;}if(g_controlDragging==2){set_pitch_from_x(x,0);return 0;}}
 if(msg==WM_LBUTTONUP){int x=(WL_I16)LOWORD_(lp);if(g_waveDragging){g_waveDragging=0;g_api.ReleaseCapture();apply_wave_drag(wave_frac(x),1);set_status_ascii("Playback range saved. Pad/hotkey playback now uses the highlighted section.");return 0;}if(g_controlDragging==1){g_controlDragging=0;g_api.ReleaseCapture();set_volume_from_x(x,1);set_status_ascii("Volume saved. Double-click the Volume bar to reset to 100%.");return 0;}if(g_controlDragging==2){g_controlDragging=0;g_api.ReleaseCapture();set_pitch_from_x(x,1);set_status_ascii("Pitch saved. Double-click the Pitch bar to reset to 0 semitones.");return 0;}}
 if(msg==WM_LBUTTONDBLCLK){int x=(WL_I16)LOWORD_(lp),y=(WL_I16)HIWORD_(lp);if(in_wave(x,y)){reset_wave_range();return 0;}if(in_volume_control(x,y)){reset_volume();return 0;}if(in_pitch_control(x,y)){reset_pitch();return 0;}}
 if(msg==WM_COMMAND){int code=HIWORD_(wp),id=LOWORD_(wp);if(id>=ID_PAD_BASE&&id<ID_PAD_BASE+PAD_COUNT){int i=id-ID_PAD_BASE;if(code==BN_CLICKED){g_selected=i;update_selected_text();invalidate_wave();invalidate_controls();set_status_ascii(g_pads[i].mono?"Pad selected. Double-click it to play the highlighted waveform range.":"Empty pad selected. Use Load / Replace Audio to assign a file.");return 0;}if(code==BN_DOUBLECLICKED){g_selected=i;update_selected_text();invalidate_wave();invalidate_controls();if(g_pads[i].mono){trigger_pad(i);set_status_ascii("Playing selected waveform range.");}else set_status_ascii("This pad is empty. Use Load / Replace Audio first.");return 0;}return 0;}if(code!=BN_CLICKED)return 0;if(id==ID_LOAD){choose_file_for_pad(g_selected);return 0;}if(id==ID_HOTKEY){g_capturePad=g_selected;g_api.SetFocus(g_main);set_status_ascii("Press the key combination you want for this pad. Esc cancels.");return 0;}if(id==ID_CLEAR_HOTKEY){g_api.UnregisterHotKey(g_main,HOTKEY_BASE+g_selected);g_pads[g_selected].vk=0;g_pads[g_selected].mods=0;g_hotkeyState[g_selected]=0;save_config();update_pad_button(g_selected);update_selected_text();update_diagnostics();set_status_ascii("Hotkey cleared.");return 0;}if(id==ID_FULL_RANGE){reset_wave_range();return 0;}if(id==ID_STOP_ALL){__atomic_add_fetch(&g_stopSerial,1,__ATOMIC_SEQ_CST);set_status_ascii("All soundboard playback stopped.");return 0;}}
 if((msg==WM_KEYDOWN||msg==WM_SYSKEYDOWN)&&g_capturePad>=0){WL_U32 vk=(WL_U32)wp;if(vk==0x1b){g_capturePad=-1;set_status_ascii("Hotkey capture cancelled.");return 0;}if(vk==VK_SHIFT||vk==VK_CONTROL||vk==VK_MENU||vk==VK_LWIN||vk==VK_RWIN)return 0;WL_U32 mods=0;if(g_api.GetKeyState(VK_CONTROL)<0)mods|=MOD_CONTROL;if(g_api.GetKeyState(VK_MENU)<0)mods|=MOD_ALT;if(g_api.GetKeyState(VK_SHIFT)<0)mods|=MOD_SHIFT;if(g_api.GetKeyState(VK_LWIN)<0||g_api.GetKeyState(VK_RWIN)<0)mods|=MOD_WIN;int pad=g_capturePad;g_capturePad=-1;int conflict=find_internal_hotkey_conflict(pad,vk,mods);if(conflict>=0){WL_WCHAR m[180];m[0]=0;wadd_ascii(m,180,"That hotkey is already assigned to Pad ");wadd_num(m,180,conflict+1);wadd_ascii(m,180,". Choose a different shortcut.");g_api.MessageBoxW(g_main,m,WINDOW_TITLE,0x30);set_status_ascii("Hotkey conflict: another pad already uses that shortcut.");return 0;}g_api.UnregisterHotKey(g_main,HOTKEY_BASE+pad);WL_U32 oldvk=g_pads[pad].vk,oldmods=g_pads[pad].mods;g_pads[pad].vk=vk;g_pads[pad].mods=mods;if(!register_pad_hotkey(pad)){g_pads[pad].vk=oldvk;g_pads[pad].mods=oldmods;register_pad_hotkey(pad);g_api.MessageBoxW(g_main,(const WL_WCHAR[]){'W','i','n','d','o','w','s',' ','r','e','j','e','c','t','e','d',' ','t','h','a','t',' ','h','o','t','k','e','y','.',13,10,'I','t',' ','m','a','y',' ','b','e',' ','u','s','e','d',' ','b','y',' ','a','n','o','t','h','e','r',' ','a','p','p',' ','o','r',' ','r','e','s','e','r','v','e','d',' ','b','y',' ','t','h','e',' ','s','y','s','t','e','m','.',0},WINDOW_TITLE,0x30);set_status_ascii("Hotkey rejected by Windows; the previous hotkey was restored.");}else{save_config();set_status_ascii("Global hotkey saved.");}update_pad_button(pad);update_selected_text();update_diagnostics();return 0;}
 if(msg==WM_HOTKEY){int id=(int)wp;if(id>=HOTKEY_BASE&&id<HOTKEY_BASE+PAD_COUNT){trigger_pad(id-HOTKEY_BASE);return 0;}}
 if(msg==WM_DESTROY){if(pKillTimer)pKillTimer(hwnd,DIAG_TIMER_ID);g_running=0;for(int i=0;i<PAD_COUNT;i++)g_api.UnregisterHotKey(g_main,HOTKEY_BASE+i);if(g_waveBg)g_api.DeleteObject((WL_HGDIOBJ)g_waveBg);if(g_waveSel)g_api.DeleteObject((WL_HGDIOBJ)g_waveSel);if(g_waveBorder)g_api.DeleteObject((WL_HGDIOBJ)g_waveBorder);if(g_wavePen)g_api.DeleteObject((WL_HGDIOBJ)g_wavePen);if(g_waveMidPen)g_api.DeleteObject((WL_HGDIOBJ)g_waveMidPen);g_api.PostQuitMessage(0);return 0;}return g_api.DefWindowProcW(hwnd,msg,wp,lp);}

void WL_CALLBACK entry(void){if(!wl_init_gui(&g_api))return;init_optional_apis();static const WL_WCHAR MUTEX_NAME[]={'L','o','c','a','l',92,'A','P','O','S','o','u','n','d','b','o','a','r','d','C','o','n','t','r','o','l','l','e','r','_','v','1',0};WL_HANDLE mx=g_api.CreateMutexW(0,WL_TRUE,MUTEX_NAME);if(mx&&g_api.GetLastError()==183){g_api.CloseHandle(mx);g_api.ExitProcess(0);}load_config();WL_HANDLE srv=g_api.CreateThread(0,0,server_thread,0,0,0);if(srv)g_api.CloseHandle(srv);WL_WNDCLASSEXW wc;memset(&wc,0,sizeof(wc));wc.cbSize=sizeof(wc);wc.style=CS_DBLCLKS;wc.lpfnWndProc=wndproc;wc.hInstance=(WL_HINSTANCE)g_api.GetModuleHandleW(0);wc.hbrBackground=(WL_HBRUSH)(WL_UPTR)(COLOR_WINDOW+1);wc.lpszClassName=CLASS_NAME;if(!g_api.RegisterClassExW(&wc)){if(mx)g_api.CloseHandle(mx);mf_shutdown();g_api.ExitProcess(2);}WL_HWND w=g_api.CreateWindowExW(0,CLASS_NAME,WINDOW_TITLE,WS_OVERLAPPEDWINDOW,100,70,760,810,0,0,wc.hInstance,0);if(!w){if(mx)g_api.CloseHandle(mx);mf_shutdown();g_api.ExitProcess(3);}g_api.ShowWindow(w,SW_SHOW);g_api.UpdateWindow(w);WL_MSG m;while(g_api.GetMessageW(&m,0,0,0)>0){g_api.TranslateMessage(&m);g_api.DispatchMessageW(&m);}for(int tries=0;tries<200&&__atomic_load_n(&g_connections,__ATOMIC_ACQUIRE)>0;tries++)g_api.Sleep(10);if(__atomic_load_n(&g_connections,__ATOMIC_ACQUIRE)==0){for(int i=0;i<PAD_COUNT;i++){if(g_pads[i].mono){g_api.HeapFree(g_api.GetProcessHeap(),0,g_pads[i].mono);g_pads[i].mono=0;}}g_totalSampleBytes=0;}mf_shutdown();if(mx)g_api.CloseHandle(mx);g_api.ExitProcess(0);}