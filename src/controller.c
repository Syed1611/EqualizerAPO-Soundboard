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

#define WM_CREATE 0x0001u
#define WM_DESTROY 0x0002u
#define WM_COMMAND 0x0111u
#define WM_KEYDOWN 0x0100u
#define WM_SYSKEYDOWN 0x0104u
#define WM_HOTKEY 0x0312u
#define WM_SETFONT 0x0030u
#define BN_CLICKED 0
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
static WL_HWND g_main=0,g_padButtons[PAD_COUNT],g_selectedText=0,g_statusText=0,g_connText=0;
static WL_HFONT g_font=0;
static volatile WL_I32 g_running=1;
static volatile WL_I32 g_connections=0;
static volatile WL_I32 g_padLock=0;
static volatile WL_I64 g_triggerSerial[PAD_COUNT];
static volatile WL_I64 g_stopSerial=0;
static WL_I32 g_selected=0;
static WL_I32 g_capturePad=-1;

static const WL_WCHAR CLASS_NAME[]={'A','P','O','S','o','u','n','d','b','o','a','r','d','C','t','r','l',0};
static const WL_WCHAR WINDOW_TITLE[]={'A','P','O',' ','S','o','u','n','d','b','o','a','r','d',' ','C','o','n','t','r','o','l','l','e','r',0};
static const WL_WCHAR BTN_CLASS[]={'B','U','T','T','O','N',0};
static const WL_WCHAR STATIC_CLASS[]={'S','T','A','T','I','C',0};

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

/* Sample storage. */
typedef struct { WL_WCHAR path[MAX_PATH_W]; float* mono; WL_U64 frames; WL_U32 sampleRate; float volume; WL_U32 vk,mods; } Pad;
static Pad g_pads[PAD_COUNT];

typedef struct { WL_U32 magic,version; } ConfigHeader;
typedef struct { WL_WCHAR path[MAX_PATH_W]; WL_U32 vk,mods; float volume; } PadDisk;
#define CFG_MAGIC 0x31424653u
static const WL_WCHAR ENV_LOCALAPPDATA[]={'L','O','C','A','L','A','P','P','D','A','T','A',0};
static const WL_WCHAR APP_DIR_SUFFIX[]={'\\','A','P','O','S','o','u','n','d','b','o','a','r','d',0};
static const WL_WCHAR CFG_SUFFIX[]={'\\','c','o','n','f','i','g','.','b','i','n',0};
static int config_path(WL_WCHAR*out,WL_SIZE_T cap){WL_WCHAR dir[512];WL_DWORD n=g_api.GetEnvironmentVariableW(ENV_LOCALAPPDATA,dir,500);if(!n||n>=500)return 0;wl_wcat(dir,APP_DIR_SUFFIX,512);g_api.CreateDirectoryW(dir,0);wl_wcpy(out,dir,cap);wl_wcat(out,CFG_SUFFIX,cap);return 1;}
static void save_config(void){WL_WCHAR path[600];if(!config_path(path,600))return;WL_HANDLE h=g_api.CreateFileW(path,GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);if(h==WL_INVALID_HANDLE_VALUE)return;ConfigHeader ch={CFG_MAGIC,1};WL_DWORD wr=0;g_api.WriteFile(h,&ch,sizeof(ch),&wr,0);for(int i=0;i<PAD_COUNT;i++){PadDisk d;memset(&d,0,sizeof(d));wl_wcpy(d.path,g_pads[i].path,MAX_PATH_W);d.vk=g_pads[i].vk;d.mods=g_pads[i].mods;d.volume=g_pads[i].volume;g_api.WriteFile(h,&d,sizeof(d),&wr,0);}g_api.CloseHandle(h);}

static int load_wav_into_pad(int idx,const WL_WCHAR* path){WL_HANDLE h=g_api.CreateFileW(path,GENERIC_READ,1,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);if(h==WL_INVALID_HANDLE_VALUE)return 0;WL_I64 size=0;if(!g_api.GetFileSizeEx(h,&size)||size<44||size>(WL_I64)(256*1024*1024)){g_api.CloseHandle(h);return 0;}WL_U8* bytes=(WL_U8*)g_api.HeapAlloc(g_api.GetProcessHeap(),0,(WL_SIZE_T)size);if(!bytes){g_api.CloseHandle(h);return 0;}WL_DWORD got=0,total=0;while(total<(WL_U32)size){WL_DWORD n=0;if(!g_api.ReadFile(h,bytes+total,(WL_DWORD)size-total,&n,0)||!n)break;total+=n;}g_api.CloseHandle(h);if(total!=(WL_U32)size||!fourcc(bytes,"RIFF")||!fourcc(bytes+8,"WAVE")){g_api.HeapFree(g_api.GetProcessHeap(),0,bytes);return 0;}
 WL_U16 tag=0,ch=0,bits=0,block=0;WL_U32 sr=0;WL_U8*data=0;WL_U32 dataBytes=0;WL_U32 pos=12;while(pos+8<=(WL_U32)size){WL_U8*c=bytes+pos;WL_U32 cs=rd32(c+4);WL_U32 body=pos+8;if(body+cs>(WL_U32)size)break;if(fourcc(c,"fmt ")&&cs>=16){tag=rd16(bytes+body);ch=rd16(bytes+body+2);sr=rd32(bytes+body+4);block=rd16(bytes+body+12);bits=rd16(bytes+body+14);if(tag==0xfffe&&cs>=40)tag=rd16(bytes+body+24);}else if(fourcc(c,"data")){data=bytes+body;dataBytes=cs;}pos=body+cs+(cs&1u);}
 WL_U32 bps=(bits+7)/8;if(!data||!ch||!sr||!block||!bits||!(tag==1||tag==3)||!bps||block<(WL_U32)ch*bps||((tag==1)&&!(bits==8||bits==16||bits==24||bits==32))||((tag==3)&&bits!=32)){g_api.HeapFree(g_api.GetProcessHeap(),0,bytes);return 0;}WL_U64 frames=dataBytes/block;if(!frames||frames>100000000ull){g_api.HeapFree(g_api.GetProcessHeap(),0,bytes);return 0;}float* mono=(float*)g_api.HeapAlloc(g_api.GetProcessHeap(),0,(WL_SIZE_T)frames*sizeof(float));if(!mono){g_api.HeapFree(g_api.GetProcessHeap(),0,bytes);return 0;}
 for(WL_U64 f=0;f<frames;f++){float sum=0;int used=0;for(WL_U16 c=0;c<ch;c++){const WL_U8*s=data+f*block+c*bps;float v=0;if(tag==3&&bits==32){union{WL_U32 u;float f;}u;u.u=rd32(s);v=u.f;}else if(tag==1&&bits==8){v=((int)s[0]-128)/128.0f;}else if(tag==1&&bits==16){WL_I16 q=(WL_I16)rd16(s);v=(float)q/32768.0f;}else if(tag==1&&bits==24){v=(float)rd24s(s)/8388608.0f;}else if(tag==1&&bits==32){WL_I32 q=(WL_I32)rd32(s);v=(float)q/2147483648.0f;}else{g_api.HeapFree(g_api.GetProcessHeap(),0,mono);g_api.HeapFree(g_api.GetProcessHeap(),0,bytes);return 0;}if(c<2){sum+=v;used++;}}mono[f]=used?sum/(float)used:0;}
 g_api.HeapFree(g_api.GetProcessHeap(),0,bytes);spin_lock();float*old=g_pads[idx].mono;g_pads[idx].mono=mono;g_pads[idx].frames=frames;g_pads[idx].sampleRate=sr;wl_wcpy(g_pads[idx].path,path,MAX_PATH_W);spin_unlock();if(old)g_api.HeapFree(g_api.GetProcessHeap(),0,old);return 1;}

static void load_config(void){for(int i=0;i<PAD_COUNT;i++)g_pads[i].volume=1.0f;WL_WCHAR path[600];if(!config_path(path,600))return;WL_HANDLE h=g_api.CreateFileW(path,GENERIC_READ,1,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);if(h==WL_INVALID_HANDLE_VALUE)return;ConfigHeader ch;WL_DWORD n=0;if(!g_api.ReadFile(h,&ch,sizeof(ch),&n,0)||n!=sizeof(ch)||ch.magic!=CFG_MAGIC||ch.version!=1){g_api.CloseHandle(h);return;}PadDisk* disks=(PadDisk*)g_api.HeapAlloc(g_api.GetProcessHeap(),0,sizeof(PadDisk)*PAD_COUNT);if(!disks){g_api.CloseHandle(h);return;}for(int i=0;i<PAD_COUNT;i++){if(!g_api.ReadFile(h,&disks[i],sizeof(PadDisk),&n,0)||n!=sizeof(PadDisk)){memset(&disks[i],0,sizeof(PadDisk));disks[i].volume=1.0f;}}g_api.CloseHandle(h);for(int i=0;i<PAD_COUNT;i++){g_pads[i].vk=disks[i].vk;g_pads[i].mods=disks[i].mods;g_pads[i].volume=fclamp(disks[i].volume,0,2);if(disks[i].path[0])load_wav_into_pad(i,disks[i].path);}g_api.HeapFree(g_api.GetProcessHeap(),0,disks);}

/* Hotkey/UI text. */
static void key_name(WL_U32 vk,WL_WCHAR*out,WL_SIZE_T cap){out[0]=0;if((vk>='0'&&vk<='9')||(vk>='A'&&vk<='Z')){out[0]=(WL_WCHAR)vk;out[1]=0;return;}if(vk>=0x70&&vk<=0x87){wl_wcat(out,(const WL_WCHAR[]){'F',0},cap);wadd_num(out,cap,(int)(vk-0x6f));return;}if(vk>=0x60&&vk<=0x69){wl_wcat(out,(const WL_WCHAR[]){'N','u','m',' ',0},cap);wadd_num(out,cap,(int)(vk-0x60));return;}WL_UINT sc=g_api.MapVirtualKeyW(vk,0);WL_I32 lp=(WL_I32)(sc<<16);if(!g_api.GetKeyNameTextW(lp,out,(WL_I32)cap))wadd_num(out,cap,(int)vk);}
static void hotkey_text(int i,WL_WCHAR*out,WL_SIZE_T cap){out[0]=0;WL_U32 m=g_pads[i].mods,v=g_pads[i].vk;if(!v){wadd_ascii(out,cap,"None");return;}if(m&MOD_CONTROL)wadd_ascii(out,cap,"Ctrl+");if(m&MOD_ALT)wadd_ascii(out,cap,"Alt+");if(m&MOD_SHIFT)wadd_ascii(out,cap,"Shift+");if(m&MOD_WIN)wadd_ascii(out,cap,"Win+");WL_WCHAR k[64];key_name(v,k,64);wl_wcat(out,k,cap);}
static void update_pad_button(int i){if(!g_padButtons[i])return;WL_WCHAR t[512];t[0]=0;wadd_ascii(t,512,"Pad ");wadd_num(t,512,i+1);wl_wcat(t,(const WL_WCHAR[]){'\r','\n',0},512);if(g_pads[i].path[0]){const WL_WCHAR*b=base_name(g_pads[i].path);WL_SIZE_T len=wl_wlen(b);if(len>26)b+=len-26;wl_wcat(t,b,512);}else wadd_ascii(t,512,"(empty - click to load)");wl_wcat(t,(const WL_WCHAR[]){'\r','\n',0},512);WL_WCHAR hk[100];hotkey_text(i,hk,100);wl_wcat(t,hk,512);g_api.SetWindowTextW(g_padButtons[i],t);}
static void update_selected_text(void){if(!g_selectedText)return;WL_WCHAR t[256];t[0]=0;wadd_ascii(t,256,"Selected pad: ");wadd_num(t,256,g_selected+1);wadd_ascii(t,256,"    Volume: ");wadd_num(t,256,(int)(g_pads[g_selected].volume*100.0f+0.5f));wadd_ascii(t,256,"%    Hotkey: ");WL_WCHAR hk[100];hotkey_text(g_selected,hk,100);wl_wcat(t,hk,256);g_api.SetWindowTextW(g_selectedText,t);}
static void set_status_ascii(const char*s){if(!g_statusText)return;WL_WCHAR t[300];wset_ascii(t,300,s);g_api.SetWindowTextW(g_statusText,t);}
static void update_conn_text(void){if(!g_connText)return;WL_WCHAR t[128];t[0]=0;wadd_ascii(t,128,"VST audio connections: ");wadd_num(t,128,__atomic_load_n(&g_connections,__ATOMIC_RELAXED));g_api.SetWindowTextW(g_connText,t);}
static int register_pad_hotkey(int i){g_api.UnregisterHotKey(g_main,HOTKEY_BASE+i);if(!g_pads[i].vk)return 1;return g_api.RegisterHotKey(g_main,HOTKEY_BASE+i,g_pads[i].mods|MOD_NOREPEAT,g_pads[i].vk)?1:0;}
static void trigger_pad(int i){if(i<0||i>=PAD_COUNT||!g_pads[i].mono)return;__atomic_add_fetch(&g_triggerSerial[i],1,__ATOMIC_SEQ_CST);}

static int choose_file_for_pad(int i){static const WL_WCHAR FILTER[]={ 'W','A','V',' ','f','i','l','e','s',' ','(','*','.','w','a','v',')',0,'*','.','w','a','v',0,'A','l','l',' ','f','i','l','e','s',0,'*','.','*',0,0};WL_WCHAR file[MAX_PATH_W];file[0]=0;WL_OPENFILENAMEW ofn;memset(&ofn,0,sizeof(ofn));ofn.lStructSize=sizeof(ofn);ofn.hwndOwner=g_main;ofn.lpstrFilter=FILTER;ofn.lpstrFile=file;ofn.nMaxFile=MAX_PATH_W;ofn.Flags=OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_EXPLORER|OFN_NOCHANGEDIR;ofn.lpstrDefExt=(const WL_WCHAR[]){'w','a','v',0};if(!g_api.GetOpenFileNameW(&ofn))return 0;if(!load_wav_into_pad(i,file)){g_api.MessageBoxW(g_main,(const WL_WCHAR[]){'T','h','a','t',' ','W','A','V',' ','f','i','l','e',' ','i','s',' ','n','o','t',' ','s','u','p','p','o','r','t','e','d','.',0},WINDOW_TITLE,0x10);return 0;}save_config();update_pad_button(i);update_selected_text();set_status_ascii("Sample loaded. Click the pad or use its hotkey to play it into the microphone.");return 1;}

/* Pipe audio server. */
typedef struct { WL_HANDLE pipe; WL_U32 sampleRate; WL_I64 seen[PAD_COUNT]; WL_I64 seenStop; struct {WL_I32 active,pad;double pos;} voices[MAX_VOICES]; } Client;
static int pipe_read_exact(WL_HANDLE h,void*buf,WL_U32 bytes){WL_U8*d=(WL_U8*)buf;WL_U32 done=0;while(done<bytes&&g_running){WL_DWORD n=0;if(!g_api.ReadFile(h,d+done,bytes-done,&n,0)||!n)return 0;done+=n;}return done==bytes;}
static int pipe_write_exact(WL_HANDLE h,const void*buf,WL_U32 bytes){const WL_U8*s=(const WL_U8*)buf;WL_U32 done=0;while(done<bytes&&g_running){WL_DWORD n=0;if(!g_api.WriteFile(h,s+done,bytes-done,&n,0)||!n)return 0;done+=n;}return done==bytes;}
static void add_voice(Client*c,int pad){for(int v=0;v<MAX_VOICES;v++)if(!c->voices[v].active){c->voices[v].active=1;c->voices[v].pad=pad;c->voices[v].pos=0;return;}c->voices[0].active=1;c->voices[0].pad=pad;c->voices[0].pos=0;}
static void sync_triggers(Client*c){WL_I64 stop=__atomic_load_n(&g_stopSerial,__ATOMIC_ACQUIRE);if(stop!=c->seenStop){for(int v=0;v<MAX_VOICES;v++)c->voices[v].active=0;c->seenStop=stop;}for(int i=0;i<PAD_COUNT;i++){WL_I64 now=__atomic_load_n(&g_triggerSerial[i],__ATOMIC_ACQUIRE),delta=now-c->seen[i];if(delta>8)delta=8;while(delta-->0)add_voice(c,i);c->seen[i]=now;}}
static void generate_audio(Client*c,float*out,WL_U32 frames){sync_triggers(c);spin_lock();for(WL_U32 f=0;f<frames;f++){float sum=0;for(int v=0;v<MAX_VOICES;v++){if(!c->voices[v].active)continue;Pad*p=&g_pads[c->voices[v].pad];if(!p->mono||!p->frames||!p->sampleRate){c->voices[v].active=0;continue;}double pos=c->voices[v].pos;WL_U64 i0=(WL_U64)pos;if(i0>=p->frames){c->voices[v].active=0;continue;}WL_U64 i1=i0+1<p->frames?i0+1:i0;float frac=(float)(pos-(double)i0);float s=p->mono[i0]+(p->mono[i1]-p->mono[i0])*frac;sum+=s*p->volume;pos+=(double)p->sampleRate/(double)c->sampleRate;c->voices[v].pos=pos;if(pos>=(double)p->frames)c->voices[v].active=0;}sum=fclamp(sum,-4.0f,4.0f);out[f*2]=sum;out[f*2+1]=sum;}spin_unlock();}
static WL_DWORD WL_CALLBACK client_thread(void* ctx){Client*c=(Client*)ctx;__atomic_add_fetch(&g_connections,1,__ATOMIC_SEQ_CST);update_conn_text();PipeHello hi;if(!pipe_read_exact(c->pipe,&hi,sizeof(hi))||hi.magic!=M_HELLO||hi.version!=1||hi.sampleRate<8000||hi.sampleRate>384000){goto done;}c->sampleRate=hi.sampleRate;for(int i=0;i<PAD_COUNT;i++)c->seen[i]=__atomic_load_n(&g_triggerSerial[i],__ATOMIC_ACQUIRE);c->seenStop=__atomic_load_n(&g_stopSerial,__ATOMIC_ACQUIRE);float*audio=(float*)g_api.HeapAlloc(g_api.GetProcessHeap(),0,2048*2*sizeof(float));if(!audio)goto done;while(g_running){PipeRequest rq;if(!pipe_read_exact(c->pipe,&rq,sizeof(rq)))break;if(rq.magic!=M_REQ||rq.frames==0||rq.frames>2048)break;generate_audio(c,audio,rq.frames);PipeAudio ah={M_AUDIO,rq.frames,2};if(!pipe_write_exact(c->pipe,&ah,sizeof(ah))||!pipe_write_exact(c->pipe,audio,rq.frames*2*sizeof(float)))break;}g_api.HeapFree(g_api.GetProcessHeap(),0,audio);
done:g_api.DisconnectNamedPipe(c->pipe);g_api.CloseHandle(c->pipe);g_api.HeapFree(g_api.GetProcessHeap(),0,c);__atomic_sub_fetch(&g_connections,1,__ATOMIC_SEQ_CST);update_conn_text();return 0;}
static WL_DWORD WL_CALLBACK server_thread(void* ctx){(void)ctx;void*sd=0;WL_SECURITY_ATTRIBUTES sa;memset(&sa,0,sizeof(sa));sa.nLength=sizeof(sa);static const WL_WCHAR SDDL[]={ 'D',':','(','A',';',';','G','A',';',';',';','W','D',')',0};if(g_api.ConvertStringSecurityDescriptorToSecurityDescriptorW(SDDL,SDDL_REVISION_1,&sd,0)){sa.lpSecurityDescriptor=sd;}while(g_running){WL_HANDLE p=g_api.CreateNamedPipeW(PIPE_NAME,PIPE_ACCESS_DUPLEX,PIPE_TYPE_MESSAGE|PIPE_READMODE_MESSAGE|PIPE_WAIT|PIPE_REJECT_REMOTE_CLIENTS,PIPE_UNLIMITED_INSTANCES,16384,4096,0,sd?&sa:0);if(p==WL_INVALID_HANDLE_VALUE){g_api.Sleep(500);continue;}WL_BOOL ok=g_api.ConnectNamedPipe(p,0);if(!ok&&g_api.GetLastError()!=ERROR_PIPE_CONNECTED){g_api.CloseHandle(p);g_api.Sleep(50);continue;}Client*c=(Client*)g_api.HeapAlloc(g_api.GetProcessHeap(),0,sizeof(Client));if(!c){g_api.CloseHandle(p);continue;}memset(c,0,sizeof(*c));c->pipe=p;WL_HANDLE th=g_api.CreateThread(0,0,client_thread,c,0,0);if(th)g_api.CloseHandle(th);else{g_api.CloseHandle(p);g_api.HeapFree(g_api.GetProcessHeap(),0,c);}}if(sd)g_api.LocalFree(sd);return 0;}

static void create_child(WL_HWND* out,const WL_WCHAR* cls,const WL_WCHAR* text,WL_DWORD style,int x,int y,int w,int h,int id){*out=g_api.CreateWindowExW(0,cls,text,WS_CHILD|WS_VISIBLE|style,x,y,w,h,g_main,(void*)(WL_UPTR)id,(WL_HINSTANCE)g_api.GetModuleHandleW(0),0);if(*out&&g_font)g_api.SendMessageW(*out,WM_SETFONT,(WL_WPARAM)g_font,1);}
static WL_LRESULT WL_CALLBACK wndproc(WL_HWND hwnd,WL_UINT msg,WL_WPARAM wp,WL_LPARAM lp){(void)lp;if(msg==WM_CREATE){g_main=hwnd;g_font=(WL_HFONT)g_api.GetStockObject(DEFAULT_GUI_FONT);for(int i=0;i<PAD_COUNT;i++){int col=i%4,row=i/4;create_child(&g_padButtons[i],BTN_CLASS,(const WL_WCHAR[]){0},BS_PUSHBUTTON|BS_MULTILINE,15+col*180,15+row*90,170,80,ID_PAD_BASE+i);update_pad_button(i);}create_child(&g_selectedText,STATIC_CLASS,(const WL_WCHAR[]){0},SS_LEFT,15,385,710,24,500);WL_HWND b;create_child(&b,BTN_CLASS,(const WL_WCHAR[]){'L','o','a','d',' ','/',' ','R','e','p','l','a','c','e',' ','W','A','V',0},BS_PUSHBUTTON,15,418,150,34,ID_LOAD);create_child(&b,BTN_CLASS,(const WL_WCHAR[]){'S','e','t',' ','H','o','t','k','e','y',0},BS_PUSHBUTTON,175,418,110,34,ID_HOTKEY);create_child(&b,BTN_CLASS,(const WL_WCHAR[]){'C','l','e','a','r',' ','H','o','t','k','e','y',0},BS_PUSHBUTTON,295,418,120,34,ID_CLEAR_HOTKEY);create_child(&b,BTN_CLASS,(const WL_WCHAR[]){'V','o','l',' ','-',0},BS_PUSHBUTTON,425,418,70,34,ID_VOL_DOWN);create_child(&b,BTN_CLASS,(const WL_WCHAR[]){'V','o','l',' ','+',0},BS_PUSHBUTTON,505,418,70,34,ID_VOL_UP);create_child(&b,BTN_CLASS,(const WL_WCHAR[]){'S','t','o','p',' ','A','l','l',0},BS_PUSHBUTTON,585,418,95,34,ID_STOP_ALL);create_child(&g_connText,STATIC_CLASS,(const WL_WCHAR[]){0},SS_LEFT,15,463,300,22,501);create_child(&g_statusText,STATIC_CLASS,(const WL_WCHAR[]){0},SS_LEFT,15,492,710,44,502);for(int i=0;i<PAD_COUNT;i++)if(g_pads[i].vk&&!register_pad_hotkey(i)){}update_selected_text();update_conn_text();set_status_ascii("Ready. Empty pad = choose a WAV. Loaded pad = play. Use Set Hotkey for a global shortcut.");return 0;}
 if(msg==WM_COMMAND&&HIWORD_(wp)==BN_CLICKED){int id=LOWORD_(wp);if(id>=ID_PAD_BASE&&id<ID_PAD_BASE+PAD_COUNT){int i=id-ID_PAD_BASE;g_selected=i;update_selected_text();if(g_pads[i].mono)trigger_pad(i);else choose_file_for_pad(i);return 0;}if(id==ID_LOAD){choose_file_for_pad(g_selected);return 0;}if(id==ID_HOTKEY){g_capturePad=g_selected;g_api.SetFocus(g_main);set_status_ascii("Press the key combination you want for this pad. Esc cancels.");return 0;}if(id==ID_CLEAR_HOTKEY){g_api.UnregisterHotKey(g_main,HOTKEY_BASE+g_selected);g_pads[g_selected].vk=0;g_pads[g_selected].mods=0;save_config();update_pad_button(g_selected);update_selected_text();set_status_ascii("Hotkey cleared.");return 0;}if(id==ID_VOL_DOWN||id==ID_VOL_UP){float d=id==ID_VOL_UP?0.1f:-0.1f;g_pads[g_selected].volume=fclamp(g_pads[g_selected].volume+d,0,2);save_config();update_selected_text();return 0;}if(id==ID_STOP_ALL){__atomic_add_fetch(&g_stopSerial,1,__ATOMIC_SEQ_CST);set_status_ascii("All soundboard playback stopped.");return 0;}}
 if((msg==WM_KEYDOWN||msg==WM_SYSKEYDOWN)&&g_capturePad>=0){WL_U32 vk=(WL_U32)wp;if(vk==0x1b){g_capturePad=-1;set_status_ascii("Hotkey capture cancelled.");return 0;}if(vk==VK_SHIFT||vk==VK_CONTROL||vk==VK_MENU||vk==VK_LWIN||vk==VK_RWIN)return 0;WL_U32 mods=0;if(g_api.GetKeyState(VK_CONTROL)<0)mods|=MOD_CONTROL;if(g_api.GetKeyState(VK_MENU)<0)mods|=MOD_ALT;if(g_api.GetKeyState(VK_SHIFT)<0)mods|=MOD_SHIFT;if(g_api.GetKeyState(VK_LWIN)<0||g_api.GetKeyState(VK_RWIN)<0)mods|=MOD_WIN;int pad=g_capturePad;g_capturePad=-1;g_api.UnregisterHotKey(g_main,HOTKEY_BASE+pad);WL_U32 oldvk=g_pads[pad].vk,oldmods=g_pads[pad].mods;g_pads[pad].vk=vk;g_pads[pad].mods=mods;if(!register_pad_hotkey(pad)){g_pads[pad].vk=oldvk;g_pads[pad].mods=oldmods;register_pad_hotkey(pad);g_api.MessageBoxW(g_main,(const WL_WCHAR[]){'T','h','a','t',' ','h','o','t','k','e','y',' ','i','s',' ','a','l','r','e','a','d','y',' ','i','n',' ','u','s','e','.',0},WINDOW_TITLE,0x30);}else{save_config();set_status_ascii("Global hotkey saved.");}update_pad_button(pad);update_selected_text();return 0;}
 if(msg==WM_HOTKEY){int id=(int)wp;if(id>=HOTKEY_BASE&&id<HOTKEY_BASE+PAD_COUNT){trigger_pad(id-HOTKEY_BASE);return 0;}}
 if(msg==WM_DESTROY){g_running=0;for(int i=0;i<PAD_COUNT;i++)g_api.UnregisterHotKey(g_main,HOTKEY_BASE+i);g_api.PostQuitMessage(0);return 0;}return g_api.DefWindowProcW(hwnd,msg,wp,lp);}

void WL_CALLBACK entry(void){if(!wl_init_gui(&g_api))return;static const WL_WCHAR MUTEX_NAME[]={'L','o','c','a','l','\\','A','P','O','S','o','u','n','d','b','o','a','r','d','C','o','n','t','r','o','l','l','e','r','_','v','1',0};WL_HANDLE mx=g_api.CreateMutexW(0,WL_TRUE,MUTEX_NAME);if(mx&&g_api.GetLastError()==183)g_api.ExitProcess(0);load_config();WL_HANDLE srv=g_api.CreateThread(0,0,server_thread,0,0,0);if(srv)g_api.CloseHandle(srv);WL_WNDCLASSEXW wc;memset(&wc,0,sizeof(wc));wc.cbSize=sizeof(wc);wc.lpfnWndProc=wndproc;wc.hInstance=(WL_HINSTANCE)g_api.GetModuleHandleW(0);wc.hbrBackground=(WL_HBRUSH)(WL_UPTR)(COLOR_WINDOW+1);wc.lpszClassName=CLASS_NAME;if(!g_api.RegisterClassExW(&wc))g_api.ExitProcess(2);WL_HWND w=g_api.CreateWindowExW(0,CLASS_NAME,WINDOW_TITLE,WS_OVERLAPPEDWINDOW,100,100,760,585,0,0,wc.hInstance,0);if(!w)g_api.ExitProcess(3);g_api.ShowWindow(w,SW_SHOW);g_api.UpdateWindow(w);WL_MSG m;while(g_api.GetMessageW(&m,0,0,0)>0){g_api.TranslateMessage(&m);g_api.DispatchMessageW(&m);}g_api.ExitProcess(0);}