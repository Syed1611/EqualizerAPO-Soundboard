#ifndef WINLITE_H
#define WINLITE_H

/* Minimal Win32 declarations + no-import runtime resolver for x64 Windows. */
typedef unsigned char  WL_U8;
typedef signed char    WL_I8;
typedef unsigned short WL_U16;
typedef signed short   WL_I16;
typedef unsigned int   WL_U32;
typedef signed int     WL_I32;
typedef unsigned long long WL_U64;
typedef signed long long   WL_I64;
typedef WL_U64 WL_UPTR;
typedef WL_I64 WL_IPTR;
typedef void* WL_HANDLE;
typedef void* WL_HWND;
typedef void* WL_HINSTANCE;
typedef void* WL_HMODULE;
typedef void* WL_HBRUSH;
typedef void* WL_HFONT;
typedef void* WL_HDC;
typedef void* WL_HPEN;
typedef void* WL_HGDIOBJ;
typedef unsigned short WL_WCHAR;
typedef WL_U64 WL_WPARAM;
typedef WL_I64 WL_LPARAM;
typedef WL_I64 WL_LRESULT;
typedef WL_U32 WL_DWORD;
typedef WL_I32 WL_BOOL;
typedef WL_U16 WL_WORD;
typedef WL_U32 WL_UINT;
typedef WL_U64 WL_SIZE_T;
#define WL_NULL ((void*)0)
#define WL_TRUE 1
#define WL_FALSE 0
#define WL_INVALID_HANDLE_VALUE ((WL_HANDLE)(WL_IPTR)-1)
#define WL_WINAPI __attribute__((ms_abi))
#define WL_CALLBACK __attribute__((ms_abi))

void* memcpy(void* d, const void* s, WL_SIZE_T n) { WL_U8* D=(WL_U8*)d; const WL_U8* S=(const WL_U8*)s; while(n--) *D++=*S++; return d; }
void* memset(void* d, int c, WL_SIZE_T n) { WL_U8* D=(WL_U8*)d; while(n--) *D++=(WL_U8)c; return d; }
int memcmp(const void* a,const void* b,WL_SIZE_T n){const WL_U8*A=(const WL_U8*)a,*B=(const WL_U8*)b;while(n--){if(*A!=*B)return *A<*B?-1:1;A++;B++;}return 0;}
int _fltused = 0;

static WL_SIZE_T wl_wlen(const WL_WCHAR* s){WL_SIZE_T n=0;if(!s)return 0;while(s[n])n++;return n;}
static WL_SIZE_T wl_alen(const char* s){WL_SIZE_T n=0;if(!s)return 0;while(s[n])n++;return n;}
static void wl_wcpy(WL_WCHAR* d,const WL_WCHAR*s,WL_SIZE_T cap){if(!cap)return;WL_SIZE_T i=0;while(i+1<cap&&s&&s[i]){d[i]=s[i];i++;}d[i]=0;}
static void wl_wcat(WL_WCHAR* d,const WL_WCHAR*s,WL_SIZE_T cap){WL_SIZE_T i=wl_wlen(d),j=0;if(i>=cap)return;while(i+1<cap&&s&&s[j])d[i++]=s[j++];d[i]=0;}
static int wl_ascii_eq(const char*a,const char*b){while(*a&&*b&&*a==*b){a++;b++;}return *a==*b;}
static char wl_lowera(char c){return c>='A'&&c<='Z'?(char)(c+32):c;}
static int wl_module_name_eq(const WL_WCHAR* w,WL_U16 wbytes,const char*a){WL_U32 n=wbytes/2,i=0;for(;i<n&&a[i];i++){unsigned c=w[i];char d=a[i];if(c>='A'&&c<='Z')c+=32;d=wl_lowera(d);if(c!=(unsigned char)d)return 0;}return i==n&&a[i]==0;}

/* PEB structures, x64 offsets used by all supported Windows 10/11 builds. */
typedef struct { void* Flink; void* Blink; } WL_LIST_ENTRY;
typedef struct { WL_U16 Length,MaximumLength; WL_WCHAR* Buffer; } WL_UNICODE_STRING;
typedef struct { WL_U8 pad0[0x20]; WL_LIST_ENTRY InMemoryOrderModuleList; } WL_PEB_LDR_DATA;
typedef struct { WL_LIST_ENTRY InLoadOrderLinks; WL_LIST_ENTRY InMemoryOrderLinks; WL_LIST_ENTRY InInitializationOrderLinks; void* DllBase; void* EntryPoint; WL_U32 SizeOfImage; WL_U32 pad; WL_UNICODE_STRING FullDllName; WL_UNICODE_STRING BaseDllName; } WL_LDR_DATA_TABLE_ENTRY;
typedef struct { WL_U8 pad0[0x18]; WL_PEB_LDR_DATA* Ldr; } WL_PEB;
static WL_PEB* wl_get_peb(void){WL_PEB*p;__asm__("movq %%gs:0x60,%0":"=r"(p));return p;}
static void* wl_find_module(const char* name){WL_PEB*p=wl_get_peb();if(!p||!p->Ldr)return 0;WL_LIST_ENTRY*head=&p->Ldr->InMemoryOrderModuleList;WL_LIST_ENTRY*cur=(WL_LIST_ENTRY*)head->Flink;while(cur&&cur!=head){WL_LDR_DATA_TABLE_ENTRY*e=(WL_LDR_DATA_TABLE_ENTRY*)((WL_U8*)cur-16);if(e->BaseDllName.Buffer&&wl_module_name_eq(e->BaseDllName.Buffer,e->BaseDllName.Length,name))return e->DllBase;cur=(WL_LIST_ENTRY*)cur->Flink;}return 0;}
static void* wl_get_proc_depth(void* mod,const char* name,int depth){if(!mod||!name||depth>8)return 0;WL_U8*b=(WL_U8*)mod;WL_U32 pe=*(WL_U32*)(b+0x3c);WL_U8*opt=b+pe+24;WL_U32 exrva=*(WL_U32*)(opt+112),exsz=*(WL_U32*)(opt+116);if(!exrva)return 0;WL_U8*ex=b+exrva;WL_U32 n=*(WL_U32*)(ex+24);WL_U32*funcs=(WL_U32*)(b+*(WL_U32*)(ex+28));WL_U32*names=(WL_U32*)(b+*(WL_U32*)(ex+32));WL_U16*ords=(WL_U16*)(b+*(WL_U32*)(ex+36));for(WL_U32 i=0;i<n;i++){char*s=(char*)(b+names[i]);if(wl_ascii_eq(s,name)){WL_U32 r=funcs[ords[i]];if(r>=exrva&&r<exrva+exsz){char*f=(char*)(b+r);char modname[96];char fname[128];int mi=0,fi=0;while(f[mi]&&f[mi]!='.'&&mi<80){modname[mi]=f[mi];mi++;}if(f[mi]!='.')return 0;modname[mi]=0;int pos=mi+1;while(f[pos]&&fi<120)fname[fi++]=f[pos++];fname[fi]=0;if(fname[0]=='#')return 0;int hasdot=0;for(int k=0;k<mi;k++)if(modname[k]=='.')hasdot=1;if(!hasdot&&mi<90){modname[mi++]='.';modname[mi++]='d';modname[mi++]='l';modname[mi++]='l';modname[mi]=0;}void*m=wl_find_module(modname);if(!m){char c0=wl_lowera(modname[0]),c1=wl_lowera(modname[1]),c2=wl_lowera(modname[2]),c3=wl_lowera(modname[3]);if((c0=='a'&&c1=='p'&&c2=='i'&&c3=='-')||(c0=='e'&&c1=='x'&&c2=='t'&&c3=='-'))m=wl_find_module("kernelbase.dll");}if(!m)return 0;return wl_get_proc_depth(m,fname,depth+1);}return b+r;}}return 0;}
static void* wl_get_proc(void* mod,const char* name){return wl_get_proc_depth(mod,name,0);}

/* Core structs. */
typedef struct { WL_I32 left,top,right,bottom; } WL_RECT;
typedef struct { WL_HDC hdc; WL_BOOL fErase; WL_RECT rcPaint; WL_BOOL fRestore; WL_BOOL fIncUpdate; WL_U8 rgbReserved[32]; } WL_PAINTSTRUCT;
typedef struct { WL_HWND hwnd; WL_UINT message; WL_WPARAM wParam; WL_LPARAM lParam; WL_DWORD time; WL_I32 pt_x,pt_y; WL_DWORD lPrivate; } WL_MSG;
typedef WL_LRESULT (WL_CALLBACK *WL_WNDPROC)(WL_HWND,WL_UINT,WL_WPARAM,WL_LPARAM);
typedef struct { WL_UINT cbSize,style; WL_WNDPROC lpfnWndProc; WL_I32 cbClsExtra,cbWndExtra; WL_HINSTANCE hInstance; void* hIcon; void* hCursor; WL_HBRUSH hbrBackground; const WL_WCHAR* lpszMenuName; const WL_WCHAR* lpszClassName; void* hIconSm; } WL_WNDCLASSEXW;
typedef struct { WL_DWORD nLength; void* lpSecurityDescriptor; WL_BOOL bInheritHandle; } WL_SECURITY_ATTRIBUTES;
typedef WL_HANDLE (WL_WINAPI *WL_CreateMutexW)(WL_SECURITY_ATTRIBUTES*,WL_BOOL,const WL_WCHAR*);
typedef struct { WL_DWORD lStructSize; WL_HWND hwndOwner; WL_HINSTANCE hInstance; const WL_WCHAR* lpstrFilter; WL_WCHAR* lpstrCustomFilter; WL_DWORD nMaxCustFilter; WL_DWORD nFilterIndex; WL_WCHAR* lpstrFile; WL_DWORD nMaxFile; WL_WCHAR* lpstrFileTitle; WL_DWORD nMaxFileTitle; const WL_WCHAR* lpstrInitialDir; const WL_WCHAR* lpstrTitle; WL_DWORD Flags; WL_WORD nFileOffset; WL_WORD nFileExtension; const WL_WCHAR* lpstrDefExt; WL_LPARAM lCustData; void* lpfnHook; const WL_WCHAR* lpTemplateName; void* pvReserved; WL_DWORD dwReserved; WL_DWORD FlagsEx; } WL_OPENFILENAMEW;

/* Function pointer types. */
typedef void* (WL_WINAPI *WL_LoadLibraryW)(const WL_WCHAR*);
typedef WL_BOOL (WL_WINAPI *WL_FreeLibrary)(WL_HMODULE);
typedef void* (WL_WINAPI *WL_GetModuleHandleW)(const WL_WCHAR*);
typedef void* (WL_WINAPI *WL_GetProcessHeap)(void);
typedef void* (WL_WINAPI *WL_HeapAlloc)(void*,WL_DWORD,WL_SIZE_T);
typedef WL_BOOL (WL_WINAPI *WL_HeapFree)(void*,WL_DWORD,void*);
typedef WL_HANDLE (WL_WINAPI *WL_CreateThread)(WL_SECURITY_ATTRIBUTES*,WL_SIZE_T,WL_DWORD(WL_CALLBACK*)(void*),void*,WL_DWORD,WL_DWORD*);
typedef void (WL_WINAPI *WL_Sleep)(WL_DWORD);
typedef WL_HANDLE (WL_WINAPI *WL_CreateFileW)(const WL_WCHAR*,WL_DWORD,WL_DWORD,WL_SECURITY_ATTRIBUTES*,WL_DWORD,WL_DWORD,WL_HANDLE);
typedef WL_BOOL (WL_WINAPI *WL_ReadFile)(WL_HANDLE,void*,WL_DWORD,WL_DWORD*,void*);
typedef WL_BOOL (WL_WINAPI *WL_WriteFile)(WL_HANDLE,const void*,WL_DWORD,WL_DWORD*,void*);
typedef WL_BOOL (WL_WINAPI *WL_CloseHandle)(WL_HANDLE);
typedef WL_BOOL (WL_WINAPI *WL_CancelIoEx)(WL_HANDLE,void*);
typedef WL_BOOL (WL_WINAPI *WL_WaitNamedPipeW)(const WL_WCHAR*,WL_DWORD);
typedef WL_DWORD (WL_WINAPI *WL_WaitForSingleObject)(WL_HANDLE,WL_DWORD);
typedef WL_HANDLE (WL_WINAPI *WL_CreateNamedPipeW)(const WL_WCHAR*,WL_DWORD,WL_DWORD,WL_DWORD,WL_DWORD,WL_DWORD,WL_DWORD,WL_SECURITY_ATTRIBUTES*);
typedef WL_BOOL (WL_WINAPI *WL_ConnectNamedPipe)(WL_HANDLE,void*);
typedef WL_BOOL (WL_WINAPI *WL_DisconnectNamedPipe)(WL_HANDLE);
typedef WL_DWORD (WL_WINAPI *WL_GetLastError)(void);
typedef WL_BOOL (WL_WINAPI *WL_GetFileSizeEx)(WL_HANDLE,WL_I64*);
typedef WL_BOOL (WL_WINAPI *WL_CreateDirectoryW)(const WL_WCHAR*,WL_SECURITY_ATTRIBUTES*);
typedef WL_DWORD (WL_WINAPI *WL_GetEnvironmentVariableW)(const WL_WCHAR*,WL_WCHAR*,WL_DWORD);
typedef WL_DWORD (WL_WINAPI *WL_GetModuleFileNameW)(WL_HMODULE,WL_WCHAR*,WL_DWORD);
typedef void* (WL_WINAPI *WL_LocalFree)(void*);
typedef void (WL_WINAPI *WL_ExitProcess)(WL_UINT);

typedef WL_U16 (WL_WINAPI *WL_RegisterClassExW)(const WL_WNDCLASSEXW*);
typedef WL_HWND (WL_WINAPI *WL_CreateWindowExW)(WL_DWORD,const WL_WCHAR*,const WL_WCHAR*,WL_DWORD,WL_I32,WL_I32,WL_I32,WL_I32,WL_HWND,void*,WL_HINSTANCE,void*);
typedef WL_LRESULT (WL_WINAPI *WL_DefWindowProcW)(WL_HWND,WL_UINT,WL_WPARAM,WL_LPARAM);
typedef WL_BOOL (WL_WINAPI *WL_ShowWindow)(WL_HWND,WL_I32);
typedef WL_BOOL (WL_WINAPI *WL_UpdateWindow)(WL_HWND);
typedef WL_I32 (WL_WINAPI *WL_GetMessageW)(WL_MSG*,WL_HWND,WL_UINT,WL_UINT);
typedef WL_BOOL (WL_WINAPI *WL_TranslateMessage)(const WL_MSG*);
typedef WL_LRESULT (WL_WINAPI *WL_DispatchMessageW)(const WL_MSG*);
typedef void (WL_WINAPI *WL_PostQuitMessage)(WL_I32);
typedef WL_BOOL (WL_WINAPI *WL_SetWindowTextW)(WL_HWND,const WL_WCHAR*);
typedef WL_BOOL (WL_WINAPI *WL_PostMessageW)(WL_HWND,WL_UINT,WL_WPARAM,WL_LPARAM);
typedef WL_HWND (WL_WINAPI *WL_SetFocus)(WL_HWND);
typedef short (WL_WINAPI *WL_GetKeyState)(WL_I32);
typedef WL_BOOL (WL_WINAPI *WL_RegisterHotKey)(WL_HWND,WL_I32,WL_UINT,WL_UINT);
typedef WL_BOOL (WL_WINAPI *WL_UnregisterHotKey)(WL_HWND,WL_I32);
typedef WL_I32 (WL_WINAPI *WL_MessageBoxW)(WL_HWND,const WL_WCHAR*,const WL_WCHAR*,WL_UINT);
typedef WL_BOOL (WL_WINAPI *WL_DestroyWindow)(WL_HWND);
typedef WL_UINT (WL_WINAPI *WL_MapVirtualKeyW)(WL_UINT,WL_UINT);
typedef WL_I32 (WL_WINAPI *WL_GetKeyNameTextW)(WL_I32,WL_WCHAR*,WL_I32);
typedef WL_LRESULT (WL_WINAPI *WL_SendMessageW)(WL_HWND,WL_UINT,WL_WPARAM,WL_LPARAM);
typedef WL_HDC (WL_WINAPI *WL_BeginPaint)(WL_HWND,WL_PAINTSTRUCT*);
typedef WL_BOOL (WL_WINAPI *WL_EndPaint)(WL_HWND,const WL_PAINTSTRUCT*);
typedef WL_BOOL (WL_WINAPI *WL_InvalidateRect)(WL_HWND,const WL_RECT*,WL_BOOL);
typedef WL_HWND (WL_WINAPI *WL_SetCapture)(WL_HWND);
typedef WL_BOOL (WL_WINAPI *WL_ReleaseCapture)(void);
typedef WL_I32 (WL_WINAPI *WL_FillRect)(WL_HDC,const WL_RECT*,WL_HBRUSH);
typedef WL_HINSTANCE (WL_WINAPI *WL_ShellExecuteW)(WL_HWND,const WL_WCHAR*,const WL_WCHAR*,const WL_WCHAR*,const WL_WCHAR*,WL_I32);

typedef WL_BOOL (WL_WINAPI *WL_GetOpenFileNameW)(WL_OPENFILENAMEW*);
typedef WL_BOOL (WL_WINAPI *WL_ConvertStringSecurityDescriptorToSecurityDescriptorW)(const WL_WCHAR*,WL_DWORD,void**,WL_DWORD*);
typedef WL_HGDIOBJ (WL_WINAPI *WL_GetStockObject)(WL_I32);
typedef WL_HBRUSH (WL_WINAPI *WL_CreateSolidBrush)(WL_DWORD);
typedef WL_HPEN (WL_WINAPI *WL_CreatePen)(WL_I32,WL_I32,WL_DWORD);
typedef WL_HGDIOBJ (WL_WINAPI *WL_SelectObject)(WL_HDC,WL_HGDIOBJ);
typedef WL_BOOL (WL_WINAPI *WL_DeleteObject)(WL_HGDIOBJ);
typedef WL_BOOL (WL_WINAPI *WL_MoveToEx)(WL_HDC,WL_I32,WL_I32,void*);
typedef WL_BOOL (WL_WINAPI *WL_LineTo)(WL_HDC,WL_I32,WL_I32);

typedef struct {
 void* kernel32; WL_LoadLibraryW LoadLibraryW; WL_FreeLibrary FreeLibrary; WL_GetModuleHandleW GetModuleHandleW; WL_GetProcessHeap GetProcessHeap; WL_HeapAlloc HeapAlloc; WL_HeapFree HeapFree; WL_CreateThread CreateThread; WL_Sleep Sleep; WL_CreateFileW CreateFileW; WL_ReadFile ReadFile; WL_WriteFile WriteFile; WL_CloseHandle CloseHandle; WL_CancelIoEx CancelIoEx; WL_CreateMutexW CreateMutexW; WL_WaitNamedPipeW WaitNamedPipeW; WL_WaitForSingleObject WaitForSingleObject; WL_CreateNamedPipeW CreateNamedPipeW; WL_ConnectNamedPipe ConnectNamedPipe; WL_DisconnectNamedPipe DisconnectNamedPipe; WL_GetLastError GetLastError; WL_GetFileSizeEx GetFileSizeEx; WL_CreateDirectoryW CreateDirectoryW; WL_GetEnvironmentVariableW GetEnvironmentVariableW; WL_GetModuleFileNameW GetModuleFileNameW; WL_LocalFree LocalFree; WL_ExitProcess ExitProcess;
 void* user32; WL_RegisterClassExW RegisterClassExW; WL_CreateWindowExW CreateWindowExW; WL_DefWindowProcW DefWindowProcW; WL_ShowWindow ShowWindow; WL_UpdateWindow UpdateWindow; WL_GetMessageW GetMessageW; WL_TranslateMessage TranslateMessage; WL_DispatchMessageW DispatchMessageW; WL_PostQuitMessage PostQuitMessage; WL_SetWindowTextW SetWindowTextW; WL_PostMessageW PostMessageW; WL_SetFocus SetFocus; WL_GetKeyState GetKeyState; WL_RegisterHotKey RegisterHotKey; WL_UnregisterHotKey UnregisterHotKey; WL_MessageBoxW MessageBoxW; WL_DestroyWindow DestroyWindow; WL_MapVirtualKeyW MapVirtualKeyW; WL_GetKeyNameTextW GetKeyNameTextW; WL_SendMessageW SendMessageW; WL_BeginPaint BeginPaint; WL_EndPaint EndPaint; WL_InvalidateRect InvalidateRect; WL_SetCapture SetCapture; WL_ReleaseCapture ReleaseCapture; WL_FillRect FillRect;
 void* comdlg32; WL_GetOpenFileNameW GetOpenFileNameW;
 void* advapi32; WL_ConvertStringSecurityDescriptorToSecurityDescriptorW ConvertStringSecurityDescriptorToSecurityDescriptorW;
 void* gdi32; WL_GetStockObject GetStockObject; WL_CreateSolidBrush CreateSolidBrush; WL_CreatePen CreatePen; WL_SelectObject SelectObject; WL_DeleteObject DeleteObject; WL_MoveToEx MoveToEx; WL_LineTo LineTo;
 void* shell32; WL_ShellExecuteW ShellExecuteW;
} WL_API;

#define WL_RESOLVE(api,field,mod) do{ (api)->field=(WL_##field)wl_get_proc((mod),#field); if(!(api)->field)return 0; }while(0)
static int wl_init_kernel(WL_API* a){memset(a,0,sizeof(*a));a->kernel32=wl_find_module("kernel32.dll");if(!a->kernel32)return 0;WL_RESOLVE(a,LoadLibraryW,a->kernel32);WL_RESOLVE(a,FreeLibrary,a->kernel32);WL_RESOLVE(a,GetModuleHandleW,a->kernel32);WL_RESOLVE(a,GetProcessHeap,a->kernel32);WL_RESOLVE(a,HeapAlloc,a->kernel32);WL_RESOLVE(a,HeapFree,a->kernel32);WL_RESOLVE(a,CreateThread,a->kernel32);WL_RESOLVE(a,Sleep,a->kernel32);WL_RESOLVE(a,CreateFileW,a->kernel32);WL_RESOLVE(a,ReadFile,a->kernel32);WL_RESOLVE(a,WriteFile,a->kernel32);WL_RESOLVE(a,CloseHandle,a->kernel32);WL_RESOLVE(a,CancelIoEx,a->kernel32);WL_RESOLVE(a,CreateMutexW,a->kernel32);WL_RESOLVE(a,WaitNamedPipeW,a->kernel32);WL_RESOLVE(a,WaitForSingleObject,a->kernel32);WL_RESOLVE(a,CreateNamedPipeW,a->kernel32);WL_RESOLVE(a,ConnectNamedPipe,a->kernel32);WL_RESOLVE(a,DisconnectNamedPipe,a->kernel32);WL_RESOLVE(a,GetLastError,a->kernel32);WL_RESOLVE(a,GetFileSizeEx,a->kernel32);WL_RESOLVE(a,CreateDirectoryW,a->kernel32);WL_RESOLVE(a,GetEnvironmentVariableW,a->kernel32);WL_RESOLVE(a,GetModuleFileNameW,a->kernel32);WL_RESOLVE(a,LocalFree,a->kernel32);WL_RESOLVE(a,ExitProcess,a->kernel32);return 1;}
static const WL_WCHAR wl_user32_name[]={'u','s','e','r','3','2','.','d','l','l',0};
static const WL_WCHAR wl_comdlg32_name[]={'c','o','m','d','l','g','3','2','.','d','l','l',0};
static const WL_WCHAR wl_advapi32_name[]={'a','d','v','a','p','i','3','2','.','d','l','l',0};
static const WL_WCHAR wl_gdi32_name[]={'g','d','i','3','2','.','d','l','l',0};
static const WL_WCHAR wl_shell32_name[]={'s','h','e','l','l','3','2','.','d','l','l',0};
static int wl_init_gui(WL_API* a){if(!wl_init_kernel(a))return 0;a->user32=a->LoadLibraryW(wl_user32_name);a->comdlg32=a->LoadLibraryW(wl_comdlg32_name);a->advapi32=a->LoadLibraryW(wl_advapi32_name);a->gdi32=a->LoadLibraryW(wl_gdi32_name);a->shell32=a->LoadLibraryW(wl_shell32_name);if(!a->user32||!a->comdlg32||!a->advapi32||!a->gdi32||!a->shell32)return 0;WL_RESOLVE(a,RegisterClassExW,a->user32);WL_RESOLVE(a,CreateWindowExW,a->user32);WL_RESOLVE(a,DefWindowProcW,a->user32);WL_RESOLVE(a,ShowWindow,a->user32);WL_RESOLVE(a,UpdateWindow,a->user32);WL_RESOLVE(a,GetMessageW,a->user32);WL_RESOLVE(a,TranslateMessage,a->user32);WL_RESOLVE(a,DispatchMessageW,a->user32);WL_RESOLVE(a,PostQuitMessage,a->user32);WL_RESOLVE(a,SetWindowTextW,a->user32);WL_RESOLVE(a,PostMessageW,a->user32);WL_RESOLVE(a,SetFocus,a->user32);WL_RESOLVE(a,GetKeyState,a->user32);WL_RESOLVE(a,RegisterHotKey,a->user32);WL_RESOLVE(a,UnregisterHotKey,a->user32);WL_RESOLVE(a,MessageBoxW,a->user32);WL_RESOLVE(a,DestroyWindow,a->user32);WL_RESOLVE(a,MapVirtualKeyW,a->user32);WL_RESOLVE(a,GetKeyNameTextW,a->user32);WL_RESOLVE(a,SendMessageW,a->user32);WL_RESOLVE(a,BeginPaint,a->user32);WL_RESOLVE(a,EndPaint,a->user32);WL_RESOLVE(a,InvalidateRect,a->user32);WL_RESOLVE(a,SetCapture,a->user32);WL_RESOLVE(a,ReleaseCapture,a->user32);WL_RESOLVE(a,FillRect,a->user32);WL_RESOLVE(a,GetOpenFileNameW,a->comdlg32);WL_RESOLVE(a,ConvertStringSecurityDescriptorToSecurityDescriptorW,a->advapi32);WL_RESOLVE(a,GetStockObject,a->gdi32);WL_RESOLVE(a,CreateSolidBrush,a->gdi32);WL_RESOLVE(a,CreatePen,a->gdi32);WL_RESOLVE(a,SelectObject,a->gdi32);WL_RESOLVE(a,DeleteObject,a->gdi32);WL_RESOLVE(a,MoveToEx,a->gdi32);WL_RESOLVE(a,LineTo,a->gdi32);WL_RESOLVE(a,ShellExecuteW,a->shell32);return 1;}

#endif