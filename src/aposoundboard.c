#include "winlite.h"

/* ================================================================
   APO Soundboard v0.3 - single DLL VST2 + rundll32 controller
   ================================================================ */
#define PAD_COUNT 16
#define MAX_PATH_W 260
#define MAX_VOICES 32
#define HOTKEY_BASE 1400
#define PIPE_CHUNK 512u
#define RING_FRAMES 4096u
#define RING_MASK (RING_FRAMES-1u)
#define TARGET_FILL 1024u

#define WM_CREATE 0x0001u
#define WM_DESTROY 0x0002u
#define WM_PAINT 0x000Fu
#define WM_ERASEBKGND 0x0014u
#define WM_KEYDOWN 0x0100u
#define WM_SYSKEYDOWN 0x0104u
#define WM_HOTKEY 0x0312u
#define WM_MOUSEMOVE 0x0200u
#define WM_LBUTTONDOWN 0x0201u
#define WM_LBUTTONUP 0x0202u
#define WM_LBUTTONDBLCLK 0x0203u
#define WM_MOUSEWHEEL 0x020Au
#define WM_APP_CONN 0x8001u
#define CS_DBLCLKS 0x0008u
#define WS_OVERLAPPEDWINDOW 0x00CF0000u
#define WS_CHILD 0x40000000u
#define WS_VISIBLE 0x10000000u
#define SS_LEFT 0x00000000u
#define SW_SHOW 5
#define SW_RESTORE 9
#define SW_SHOWNORMAL 1
#define COLOR_WINDOW 5
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
#define VK_LWIN 0x5Bu
#define VK_RWIN 0x5Cu
#define VK_SPACE 0x20u
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
#define DT_LEFT 0x0000u
#define DT_CENTER 0x0001u
#define DT_RIGHT 0x0002u
#define DT_VCENTER 0x0004u
#define DT_WORDBREAK 0x0010u
#define DT_SINGLELINE 0x0020u
#define DT_NOPREFIX 0x0800u
#define DT_END_ELLIPSIS 0x8000u
#define MK_LBUTTON 0x0001u
#define FW_NORMAL 400
#define FW_SEMIBOLD 600
#define FW_BOLD 700
#define DEFAULT_CHARSET 1
#define OUT_DEFAULT_PRECIS 0
#define CLIP_DEFAULT_PRECIS 0
#define CLEARTYPE_QUALITY 5
#define DEFAULT_PITCH 0
#define INFINITE 0xffffffffu
#define RGB_(r,g,b) ((WL_DWORD)((r)|((g)<<8)|((b)<<16)))
#define GET_X(lp) ((WL_I32)(WL_I16)((WL_U16)((WL_UPTR)(lp)&0xffffu)))
#define GET_Y(lp) ((WL_I32)(WL_I16)((WL_U16)(((WL_UPTR)(lp)>>16)&0xffffu)))
#define HIWORD_S(wp) ((WL_I16)(((WL_UPTR)(wp)>>16)&0xffffu))

/* ----------------------- shared utility ----------------------- */
static WL_HMODULE g_dll_module=0;
static float fclamp(float x,float a,float b){return x<a?a:(x>b?b:x);}
static int iclamp(int x,int a,int b){return x<a?a:(x>b?b:x);}
static void acpy(char* d,const char*s,WL_U32 cap){WL_U32 i=0;if(!d||!cap)return;while(i+1<cap&&s&&s[i]){d[i]=s[i];i++;}d[i]=0;}
static WL_SIZE_T wfrom_ascii(WL_WCHAR*d,WL_SIZE_T cap,const char*s){WL_SIZE_T i=0;if(!cap)return 0;while(i+1<cap&&s&&s[i]){d[i]=(WL_U8)s[i];i++;}d[i]=0;return i;}
static void wadd_ascii(WL_WCHAR*d,WL_SIZE_T cap,const char*s){WL_SIZE_T n=wl_wlen(d),i=0;while(n+1<cap&&s&&s[i])d[n++]=(WL_U8)s[i++];d[n]=0;}
static void wadd_num(WL_WCHAR*d,WL_SIZE_T cap,int v){WL_WCHAR t[24];int n=0;if(v==0)t[n++]='0';else{if(v<0){WL_SIZE_T q=wl_wlen(d);if(q+1<cap){d[q]='-';d[q+1]=0;}v=-v;}while(v&&n<23){t[n++]=(WL_WCHAR)('0'+v%10);v/=10;}}while(n--&&wl_wlen(d)+1<cap){WL_SIZE_T q=wl_wlen(d);d[q]=t[n];d[q+1]=0;}}
static const WL_WCHAR* base_name(const WL_WCHAR*p){const WL_WCHAR*b=p;if(!p)return p;for(const WL_WCHAR*s=p;*s;s++)if(*s=='\\'||*s=='/')b=s+1;return b;}
static WL_U16 rd16(const WL_U8*p){return (WL_U16)(p[0]|((WL_U16)p[1]<<8));}
static WL_U32 rd32(const WL_U8*p){return (WL_U32)p[0]|((WL_U32)p[1]<<8)|((WL_U32)p[2]<<16)|((WL_U32)p[3]<<24);}
static WL_I32 rd24s(const WL_U8*p){WL_I32 v=(WL_I32)((WL_U32)p[0]|((WL_U32)p[1]<<8)|((WL_U32)p[2]<<16));if(v&0x00800000)v|=(WL_I32)0xff000000;return v;}
static int fourcc(const WL_U8*p,const char*s){return p[0]==(WL_U8)s[0]&&p[1]==(WL_U8)s[1]&&p[2]==(WL_U8)s[2]&&p[3]==(WL_U8)s[3];}

#pragma pack(push,1)
typedef struct { WL_U32 magic,version,sampleRate,channels; } PipeHello;
typedef struct { WL_U32 magic,frames; } PipeRequest;
typedef struct { WL_U32 magic,frames,channels; } PipeAudio;
#pragma pack(pop)
#define M_HELLO 0x33425341u
#define M_REQ   0x33525341u
#define M_AUDIO 0x33525344u
static const WL_WCHAR PIPE_NAME[]={ '\\','\\','.','\\','p','i','p','e','\\','A','P','O','S','o','u','n','d','b','o','a','r','d','_','v','3',0 };

/* ================================================================
   CONTROLLER / SAMPLER RUNTIME - hosted by rundll32.exe
   ================================================================ */
static WL_API g_api;
static WL_HWND g_main=0;
static volatile WL_I32 g_running=1,g_connections=0,g_padLock=0;
static volatile WL_I64 g_triggerSerial[PAD_COUNT],g_stopSerial=0;
static WL_I32 g_selected=0,g_capturePad=-1,g_dragMode=0,g_dragStartY=0;
static float g_dragStartVolume=1.0f;
static int g_dragStartPitch=0;
static WL_HANDLE g_serverThread=0;
static volatile WL_HANDLE g_serverPipe=0;
static WL_HFONT g_fontTitle=0,g_fontBody=0,g_fontSmall=0,g_fontPad=0;
static WL_HBRUSH g_brBg=0,g_brPanel=0,g_brPad=0,g_brPadSel=0,g_brAccent=0,g_brKnob=0,g_brDanger=0;
static WL_HPEN g_penOutline=0,g_penAccent=0,g_penKnob=0;

static const WL_WCHAR CTRL_CLASS[]={'A','P','O','S','o','u','n','d','b','o','a','r','d','V','3',0};
static const WL_WCHAR CTRL_TITLE[]={'A','P','O',' ','S','o','u','n','d','b','o','a','r','d',' ','v','0','.','3',0};
static const WL_WCHAR ENV_LOCALAPPDATA[]={'L','O','C','A','L','A','P','P','D','A','T','A',0};
static const WL_WCHAR APP_DIR_SUFFIX[]={'\\','A','P','O','S','o','u','n','d','b','o','a','r','d',0};
static const WL_WCHAR CFG_SUFFIX[]={'\\','c','o','n','f','i','g','.','b','i','n',0};
static const WL_WCHAR FILTER_WAV[]={ 'W','A','V',' ','f','i','l','e','s',0,'*','.','w','a','v',0,'A','l','l',' ','f','i','l','e','s',0,'*','.','*',0,0 };

typedef struct { WL_WCHAR path[MAX_PATH_W]; float* mono; WL_U64 frames; WL_U32 sampleRate; float volume; WL_I32 pitch; WL_U32 vk,mods; } Pad;
static Pad g_pads[PAD_COUNT];
typedef struct { WL_U32 magic,version; } ConfigHeader;
typedef struct { WL_WCHAR path[MAX_PATH_W]; WL_U32 vk,mods; float volume; } PadDiskV1;
typedef struct { WL_WCHAR path[MAX_PATH_W]; WL_U32 vk,mods; float volume; WL_I32 pitch; } PadDiskV2;
#define CFG_MAGIC 0x31424653u

static const float PITCH_RATIO[49]={
0.250000000f,0.264865774f,0.280615512f,0.297301779f,0.314980262f,0.333709964f,0.353553391f,0.374576769f,0.396850263f,0.420448208f,0.445449359f,0.471937156f,0.500000000f,0.529731547f,0.561231024f,0.594603558f,0.629960525f,0.667419927f,0.707106781f,0.749153538f,0.793700526f,0.840896415f,0.890898718f,0.943874313f,1.000000000f,1.059463094f,1.122462048f,1.189207115f,1.259921050f,1.334839854f,1.414213562f,1.498307077f,1.587401052f,1.681792831f,1.781797436f,1.887748625f,2.000000000f,2.118926189f,2.244924097f,2.378414230f,2.519842100f,2.669679708f,2.828427125f,2.996614154f,3.174802104f,3.363585661f,3.563594873f,3.775497251f,4.000000000f};
static const WL_I8 KNOB_X[17]={-13,-17,-19,-19,-18,-15,-11,-6,0,6,11,15,18,19,19,17,13};
static const WL_I8 KNOB_Y[17]={13,9,4,-2,-7,-12,-16,-18,-19,-18,-16,-12,-7,-2,4,9,13};

static void spin_lock(void){while(1){WL_I32 e=0;if(__atomic_compare_exchange_n(&g_padLock,&e,1,0,__ATOMIC_ACQ_REL,__ATOMIC_ACQUIRE))return;g_api.Sleep(0);}}
static void spin_unlock(void){__atomic_store_n(&g_padLock,0,__ATOMIC_RELEASE);}
static int config_path(WL_WCHAR*out,WL_SIZE_T cap){WL_WCHAR dir[512];WL_DWORD n=g_api.GetEnvironmentVariableW(ENV_LOCALAPPDATA,dir,500);if(!n||n>=500)return 0;wl_wcat(dir,APP_DIR_SUFFIX,512);g_api.CreateDirectoryW(dir,0);wl_wcpy(out,dir,cap);wl_wcat(out,CFG_SUFFIX,cap);return 1;}
static void save_config(void){WL_WCHAR path[600];if(!config_path(path,600))return;WL_HANDLE h=g_api.CreateFileW(path,GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);if(h==WL_INVALID_HANDLE_VALUE)return;ConfigHeader ch={CFG_MAGIC,2};WL_DWORD wr=0;g_api.WriteFile(h,&ch,sizeof(ch),&wr,0);for(int i=0;i<PAD_COUNT;i++){PadDiskV2 d;memset(&d,0,sizeof(d));spin_lock();wl_wcpy(d.path,g_pads[i].path,MAX_PATH_W);d.vk=g_pads[i].vk;d.mods=g_pads[i].mods;d.volume=g_pads[i].volume;d.pitch=g_pads[i].pitch;spin_unlock();g_api.WriteFile(h,&d,sizeof(d),&wr,0);}g_api.CloseHandle(h);}
static int load_wav_into_pad(int idx,const WL_WCHAR*path){WL_HANDLE h=g_api.CreateFileW(path,GENERIC_READ,1,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);if(h==WL_INVALID_HANDLE_VALUE)return 0;WL_I64 size=0;if(!g_api.GetFileSizeEx(h,&size)||size<44||size>(WL_I64)(384*1024*1024)){g_api.CloseHandle(h);return 0;}WL_U8*bytes=(WL_U8*)g_api.HeapAlloc(g_api.GetProcessHeap(),0,(WL_SIZE_T)size);if(!bytes){g_api.CloseHandle(h);return 0;}WL_DWORD total=0;while(total<(WL_U32)size){WL_DWORD n=0;if(!g_api.ReadFile(h,bytes+total,(WL_DWORD)size-total,&n,0)||!n)break;total+=n;}g_api.CloseHandle(h);if(total!=(WL_U32)size||!fourcc(bytes,"RIFF")||!fourcc(bytes+8,"WAVE")){g_api.HeapFree(g_api.GetProcessHeap(),0,bytes);return 0;}WL_U16 tag=0,ch=0,bits=0,block=0;WL_U32 sr=0;WL_U8*data=0;WL_U32 dataBytes=0,pos=12;while(pos+8<=(WL_U32)size){WL_U8*c=bytes+pos;WL_U32 cs=rd32(c+4),body=pos+8;if(body+cs>(WL_U32)size)break;if(fourcc(c,"fmt ")&&cs>=16){tag=rd16(bytes+body);ch=rd16(bytes+body+2);sr=rd32(bytes+body+4);block=rd16(bytes+body+12);bits=rd16(bytes+body+14);if(tag==0xfffe&&cs>=40)tag=rd16(bytes+body+24);}else if(fourcc(c,"data")){data=bytes+body;dataBytes=cs;}pos=body+cs+(cs&1u);}WL_U32 bps=(bits+7)/8;if(!data||!ch||!sr||!block||!bits||!(tag==1||tag==3)||!bps||block<(WL_U32)ch*bps||((tag==1)&&!(bits==8||bits==16||bits==24||bits==32))||((tag==3)&&bits!=32)){g_api.HeapFree(g_api.GetProcessHeap(),0,bytes);return 0;}WL_U64 frames=dataBytes/block;if(!frames||frames>150000000ull){g_api.HeapFree(g_api.GetProcessHeap(),0,bytes);return 0;}float*mono=(float*)g_api.HeapAlloc(g_api.GetProcessHeap(),0,(WL_SIZE_T)frames*sizeof(float));if(!mono){g_api.HeapFree(g_api.GetProcessHeap(),0,bytes);return 0;}for(WL_U64 f=0;f<frames;f++){float sum=0;int used=0;for(WL_U16 c=0;c<ch&&c<2;c++){const WL_U8*s=data+f*block+c*bps;float v=0;if(tag==3&&bits==32){union{WL_U32 u;float f;}u;u.u=rd32(s);v=u.f;}else if(bits==8)v=((int)s[0]-128)/128.0f;else if(bits==16)v=(float)(WL_I16)rd16(s)/32768.0f;else if(bits==24)v=(float)rd24s(s)/8388608.0f;else if(bits==32)v=(float)(WL_I32)rd32(s)/2147483648.0f;sum+=v;used++;}mono[f]=used?sum/(float)used:0;}g_api.HeapFree(g_api.GetProcessHeap(),0,bytes);spin_lock();float*old=g_pads[idx].mono;g_pads[idx].mono=mono;g_pads[idx].frames=frames;g_pads[idx].sampleRate=sr;wl_wcpy(g_pads[idx].path,path,MAX_PATH_W);spin_unlock();if(old)g_api.HeapFree(g_api.GetProcessHeap(),0,old);return 1;}
static void load_config(void){for(int i=0;i<PAD_COUNT;i++){g_pads[i].volume=1.0f;g_pads[i].pitch=0;}WL_WCHAR path[600];if(!config_path(path,600))return;WL_HANDLE h=g_api.CreateFileW(path,GENERIC_READ,1,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);if(h==WL_INVALID_HANDLE_VALUE)return;ConfigHeader ch;WL_DWORD n=0;if(!g_api.ReadFile(h,&ch,sizeof(ch),&n,0)||n!=sizeof(ch)||ch.magic!=CFG_MAGIC||(ch.version!=1&&ch.version!=2)){g_api.CloseHandle(h);return;}for(int i=0;i<PAD_COUNT;i++){WL_WCHAR pth[MAX_PATH_W];pth[0]=0;WL_U32 vk=0,mods=0;float vol=1;int pitch=0;if(ch.version==1){PadDiskV1 d;memset(&d,0,sizeof(d));if(g_api.ReadFile(h,&d,sizeof(d),&n,0)&&n==sizeof(d)){wl_wcpy(pth,d.path,MAX_PATH_W);vk=d.vk;mods=d.mods;vol=d.volume;}}else{PadDiskV2 d;memset(&d,0,sizeof(d));if(g_api.ReadFile(h,&d,sizeof(d),&n,0)&&n==sizeof(d)){wl_wcpy(pth,d.path,MAX_PATH_W);vk=d.vk;mods=d.mods;vol=d.volume;pitch=d.pitch;}}g_pads[i].vk=vk;g_pads[i].mods=mods;g_pads[i].volume=fclamp(vol,0,2);g_pads[i].pitch=iclamp(pitch,-24,24);if(pth[0])load_wav_into_pad(i,pth);}g_api.CloseHandle(h);}

static void key_name(WL_U32 vk,WL_WCHAR*out,WL_SIZE_T cap){out[0]=0;if((vk>='0'&&vk<='9')||(vk>='A'&&vk<='Z')){out[0]=(WL_WCHAR)vk;out[1]=0;return;}if(vk>=0x70&&vk<=0x87){wl_wcat(out,(const WL_WCHAR[]){'F',0},cap);wadd_num(out,cap,(int)(vk-0x6f));return;}WL_U32 scan=g_api.MapVirtualKeyW(vk,0);WL_I32 lp=(WL_I32)(scan<<16);if(vk==0x25||vk==0x26||vk==0x27||vk==0x28||vk==0x21||vk==0x22||vk==0x23||vk==0x24||vk==0x2d||vk==0x2e)lp|=1<<24;if(!g_api.GetKeyNameTextW(lp,out,(WL_I32)cap))wadd_num(out,cap,(int)vk);}
static void hotkey_text(int i,WL_WCHAR*out,WL_SIZE_T cap){out[0]=0;Pad*p=&g_pads[i];if(!p->vk){wadd_ascii(out,cap,"No hotkey");return;}if(p->mods&MOD_CONTROL)wadd_ascii(out,cap,"Ctrl+");if(p->mods&MOD_ALT)wadd_ascii(out,cap,"Alt+");if(p->mods&MOD_SHIFT)wadd_ascii(out,cap,"Shift+");if(p->mods&MOD_WIN)wadd_ascii(out,cap,"Win+");WL_WCHAR k[64];key_name(p->vk,k,64);wl_wcat(out,k,cap);}
static int register_pad_hotkey(int i){if(!g_pads[i].vk)return 1;return g_api.RegisterHotKey(g_main,HOTKEY_BASE+i,g_pads[i].mods|MOD_NOREPEAT,g_pads[i].vk)!=0;}
static void trigger_pad(int i){if(i>=0&&i<PAD_COUNT&&g_pads[i].mono)__atomic_add_fetch(&g_triggerSerial[i],1,__ATOMIC_SEQ_CST);}
static void msgbox_ascii(const char*text,WL_UINT flags){WL_WCHAR b[512];wfrom_ascii(b,512,text);g_api.MessageBoxW(g_main,b,CTRL_TITLE,flags);}
static int choose_file_for_pad(int i){WL_WCHAR file[520];memset(file,0,sizeof(file));WL_OPENFILENAMEW o;memset(&o,0,sizeof(o));o.lStructSize=sizeof(o);o.hwndOwner=g_main;o.lpstrFilter=FILTER_WAV;o.lpstrFile=file;o.nMaxFile=520;o.Flags=OFN_EXPLORER|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_NOCHANGEDIR;if(!g_api.GetOpenFileNameW(&o))return 0;if(!load_wav_into_pad(i,file)){msgbox_ascii("That WAV file is not supported. Use PCM WAV (8/16/24/32-bit) or 32-bit float WAV.",0x10);return 0;}save_config();g_api.InvalidateRect(g_main,0,0);return 1;}

/* ---------- audio pipe: one persistent server thread, no thread churn ---------- */
typedef struct { WL_U32 sampleRate; WL_I64 seen[PAD_COUNT]; WL_I64 seenStop; struct{WL_I32 active,pad;double pos;} voices[MAX_VOICES]; } ClientState;
static int pipe_read_exact(WL_HANDLE h,void*buf,WL_U32 bytes){WL_U8*d=(WL_U8*)buf;WL_U32 done=0;while(done<bytes&&g_running){WL_DWORD n=0;if(!g_api.ReadFile(h,d+done,bytes-done,&n,0)||!n)return 0;done+=n;}return done==bytes;}
static int pipe_write_exact(WL_HANDLE h,const void*buf,WL_U32 bytes){const WL_U8*s=(const WL_U8*)buf;WL_U32 done=0;while(done<bytes&&g_running){WL_DWORD n=0;if(!g_api.WriteFile(h,s+done,bytes-done,&n,0)||!n)return 0;done+=n;}return done==bytes;}
static void add_voice(ClientState*c,int pad){for(int v=0;v<MAX_VOICES;v++)if(!c->voices[v].active){c->voices[v].active=1;c->voices[v].pad=pad;c->voices[v].pos=0;return;}c->voices[0].active=1;c->voices[0].pad=pad;c->voices[0].pos=0;}
static void sync_triggers(ClientState*c){WL_I64 stop=__atomic_load_n(&g_stopSerial,__ATOMIC_ACQUIRE);if(stop!=c->seenStop){for(int v=0;v<MAX_VOICES;v++)c->voices[v].active=0;c->seenStop=stop;}for(int i=0;i<PAD_COUNT;i++){WL_I64 now=__atomic_load_n(&g_triggerSerial[i],__ATOMIC_ACQUIRE),d=now-c->seen[i];if(d>8)d=8;while(d-->0)add_voice(c,i);c->seen[i]=now;}}
static void generate_audio(ClientState*c,float*out,WL_U32 frames){sync_triggers(c);spin_lock();for(WL_U32 f=0;f<frames;f++){float sum=0;for(int v=0;v<MAX_VOICES;v++){if(!c->voices[v].active)continue;Pad*p=&g_pads[c->voices[v].pad];if(!p->mono||!p->frames||!p->sampleRate){c->voices[v].active=0;continue;}double pos=c->voices[v].pos;WL_U64 i0=(WL_U64)pos;if(i0>=p->frames){c->voices[v].active=0;continue;}WL_U64 i1=i0+1<p->frames?i0+1:i0;float frac=(float)(pos-(double)i0),s=p->mono[i0]+(p->mono[i1]-p->mono[i0])*frac;sum+=s*p->volume;double step=((double)p->sampleRate/(double)c->sampleRate)*(double)PITCH_RATIO[p->pitch+24];c->voices[v].pos=pos+step;if(c->voices[v].pos>=(double)p->frames)c->voices[v].active=0;}sum=fclamp(sum,-4.0f,4.0f);out[f*2]=sum;out[f*2+1]=sum;}spin_unlock();}
static WL_DWORD WL_CALLBACK server_thread(void*ctx){(void)ctx;void*sd=0;WL_SECURITY_ATTRIBUTES sa;memset(&sa,0,sizeof(sa));sa.nLength=sizeof(sa);static const WL_WCHAR SDDL[]={'D',':','(','A',';',';','G','A',';',';',';','W','D',')',0};if(g_api.ConvertStringSecurityDescriptorToSecurityDescriptorW(SDDL,SDDL_REVISION_1,&sd,0))sa.lpSecurityDescriptor=sd;float*audio=(float*)g_api.HeapAlloc(g_api.GetProcessHeap(),0,2048*2*sizeof(float));while(g_running&&audio){WL_HANDLE p=g_api.CreateNamedPipeW(PIPE_NAME,PIPE_ACCESS_DUPLEX,PIPE_TYPE_MESSAGE|PIPE_READMODE_MESSAGE|PIPE_WAIT|PIPE_REJECT_REMOTE_CLIENTS,1,16384,4096,0,sd?&sa:0);if(p==WL_INVALID_HANDLE_VALUE){g_api.Sleep(250);continue;}g_serverPipe=p;WL_BOOL ok=g_api.ConnectNamedPipe(p,0);if(!ok&&g_api.GetLastError()!=ERROR_PIPE_CONNECTED){g_api.CloseHandle(p);g_serverPipe=0;if(g_running)g_api.Sleep(50);continue;}ClientState c;memset(&c,0,sizeof(c));PipeHello hi;if(pipe_read_exact(p,&hi,sizeof(hi))&&hi.magic==M_HELLO&&hi.version==3&&hi.sampleRate>=8000&&hi.sampleRate<=384000){c.sampleRate=hi.sampleRate;for(int i=0;i<PAD_COUNT;i++)c.seen[i]=__atomic_load_n(&g_triggerSerial[i],__ATOMIC_ACQUIRE);c.seenStop=__atomic_load_n(&g_stopSerial,__ATOMIC_ACQUIRE);__atomic_store_n(&g_connections,1,__ATOMIC_RELEASE);if(g_main)g_api.PostMessageW(g_main,WM_APP_CONN,1,0);while(g_running){PipeRequest rq;if(!pipe_read_exact(p,&rq,sizeof(rq)))break;if(rq.magic!=M_REQ||rq.frames==0||rq.frames>2048)break;generate_audio(&c,audio,rq.frames);PipeAudio ah={M_AUDIO,rq.frames,2};if(!pipe_write_exact(p,&ah,sizeof(ah))||!pipe_write_exact(p,audio,rq.frames*2*sizeof(float)))break;}}__atomic_store_n(&g_connections,0,__ATOMIC_RELEASE);if(g_main)g_api.PostMessageW(g_main,WM_APP_CONN,0,0);g_api.DisconnectNamedPipe(p);g_api.CloseHandle(p);g_serverPipe=0;}if(audio)g_api.HeapFree(g_api.GetProcessHeap(),0,audio);if(sd)g_api.LocalFree(sd);return 0;}

/* ---------- modern custom UI ---------- */
static int in_rect(int x,int y,int l,int t,int r,int b){return x>=l&&x<r&&y>=t&&y<b;}
static void pad_rect(int i,WL_RECT*r){int col=i%4,row=i/4;r->left=20+col*168;r->top=82+row*126;r->right=r->left+154;r->bottom=r->top+112;}
static void draw_text_ascii(WL_HDC dc,const char*s,WL_RECT r,WL_DWORD color,WL_HFONT font,WL_UINT flags){WL_WCHAR w[512];wfrom_ascii(w,512,s);g_api.SetTextColor(dc,color);g_api.SetBkMode(dc,TRANSPARENT);if(font)g_api.SelectObject(dc,font);g_api.DrawTextW(dc,w,-1,&r,flags|DT_NOPREFIX);}
static void draw_text_w(WL_HDC dc,const WL_WCHAR*s,WL_RECT r,WL_DWORD color,WL_HFONT font,WL_UINT flags){g_api.SetTextColor(dc,color);g_api.SetBkMode(dc,TRANSPARENT);if(font)g_api.SelectObject(dc,font);g_api.DrawTextW(dc,s,-1,&r,flags|DT_NOPREFIX);}
static void fill(WL_HDC dc,int l,int t,int r,int b,WL_HBRUSH br){WL_RECT q={l,t,r,b};g_api.FillRect(dc,&q,br);}
static void draw_knob(WL_HDC dc,int cx,int cy,int norm16,const char*label,const char*value){norm16=iclamp(norm16,0,16);g_api.SelectObject(dc,g_brKnob);g_api.SelectObject(dc,g_penOutline);g_api.Ellipse(dc,cx-32,cy-32,cx+32,cy+32);g_api.SelectObject(dc,g_penAccent);g_api.MoveToEx(dc,cx,cy,0);g_api.LineTo(dc,cx+KNOB_X[norm16],cy+KNOB_Y[norm16]);WL_RECT a={cx-55,cy+39,cx+55,cy+59};draw_text_ascii(dc,label,a,RGB_(154,164,178),g_fontSmall,DT_CENTER|DT_SINGLELINE);WL_RECT b={cx-60,cy+61,cx+60,cy+83};draw_text_ascii(dc,value,b,RGB_(238,241,246),g_fontBody,DT_CENTER|DT_SINGLELINE);}
static void paint_ui(WL_HDC dc){WL_RECT cr;g_api.GetClientRect(g_main,&cr);g_api.FillRect(dc,&cr,g_brBg);fill(dc,0,0,cr.right,62,g_brPanel);WL_RECT t={20,13,500,47};draw_text_ascii(dc,"APO SOUNDBOARD",t,RGB_(241,244,249),g_fontTitle,DT_LEFT|DT_VCENTER|DT_SINGLELINE);WL_RECT st={560,18,cr.right-20,44};draw_text_ascii(dc,__atomic_load_n(&g_connections,__ATOMIC_ACQUIRE)?"MIC LINK  •  CONNECTED":"MIC LINK  •  WAITING FOR EQUALIZER APO",st,__atomic_load_n(&g_connections,__ATOMIC_ACQUIRE)?RGB_(91,214,151):RGB_(235,179,78),g_fontSmall,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
 for(int i=0;i<PAD_COUNT;i++){WL_RECT r;pad_rect(i,&r);g_api.FillRect(dc,&r,i==g_selected?g_brPadSel:g_brPad);if(i==g_selected){fill(dc,r.left,r.top,r.left+4,r.bottom,g_brAccent);}char num[16]="PAD ";char nbuf[8];int n=i+1;nbuf[0]=(n>=10)?(char)('0'+n/10):(char)('0'+n);nbuf[1]=(n>=10)?(char)('0'+n%10):0;nbuf[2]=0;int q=4,k=0;while(nbuf[k]&&q<15)num[q++]=nbuf[k++];num[q]=0;WL_RECT nr={r.left+12,r.top+8,r.right-8,r.top+30};draw_text_ascii(dc,num,nr,RGB_(119,132,151),g_fontSmall,DT_LEFT|DT_SINGLELINE);WL_WCHAR name[260];if(g_pads[i].path[0])wl_wcpy(name,base_name(g_pads[i].path),260);else wfrom_ascii(name,260,"Empty — select then Load Sample");WL_RECT rr={r.left+12,r.top+33,r.right-10,r.top+67};draw_text_w(dc,name,rr,g_pads[i].path[0]?RGB_(239,242,247):RGB_(120,130,145),g_fontPad,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);WL_WCHAR hk[100];hotkey_text(i,hk,100);WL_RECT hr={r.left+12,r.top+76,r.right-10,r.top+98};draw_text_w(dc,hk,hr,RGB_(151,163,180),g_fontSmall,DT_LEFT|DT_SINGLELINE|DT_END_ELLIPSIS);}
 int sx=708;fill(dc,sx,72,cr.right-18,cr.bottom-22,g_brPanel);WL_RECT sh={sx+22,91,cr.right-30,120};char sbuf[40]="SELECTED PAD ";int pn=g_selected+1;int z=13;if(pn>=10){sbuf[z++]=(char)('0'+pn/10);sbuf[z++]=(char)('0'+pn%10);}else sbuf[z++]=(char)('0'+pn);sbuf[z]=0;draw_text_ascii(dc,sbuf,sh,RGB_(115,198,255),g_fontSmall,DT_LEFT|DT_SINGLELINE);WL_WCHAR fn[260];if(g_pads[g_selected].path[0])wl_wcpy(fn,base_name(g_pads[g_selected].path),260);else wfrom_ascii(fn,260,"No sample loaded");WL_RECT fr={sx+22,126,cr.right-32,174};draw_text_w(dc,fn,fr,RGB_(241,244,249),g_fontPad,DT_LEFT|DT_WORDBREAK|DT_END_ELLIPSIS);
 char vv[32];int pct=(int)(g_pads[g_selected].volume*100+0.5f);vv[0]=0;WL_WCHAR dummy[2];(void)dummy;char tmp[16];int ti=0;if(pct>=100)tmp[ti++]=(char)('0'+(pct/100)%10);if(pct>=10)tmp[ti++]=(char)('0'+(pct/10)%10);tmp[ti++]=(char)('0'+pct%10);tmp[ti++]='%';tmp[ti]=0;for(int i=0;i<=ti;i++)vv[i]=tmp[i];char pv[32];int ps=g_pads[g_selected].pitch;int pi=0;if(ps>0)pv[pi++]='+';else if(ps<0){pv[pi++]='-';ps=-ps;}if(ps>=10)pv[pi++]=(char)('0'+ps/10);pv[pi++]=(char)('0'+ps%10);pv[pi++]=' ';pv[pi++]='s';pv[pi++]='t';pv[pi]=0;draw_knob(dc,sx+75,245,iclamp((int)(g_pads[g_selected].volume*8.0f+0.5f),0,16),"VOLUME",vv);draw_knob(dc,sx+202,245,iclamp((g_pads[g_selected].pitch+24)*16/48,0,16),"PITCH",pv);
 fill(dc,sx+22,344,cr.right-34,382,g_brAccent);WL_RECT lb={sx+22,344,cr.right-34,382};draw_text_ascii(dc,"LOAD / REPLACE SAMPLE",lb,RGB_(8,20,30),g_fontBody,DT_CENTER|DT_VCENTER|DT_SINGLELINE);fill(dc,sx+22,395,cr.right-34,433,g_brPad);WL_RECT hb={sx+22,395,cr.right-34,433};draw_text_ascii(dc,g_capturePad==g_selected?"PRESS A HOTKEY...":"SET GLOBAL HOTKEY",hb,RGB_(231,236,243),g_fontBody,DT_CENTER|DT_VCENTER|DT_SINGLELINE);fill(dc,sx+22,446,cr.right-34,484,g_brPad);WL_RECT cb={sx+22,446,cr.right-34,484};draw_text_ascii(dc,"CLEAR HOTKEY",cb,RGB_(198,207,220),g_fontBody,DT_CENTER|DT_VCENTER|DT_SINGLELINE);fill(dc,sx+22,513,cr.right-34,551,g_brDanger);WL_RECT xb={sx+22,513,cr.right-34,551};draw_text_ascii(dc,"STOP ALL PLAYBACK",xb,RGB_(255,232,232),g_fontBody,DT_CENTER|DT_VCENTER|DT_SINGLELINE);WL_RECT help={sx+22,575,cr.right-34,cr.bottom-35};draw_text_ascii(dc,"Single click: select pad\nDouble click: play pad\nDrag knobs or use mouse wheel\nSpace: play selected pad",help,RGB_(130,142,159),g_fontSmall,DT_LEFT|DT_WORDBREAK);}
static void select_pad(int i){g_selected=iclamp(i,0,PAD_COUNT-1);g_capturePad=-1;g_api.InvalidateRect(g_main,0,0);}
static void knob_change(int mode,int y){if(mode==1){float v=g_dragStartVolume+(float)(g_dragStartY-y)/90.0f;g_pads[g_selected].volume=fclamp(v,0,2);}else if(mode==2){int p=g_dragStartPitch+(g_dragStartY-y)/5;g_pads[g_selected].pitch=iclamp(p,-24,24);}g_api.InvalidateRect(g_main,0,0);}
static void destroy_ui_resources(void){WL_HGDIOBJ objs[]={g_fontTitle,g_fontBody,g_fontSmall,g_fontPad,g_brBg,g_brPanel,g_brPad,g_brPadSel,g_brAccent,g_brKnob,g_brDanger,g_penOutline,g_penAccent,g_penKnob};for(unsigned i=0;i<sizeof(objs)/sizeof(objs[0]);i++)if(objs[i])g_api.DeleteObject(objs[i]);g_fontTitle=g_fontBody=g_fontSmall=g_fontPad=0;g_brBg=g_brPanel=g_brPad=g_brPadSel=g_brAccent=g_brKnob=g_brDanger=0;g_penOutline=g_penAccent=g_penKnob=0;}
static WL_LRESULT WL_CALLBACK ctrl_wndproc(WL_HWND hwnd,WL_UINT msg,WL_WPARAM wp,WL_LPARAM lp){if(msg==WM_CREATE){g_main=hwnd;static const WL_WCHAR SEGOE[]={'S','e','g','o','e',' ','U','I',0};g_fontTitle=g_api.CreateFontW(-25,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH,SEGOE);g_fontBody=g_api.CreateFontW(-16,0,0,0,FW_SEMIBOLD,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH,SEGOE);g_fontSmall=g_api.CreateFontW(-13,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH,SEGOE);g_fontPad=g_api.CreateFontW(-17,0,0,0,FW_SEMIBOLD,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH,SEGOE);g_brBg=g_api.CreateSolidBrush(RGB_(15,18,24));g_brPanel=g_api.CreateSolidBrush(RGB_(24,29,38));g_brPad=g_api.CreateSolidBrush(RGB_(31,37,48));g_brPadSel=g_api.CreateSolidBrush(RGB_(38,49,64));g_brAccent=g_api.CreateSolidBrush(RGB_(87,188,255));g_brKnob=g_api.CreateSolidBrush(RGB_(42,49,62));g_brDanger=g_api.CreateSolidBrush(RGB_(103,46,52));g_penOutline=g_api.CreatePen(PS_SOLID,1,RGB_(73,83,99));g_penAccent=g_api.CreatePen(PS_SOLID,3,RGB_(87,188,255));g_penKnob=g_api.CreatePen(PS_SOLID,2,RGB_(200,210,224));for(int i=0;i<PAD_COUNT;i++)if(g_pads[i].vk)register_pad_hotkey(i);return 0;}if(msg==WM_ERASEBKGND)return 1;if(msg==WM_PAINT){WL_PAINTSTRUCT ps;WL_HDC dc=g_api.BeginPaint(hwnd,&ps);paint_ui(dc);g_api.EndPaint(hwnd,&ps);return 0;}if(msg==WM_APP_CONN){g_api.InvalidateRect(hwnd,0,0);return 0;}if(msg==WM_LBUTTONDOWN){int x=GET_X(lp),y=GET_Y(lp);for(int i=0;i<PAD_COUNT;i++){WL_RECT r;pad_rect(i,&r);if(in_rect(x,y,r.left,r.top,r.right,r.bottom)){select_pad(i);return 0;}}int sx=708;if(in_rect(x,y,sx+38,205,sx+112,290)){g_dragMode=1;g_dragStartY=y;g_dragStartVolume=g_pads[g_selected].volume;g_api.SetCapture(hwnd);return 0;}if(in_rect(x,y,sx+165,205,sx+239,290)){g_dragMode=2;g_dragStartY=y;g_dragStartPitch=g_pads[g_selected].pitch;g_api.SetCapture(hwnd);return 0;}if(in_rect(x,y,sx+22,344,980-34,382)){choose_file_for_pad(g_selected);return 0;}if(in_rect(x,y,sx+22,395,980-34,433)){g_capturePad=g_selected;g_api.SetFocus(hwnd);g_api.InvalidateRect(hwnd,0,0);return 0;}if(in_rect(x,y,sx+22,446,980-34,484)){g_api.UnregisterHotKey(hwnd,HOTKEY_BASE+g_selected);g_pads[g_selected].vk=0;g_pads[g_selected].mods=0;save_config();g_api.InvalidateRect(hwnd,0,0);return 0;}if(in_rect(x,y,sx+22,513,980-34,551)){__atomic_add_fetch(&g_stopSerial,1,__ATOMIC_SEQ_CST);return 0;}return 0;}if(msg==WM_MOUSEMOVE&&g_dragMode&&(wp&MK_LBUTTON)){knob_change(g_dragMode,GET_Y(lp));return 0;}if(msg==WM_LBUTTONUP&&g_dragMode){knob_change(g_dragMode,GET_Y(lp));g_dragMode=0;g_api.ReleaseCapture();save_config();return 0;}if(msg==WM_MOUSEWHEEL){WL_POINT pt={GET_X(lp),GET_Y(lp)};g_api.ScreenToClient(hwnd,&pt);int x=pt.x,y=pt.y,d=HIWORD_S(wp);int sx=708;if(in_rect(x,y,sx+38,205,sx+112,305)){g_pads[g_selected].volume=fclamp(g_pads[g_selected].volume+(d>0?0.05f:-0.05f),0,2);save_config();g_api.InvalidateRect(hwnd,0,0);return 0;}if(in_rect(x,y,sx+165,205,sx+239,305)){g_pads[g_selected].pitch=iclamp(g_pads[g_selected].pitch+(d>0?1:-1),-24,24);save_config();g_api.InvalidateRect(hwnd,0,0);return 0;}}if(msg==WM_LBUTTONDBLCLK){int x=GET_X(lp),y=GET_Y(lp);for(int i=0;i<PAD_COUNT;i++){WL_RECT r;pad_rect(i,&r);if(in_rect(x,y,r.left,r.top,r.right,r.bottom)){select_pad(i);if(g_pads[i].mono)trigger_pad(i);else choose_file_for_pad(i);return 0;}}}if((msg==WM_KEYDOWN||msg==WM_SYSKEYDOWN)&&g_capturePad>=0){WL_U32 vk=(WL_U32)wp;if(vk==0x1b){g_capturePad=-1;g_api.InvalidateRect(hwnd,0,0);return 0;}if(vk==VK_SHIFT||vk==VK_CONTROL||vk==VK_MENU||vk==VK_LWIN||vk==VK_RWIN)return 0;WL_U32 mods=0;if(g_api.GetKeyState(VK_CONTROL)<0)mods|=MOD_CONTROL;if(g_api.GetKeyState(VK_MENU)<0)mods|=MOD_ALT;if(g_api.GetKeyState(VK_SHIFT)<0)mods|=MOD_SHIFT;if(g_api.GetKeyState(VK_LWIN)<0||g_api.GetKeyState(VK_RWIN)<0)mods|=MOD_WIN;int pad=g_capturePad;g_capturePad=-1;g_api.UnregisterHotKey(hwnd,HOTKEY_BASE+pad);WL_U32 ov=g_pads[pad].vk,om=g_pads[pad].mods;g_pads[pad].vk=vk;g_pads[pad].mods=mods;if(!register_pad_hotkey(pad)){g_pads[pad].vk=ov;g_pads[pad].mods=om;register_pad_hotkey(pad);msgbox_ascii("That hotkey is already in use.",0x30);}else save_config();g_api.InvalidateRect(hwnd,0,0);return 0;}if(msg==WM_KEYDOWN&&g_capturePad<0&&wp==VK_SPACE){trigger_pad(g_selected);return 0;}if(msg==WM_HOTKEY){int id=(int)wp;if(id>=HOTKEY_BASE&&id<HOTKEY_BASE+PAD_COUNT){trigger_pad(id-HOTKEY_BASE);return 0;}}if(msg==WM_DESTROY){g_running=0;for(int i=0;i<PAD_COUNT;i++)g_api.UnregisterHotKey(hwnd,HOTKEY_BASE+i);WL_HANDLE pipe=(WL_HANDLE)g_serverPipe;if(pipe)g_api.CancelIoEx(pipe,0);if(g_serverThread){g_api.WaitForSingleObject(g_serverThread,1200);g_api.CloseHandle(g_serverThread);g_serverThread=0;}for(int i=0;i<PAD_COUNT;i++)if(g_pads[i].mono){g_api.HeapFree(g_api.GetProcessHeap(),0,g_pads[i].mono);g_pads[i].mono=0;}destroy_ui_resources();g_api.PostQuitMessage(0);return 0;}return g_api.DefWindowProcW(hwnd,msg,wp,lp);}

/* Export called by rundll32.exe. */
__declspec(dllexport) void WL_CALLBACK APOSoundboardController(WL_HWND hwnd,WL_HINSTANCE inst,char*cmd,int show){(void)hwnd;(void)inst;(void)cmd;(void)show;if(!wl_init_gui(&g_api))return;static const WL_WCHAR MUTEX_NAME[]={'L','o','c','a','l','\\','A','P','O','S','o','u','n','d','b','o','a','r','d','R','u','n','t','i','m','e','_','v','3',0};WL_HANDLE mx=g_api.CreateMutexW(0,WL_TRUE,MUTEX_NAME);if(mx&&g_api.GetLastError()==183){WL_HWND w=g_api.FindWindowW(CTRL_CLASS,0);if(w){g_api.ShowWindow(w,SW_RESTORE);g_api.SetForegroundWindow(w);}g_api.CloseHandle(mx);return;}load_config();g_running=1;g_serverThread=g_api.CreateThread(0,0,server_thread,0,0,0);WL_WNDCLASSEXW wc;memset(&wc,0,sizeof(wc));wc.cbSize=sizeof(wc);wc.style=CS_DBLCLKS;wc.lpfnWndProc=ctrl_wndproc;wc.hInstance=(WL_HINSTANCE)g_dll_module;wc.hbrBackground=(WL_HBRUSH)(WL_UPTR)(COLOR_WINDOW+1);wc.lpszClassName=CTRL_CLASS;if(!g_api.RegisterClassExW(&wc)){if(mx)g_api.CloseHandle(mx);return;}WL_HWND w=g_api.CreateWindowExW(0,CTRL_CLASS,CTRL_TITLE,WS_OVERLAPPEDWINDOW,100,80,1000,690,0,0,(WL_HINSTANCE)g_dll_module,0);if(!w){if(mx)g_api.CloseHandle(mx);return;}g_api.ShowWindow(w,SW_SHOW);g_api.UpdateWindow(w);WL_MSG m;while(g_api.GetMessageW(&m,0,0,0)>0){g_api.TranslateMessage(&m);g_api.DispatchMessageW(&m);}if(mx)g_api.CloseHandle(mx);}

/* ================================================================
   VST2 EFFECT - microphone passthrough + soundboard mix
   ================================================================ */
#define VST_MAGIC ((WL_I32)0x56737450)
#define FOURCC(a,b,c,d) ((WL_I32)(((WL_U32)(a)<<24)|((WL_U32)(b)<<16)|((WL_U32)(c)<<8)|(WL_U32)(d)))
#define PLUGIN_ID FOURCC('A','P','S','3')
#define EFF_HAS_EDITOR (1<<0)
#define EFF_CAN_REPLACING (1<<4)
typedef struct AEffect AEffect;
typedef WL_IPTR (WL_CALLBACK *audioMasterCallback)(AEffect*,WL_I32,WL_I32,WL_IPTR,void*,float);
typedef WL_IPTR (WL_CALLBACK *AEffectDispatcherProc)(AEffect*,WL_I32,WL_I32,WL_IPTR,void*,float);
typedef void (WL_CALLBACK *AEffectProcessProc)(AEffect*,float**,float**,WL_I32);
typedef void (WL_CALLBACK *AEffectSetParameterProc)(AEffect*,WL_I32,float);
typedef float (WL_CALLBACK *AEffectGetParameterProc)(AEffect*,WL_I32);
typedef void (WL_CALLBACK *AEffectProcessDoubleProc)(AEffect*,double**,double**,WL_I32);
struct AEffect {WL_I32 magic;AEffectDispatcherProc dispatcher;AEffectProcessProc process;AEffectSetParameterProc setParameter;AEffectGetParameterProc getParameter;WL_I32 numPrograms,numParams,numInputs,numOutputs,flags;void*resvd1;void*resvd2;WL_I32 initialDelay,realQualities,offQualities;float ioRatio;void*object;void*user;WL_I32 uniqueID,version;AEffectProcessProc processReplacing;AEffectProcessDoubleProc processDoubleReplacing;char future[56];};
enum{effOpen=0,effClose=1,effGetParamLabel=6,effGetParamDisplay=7,effGetParamName=8,effSetSampleRate=10,effSetBlockSize=11,effMainsChanged=12,effEditGetRect=13,effEditOpen=14,effEditClose=15,effEditIdle=19,effGetPlugCategory=35,effGetEffectName=45,effGetVendorString=47,effGetProductString=48,effGetVendorVersion=49,effCanDo=51,effGetVstVersion=58,effStartProcess=71,effStopProcess=72};
typedef struct{WL_I16 top,left,bottom,right;}VstRect;
typedef struct{AEffect effect;audioMasterCallback host;WL_API api;volatile WL_I32 apiReady,running;WL_HANDLE thread;volatile WL_HANDLE pipe;volatile WL_U32 sampleRate,rpos,wpos;float ring[RING_FRAMES*2];float params[3];VstRect editorRect;WL_HWND editorChild;WL_API editorApi;WL_I32 editorReady;}Plugin;
static const WL_WCHAR STATIC_CLASS[]={'S','T','A','T','I','C',0};
static const WL_WCHAR PANEL_TEXT[]={'A','P','O',' ','S','o','u','n','d','b','o','a','r','d',' ','v','0','.','3',' ','i','s',' ','r','u','n','n','i','n','g','.', '\r','\n','T','h','e',' ','f','u','l','l',' ','s','o','u','n','d','b','o','a','r','d',' ','w','i','n','d','o','w',' ','h','a','s',' ','b','e','e','n',' ','o','p','e','n','e','d','.',0};
static const WL_WCHAR OPEN_VERB[]={'o','p','e','n',0};
static const WL_WCHAR RUNDLL[]={'r','u','n','d','l','l','3','2','.','e','x','e',0};
static Plugin* P(AEffect*e){return (Plugin*)e->object;}
static void launch_controller(Plugin*p){WL_API*a=&p->editorApi;if(!p->editorReady||!g_dll_module)return;WL_WCHAR path[520];WL_DWORD n=a->GetModuleFileNameW(g_dll_module,path,520);if(!n||n>=510)return;WL_WCHAR par[620];par[0]='"';par[1]=0;wl_wcat(par,path,620);wl_wcat(par,(const WL_WCHAR[]){'"',',','A','P','O','S','o','u','n','d','b','o','a','r','d','C','o','n','t','r','o','l','l','e','r',0},620);a->ShellExecuteW(0,OPEN_VERB,RUNDLL,par,0,SW_SHOWNORMAL);}
static void utoa3(char*d,int v){if(v<0)v=0;if(v>999)v=999;if(v>=100){d[0]=(char)('0'+v/100);d[1]=(char)('0'+(v/10)%10);d[2]=(char)('0'+v%10);d[3]=0;}else if(v>=10){d[0]=(char)('0'+v/10);d[1]=(char)('0'+v%10);d[2]=0;}else{d[0]=(char)('0'+v);d[1]=0;}}
static int exact_write(Plugin*p,WL_HANDLE h,const void*buf,WL_U32 bytes){const WL_U8*s=(const WL_U8*)buf;WL_U32 done=0;while(done<bytes&&p->running){WL_DWORD n=0;if(!p->api.WriteFile(h,s+done,bytes-done,&n,0)||!n)return 0;done+=n;}return done==bytes;}
static int exact_read(Plugin*p,WL_HANDLE h,void*buf,WL_U32 bytes){WL_U8*d=(WL_U8*)buf;WL_U32 done=0;while(done<bytes&&p->running){WL_DWORD n=0;if(!p->api.ReadFile(h,d+done,bytes-done,&n,0)||!n)return 0;done+=n;}return done==bytes;}
static WL_DWORD WL_CALLBACK vst_pipe_thread(void*ctx){Plugin*p=(Plugin*)ctx;float*temp=(float*)p->api.HeapAlloc(p->api.GetProcessHeap(),0,PIPE_CHUNK*2*sizeof(float));if(!temp){p->running=0;return 0;}while(p->running){if(!p->api.WaitNamedPipeW(PIPE_NAME,250)){p->api.Sleep(100);continue;}WL_HANDLE h=p->api.CreateFileW(PIPE_NAME,GENERIC_READ|GENERIC_WRITE,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);if(h==WL_INVALID_HANDLE_VALUE){p->api.Sleep(100);continue;}p->pipe=h;PipeHello hello={M_HELLO,3,p->sampleRate?p->sampleRate:48000,2};if(!exact_write(p,h,&hello,sizeof(hello))){p->api.CloseHandle(h);p->pipe=0;continue;}while(p->running){WL_U32 r=__atomic_load_n(&p->rpos,__ATOMIC_ACQUIRE),w=__atomic_load_n(&p->wpos,__ATOMIC_ACQUIRE),avail=w-r;if(avail>=TARGET_FILL){p->api.Sleep(2);continue;}WL_U32 freef=RING_FRAMES-avail;if(freef<PIPE_CHUNK){p->api.Sleep(1);continue;}PipeRequest rq={M_REQ,PIPE_CHUNK};if(!exact_write(p,h,&rq,sizeof(rq)))break;PipeAudio ah;if(!exact_read(p,h,&ah,sizeof(ah)))break;if(ah.magic!=M_AUDIO||!ah.frames||ah.frames>PIPE_CHUNK||(ah.channels!=1&&ah.channels!=2))break;WL_U32 vals=ah.frames*ah.channels;if(!exact_read(p,h,temp,vals*sizeof(float)))break;w=__atomic_load_n(&p->wpos,__ATOMIC_RELAXED);for(WL_U32 i=0;i<ah.frames;i++){WL_U32 idx=(w+i)&RING_MASK;float l=temp[i*ah.channels],rr=ah.channels==2?temp[i*2+1]:l;p->ring[idx*2]=l;p->ring[idx*2+1]=rr;}__atomic_store_n(&p->wpos,w+ah.frames,__ATOMIC_RELEASE);}p->api.CloseHandle(h);p->pipe=0;__atomic_store_n(&p->rpos,0,__ATOMIC_RELEASE);__atomic_store_n(&p->wpos,0,__ATOMIC_RELEASE);p->api.Sleep(50);}p->api.HeapFree(p->api.GetProcessHeap(),0,temp);return 0;}
static int ensure_api(Plugin*p){if(p->apiReady)return 1;if(!wl_init_kernel(&p->api))return 0;p->apiReady=1;return 1;}
static void start_stream(Plugin*p){if(!ensure_api(p))return;WL_I32 e=0;if(!__atomic_compare_exchange_n(&p->running,&e,1,0,__ATOMIC_ACQ_REL,__ATOMIC_ACQUIRE))return;__atomic_store_n(&p->rpos,0,__ATOMIC_RELEASE);__atomic_store_n(&p->wpos,0,__ATOMIC_RELEASE);p->thread=p->api.CreateThread(0,0,vst_pipe_thread,p,0,0);if(!p->thread)p->running=0;}
static void stop_stream(Plugin*p){if(!p->apiReady)return;if(!__atomic_exchange_n(&p->running,0,__ATOMIC_ACQ_REL))return;WL_HANDLE h=(WL_HANDLE)p->pipe;if(h)p->api.CancelIoEx(h,0);if(p->thread){p->api.WaitForSingleObject(p->thread,1500);p->api.CloseHandle(p->thread);p->thread=0;}p->pipe=0;}
static void WL_CALLBACK process_replacing(AEffect*e,float**in,float**out,WL_I32 frames){Plugin*p=P(e);float mic=p->params[1]*2.0f,board=p->params[0]*2.0f;int limit=p->params[2]>=0.5f;WL_U32 r=__atomic_load_n(&p->rpos,__ATOMIC_RELAXED),w=__atomic_load_n(&p->wpos,__ATOMIC_ACQUIRE),avail=w-r;for(WL_I32 i=0;i<frames;i++){float s0=0,s1=0;if((WL_U32)i<avail){WL_U32 idx=(r+(WL_U32)i)&RING_MASK;s0=p->ring[idx*2];s1=p->ring[idx*2+1];}float a=(in&&in[0]?in[0][i]:0)*mic+s0*board;float b=(in&&in[1]?in[1][i]:0)*mic+s1*board;if(limit){a=fclamp(a,-1,1);b=fclamp(b,-1,1);}if(out&&out[0])out[0][i]=a;if(out&&out[1])out[1][i]=b;}WL_U32 used=(WL_U32)frames<avail?(WL_U32)frames:avail;__atomic_store_n(&p->rpos,r+used,__ATOMIC_RELEASE);}
static void WL_CALLBACK process_accum(AEffect*e,float**in,float**out,WL_I32 f){process_replacing(e,in,out,f);}static void WL_CALLBACK set_param(AEffect*e,WL_I32 i,float v){Plugin*p=P(e);if(i>=0&&i<3)p->params[i]=fclamp(v,0,1);}static float WL_CALLBACK get_param(AEffect*e,WL_I32 i){Plugin*p=P(e);return(i>=0&&i<3)?p->params[i]:0;}
static WL_IPTR WL_CALLBACK vst_dispatch(AEffect*e,WL_I32 op,WL_I32 index,WL_IPTR value,void*ptr,float opt){Plugin*p=P(e);switch(op){case effOpen:return 0;case effClose:if(p->editorChild&&p->editorReady)p->editorApi.DestroyWindow(p->editorChild);stop_stream(p);if(ensure_api(p))p->api.HeapFree(p->api.GetProcessHeap(),0,p);return 0;case effSetSampleRate:p->sampleRate=(WL_U32)(opt>8000?opt:48000);return 0;case effSetBlockSize:return 0;case effMainsChanged:if(value)start_stream(p);else stop_stream(p);return 0;case effEditGetRect:if(ptr){*(VstRect**)ptr=&p->editorRect;return 1;}return 0;case effEditOpen:if(!ptr)return 0;if(!p->editorReady){if(!wl_init_gui(&p->editorApi))return 0;p->editorReady=1;}if(p->editorChild)p->editorApi.DestroyWindow(p->editorChild);p->editorChild=p->editorApi.CreateWindowExW(0,STATIC_CLASS,PANEL_TEXT,WS_CHILD|WS_VISIBLE|SS_LEFT,12,12,556,52,(WL_HWND)ptr,0,(WL_HINSTANCE)g_dll_module,0);launch_controller(p);return 1;case effEditClose:if(p->editorChild&&p->editorReady){p->editorApi.DestroyWindow(p->editorChild);p->editorChild=0;}return 1;case effEditIdle:return 1;case effStartProcess:start_stream(p);return 1;case effStopProcess:stop_stream(p);return 1;case effGetParamName:if(ptr)acpy((char*)ptr,index==0?"Board":index==1?"Mic":index==2?"Limiter":"",32);return 1;case effGetParamLabel:if(ptr)acpy((char*)ptr,index<2?"%":"",16);return 1;case effGetParamDisplay:if(ptr){if(index<2)utoa3((char*)ptr,(int)(p->params[index]*200.0f+0.5f));else acpy((char*)ptr,p->params[2]>=0.5f?"On":"Off",16);}return 1;case effGetEffectName:if(ptr)acpy((char*)ptr,"APO Soundboard v0.3",64);return 1;case effGetVendorString:if(ptr)acpy((char*)ptr,"APO Soundboard",64);return 1;case effGetProductString:if(ptr)acpy((char*)ptr,"APO Soundboard",64);return 1;case effGetVendorVersion:return 30000;case effGetVstVersion:return 2400;case effGetPlugCategory:return 1;case effCanDo:return 0;default:return 0;}}
__declspec(dllexport) AEffect* WL_CALLBACK VSTPluginMain(audioMasterCallback host){WL_API api;if(!wl_init_kernel(&api))return 0;Plugin*p=(Plugin*)api.HeapAlloc(api.GetProcessHeap(),0,sizeof(Plugin));if(!p)return 0;memset(p,0,sizeof(*p));p->api=api;p->apiReady=1;p->host=host;p->sampleRate=48000;p->params[0]=0.5f;p->params[1]=0.5f;p->params[2]=1.0f;p->editorRect.top=0;p->editorRect.left=0;p->editorRect.bottom=76;p->editorRect.right=580;AEffect*e=&p->effect;e->magic=VST_MAGIC;e->dispatcher=vst_dispatch;e->process=process_accum;e->setParameter=set_param;e->getParameter=get_param;e->numPrograms=1;e->numParams=3;e->numInputs=2;e->numOutputs=2;e->flags=EFF_HAS_EDITOR|EFF_CAN_REPLACING;e->object=p;e->uniqueID=PLUGIN_ID;e->version=30000;e->processReplacing=process_replacing;return e;}
WL_BOOL WL_CALLBACK DllMain(void*h,WL_DWORD reason,void*reserved){(void)reserved;if(reason==1)g_dll_module=(WL_HMODULE)h;return WL_TRUE;}