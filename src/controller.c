#include "winlite.h"

#define PAD_COUNT 16
#define MAX_PATH_W 260
#define MAX_VOICES 32
#define HOTKEY_BASE 1000
#define HOTKEY_STOP 2000
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
#define ID_RENAME 310
#define ID_SET_STOP_HOTKEY 311
#define ID_CLEAR_STOP_HOTKEY 312
#define ID_TOGGLE_MUTE 313
#define ID_SET_MUTE_HOTKEY 314
#define ID_CLEAR_MUTE_HOTKEY 315
#define ID_RELOCATE_MISSING 316
#define ID_SELF_TEST 317
#define ID_RESET_STATS 318
#define ID_TOGGLE_MONITOR 319
#define ID_REMOVE_AUDIO 320
#define ID_RELOAD_AUDIO 321
#define ID_RESET_PAD 322
#define ID_MONITOR_DEVICE 323
#define ID_MONITOR_TEST 324
#define ID_REFRESH_DEVICES 325
#define ID_CTX_RENAME 330
#define ID_CTX_RESET_VOL 331
#define ID_CTX_RESET_PITCH 332
#define ID_CTX_OPEN_LOCATION 333
#define ID_DUP_BASE 400
#define ID_TAB 450
#define ID_TAB_SOUND 451
#define ID_TAB_DIAG 452

#define WM_CREATE 0x0001u
#define WM_DESTROY 0x0002u
#define WM_SIZE 0x0005u
#define WM_NOTIFY 0x004Eu
#define WM_CONTEXTMENU 0x007Bu
#define WM_GETMINMAXINFO 0x0024u
#define WM_DPICHANGED 0x02E0u
#define WM_COMMAND 0x0111u
#define WM_KEYDOWN 0x0100u
#define WM_SYSKEYDOWN 0x0104u
#define WM_HOTKEY 0x0312u
#define WM_TIMER 0x0113u
#define WM_SETFONT 0x0030u
#define WM_SETREDRAW 0x000Bu
#define WM_PAINT 0x000Fu
#define WM_ERASEBKGND 0x0014u
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
#define WS_CLIPCHILDREN 0x02000000u
#define WS_EX_COMPOSITED 0x02000000u
#define WS_VSCROLL 0x00200000u
#define WS_CHILD 0x40000000u
#define WS_VISIBLE 0x10000000u
#define BS_PUSHBUTTON 0x00000000u
#define BS_MULTILINE 0x00002000u
#define BS_GROUPBOX 0x00000007u
#define WS_BORDER 0x00800000u
#define ES_AUTOHSCROLL 0x0080u
#define SS_LEFT 0x00000000u
#define CBS_DROPDOWNLIST 0x0003u
#define CBN_SELCHANGE 1
#define CB_ADDSTRING 0x0143u
#define CB_RESETCONTENT 0x014Bu
#define CB_SETCURSEL 0x014Eu
#define CB_GETCURSEL 0x0147u
#define CB_SETITEMDATA 0x0151u
#define CB_GETITEMDATA 0x0150u
#define IDYES 6
#define MB_YESNO 0x00000004u
#define MB_ICONWARNING 0x00000030u
#define SW_HIDE 0
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
#define OFN_EXPLORER 0x00080100u
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
#define WM_PLAY_STATE (WM_APP+2u)
#define WM_AUDIO_STATS (WM_APP+3u)
#define WM_LOAD_DONE (WM_APP+4u)
#define WM_HOST_RATE (WM_APP+5u)
#define WM_PROTOCOL_STATUS (WM_APP+6u)
#define WM_MONITOR_STATUS (WM_APP+7u)
#define DIAG_TIMER_ID 77u
#define PLAYHEAD_TIMER_ID 78u
#define FAST_TIMER_ID 79u
#define MOVEFILE_REPLACE_EXISTING 0x00000001u
#define MOVEFILE_WRITE_THROUGH 0x00000008u
#define TH32CS_SNAPTHREAD 0x00000004u
#define GR_GDIOBJECTS 0u
#define GR_USEROBJECTS 1u
#define ALL_PROCESSOR_GROUPS 0xffffu
#define WAVE_X g_waveX
#define WAVE_Y g_waveY
#define WAVE_W g_waveW
#define WAVE_H g_waveH
#define VOL_X g_volX
#define VOL_Y g_volY
#define VOL_W g_volW
#define VOL_H g_volH
#define PITCH_X g_pitchX
#define PITCH_Y g_pitchY
#define PITCH_W g_pitchW
#define PITCH_H g_pitchH
#define MONVOL_X g_monVolX
#define MONVOL_Y g_monVolY
#define MONVOL_W g_monVolW
#define MONVOL_H g_monVolH
#define PROTOCOL_VERSION 3u
#define BUILD_VERSION 80700u
#define MAX_DECODE_WORKERS 2
#define PIPE_AUDIO_IDLE 0x00000001u
#define PAD_EMPTY 0
#define PAD_READY 1
#define PAD_LOADING 2
#define PAD_MISSING 3
#define PAD_DECODE_ERROR 4
#define RGB_(r,g,b) ((WL_DWORD)((r)|((g)<<8)|((b)<<16)))
#define SRCCOPY 0x00CC0020u
#define WAVE_FORMAT_PCM 1u
#define WHDR_DONE 0x00000001u
#define WHDR_PREPARED 0x00000002u
#define WAVE_MAPPER ((WL_UPTR)-1)
#define MONITOR_BUFFERS 6
#define MONITOR_MAX_FRAMES 2048
#define MF_STRING 0x00000000u
#define MF_SEPARATOR 0x00000800u
#define MF_POPUP 0x00000010u
#define MF_GRAYED 0x00000001u
#define MF_ENABLED 0x00000000u
#define TPM_RIGHTBUTTON 0x0002u
#define TPM_RETURNCMD 0x0100u
#define TCM_FIRST 0x1300u
#define TCM_GETCURSEL (TCM_FIRST+11u)
#define TCM_SETCURSEL (TCM_FIRST+12u)
#define TCM_INSERTITEMW (TCM_FIRST+62u)
#define TCIF_TEXT 0x0001u
#define TCN_SELCHANGE ((WL_UINT)-551)
#define ICC_TAB_CLASSES 0x00000008u
#define TRANSPARENT 1

#define LOWORD_(x) ((WL_U16)((WL_UPTR)(x)&0xffffu))
#define HIWORD_(x) ((WL_U16)(((WL_UPTR)(x)>>16)&0xffffu))

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
static const WL_WCHAR PIPE_NAME[]={ '\\','\\','.','\\','p','i','p','e','\\','A','P','O','S','o','u','n','d','b','o','a','r','d','_','v','2',0 };

static WL_API g_api;
static WL_HWND g_main=0,g_tabSoundButton=0,g_tabDiagButton=0,g_padButtons[PAD_COUNT],g_selectedText=0,g_statusText=0,g_nameEdit=0,g_applyNameButton=0,g_monitorButton=0,g_fileInfoText=0,g_monitorDeviceCombo=0,g_monitorVolumeText=0;
static WL_HWND g_waveLabel=0,g_monitorOutputLabel=0,g_padNameLabel=0;
static void* g_mainMenu=0,*g_soundboardMenu=0,*g_toolsMenu=0;
static WL_HWND g_diagCards[6]={0},g_diagValues[6]={0};
/* Legacy text handles remain null; grouped Diagnostics cards replace them. */
static WL_HWND g_stopHotkeyText=0,g_muteHotkeyText=0,g_hotkeyStateText=0,g_muteStateText=0;
static WL_HFONT g_font=0;
static WL_HBRUSH g_waveBg=0,g_waveSel=0,g_waveBorder=0,g_windowBg=0,g_controlPanelBg=0,g_controlTrack=0,g_controlFill=0;
static WL_I32 g_activeTab=0; /* 0=Soundboard, 1=Diagnostics */
static WL_I32 g_pageVisibilityTab=-1;
static int g_clientW=960,g_clientH=900,g_savedWindowW=960,g_savedWindowH=900,g_savedTab=0;
static int g_waveX=12,g_waveY=420,g_waveW=920,g_waveH=120;
static int g_volX=40,g_volY=690,g_volW=390,g_volH=20;
static int g_pitchX=520,g_pitchY=690,g_pitchW=390,g_pitchH=20;
static int g_monVolX=520,g_monVolY=610,g_monVolW=390,g_monVolH=18;
static int g_controlPanelX=12,g_controlPanelY=650,g_controlPanelW=936,g_controlPanelH=104;
static WL_HPEN g_wavePen=0,g_waveMidPen=0,g_playheadPen=0;
static volatile WL_I32 g_running=1;
static volatile WL_I32 g_connections=0;
static volatile WL_I32 g_padLock=0;
static volatile WL_I32 g_decodeLock=0;
static volatile WL_I32 g_decodeSlots=0;
static volatile WL_I64 g_totalSampleBytes=0;
static volatile WL_I64 g_triggerSerial[PAD_COUNT];
static volatile WL_I64 g_stopSerial=0;
static volatile WL_I64 g_padStopSerial[PAD_COUNT];
static volatile WL_I32 g_activeVoices[PAD_COUNT];
static WL_I32 g_selected=0;
static WL_I32 g_capturePad=-1;
static WL_I32 g_hotkeysSuspended=0;
static WL_I32 g_waveDragging=0;
static float g_waveAnchor=0.0f;
static WL_I32 g_controlDragging=0; /* 1=volume, 2=pitch, 3=monitor volume */
static WL_I32 g_hotkeyState[PAD_COUNT]; /* 0=none, 1=registered, -1=conflict/rejected */
static WL_U32 g_stopVk=0,g_stopMods=0,g_muteVk=0,g_muteMods=0;
static WL_I32 g_stopHotkeyState=0,g_muteHotkeyState=0;
static volatile WL_I32 g_boardMuted=0;
static WL_I32 g_backupState=0; /* 0=none, 1=ready, 2=restored, 3=error */
static WL_I32 g_loadedFromBackup=0;
static WL_U64 g_diagStartTick=0,g_diagLastTick=0,g_diagLastCpu=0;
static WL_I32 g_diagCpuPct=0;
static volatile WL_U32 g_hostSampleRate=0,g_hostBlockSize=0,g_ringFill=0,g_targetFill=0,g_underruns=0,g_clipCount=0,g_peakMilli=0;
static volatile WL_U32 g_protocolMismatch=0,g_reconnects=0,g_connectEvents=0,g_voiceSteals=0,g_droppedTriggers=0;
static volatile WL_I32 g_protocolStatus=0; /* 0=waiting, 1=current protocol OK, -1=mismatch */
static volatile WL_U64 g_totalUnderrunEvents=0,g_totalClipFrames=0;
static volatile WL_I32 g_playPosPpm[PAD_COUNT];
static volatile WL_U32 g_loadGeneration[PAD_COUNT];
static WL_U32 g_peakCpuPct=0,g_maxRamMb=0,g_lowestRingFill=0xffffffffu;
static WL_U64 g_statsBaseUnderruns=0,g_statsBaseClips=0;
static WL_U32 g_statsBaseSteals=0,g_statsBaseReconnects=0,g_statsBaseDropped=0;
static WL_U32 g_lastResampleTarget=0;
static volatile WL_I32 g_resampleWorkerRunning=0,g_loadWorkers=0;
static WL_U32 g_configLoadedVersion=0,g_configMigratedFrom=0;
static WL_I32 g_lastPlayheadX=-1;
static void update_monitor_volume_text(void);
static void invalidate_wave(void);
static void invalidate_controls(void);

/* Local soundboard monitoring starts OFF every launch. The monitor is isolated
   from the microphone/VST path by a single-producer/single-consumer ring. */
#define MONITOR_RING_FRAMES 65536u
#define MONITOR_DEVICE_MAX 32
#define MONITOR_DEVICE_ID_CHARS 192
#define MONITOR_DEVICE_NAME_CHARS 64
#define MONITOR_TARGET_MS 75u
#define MONITOR_CROSSFADE_MS 5u
static volatile WL_I32 g_monitorEnabled=0;
static volatile WL_U32 g_monitorTestFrames=0;
static volatile WL_UPTR g_monitorOwner=0;
static volatile WL_I32 g_monitorReopen=0;
static volatile WL_I32 g_monitorVolumeHalfPct=200; /* 0..200 = 0..100% */
static volatile WL_U32 g_monitorSourceRate=0,g_monitorDeviceRate=0;
static volatile WL_I32 g_monitorBackendMode=0; /* 0=closed, 1=event WASAPI, 2=shared polling, 3=WinMM waveOut compatibility path */
static volatile WL_U32 g_monitorUnderruns=0,g_monitorOverruns=0,g_monitorErrors=0,g_monitorCorrections=0;
static volatile WL_U32 g_monitorBufferMs=0,g_monitorDeviceLatencyMs=0,g_monitorMaxWakeMs=0;
static volatile WL_I32 g_monitorDriftPpm=0,g_monitorSourceActive=0;
static volatile WL_U64 g_monitorWriteFrame=0,g_monitorReadFrame=0;
static volatile WL_U64 g_monitorDiscAt=0;
static volatile WL_I32 g_monitorDiscValid=0;
static volatile WL_U32 g_monitorEpoch=1;
static float g_monitorRing[MONITOR_RING_FRAMES];
static WL_WCHAR g_monitorDeviceName[MONITOR_DEVICE_NAME_CHARS]={0}; /* empty = Windows default */
static WL_WCHAR g_monitorDeviceId[MONITOR_DEVICE_ID_CHARS]={0};
typedef struct { WL_WCHAR name[MONITOR_DEVICE_NAME_CHARS]; WL_WCHAR id[MONITOR_DEVICE_ID_CHARS]; } MonitorDeviceInfo;
static MonitorDeviceInfo g_monitorDevices[MONITOR_DEVICE_MAX];
static WL_I32 g_monitorDeviceCount=0;
static WL_HANDLE g_monitorThread=0;
static volatile WL_I32 g_monitorThreadStarted=0;
static volatile WL_UPTR g_monitorEventHandle=0;

/* Persistent GDI backbuffer for the high-frequency waveform/playhead paint path. */
static WL_HDC g_backDc=0;
static WL_HGDIOBJ g_backBitmap=0,g_backOldBitmap=0;
static int g_backW=0,g_backH=0;

static const WL_WCHAR CLASS_NAME[]={'A','P','O','S','o','u','n','d','b','o','a','r','d','C','t','r','l',0};
static const WL_WCHAR WINDOW_TITLE[]={'A','P','O',' ','S','o','u','n','d','b','o','a','r','d',' ','C','o','n','t','r','o','l','l','e','r',' ','v','0','.','8','.','7',0};
static const WL_WCHAR BTN_CLASS[]={'B','U','T','T','O','N',0};
static const WL_WCHAR STATIC_CLASS[]={'S','T','A','T','I','C',0};
static const WL_WCHAR EDIT_CLASS[]={'E','D','I','T',0};
static const WL_WCHAR COMBO_CLASS[]={'C','O','M','B','O','B','O','X',0};


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
typedef WL_HDC (WL_WINAPI *PFN_CreateCompatibleDC)(WL_HDC);
typedef WL_BOOL (WL_WINAPI *PFN_DeleteDC)(WL_HDC);
typedef WL_HGDIOBJ (WL_WINAPI *PFN_CreateCompatibleBitmap)(WL_HDC,WL_I32,WL_I32);
typedef WL_BOOL (WL_WINAPI *PFN_BitBlt)(WL_HDC,WL_I32,WL_I32,WL_I32,WL_I32,WL_HDC,WL_I32,WL_I32,WL_DWORD);

#pragma pack(push,1)
typedef struct {
 WL_WORD wFormatTag,nChannels;
 WL_DWORD nSamplesPerSec,nAvgBytesPerSec;
 WL_WORD nBlockAlign,wBitsPerSample,cbSize;
} MON_WAVEFORMATEX;
#pragma pack(pop)
_Static_assert(sizeof(MON_WAVEFORMATEX)==18,"MON_WAVEFORMATEX ABI");
/* WinMM waveOut compatibility monitor. This is the known-good v0.8.1/v0.8.2
   path and is preferred for local monitoring because it enumerates/opens the
   same playback devices that worked reliably in those releases. */
typedef struct MON_WAVEHDR {
 char* lpData;
 WL_DWORD dwBufferLength,dwBytesRecorded;
 WL_UPTR dwUser;
 WL_DWORD dwFlags,dwLoops;
 struct MON_WAVEHDR* lpNext;
 WL_UPTR reserved;
} MON_WAVEHDR;
_Static_assert(sizeof(MON_WAVEHDR)==48,"MON_WAVEHDR ABI");
typedef struct { WL_WORD wMid,wPid; WL_DWORD vDriverVersion; WL_WCHAR szPname[32]; WL_DWORD dwFormats; WL_WORD wChannels,wReserved1; WL_DWORD dwSupport; } MON_WAVEOUTCAPSW;
typedef WL_U32 (WL_WINAPI *PFN_waveOutOpen)(void**,WL_UPTR,const MON_WAVEFORMATEX*,WL_UPTR,WL_UPTR,WL_DWORD);
typedef WL_U32 (WL_WINAPI *PFN_waveOutPrepareHeader)(void*,MON_WAVEHDR*,WL_UINT);
typedef WL_U32 (WL_WINAPI *PFN_waveOutUnprepareHeader)(void*,MON_WAVEHDR*,WL_UINT);
typedef WL_U32 (WL_WINAPI *PFN_waveOutWrite)(void*,MON_WAVEHDR*,WL_UINT);
typedef WL_U32 (WL_WINAPI *PFN_waveOutReset)(void*);
typedef WL_U32 (WL_WINAPI *PFN_waveOutClose)(void*);
typedef WL_U32 (WL_WINAPI *PFN_waveOutGetNumDevs)(void);
typedef WL_U32 (WL_WINAPI *PFN_waveOutGetDevCapsW)(WL_UPTR,MON_WAVEOUTCAPSW*,WL_UINT);
typedef struct { MON_WAVEHDR hdr; WL_I16 samples[MONITOR_MAX_FRAMES*2]; WL_I32 prepared; } MonitorWaveBuffer;
static MonitorWaveBuffer g_monitorWaveBuffers[MONITOR_BUFFERS];
static PFN_waveOutOpen pWaveOutOpen=0;
static PFN_waveOutPrepareHeader pWaveOutPrepareHeader=0;
static PFN_waveOutUnprepareHeader pWaveOutUnprepareHeader=0;
static PFN_waveOutWrite pWaveOutWrite=0;
static PFN_waveOutReset pWaveOutReset=0;
static PFN_waveOutClose pWaveOutClose=0;
static PFN_waveOutGetNumDevs pWaveOutGetNumDevs=0;
static PFN_waveOutGetDevCapsW pWaveOutGetDevCapsW=0;
static void* g_winmm=0;
static void* g_monitorWave=0;
static WL_U32 g_monitorWaveRate=0;
static WL_UPTR g_monitorWaveDeviceId=WAVE_MAPPER;
static WL_U32 g_monitorWaveTestPhase=0;
typedef struct { WL_U32 Data1; WL_U16 Data2,Data3; WL_U8 Data4[8]; } MON_GUID;
typedef struct { MON_GUID fmtid; WL_DWORD pid; } MON_PROPERTYKEY;
typedef struct { WL_U16 vt,wReserved1,wReserved2,wReserved3; union { WL_WCHAR* pwszVal; WL_U64 raw[2]; } u; } MON_PROPVARIANT;
typedef WL_I32 MON_HRESULT;
#define MON_S_OK 0
#define MON_CLSCTX_ALL 0x17u
#define MON_COINIT_MULTITHREADED 0u
#define MON_DEVICE_STATE_ACTIVE 0x00000001u
#define MON_ERENDER 0
#define MON_ECONSOLE 0
#define MON_EMULTIMEDIA 1
#define MON_STGM_READ 0u
#define MON_VT_LPWSTR 31u
#define MON_AUDCLNT_SHAREMODE_SHARED 0
#define MON_AUDCLNT_STREAMFLAGS_EVENTCALLBACK 0x00040000u
#define MON_AUDCLNT_BUFFERFLAGS_SILENT 0x00000002u
#define MON_WAIT_TIMEOUT 258u
#define MON_WAVE_FORMAT_PCM 1u
#define MON_WAVE_FORMAT_IEEE_FLOAT 3u
#define MON_WAVE_FORMAT_EXTENSIBLE 0xfffeu
#define MON_HNS_PER_MS 10000ll

static const MON_GUID MON_CLSID_MMDeviceEnumerator={0xbcde0395u,0xe52fu,0x467cu,{0x8e,0x3d,0xc4,0x57,0x92,0x91,0x69,0x2e}};
static const MON_GUID MON_IID_IMMDeviceEnumerator={0xa95664d2u,0x9614u,0x4f35u,{0xa7,0x46,0xde,0x8d,0xb6,0x36,0x17,0xe6}};
static const MON_GUID MON_IID_IAudioClient={0x1cb9ad4cu,0xdbfau,0x4c32u,{0xb1,0x78,0xc2,0xf5,0x68,0xa7,0x03,0xb2}};
static const MON_GUID MON_IID_IAudioRenderClient={0xf294acfcu,0x3146u,0x4483u,{0xa7,0xbf,0xad,0xdc,0xa7,0xc2,0x60,0xe2}};
static const MON_PROPERTYKEY MON_PKEY_Device_FriendlyName={{0xa45c254eu,0xdf1cu,0x4efdu,{0x80,0x20,0x67,0xd1,0x46,0xa8,0x50,0xe0}},14u};

typedef struct MON_IMMDeviceEnumerator MON_IMMDeviceEnumerator;
typedef struct MON_IMMDeviceCollection MON_IMMDeviceCollection;
typedef struct MON_IMMDevice MON_IMMDevice;
typedef struct MON_IPropertyStore MON_IPropertyStore;
typedef struct MON_IAudioClient MON_IAudioClient;
typedef struct MON_IAudioRenderClient MON_IAudioRenderClient;

typedef struct {
 MON_HRESULT (WL_WINAPI *QueryInterface)(MON_IMMDeviceEnumerator*,const MON_GUID*,void**);
 WL_U32 (WL_WINAPI *AddRef)(MON_IMMDeviceEnumerator*); WL_U32 (WL_WINAPI *Release)(MON_IMMDeviceEnumerator*);
 MON_HRESULT (WL_WINAPI *EnumAudioEndpoints)(MON_IMMDeviceEnumerator*,int,WL_DWORD,MON_IMMDeviceCollection**);
 MON_HRESULT (WL_WINAPI *GetDefaultAudioEndpoint)(MON_IMMDeviceEnumerator*,int,int,MON_IMMDevice**);
 MON_HRESULT (WL_WINAPI *GetDevice)(MON_IMMDeviceEnumerator*,const WL_WCHAR*,MON_IMMDevice**);
 void* RegisterEndpointNotificationCallback; void* UnregisterEndpointNotificationCallback;
} MON_IMMDeviceEnumeratorVtbl;
struct MON_IMMDeviceEnumerator { MON_IMMDeviceEnumeratorVtbl* lpVtbl; };
typedef struct {
 MON_HRESULT (WL_WINAPI *QueryInterface)(MON_IMMDeviceCollection*,const MON_GUID*,void**);
 WL_U32 (WL_WINAPI *AddRef)(MON_IMMDeviceCollection*); WL_U32 (WL_WINAPI *Release)(MON_IMMDeviceCollection*);
 MON_HRESULT (WL_WINAPI *GetCount)(MON_IMMDeviceCollection*,WL_UINT*);
 MON_HRESULT (WL_WINAPI *Item)(MON_IMMDeviceCollection*,WL_UINT,MON_IMMDevice**);
} MON_IMMDeviceCollectionVtbl;
struct MON_IMMDeviceCollection { MON_IMMDeviceCollectionVtbl* lpVtbl; };
typedef struct {
 MON_HRESULT (WL_WINAPI *QueryInterface)(MON_IMMDevice*,const MON_GUID*,void**);
 WL_U32 (WL_WINAPI *AddRef)(MON_IMMDevice*); WL_U32 (WL_WINAPI *Release)(MON_IMMDevice*);
 MON_HRESULT (WL_WINAPI *Activate)(MON_IMMDevice*,const MON_GUID*,WL_DWORD,void*,void**);
 MON_HRESULT (WL_WINAPI *OpenPropertyStore)(MON_IMMDevice*,WL_DWORD,MON_IPropertyStore**);
 MON_HRESULT (WL_WINAPI *GetId)(MON_IMMDevice*,WL_WCHAR**);
 MON_HRESULT (WL_WINAPI *GetState)(MON_IMMDevice*,WL_DWORD*);
} MON_IMMDeviceVtbl;
struct MON_IMMDevice { MON_IMMDeviceVtbl* lpVtbl; };
typedef struct {
 MON_HRESULT (WL_WINAPI *QueryInterface)(MON_IPropertyStore*,const MON_GUID*,void**);
 WL_U32 (WL_WINAPI *AddRef)(MON_IPropertyStore*); WL_U32 (WL_WINAPI *Release)(MON_IPropertyStore*);
 MON_HRESULT (WL_WINAPI *GetCount)(MON_IPropertyStore*,WL_DWORD*);
 MON_HRESULT (WL_WINAPI *GetAt)(MON_IPropertyStore*,WL_DWORD,MON_PROPERTYKEY*);
 MON_HRESULT (WL_WINAPI *GetValue)(MON_IPropertyStore*,const MON_PROPERTYKEY*,MON_PROPVARIANT*);
 void* SetValue; void* Commit;
} MON_IPropertyStoreVtbl;
struct MON_IPropertyStore { MON_IPropertyStoreVtbl* lpVtbl; };
typedef struct {
 MON_HRESULT (WL_WINAPI *QueryInterface)(MON_IAudioClient*,const MON_GUID*,void**);
 WL_U32 (WL_WINAPI *AddRef)(MON_IAudioClient*); WL_U32 (WL_WINAPI *Release)(MON_IAudioClient*);
 MON_HRESULT (WL_WINAPI *Initialize)(MON_IAudioClient*,int,WL_DWORD,WL_I64,WL_I64,const MON_WAVEFORMATEX*,const MON_GUID*);
 MON_HRESULT (WL_WINAPI *GetBufferSize)(MON_IAudioClient*,WL_UINT*);
 MON_HRESULT (WL_WINAPI *GetStreamLatency)(MON_IAudioClient*,WL_I64*);
 MON_HRESULT (WL_WINAPI *GetCurrentPadding)(MON_IAudioClient*,WL_UINT*);
 MON_HRESULT (WL_WINAPI *IsFormatSupported)(MON_IAudioClient*,int,const MON_WAVEFORMATEX*,MON_WAVEFORMATEX**);
 MON_HRESULT (WL_WINAPI *GetMixFormat)(MON_IAudioClient*,MON_WAVEFORMATEX**);
 MON_HRESULT (WL_WINAPI *GetDevicePeriod)(MON_IAudioClient*,WL_I64*,WL_I64*);
 MON_HRESULT (WL_WINAPI *Start)(MON_IAudioClient*);
 MON_HRESULT (WL_WINAPI *Stop)(MON_IAudioClient*);
 MON_HRESULT (WL_WINAPI *Reset)(MON_IAudioClient*);
 MON_HRESULT (WL_WINAPI *SetEventHandle)(MON_IAudioClient*,WL_HANDLE);
 MON_HRESULT (WL_WINAPI *GetService)(MON_IAudioClient*,const MON_GUID*,void**);
} MON_IAudioClientVtbl;
struct MON_IAudioClient { MON_IAudioClientVtbl* lpVtbl; };
typedef struct {
 MON_HRESULT (WL_WINAPI *QueryInterface)(MON_IAudioRenderClient*,const MON_GUID*,void**);
 WL_U32 (WL_WINAPI *AddRef)(MON_IAudioRenderClient*); WL_U32 (WL_WINAPI *Release)(MON_IAudioRenderClient*);
 MON_HRESULT (WL_WINAPI *GetBuffer)(MON_IAudioRenderClient*,WL_UINT,WL_U8**);
 MON_HRESULT (WL_WINAPI *ReleaseBuffer)(MON_IAudioRenderClient*,WL_UINT,WL_DWORD);
} MON_IAudioRenderClientVtbl;
struct MON_IAudioRenderClient { MON_IAudioRenderClientVtbl* lpVtbl; };

typedef MON_HRESULT (WL_WINAPI *PFN_MonCoInitializeEx)(void*,WL_DWORD);
typedef void (WL_WINAPI *PFN_MonCoUninitialize)(void);
typedef MON_HRESULT (WL_WINAPI *PFN_CoCreateInstance)(const MON_GUID*,void*,WL_DWORD,const MON_GUID*,void**);
typedef void (WL_WINAPI *PFN_CoTaskMemFree)(void*);
typedef MON_HRESULT (WL_WINAPI *PFN_PropVariantClear)(MON_PROPVARIANT*);
typedef WL_HANDLE (WL_WINAPI *PFN_CreateEventW)(void*,WL_BOOL,WL_BOOL,const WL_WCHAR*);
typedef WL_BOOL (WL_WINAPI *PFN_SetEvent)(WL_HANDLE);
typedef WL_BOOL (WL_WINAPI *PFN_MoveWindow)(WL_HWND,WL_I32,WL_I32,WL_I32,WL_I32,WL_BOOL);
typedef WL_BOOL (WL_WINAPI *PFN_GetClientRect)(WL_HWND,WL_RECT*);
typedef WL_BOOL (WL_WINAPI *PFN_IsIconic)(WL_HWND);
typedef WL_U32 (WL_WINAPI *PFN_GetDpiForWindow)(WL_HWND);
typedef WL_U32 (WL_WINAPI *PFN_GetDpiForSystem)(void);
typedef WL_BOOL (WL_WINAPI *PFN_SetProcessDPIAware)(void);
typedef WL_BOOL (WL_WINAPI *PFN_SetProcessDpiAwarenessContext)(void*);
typedef WL_HANDLE (WL_WINAPI *PFN_AvSetMmThreadCharacteristicsW)(const WL_WCHAR*,WL_DWORD*);
typedef WL_BOOL (WL_WINAPI *PFN_AvSetMmThreadPriority)(WL_HANDLE,int);
typedef WL_BOOL (WL_WINAPI *PFN_AvRevertMmThreadCharacteristics)(WL_HANDLE);
typedef void* (WL_WINAPI *PFN_CreateMenu)(void);
typedef void* (WL_WINAPI *PFN_CreatePopupMenu)(void);
typedef WL_BOOL (WL_WINAPI *PFN_AppendMenuW)(void*,WL_UINT,WL_UPTR,const WL_WCHAR*);
typedef WL_BOOL (WL_WINAPI *PFN_SetMenu)(WL_HWND,void*);
typedef WL_UINT (WL_WINAPI *PFN_TrackPopupMenu)(void*,WL_UINT,WL_I32,WL_I32,WL_I32,WL_HWND,const WL_RECT*);
typedef WL_BOOL (WL_WINAPI *PFN_DestroyMenu)(void*);
typedef WL_BOOL (WL_WINAPI *PFN_DrawMenuBar)(WL_HWND);
typedef WL_BOOL (WL_WINAPI *PFN_TextOutW)(WL_HDC,WL_I32,WL_I32,const WL_WCHAR*,WL_I32);
typedef WL_DWORD (WL_WINAPI *PFN_SetTextColor)(WL_HDC,WL_DWORD);
typedef WL_I32 (WL_WINAPI *PFN_SetBkMode)(WL_HDC,WL_I32);
typedef struct { WL_DWORD dwSize,dwICC; } MON_INITCOMMONCONTROLSEX;
typedef WL_BOOL (WL_WINAPI *PFN_InitCommonControlsEx)(const MON_INITCOMMONCONTROLSEX*);
typedef struct { WL_HWND hwndFrom; WL_UPTR idFrom; WL_UINT code; } MON_NMHDR;
typedef struct { WL_UINT mask; WL_DWORD dwState,dwStateMask; WL_WCHAR* pszText; WL_I32 cchTextMax,iImage; WL_LPARAM lParam; } MON_TCITEMW;
typedef struct { WL_I32 x,y; } MON_POINT;
typedef struct { MON_POINT ptReserved,ptMaxSize,ptMaxPosition,ptMinTrackSize,ptMaxTrackSize; } MON_MINMAXINFO;

static void* g_monitorOle32=0,*g_avrt=0;
static PFN_MonCoInitializeEx pMonCoInitializeEx=0; static PFN_MonCoUninitialize pMonCoUninitialize=0;
static PFN_CoCreateInstance pMonCoCreateInstance=0; static PFN_CoTaskMemFree pMonCoTaskMemFree=0; static PFN_PropVariantClear pMonPropVariantClear=0;
static PFN_CreateEventW pCreateEventW=0; static PFN_SetEvent pSetEvent=0;
static PFN_AvSetMmThreadCharacteristicsW pAvSetMmThreadCharacteristicsW=0; static PFN_AvSetMmThreadPriority pAvSetMmThreadPriority=0; static PFN_AvRevertMmThreadCharacteristics pAvRevertMmThreadCharacteristics=0;
static PFN_MoveWindow pMoveWindow=0; static PFN_GetClientRect pGetClientRect=0; static PFN_IsIconic pIsIconic=0; static PFN_GetDpiForWindow pGetDpiForWindow=0; static PFN_GetDpiForSystem pGetDpiForSystem=0; static PFN_SetProcessDPIAware pSetProcessDPIAware=0; static PFN_SetProcessDpiAwarenessContext pSetProcessDpiAwarenessContext=0;
static PFN_CreateMenu pCreateMenu=0; static PFN_CreatePopupMenu pCreatePopupMenu=0; static PFN_AppendMenuW pAppendMenuW=0; static PFN_SetMenu pSetMenu=0; static PFN_TrackPopupMenu pTrackPopupMenu=0; static PFN_DestroyMenu pDestroyMenu=0; static PFN_DrawMenuBar pDrawMenuBar=0;
static PFN_TextOutW pTextOutW=0; static PFN_SetTextColor pSetTextColor=0; static PFN_SetBkMode pSetBkMode=0; static PFN_InitCommonControlsEx pInitCommonControlsEx=0; static void* g_comctl32=0;

static PFN_SetTimer pSetTimer=0; static PFN_KillTimer pKillTimer=0; static PFN_GetCurrentProcess pGetCurrentProcess=0; static PFN_GetCurrentProcessId pGetCurrentProcessId=0;
static PFN_GetProcessHandleCount pGetProcessHandleCount=0; static PFN_GetProcessTimes pGetProcessTimes=0; static PFN_GetTickCount64 pGetTickCount64=0; static PFN_GetActiveProcessorCount pGetActiveProcessorCount=0;
static PFN_K32GetProcessMemoryInfo pK32GetProcessMemoryInfo=0; static PFN_GetGuiResources pGetGuiResources=0; static PFN_CreateToolhelp32Snapshot pCreateToolhelp32Snapshot=0; static PFN_Thread32First pThread32First=0; static PFN_Thread32Next pThread32Next=0;
static PFN_CopyFileW pCopyFileW=0; static PFN_MoveFileExW pMoveFileExW=0; static PFN_DeleteFileW pDeleteFileW=0; static PFN_FlushFileBuffers pFlushFileBuffers=0;
static PFN_CreateCompatibleDC pCreateCompatibleDC=0; static PFN_DeleteDC pDeleteDC=0; static PFN_CreateCompatibleBitmap pCreateCompatibleBitmap=0; static PFN_BitBlt pBitBlt=0;
static void init_optional_apis(void){
 pSetTimer=(PFN_SetTimer)wl_get_proc(g_api.user32,"SetTimer"); pKillTimer=(PFN_KillTimer)wl_get_proc(g_api.user32,"KillTimer"); pGetGuiResources=(PFN_GetGuiResources)wl_get_proc(g_api.user32,"GetGuiResources");
 pGetCurrentProcess=(PFN_GetCurrentProcess)wl_get_proc(g_api.kernel32,"GetCurrentProcess"); pGetCurrentProcessId=(PFN_GetCurrentProcessId)wl_get_proc(g_api.kernel32,"GetCurrentProcessId"); pGetProcessHandleCount=(PFN_GetProcessHandleCount)wl_get_proc(g_api.kernel32,"GetProcessHandleCount");
 pGetProcessTimes=(PFN_GetProcessTimes)wl_get_proc(g_api.kernel32,"GetProcessTimes"); pGetTickCount64=(PFN_GetTickCount64)wl_get_proc(g_api.kernel32,"GetTickCount64"); pGetActiveProcessorCount=(PFN_GetActiveProcessorCount)wl_get_proc(g_api.kernel32,"GetActiveProcessorCount");
 pK32GetProcessMemoryInfo=(PFN_K32GetProcessMemoryInfo)wl_get_proc(g_api.kernel32,"K32GetProcessMemoryInfo"); pCreateToolhelp32Snapshot=(PFN_CreateToolhelp32Snapshot)wl_get_proc(g_api.kernel32,"CreateToolhelp32Snapshot"); pThread32First=(PFN_Thread32First)wl_get_proc(g_api.kernel32,"Thread32First"); pThread32Next=(PFN_Thread32Next)wl_get_proc(g_api.kernel32,"Thread32Next");
 pCopyFileW=(PFN_CopyFileW)wl_get_proc(g_api.kernel32,"CopyFileW"); pMoveFileExW=(PFN_MoveFileExW)wl_get_proc(g_api.kernel32,"MoveFileExW"); pDeleteFileW=(PFN_DeleteFileW)wl_get_proc(g_api.kernel32,"DeleteFileW"); pFlushFileBuffers=(PFN_FlushFileBuffers)wl_get_proc(g_api.kernel32,"FlushFileBuffers");
 pCreateCompatibleDC=(PFN_CreateCompatibleDC)wl_get_proc(g_api.gdi32,"CreateCompatibleDC"); pDeleteDC=(PFN_DeleteDC)wl_get_proc(g_api.gdi32,"DeleteDC"); pCreateCompatibleBitmap=(PFN_CreateCompatibleBitmap)wl_get_proc(g_api.gdi32,"CreateCompatibleBitmap"); pBitBlt=(PFN_BitBlt)wl_get_proc(g_api.gdi32,"BitBlt");
 pCreateEventW=(PFN_CreateEventW)wl_get_proc(g_api.kernel32,"CreateEventW");pSetEvent=(PFN_SetEvent)wl_get_proc(g_api.kernel32,"SetEvent");
 pMoveWindow=(PFN_MoveWindow)wl_get_proc(g_api.user32,"MoveWindow");pGetClientRect=(PFN_GetClientRect)wl_get_proc(g_api.user32,"GetClientRect");pIsIconic=(PFN_IsIconic)wl_get_proc(g_api.user32,"IsIconic");
 pGetDpiForWindow=(PFN_GetDpiForWindow)wl_get_proc(g_api.user32,"GetDpiForWindow");pGetDpiForSystem=(PFN_GetDpiForSystem)wl_get_proc(g_api.user32,"GetDpiForSystem");pSetProcessDPIAware=(PFN_SetProcessDPIAware)wl_get_proc(g_api.user32,"SetProcessDPIAware");pSetProcessDpiAwarenessContext=(PFN_SetProcessDpiAwarenessContext)wl_get_proc(g_api.user32,"SetProcessDpiAwarenessContext");
 pCreateMenu=(PFN_CreateMenu)wl_get_proc(g_api.user32,"CreateMenu");pCreatePopupMenu=(PFN_CreatePopupMenu)wl_get_proc(g_api.user32,"CreatePopupMenu");pAppendMenuW=(PFN_AppendMenuW)wl_get_proc(g_api.user32,"AppendMenuW");pSetMenu=(PFN_SetMenu)wl_get_proc(g_api.user32,"SetMenu");pTrackPopupMenu=(PFN_TrackPopupMenu)wl_get_proc(g_api.user32,"TrackPopupMenu");pDestroyMenu=(PFN_DestroyMenu)wl_get_proc(g_api.user32,"DestroyMenu");pDrawMenuBar=(PFN_DrawMenuBar)wl_get_proc(g_api.user32,"DrawMenuBar");
 pTextOutW=(PFN_TextOutW)wl_get_proc(g_api.gdi32,"TextOutW");pSetTextColor=(PFN_SetTextColor)wl_get_proc(g_api.gdi32,"SetTextColor");pSetBkMode=(PFN_SetBkMode)wl_get_proc(g_api.gdi32,"SetBkMode");
 if(!g_comctl32){static const WL_WCHAR cc[]={'c','o','m','c','t','l','3','2','.','d','l','l',0};g_comctl32=g_api.LoadLibraryW(cc);}if(g_comctl32)pInitCommonControlsEx=(PFN_InitCommonControlsEx)wl_get_proc(g_comctl32,"InitCommonControlsEx");
 if(pGetTickCount64)g_diagStartTick=pGetTickCount64();
}

/* Helpers. */
static void wadd_ascii(WL_WCHAR*d,WL_SIZE_T cap,const char*s){WL_SIZE_T n=wl_wlen(d),i=0;while(n+1<cap&&s&&s[i])d[n++]=(WL_U8)s[i++];d[n]=0;}
static void wadd_num(WL_WCHAR*d,WL_SIZE_T cap,int v){WL_WCHAR t[16];int n=0;if(v==0)t[n++]='0';else{if(v<0){wl_wcat(d,(const WL_WCHAR[]){'-',0},cap);v=-v;}while(v&&n<15){t[n++]=(WL_WCHAR)('0'+v%10);v/=10;}}while(n--&&wl_wlen(d)+1<cap){WL_SIZE_T q=wl_wlen(d);d[q]=t[n];d[q+1]=0;}}
static void wset_ascii(WL_WCHAR*d,WL_SIZE_T cap,const char*s){if(cap)d[0]=0;wadd_ascii(d,cap,s);}
static const WL_WCHAR* base_name(const WL_WCHAR* p){const WL_WCHAR*b=p;if(!p)return p;for(const WL_WCHAR*s=p;*s;s++)if(*s=='\\'||*s=='/')b=s+1;return b;}
static void spin_lock(void){while(1){WL_I32 expect=0;if(__atomic_compare_exchange_n(&g_padLock,&expect,1,0,__ATOMIC_ACQ_REL,__ATOMIC_ACQUIRE))return;g_api.Sleep(0);}}
static void spin_unlock(void){__atomic_store_n(&g_padLock,0,__ATOMIC_RELEASE);}
static void decode_lock(void){while(1){WL_I32 expect=0;if(__atomic_compare_exchange_n(&g_decodeLock,&expect,1,0,__ATOMIC_ACQ_REL,__ATOMIC_ACQUIRE))return;g_api.Sleep(1);}}
static void decode_unlock(void){__atomic_store_n(&g_decodeLock,0,__ATOMIC_RELEASE);}
static float fclamp(float x,float a,float b){return x<a?a:(x>b?b:x);}
static WL_U16 rd16(const WL_U8*p){return (WL_U16)(p[0]|((WL_U16)p[1]<<8));}
static WL_U32 rd32(const WL_U8*p){return (WL_U32)p[0]|((WL_U32)p[1]<<8)|((WL_U32)p[2]<<16)|((WL_U32)p[3]<<24);}
static WL_I32 rd24s(const WL_U8*p){WL_I32 v=(WL_I32)((WL_U32)p[0]|((WL_U32)p[1]<<8)|((WL_U32)p[2]<<16));if(v&0x00800000)v|=(WL_I32)0xff000000;return v;}
static int fourcc(const WL_U8*p,const char*s){return p[0]==(WL_U8)s[0]&&p[1]==(WL_U8)s[1]&&p[2]==(WL_U8)s[2]&&p[3]==(WL_U8)s[3];}
static int weq(const WL_WCHAR*a,const WL_WCHAR*b){if(!a||!b)return a==b;while(*a&&*b&&*a==*b){a++;b++;}return *a==*b;}
static __attribute__((noinline)) void set_text_if_changed(WL_HWND h,const WL_WCHAR*t){if(!h||!t)return;WL_WCHAR old[1600];old[0]=0;g_api.GetWindowTextW(h,old,1600);if(!weq(old,t))g_api.SetWindowTextW(h,t);}
static WL_WCHAR wlower(WL_WCHAR c){return c>='A'&&c<='Z'?(WL_WCHAR)(c+32):c;}
static int wends_ascii_ci(const WL_WCHAR*path,const char*ext){WL_SIZE_T n=wl_wlen(path),m=wl_alen(ext);if(n<m)return 0;for(WL_SIZE_T i=0;i<m;i++){WL_WCHAR a=wlower(path[n-m+i]);char b=wl_lowera(ext[i]);if(a!=(WL_U8)b)return 0;}return 1;}
static float sanitize_audio_sample(float x){union{float f;WL_U32 u;}v;v.f=x;if((v.u&0x7f800000u)==0x7f800000u)return 0.0f;if(x>8.0f)return 8.0f;if(x<-8.0f)return -8.0f;return x;}
static int acquire_decode_slot(void){while(g_running){WL_I32 cur=__atomic_load_n(&g_decodeSlots,__ATOMIC_ACQUIRE);if(cur<MAX_DECODE_WORKERS){WL_I32 next=cur+1;if(__atomic_compare_exchange_n(&g_decodeSlots,&cur,next,1,__ATOMIC_ACQ_REL,__ATOMIC_ACQUIRE))return 1;}g_api.Sleep(5);}return 0;}
static void release_decode_slot(void){WL_I32 n=__atomic_sub_fetch(&g_decodeSlots,1,__ATOMIC_ACQ_REL);if(n<0)__atomic_store_n(&g_decodeSlots,0,__ATOMIC_RELEASE);}

static void wadd_u64(WL_WCHAR*d,WL_SIZE_T cap,WL_U64 v){WL_WCHAR t[32];int n=0;if(!v)t[n++]='0';else while(v&&n<31){t[n++]=(WL_WCHAR)('0'+(v%10));v/=10;}while(n--&&wl_wlen(d)+1<cap){WL_SIZE_T q=wl_wlen(d);d[q]=t[n];d[q+1]=0;}}
static void wadd_2(WL_WCHAR*d,WL_SIZE_T cap,int v){if(v<10)wadd_ascii(d,cap,"0");wadd_num(d,cap,v);}
static void wadd_duration10(WL_WCHAR*d,WL_SIZE_T cap,WL_U64 tenths){WL_U64 secs=tenths/10ull;wadd_u64(d,cap,secs/60ull);wadd_ascii(d,cap,":");wadd_2(d,cap,(int)(secs%60ull));wadd_ascii(d,cap,".");wadd_num(d,cap,(int)(tenths%10ull));}
static void wadd_half_units(WL_WCHAR*d,WL_SIZE_T cap,WL_I32 half,int forcePlus){if(half<0){wadd_ascii(d,cap,"-");half=-half;}else if(forcePlus)wadd_ascii(d,cap,"+");wadd_num(d,cap,half/2);wadd_ascii(d,cap,(half&1)?".5":".0");}
static WL_U32 crc32_bytes(const WL_U8*p,WL_SIZE_T n){WL_U32 c=0xffffffffu;while(n--){c^=*p++;for(int k=0;k<8;k++)c=(c>>1)^((c&1)?0xedb88320u:0u);}return c^0xffffffffu;}

/* Sample storage + decoding. */
typedef struct Sample {
 float* mono;
 WL_U64 frames;
 WL_U32 sampleRate;
 volatile WL_I32 refs;
 float peaks[WAVE_PEAKS];
 struct Sample* nextRetired;
} Sample;

typedef struct {
 WL_WCHAR path[MAX_PATH_W];
 WL_WCHAR name[32];
 Sample* sample;
 float volume;
 volatile WL_I32 volumeHalfPct; /* 0..400 = 0.0..200.0% */
 volatile WL_I32 pitchHalf; /* half-semitone units: -24..+24 (-12..+12 st) */
 float selStart,selEnd;
 WL_U32 vk,mods;
 volatile WL_I32 loadState;
} Pad;
static Pad g_pads[PAD_COUNT];
static Sample* volatile g_retiredSamples=0;

static double pitch_ratio_half(WL_I32 halfSteps){static const double t[49]={0.500000000000000,0.514651118321746,0.529731547179648,0.545253866332629,0.561231024154687,0.577676348436136,0.594603557501361,0.612026771652328,0.629960524947437,0.648419777325505,0.667419927085017,0.686976823729045,0.707106781186548,0.727826591421094,0.749153538438341,0.771105412703970,0.793700525984100,0.816957726620550,0.840896415253715,0.865536561006143,0.890898718140339,0.917004043204671,0.943874312681694,0.971531941153606,1.000000000000000,1.029302236643492,1.059463094359295,1.090507732665258,1.122462048309373,1.155352696872273,1.189207115002721,1.224053543304655,1.259921049894873,1.296839554651010,1.334839854170034,1.373953647458089,1.414213562373095,1.455653182842187,1.498307076876682,1.542210825407941,1.587401051968199,1.633915453241100,1.681792830507429,1.731073122012286,1.781797436280679,1.834008086409342,1.887748625363387,1.943063882307212,2.000000000000000};if(halfSteps<-24)halfSteps=-24;if(halfSteps>24)halfSteps=24;return t[halfSteps+24];}

typedef struct { WL_U32 magic,version; } ConfigHeader;
typedef struct { WL_WCHAR path[MAX_PATH_W]; WL_U32 vk,mods; float volume; } PadDiskV1;
typedef struct { WL_WCHAR path[MAX_PATH_W]; WL_U32 vk,mods; float volume; WL_I32 pitch; float selStart,selEnd; } PadDiskV2;
typedef struct { PadDiskV2 pads[PAD_COUNT]; WL_WCHAR names[PAD_COUNT][32]; WL_U32 stopVk,stopMods; } ConfigV5Data;
typedef struct { PadDiskV2 pads[PAD_COUNT]; WL_WCHAR names[PAD_COUNT][32]; WL_U32 stopVk,stopMods,muteVk,muteMods; } ConfigV6Data;
typedef struct { PadDiskV2 pads[PAD_COUNT]; WL_WCHAR names[PAD_COUNT][32]; WL_U32 stopVk,stopMods,muteVk,muteMods; WL_I32 monitorVolumeHalfPct; WL_WCHAR monitorDeviceName[32]; } ConfigV7Data;
typedef struct { PadDiskV2 pads[PAD_COUNT]; WL_WCHAR names[PAD_COUNT][32]; WL_U32 stopVk,stopMods,muteVk,muteMods; WL_I32 monitorVolumeHalfPct; WL_WCHAR monitorDeviceName[MONITOR_DEVICE_NAME_CHARS]; WL_WCHAR monitorDeviceId[MONITOR_DEVICE_ID_CHARS]; } ConfigV8Data;
typedef struct { ConfigV8Data base; WL_I32 windowW,windowH,lastTab; } ConfigV9Data;
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
static int cfg_write_file(const WL_WCHAR*path){
 WL_SIZE_T bytes=sizeof(ConfigV9Data);ConfigV9Data*d=(ConfigV9Data*)g_api.HeapAlloc(g_api.GetProcessHeap(),0,bytes);if(!d)return 0;memset(d,0,bytes);
 for(int i=0;i<PAD_COUNT;i++){spin_lock();wl_wcpy(d->base.pads[i].path,g_pads[i].path,MAX_PATH_W);d->base.pads[i].vk=g_pads[i].vk;d->base.pads[i].mods=g_pads[i].mods;d->base.pads[i].volume=g_pads[i].volume;d->base.pads[i].pitch=__atomic_load_n(&g_pads[i].pitchHalf,__ATOMIC_RELAXED);d->base.pads[i].selStart=g_pads[i].selStart;d->base.pads[i].selEnd=g_pads[i].selEnd;wl_wcpy(d->base.names[i],g_pads[i].name,32);spin_unlock();}
 d->base.stopVk=g_stopVk;d->base.stopMods=g_stopMods;d->base.muteVk=g_muteVk;d->base.muteMods=g_muteMods;d->base.monitorVolumeHalfPct=__atomic_load_n(&g_monitorVolumeHalfPct,__ATOMIC_RELAXED);wl_wcpy(d->base.monitorDeviceName,g_monitorDeviceName,MONITOR_DEVICE_NAME_CHARS);wl_wcpy(d->base.monitorDeviceId,g_monitorDeviceId,MONITOR_DEVICE_ID_CHARS);
 d->windowW=g_clientW;d->windowH=g_clientH;d->lastTab=g_activeTab;
 ConfigHeader ch={CFG_MAGIC,9};WL_U32 crc=crc32_bytes((const WL_U8*)d,bytes);WL_HANDLE h=g_api.CreateFileW(path,GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);if(h==WL_INVALID_HANDLE_VALUE){g_api.HeapFree(g_api.GetProcessHeap(),0,d);return 0;}int ok=cfg_write_exact(h,&ch,sizeof(ch))&&cfg_write_exact(h,d,(WL_DWORD)bytes)&&cfg_write_exact(h,&crc,sizeof(crc));if(ok&&pFlushFileBuffers)ok=pFlushFileBuffers(h)?1:0;g_api.CloseHandle(h);g_api.HeapFree(g_api.GetProcessHeap(),0,d);return ok;
}
static void save_config(void){WL_WCHAR path[600],tmp[610],bak[610];if(!config_path(path,600,0))return;wl_wcpy(tmp,path,610);wl_wcat(tmp,CFG_TMP_EXT,610);wl_wcpy(bak,path,610);wl_wcat(bak,CFG_BAK_EXT,610);if(!cfg_write_file(tmp)){g_backupState=3;if(pDeleteFileW)pDeleteFileW(tmp);return;}if(cfg_file_exists(path)&&!g_loadedFromBackup&&pCopyFileW)pCopyFileW(path,bak,WL_FALSE);int replaced=0;if(pMoveFileExW)replaced=pMoveFileExW(tmp,path,MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)?1:0;if(!replaced){if(pDeleteFileW)pDeleteFileW(tmp);g_backupState=3;return;}g_loadedFromBackup=0;if(!cfg_file_exists(bak)&&pCopyFileW)pCopyFileW(path,bak,WL_FALSE);g_backupState=cfg_file_exists(bak)?1:0;g_configLoadedVersion=9;}


static void compute_peaks(const float*mono,WL_U64 frames,float*out){for(int i=0;i<WAVE_PEAKS;i++)out[i]=0;if(!mono||!frames)return;for(WL_U64 f=0;f<frames;f++){WL_U64 bi=(f*(WL_U64)WAVE_PEAKS)/frames;if(bi>=WAVE_PEAKS)bi=WAVE_PEAKS-1;float v=mono[f];if(v<0)v=-v;if(v>out[bi])out[bi]=v;}for(int i=0;i<WAVE_PEAKS;i++)if(out[i]>1.0f)out[i]=1.0f;}
static void sample_retain(Sample*s){if(s)__atomic_add_fetch(&s->refs,1,__ATOMIC_ACQ_REL);}
static void sample_retire(Sample*s){if(!s)return;Sample*head;do{head=__atomic_load_n(&g_retiredSamples,__ATOMIC_ACQUIRE);s->nextRetired=head;}while(!__atomic_compare_exchange_n(&g_retiredSamples,&head,s,1,__ATOMIC_ACQ_REL,__ATOMIC_ACQUIRE));}
static void sample_release(Sample*s){if(!s)return;if(__atomic_sub_fetch(&s->refs,1,__ATOMIC_ACQ_REL)==0)sample_retire(s);}
static void drain_retired_samples(void){Sample*s=__atomic_exchange_n(&g_retiredSamples,0,__ATOMIC_ACQ_REL);while(s){Sample*n=s->nextRetired;WL_I64 bytes=(WL_I64)(s->frames*sizeof(float));if(s->mono)g_api.HeapFree(g_api.GetProcessHeap(),0,s->mono);g_api.HeapFree(g_api.GetProcessHeap(),0,s);__atomic_sub_fetch(&g_totalSampleBytes,bytes,__ATOMIC_ACQ_REL);s=n;}}
static int reserve_sample_bytes(WL_U64 bytes){WL_I64 cur=__atomic_load_n(&g_totalSampleBytes,__ATOMIC_ACQUIRE);for(;;){WL_U64 used=(WL_U64)(cur<0?0:cur);if(used+bytes>MAX_TOTAL_SAMPLE_BYTES)return 0;WL_I64 next=cur+(WL_I64)bytes;if(__atomic_compare_exchange_n(&g_totalSampleBytes,&cur,next,1,__ATOMIC_ACQ_REL,__ATOMIC_ACQUIRE))return 1;}}
static Sample* sample_create(float*mono,WL_U64 frames,WL_U32 sr,const float*peaks){if(!mono||!frames||!sr)return 0;WL_U64 bytes=frames*sizeof(float);if(bytes>MAX_PAD_SAMPLE_BYTES||!reserve_sample_bytes(bytes)){g_api.HeapFree(g_api.GetProcessHeap(),0,mono);return 0;}Sample*s=(Sample*)g_api.HeapAlloc(g_api.GetProcessHeap(),0,sizeof(Sample));if(!s){__atomic_sub_fetch(&g_totalSampleBytes,(WL_I64)bytes,__ATOMIC_ACQ_REL);g_api.HeapFree(g_api.GetProcessHeap(),0,mono);return 0;}memset(s,0,sizeof(*s));s->mono=mono;s->frames=frames;s->sampleRate=sr;s->refs=1;if(peaks)memcpy(s->peaks,peaks,sizeof(s->peaks));else compute_peaks(mono,frames,s->peaks);return s;}
static int commit_sample_ex(int idx,const WL_WCHAR*path,float*mono,WL_U64 frames,WL_U32 sr,int preserveRange,Sample*expectedCurrent,const float*peaks){Sample*ns=sample_create(mono,frames,sr,peaks);if(!ns)return 0;Sample*old=0;spin_lock();if(expectedCurrent&&g_pads[idx].sample!=expectedCurrent){spin_unlock();sample_release(ns);return 0;}old=g_pads[idx].sample;g_pads[idx].sample=ns;g_pads[idx].loadState=PAD_READY;if(path)wl_wcpy(g_pads[idx].path,path,MAX_PATH_W);if(!preserveRange){g_pads[idx].selStart=0.0f;g_pads[idx].selEnd=1.0f;}spin_unlock();sample_release(old);return 1;}
static float cubic_array_sample(const float*s,WL_U64 frames,double pos){if(!s||!frames)return 0;if(pos<0)pos=0;if(pos>(double)(frames-1))pos=(double)(frames-1);WL_U64 i1=(WL_U64)pos,i0=i1?i1-1:i1,i2=i1+1<frames?i1+1:i1,i3=i2+1<frames?i2+1:i2;float t=(float)(pos-(double)i1),t2=t*t,t3=t2*t;float p0=s[i0],p1=s[i1],p2=s[i2],p3=s[i3];return 0.5f*((2.0f*p1)+(-p0+p2)*t+(2.0f*p0-5.0f*p1+4.0f*p2-p3)*t2+(-p0+3.0f*p1-3.0f*p2+p3)*t3);}
static int resample_buffer(float*src,WL_U64 frames,WL_U32 srcRate,WL_U32 dstRate,float**out,WL_U64*outFrames,float*peaks){*out=0;*outFrames=0;if(peaks)for(int i=0;i<WAVE_PEAKS;i++)peaks[i]=0;if(!src||!frames||!srcRate||!dstRate)return 0;if(srcRate==dstRate){*out=src;*outFrames=frames;if(peaks)compute_peaks(src,frames,peaks);return 2;}WL_U64 nf=(frames*(WL_U64)dstRate+(srcRate/2u))/(WL_U64)srcRate;if(!nf||nf>MAX_PAD_FRAMES||nf*sizeof(float)>MAX_PAD_SAMPLE_BYTES)return 0;float*d=(float*)g_api.HeapAlloc(g_api.GetProcessHeap(),0,(WL_SIZE_T)nf*sizeof(float));if(!d)return 0;double step=(double)srcRate/(double)dstRate;for(WL_U64 i=0;i<nf;i++){float v=sanitize_audio_sample(cubic_array_sample(src,frames,(double)i*step));d[i]=v;if(peaks){WL_U64 bi=(i*(WL_U64)WAVE_PEAKS)/nf;if(bi>=WAVE_PEAKS)bi=WAVE_PEAKS-1;float a=v<0?-v:v;if(a>peaks[bi])peaks[bi]=a;}}if(peaks)for(int i=0;i<WAVE_PEAKS;i++)if(peaks[i]>1.0f)peaks[i]=1.0f;*out=d;*outFrames=nf;return 1;}
static int path_exists(const WL_WCHAR*path){WL_HANDLE h=g_api.CreateFileW(path,GENERIC_READ,1,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);if(h==WL_INVALID_HANDLE_VALUE)return 0;g_api.CloseHandle(h);return 1;}
static int audio_file_preflight(const WL_WCHAR*path){
 WL_HANDLE h=g_api.CreateFileW(path,GENERIC_READ,1,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);if(h==WL_INVALID_HANDLE_VALUE)return 0;
 WL_I64 sz=0;if(!g_api.GetFileSizeEx(h,&sz)||sz<=0||sz>(WL_I64)(1024ull*1024ull*1024ull)){g_api.CloseHandle(h);return 0;}
 if(wends_ascii_ci(path,".ogg")||wends_ascii_ci(path,".oga")||wends_ascii_ci(path,".opus")){WL_U8 b[4];WL_DWORD n=0;if(!g_api.ReadFile(h,b,4,&n,0)||n!=4||!fourcc(b,"OggS")){g_api.CloseHandle(h);return 0;}}
 g_api.CloseHandle(h);return 1;
}

static int decode_wav(const WL_WCHAR*path,float**out,WL_U64*outFrames,WL_U32*outRate,float*peaks){*out=0;*outFrames=0;*outRate=0;if(peaks)for(int i=0;i<WAVE_PEAKS;i++)peaks[i]=0;WL_HANDLE h=g_api.CreateFileW(path,GENERIC_READ,1,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);if(h==WL_INVALID_HANDLE_VALUE)return 0;WL_I64 size=0;if(!g_api.GetFileSizeEx(h,&size)||size<44||size>(WL_I64)(512*1024*1024)){g_api.CloseHandle(h);return 0;}WL_U8*bytes=(WL_U8*)g_api.HeapAlloc(g_api.GetProcessHeap(),0,(WL_SIZE_T)size);if(!bytes){g_api.CloseHandle(h);return 0;}WL_DWORD total=0;while(total<(WL_U32)size){WL_DWORD n=0;if(!g_api.ReadFile(h,bytes+total,(WL_DWORD)size-total,&n,0)||!n)break;total+=n;}g_api.CloseHandle(h);if(total!=(WL_U32)size||!fourcc(bytes,"RIFF")||!fourcc(bytes+8,"WAVE")){g_api.HeapFree(g_api.GetProcessHeap(),0,bytes);return 0;}
 WL_U16 tag=0,ch=0,bits=0,block=0;WL_U32 sr=0;WL_U8*data=0;WL_U32 dataBytes=0;WL_U32 pos=12;while(pos+8<=(WL_U32)size){WL_U8*c=bytes+pos;WL_U32 cs=rd32(c+4);WL_U32 body=pos+8;if(body+cs>(WL_U32)size)break;if(fourcc(c,"fmt ")&&cs>=16){tag=rd16(bytes+body);ch=rd16(bytes+body+2);sr=rd32(bytes+body+4);block=rd16(bytes+body+12);bits=rd16(bytes+body+14);if(tag==0xfffe&&cs>=40)tag=rd16(bytes+body+24);}else if(fourcc(c,"data")){data=bytes+body;dataBytes=cs;}pos=body+cs+(cs&1u);}
 WL_U32 bps=(bits+7)/8;if(!data||!ch||!sr||!block||!bits||!(tag==1||tag==3)||!bps||block<(WL_U32)ch*bps||((tag==1)&&!(bits==8||bits==16||bits==24||bits==32))||((tag==3)&&bits!=32)){g_api.HeapFree(g_api.GetProcessHeap(),0,bytes);return 0;}WL_U64 frames=dataBytes/block;if(!frames||frames>MAX_PAD_FRAMES){g_api.HeapFree(g_api.GetProcessHeap(),0,bytes);return 0;}float*mono=(float*)g_api.HeapAlloc(g_api.GetProcessHeap(),0,(WL_SIZE_T)frames*sizeof(float));if(!mono){g_api.HeapFree(g_api.GetProcessHeap(),0,bytes);return 0;}
 for(WL_U64 f=0;f<frames;f++){float sum=0;int used=0;for(WL_U16 c=0;c<ch;c++){const WL_U8*q=data+f*block+c*bps;float v=0;if(tag==3&&bits==32){union{WL_U32 u;float f;}u;u.u=rd32(q);v=u.f;}else if(tag==1&&bits==8)v=(float)((int)q[0]-128)/128.0f;else if(tag==1&&bits==16){WL_I16 z=(WL_I16)rd16(q);v=(float)z/32768.0f;}else if(tag==1&&bits==24)v=(float)rd24s(q)/8388608.0f;else if(tag==1&&bits==32){WL_I32 z=(WL_I32)rd32(q);v=(float)z/2147483648.0f;}else{g_api.HeapFree(g_api.GetProcessHeap(),0,mono);g_api.HeapFree(g_api.GetProcessHeap(),0,bytes);return 0;}if(c<2){sum+=v;used++;}}float mv=sanitize_audio_sample(used?sum/(float)used:0);mono[f]=mv;if(peaks){WL_U64 bi=(f*(WL_U64)WAVE_PEAKS)/frames;if(bi>=WAVE_PEAKS)bi=WAVE_PEAKS-1;float av=mv<0?-mv:mv;if(av>peaks[bi])peaks[bi]=av;}}if(peaks)for(int i=0;i<WAVE_PEAKS;i++)if(peaks[i]>1.0f)peaks[i]=1.0f;
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
static void* g_mfplat=0,*g_mfrw=0,*g_ole32=0;static PFN_MFStartup g_MFStartup=0;static PFN_MFShutdown g_MFShutdown=0;static PFN_MFCreateMediaType g_MFCreateMediaType=0;static PFN_MFCreateSourceReaderFromURL g_MFCreateReader=0;static PFN_CoInitializeEx g_CoInitializeEx=0;static PFN_CoUninitialize g_CoUninitialize=0;static int g_mfState=0,g_mfStarted=0;
static const MF_GUID G_MT_MAJOR={0x48eba18e,0xf8c9,0x4687,{0xbf,0x11,0x0a,0x74,0xc9,0xf9,0x6a,0x8f}};
static const MF_GUID G_MT_SUBTYPE={0xf7e34c9a,0x42e8,0x4714,{0xb7,0x4b,0xcb,0x29,0xd7,0x2c,0x35,0xe5}};
static const MF_GUID G_MT_CH={0x37e48bf5,0x645e,0x4c5b,{0x89,0xde,0xad,0xa9,0xe2,0x9b,0x69,0x6a}};
static const MF_GUID G_MT_SR={0x5faeeae7,0x0290,0x4c31,{0x9e,0x8a,0xc5,0x34,0xf6,0x8d,0x9d,0xba}};
static const MF_GUID G_AUDIO={0x73647561,0x0000,0x0010,{0x80,0x00,0x00,0xaa,0x00,0x38,0x9b,0x71}};
static const MF_GUID G_FLOAT={0x00000003,0x0000,0x0010,{0x80,0x00,0x00,0xaa,0x00,0x38,0x9b,0x71}};
static const WL_WCHAR MFPLAT_DLL[]={'m','f','p','l','a','t','.','d','l','l',0};
static const WL_WCHAR MFRW_DLL[]={'m','f','r','e','a','d','w','r','i','t','e','.','d','l','l',0};
static const WL_WCHAR OLE32_DLL[]={'o','l','e','3','2','.','d','l','l',0};
static void mf_shutdown(void){if(g_mfStarted&&g_MFShutdown)g_MFShutdown();g_mfStarted=0;if(g_mfrw&&g_api.FreeLibrary)g_api.FreeLibrary((WL_HMODULE)g_mfrw);if(g_mfplat&&g_api.FreeLibrary)g_api.FreeLibrary((WL_HMODULE)g_mfplat);if(g_ole32&&g_api.FreeLibrary)g_api.FreeLibrary((WL_HMODULE)g_ole32);g_mfrw=g_mfplat=g_ole32=0;g_mfState=0;}
static int mf_init(void){if(g_mfState)return g_mfState>0;g_mfState=-1;g_mfplat=g_api.LoadLibraryW(MFPLAT_DLL);g_mfrw=g_api.LoadLibraryW(MFRW_DLL);g_ole32=g_api.LoadLibraryW(OLE32_DLL);if(!g_mfplat||!g_mfrw){mf_shutdown();g_mfState=-1;return 0;}g_MFStartup=(PFN_MFStartup)wl_get_proc(g_mfplat,"MFStartup");g_MFShutdown=(PFN_MFShutdown)wl_get_proc(g_mfplat,"MFShutdown");g_MFCreateMediaType=(PFN_MFCreateMediaType)wl_get_proc(g_mfplat,"MFCreateMediaType");g_MFCreateReader=(PFN_MFCreateSourceReaderFromURL)wl_get_proc(g_mfrw,"MFCreateSourceReaderFromURL");if(g_ole32){g_CoInitializeEx=(PFN_CoInitializeEx)wl_get_proc(g_ole32,"CoInitializeEx");g_CoUninitialize=(PFN_CoUninitialize)wl_get_proc(g_ole32,"CoUninitialize");}if(!g_MFStartup||!g_MFShutdown||!g_MFCreateMediaType||!g_MFCreateReader){mf_shutdown();g_mfState=-1;return 0;}if(g_MFStartup(0x00020070u,0)<0){mf_shutdown();g_mfState=-1;return 0;}g_mfStarted=1;g_mfState=1;return 1;}
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
 WL_U64 cap=262144,count=0;float*mono=(float*)g_api.HeapAlloc(g_api.GetProcessHeap(),0,(WL_SIZE_T)cap*sizeof(float));if(!mono){mf_release(r);return 0;}int ok=1;for(;;){WL_U32 flags=0;MF_OBJ*samp=0;WL_I32 hr=reader_read(r,0xfffffffdu,&flags,&samp);if(hr<0){ok=0;break;}if(samp){MF_OBJ*buf=0;if(sample_buffer(samp,&buf)<0||!buf){mf_release(samp);ok=0;break;}WL_U8*data=0;WL_U32 bytes=0;if(buffer_lock(buf,&data,&bytes)<0||!data){mf_release(buf);mf_release(samp);ok=0;break;}WL_U64 fr=bytes/((WL_U64)ch*4u);if(count+fr>MAX_PAD_FRAMES){buffer_unlock(buf);mf_release(buf);mf_release(samp);ok=0;break;}if(count+fr>cap){WL_U64 nc=cap;while(nc<count+fr)nc*=2;if(nc>MAX_PAD_FRAMES)nc=MAX_PAD_FRAMES;float*n=(float*)g_api.HeapAlloc(g_api.GetProcessHeap(),0,(WL_SIZE_T)nc*sizeof(float));if(!n){buffer_unlock(buf);mf_release(buf);mf_release(samp);ok=0;break;}memcpy(n,mono,(WL_SIZE_T)count*sizeof(float));g_api.HeapFree(g_api.GetProcessHeap(),0,mono);mono=n;cap=nc;}float*f=(float*)data;for(WL_U64 i=0;i<fr;i++){float sum=0;WL_U32 used=ch<2?ch:2;for(WL_U32 c=0;c<used;c++)sum+=f[i*ch+c];mono[count+i]=sanitize_audio_sample(sum/(float)used);}count+=fr;buffer_unlock(buf);mf_release(buf);mf_release(samp);}if(flags&0x2u)break;}mf_release(r);if(!ok||!count){g_api.HeapFree(g_api.GetProcessHeap(),0,mono);return 0;}*out=mono;*outFrames=count;*outRate=sr;return 1;}

typedef struct { int idx,preserveRange,hadSample; WL_U32 generation; WL_WCHAR path[MAX_PATH_W]; } LoadJob;
typedef struct { WL_U32 targetRate; } ResampleJob;
static int decode_audio_for_host(const WL_WCHAR*path,float**out,WL_U64*outFrames,WL_U32*outRate,float*peaks){
 *out=0;*outFrames=0;*outRate=0;if(peaks)for(int i=0;i<WAVE_PEAKS;i++)peaks[i]=0;
 if(!audio_file_preflight(path))return 0;
 float*mono=0;WL_U64 frames=0;WL_U32 sr=0;int havePeaks=0;
 if(decode_wav(path,&mono,&frames,&sr,peaks))havePeaks=1;else{
  int ok=0,com=0;decode_lock();if(mf_init()){if(g_CoInitializeEx){WL_I32 hr=g_CoInitializeEx(0,0);if(hr>=0)com=1;}ok=decode_mf(path,&mono,&frames,&sr);if(com&&g_CoUninitialize)g_CoUninitialize();}decode_unlock();if(!ok)return 0;
 }
 WL_U32 target=__atomic_load_n(&g_hostSampleRate,__ATOMIC_ACQUIRE);
 if(target>=8000&&target<=384000&&target!=sr){float*r=0;WL_U64 rf=0;int rr=resample_buffer(mono,frames,sr,target,&r,&rf,peaks);if(rr==1){g_api.HeapFree(g_api.GetProcessHeap(),0,mono);mono=r;frames=rf;sr=target;havePeaks=1;}else if(rr==0){g_api.HeapFree(g_api.GetProcessHeap(),0,mono);return 0;}}
 if(peaks&&!havePeaks)compute_peaks(mono,frames,peaks);
 *out=mono;*outFrames=frames;*outRate=sr;return 1;
}

static WL_DWORD WL_CALLBACK load_job_thread(void*ctx){
 LoadJob*j=(LoadJob*)ctx;float*mono=0;WL_U64 frames=0;WL_U32 sr=0;float peaks[WAVE_PEAKS];int result=-1;
 if(path_exists(j->path)){if(acquire_decode_slot()){result=decode_audio_for_host(j->path,&mono,&frames,&sr,peaks)?1:0;release_decode_slot();}else result=-2;}
 if(result==1){if(__atomic_load_n(&g_loadGeneration[j->idx],__ATOMIC_ACQUIRE)!=j->generation){g_api.HeapFree(g_api.GetProcessHeap(),0,mono);result=-2;}else if(!commit_sample_ex(j->idx,j->path,mono,frames,sr,j->preserveRange,0,peaks))result=0;}
 if(result<=0&&result!=-2&&__atomic_load_n(&g_loadGeneration[j->idx],__ATOMIC_ACQUIRE)==j->generation)__atomic_store_n(&g_pads[j->idx].loadState,j->hadSample?PAD_READY:(result==-1?PAD_MISSING:PAD_DECODE_ERROR),__ATOMIC_RELEASE);
 if(g_main)g_api.PostMessageW(g_main,WM_LOAD_DONE,(WL_WPARAM)j->idx,(WL_LPARAM)result);g_api.HeapFree(g_api.GetProcessHeap(),0,j);__atomic_sub_fetch(&g_loadWorkers,1,__ATOMIC_ACQ_REL);return 0;
}

static int schedule_load_pad(int idx,const WL_WCHAR*path,int preserveRange){if(idx<0||idx>=PAD_COUNT||!path||!path[0])return 0;LoadJob*j=(LoadJob*)g_api.HeapAlloc(g_api.GetProcessHeap(),0,sizeof(LoadJob));if(!j)return 0;memset(j,0,sizeof(*j));j->idx=idx;j->preserveRange=preserveRange;wl_wcpy(j->path,path,MAX_PATH_W);spin_lock();j->hadSample=g_pads[idx].sample?1:0;if(!preserveRange)wl_wcpy(g_pads[idx].path,path,MAX_PATH_W);g_pads[idx].loadState=PAD_LOADING;spin_unlock();j->generation=__atomic_add_fetch(&g_loadGeneration[idx],1,__ATOMIC_ACQ_REL);__atomic_add_fetch(&g_loadWorkers,1,__ATOMIC_ACQ_REL);WL_HANDLE th=g_api.CreateThread(0,0,load_job_thread,j,0,0);if(!th){__atomic_sub_fetch(&g_loadWorkers,1,__ATOMIC_ACQ_REL);__atomic_store_n(&g_pads[idx].loadState,j->hadSample?PAD_READY:PAD_DECODE_ERROR,__ATOMIC_RELEASE);g_api.HeapFree(g_api.GetProcessHeap(),0,j);return 0;}g_api.CloseHandle(th);return 1;}
static WL_DWORD WL_CALLBACK resample_all_thread(void*ctx){
 ResampleJob*j=(ResampleJob*)ctx;WL_U32 target=j->targetRate;g_api.HeapFree(g_api.GetProcessHeap(),0,j);
 for(int i=0;i<PAD_COUNT&&g_running;i++){Sample*old=0;spin_lock();old=g_pads[i].sample;if(old)sample_retain(old);spin_unlock();if(!old)continue;if(old->sampleRate!=target&&acquire_decode_slot()){float*d=0;WL_U64 nf=0;float peaks[WAVE_PEAKS];int rr=resample_buffer(old->mono,old->frames,old->sampleRate,target,&d,&nf,peaks);release_decode_slot();if(rr==1)commit_sample_ex(i,0,d,nf,target,1,old,peaks);}sample_release(old);}
 __atomic_store_n(&g_resampleWorkerRunning,0,__ATOMIC_RELEASE);WL_U32 current=__atomic_load_n(&g_hostSampleRate,__ATOMIC_ACQUIRE);if(current&&current!=target&&g_main)g_api.PostMessageW(g_main,WM_HOST_RATE,(WL_WPARAM)current,0);if(g_main)g_api.PostMessageW(g_main,WM_LOAD_DONE,(WL_WPARAM)PAD_COUNT,1);return 0;
}

static void schedule_resample_all(WL_U32 target){if(target<8000||target>384000||target==g_lastResampleTarget)return;WL_I32 expect=0;if(!__atomic_compare_exchange_n(&g_resampleWorkerRunning,&expect,1,0,__ATOMIC_ACQ_REL,__ATOMIC_ACQUIRE))return;ResampleJob*j=(ResampleJob*)g_api.HeapAlloc(g_api.GetProcessHeap(),0,sizeof(ResampleJob));if(!j){g_resampleWorkerRunning=0;return;}j->targetRate=target;g_lastResampleTarget=target;WL_HANDLE th=g_api.CreateThread(0,0,resample_all_thread,j,0,0);if(th)g_api.CloseHandle(th);else{g_api.HeapFree(g_api.GetProcessHeap(),0,j);g_resampleWorkerRunning=0;}}
static void schedule_config_samples(void){for(int i=0;i<PAD_COUNT;i++){WL_WCHAR path[MAX_PATH_W];spin_lock();wl_wcpy(path,g_pads[i].path,MAX_PATH_W);spin_unlock();if(path[0])schedule_load_pad(i,path,1);}}

static int apply_disk_config(PadDiskV2*disk,WL_U32 version){for(int i=0;i<PAD_COUNT;i++){PadDiskV2*d=&disk[i];g_pads[i].vk=d->vk;g_pads[i].mods=d->mods;g_pads[i].volume=fclamp(d->volume,0,2);g_pads[i].volumeHalfPct=(WL_I32)(g_pads[i].volume*200.0f+0.5f);WL_I32 ph=(version>=4)?d->pitch:d->pitch*2;g_pads[i].pitchHalf=ph<-24?-24:(ph>24?24:ph);g_pads[i].selStart=fclamp(d->selStart,0,1);g_pads[i].selEnd=fclamp(d->selEnd,0,1);if(g_pads[i].selEnd<=g_pads[i].selStart){g_pads[i].selStart=0;g_pads[i].selEnd=1;}wl_wcpy(g_pads[i].path,d->path,MAX_PATH_W);g_pads[i].loadState=d->path[0]?(path_exists(d->path)?PAD_LOADING:PAD_MISSING):PAD_EMPTY;}return 1;}
static int load_new_config_file(const WL_WCHAR*path){
 WL_HANDLE h=g_api.CreateFileW(path,GENERIC_READ,1,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
 if(h==WL_INVALID_HANDLE_VALUE)return 0;
 ConfigHeader ch;WL_DWORD n=0;int ok=0;
 if(!g_api.ReadFile(h,&ch,sizeof(ch),&n,0)||n!=sizeof(ch)||ch.magic!=CFG_MAGIC){g_api.CloseHandle(h);return 0;}
 if(ch.version==9){
  ConfigV9Data*d=(ConfigV9Data*)g_api.HeapAlloc(g_api.GetProcessHeap(),0,sizeof(ConfigV9Data));
  if(d){WL_U32 stored=0;if(g_api.ReadFile(h,d,sizeof(*d),&n,0)&&n==sizeof(*d)&&g_api.ReadFile(h,&stored,sizeof(stored),&n,0)&&n==sizeof(stored)&&stored==crc32_bytes((const WL_U8*)d,sizeof(*d))){
   apply_disk_config(d->base.pads,9);for(int i=0;i<PAD_COUNT;i++)wl_wcpy(g_pads[i].name,d->base.names[i],32);g_stopVk=d->base.stopVk;g_stopMods=d->base.stopMods;g_muteVk=d->base.muteVk;g_muteMods=d->base.muteMods;WL_I32 mv=d->base.monitorVolumeHalfPct;if(mv<0)mv=0;if(mv>200)mv=200;__atomic_store_n(&g_monitorVolumeHalfPct,mv,__ATOMIC_RELEASE);wl_wcpy(g_monitorDeviceName,d->base.monitorDeviceName,MONITOR_DEVICE_NAME_CHARS);wl_wcpy(g_monitorDeviceId,d->base.monitorDeviceId,MONITOR_DEVICE_ID_CHARS);g_savedWindowW=d->windowW;g_savedWindowH=d->windowH;if(g_savedWindowW<700)g_savedWindowW=700;if(g_savedWindowW>2400)g_savedWindowW=2400;if(g_savedWindowH<650)g_savedWindowH=650;if(g_savedWindowH>1600)g_savedWindowH=1600;g_savedTab=d->lastTab?1:0;ok=1;
  }g_api.HeapFree(g_api.GetProcessHeap(),0,d);}
 }else if(ch.version==8){
  ConfigV8Data*d=(ConfigV8Data*)g_api.HeapAlloc(g_api.GetProcessHeap(),0,sizeof(ConfigV8Data));
  if(d){WL_U32 stored=0;if(g_api.ReadFile(h,d,sizeof(*d),&n,0)&&n==sizeof(*d)&&g_api.ReadFile(h,&stored,sizeof(stored),&n,0)&&n==sizeof(stored)&&stored==crc32_bytes((const WL_U8*)d,sizeof(*d))){
   apply_disk_config(d->pads,8);for(int i=0;i<PAD_COUNT;i++)wl_wcpy(g_pads[i].name,d->names[i],32);g_stopVk=d->stopVk;g_stopMods=d->stopMods;g_muteVk=d->muteVk;g_muteMods=d->muteMods;WL_I32 mv=d->monitorVolumeHalfPct;if(mv<0)mv=0;if(mv>200)mv=200;__atomic_store_n(&g_monitorVolumeHalfPct,mv,__ATOMIC_RELEASE);wl_wcpy(g_monitorDeviceName,d->monitorDeviceName,MONITOR_DEVICE_NAME_CHARS);wl_wcpy(g_monitorDeviceId,d->monitorDeviceId,MONITOR_DEVICE_ID_CHARS);ok=1;
  }g_api.HeapFree(g_api.GetProcessHeap(),0,d);}
 }else if(ch.version==7){
  ConfigV7Data*d=(ConfigV7Data*)g_api.HeapAlloc(g_api.GetProcessHeap(),0,sizeof(ConfigV7Data));
  if(d){WL_U32 stored=0;if(g_api.ReadFile(h,d,sizeof(*d),&n,0)&&n==sizeof(*d)&&g_api.ReadFile(h,&stored,sizeof(stored),&n,0)&&n==sizeof(stored)&&stored==crc32_bytes((const WL_U8*)d,sizeof(*d))){
   apply_disk_config(d->pads,7);for(int i=0;i<PAD_COUNT;i++)wl_wcpy(g_pads[i].name,d->names[i],32);
   g_stopVk=d->stopVk;g_stopMods=d->stopMods;g_muteVk=d->muteVk;g_muteMods=d->muteMods;
   WL_I32 mv=d->monitorVolumeHalfPct;if(mv<0)mv=0;if(mv>200)mv=200;__atomic_store_n(&g_monitorVolumeHalfPct,mv,__ATOMIC_RELEASE);wl_wcpy(g_monitorDeviceName,d->monitorDeviceName,32);ok=1;
  }g_api.HeapFree(g_api.GetProcessHeap(),0,d);}
 }else if(ch.version==6){
  ConfigV6Data*d=(ConfigV6Data*)g_api.HeapAlloc(g_api.GetProcessHeap(),0,sizeof(ConfigV6Data));
  if(d){WL_U32 stored=0;if(g_api.ReadFile(h,d,sizeof(*d),&n,0)&&n==sizeof(*d)&&g_api.ReadFile(h,&stored,sizeof(stored),&n,0)&&n==sizeof(stored)&&stored==crc32_bytes((const WL_U8*)d,sizeof(*d))){
   apply_disk_config(d->pads,6);for(int i=0;i<PAD_COUNT;i++)wl_wcpy(g_pads[i].name,d->names[i],32);g_stopVk=d->stopVk;g_stopMods=d->stopMods;g_muteVk=d->muteVk;g_muteMods=d->muteMods;ok=1;
  }g_api.HeapFree(g_api.GetProcessHeap(),0,d);}
 }else if(ch.version==5){
  ConfigV5Data*d=(ConfigV5Data*)g_api.HeapAlloc(g_api.GetProcessHeap(),0,sizeof(ConfigV5Data));
  if(d){WL_U32 stored=0;if(g_api.ReadFile(h,d,sizeof(*d),&n,0)&&n==sizeof(*d)&&g_api.ReadFile(h,&stored,sizeof(stored),&n,0)&&n==sizeof(stored)&&stored==crc32_bytes((const WL_U8*)d,sizeof(*d))){
   apply_disk_config(d->pads,5);for(int i=0;i<PAD_COUNT;i++)wl_wcpy(g_pads[i].name,d->names[i],32);g_stopVk=d->stopVk;g_stopMods=d->stopMods;ok=1;
  }g_api.HeapFree(g_api.GetProcessHeap(),0,d);}
 }else if(ch.version==2||ch.version==3||ch.version==4){
  WL_SIZE_T diskBytes=(WL_SIZE_T)sizeof(PadDiskV2)*PAD_COUNT;PadDiskV2*disk=(PadDiskV2*)g_api.HeapAlloc(g_api.GetProcessHeap(),0,diskBytes);
  if(disk){if(g_api.ReadFile(h,disk,(WL_DWORD)diskBytes,&n,0)&&n==(WL_DWORD)diskBytes){ok=1;if(ch.version>=3){WL_U32 stored=0;if(!g_api.ReadFile(h,&stored,sizeof(stored),&n,0)||n!=sizeof(stored)||stored!=crc32_bytes((const WL_U8*)disk,diskBytes))ok=0;}if(ok)apply_disk_config(disk,ch.version);}g_api.HeapFree(g_api.GetProcessHeap(),0,disk);}
 }
 g_api.CloseHandle(h);if(ok)g_configLoadedVersion=ch.version;return ok;
}
static void load_config(void){
 for(int i=0;i<PAD_COUNT;i++){g_pads[i].volume=1.0f;g_pads[i].volumeHalfPct=200;g_pads[i].pitchHalf=0;g_pads[i].selStart=0.0f;g_pads[i].selEnd=1.0f;g_pads[i].name[0]=0;g_pads[i].path[0]=0;g_pads[i].sample=0;g_pads[i].loadState=PAD_EMPTY;g_playPosPpm[i]=-1;g_padStopSerial[i]=0;}
 g_stopVk=0;g_stopMods=0;g_muteVk=0;g_muteMods=0;g_monitorDeviceName[0]=0;g_monitorDeviceId[0]=0;__atomic_store_n(&g_monitorVolumeHalfPct,200,__ATOMIC_RELEASE);
 WL_WCHAR path[600],bak[610];int loaded=0;
 if(config_path(path,600,0)){wl_wcpy(bak,path,610);wl_wcat(bak,CFG_BAK_EXT,610);if(load_new_config_file(path)){if(!cfg_file_exists(bak)&&pCopyFileW)pCopyFileW(path,bak,WL_TRUE);g_backupState=cfg_file_exists(bak)?1:0;loaded=1;}else if(load_new_config_file(bak)){g_loadedFromBackup=1;g_backupState=2;loaded=1;}}
 if(!loaded&&config_path(path,600,1)){
  WL_HANDLE h=g_api.CreateFileW(path,GENERIC_READ,1,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);if(h!=WL_INVALID_HANDLE_VALUE){ConfigHeader ch;WL_DWORD n=0;if(g_api.ReadFile(h,&ch,sizeof(ch),&n,0)&&n==sizeof(ch)&&ch.magic==CFG_MAGIC&&ch.version==1){for(int i=0;i<PAD_COUNT;i++){PadDiskV1 d;memset(&d,0,sizeof(d));if(!g_api.ReadFile(h,&d,sizeof(d),&n,0)||n!=sizeof(d))break;g_pads[i].vk=d.vk;g_pads[i].mods=d.mods;g_pads[i].volume=fclamp(d.volume,0,2);g_pads[i].volumeHalfPct=(WL_I32)(g_pads[i].volume*200.0f+0.5f);wl_wcpy(g_pads[i].path,d.path,MAX_PATH_W);g_pads[i].loadState=d.path[0]?(path_exists(d.path)?PAD_LOADING:PAD_MISSING):PAD_EMPTY;}g_configLoadedVersion=1;loaded=1;}g_api.CloseHandle(h);}
 }
 if(loaded&&g_configLoadedVersion<9){g_configMigratedFrom=g_configLoadedVersion;save_config();}
}

/* Hotkey/UI text. */
static void key_name(WL_U32 vk,WL_WCHAR*out,WL_SIZE_T cap){out[0]=0;if((vk>='0'&&vk<='9')||(vk>='A'&&vk<='Z')){out[0]=(WL_WCHAR)vk;out[1]=0;return;}if(vk>=0x70&&vk<=0x87){wl_wcat(out,(const WL_WCHAR[]){'F',0},cap);wadd_num(out,cap,(int)(vk-0x6f));return;}if(vk>=0x60&&vk<=0x69){wl_wcat(out,(const WL_WCHAR[]){'N','u','m',' ',0},cap);wadd_num(out,cap,(int)(vk-0x60));return;}WL_UINT sc=g_api.MapVirtualKeyW(vk,0);WL_I32 lp=(WL_I32)(sc<<16);if(!g_api.GetKeyNameTextW(lp,out,(WL_I32)cap))wadd_num(out,cap,(int)vk);}
static void hotkey_value_text(WL_U32 v,WL_U32 m,WL_I32 state,WL_WCHAR*out,WL_SIZE_T cap){out[0]=0;if(!v){wadd_ascii(out,cap,"None");return;}if(m&MOD_CONTROL)wadd_ascii(out,cap,"Ctrl+");if(m&MOD_ALT)wadd_ascii(out,cap,"Alt+");if(m&MOD_SHIFT)wadd_ascii(out,cap,"Shift+");if(m&MOD_WIN)wadd_ascii(out,cap,"Win+");WL_WCHAR k[64];key_name(v,k,64);wl_wcat(out,k,cap);if(state<0)wadd_ascii(out,cap," [CONFLICT]");}
static void hotkey_text(int i,WL_WCHAR*out,WL_SIZE_T cap){hotkey_value_text(g_pads[i].vk,g_pads[i].mods,g_hotkeyState[i],out,cap);}
static void stop_hotkey_text(WL_WCHAR*out,WL_SIZE_T cap){hotkey_value_text(g_stopVk,g_stopMods,g_stopHotkeyState,out,cap);}
static void mute_hotkey_text(WL_WCHAR*out,WL_SIZE_T cap){hotkey_value_text(g_muteVk,g_muteMods,g_muteHotkeyState,out,cap);}
static void update_pad_button(int i){if(!g_padButtons[i])return;WL_WCHAR t[512],name[32],path[MAX_PATH_W];WL_I32 state;t[0]=0;spin_lock();wl_wcpy(name,g_pads[i].name,32);wl_wcpy(path,g_pads[i].path,MAX_PATH_W);state=g_pads[i].loadState;spin_unlock();if(__atomic_load_n(&g_activeVoices[i],__ATOMIC_RELAXED)>0)wadd_ascii(t,512,"[PLAYING] ");else if(state==PAD_LOADING)wadd_ascii(t,512,"[LOADING] ");else if(state==PAD_MISSING)wadd_ascii(t,512,"[MISSING] ");else if(state==PAD_DECODE_ERROR)wadd_ascii(t,512,"[DECODE ERROR] ");wadd_ascii(t,512,"Pad ");wadd_num(t,512,i+1);wl_wcat(t,(const WL_WCHAR[]){'\r','\n',0},512);if(name[0])wl_wcat(t,name,512);else if(path[0]){const WL_WCHAR*b=base_name(path);WL_SIZE_T len=wl_wlen(b);if(len>26)b+=len-26;wl_wcat(t,b,512);}else wadd_ascii(t,512,"(empty - right-click to load)");wl_wcat(t,(const WL_WCHAR[]){'\r','\n',0},512);WL_WCHAR hk[100];hotkey_text(i,hk,100);wl_wcat(t,hk,512);set_text_if_changed(g_padButtons[i],t);}
static void update_name_edit(void){if(!g_nameEdit)return;WL_WCHAR n[32];spin_lock();wl_wcpy(n,g_pads[g_selected].name,32);spin_unlock();set_text_if_changed(g_nameEdit,n);}
static const char* file_type_ascii(const WL_WCHAR*path){if(wends_ascii_ci(path,".wav"))return "WAV";if(wends_ascii_ci(path,".mp3"))return "MP3";if(wends_ascii_ci(path,".flac"))return "FLAC";if(wends_ascii_ci(path,".m4a"))return "M4A";if(wends_ascii_ci(path,".aac"))return "AAC";if(wends_ascii_ci(path,".wma"))return "WMA";if(wends_ascii_ci(path,".opus"))return "OPUS";if(wends_ascii_ci(path,".ogg")||wends_ascii_ci(path,".oga"))return "OGG";return "AUDIO";}
static void update_file_info(void){
 if(!g_fileInfoText)return;WL_WCHAR t[520],path[MAX_PATH_W];t[0]=0;WL_U64 frames=0;WL_U32 sr=0;WL_I32 state=PAD_EMPTY;
 spin_lock();wl_wcpy(path,g_pads[g_selected].path,MAX_PATH_W);Sample*s=g_pads[g_selected].sample;if(s){frames=s->frames;sr=s->sampleRate;}state=g_pads[g_selected].loadState;spin_unlock();
 wadd_ascii(t,520,"File: ");if(!path[0]){wadd_ascii(t,520,"(none)");set_text_if_changed(g_fileInfoText,t);return;}const WL_WCHAR*b=base_name(path);wl_wcat(t,b,520);wadd_ascii(t,520," | ");wadd_ascii(t,520,file_type_ascii(path));
 if(state==PAD_LOADING)wadd_ascii(t,520," | Loading...");else if(state==PAD_MISSING)wadd_ascii(t,520," | Missing");else if(state==PAD_DECODE_ERROR)wadd_ascii(t,520," | Decode failed");else if(frames&&sr){wadd_ascii(t,520," | ");wadd_num(t,520,(int)sr);wadd_ascii(t,520," Hz | ");wadd_duration10(t,520,(frames*10ull)/(WL_U64)sr);wadd_ascii(t,520," | Decoded ");WL_U64 kb=(frames*sizeof(float)+1023ull)/1024ull;if(kb>=1024ull){WL_U64 tenths=(kb*10ull)/1024ull;wadd_u64(t,520,tenths/10ull);wadd_ascii(t,520,".");wadd_num(t,520,(int)(tenths%10ull));wadd_ascii(t,520," MB");}else{wadd_u64(t,520,kb);wadd_ascii(t,520," KB");}}
 set_text_if_changed(g_fileInfoText,t);
}
static void update_selected_text(void){if(!g_selectedText)return;WL_WCHAR t[480];t[0]=0;WL_I32 volHalf=__atomic_load_n(&g_pads[g_selected].volumeHalfPct,__ATOMIC_RELAXED),pitchHalf=__atomic_load_n(&g_pads[g_selected].pitchHalf,__ATOMIC_RELAXED);float a,b;WL_I32 state;spin_lock();a=g_pads[g_selected].selStart;b=g_pads[g_selected].selEnd;state=g_pads[g_selected].loadState;spin_unlock();wadd_ascii(t,480,"Selected pad: ");wadd_num(t,480,g_selected+1);wadd_ascii(t,480,"    Volume: ");wadd_half_units(t,480,volHalf,0);wadd_ascii(t,480,"%    Pitch: ");wadd_half_units(t,480,pitchHalf,1);wadd_ascii(t,480," st    Range: ");wadd_num(t,480,(int)(a*100.0f+0.5f));wadd_ascii(t,480,"-");wadd_num(t,480,(int)(b*100.0f+0.5f));wadd_ascii(t,480,"%    Hotkey: ");WL_WCHAR hk[100];hotkey_text(g_selected,hk,100);wl_wcat(t,hk,480);if(state==PAD_LOADING)wadd_ascii(t,480,"    [Loading]");else if(state==PAD_MISSING)wadd_ascii(t,480,"    [Missing file]");else if(state==PAD_DECODE_ERROR)wadd_ascii(t,480,"    [Decode failed]");set_text_if_changed(g_selectedText,t);}
static void set_status_ascii(const char*s){if(!g_statusText)return;WL_WCHAR t[420];wset_ascii(t,420,s);set_text_if_changed(g_statusText,t);}
static void update_stop_hotkey_text(void){if(!g_stopHotkeyText)return;WL_WCHAR t[220],hk[100];t[0]=0;wadd_ascii(t,220,"Global Stop All hotkey: ");stop_hotkey_text(hk,100);wl_wcat(t,hk,220);set_text_if_changed(g_stopHotkeyText,t);}
static void update_mute_hotkey_text(void){if(!g_muteHotkeyText)return;WL_WCHAR t[260],hk[100];t[0]=0;wadd_ascii(t,260,"Board Mute hotkey: ");mute_hotkey_text(hk,100);wl_wcat(t,hk,260);set_text_if_changed(g_muteHotkeyText,t);}
static void update_mute_state_text(void){if(!g_muteStateText)return;WL_WCHAR t[160];t[0]=0;wadd_ascii(t,160,"Soundboard output: ");wadd_ascii(t,160,__atomic_load_n(&g_boardMuted,__ATOMIC_RELAXED)?"MUTED":"ON");set_text_if_changed(g_muteStateText,t);}
static void update_hotkey_state_text(void){if(!g_hotkeyStateText)return;WL_WCHAR t[240];t[0]=0;if(g_capturePad>=0){wadd_ascii(t,240,"Hotkeys: CAPTURING ");if(g_capturePad<PAD_COUNT){wadd_ascii(t,240,"PAD ");wadd_num(t,240,g_capturePad+1);}else if(g_capturePad==PAD_COUNT)wadd_ascii(t,240,"STOP ALL");else wadd_ascii(t,240,"BOARD MUTE");wadd_ascii(t,240," (normal triggers suspended)");}else if(g_hotkeysSuspended)wadd_ascii(t,240,"Hotkeys: SUSPENDED");else wadd_ascii(t,240,"Hotkeys: ACTIVE");set_text_if_changed(g_hotkeyStateText,t);}
static int find_internal_hotkey_conflict(int pad,WL_U32 vk,WL_U32 mods){if(!vk)return -1;for(int i=0;i<PAD_COUNT;i++)if(i!=pad&&g_pads[i].vk==vk&&g_pads[i].mods==mods)return i;if(g_stopVk==vk&&g_stopMods==mods)return PAD_COUNT;if(g_muteVk==vk&&g_muteMods==mods)return PAD_COUNT+1;return -1;}
static int find_stop_hotkey_conflict(WL_U32 vk,WL_U32 mods){if(!vk)return -1;for(int i=0;i<PAD_COUNT;i++)if(g_pads[i].vk==vk&&g_pads[i].mods==mods)return i;if(g_muteVk==vk&&g_muteMods==mods)return PAD_COUNT+1;return -1;}
static int find_mute_hotkey_conflict(WL_U32 vk,WL_U32 mods){if(!vk)return -1;for(int i=0;i<PAD_COUNT;i++)if(g_pads[i].vk==vk&&g_pads[i].mods==mods)return i;if(g_stopVk==vk&&g_stopMods==mods)return PAD_COUNT;return -1;}
static int register_pad_hotkey(int i){g_api.UnregisterHotKey(g_main,HOTKEY_BASE+i);g_hotkeyState[i]=0;if(!g_pads[i].vk)return 1;if(find_internal_hotkey_conflict(i,g_pads[i].vk,g_pads[i].mods)>=0){g_hotkeyState[i]=-1;return 0;}if(g_api.RegisterHotKey(g_main,HOTKEY_BASE+i,g_pads[i].mods|MOD_NOREPEAT,g_pads[i].vk)){g_hotkeyState[i]=1;return 1;}g_hotkeyState[i]=-1;return 0;}
static int register_stop_hotkey(void){g_api.UnregisterHotKey(g_main,HOTKEY_STOP);g_stopHotkeyState=0;if(!g_stopVk)return 1;if(find_stop_hotkey_conflict(g_stopVk,g_stopMods)>=0){g_stopHotkeyState=-1;return 0;}if(g_api.RegisterHotKey(g_main,HOTKEY_STOP,g_stopMods|MOD_NOREPEAT,g_stopVk)){g_stopHotkeyState=1;return 1;}g_stopHotkeyState=-1;return 0;}
static int register_mute_hotkey(void){g_api.UnregisterHotKey(g_main,HOTKEY_STOP+1);g_muteHotkeyState=0;if(!g_muteVk)return 1;if(find_mute_hotkey_conflict(g_muteVk,g_muteMods)>=0){g_muteHotkeyState=-1;return 0;}if(g_api.RegisterHotKey(g_main,HOTKEY_STOP+1,g_muteMods|MOD_NOREPEAT,g_muteVk)){g_muteHotkeyState=1;return 1;}g_muteHotkeyState=-1;return 0;}
static void suspend_hotkeys_for_capture(void){if(g_hotkeysSuspended)return;for(int i=0;i<PAD_COUNT;i++)g_api.UnregisterHotKey(g_main,HOTKEY_BASE+i);g_api.UnregisterHotKey(g_main,HOTKEY_STOP);g_api.UnregisterHotKey(g_main,HOTKEY_STOP+1);g_hotkeysSuspended=1;update_hotkey_state_text();}
static void restore_hotkeys_after_capture(void){if(!g_hotkeysSuspended)return;g_hotkeysSuspended=0;for(int i=0;i<PAD_COUNT;i++)register_pad_hotkey(i);register_stop_hotkey();register_mute_hotkey();for(int i=0;i<PAD_COUNT;i++)update_pad_button(i);update_selected_text();update_stop_hotkey_text();update_mute_hotkey_text();update_hotkey_state_text();}
static void trigger_pad(int i){if(i<0||i>=PAD_COUNT||__atomic_load_n(&g_boardMuted,__ATOMIC_ACQUIRE))return;Sample*s=0;spin_lock();s=g_pads[i].sample;spin_unlock();if(!s){__atomic_add_fetch(&g_droppedTriggers,1,__ATOMIC_RELAXED);return;}__atomic_add_fetch(&g_triggerSerial[i],1,__ATOMIC_SEQ_CST);}
static void stop_all_playback(void){__atomic_add_fetch(&g_stopSerial,1,__ATOMIC_SEQ_CST);}
static void stop_pad_playback(int i){if(i>=0&&i<PAD_COUNT)__atomic_add_fetch(&g_padStopSerial[i],1,__ATOMIC_SEQ_CST);}
static void toggle_board_mute(void){WL_I32 now=!__atomic_load_n(&g_boardMuted,__ATOMIC_ACQUIRE);__atomic_store_n(&g_boardMuted,now,__ATOMIC_RELEASE);if(now)stop_all_playback();update_mute_state_text();set_status_ascii(now?"Soundboard output muted. Microphone passthrough remains active.":"Soundboard output enabled.");}

static WL_U64 diag_ft64(DIAG_FILETIME f){return ((WL_U64)f.hi<<32)|f.lo;}
static int diag_thread_count(void){if(!pCreateToolhelp32Snapshot||!pThread32First||!pThread32Next||!pGetCurrentProcessId)return -1;WL_HANDLE s=pCreateToolhelp32Snapshot(TH32CS_SNAPTHREAD,0);if(s==WL_INVALID_HANDLE_VALUE)return -1;DIAG_THREADENTRY32 te;memset(&te,0,sizeof(te));te.dwSize=sizeof(te);WL_DWORD pid=pGetCurrentProcessId();int c=0;if(pThread32First(s,&te)){do{if(te.th32OwnerProcessID==pid)c++;te.dwSize=sizeof(te);}while(pThread32Next(s,&te));}g_api.CloseHandle(s);return c;}
static int diag_hotkey_conflicts(void){int c=(g_stopHotkeyState<0?1:0)+(g_muteHotkeyState<0?1:0);for(int i=0;i<PAD_COUNT;i++)if(g_hotkeyState[i]<0)c++;return c;}
static void reset_session_stats(void){g_peakCpuPct=(WL_U32)g_diagCpuPct;WL_HANDLE proc=pGetCurrentProcess?pGetCurrentProcess():0;g_maxRamMb=0;if(proc&&pK32GetProcessMemoryInfo){DIAG_PMC pm;memset(&pm,0,sizeof(pm));pm.cb=sizeof(pm);if(pK32GetProcessMemoryInfo(proc,&pm,sizeof(pm)))g_maxRamMb=(WL_U32)(pm.WorkingSetSize/(1024u*1024u));}g_lowestRingFill=0xffffffffu;g_statsBaseUnderruns=__atomic_load_n(&g_totalUnderrunEvents,__ATOMIC_RELAXED);g_statsBaseClips=__atomic_load_n(&g_totalClipFrames,__ATOMIC_RELAXED);g_statsBaseSteals=__atomic_load_n(&g_voiceSteals,__ATOMIC_RELAXED);g_statsBaseReconnects=__atomic_load_n(&g_reconnects,__ATOMIC_RELAXED);g_statsBaseDropped=__atomic_load_n(&g_droppedTriggers,__ATOMIC_RELAXED);__atomic_store_n(&g_monitorUnderruns,0,__ATOMIC_RELAXED);__atomic_store_n(&g_monitorOverruns,0,__ATOMIC_RELAXED);__atomic_store_n(&g_monitorCorrections,0,__ATOMIC_RELAXED);__atomic_store_n(&g_monitorErrors,0,__ATOMIC_RELAXED);__atomic_store_n(&g_monitorMaxWakeMs,0,__ATOMIC_RELAXED);set_status_ascii("Session performance statistics reset.");}
static void run_self_test(void){WL_WCHAR t[700];t[0]=0;int pass=1;WL_I32 con=__atomic_load_n(&g_connections,__ATOMIC_RELAXED);WL_U32 sr=__atomic_load_n(&g_hostSampleRate,__ATOMIC_RELAXED),bs=__atomic_load_n(&g_hostBlockSize,__ATOMIC_RELAXED),tf=__atomic_load_n(&g_targetFill,__ATOMIC_RELAXED);WL_I32 ps=__atomic_load_n(&g_protocolStatus,__ATOMIC_RELAXED);wadd_ascii(t,700,"APO Soundboard engine self-test\r\n\r\n");wadd_ascii(t,700,"Controller <-> VST connection: ");if(con>0)wadd_ascii(t,700,"PASS\r\n");else{wadd_ascii(t,700,"FAIL (no VST connection)\r\n");pass=0;}wadd_ascii(t,700,"Protocol v3 handshake: ");if(ps>0)wadd_ascii(t,700,"PASS\r\n");else if(ps<0){wadd_ascii(t,700,"FAIL (version mismatch detected)\r\n");pass=0;}else{wadd_ascii(t,700,"FAIL / waiting\r\n");pass=0;}wadd_ascii(t,700,"Host sample rate: ");if(sr>=8000&&sr<=384000){wadd_ascii(t,700,"PASS (");wadd_num(t,700,(int)sr);wadd_ascii(t,700," Hz)\r\n");}else{wadd_ascii(t,700,"FAIL / waiting\r\n");pass=0;}wadd_ascii(t,700,"Host block size: ");if(bs>0){wadd_ascii(t,700,"PASS (");wadd_num(t,700,(int)bs);wadd_ascii(t,700,")\r\n");}else{wadd_ascii(t,700,"FAIL / waiting\r\n");pass=0;}wadd_ascii(t,700,"Adaptive ring telemetry: ");if(tf>=256&&tf<=4096)wadd_ascii(t,700,"PASS\r\n");else{wadd_ascii(t,700,"FAIL / waiting\r\n");pass=0;}wadd_ascii(t,700,"\r\nResult: ");wadd_ascii(t,700,pass?"PASS":"CHECK FAILED ITEMS");g_api.MessageBoxW(g_main,t,WINDOW_TITLE,pass?0x40:0x30);set_status_ascii(pass?"Audio engine self-test passed.":"Audio engine self-test found a problem; see the test dialog.");}
static void update_diagnostics(void){
 drain_retired_samples();WL_HANDLE proc=pGetCurrentProcess?pGetCurrentProcess():0;int ram=-1,handles=-1,gdi=-1,user=-1,threads=diag_thread_count();
 if(proc&&pK32GetProcessMemoryInfo){DIAG_PMC pm;memset(&pm,0,sizeof(pm));pm.cb=sizeof(pm);if(pK32GetProcessMemoryInfo(proc,&pm,sizeof(pm)))ram=(int)(pm.WorkingSetSize/(1024u*1024u));}if(ram>=0&&(WL_U32)ram>g_maxRamMb)g_maxRamMb=(WL_U32)ram;
 if(proc&&pGetProcessHandleCount){WL_DWORD h=0;if(pGetProcessHandleCount(proc,&h))handles=(int)h;}if(proc&&pGetGuiResources){gdi=(int)pGetGuiResources(proc,GR_GDIOBJECTS);user=(int)pGetGuiResources(proc,GR_USEROBJECTS);}
 if(proc&&pGetProcessTimes&&pGetTickCount64){DIAG_FILETIME a,b,k,u;if(pGetProcessTimes(proc,&a,&b,&k,&u)){WL_U64 now=pGetTickCount64(),cpu=diag_ft64(k)+diag_ft64(u);if(g_diagLastTick&&now>g_diagLastTick){WL_U64 wall=now-g_diagLastTick,diff=cpu>=g_diagLastCpu?cpu-g_diagLastCpu:0;WL_DWORD cores=pGetActiveProcessorCount?pGetActiveProcessorCount((WL_WORD)ALL_PROCESSOR_GROUPS):1;if(!cores)cores=1;WL_U64 den=wall*100ull*(WL_U64)cores;if(den)g_diagCpuPct=(WL_I32)(diff/den);if(g_diagCpuPct>999)g_diagCpuPct=999;if((WL_U32)g_diagCpuPct>g_peakCpuPct)g_peakCpuPct=(WL_U32)g_diagCpuPct;}g_diagLastTick=now;g_diagLastCpu=cpu;}}
 WL_U32 sr=__atomic_load_n(&g_hostSampleRate,__ATOMIC_RELAXED),bs=__atomic_load_n(&g_hostBlockSize,__ATOMIC_RELAXED),rf=__atomic_load_n(&g_ringFill,__ATOMIC_RELAXED),tf=__atomic_load_n(&g_targetFill,__ATOMIC_RELAXED);if(sr&&tf&&rf<g_lowestRingFill)g_lowestRingFill=rf;
 WL_U64 su=__atomic_load_n(&g_totalUnderrunEvents,__ATOMIC_RELAXED)-g_statsBaseUnderruns,sc=__atomic_load_n(&g_totalClipFrames,__ATOMIC_RELAXED)-g_statsBaseClips;WL_U32 steals=__atomic_load_n(&g_voiceSteals,__ATOMIC_RELAXED)-g_statsBaseSteals,recs=__atomic_load_n(&g_reconnects,__ATOMIC_RELAXED)-g_statsBaseReconnects,drops=__atomic_load_n(&g_droppedTriggers,__ATOMIC_RELAXED)-g_statsBaseDropped;
 WL_U32 mu=__atomic_load_n(&g_monitorUnderruns,__ATOMIC_RELAXED),mo=__atomic_load_n(&g_monitorOverruns,__ATOMIC_RELAXED),mc=__atomic_load_n(&g_monitorCorrections,__ATOMIC_RELAXED),mb=__atomic_load_n(&g_monitorBufferMs,__ATOMIC_RELAXED),mdr=__atomic_load_n(&g_monitorDeviceRate,__ATOMIC_RELAXED),mdl=__atomic_load_n(&g_monitorDeviceLatencyMs,__ATOMIC_RELAXED),mw=__atomic_load_n(&g_monitorMaxWakeMs,__ATOMIC_RELAXED);WL_I32 ppm=__atomic_load_n(&g_monitorDriftPpm,__ATOMIC_RELAXED);
 static WL_WCHAR t[6][600];for(int i=0;i<6;i++)t[i][0]=0;
 /* Audio Engine */
 wadd_ascii(t[0],600,"VST: ");wadd_ascii(t[0],600,__atomic_load_n(&g_connections,__ATOMIC_RELAXED)>0?"Connected":"Waiting");wadd_ascii(t[0],600,"\r\nHost: ");if(sr){wadd_num(t[0],600,(int)sr);wadd_ascii(t[0],600," Hz");}else wadd_ascii(t[0],600,"waiting");wadd_ascii(t[0],600,"\r\nBlock: ");wadd_num(t[0],600,(int)bs);wadd_ascii(t[0],600," frames\r\nRing: ");wadd_num(t[0],600,(int)rf);wadd_ascii(t[0],600," / ");wadd_num(t[0],600,(int)tf);if(sr){WL_U64 lat10=((WL_U64)(tf+bs)*10000ull)/(WL_U64)sr;wadd_ascii(t[0],600,"\r\nLatency: ");wadd_num(t[0],600,(int)(lat10/10ull));wadd_ascii(t[0],600,".");wadd_num(t[0],600,(int)(lat10%10ull));wadd_ascii(t[0],600," ms");}wadd_ascii(t[0],600,"\r\nVST underruns: ");wadd_u64(t[0],600,su);
 /* Output */
 WL_U32 peak=__atomic_exchange_n(&g_peakMilli,0,__ATOMIC_ACQ_REL);wadd_ascii(t[1],600,"Peak: ");wadd_num(t[1],600,(int)(peak/10));wadd_ascii(t[1],600,".");wadd_num(t[1],600,(int)(peak%10));wadd_ascii(t[1],600,"%\r\nActual clipped frames: ");wadd_u64(t[1],600,sc);wadd_ascii(t[1],600,"\r\nBoard: ");wadd_ascii(t[1],600,__atomic_load_n(&g_boardMuted,__ATOMIC_RELAXED)?"MUTED":"ON");int active=0;for(int i=0;i<PAD_COUNT;i++)active+=__atomic_load_n(&g_activeVoices[i],__ATOMIC_RELAXED);wadd_ascii(t[1],600,"\r\nActive voices: ");wadd_num(t[1],600,active);wadd_ascii(t[1],600," / 32\r\nVoice steals: ");wadd_num(t[1],600,(int)steals);wadd_ascii(t[1],600,"\r\nDropped triggers: ");wadd_num(t[1],600,(int)drops);
 /* Local Monitor */
 wadd_ascii(t[2],600,"State: ");wadd_ascii(t[2],600,__atomic_load_n(&g_monitorEnabled,__ATOMIC_RELAXED)?"ON":"OFF");wadd_ascii(t[2],600,"\r\nMode: ");WL_I32 mmode=__atomic_load_n(&g_monitorBackendMode,__ATOMIC_RELAXED);wadd_ascii(t[2],600,mmode==3?"WinMM compatible":(mmode==1?"Event-driven WASAPI":(mmode==2?"Shared WASAPI fallback":"Closed")));wadd_ascii(t[2],600,"\r\nDevice: ");if(g_monitorDeviceName[0])wl_wcat(t[2],g_monitorDeviceName,600);else wadd_ascii(t[2],600,"Windows Default");wadd_ascii(t[2],600,"\r\nBuffer: ");wadd_num(t[2],600,(int)mb);wadd_ascii(t[2],600," ms\r\nUnderruns: ");wadd_num(t[2],600,(int)mu);wadd_ascii(t[2],600,"\r\nOverruns: ");wadd_num(t[2],600,(int)mo);wadd_ascii(t[2],600,"\r\nCorrections: ");wadd_num(t[2],600,(int)mc);wadd_ascii(t[2],600,"\r\nDrift: ");if(ppm>=0)wadd_ascii(t[2],600,"+");wadd_num(t[2],600,ppm);wadd_ascii(t[2],600," ppm\r\nDevice rate: ");if(mdr){wadd_num(t[2],600,(int)mdr);wadd_ascii(t[2],600," Hz");}else wadd_ascii(t[2],600,"waiting");wadd_ascii(t[2],600,"\r\nDevice latency: ");wadd_num(t[2],600,(int)mdl);wadd_ascii(t[2],600," ms\r\nMax wake gap: ");wadd_num(t[2],600,(int)mw);wadd_ascii(t[2],600," ms\r\nErrors: ");wadd_num(t[2],600,(int)__atomic_load_n(&g_monitorErrors,__ATOMIC_RELAXED));
 /* System */
 wadd_ascii(t[3],600,"RAM: ");if(ram>=0)wadd_num(t[3],600,ram);else wadd_ascii(t[3],600,"?");wadd_ascii(t[3],600," MB\r\nPeak RAM: ");wadd_num(t[3],600,(int)g_maxRamMb);wadd_ascii(t[3],600," MB\r\nSample RAM: ");WL_I64 sampleBytes=__atomic_load_n(&g_totalSampleBytes,__ATOMIC_RELAXED);wadd_u64(t[3],600,(WL_U64)(sampleBytes<0?0:sampleBytes)/(1024ull*1024ull));wadd_ascii(t[3],600," MB\r\nCPU: ");wadd_num(t[3],600,g_diagCpuPct);wadd_ascii(t[3],600,"%\r\nPeak CPU: ");wadd_num(t[3],600,(int)g_peakCpuPct);wadd_ascii(t[3],600,"%\r\nThreads: ");if(threads>=0)wadd_num(t[3],600,threads);else wadd_ascii(t[3],600,"?");wadd_ascii(t[3],600,"\r\nHandles: ");if(handles>=0)wadd_num(t[3],600,handles);else wadd_ascii(t[3],600,"?");wadd_ascii(t[3],600,"\r\nGDI / USER: ");if(gdi>=0)wadd_num(t[3],600,gdi);else wadd_ascii(t[3],600,"?");wadd_ascii(t[3],600," / ");if(user>=0)wadd_num(t[3],600,user);else wadd_ascii(t[3],600,"?");
 /* Session */
 wadd_ascii(t[4],600,"Reconnects: ");wadd_num(t[4],600,(int)recs);wadd_ascii(t[4],600,"\r\nProtocol mismatches: ");wadd_num(t[4],600,(int)__atomic_load_n(&g_protocolMismatch,__ATOMIC_RELAXED));wadd_ascii(t[4],600,"\r\nLowest ring fill: ");if(g_lowestRingFill==0xffffffffu)wadd_ascii(t[4],600,"-");else wadd_num(t[4],600,(int)g_lowestRingFill);wadd_ascii(t[4],600,"\r\nHQ resampler: cubic\r\nLive smoothing: 15 ms\r\nDecode workers: ");wadd_num(t[4],600,(int)__atomic_load_n(&g_decodeSlots,__ATOMIC_RELAXED));wadd_ascii(t[4],600," / ");wadd_num(t[4],600,MAX_DECODE_WORKERS);
 /* Status */
 wadd_ascii(t[5],600,"Hotkeys: ");wadd_ascii(t[5],600,g_hotkeysSuspended?"SUSPENDED":"ACTIVE");wadd_ascii(t[5],600,"\r\nConflicts: ");wadd_num(t[5],600,diag_hotkey_conflicts());wadd_ascii(t[5],600,"\r\nBackup: ");if(g_backupState==1)wadd_ascii(t[5],600,"Ready");else if(g_backupState==2)wadd_ascii(t[5],600,"Restored");else if(g_backupState==3)wadd_ascii(t[5],600,"ERROR");else wadd_ascii(t[5],600,"None");WL_WCHAR shk[100],mhk[100];stop_hotkey_text(shk,100);mute_hotkey_text(mhk,100);wadd_ascii(t[5],600,"\r\nStop All: ");wl_wcat(t[5],shk,600);wadd_ascii(t[5],600,"\r\nBoard Mute: ");wl_wcat(t[5],mhk,600);wadd_ascii(t[5],600,"\r\nConfig: v9");if(g_configMigratedFrom){wadd_ascii(t[5],600," (migrated from v");wadd_num(t[5],600,(int)g_configMigratedFrom);wadd_ascii(t[5],600,")");}wadd_ascii(t[5],600,"\r\nUptime: ");WL_U64 secs=(pGetTickCount64&&g_diagStartTick)?(pGetTickCount64()-g_diagStartTick)/1000ull:0;wadd_u64(t[5],600,secs/3600ull);wadd_ascii(t[5],600,":");wadd_2(t[5],600,(int)((secs/60ull)%60ull));wadd_ascii(t[5],600,":");wadd_2(t[5],600,(int)(secs%60ull));
 for(int i=0;i<6;i++)if(g_diagValues[i])set_text_if_changed(g_diagValues[i],t[i]);
}


static const WL_WCHAR AUDIO_FILTER[]={ 'A','u','d','i','o',' ','f','i','l','e','s',' ','(','W','A','V',',',' ','M','P','3',',',' ','F','L','A','C',',',' ','M','4','A',',',' ','A','A','C',',',' ','W','M','A',',',' ','O','G','G',',',' ','O','P','U','S',')',0,'*','.','w','a','v',';','*','.','m','p','3',';','*','.','f','l','a','c',';','*','.','m','4','a',';','*','.','a','a','c',';','*','.','w','m','a',';','*','.','o','g','g',';','*','.','o','g','a',';','*','.','o','p','u','s',0,'O','g','g',' ','/',' ','O','p','u','s',' ','f','i','l','e','s',0,'*','.','o','g','g',';','*','.','o','g','a',';','*','.','o','p','u','s',0,'W','A','V',' ','f','i','l','e','s',0,'*','.','w','a','v',0,'A','l','l',' ','f','i','l','e','s',0,'*','.','*',0,0};
static int pick_audio_file(WL_WCHAR*file){file[0]=0;WL_OPENFILENAMEW ofn;memset(&ofn,0,sizeof(ofn));ofn.lStructSize=sizeof(ofn);ofn.hwndOwner=g_main;ofn.lpstrFilter=AUDIO_FILTER;ofn.lpstrFile=file;ofn.nMaxFile=MAX_PATH_W;ofn.Flags=OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_EXPLORER|OFN_NOCHANGEDIR;return g_api.GetOpenFileNameW(&ofn)?1:0;}
static int choose_file_for_pad(int i){WL_WCHAR file[MAX_PATH_W];if(!pick_audio_file(file))return 0;if(!schedule_load_pad(i,file,0)){g_api.MessageBoxW(g_main,(const WL_WCHAR[]){'C','o','u','l','d',' ','n','o','t',' ','s','t','a','r','t',' ','t','h','e',' ','b','a','c','k','g','r','o','u','n','d',' ','a','u','d','i','o',' ','l','o','a','d','e','r','.',0},WINDOW_TITLE,0x10);return 0;}update_pad_button(i);update_selected_text();set_status_ascii("Loading sample in the background. The controller remains responsive.");return 1;}
static void detach_pad_audio(int i){
 if(i<0||i>=PAD_COUNT)return;__atomic_add_fetch(&g_loadGeneration[i],1,__ATOMIC_ACQ_REL);stop_pad_playback(i);Sample*old=0;
 spin_lock();old=g_pads[i].sample;g_pads[i].sample=0;g_pads[i].path[0]=0;g_pads[i].loadState=PAD_EMPTY;g_pads[i].selStart=0.0f;g_pads[i].selEnd=1.0f;spin_unlock();sample_release(old);drain_retired_samples();
}
static void remove_audio_selected(void){
 int i=g_selected;detach_pad_audio(i);save_config();update_pad_button(i);update_selected_text();update_file_info();invalidate_wave();set_status_ascii("Audio removed from this pad. Name, hotkey, volume and pitch were kept.");
}
static void reload_audio_selected(void){
 WL_WCHAR path[MAX_PATH_W];spin_lock();wl_wcpy(path,g_pads[g_selected].path,MAX_PATH_W);spin_unlock();if(!path[0]){set_status_ascii("This pad has no audio file to reload.");return;}if(!schedule_load_pad(g_selected,path,1)){set_status_ascii("Could not start the background reload.");return;}update_pad_button(g_selected);update_selected_text();update_file_info();set_status_ascii("Reloading this pad's audio from disk in the background.");
}
static void reset_selected_pad(void){
 static const WL_WCHAR q[]={'R','e','s','e','t',' ','t','h','i','s',' ','p','a','d','?','\r','\n','\r','\n','T','h','i','s',' ','c','l','e','a','r','s',' ','i','t','s',' ','a','u','d','i','o',',',' ','n','a','m','e',',',' ','h','o','t','k','e','y',',',' ','w','a','v','e','f','o','r','m',' ','r','a','n','g','e',' ','a','n','d',' ','r','e','s','e','t','s',' ','v','o','l','u','m','e','/','p','i','t','c','h','.',0};
 if(g_api.MessageBoxW(g_main,q,WINDOW_TITLE,MB_YESNO|MB_ICONWARNING)!=IDYES)return;int i=g_selected;g_api.UnregisterHotKey(g_main,HOTKEY_BASE+i);detach_pad_audio(i);spin_lock();g_pads[i].name[0]=0;g_pads[i].vk=0;g_pads[i].mods=0;g_pads[i].volume=1.0f;g_pads[i].selStart=0.0f;g_pads[i].selEnd=1.0f;spin_unlock();__atomic_store_n(&g_pads[i].volumeHalfPct,200,__ATOMIC_RELEASE);__atomic_store_n(&g_pads[i].pitchHalf,0,__ATOMIC_RELEASE);g_hotkeyState[i]=0;save_config();update_pad_button(i);update_selected_text();update_name_edit();update_file_info();invalidate_wave();invalidate_controls();update_diagnostics();set_status_ascii("Pad reset to defaults.");
}

static void relocate_missing_sounds(void){int missing=0;for(int i=0;i<PAD_COUNT;i++)if(__atomic_load_n(&g_pads[i].loadState,__ATOMIC_RELAXED)==PAD_MISSING)missing++;if(!missing){g_api.MessageBoxW(g_main,(const WL_WCHAR[]){'N','o',' ','m','i','s','s','i','n','g',' ','s','o','u','n','d','s',' ','w','e','r','e',' ','d','e','t','e','c','t','e','d','.',0},WINDOW_TITLE,0x40);return;}WL_WCHAR picked[MAX_PATH_W];if(!pick_audio_file(picked))return;WL_SIZE_T slash=0,len=wl_wlen(picked);for(WL_SIZE_T i=0;i<len;i++)if(picked[i]=='\\'||picked[i]=='/')slash=i+1;if(!slash)return;WL_WCHAR folder[MAX_PATH_W];if(slash>=MAX_PATH_W)slash=MAX_PATH_W-1;for(WL_SIZE_T i=0;i<slash;i++)folder[i]=picked[i];folder[slash]=0;int found=0;for(int i=0;i<PAD_COUNT;i++){if(__atomic_load_n(&g_pads[i].loadState,__ATOMIC_RELAXED)!=PAD_MISSING)continue;WL_WCHAR old[MAX_PATH_W],candidate[MAX_PATH_W];spin_lock();wl_wcpy(old,g_pads[i].path,MAX_PATH_W);spin_unlock();const WL_WCHAR*b=base_name(old);wl_wcpy(candidate,folder,MAX_PATH_W);wl_wcat(candidate,b,MAX_PATH_W);if(path_exists(candidate)){spin_lock();wl_wcpy(g_pads[i].path,candidate,MAX_PATH_W);spin_unlock();if(schedule_load_pad(i,candidate,1)){found++;update_pad_button(i);}}}if(found){save_config();set_status_ascii("Relocating missing sounds from the selected folder in the background.");}else g_api.MessageBoxW(g_main,(const WL_WCHAR[]){'N','o',' ','m','a','t','c','h','i','n','g',' ','m','i','s','s','i','n','g',' ','f','i','l','e','n','a','m','e','s',' ','w','e','r','e',' ','f','o','u','n','d',' ','i','n',' ','t','h','a','t',' ','f','o','l','d','e','r','.',0},WINDOW_TITLE,0x30);}

/* Pipe audio server. */
static int monitor_wasapi_api_ready(void){
 if(pMonCoCreateInstance&&pMonCoInitializeEx&&pMonCoUninitialize&&pMonCoTaskMemFree&&pCreateEventW)return 1;
 if(!g_monitorOle32){static const WL_WCHAR n[]={'o','l','e','3','2','.','d','l','l',0};g_monitorOle32=g_api.LoadLibraryW(n);}
 if(!g_monitorOle32)return 0;
 pMonCoInitializeEx=(PFN_MonCoInitializeEx)wl_get_proc(g_monitorOle32,"CoInitializeEx");
 pMonCoUninitialize=(PFN_MonCoUninitialize)wl_get_proc(g_monitorOle32,"CoUninitialize");
 pMonCoCreateInstance=(PFN_CoCreateInstance)wl_get_proc(g_monitorOle32,"CoCreateInstance");
 pMonCoTaskMemFree=(PFN_CoTaskMemFree)wl_get_proc(g_monitorOle32,"CoTaskMemFree");
 pMonPropVariantClear=(PFN_PropVariantClear)wl_get_proc(g_monitorOle32,"PropVariantClear");
 if(!g_avrt){static const WL_WCHAR n[]={'a','v','r','t','.','d','l','l',0};g_avrt=g_api.LoadLibraryW(n);}
 if(g_avrt){pAvSetMmThreadCharacteristicsW=(PFN_AvSetMmThreadCharacteristicsW)wl_get_proc(g_avrt,"AvSetMmThreadCharacteristicsW");pAvSetMmThreadPriority=(PFN_AvSetMmThreadPriority)wl_get_proc(g_avrt,"AvSetMmThreadPriority");pAvRevertMmThreadCharacteristics=(PFN_AvRevertMmThreadCharacteristics)wl_get_proc(g_avrt,"AvRevertMmThreadCharacteristics");}
 return pMonCoInitializeEx&&pMonCoUninitialize&&pMonCoCreateInstance&&pMonCoTaskMemFree&&pCreateEventW;
}
static int monitor_populate_device_array(void);
static void monitor_populate_devices_wasapi(void);
static void monitor_device_changed_wasapi(void);
typedef struct { MON_IMMDeviceEnumerator*en;MON_IMMDevice*dev;MON_IAudioClient*client;MON_IAudioRenderClient*render;MON_WAVEFORMATEX*fmt;WL_HANDLE event;WL_UINT bufferFrames;WL_U32 rate;WL_U16 channels,bits,tag,bytesPerSample; WL_I32 eventDriven; } MonitorWasapi;

static int monitor_wave_api_ready(void){
 if(pWaveOutOpen&&pWaveOutPrepareHeader&&pWaveOutUnprepareHeader&&pWaveOutWrite&&pWaveOutReset&&pWaveOutClose)return 1;
 if(!g_winmm){static const WL_WCHAR n[]={'w','i','n','m','m','.','d','l','l',0};g_winmm=g_api.LoadLibraryW(n);}
 if(!g_winmm)return 0;
 pWaveOutOpen=(PFN_waveOutOpen)wl_get_proc(g_winmm,"waveOutOpen");
 pWaveOutPrepareHeader=(PFN_waveOutPrepareHeader)wl_get_proc(g_winmm,"waveOutPrepareHeader");
 pWaveOutUnprepareHeader=(PFN_waveOutUnprepareHeader)wl_get_proc(g_winmm,"waveOutUnprepareHeader");
 pWaveOutWrite=(PFN_waveOutWrite)wl_get_proc(g_winmm,"waveOutWrite");
 pWaveOutReset=(PFN_waveOutReset)wl_get_proc(g_winmm,"waveOutReset");
 pWaveOutClose=(PFN_waveOutClose)wl_get_proc(g_winmm,"waveOutClose");
 pWaveOutGetNumDevs=(PFN_waveOutGetNumDevs)wl_get_proc(g_winmm,"waveOutGetNumDevs");
 pWaveOutGetDevCapsW=(PFN_waveOutGetDevCapsW)wl_get_proc(g_winmm,"waveOutGetDevCapsW");
 return pWaveOutOpen&&pWaveOutPrepareHeader&&pWaveOutUnprepareHeader&&pWaveOutWrite&&pWaveOutReset&&pWaveOutClose;
}
static void monitor_wave_close_device(void){
 void*h=g_monitorWave;if(!h)return;if(pWaveOutReset)pWaveOutReset(h);
 for(int i=0;i<MONITOR_BUFFERS;i++){MonitorWaveBuffer*b=&g_monitorWaveBuffers[i];if(b->prepared&&pWaveOutUnprepareHeader){for(int tries=0;tries<20;tries++){if(pWaveOutUnprepareHeader(h,&b->hdr,sizeof(b->hdr))==0){b->prepared=0;break;}g_api.Sleep(1);}}b->prepared=0;memset(&b->hdr,0,sizeof(b->hdr));}
 if(pWaveOutClose)pWaveOutClose(h);g_monitorWave=0;g_monitorWaveRate=0;__atomic_store_n(&g_monitorDeviceRate,0,__ATOMIC_RELEASE);__atomic_store_n(&g_monitorBufferMs,0,__ATOMIC_RELEASE);__atomic_store_n(&g_monitorBackendMode,0,__ATOMIC_RELEASE);
}
static int monitor_wave_open_device(WL_U32 sampleRate){
 if(g_monitorWave&&g_monitorWaveRate==sampleRate)return 1;monitor_wave_close_device();if(!monitor_wave_api_ready()||sampleRate<8000||sampleRate>384000)return 0;
 MON_WAVEFORMATEX f;memset(&f,0,sizeof(f));f.wFormatTag=WAVE_FORMAT_PCM;f.nChannels=2;f.nSamplesPerSec=sampleRate;f.wBitsPerSample=16;f.nBlockAlign=4;f.nAvgBytesPerSec=sampleRate*4u;f.cbSize=0;
 void*h=0;if(pWaveOutOpen(&h,g_monitorWaveDeviceId,&f,0,0,0)!=0||!h){if(g_monitorWaveDeviceId!=WAVE_MAPPER&&pWaveOutOpen(&h,WAVE_MAPPER,&f,0,0,0)==0&&h){g_monitorWaveDeviceId=WAVE_MAPPER;g_monitorDeviceName[0]=0;}else return 0;}
 g_monitorWave=h;g_monitorWaveRate=sampleRate;for(int i=0;i<MONITOR_BUFFERS;i++)memset(&g_monitorWaveBuffers[i],0,sizeof(g_monitorWaveBuffers[i]));__atomic_store_n(&g_monitorDeviceRate,sampleRate,__ATOMIC_RELEASE);__atomic_store_n(&g_monitorBackendMode,3,__ATOMIC_RELEASE);return 1;
}
static WL_I16 monitor_wave_s16(float x){if(x>1.0f)x=1.0f;else if(x<-1.0f)x=-1.0f;return x>=0?(WL_I16)(x*32767.0f+0.5f):(WL_I16)(x*32768.0f-0.5f);}
static void monitor_populate_devices(void){
 if(!g_monitorDeviceCombo)return;if(!monitor_wave_api_ready()){monitor_populate_devices_wasapi();return;}g_api.SendMessageW(g_monitorDeviceCombo,CB_RESETCONTENT,0,0);
 static const WL_WCHAR defName[]={'W','i','n','d','o','w','s',' ','D','e','f','a','u','l','t',0};WL_LRESULT idx=g_api.SendMessageW(g_monitorDeviceCombo,CB_ADDSTRING,0,(WL_LPARAM)defName);if(idx>=0)g_api.SendMessageW(g_monitorDeviceCombo,CB_SETITEMDATA,(WL_WPARAM)idx,(WL_LPARAM)WAVE_MAPPER);
 WL_I32 selected=0;WL_U32 count=pWaveOutGetNumDevs?pWaveOutGetNumDevs():0;for(WL_U32 i=0;i<count;i++){MON_WAVEOUTCAPSW caps;memset(&caps,0,sizeof(caps));if(!pWaveOutGetDevCapsW||pWaveOutGetDevCapsW((WL_UPTR)i,&caps,sizeof(caps))!=0)continue;WL_LRESULT j=g_api.SendMessageW(g_monitorDeviceCombo,CB_ADDSTRING,0,(WL_LPARAM)caps.szPname);if(j>=0){g_api.SendMessageW(g_monitorDeviceCombo,CB_SETITEMDATA,(WL_WPARAM)j,(WL_LPARAM)i);if(g_monitorDeviceName[0]&&weq(g_monitorDeviceName,caps.szPname))selected=(WL_I32)j;}}
 g_api.SendMessageW(g_monitorDeviceCombo,CB_SETCURSEL,(WL_WPARAM)selected,0);if(selected==0){g_monitorWaveDeviceId=WAVE_MAPPER;g_monitorDeviceName[0]=0;g_monitorDeviceId[0]=0;}else{WL_LRESULT d=g_api.SendMessageW(g_monitorDeviceCombo,CB_GETITEMDATA,(WL_WPARAM)selected,0);g_monitorWaveDeviceId=(WL_UPTR)d;MON_WAVEOUTCAPSW caps;memset(&caps,0,sizeof(caps));if(pWaveOutGetDevCapsW&&pWaveOutGetDevCapsW(g_monitorWaveDeviceId,&caps,sizeof(caps))==0)wl_wcpy(g_monitorDeviceName,caps.szPname,MONITOR_DEVICE_NAME_CHARS);g_monitorDeviceId[0]=0;}
}
static void monitor_device_changed(void){
 if(!g_monitorDeviceCombo)return;if(!monitor_wave_api_ready()){monitor_device_changed_wasapi();return;}WL_LRESULT sel=g_api.SendMessageW(g_monitorDeviceCombo,CB_GETCURSEL,0,0);if(sel<0)return;if(sel==0){g_monitorWaveDeviceId=WAVE_MAPPER;g_monitorDeviceName[0]=0;g_monitorDeviceId[0]=0;}else{WL_LRESULT d=g_api.SendMessageW(g_monitorDeviceCombo,CB_GETITEMDATA,(WL_WPARAM)sel,0);g_monitorWaveDeviceId=(WL_UPTR)d;MON_WAVEOUTCAPSW caps;memset(&caps,0,sizeof(caps));if(pWaveOutGetDevCapsW&&pWaveOutGetDevCapsW(g_monitorWaveDeviceId,&caps,sizeof(caps))==0)wl_wcpy(g_monitorDeviceName,caps.szPname,MONITOR_DEVICE_NAME_CHARS);else g_monitorDeviceName[0]=0;g_monitorDeviceId[0]=0;}__atomic_store_n(&g_monitorReopen,1,__ATOMIC_RELEASE);monitor_wave_close_device();save_config();set_status_ascii("Monitor output changed. The compatibility playback path will use this device on the next sound.");
}
static void monitor_populate_devices_wasapi(void){
 if(!g_monitorDeviceCombo)return;monitor_populate_device_array();g_api.SendMessageW(g_monitorDeviceCombo,CB_RESETCONTENT,0,0);int selected=0;
 for(int i=0;i<g_monitorDeviceCount;i++){WL_LRESULT x=g_api.SendMessageW(g_monitorDeviceCombo,CB_ADDSTRING,0,(WL_LPARAM)g_monitorDevices[i].name);if(x>=0)g_api.SendMessageW(g_monitorDeviceCombo,CB_SETITEMDATA,(WL_WPARAM)x,(WL_LPARAM)i);if(i>0){if(g_monitorDeviceId[0]&&weq(g_monitorDeviceId,g_monitorDevices[i].id))selected=i;else if(!g_monitorDeviceId[0]&&g_monitorDeviceName[0]&&weq(g_monitorDeviceName,g_monitorDevices[i].name)){selected=i;wl_wcpy(g_monitorDeviceId,g_monitorDevices[i].id,MONITOR_DEVICE_ID_CHARS);}}}
 if(selected<=0){selected=0;if(g_monitorDeviceId[0]){g_monitorDeviceId[0]=0;g_monitorDeviceName[0]=0;}}
 g_api.SendMessageW(g_monitorDeviceCombo,CB_SETCURSEL,(WL_WPARAM)selected,0);
}
static void monitor_device_changed_wasapi(void){
 if(!g_monitorDeviceCombo)return;WL_LRESULT sel=g_api.SendMessageW(g_monitorDeviceCombo,CB_GETCURSEL,0,0);if(sel<0)return;WL_LRESULT item=g_api.SendMessageW(g_monitorDeviceCombo,CB_GETITEMDATA,(WL_WPARAM)sel,0);int idx=(int)item;if(idx<0||idx>=g_monitorDeviceCount)idx=0;spin_lock();wl_wcpy(g_monitorDeviceName,g_monitorDevices[idx].name,MONITOR_DEVICE_NAME_CHARS);wl_wcpy(g_monitorDeviceId,g_monitorDevices[idx].id,MONITOR_DEVICE_ID_CHARS);if(idx==0){g_monitorDeviceName[0]=0;g_monitorDeviceId[0]=0;}spin_unlock();__atomic_store_n(&g_monitorReopen,1,__ATOMIC_RELEASE);if(pSetEvent){WL_HANDLE e=(WL_HANDLE)(WL_UPTR)__atomic_load_n((volatile WL_UPTR*)&g_monitorEventHandle,__ATOMIC_ACQUIRE);if(e)pSetEvent(e);}save_config();set_status_ascii("Monitor output device changed. WASAPI will reopen it without touching the microphone stream.");
}
static void monitor_ring_reset(WL_U32 sampleRate){
 __atomic_store_n(&g_monitorSourceRate,sampleRate,__ATOMIC_RELEASE);__atomic_store_n(&g_monitorWriteFrame,0,__ATOMIC_RELEASE);__atomic_store_n(&g_monitorReadFrame,0,__ATOMIC_RELEASE);__atomic_store_n(&g_monitorBufferMs,0,__ATOMIC_RELEASE);__atomic_store_n(&g_monitorDiscValid,0,__ATOMIC_RELEASE);__atomic_add_fetch(&g_monitorEpoch,1,__ATOMIC_ACQ_REL);
}
static void monitor_device_name(MON_IMMDevice*d,WL_WCHAR*out,WL_SIZE_T cap){
 if(cap)out[0]=0;if(!d||!out||!cap)return;MON_IPropertyStore*ps=0;if(d->lpVtbl->OpenPropertyStore(d,MON_STGM_READ,&ps)<0||!ps)return;MON_PROPVARIANT pv;memset(&pv,0,sizeof(pv));if(ps->lpVtbl->GetValue(ps,&MON_PKEY_Device_FriendlyName,&pv)>=0&&pv.vt==MON_VT_LPWSTR&&pv.u.pwszVal)wl_wcpy(out,pv.u.pwszVal,cap);if(pMonPropVariantClear)pMonPropVariantClear(&pv);ps->lpVtbl->Release(ps);
}
static int monitor_populate_device_array(void){
 g_monitorDeviceCount=0;memset(g_monitorDevices,0,sizeof(g_monitorDevices));wl_wcpy(g_monitorDevices[0].name,(const WL_WCHAR[]){'W','i','n','d','o','w','s',' ','D','e','f','a','u','l','t',0},MONITOR_DEVICE_NAME_CHARS);g_monitorDeviceCount=1;
 if(!monitor_wasapi_api_ready())return 0;int co=0;MON_HRESULT hr=pMonCoInitializeEx(0,MON_COINIT_MULTITHREADED);if(hr>=0)co=1;MON_IMMDeviceEnumerator*en=0;if(pMonCoCreateInstance(&MON_CLSID_MMDeviceEnumerator,0,MON_CLSCTX_ALL,&MON_IID_IMMDeviceEnumerator,(void**)&en)<0||!en){if(co)pMonCoUninitialize();return 0;}MON_IMMDeviceCollection*col=0;if(en->lpVtbl->EnumAudioEndpoints(en,MON_ERENDER,MON_DEVICE_STATE_ACTIVE,&col)>=0&&col){WL_UINT count=0;if(col->lpVtbl->GetCount(col,&count)>=0){for(WL_UINT i=0;i<count&&g_monitorDeviceCount<MONITOR_DEVICE_MAX;i++){MON_IMMDevice*d=0;if(col->lpVtbl->Item(col,i,&d)<0||!d)continue;WL_WCHAR*id=0;WL_WCHAR name[MONITOR_DEVICE_NAME_CHARS];name[0]=0;monitor_device_name(d,name,MONITOR_DEVICE_NAME_CHARS);if(d->lpVtbl->GetId(d,&id)>=0&&id&&name[0]){MonitorDeviceInfo*mi=&g_monitorDevices[g_monitorDeviceCount++];wl_wcpy(mi->name,name,MONITOR_DEVICE_NAME_CHARS);wl_wcpy(mi->id,id,MONITOR_DEVICE_ID_CHARS);}if(id)pMonCoTaskMemFree(id);d->lpVtbl->Release(d);}}col->lpVtbl->Release(col);}en->lpVtbl->Release(en);if(co)pMonCoUninitialize();return g_monitorDeviceCount>0;
}
static void monitor_close_wasapi(MonitorWasapi*m){if(!m)return;if(m->client){m->client->lpVtbl->Stop(m->client);m->client->lpVtbl->Reset(m->client);}if(m->render)m->render->lpVtbl->Release(m->render);if(m->client)m->client->lpVtbl->Release(m->client);if(m->dev)m->dev->lpVtbl->Release(m->dev);if(m->en)m->en->lpVtbl->Release(m->en);if(m->fmt&&pMonCoTaskMemFree)pMonCoTaskMemFree(m->fmt);if(m->event)g_api.CloseHandle(m->event);memset(m,0,sizeof(*m));__atomic_store_n(&g_monitorDeviceRate,0,__ATOMIC_RELEASE);__atomic_store_n(&g_monitorDeviceLatencyMs,0,__ATOMIC_RELEASE);__atomic_store_n(&g_monitorBackendMode,0,__ATOMIC_RELEASE);__atomic_store_n((volatile WL_UPTR*)&g_monitorEventHandle,0,__ATOMIC_RELEASE);}
static int monitor_format_info(MonitorWasapi*m){if(!m||!m->fmt||!m->fmt->nChannels||!m->fmt->nSamplesPerSec||m->fmt->nChannels>16)return 0;WL_U16 tag=m->fmt->wFormatTag;if(tag==MON_WAVE_FORMAT_EXTENSIBLE&&m->fmt->cbSize>=22){const MON_GUID*sub=(const MON_GUID*)((const WL_U8*)m->fmt+24);tag=(WL_U16)sub->Data1;}WL_U16 bps=m->fmt->nBlockAlign/m->fmt->nChannels;if((tag==MON_WAVE_FORMAT_IEEE_FLOAT&&bps==4&&m->fmt->wBitsPerSample==32)||(tag==MON_WAVE_FORMAT_PCM&&(bps==2||bps==3||bps==4))){m->tag=tag;m->rate=m->fmt->nSamplesPerSec;m->channels=m->fmt->nChannels;m->bits=m->fmt->wBitsPerSample;m->bytesPerSample=bps;return 1;}return 0;}
static int monitor_open_wasapi(MonitorWasapi*m){
 monitor_close_wasapi(m);if(!monitor_wasapi_api_ready())return 0;
 if(pMonCoCreateInstance(&MON_CLSID_MMDeviceEnumerator,0,MON_CLSCTX_ALL,&MON_IID_IMMDeviceEnumerator,(void**)&m->en)<0||!m->en)return 0;
 WL_WCHAR id[MONITOR_DEVICE_ID_CHARS];spin_lock();wl_wcpy(id,g_monitorDeviceId,MONITOR_DEVICE_ID_CHARS);spin_unlock();
 MON_HRESULT hr=id[0]?m->en->lpVtbl->GetDevice(m->en,id,&m->dev):m->en->lpVtbl->GetDefaultAudioEndpoint(m->en,MON_ERENDER,MON_ECONSOLE,&m->dev);if((hr<0||!m->dev)&&!id[0])hr=m->en->lpVtbl->GetDefaultAudioEndpoint(m->en,MON_ERENDER,MON_EMULTIMEDIA,&m->dev);if(hr<0||!m->dev)return 0;
 if(m->dev->lpVtbl->Activate(m->dev,&MON_IID_IAudioClient,MON_CLSCTX_ALL,0,(void**)&m->client)<0||!m->client)return 0;
 if(m->client->lpVtbl->GetMixFormat(m->client,&m->fmt)<0||!m->fmt||!monitor_format_info(m))return 0;
 WL_I64 duration=100*MON_HNS_PER_MS;m->eventDriven=0;
 /* Prefer event-driven shared mode. Some real devices/drivers reject the
    EVENTCALLBACK flag even though ordinary shared-mode playback works. If
    that happens, reactivate a fresh IAudioClient and fall back to timer-
    polled shared mode instead of disabling monitoring completely. */
 if(pCreateEventW){
  m->event=pCreateEventW(0,WL_FALSE,WL_FALSE,0);
  if(m->event&&m->client->lpVtbl->Initialize(m->client,MON_AUDCLNT_SHAREMODE_SHARED,MON_AUDCLNT_STREAMFLAGS_EVENTCALLBACK,duration,0,m->fmt,0)>=0&&m->client->lpVtbl->SetEventHandle(m->client,m->event)>=0)m->eventDriven=1;
 }
 if(!m->eventDriven){
  if(m->event){g_api.CloseHandle(m->event);m->event=0;}
  if(m->client){m->client->lpVtbl->Release(m->client);m->client=0;}
  if(m->dev->lpVtbl->Activate(m->dev,&MON_IID_IAudioClient,MON_CLSCTX_ALL,0,(void**)&m->client)<0||!m->client)return 0;
  if(m->client->lpVtbl->Initialize(m->client,MON_AUDCLNT_SHAREMODE_SHARED,0,duration,0,m->fmt,0)<0)return 0;
 }
 if(m->client->lpVtbl->GetBufferSize(m->client,&m->bufferFrames)<0||!m->bufferFrames)return 0;
 if(m->client->lpVtbl->GetService(m->client,&MON_IID_IAudioRenderClient,(void**)&m->render)<0||!m->render)return 0;
 WL_I64 lat=0;if(m->client->lpVtbl->GetStreamLatency(m->client,&lat)>=0&&lat>0)__atomic_store_n(&g_monitorDeviceLatencyMs,(WL_U32)(lat/MON_HNS_PER_MS),__ATOMIC_RELEASE);
 /* Start with a fully silent device buffer to avoid a startup click. */
 WL_U8*buf=0;if(m->render->lpVtbl->GetBuffer(m->render,m->bufferFrames,&buf)>=0&&buf)m->render->lpVtbl->ReleaseBuffer(m->render,m->bufferFrames,MON_AUDCLNT_BUFFERFLAGS_SILENT);
 if(m->client->lpVtbl->Start(m->client)<0)return 0;
 __atomic_store_n(&g_monitorDeviceRate,m->rate,__ATOMIC_RELEASE);__atomic_store_n(&g_monitorBackendMode,m->eventDriven?1:2,__ATOMIC_RELEASE);__atomic_store_n((volatile WL_UPTR*)&g_monitorEventHandle,(WL_UPTR)(m->eventDriven?m->event:0),__ATOMIC_RELEASE);return 1;
}
static float monitor_clampf(float x){return x<-1.0f?-1.0f:(x>1.0f?1.0f:x);}
static void monitor_store_frame(MonitorWasapi*m,WL_U8*base,WL_UINT frame,float sample){sample=monitor_clampf(sample*((float)__atomic_load_n(&g_monitorVolumeHalfPct,__ATOMIC_RELAXED)/200.0f));WL_U8*p=base+(WL_SIZE_T)frame*m->fmt->nBlockAlign;for(WL_U16 c=0;c<m->channels;c++){float v=(c<2)?sample:0.0f;WL_U8*q=p+(WL_SIZE_T)c*m->bytesPerSample;if(m->tag==MON_WAVE_FORMAT_IEEE_FLOAT){*(float*)q=v;}else if(m->bytesPerSample==2){WL_I32 z=(WL_I32)(v>=0?v*32767.0f+0.5f:v*32768.0f-0.5f);q[0]=(WL_U8)z;q[1]=(WL_U8)(z>>8);}else if(m->bytesPerSample==3){WL_I32 z=(WL_I32)(v>=0?v*8388607.0f+0.5f:v*8388608.0f-0.5f);q[0]=(WL_U8)z;q[1]=(WL_U8)(z>>8);q[2]=(WL_U8)(z>>16);}else{WL_I64 z=(WL_I64)(v>=0?v*2147483647.0f+0.5f:v*2147483648.0f-0.5f);q[0]=(WL_U8)z;q[1]=(WL_U8)(z>>8);q[2]=(WL_U8)(z>>16);q[3]=(WL_U8)(z>>24);}}}
static WL_U64 monitor_target_frames(WL_U32 sr){WL_U64 t=((WL_U64)sr*MONITOR_TARGET_MS)/1000ull;if(t<64)t=64;if(t>MONITOR_RING_FRAMES/3u)t=MONITOR_RING_FRAMES/3u;return t;}
static WL_DWORD WL_CALLBACK monitor_thread_proc(void*ctx){
 (void)ctx;if(!monitor_wasapi_api_ready()){__atomic_store_n(&g_monitorThreadStarted,0,__ATOMIC_RELEASE);return 0;}MON_HRESULT chr=pMonCoInitializeEx(0,MON_COINIT_MULTITHREADED);int co=chr>=0;WL_DWORD taskIndex=0;WL_HANDLE mmcss=0;if(pAvSetMmThreadCharacteristicsW){static const WL_WCHAR task[]={'P','r','o',' ','A','u','d','i','o',0};mmcss=pAvSetMmThreadCharacteristicsW(task,&taskIndex);if(mmcss&&pAvSetMmThreadPriority)pAvSetMmThreadPriority(mmcss,1);}MonitorWasapi m;memset(&m,0,sizeof(m));WL_U32 localEpoch=0;double pos=0.0;int primed=0;float lastOut=0.0f,crossStart=0.0f;WL_U32 crossRemain=0,crossTotal=1;WL_U64 lastWake=0;WL_U32 testPhase=0;
 while(g_running){if(!__atomic_load_n(&g_monitorEnabled,__ATOMIC_ACQUIRE)){if(m.en||m.dev||m.client||m.render||m.fmt||m.event)monitor_close_wasapi(&m);primed=0;lastOut=0;g_api.Sleep(30);continue;}if(__atomic_exchange_n(&g_monitorReopen,0,__ATOMIC_ACQ_REL)||!m.client){if(!monitor_open_wasapi(&m)){__atomic_add_fetch(&g_monitorErrors,1,__ATOMIC_RELAXED);__atomic_store_n(&g_monitorEnabled,0,__ATOMIC_RELEASE);if(g_main)g_api.PostMessageW(g_main,WM_MONITOR_STATUS,1,0);g_api.Sleep(100);continue;}primed=0;localEpoch=0;lastOut=0;}
  if(m.eventDriven){WL_DWORD wait=g_api.WaitForSingleObject(m.event,100);if(wait==MON_WAIT_TIMEOUT)continue;if(wait!=0){__atomic_add_fetch(&g_monitorErrors,1,__ATOMIC_RELAXED);__atomic_store_n(&g_monitorReopen,1,__ATOMIC_RELEASE);continue;}}else g_api.Sleep(5);WL_U64 now=pGetTickCount64?pGetTickCount64():0;if(now&&lastWake&&now-lastWake>__atomic_load_n(&g_monitorMaxWakeMs,__ATOMIC_RELAXED)){WL_U32 gap=(WL_U32)(now-lastWake);__atomic_store_n(&g_monitorMaxWakeMs,gap,__ATOMIC_RELAXED);}if(now)lastWake=now;
  WL_UINT padding=0;if(m.client->lpVtbl->GetCurrentPadding(m.client,&padding)<0||padding>m.bufferFrames){__atomic_add_fetch(&g_monitorErrors,1,__ATOMIC_RELAXED);__atomic_store_n(&g_monitorReopen,1,__ATOMIC_RELEASE);continue;}WL_UINT avail=m.bufferFrames-padding;if(!avail)continue;WL_U8*out=0;if(m.render->lpVtbl->GetBuffer(m.render,avail,&out)<0||!out){__atomic_add_fetch(&g_monitorErrors,1,__ATOMIC_RELAXED);continue;}
  WL_U32 epoch=__atomic_load_n(&g_monitorEpoch,__ATOMIC_ACQUIRE);if(epoch!=localEpoch){localEpoch=epoch;pos=(double)__atomic_load_n(&g_monitorReadFrame,__ATOMIC_ACQUIRE);primed=0;lastOut=0;crossRemain=0;}
  WL_U32 sr=__atomic_load_n(&g_monitorSourceRate,__ATOMIC_ACQUIRE);WL_U64 write=__atomic_load_n(&g_monitorWriteFrame,__ATOMIC_ACQUIRE);WL_U64 read=(WL_U64)pos;if(write<read){pos=(double)write;read=write;}WL_U64 fill=write-read;WL_U64 target=sr?monitor_target_frames(sr):0;if(sr)__atomic_store_n(&g_monitorBufferMs,(WL_U32)((fill*1000ull)/(WL_U64)sr),__ATOMIC_RELAXED);else __atomic_store_n(&g_monitorBufferMs,0,__ATOMIC_RELAXED);
  if(target&&fill>target*3u){WL_U64 nr=write-target;pos=(double)nr;fill=target;__atomic_add_fetch(&g_monitorCorrections,1,__ATOMIC_RELAXED);crossStart=lastOut;crossTotal=crossRemain=(m.rate*MONITOR_CROSSFADE_MS)/1000u;if(!crossTotal)crossTotal=crossRemain=1;}
  if(!primed){if(target&&fill>=target){primed=1;crossStart=0;crossTotal=crossRemain=(m.rate*MONITOR_CROSSFADE_MS)/1000u;if(!crossTotal)crossTotal=crossRemain=1;}else if(!__atomic_load_n(&g_monitorSourceActive,__ATOMIC_ACQUIRE)&&fill>=2){primed=1;crossStart=0;crossTotal=crossRemain=(m.rate*MONITOR_CROSSFADE_MS)/1000u;if(!crossTotal)crossTotal=crossRemain=1;}}
  double baseStep=(sr&&m.rate)?((double)sr/(double)m.rate):1.0;double corr=0.0;if(primed&&target){double e=((double)fill-(double)target)/(double)target;corr=e*0.0015;if(corr>0.0025)corr=0.0025;if(corr<-0.0025)corr=-0.0025;}__atomic_store_n(&g_monitorDriftPpm,(WL_I32)(corr*1000000.0),__ATOMIC_RELAXED);double step=baseStep*(1.0+corr);int starved=0;
  for(WL_UINT f=0;f<avail;f++){float v=0.0f;if(primed&&sr){write=__atomic_load_n(&g_monitorWriteFrame,__ATOMIC_ACQUIRE);WL_U64 i0=(WL_U64)pos;if(i0+1<write){WL_U64 disc=__atomic_load_n(&g_monitorDiscAt,__ATOMIC_ACQUIRE);if(__atomic_load_n(&g_monitorDiscValid,__ATOMIC_ACQUIRE)&&i0>=disc){__atomic_store_n(&g_monitorDiscValid,0,__ATOMIC_RELEASE);__atomic_add_fetch(&g_monitorCorrections,1,__ATOMIC_RELAXED);crossStart=lastOut;crossTotal=crossRemain=(m.rate*MONITOR_CROSSFADE_MS)/1000u;if(!crossTotal)crossTotal=crossRemain=1;}float a=g_monitorRing[i0&(MONITOR_RING_FRAMES-1u)],b=g_monitorRing[(i0+1u)&(MONITOR_RING_FRAMES-1u)];float frac=(float)(pos-(double)i0);v=a+(b-a)*frac;pos+=step;}else{starved=1;primed=0;crossStart=lastOut;crossTotal=crossRemain=(m.rate*MONITOR_CROSSFADE_MS)/1000u;if(!crossTotal)crossTotal=crossRemain=1;v=0.0f;}}WL_U32 tf=__atomic_load_n(&g_monitorTestFrames,__ATOMIC_RELAXED);if(tf){testPhase+=440u;if(testPhase>=m.rate)testPhase-=m.rate;float q=(float)testPhase/(float)(m.rate?m.rate:48000u);float tri=q<0.5f?(q*4.0f-1.0f):(3.0f-q*4.0f);v+=tri*0.18f;__atomic_sub_fetch(&g_monitorTestFrames,1,__ATOMIC_RELAXED);}if(crossRemain){WL_U32 done=crossTotal-crossRemain;float t=(float)(done+1u)/(float)crossTotal;v=crossStart+(v-crossStart)*t;crossRemain--;}monitor_store_frame(&m,out,f,v);lastOut=v;}
  __atomic_store_n(&g_monitorReadFrame,(WL_U64)pos,__ATOMIC_RELEASE);if(starved&&__atomic_load_n(&g_monitorSourceActive,__ATOMIC_ACQUIRE))__atomic_add_fetch(&g_monitorUnderruns,1,__ATOMIC_RELAXED);m.render->lpVtbl->ReleaseBuffer(m.render,avail,0);
 }
 monitor_close_wasapi(&m);if(mmcss&&pAvRevertMmThreadCharacteristics)pAvRevertMmThreadCharacteristics(mmcss);if(co)pMonCoUninitialize();__atomic_store_n(&g_monitorThreadStarted,0,__ATOMIC_RELEASE);return 0;
}
static int monitor_ensure_thread(void){WL_I32 expect=0;if(__atomic_load_n(&g_monitorThreadStarted,__ATOMIC_ACQUIRE))return 1;if(!__atomic_compare_exchange_n(&g_monitorThreadStarted,&expect,1,0,__ATOMIC_ACQ_REL,__ATOMIC_ACQUIRE))return 1;WL_HANDLE th=g_api.CreateThread(0,0,monitor_thread_proc,0,0,0);if(!th){__atomic_store_n(&g_monitorThreadStarted,0,__ATOMIC_RELEASE);return 0;}g_monitorThread=th;return 1;}
static void update_monitor_volume_text(void){if(!g_monitorVolumeText)return;WL_WCHAR t[220];t[0]=0;wadd_ascii(t,220,"Monitor volume: ");wadd_half_units(t,220,__atomic_load_n(&g_monitorVolumeHalfPct,__ATOMIC_RELAXED),0);wadd_ascii(t,220,"% (local only)");set_text_if_changed(g_monitorVolumeText,t);}
static void update_monitor_button(void){if(!g_monitorButton)return;if(__atomic_load_n(&g_monitorEnabled,__ATOMIC_ACQUIRE))set_text_if_changed(g_monitorButton,(const WL_WCHAR[]){'M','o','n','i','t','o','r',':',' ','O','N',0});else set_text_if_changed(g_monitorButton,(const WL_WCHAR[]){'M','o','n','i','t','o','r',':',' ','O','F','F',0});}
static void monitor_set_enabled(int on){
 if(on){
  if(monitor_wave_api_ready()){
   __atomic_store_n(&g_monitorEnabled,1,__ATOMIC_RELEASE);__atomic_store_n(&g_monitorReopen,1,__ATOMIC_RELEASE);__atomic_store_n(&g_monitorBackendMode,3,__ATOMIC_RELEASE);update_monitor_button();set_status_ascii("Local soundboard monitor enabled using the v0.8.2-compatible Windows playback path. Headphones are recommended.");return;
  }
  if(!monitor_wasapi_api_ready()||!monitor_ensure_thread()){__atomic_add_fetch(&g_monitorErrors,1,__ATOMIC_RELAXED);set_status_ascii("Local monitor is unavailable because Windows audio output could not be initialized.");return;}
 }
 __atomic_store_n(&g_monitorEnabled,on?1:0,__ATOMIC_RELEASE);
 if(!on){__atomic_store_n(&g_monitorSourceActive,0,__ATOMIC_RELEASE);monitor_wave_close_device();monitor_ring_reset(__atomic_load_n(&g_monitorSourceRate,__ATOMIC_RELAXED));}
 __atomic_store_n(&g_monitorReopen,1,__ATOMIC_RELEASE);WL_HANDLE e=(WL_HANDLE)(WL_UPTR)__atomic_load_n((volatile WL_UPTR*)&g_monitorEventHandle,__ATOMIC_ACQUIRE);if(e&&pSetEvent)pSetEvent(e);update_monitor_button();if(on)set_status_ascii("Local soundboard monitor enabled. Windows compatibility playback is preferred; WASAPI remains a fallback.");else set_status_ascii("Local soundboard monitor disabled.");
}
static void monitor_process(void*owner,const float*audio,WL_U32 frames,WL_U32 sampleRate,int active){
 WL_UPTR me=(WL_UPTR)owner,cur=__atomic_load_n(&g_monitorOwner,__ATOMIC_ACQUIRE);if(!__atomic_load_n(&g_monitorEnabled,__ATOMIC_ACQUIRE))return;if(!cur){WL_UPTR expect=0;if(__atomic_compare_exchange_n(&g_monitorOwner,&expect,me,0,__ATOMIC_ACQ_REL,__ATOMIC_ACQUIRE))cur=me;else cur=expect;}if(cur!=me)return;__atomic_store_n(&g_monitorSourceActive,active?1:0,__ATOMIC_RELEASE);
 /* Preferred v0.8.2-compatible waveOut path. */
 if(monitor_wave_api_ready()){
  if(__atomic_exchange_n(&g_monitorReopen,0,__ATOMIC_ACQ_REL))monitor_wave_close_device();WL_U32 tf=__atomic_load_n(&g_monitorTestFrames,__ATOMIC_RELAXED);if((!active||!audio||!frames)&&!tf)return;if(sampleRate<8000||sampleRate>384000)sampleRate=48000;if(frames==0)frames=528;if(frames>MONITOR_MAX_FRAMES)frames=MONITOR_MAX_FRAMES;
  if(!monitor_wave_open_device(sampleRate)){__atomic_add_fetch(&g_monitorErrors,1,__ATOMIC_RELAXED);__atomic_store_n(&g_monitorEnabled,0,__ATOMIC_RELEASE);if(g_main)g_api.PostMessageW(g_main,WM_MONITOR_STATUS,1,0);monitor_wave_close_device();__atomic_store_n(&g_monitorOwner,0,__ATOMIC_RELEASE);return;}
  int slot=-1,busy=0;for(int i=0;i<MONITOR_BUFFERS;i++){MonitorWaveBuffer*b=&g_monitorWaveBuffers[i];if(b->prepared){if(b->hdr.dwFlags&WHDR_DONE){if(pWaveOutUnprepareHeader(g_monitorWave,&b->hdr,sizeof(b->hdr))==0){b->prepared=0;memset(&b->hdr,0,sizeof(b->hdr));}}else busy++;}if(slot<0&&!b->prepared)slot=i;}
  if(sampleRate)__atomic_store_n(&g_monitorBufferMs,(WL_U32)(((WL_U64)busy*(WL_U64)frames*1000ull)/(WL_U64)sampleRate),__ATOMIC_RELAXED);__atomic_store_n(&g_monitorDeviceRate,sampleRate,__ATOMIC_RELAXED);__atomic_store_n(&g_monitorDriftPpm,0,__ATOMIC_RELAXED);__atomic_store_n(&g_monitorDeviceLatencyMs,0,__ATOMIC_RELAXED);
  if(slot<0||busy>=4){__atomic_add_fetch(&g_monitorOverruns,1,__ATOMIC_RELAXED);return;}MonitorWaveBuffer*b=&g_monitorWaveBuffers[slot];float gain=(float)__atomic_load_n(&g_monitorVolumeHalfPct,__ATOMIC_RELAXED)/200.0f;
  for(WL_U32 i=0;i<frames;i++){float v=(active&&audio)?sanitize_audio_sample(audio[i*2]):0.0f;tf=__atomic_load_n(&g_monitorTestFrames,__ATOMIC_RELAXED);if(tf){g_monitorWaveTestPhase+=440u;if(g_monitorWaveTestPhase>=sampleRate)g_monitorWaveTestPhase-=sampleRate;float q=(float)g_monitorWaveTestPhase/(float)sampleRate;float tri=q<0.5f?(q*4.0f-1.0f):(3.0f-q*4.0f);v+=tri*0.18f;__atomic_sub_fetch(&g_monitorTestFrames,1,__ATOMIC_RELAXED);}WL_I16 z=monitor_wave_s16(v*gain);b->samples[i*2]=z;b->samples[i*2+1]=z;}
  memset(&b->hdr,0,sizeof(b->hdr));b->hdr.lpData=(char*)b->samples;b->hdr.dwBufferLength=frames*2u*sizeof(WL_I16);if(pWaveOutPrepareHeader(g_monitorWave,&b->hdr,sizeof(b->hdr))!=0){__atomic_add_fetch(&g_monitorErrors,1,__ATOMIC_RELAXED);return;}b->prepared=1;if(pWaveOutWrite(g_monitorWave,&b->hdr,sizeof(b->hdr))!=0){pWaveOutUnprepareHeader(g_monitorWave,&b->hdr,sizeof(b->hdr));b->prepared=0;__atomic_add_fetch(&g_monitorErrors,1,__ATOMIC_RELAXED);}return;
 }
 /* WASAPI fallback retained for systems where WinMM is unavailable. */
 if(!active||!audio||!frames)return;if(sampleRate<8000||sampleRate>384000)return;WL_U32 oldRate=__atomic_load_n(&g_monitorSourceRate,__ATOMIC_ACQUIRE);if(oldRate!=sampleRate)monitor_ring_reset(sampleRate);if(frames>=MONITOR_RING_FRAMES)frames=MONITOR_RING_FRAMES-2u;WL_U64 write=__atomic_load_n(&g_monitorWriteFrame,__ATOMIC_RELAXED),read=__atomic_load_n(&g_monitorReadFrame,__ATOMIC_ACQUIRE);WL_U64 fill=write>=read?write-read:0;if(fill+frames+2u>=MONITOR_RING_FRAMES){__atomic_add_fetch(&g_monitorOverruns,1,__ATOMIC_RELAXED);if(!__atomic_exchange_n(&g_monitorDiscValid,1,__ATOMIC_ACQ_REL))__atomic_store_n(&g_monitorDiscAt,write,__ATOMIC_RELEASE);return;}for(WL_U32 i=0;i<frames;i++)g_monitorRing[(write+i)&(MONITOR_RING_FRAMES-1u)]=sanitize_audio_sample(audio[i*2]);__atomic_store_n(&g_monitorWriteFrame,write+frames,__ATOMIC_RELEASE);
}
static void monitor_owner_disconnect(void*owner){WL_UPTR me=(WL_UPTR)owner;if(__atomic_load_n(&g_monitorOwner,__ATOMIC_ACQUIRE)==me){monitor_wave_close_device();__atomic_store_n(&g_monitorOwner,0,__ATOMIC_RELEASE);__atomic_store_n(&g_monitorSourceActive,0,__ATOMIC_RELEASE);monitor_ring_reset(__atomic_load_n(&g_monitorSourceRate,__ATOMIC_RELAXED));}}
static void monitor_shutdown(void){__atomic_store_n(&g_monitorEnabled,0,__ATOMIC_RELEASE);monitor_wave_close_device();WL_HANDLE e=(WL_HANDLE)(WL_UPTR)__atomic_load_n((volatile WL_UPTR*)&g_monitorEventHandle,__ATOMIC_ACQUIRE);if(e&&pSetEvent)pSetEvent(e);if(g_monitorThread){g_api.WaitForSingleObject(g_monitorThread,2000);g_api.CloseHandle(g_monitorThread);g_monitorThread=0;}if(g_winmm&&g_api.FreeLibrary)g_api.FreeLibrary((WL_HMODULE)g_winmm);g_winmm=0;pWaveOutOpen=0;pWaveOutPrepareHeader=0;pWaveOutUnprepareHeader=0;pWaveOutWrite=0;pWaveOutReset=0;pWaveOutClose=0;pWaveOutGetNumDevs=0;pWaveOutGetDevCapsW=0;if(g_avrt&&g_api.FreeLibrary)g_api.FreeLibrary((WL_HMODULE)g_avrt);if(g_monitorOle32&&g_api.FreeLibrary)g_api.FreeLibrary((WL_HMODULE)g_monitorOle32);g_avrt=g_monitorOle32=0;pMonCoInitializeEx=0;pMonCoUninitialize=0;pMonCoCreateInstance=0;pMonCoTaskMemFree=0;pMonPropVariantClear=0;pAvSetMmThreadCharacteristicsW=0;pAvSetMmThreadPriority=0;pAvRevertMmThreadCharacteristics=0;}

typedef struct { WL_I32 active,pad; Sample* sample; double start,pos,end,step,targetStep; float volume,targetVolume; WL_U64 age; } Voice;
typedef struct { WL_HANDLE pipe; WL_U32 sampleRate,lastUnderruns,lastClips; WL_I32 haveCounters; WL_I64 seen[PAD_COUNT]; WL_I64 seenPadStop[PAD_COUNT]; WL_I64 seenStop; WL_U64 nextAge; Voice voices[MAX_VOICES]; float audio[2048*2]; } Client;
static int pipe_read_exact(WL_HANDLE h,void*buf,WL_U32 bytes){WL_U8*d=(WL_U8*)buf;WL_U32 done=0;while(done<bytes&&g_running){WL_DWORD n=0;if(!g_api.ReadFile(h,d+done,bytes-done,&n,0)||!n)return 0;done+=n;}return done==bytes;}
static int pipe_write_exact(WL_HANDLE h,const void*buf,WL_U32 bytes){const WL_U8*s=(const WL_U8*)buf;WL_U32 done=0;while(done<bytes&&g_running){WL_DWORD n=0;if(!g_api.WriteFile(h,s+done,bytes-done,&n,0)||!n)return 0;done+=n;}return done==bytes;}
static float flush_tiny(float x){return (x<1.0e-20f&&x>-1.0e-20f)?0.0f:x;}
static void playing_inc(int pad){WL_I32 n=__atomic_add_fetch(&g_activeVoices[pad],1,__ATOMIC_ACQ_REL);if(n==1&&g_main)g_api.PostMessageW(g_main,WM_PLAY_STATE,(WL_WPARAM)pad,1);}
static void playing_dec(int pad){WL_I32 n=__atomic_sub_fetch(&g_activeVoices[pad],1,__ATOMIC_ACQ_REL);if(n<0){__atomic_store_n(&g_activeVoices[pad],0,__ATOMIC_RELEASE);n=0;}if(n==0){__atomic_store_n(&g_playPosPpm[pad],-1,__ATOMIC_RELEASE);if(g_main)g_api.PostMessageW(g_main,WM_PLAY_STATE,(WL_WPARAM)pad,0);}}
static void deactivate_voice(Client*c,int v){Voice*vv=&c->voices[v];if(!vv->active)return;int pad=vv->pad;Sample*s=vv->sample;vv->active=0;vv->sample=0;if(pad>=0&&pad<PAD_COUNT)playing_dec(pad);sample_release(s);}
static int add_voice(Client*c,int pad){Sample*s=0;float a=0,b=1;spin_lock();s=g_pads[pad].sample;if(s)sample_retain(s);a=fclamp(g_pads[pad].selStart,0,1);b=fclamp(g_pads[pad].selEnd,0,1);spin_unlock();if(!s){__atomic_add_fetch(&g_droppedTriggers,1,__ATOMIC_RELAXED);return 0;}if(b<=a){a=0;b=1;}double start=(double)a*(double)s->frames,end=(double)b*(double)s->frames;if(end>(double)s->frames)end=(double)s->frames;if(end<=start+0.5){sample_release(s);__atomic_add_fetch(&g_droppedTriggers,1,__ATOMIC_RELAXED);return 0;}int slot=-1;WL_U64 oldest=(WL_U64)-1;int oldestSlot=0;for(int v=0;v<MAX_VOICES;v++){if(!c->voices[v].active){slot=v;break;}if(c->voices[v].age<oldest){oldest=c->voices[v].age;oldestSlot=v;}}if(slot<0){slot=oldestSlot;deactivate_voice(c,slot);__atomic_add_fetch(&g_voiceSteals,1,__ATOMIC_RELAXED);}WL_I32 ph=__atomic_load_n(&g_pads[pad].pitchHalf,__ATOMIC_ACQUIRE),vh=__atomic_load_n(&g_pads[pad].volumeHalfPct,__ATOMIC_ACQUIRE);double step=((double)s->sampleRate/(double)(c->sampleRate?c->sampleRate:48000))*pitch_ratio_half(ph);float vol=(float)vh/200.0f;Voice*vv=&c->voices[slot];vv->active=1;vv->pad=pad;vv->sample=s;vv->start=start;vv->pos=start;vv->end=end;vv->step=vv->targetStep=step;vv->volume=vv->targetVolume=vol;vv->age=++c->nextAge;playing_inc(pad);return 1;}
static void sync_triggers(Client*c){WL_I64 stop=__atomic_load_n(&g_stopSerial,__ATOMIC_ACQUIRE);if(stop!=c->seenStop){for(int v=0;v<MAX_VOICES;v++)deactivate_voice(c,v);c->seenStop=stop;}for(int i=0;i<PAD_COUNT;i++){WL_I64 pst=__atomic_load_n(&g_padStopSerial[i],__ATOMIC_ACQUIRE);if(pst!=c->seenPadStop[i]){for(int v=0;v<MAX_VOICES;v++)if(c->voices[v].active&&c->voices[v].pad==i)deactivate_voice(c,v);c->seenPadStop[i]=pst;}WL_I64 now=__atomic_load_n(&g_triggerSerial[i],__ATOMIC_ACQUIRE),delta=now-c->seen[i];if(delta>8){__atomic_add_fetch(&g_droppedTriggers,(WL_U32)(delta-8),__ATOMIC_RELAXED);delta=8;}while(delta-->0)add_voice(c,i);c->seen[i]=now;}}
static float cubic_sample(const float*s,WL_U64 frames,double start,double end,double pos){if(!s||!frames)return 0;WL_U64 lo=(WL_U64)start,hi=end>=1.0?(WL_U64)(end-1.0):0;if(hi>=frames)hi=frames-1;if(lo>hi)lo=hi;WL_U64 i1=(WL_U64)pos;if(i1<lo)i1=lo;if(i1>hi)i1=hi;WL_U64 i0=i1>lo?i1-1:i1,i2=i1<hi?i1+1:i1,i3=i2<hi?i2+1:i2;float t=(float)(pos-(double)((WL_U64)pos)),t2=t*t,t3=t2*t;float p0=s[i0],p1=s[i1],p2=s[i2],p3=s[i3];return 0.5f*((2.0f*p1)+(-p0+p2)*t+(2.0f*p0-5.0f*p1+4.0f*p2-p3)*t2+(-p0+3.0f*p1-3.0f*p2+p3)*t3);}
static void publish_playheads(Client*c){WL_U64 bestAge[PAD_COUNT];WL_I32 bestPos[PAD_COUNT];for(int i=0;i<PAD_COUNT;i++){bestAge[i]=0;bestPos[i]=-1;}for(int v=0;v<MAX_VOICES;v++){Voice*vv=&c->voices[v];if(!vv->active||!vv->sample)continue;int p=vv->pad;if(vv->age>=bestAge[p]){bestAge[p]=vv->age;double f=vv->sample->frames?vv->pos/(double)vv->sample->frames:0;if(f<0)f=0;if(f>1)f=1;bestPos[p]=(WL_I32)(f*1000000.0+0.5);}}for(int i=0;i<PAD_COUNT;i++)if(bestPos[i]>=0)__atomic_store_n(&g_playPosPpm[i],bestPos[i],__ATOMIC_RELEASE);}
static int generate_audio(Client*c,float*out,WL_U32 frames){sync_triggers(c);if(__atomic_load_n(&g_boardMuted,__ATOMIC_ACQUIRE)){for(int v=0;v<MAX_VOICES;v++)deactivate_voice(c,v);return 0;}int any=0;double smooth=1.0/((double)(c->sampleRate?c->sampleRate:48000)*0.015);if(smooth>1.0)smooth=1.0;for(int v=0;v<MAX_VOICES;v++){Voice*vv=&c->voices[v];if(!vv->active||!vv->sample)continue;any=1;WL_I32 ph=__atomic_load_n(&g_pads[vv->pad].pitchHalf,__ATOMIC_ACQUIRE),vh=__atomic_load_n(&g_pads[vv->pad].volumeHalfPct,__ATOMIC_ACQUIRE);vv->targetStep=((double)vv->sample->sampleRate/(double)(c->sampleRate?c->sampleRate:48000))*pitch_ratio_half(ph);vv->targetVolume=(float)vh/200.0f;}if(!any)return 0;for(WL_U32 f=0;f<frames;f++){float sum=0;for(int v=0;v<MAX_VOICES;v++){Voice*vv=&c->voices[v];if(!vv->active||!vv->sample)continue;double pos=vv->pos;if(pos>=vv->end||pos>=(double)vv->sample->frames){deactivate_voice(c,v);continue;}vv->step+=(vv->targetStep-vv->step)*smooth;vv->volume+=(vv->targetVolume-vv->volume)*(float)smooth;float q=cubic_sample(vv->sample->mono,vv->sample->frames,vv->start,vv->end,pos);sum+=q*vv->volume;vv->pos=pos+vv->step;if(vv->pos>=vv->end)deactivate_voice(c,v);}sum=flush_tiny(fclamp(sum,-4.0f,4.0f));out[f*2]=sum;out[f*2+1]=sum;}publish_playheads(c);return 1;}
static WL_DWORD WL_CALLBACK client_thread(void*ctx){Client*c=(Client*)ctx;__atomic_add_fetch(&g_connections,1,__ATOMIC_SEQ_CST);if(g_main)g_api.PostMessageW(g_main,WM_CONN_UPDATE,0,0);PipeHello hi;if(!pipe_read_exact(c->pipe,&hi,sizeof(hi))||hi.magic!=M_HELLO){goto done;}if(hi.version!=PROTOCOL_VERSION||hi.sampleRate<8000||hi.sampleRate>384000){__atomic_store_n(&g_protocolStatus,-1,__ATOMIC_RELEASE);__atomic_add_fetch(&g_protocolMismatch,1,__ATOMIC_RELAXED);if(g_main)g_api.PostMessageW(g_main,WM_PROTOCOL_STATUS,0,0);goto done;}PipeAck ack={M_ACK,PROTOCOL_VERSION,BUILD_VERSION,0};if(!pipe_write_exact(c->pipe,&ack,sizeof(ack)))goto done;__atomic_store_n(&g_protocolStatus,1,__ATOMIC_RELEASE);c->sampleRate=hi.sampleRate;WL_U32 ev=__atomic_add_fetch(&g_connectEvents,1,__ATOMIC_RELAXED);if(ev>1)__atomic_add_fetch(&g_reconnects,1,__ATOMIC_RELAXED);for(int i=0;i<PAD_COUNT;i++){c->seen[i]=__atomic_load_n(&g_triggerSerial[i],__ATOMIC_ACQUIRE);c->seenPadStop[i]=__atomic_load_n(&g_padStopSerial[i],__ATOMIC_ACQUIRE);}c->seenStop=__atomic_load_n(&g_stopSerial,__ATOMIC_ACQUIRE);while(g_running){PipeRequest rq;if(!pipe_read_exact(c->pipe,&rq,sizeof(rq)))break;if(rq.magic!=M_REQ||rq.frames==0||rq.frames>2048)break;WL_U32 oldSr=__atomic_exchange_n(&g_hostSampleRate,rq.sampleRate,__ATOMIC_ACQ_REL);__atomic_store_n(&g_hostBlockSize,rq.blockSize,__ATOMIC_RELAXED);__atomic_store_n(&g_ringFill,rq.ringFill,__ATOMIC_RELAXED);__atomic_store_n(&g_targetFill,rq.targetFill,__ATOMIC_RELAXED);__atomic_store_n(&g_underruns,rq.underruns,__ATOMIC_RELAXED);__atomic_store_n(&g_clipCount,rq.clips,__ATOMIC_RELAXED);if(c->haveCounters){if(rq.underruns>=c->lastUnderruns)__atomic_add_fetch(&g_totalUnderrunEvents,(WL_U64)(rq.underruns-c->lastUnderruns),__ATOMIC_RELAXED);else __atomic_add_fetch(&g_totalUnderrunEvents,(WL_U64)rq.underruns,__ATOMIC_RELAXED);if(rq.clips>=c->lastClips)__atomic_add_fetch(&g_totalClipFrames,(WL_U64)(rq.clips-c->lastClips),__ATOMIC_RELAXED);else __atomic_add_fetch(&g_totalClipFrames,(WL_U64)rq.clips,__ATOMIC_RELAXED);}else c->haveCounters=1;c->lastUnderruns=rq.underruns;c->lastClips=rq.clips;WL_U32 po=__atomic_load_n(&g_peakMilli,__ATOMIC_RELAXED);while(rq.peakMilli>po&&!__atomic_compare_exchange_n(&g_peakMilli,&po,rq.peakMilli,1,__ATOMIC_RELEASE,__ATOMIC_RELAXED)){}if(oldSr!=rq.sampleRate&&g_main)g_api.PostMessageW(g_main,WM_HOST_RATE,(WL_WPARAM)rq.sampleRate,0);c->sampleRate=rq.sampleRate;int active=generate_audio(c,c->audio,rq.frames);monitor_process(c,c->audio,rq.frames,rq.sampleRate,active);PipeAudio ah={M_AUDIO,active?rq.frames:0,2,active?0:PIPE_AUDIO_IDLE};if(!pipe_write_exact(c->pipe,&ah,sizeof(ah)))break;if(active&&!pipe_write_exact(c->pipe,c->audio,rq.frames*2*sizeof(float)))break;}
done:for(int v=0;v<MAX_VOICES;v++)deactivate_voice(c,v);monitor_owner_disconnect(c);g_api.DisconnectNamedPipe(c->pipe);g_api.CloseHandle(c->pipe);g_api.HeapFree(g_api.GetProcessHeap(),0,c);WL_I32 left=__atomic_sub_fetch(&g_connections,1,__ATOMIC_SEQ_CST);if(left==0){if(__atomic_load_n(&g_protocolStatus,__ATOMIC_RELAXED)>0)__atomic_store_n(&g_protocolStatus,0,__ATOMIC_RELEASE);__atomic_store_n(&g_hostSampleRate,0,__ATOMIC_RELAXED);__atomic_store_n(&g_hostBlockSize,0,__ATOMIC_RELAXED);__atomic_store_n(&g_ringFill,0,__ATOMIC_RELAXED);__atomic_store_n(&g_targetFill,0,__ATOMIC_RELAXED);__atomic_store_n(&g_peakMilli,0,__ATOMIC_RELAXED);}if(g_main)g_api.PostMessageW(g_main,WM_CONN_UPDATE,0,0);return 0;}
static WL_DWORD WL_CALLBACK server_thread(void*ctx){(void)ctx;void*sd=0;WL_SECURITY_ATTRIBUTES sa;memset(&sa,0,sizeof(sa));sa.nLength=sizeof(sa);static const WL_WCHAR SDDL[]={ 'D',':','(','A',';',';','G','A',';',';',';','W','D',')',0};if(g_api.ConvertStringSecurityDescriptorToSecurityDescriptorW(SDDL,SDDL_REVISION_1,&sd,0)){sa.lpSecurityDescriptor=sd;}while(g_running){WL_HANDLE p=g_api.CreateNamedPipeW(PIPE_NAME,PIPE_ACCESS_DUPLEX,PIPE_TYPE_MESSAGE|PIPE_READMODE_MESSAGE|PIPE_WAIT|PIPE_REJECT_REMOTE_CLIENTS,PIPE_UNLIMITED_INSTANCES,32768,8192,0,sd?&sa:0);if(p==WL_INVALID_HANDLE_VALUE){g_api.Sleep(500);continue;}WL_BOOL ok=g_api.ConnectNamedPipe(p,0);if(!ok&&g_api.GetLastError()!=ERROR_PIPE_CONNECTED){g_api.CloseHandle(p);g_api.Sleep(50);continue;}Client*c=(Client*)g_api.HeapAlloc(g_api.GetProcessHeap(),0,sizeof(Client));if(!c){g_api.CloseHandle(p);continue;}memset(c,0,sizeof(*c));c->pipe=p;WL_HANDLE th=g_api.CreateThread(0,0,client_thread,c,0,0);if(th)g_api.CloseHandle(th);else{g_api.CloseHandle(p);g_api.HeapFree(g_api.GetProcessHeap(),0,c);}}if(sd)g_api.LocalFree(sd);return 0;}

static void invalidate_wave(void){if(g_main){WL_RECT r={WAVE_X,WAVE_Y,WAVE_X+WAVE_W,WAVE_Y+WAVE_H};g_api.InvalidateRect(g_main,&r,WL_FALSE);}}
static void invalidate_playhead_delta(void){
 if(!g_main)return;WL_I32 ppm=__atomic_load_n(&g_playPosPpm[g_selected],__ATOMIC_ACQUIRE);int cur=-1;if(ppm>=0&&__atomic_load_n(&g_activeVoices[g_selected],__ATOMIC_RELAXED)>0)cur=WAVE_X+(int)(((WL_I64)ppm*(WAVE_W-1))/1000000ll);
 if(cur==g_lastPlayheadX)return;int xs[2]={g_lastPlayheadX,cur};for(int i=0;i<2;i++)if(xs[i]>=WAVE_X&&xs[i]<WAVE_X+WAVE_W){WL_RECT r={xs[i]-3,WAVE_Y+1,xs[i]+4,WAVE_Y+WAVE_H-1};if(r.left<WAVE_X)r.left=WAVE_X;if(r.right>WAVE_X+WAVE_W)r.right=WAVE_X+WAVE_W;g_api.InvalidateRect(g_main,&r,WL_FALSE);}g_lastPlayheadX=cur;
}
static int in_wave(int x,int y){return x>=WAVE_X&&x<WAVE_X+WAVE_W&&y>=WAVE_Y&&y<WAVE_Y+WAVE_H;}
static float wave_frac(int x){float f=(float)(x-WAVE_X)/(float)(WAVE_W-1);return fclamp(f,0,1);}
static void apply_wave_drag(float cur,int finish){float a=g_waveAnchor,b=cur;if(a>b){float t=a;a=b;b=t;}if(finish&&b-a<1.0f/(float)WAVE_W){b=a+1.0f/(float)WAVE_W;if(b>1){b=1;a=b-1.0f/(float)WAVE_W;}}spin_lock();if(g_pads[g_selected].sample){g_pads[g_selected].selStart=fclamp(a,0,1);g_pads[g_selected].selEnd=fclamp(b,0,1);}spin_unlock();update_selected_text();invalidate_wave();if(finish)save_config();}
static void reset_wave_range(void){spin_lock();if(g_pads[g_selected].sample){g_pads[g_selected].selStart=0;g_pads[g_selected].selEnd=1;}spin_unlock();save_config();update_selected_text();invalidate_wave();set_status_ascii("Playback range reset to the full sample.");}
static void draw_waveform(WL_HDC dc){WL_RECT r={WAVE_X,WAVE_Y,WAVE_X+WAVE_W,WAVE_Y+WAVE_H};g_api.FillRect(dc,&r,g_waveBg);float peaks[WAVE_PEAKS],a=0,b=1;int loaded=0;spin_lock();Pad*p=&g_pads[g_selected];loaded=p->sample&&p->sample->frames;if(loaded)memcpy(peaks,p->sample->peaks,sizeof(peaks));else memset(peaks,0,sizeof(peaks));a=p->selStart;b=p->selEnd;spin_unlock();if(loaded){int sx=WAVE_X+(int)(a*(float)(WAVE_W-1)),ex=WAVE_X+(int)(b*(float)(WAVE_W-1));if(ex<sx){int t=sx;sx=ex;ex=t;}WL_RECT sr={sx,WAVE_Y+1,ex+1,WAVE_Y+WAVE_H-1};g_api.FillRect(dc,&sr,g_waveSel);WL_HGDIOBJ old=g_api.SelectObject(dc,(WL_HGDIOBJ)g_waveMidPen);int cy=WAVE_Y+WAVE_H/2;g_api.MoveToEx(dc,WAVE_X+1,cy,0);g_api.LineTo(dc,WAVE_X+WAVE_W-1,cy);g_api.SelectObject(dc,(WL_HGDIOBJ)g_wavePen);for(int x=1;x<WAVE_W-1;x++){int bi=(x*WAVE_PEAKS)/WAVE_W;if(bi>=WAVE_PEAKS)bi=WAVE_PEAKS-1;int amp=(int)(peaks[bi]*(float)(WAVE_H/2-4));g_api.MoveToEx(dc,WAVE_X+x,cy-amp,0);g_api.LineTo(dc,WAVE_X+x,cy+amp+1);}WL_I32 ppm=__atomic_load_n(&g_playPosPpm[g_selected],__ATOMIC_ACQUIRE);if(ppm>=0&&__atomic_load_n(&g_activeVoices[g_selected],__ATOMIC_RELAXED)>0){int px=WAVE_X+(int)(((WL_I64)ppm*(WAVE_W-1))/1000000ll);g_api.SelectObject(dc,(WL_HGDIOBJ)g_playheadPen);g_api.MoveToEx(dc,px,WAVE_Y+2,0);g_api.LineTo(dc,px,WAVE_Y+WAVE_H-2);}g_api.SelectObject(dc,old);}WL_RECT top={WAVE_X,WAVE_Y,WAVE_X+WAVE_W,WAVE_Y+1},bot={WAVE_X,WAVE_Y+WAVE_H-1,WAVE_X+WAVE_W,WAVE_Y+WAVE_H},le={WAVE_X,WAVE_Y,WAVE_X+1,WAVE_Y+WAVE_H},ri={WAVE_X+WAVE_W-1,WAVE_Y,WAVE_X+WAVE_W,WAVE_Y+WAVE_H};g_api.FillRect(dc,&top,g_waveBorder);g_api.FillRect(dc,&bot,g_waveBorder);g_api.FillRect(dc,&le,g_waveBorder);g_api.FillRect(dc,&ri,g_waveBorder);}


static void invalidate_controls(void){
 if(!g_main)return;
 WL_RECT a={g_controlPanelX,g_controlPanelY,g_controlPanelX+g_controlPanelW,g_controlPanelY+g_controlPanelH};
 WL_RECT c={MONVOL_X-4,MONVOL_Y-4,MONVOL_X+MONVOL_W+4,MONVOL_Y+MONVOL_H+4};
 g_api.InvalidateRect(g_main,&a,WL_FALSE);g_api.InvalidateRect(g_main,&c,WL_FALSE);
}
static int in_volume_control(int x,int y){return x>=VOL_X&&x<VOL_X+VOL_W&&y>=VOL_Y&&y<VOL_Y+VOL_H;}
static int in_pitch_control(int x,int y){return x>=PITCH_X&&x<PITCH_X+PITCH_W&&y>=PITCH_Y&&y<PITCH_Y+PITCH_H;}
static int in_monitor_control(int x,int y){return x>=MONVOL_X&&x<MONVOL_X+MONVOL_W&&y>=MONVOL_Y&&y<MONVOL_Y+MONVOL_H;}
static float control_frac(int x,int ox,int ow){return fclamp((float)(x-ox)/(float)(ow-1),0,1);}
static WL_I32 iabs32(WL_I32 v){return v<0?-v:v;}
/* Fine movement remains 0.5 units. Major notches are magnetic near each 25% position. */
static WL_I32 snap_volume_major(WL_I32 halfPct){WL_I32 nearest=((halfPct+25)/50)*50;if(nearest<0)nearest=0;if(nearest>400)nearest=400;return iabs32(halfPct-nearest)<=8?nearest:halfPct;}
static WL_I32 snap_pitch_major(WL_I32 halfSteps){WL_I32 shifted=halfSteps+24;WL_I32 nearest=((shifted+3)/6)*6-24;if(nearest<-24)nearest=-24;if(nearest>24)nearest=24;return iabs32(halfSteps-nearest)<=1?nearest:halfSteps;}
static void set_volume_from_x(int x,int finish){WL_I32 halfPct=(WL_I32)(control_frac(x,VOL_X,VOL_W)*400.0f+0.5f);if(halfPct<0)halfPct=0;if(halfPct>400)halfPct=400;halfPct=snap_volume_major(halfPct);float v=(float)halfPct/200.0f;g_pads[g_selected].volume=v;__atomic_store_n(&g_pads[g_selected].volumeHalfPct,halfPct,__ATOMIC_RELEASE);update_selected_text();invalidate_controls();if(finish)save_config();}
static void set_pitch_from_x(int x,int finish){float f=control_frac(x,PITCH_X,PITCH_W);WL_I32 ph=(WL_I32)(f*48.0f+0.5f)-24;if(ph<-24)ph=-24;if(ph>24)ph=24;ph=snap_pitch_major(ph);__atomic_store_n(&g_pads[g_selected].pitchHalf,ph,__ATOMIC_RELEASE);update_selected_text();invalidate_controls();if(finish)save_config();}
static void reset_volume(void){g_pads[g_selected].volume=1.0f;__atomic_store_n(&g_pads[g_selected].volumeHalfPct,200,__ATOMIC_RELEASE);save_config();update_selected_text();invalidate_controls();set_status_ascii("Volume reset to 100.0%.");}
static void reset_pitch(void){__atomic_store_n(&g_pads[g_selected].pitchHalf,0,__ATOMIC_RELEASE);save_config();update_selected_text();invalidate_controls();set_status_ascii("Pitch reset to 0.0 semitones.");}
static void set_monitor_volume_from_x(int x,int finish){WL_I32 halfPct=(WL_I32)(control_frac(x,MONVOL_X,MONVOL_W)*200.0f+0.5f);if(halfPct<0)halfPct=0;if(halfPct>200)halfPct=200;__atomic_store_n(&g_monitorVolumeHalfPct,halfPct,__ATOMIC_RELEASE);update_monitor_volume_text();invalidate_controls();if(finish){save_config();set_status_ascii("Monitor volume saved. This changes only what you hear locally, not the microphone stream.");}}
static void reset_monitor_volume(void){__atomic_store_n(&g_monitorVolumeHalfPct,200,__ATOMIC_RELEASE);update_monitor_volume_text();save_config();invalidate_controls();set_status_ascii("Monitor volume reset to 100.0%.");}
static void draw_control_bar_light(WL_HDC dc,int x,int y,int w,int h,int pos,int centerMode){
 WL_RECT r={x,y,x+w,y+h};g_api.FillRect(dc,&r,g_waveBg);WL_RECT top={x,y,x+w,y+1},bot={x,y+h-1,x+w,y+h},le={x,y,x+1,y+h},ri={x+w-1,y,x+w,y+h};g_api.FillRect(dc,&top,g_waveBorder);g_api.FillRect(dc,&bot,g_waveBorder);g_api.FillRect(dc,&le,g_waveBorder);g_api.FillRect(dc,&ri,g_waveBorder);
 for(int i=0;i<=8;i++){int tx=x+(i*(w-1))/8;WL_RECT ta={tx,y-3,tx+1,y};WL_RECT tb={tx,y+h,tx+1,y+h+3};g_api.FillRect(dc,&ta,g_waveBorder);g_api.FillRect(dc,&tb,g_waveBorder);}if(pos<x+1)pos=x+1;if(pos>x+w-2)pos=x+w-2;
 if(centerMode){int c=x+w/2;WL_RECT mid={c,y+2,c+1,y+h-2};g_api.FillRect(dc,&mid,g_waveBorder);WL_RECT fill;if(pos<c){fill.left=pos;fill.right=c;}else{fill.left=c;fill.right=pos;}fill.top=y+3;fill.bottom=y+h-3;if(fill.right>fill.left)g_api.FillRect(dc,&fill,g_waveSel);}else{WL_RECT fill={x+1,y+3,pos,y+h-3};if(fill.right>fill.left)g_api.FillRect(dc,&fill,g_waveSel);}WL_RECT knob={pos-2,y+1,pos+3,y+h-1};g_api.FillRect(dc,&knob,g_waveBorder);
}
static void draw_control_bar_dark(WL_HDC dc,int x,int y,int w,int h,int pos,int centerMode){
 WL_RECT r={x,y,x+w,y+h};g_api.FillRect(dc,&r,g_controlTrack);for(int i=0;i<=8;i++){int tx=x+(i*(w-1))/8;WL_RECT tick={tx,y-4,tx+1,y+h+4};g_api.FillRect(dc,&tick,g_controlTrack);}if(pos<x+2)pos=x+2;if(pos>x+w-3)pos=x+w-3;
 if(centerMode){int c=x+w/2;WL_RECT mid={c,y+1,c+2,y+h-1};g_api.FillRect(dc,&mid,g_controlFill);WL_RECT fill={pos<c?pos:c,y+4,pos<c?c:pos,y+h-4};if(fill.right>fill.left)g_api.FillRect(dc,&fill,g_controlFill);}else{WL_RECT fill={x+2,y+4,pos,y+h-4};if(fill.right>fill.left)g_api.FillRect(dc,&fill,g_controlFill);}WL_RECT knob={pos-3,y-1,pos+4,y+h+1};g_api.FillRect(dc,&knob,g_controlFill);
}
static void draw_panel_text(WL_HDC dc,int x,int y,const WL_WCHAR*t){if(!pTextOutW||!t)return;if(pSetBkMode)pSetBkMode(dc,TRANSPARENT);if(pSetTextColor)pSetTextColor(dc,RGB_(235,235,235));if(g_font)g_api.SelectObject(dc,(WL_HGDIOBJ)g_font);pTextOutW(dc,x,y,t,(WL_I32)wl_wlen(t));}
static void draw_controls(WL_HDC dc){
 WL_I32 vh=__atomic_load_n(&g_pads[g_selected].volumeHalfPct,__ATOMIC_RELAXED),pitchHalf=__atomic_load_n(&g_pads[g_selected].pitchHalf,__ATOMIC_RELAXED),mh=__atomic_load_n(&g_monitorVolumeHalfPct,__ATOMIC_RELAXED);if(vh<0)vh=0;if(vh>400)vh=400;if(pitchHalf<-24)pitchHalf=-24;if(pitchHalf>24)pitchHalf=24;if(mh<0)mh=0;if(mh>200)mh=200;
 int vp=VOL_X+(int)(((float)vh/400.0f)*(float)(VOL_W-1));int pp=PITCH_X+(int)(((float)(pitchHalf+24)/48.0f)*(float)(PITCH_W-1));int mp=MONVOL_X+(int)(((float)mh/200.0f)*(float)(MONVOL_W-1));
 WL_RECT panel={g_controlPanelX,g_controlPanelY,g_controlPanelX+g_controlPanelW,g_controlPanelY+g_controlPanelH};g_api.FillRect(dc,&panel,g_controlPanelBg);
 WL_WCHAR a[96],b[96];a[0]=0;b[0]=0;wadd_ascii(a,96,"VOLUME  ");wadd_half_units(a,96,vh,0);wadd_ascii(a,96,"%");wadd_ascii(b,96,"PITCH  ");wadd_half_units(b,96,pitchHalf,1);wadd_ascii(b,96," st");draw_panel_text(dc,VOL_X,g_controlPanelY+10,a);draw_panel_text(dc,PITCH_X,g_controlPanelY+10,b);
 draw_control_bar_dark(dc,VOL_X,VOL_Y,VOL_W,VOL_H,vp,0);draw_control_bar_dark(dc,PITCH_X,PITCH_Y,PITCH_W,PITCH_H,pp,1);draw_control_bar_light(dc,MONVOL_X,MONVOL_Y,MONVOL_W,MONVOL_H,mp,0);
}


static int capture_conflict(int target,WL_U32 vk,WL_U32 mods){if(target<PAD_COUNT)return find_internal_hotkey_conflict(target,vk,mods);if(target==PAD_COUNT)return find_stop_hotkey_conflict(vk,mods);return find_mute_hotkey_conflict(vk,mods);}
static int test_hotkey_available(WL_U32 vk,WL_U32 mods){int id=HOTKEY_STOP+2;if(g_api.RegisterHotKey(g_main,id,mods|MOD_NOREPEAT,vk)){g_api.UnregisterHotKey(g_main,id);return 1;}return 0;}
static void finish_hotkey_capture(int target,WL_U32 vk,WL_U32 mods){int conflict=capture_conflict(target,vk,mods);if(conflict>=0){restore_hotkeys_after_capture();update_diagnostics();g_api.MessageBoxW(g_main,(const WL_WCHAR[]){'T','h','a','t',' ','h','o','t','k','e','y',' ','i','s',' ','a','l','r','e','a','d','y',' ','u','s','e','d',' ','b','y',' ','a','n','o','t','h','e','r',' ','s','o','u','n','d','b','o','a','r','d',' ','a','c','t','i','o','n','.',0},WINDOW_TITLE,0x30);set_status_ascii("Hotkey conflict: assignment was not changed.");return;}if(!test_hotkey_available(vk,mods)){restore_hotkeys_after_capture();update_diagnostics();g_api.MessageBoxW(g_main,(const WL_WCHAR[]){'W','i','n','d','o','w','s',' ','r','e','j','e','c','t','e','d',' ','t','h','a','t',' ','h','o','t','k','e','y','.',13,10,'I','t',' ','m','a','y',' ','b','e',' ','u','s','e','d',' ','b','y',' ','a','n','o','t','h','e','r',' ','a','p','p',' ','o','r',' ','r','e','s','e','r','v','e','d',' ','b','y',' ','t','h','e',' ','s','y','s','t','e','m','.',0},WINDOW_TITLE,0x30);set_status_ascii("Hotkey rejected by Windows; previous assignment preserved.");return;}if(target<PAD_COUNT){g_pads[target].vk=vk;g_pads[target].mods=mods;}else if(target==PAD_COUNT){g_stopVk=vk;g_stopMods=mods;}else{g_muteVk=vk;g_muteMods=mods;}save_config();restore_hotkeys_after_capture();update_diagnostics();set_status_ascii("Global hotkey saved.");}

static int rect_hits(const WL_RECT*a,const WL_RECT*b){return a->left<b->right&&a->right>b->left&&a->top<b->bottom&&a->bottom>b->top;}
static int ensure_backbuffer(WL_HDC dc){
 int needW=g_clientW>0?g_clientW:960,needH=g_clientH>0?g_clientH:900;
 if(g_backDc&&g_backBitmap&&g_backW>=needW&&g_backH>=needH)return 1;
 if(!pCreateCompatibleDC||!pCreateCompatibleBitmap||!pBitBlt||!pDeleteDC)return 0;
 if(g_backDc&&g_backOldBitmap)g_api.SelectObject(g_backDc,g_backOldBitmap);if(g_backBitmap)g_api.DeleteObject(g_backBitmap);if(g_backDc)pDeleteDC(g_backDc);g_backDc=0;g_backBitmap=0;g_backOldBitmap=0;
 WL_HDC m=pCreateCompatibleDC(dc);if(!m)return 0;WL_HGDIOBJ bm=pCreateCompatibleBitmap(dc,needW,needH);if(!bm){pDeleteDC(m);return 0;}WL_HGDIOBJ old=g_api.SelectObject(m,bm);if(!old){g_api.DeleteObject(bm);pDeleteDC(m);return 0;}g_backDc=m;g_backBitmap=bm;g_backOldBitmap=old;g_backW=needW;g_backH=needH;return 1;
}
static void destroy_backbuffer(void){
 if(g_backDc&&g_backOldBitmap)g_api.SelectObject(g_backDc,g_backOldBitmap);
 if(g_backBitmap)g_api.DeleteObject(g_backBitmap);
 if(g_backDc&&pDeleteDC)pDeleteDC(g_backDc);
 g_backDc=0;g_backBitmap=0;g_backOldBitmap=0;g_backW=0;g_backH=0;
}
static void paint_custom(WL_HDC dc,const WL_RECT*paint){
 WL_RECT wr={WAVE_X,WAVE_Y,WAVE_X+WAVE_W,WAVE_Y+WAVE_H};
 WL_RECT panel={g_controlPanelX,g_controlPanelY,g_controlPanelX+g_controlPanelW,g_controlPanelY+g_controlPanelH};
 WL_RECT mr={MONVOL_X-4,MONVOL_Y-4,MONVOL_X+MONVOL_W+4,MONVOL_Y+MONVOL_H+4};
 if(ensure_backbuffer(dc)){
  WL_RECT r=paint?*paint:(WL_RECT){0,0,g_clientW,g_clientH};if(g_windowBg)g_api.FillRect(g_backDc,&r,g_windowBg);
  if(g_activeTab==0){if(!paint||rect_hits(paint,&wr))draw_waveform(g_backDc);if(!paint||rect_hits(paint,&panel)||rect_hits(paint,&mr))draw_controls(g_backDc);}
  if(paint)pBitBlt(dc,paint->left,paint->top,paint->right-paint->left,paint->bottom-paint->top,g_backDc,paint->left,paint->top,SRCCOPY);else pBitBlt(dc,0,0,g_clientW,g_clientH,g_backDc,0,0,SRCCOPY);
 }else{if(paint&&g_windowBg)g_api.FillRect(dc,paint,g_windowBg);if(g_activeTab==0){if(!paint||rect_hits(paint,&wr))draw_waveform(dc);if(!paint||rect_hits(paint,&panel)||rect_hits(paint,&mr))draw_controls(dc);}}
}

static void create_child(WL_HWND* out,const WL_WCHAR* cls,const WL_WCHAR* text,WL_DWORD style,int x,int y,int w,int h,int id){*out=g_api.CreateWindowExW(0,cls,text,WS_CHILD|WS_VISIBLE|style,x,y,w,h,g_main,(void*)(WL_UPTR)id,(WL_HINSTANCE)g_api.GetModuleHandleW(0),0);if(*out&&g_font)g_api.SendMessageW(*out,WM_SETFONT,(WL_WPARAM)g_font,1);}
static int ui_scale(int v){WL_U32 dpi=pGetDpiForWindow&&g_main?pGetDpiForWindow(g_main):(pGetDpiForSystem?pGetDpiForSystem():96u);if(dpi<72||dpi>384)dpi=96;return (int)(((WL_I64)v*dpi+48)/96);}
static void move_child(WL_HWND h,int x,int y,int w,int ht){if(h&&pMoveWindow&&w>0&&ht>0)pMoveWindow(h,x,y,w,ht,WL_FALSE);}
static void show_child(WL_HWND h,int show){if(h)g_api.ShowWindow(h,show?SW_SHOW:SW_HIDE);}
static void update_tab_buttons(void){if(g_tabSoundButton)set_text_if_changed(g_tabSoundButton,g_activeTab==0?(const WL_WCHAR[]){'[',' ','S','o','u','n','d','b','o','a','r','d',' ',']',0}:(const WL_WCHAR[]){'S','o','u','n','d','b','o','a','r','d',0});if(g_tabDiagButton)set_text_if_changed(g_tabDiagButton,g_activeTab==1?(const WL_WCHAR[]){'[',' ','D','i','a','g','n','o','s','t','i','c','s',' ',']',0}:(const WL_WCHAR[]){'D','i','a','g','n','o','s','t','i','c','s',0});}
static void hide_all_page_controls(void){
 for(int i=0;i<PAD_COUNT;i++)show_child(g_padButtons[i],0);
 show_child(g_selectedText,0);show_child(g_waveLabel,0);show_child(g_fileInfoText,0);show_child(g_monitorButton,0);show_child(g_monitorOutputLabel,0);show_child(g_monitorDeviceCombo,0);show_child(g_monitorVolumeText,0);show_child(g_padNameLabel,0);show_child(g_nameEdit,0);show_child(g_applyNameButton,0);show_child(g_statusText,0);
 for(int i=0;i<6;i++){show_child(g_diagCards[i],0);show_child(g_diagValues[i],0);}
}
static void show_page_controls(void){
 int sb=g_activeTab==0;g_pageVisibilityTab=g_activeTab;
 /* Enforce visibility every time. This avoids stale native-child pixels after a tab switch. */
 hide_all_page_controls();
 if(sb){
  for(int i=0;i<PAD_COUNT;i++)show_child(g_padButtons[i],1);
  show_child(g_selectedText,1);show_child(g_waveLabel,1);show_child(g_fileInfoText,1);show_child(g_monitorButton,1);show_child(g_monitorOutputLabel,1);show_child(g_monitorDeviceCombo,1);show_child(g_monitorVolumeText,1);show_child(g_padNameLabel,1);show_child(g_nameEdit,1);show_child(g_applyNameButton,1);show_child(g_statusText,1);
 }else for(int i=0;i<6;i++){show_child(g_diagCards[i],1);show_child(g_diagValues[i],1);}
}
static void force_full_page_repaint(void){
 if(!g_main)return;WL_RECT all={0,0,g_clientW,g_clientH};g_api.InvalidateRect(g_main,&all,WL_TRUE);g_api.UpdateWindow(g_main);
 if(g_tabSoundButton)g_api.UpdateWindow(g_tabSoundButton);if(g_tabDiagButton)g_api.UpdateWindow(g_tabDiagButton);
 if(g_activeTab==0){for(int i=0;i<PAD_COUNT;i++)if(g_padButtons[i])g_api.UpdateWindow(g_padButtons[i]);}
 else for(int i=0;i<6;i++){if(g_diagCards[i])g_api.UpdateWindow(g_diagCards[i]);if(g_diagValues[i])g_api.UpdateWindow(g_diagValues[i]);}
}
static void layout_ui(void){
 if(!g_main||!pMoveWindow)return;WL_RECT cr;if(pGetClientRect&&pGetClientRect(g_main,&cr)){g_clientW=cr.right-cr.left;g_clientH=cr.bottom-cr.top;}if(g_clientW<1||g_clientH<1)return;
 int m=ui_scale(12),gap=ui_scale(8),tabH=ui_scale(30),tabW=ui_scale(126);move_child(g_tabSoundButton,m,ui_scale(5),tabW,tabH);move_child(g_tabDiagButton,m+tabW+gap,ui_scale(5),tabW,tabH);update_tab_buttons();
 int top=ui_scale(44);
 if(g_activeTab==0){
  int availW=g_clientW-2*m;int padGap=ui_scale(8);int padW=(availW-3*padGap)/4;int padH=ui_scale(68);if(g_clientH<ui_scale(860))padH=ui_scale(58);
  for(int i=0;i<PAD_COUNT;i++){int c=i%4,r=i/4;move_child(g_padButtons[i],m+c*(padW+padGap),top+r*(padH+padGap),padW,padH);}top+=4*padH+3*padGap+ui_scale(8);
  move_child(g_selectedText,m,top,availW,ui_scale(24));top+=ui_scale(26);
  move_child(g_waveLabel,m,top,availW,ui_scale(18));top+=ui_scale(20);
  int tailNeed=ui_scale(238);int waveH=g_clientH-top-tailNeed;if(waveH<ui_scale(82))waveH=ui_scale(82);if(waveH>ui_scale(150))waveH=ui_scale(150);g_waveX=m;g_waveY=top;g_waveW=availW;g_waveH=waveH;top+=waveH+ui_scale(4);
  move_child(g_fileInfoText,m,top,availW,ui_scale(20));top+=ui_scale(26);
  /* Monitor row: button + output selector + independent monitor volume. */
  int btnW=ui_scale(118),labW=ui_scale(92),comboW=(availW*32)/100;if(comboW<ui_scale(190))comboW=ui_scale(190);if(comboW>ui_scale(360))comboW=ui_scale(360);int x=m;
  move_child(g_monitorButton,x,top,btnW,ui_scale(30));x+=btnW+gap;move_child(g_monitorOutputLabel,x,top+ui_scale(5),labW,ui_scale(20));x+=labW;int remain=g_clientW-m-x;int rightW=remain-comboW-gap;if(rightW<ui_scale(210)){comboW-=ui_scale(40);rightW=remain-comboW-gap;}move_child(g_monitorDeviceCombo,x,top,comboW,ui_scale(220));x+=comboW+gap;move_child(g_monitorVolumeText,x,top,rightW,ui_scale(18));g_monVolX=x;g_monVolY=top+ui_scale(23);g_monVolW=rightW;g_monVolH=ui_scale(16);top+=ui_scale(50);
  /* Dark audio-control panel. */
  g_controlPanelX=m;g_controlPanelY=top;g_controlPanelW=availW;g_controlPanelH=ui_scale(88);int inner=ui_scale(18),midGap=ui_scale(42),half=(availW-2*inner-midGap)/2;g_volX=m+inner;g_volY=top+ui_scale(47);g_volW=half;g_volH=ui_scale(18);g_pitchX=g_volX+half+midGap;g_pitchY=g_volY;g_pitchW=half;g_pitchH=g_volH;top+=g_controlPanelH+ui_scale(9);
  int nameLab=ui_scale(70),applyW=ui_scale(92);move_child(g_padNameLabel,m,top+ui_scale(5),nameLab,ui_scale(22));move_child(g_nameEdit,m+nameLab,top,availW-nameLab-applyW-gap,ui_scale(28));move_child(g_applyNameButton,g_clientW-m-applyW,top,applyW,ui_scale(28));top+=ui_scale(36);
  int statusH=g_clientH-top-m;if(statusH<ui_scale(26))statusH=ui_scale(26);move_child(g_statusText,m,top,availW,statusH);
 }else{
  int availW=g_clientW-2*m,availH=g_clientH-top-m;int cols=g_clientW>=ui_scale(900)?3:(g_clientW>=ui_scale(620)?2:1);int rows=(6+cols-1)/cols;int cardGap=ui_scale(10);int cardW=(availW-(cols-1)*cardGap)/cols;int cardH=(availH-(rows-1)*cardGap)/rows;
  int minCardH=ui_scale(132);if(cardH<minCardH)cardH=minCardH;int maxBottom=g_clientH-m;
  for(int i=0;i<6;i++){int c=i%cols,r=i/cols;int x=m+c*(cardW+cardGap),y=top+r*(cardH+cardGap);int h=cardH;if(y+h>maxBottom)h=maxBottom-y;if(h<ui_scale(96))h=ui_scale(96);move_child(g_diagCards[i],x,y,cardW,h);move_child(g_diagValues[i],x+ui_scale(12),y+ui_scale(25),cardW-ui_scale(24),h-ui_scale(35));}
 }
 show_page_controls();WL_RECT all={0,0,g_clientW,g_clientH};g_api.InvalidateRect(g_main,&all,WL_TRUE);
 for(int i=0;i<PAD_COUNT;i++)if(g_activeTab==0&&g_padButtons[i])g_api.InvalidateRect(g_padButtons[i],0,WL_TRUE);
 if(g_tabSoundButton)g_api.InvalidateRect(g_tabSoundButton,0,WL_TRUE);if(g_tabDiagButton)g_api.InvalidateRect(g_tabDiagButton,0,WL_TRUE);
 if(g_activeTab==0){WL_HWND cs[]={g_selectedText,g_waveLabel,g_fileInfoText,g_monitorButton,g_monitorOutputLabel,g_monitorDeviceCombo,g_monitorVolumeText,g_padNameLabel,g_nameEdit,g_applyNameButton,g_statusText};for(WL_SIZE_T i=0;i<sizeof(cs)/sizeof(cs[0]);i++)if(cs[i])g_api.InvalidateRect(cs[i],0,WL_TRUE);}else{for(int i=0;i<6;i++){if(g_diagCards[i])g_api.InvalidateRect(g_diagCards[i],0,WL_TRUE);if(g_diagValues[i])g_api.InvalidateRect(g_diagValues[i],0,WL_TRUE);}}
}
static void build_main_menu(void){
 if(!pCreateMenu||!pCreatePopupMenu||!pAppendMenuW||!pSetMenu)return;g_mainMenu=pCreateMenu();g_soundboardMenu=pCreatePopupMenu();g_toolsMenu=pCreatePopupMenu();if(!g_mainMenu||!g_soundboardMenu||!g_toolsMenu)return;
 pAppendMenuW(g_soundboardMenu,MF_STRING,ID_STOP_ALL,(const WL_WCHAR[]){'S','t','o','p',' ','A','l','l',0});pAppendMenuW(g_soundboardMenu,MF_STRING,ID_TOGGLE_MUTE,(const WL_WCHAR[]){'T','o','g','g','l','e',' ','B','o','a','r','d',' ','M','u','t','e',0});pAppendMenuW(g_soundboardMenu,MF_SEPARATOR,0,0);pAppendMenuW(g_soundboardMenu,MF_STRING,ID_SET_STOP_HOTKEY,(const WL_WCHAR[]){'S','e','t',' ','S','t','o','p',' ','A','l','l',' ','H','o','t','k','e','y','.','.','.',0});pAppendMenuW(g_soundboardMenu,MF_STRING,ID_CLEAR_STOP_HOTKEY,(const WL_WCHAR[]){'C','l','e','a','r',' ','S','t','o','p',' ','A','l','l',' ','H','o','t','k','e','y',0});pAppendMenuW(g_soundboardMenu,MF_STRING,ID_SET_MUTE_HOTKEY,(const WL_WCHAR[]){'S','e','t',' ','B','o','a','r','d',' ','M','u','t','e',' ','H','o','t','k','e','y','.','.','.',0});pAppendMenuW(g_soundboardMenu,MF_STRING,ID_CLEAR_MUTE_HOTKEY,(const WL_WCHAR[]){'C','l','e','a','r',' ','B','o','a','r','d',' ','M','u','t','e',' ','H','o','t','k','e','y',0});
 pAppendMenuW(g_toolsMenu,MF_STRING,ID_RELOCATE_MISSING,(const WL_WCHAR[]){'R','e','l','o','c','a','t','e',' ','M','i','s','s','i','n','g',' ','S','o','u','n','d','s','.','.','.',0});pAppendMenuW(g_toolsMenu,MF_STRING,ID_SELF_TEST,(const WL_WCHAR[]){'A','u','d','i','o',' ','E','n','g','i','n','e',' ','S','e','l','f',' ','T','e','s','t',0});pAppendMenuW(g_toolsMenu,MF_STRING,ID_MONITOR_TEST,(const WL_WCHAR[]){'M','o','n','i','t','o','r',' ','T','e','s','t',0});pAppendMenuW(g_toolsMenu,MF_STRING,ID_REFRESH_DEVICES,(const WL_WCHAR[]){'R','e','f','r','e','s','h',' ','P','l','a','y','b','a','c','k',' ','D','e','v','i','c','e','s',0});pAppendMenuW(g_toolsMenu,MF_SEPARATOR,0,0);pAppendMenuW(g_toolsMenu,MF_STRING,ID_RESET_STATS,(const WL_WCHAR[]){'R','e','s','e','t',' ','D','i','a','g','n','o','s','t','i','c','s',0});
 pAppendMenuW(g_mainMenu,MF_POPUP,(WL_UPTR)g_soundboardMenu,(const WL_WCHAR[]){'S','o','u','n','d','b','o','a','r','d',0});pAppendMenuW(g_mainMenu,MF_POPUP,(WL_UPTR)g_toolsMenu,(const WL_WCHAR[]){'T','o','o','l','s',0});pSetMenu(g_main,g_mainMenu);if(pDrawMenuBar)pDrawMenuBar(g_main);
}
static void select_pad(int i){if(i<0||i>=PAD_COUNT)return;g_selected=i;g_lastPlayheadX=-1;update_selected_text();update_name_edit();update_file_info();invalidate_wave();invalidate_controls();}
static void open_selected_file_location(void){WL_WCHAR path[MAX_PATH_W];spin_lock();wl_wcpy(path,g_pads[g_selected].path,MAX_PATH_W);spin_unlock();if(!path[0])return;WL_WCHAR arg[MAX_PATH_W+32];arg[0]=0;wadd_ascii(arg,MAX_PATH_W+32,"/select,\"");wl_wcat(arg,path,MAX_PATH_W+32);wadd_ascii(arg,MAX_PATH_W+32,"\"");g_api.ShellExecuteW(g_main,(const WL_WCHAR[]){'o','p','e','n',0},(const WL_WCHAR[]){'e','x','p','l','o','r','e','r','.','e','x','e',0},arg,0,SW_SHOW);}
static void duplicate_selected_pad_to(int dst){
 int src=g_selected;if(dst<0||dst>=PAD_COUNT||dst==src)return;int occupied=0;spin_lock();occupied=g_pads[dst].sample||g_pads[dst].path[0]||g_pads[dst].name[0];spin_unlock();if(occupied&&g_api.MessageBoxW(g_main,(const WL_WCHAR[]){'T','h','e',' ','t','a','r','g','e','t',' ','p','a','d',' ','a','l','r','e','a','d','y',' ','h','a','s',' ','c','o','n','t','e','n','t','.',' ','R','e','p','l','a','c','e',' ','i','t','?',0},WINDOW_TITLE,MB_YESNO|MB_ICONWARNING)!=IDYES)return;
 __atomic_add_fetch(&g_loadGeneration[dst],1,__ATOMIC_ACQ_REL);stop_pad_playback(dst);Sample*srcSample=0,*old=0;WL_WCHAR path[MAX_PATH_W],name[32];float a,b,vol;WL_I32 vh,ph,state;spin_lock();srcSample=g_pads[src].sample;if(srcSample)sample_retain(srcSample);wl_wcpy(path,g_pads[src].path,MAX_PATH_W);wl_wcpy(name,g_pads[src].name,32);a=g_pads[src].selStart;b=g_pads[src].selEnd;vol=g_pads[src].volume;vh=__atomic_load_n(&g_pads[src].volumeHalfPct,__ATOMIC_RELAXED);ph=__atomic_load_n(&g_pads[src].pitchHalf,__ATOMIC_RELAXED);state=g_pads[src].loadState;old=g_pads[dst].sample;g_pads[dst].sample=srcSample;wl_wcpy(g_pads[dst].path,path,MAX_PATH_W);wl_wcpy(g_pads[dst].name,name,32);g_pads[dst].selStart=a;g_pads[dst].selEnd=b;g_pads[dst].volume=vol;g_pads[dst].loadState=srcSample?PAD_READY:(path[0]?state:PAD_EMPTY);spin_unlock();__atomic_store_n(&g_pads[dst].volumeHalfPct,vh,__ATOMIC_RELEASE);__atomic_store_n(&g_pads[dst].pitchHalf,ph,__ATOMIC_RELEASE);sample_release(old);if(!srcSample&&path[0]&&state==PAD_LOADING)schedule_load_pad(dst,path,1);save_config();update_pad_button(dst);update_diagnostics();set_status_ascii("Pad duplicated. The destination hotkey was intentionally preserved.");
}
static void show_pad_context_menu(int pad,int sx,int sy){
 if(!pCreatePopupMenu||!pAppendMenuW||!pTrackPopupMenu||!pDestroyMenu)return;select_pad(pad);void*m=pCreatePopupMenu();void*d=pCreatePopupMenu();if(!m)return;WL_WCHAR path[MAX_PATH_W];WL_U32 vk;Sample*sp;spin_lock();wl_wcpy(path,g_pads[pad].path,MAX_PATH_W);vk=g_pads[pad].vk;sp=g_pads[pad].sample;spin_unlock();
 pAppendMenuW(m,MF_STRING,ID_HOTKEY,(const WL_WCHAR[]){'S','e','t',' ','H','o','t','k','e','y','.','.','.',0});pAppendMenuW(m,MF_STRING|(vk?MF_ENABLED:MF_GRAYED),ID_CLEAR_HOTKEY,(const WL_WCHAR[]){'C','l','e','a','r',' ','H','o','t','k','e','y',0});pAppendMenuW(m,MF_SEPARATOR,0,0);
 pAppendMenuW(m,MF_STRING,ID_LOAD,(const WL_WCHAR[]){'L','o','a','d',' ','/',' ','R','e','p','l','a','c','e',' ','A','u','d','i','o','.','.','.',0});pAppendMenuW(m,MF_STRING|(path[0]?MF_ENABLED:MF_GRAYED),ID_RELOAD_AUDIO,(const WL_WCHAR[]){'R','e','l','o','a','d',' ','A','u','d','i','o',0});pAppendMenuW(m,MF_STRING|((path[0]||sp)?MF_ENABLED:MF_GRAYED),ID_REMOVE_AUDIO,(const WL_WCHAR[]){'R','e','m','o','v','e',' ','A','u','d','i','o',0});pAppendMenuW(m,MF_SEPARATOR,0,0);
 pAppendMenuW(m,MF_STRING,ID_CTX_RENAME,(const WL_WCHAR[]){'R','e','n','a','m','e',' ','P','a','d','.','.','.',0});pAppendMenuW(m,MF_STRING,ID_CTX_RESET_VOL,(const WL_WCHAR[]){'R','e','s','e','t',' ','V','o','l','u','m','e',' ','t','o',' ','1','0','0','%',0});pAppendMenuW(m,MF_STRING,ID_CTX_RESET_PITCH,(const WL_WCHAR[]){'R','e','s','e','t',' ','P','i','t','c','h',' ','t','o',' ','0',0});pAppendMenuW(m,MF_STRING|(sp?MF_ENABLED:MF_GRAYED),ID_FULL_RANGE,(const WL_WCHAR[]){'R','e','s','e','t',' ','P','l','a','y','b','a','c','k',' ','R','a','n','g','e',0});pAppendMenuW(m,MF_SEPARATOR,0,0);
 pAppendMenuW(m,MF_STRING|(path[0]?MF_ENABLED:MF_GRAYED),ID_CTX_OPEN_LOCATION,(const WL_WCHAR[]){'O','p','e','n',' ','F','i','l','e',' ','L','o','c','a','t','i','o','n',0});
 if(d){for(int i=0;i<PAD_COUNT;i++){WL_WCHAR n[24];n[0]=0;wadd_ascii(n,24,"Pad ");wadd_num(n,24,i+1);pAppendMenuW(d,MF_STRING|(i==pad?MF_GRAYED:MF_ENABLED),(WL_UPTR)(ID_DUP_BASE+i),n);}pAppendMenuW(m,MF_POPUP,(WL_UPTR)d,(const WL_WCHAR[]){'D','u','p','l','i','c','a','t','e',' ','P','a','d',' ','T','o','.','.','.',0});}
 pAppendMenuW(m,MF_SEPARATOR,0,0);pAppendMenuW(m,MF_STRING,ID_RESET_PAD,(const WL_WCHAR[]){'R','e','s','e','t',' ','P','a','d','.','.','.',0});if(sx==-1||sy==-1){sx=120;sy=120;}WL_UINT cmd=pTrackPopupMenu(m,TPM_RIGHTBUTTON|TPM_RETURNCMD,sx,sy,0,g_main,0);if(cmd)g_api.SendMessageW(g_main,WM_COMMAND,(WL_WPARAM)cmd,0);pDestroyMenu(m);
}
static void monitor_test(void){monitor_set_enabled(1);WL_U32 r=__atomic_load_n(&g_monitorDeviceRate,__ATOMIC_RELAXED);if(!r)r=48000;__atomic_store_n(&g_monitorTestFrames,r/2u,__ATOMIC_RELEASE);set_status_ascii("Monitor test: playing a short local-only tone. It is not sent through the microphone stream.");}

static WL_LRESULT WL_CALLBACK wndproc(WL_HWND hwnd,WL_UINT msg,WL_WPARAM wp,WL_LPARAM lp){
 if(msg==WM_CREATE){
  g_main=hwnd;g_font=(WL_HFONT)g_api.GetStockObject(DEFAULT_GUI_FONT);g_windowBg=g_api.CreateSolidBrush(RGB_(248,248,248));g_waveBg=g_api.CreateSolidBrush(RGB_(255,255,255));g_waveSel=g_api.CreateSolidBrush(RGB_(205,225,248));g_waveBorder=g_api.CreateSolidBrush(RGB_(105,105,105));g_controlPanelBg=g_api.CreateSolidBrush(RGB_(38,40,45));g_controlTrack=g_api.CreateSolidBrush(RGB_(83,88,96));g_controlFill=g_api.CreateSolidBrush(RGB_(226,228,232));g_wavePen=g_api.CreatePen(0,1,RGB_(25,25,25));g_waveMidPen=g_api.CreatePen(0,1,RGB_(180,180,180));g_playheadPen=g_api.CreatePen(0,2,RGB_(220,45,45));
  create_child(&g_tabSoundButton,BTN_CLASS,(const WL_WCHAR[]){'S','o','u','n','d','b','o','a','r','d',0},BS_PUSHBUTTON,0,0,10,10,ID_TAB_SOUND);create_child(&g_tabDiagButton,BTN_CLASS,(const WL_WCHAR[]){'D','i','a','g','n','o','s','t','i','c','s',0},BS_PUSHBUTTON,0,0,10,10,ID_TAB_DIAG);
  for(int i=0;i<PAD_COUNT;i++){create_child(&g_padButtons[i],BTN_CLASS,(const WL_WCHAR[]){0},BS_PUSHBUTTON|BS_MULTILINE|BS_NOTIFY,0,0,10,10,ID_PAD_BASE+i);update_pad_button(i);}create_child(&g_selectedText,STATIC_CLASS,(const WL_WCHAR[]){0},SS_LEFT,0,0,10,10,500);create_child(&g_waveLabel,STATIC_CLASS,(const WL_WCHAR[]){'W','a','v','e','f','o','r','m',':',' ','d','r','a','g',' ','t','o',' ','s','e','l','e','c','t',' ','w','h','a','t',' ','p','l','a','y','s','.',' ','T','h','e',' ','l','i','n','e',' ','s','h','o','w','s',' ','l','i','v','e',' ','p','l','a','y','b','a','c','k','.',0},SS_LEFT,0,0,10,10,503);create_child(&g_fileInfoText,STATIC_CLASS,(const WL_WCHAR[]){0},SS_LEFT,0,0,10,10,514);
  create_child(&g_monitorButton,BTN_CLASS,(const WL_WCHAR[]){'M','o','n','i','t','o','r',':',' ','O','F','F',0},BS_PUSHBUTTON,0,0,10,10,ID_TOGGLE_MONITOR);create_child(&g_monitorOutputLabel,STATIC_CLASS,(const WL_WCHAR[]){'M','o','n','i','t','o','r',' ','o','u','t','p','u','t',':',0},SS_LEFT,0,0,10,10,515);create_child(&g_monitorDeviceCombo,COMBO_CLASS,(const WL_WCHAR[]){0},CBS_DROPDOWNLIST|WS_VSCROLL,0,0,10,200,ID_MONITOR_DEVICE);create_child(&g_monitorVolumeText,STATIC_CLASS,(const WL_WCHAR[]){0},SS_LEFT,0,0,10,10,516);
  create_child(&g_padNameLabel,STATIC_CLASS,(const WL_WCHAR[]){'P','a','d',' ','n','a','m','e',':',0},SS_LEFT,0,0,10,10,507);create_child(&g_nameEdit,EDIT_CLASS,(const WL_WCHAR[]){0},WS_BORDER|ES_AUTOHSCROLL,0,0,10,10,508);create_child(&g_applyNameButton,BTN_CLASS,(const WL_WCHAR[]){'A','p','p','l','y',' ','N','a','m','e',0},BS_PUSHBUTTON,0,0,10,10,ID_RENAME);create_child(&g_statusText,STATIC_CLASS,(const WL_WCHAR[]){0},SS_LEFT,0,0,10,10,502);
  const WL_WCHAR* titles[6]={(const WL_WCHAR[]){'A','u','d','i','o',' ','E','n','g','i','n','e',0},(const WL_WCHAR[]){'O','u','t','p','u','t',0},(const WL_WCHAR[]){'L','o','c','a','l',' ','M','o','n','i','t','o','r',0},(const WL_WCHAR[]){'S','y','s','t','e','m',0},(const WL_WCHAR[]){'S','e','s','s','i','o','n',0},(const WL_WCHAR[]){'S','t','a','t','u','s',0}};for(int i=0;i<6;i++){create_child(&g_diagCards[i],BTN_CLASS,titles[i],BS_GROUPBOX,0,0,10,10,600+i);create_child(&g_diagValues[i],STATIC_CLASS,(const WL_WCHAR[]){0},SS_LEFT,0,0,10,10,620+i);}
  build_main_menu();g_activeTab=(g_savedTab==1)?1:0;update_tab_buttons();monitor_populate_devices();for(int i=0;i<PAD_COUNT;i++)if(g_pads[i].vk)register_pad_hotkey(i);register_stop_hotkey();register_mute_hotkey();for(int i=0;i<PAD_COUNT;i++)update_pad_button(i);update_selected_text();update_name_edit();update_file_info();update_monitor_button();update_monitor_volume_text();reset_session_stats();update_diagnostics();layout_ui();schedule_config_samples();if(pSetTimer){pSetTimer(hwnd,DIAG_TIMER_ID,1000,0);pSetTimer(hwnd,PLAYHEAD_TIMER_ID,40,0);}if(diag_hotkey_conflicts()>0)set_status_ascii("Hotkey conflict detected. Right-click the affected pad to change or clear its shortcut.");else if(g_backupState==2)set_status_ascii("Config recovery: the backup was loaded because the main config was invalid.");else if(g_configMigratedFrom)set_status_ascii("Configuration migrated to v0.8.7 format. Samples are loading in the background.");else set_status_ascii("Ready. Left-click selects, double-click plays, right-click opens pad options.");return 0;
 }
 if(msg==WM_GETMINMAXINFO){MON_MINMAXINFO*mi=(MON_MINMAXINFO*)lp;if(mi){mi->ptMinTrackSize.x=ui_scale(760);mi->ptMinTrackSize.y=ui_scale(760);}return 0;}
 if(msg==WM_SIZE){g_clientW=(int)LOWORD_(lp);g_clientH=(int)HIWORD_(lp);if(g_clientW>0&&g_clientH>0)layout_ui();return 0;}
 if(msg==WM_DPICHANGED){layout_ui();return 0;}
 if(msg==WM_CONTEXTMENU){for(int i=0;i<PAD_COUNT;i++)if((WL_HWND)(WL_UPTR)wp==g_padButtons[i]){show_pad_context_menu(i,(WL_I16)LOWORD_(lp),(WL_I16)HIWORD_(lp));return 0;}}
 if(msg==WM_CONN_UPDATE){update_diagnostics();return 0;}if(msg==WM_PROTOCOL_STATUS){update_diagnostics();set_status_ascii("Protocol mismatch detected. Replace the VST DLL and controller with compatible protocol-v3 builds.");return 0;}if(msg==WM_MONITOR_STATUS){update_monitor_button();update_diagnostics();set_status_ascii("Local monitor could not open the selected Windows playback device. Try Refresh Playback Devices, Windows Default, or another output.");return 0;}if(msg==WM_HOST_RATE){schedule_resample_all((WL_U32)wp);return 0;}
 if(msg==WM_LOAD_DONE){int i=(int)wp;if(i>=0&&i<PAD_COUNT){int r=(int)lp;update_pad_button(i);if(i==g_selected){update_selected_text();update_file_info();g_lastPlayheadX=-1;invalidate_wave();}if(r==1){save_config();set_status_ascii("Sample ready. Host-rate conversion is applied automatically when useful.");}else if(r==0)set_status_ascii("Audio decode failed. For OGG/Opus this can mean an unsupported/corrupt stream or unavailable Windows codec.");else if(r==-1)set_status_ascii("Audio file is missing. Use Tools > Relocate Missing Sounds after moving a sound folder.");}else{for(int k=0;k<PAD_COUNT;k++)update_pad_button(k);update_file_info();g_lastPlayheadX=-1;invalidate_wave();set_status_ascii("Host-rate sample conversion finished in the background.");}update_diagnostics();return 0;}
 if(msg==WM_PLAY_STATE){int i=(int)wp;if(i>=0&&i<PAD_COUNT){update_pad_button(i);if(i==g_selected){g_lastPlayheadX=-1;invalidate_wave();}}return 0;}
 if(msg==WM_TIMER&&wp==DIAG_TIMER_ID){if(!pIsIconic||!pIsIconic(hwnd))update_diagnostics();return 0;}if(msg==WM_TIMER&&wp==FAST_TIMER_ID)return 0;if(msg==WM_TIMER&&wp==PLAYHEAD_TIMER_ID){if(g_activeTab==0&&(!pIsIconic||!pIsIconic(hwnd))&&__atomic_load_n(&g_activeVoices[g_selected],__ATOMIC_RELAXED)>0)invalidate_playhead_delta();return 0;}
 if(msg==WM_ERASEBKGND)return 1;if(msg==WM_PAINT){WL_PAINTSTRUCT ps;WL_HDC dc=g_api.BeginPaint(hwnd,&ps);if(dc)paint_custom(dc,&ps.rcPaint);g_api.EndPaint(hwnd,&ps);return 0;}
 if(g_activeTab==0&&msg==WM_LBUTTONDOWN){int x=(WL_I16)LOWORD_(lp),y=(WL_I16)HIWORD_(lp);Sample*sp=0;spin_lock();sp=g_pads[g_selected].sample;spin_unlock();if(in_wave(x,y)&&sp){g_waveDragging=1;g_waveAnchor=wave_frac(x);g_api.SetCapture(hwnd);apply_wave_drag(g_waveAnchor,0);return 0;}if(in_volume_control(x,y)){g_controlDragging=1;g_api.SetCapture(hwnd);set_volume_from_x(x,0);return 0;}if(in_pitch_control(x,y)){g_controlDragging=2;g_api.SetCapture(hwnd);set_pitch_from_x(x,0);return 0;}if(in_monitor_control(x,y)){g_controlDragging=3;g_api.SetCapture(hwnd);set_monitor_volume_from_x(x,0);return 0;}}
 if(g_activeTab==0&&msg==WM_MOUSEMOVE&&(wp&MK_LBUTTON)){int x=(WL_I16)LOWORD_(lp);if(g_waveDragging){apply_wave_drag(wave_frac(x),0);return 0;}if(g_controlDragging==1){set_volume_from_x(x,0);return 0;}if(g_controlDragging==2){set_pitch_from_x(x,0);return 0;}if(g_controlDragging==3){set_monitor_volume_from_x(x,0);return 0;}}
 if(g_activeTab==0&&msg==WM_LBUTTONUP){int x=(WL_I16)LOWORD_(lp);if(g_waveDragging){g_waveDragging=0;g_api.ReleaseCapture();apply_wave_drag(wave_frac(x),1);set_status_ascii("Playback range saved.");return 0;}if(g_controlDragging==1){g_controlDragging=0;g_api.ReleaseCapture();set_volume_from_x(x,1);set_status_ascii("Volume saved. Active voices follow it live with smoothing.");return 0;}if(g_controlDragging==2){g_controlDragging=0;g_api.ReleaseCapture();set_pitch_from_x(x,1);set_status_ascii("Pitch saved. Active voices follow it live with smoothing.");return 0;}if(g_controlDragging==3){g_controlDragging=0;g_api.ReleaseCapture();set_monitor_volume_from_x(x,1);return 0;}}
 if(g_activeTab==0&&msg==WM_LBUTTONDBLCLK){int x=(WL_I16)LOWORD_(lp),y=(WL_I16)HIWORD_(lp);if(in_wave(x,y)){reset_wave_range();return 0;}if(in_volume_control(x,y)){reset_volume();return 0;}if(in_pitch_control(x,y)){reset_pitch();return 0;}if(in_monitor_control(x,y)){reset_monitor_volume();return 0;}}
 if(msg==WM_COMMAND){int code=HIWORD_(wp),id=LOWORD_(wp);if((id==ID_TAB_SOUND||id==ID_TAB_DIAG)&&code==BN_CLICKED){int newTab=(id==ID_TAB_DIAG)?1:0;if(newTab!=g_activeTab){hide_all_page_controls();g_pageVisibilityTab=-1;g_activeTab=newTab;g_savedTab=g_activeTab;update_tab_buttons();/* Clear the previous page while all page children are hidden. This is required because themed group boxes/statics can be transparent. */WL_RECT clr={0,0,g_clientW,g_clientH};g_api.InvalidateRect(g_main,&clr,WL_TRUE);g_api.UpdateWindow(g_main);layout_ui();force_full_page_repaint();}if(g_activeTab)update_diagnostics();else{update_selected_text();update_file_info();invalidate_wave();invalidate_controls();}return 0;}if(id>=ID_PAD_BASE&&id<ID_PAD_BASE+PAD_COUNT){int i=id-ID_PAD_BASE;if(code==BN_CLICKED||code==BN_DOUBLECLICKED){select_pad(i);Sample*sp=0;WL_I32 st;spin_lock();sp=g_pads[i].sample;st=g_pads[i].loadState;spin_unlock();if(code==BN_DOUBLECLICKED&&sp){trigger_pad(i);set_status_ascii(__atomic_load_n(&g_boardMuted,__ATOMIC_RELAXED)?"Board is muted; trigger ignored.":"Playing selected waveform range.");}else if(st==PAD_LOADING)set_status_ascii("This pad is still loading in the background.");else if(st==PAD_MISSING)set_status_ascii("This pad's file is missing. Use Tools > Relocate Missing Sounds.");else if(!sp)set_status_ascii("Empty pad selected. Right-click it to load audio.");else set_status_ascii("Pad selected. Right-click it for hotkey/audio/reset actions.");return 0;}return 0;}if(id==ID_MONITOR_DEVICE&&code==CBN_SELCHANGE){monitor_device_changed();update_diagnostics();return 0;}if(code!=BN_CLICKED)return 0;
  if(id>=ID_DUP_BASE&&id<ID_DUP_BASE+PAD_COUNT){duplicate_selected_pad_to(id-ID_DUP_BASE);return 0;}if(id==ID_LOAD){choose_file_for_pad(g_selected);update_file_info();return 0;}if(id==ID_REMOVE_AUDIO){remove_audio_selected();return 0;}if(id==ID_RELOAD_AUDIO){reload_audio_selected();return 0;}if(id==ID_RESET_PAD){reset_selected_pad();return 0;}if(id==ID_HOTKEY){g_capturePad=g_selected;suspend_hotkeys_for_capture();g_api.SetFocus(g_main);set_status_ascii("Press the key combination for this pad. Existing soundboard hotkeys are paused while capturing. Esc cancels.");update_diagnostics();return 0;}if(id==ID_CLEAR_HOTKEY){g_api.UnregisterHotKey(g_main,HOTKEY_BASE+g_selected);g_pads[g_selected].vk=0;g_pads[g_selected].mods=0;g_hotkeyState[g_selected]=0;save_config();update_pad_button(g_selected);update_selected_text();update_diagnostics();set_status_ascii("Pad hotkey cleared.");return 0;}if(id==ID_FULL_RANGE){reset_wave_range();return 0;}if(id==ID_STOP_ALL){stop_all_playback();set_status_ascii("All soundboard playback stopped.");return 0;}if(id==ID_CTX_RENAME){g_api.SetFocus(g_nameEdit);set_status_ascii("Edit the selected pad name, then click Apply Name.");return 0;}if(id==ID_CTX_RESET_VOL){reset_volume();return 0;}if(id==ID_CTX_RESET_PITCH){reset_pitch();return 0;}if(id==ID_CTX_OPEN_LOCATION){open_selected_file_location();return 0;}if(id==ID_RENAME){WL_WCHAR n[32];n[0]=0;g_api.GetWindowTextW(g_nameEdit,n,32);spin_lock();wl_wcpy(g_pads[g_selected].name,n,32);spin_unlock();save_config();update_pad_button(g_selected);update_selected_text();set_status_ascii(n[0]?"Pad name saved.":"Pad name cleared; filename will be shown.");return 0;}if(id==ID_SET_STOP_HOTKEY){g_capturePad=PAD_COUNT;suspend_hotkeys_for_capture();g_api.SetFocus(g_main);set_status_ascii("Press the key combination for global Stop All. Esc cancels.");update_diagnostics();return 0;}if(id==ID_CLEAR_STOP_HOTKEY){g_api.UnregisterHotKey(g_main,HOTKEY_STOP);g_stopVk=0;g_stopMods=0;g_stopHotkeyState=0;save_config();update_diagnostics();set_status_ascii("Global Stop All hotkey cleared.");return 0;}if(id==ID_TOGGLE_MUTE){toggle_board_mute();update_diagnostics();return 0;}if(id==ID_SET_MUTE_HOTKEY){g_capturePad=PAD_COUNT+1;suspend_hotkeys_for_capture();g_api.SetFocus(g_main);set_status_ascii("Press the key combination for Board Mute toggle. Esc cancels.");update_diagnostics();return 0;}if(id==ID_CLEAR_MUTE_HOTKEY){g_api.UnregisterHotKey(g_main,HOTKEY_STOP+1);g_muteVk=0;g_muteMods=0;g_muteHotkeyState=0;save_config();update_diagnostics();set_status_ascii("Board Mute hotkey cleared.");return 0;}if(id==ID_RELOCATE_MISSING){relocate_missing_sounds();return 0;}if(id==ID_SELF_TEST){run_self_test();return 0;}if(id==ID_RESET_STATS){reset_session_stats();update_diagnostics();return 0;}if(id==ID_TOGGLE_MONITOR){monitor_set_enabled(!__atomic_load_n(&g_monitorEnabled,__ATOMIC_ACQUIRE));update_diagnostics();return 0;}if(id==ID_REFRESH_DEVICES){monitor_populate_devices();__atomic_store_n(&g_monitorReopen,1,__ATOMIC_RELEASE);set_status_ascii("Playback device list refreshed.");return 0;}if(id==ID_MONITOR_TEST){monitor_test();return 0;}
 }
 if((msg==WM_KEYDOWN||msg==WM_SYSKEYDOWN)&&g_capturePad>=0){WL_U32 vk=(WL_U32)wp;if(vk==0x1b){g_capturePad=-1;restore_hotkeys_after_capture();update_diagnostics();set_status_ascii("Hotkey capture cancelled.");return 0;}if(vk==VK_SHIFT||vk==VK_CONTROL||vk==VK_MENU||vk==VK_LWIN||vk==VK_RWIN)return 0;WL_U32 mods=0;if(g_api.GetKeyState(VK_CONTROL)<0)mods|=MOD_CONTROL;if(g_api.GetKeyState(VK_MENU)<0)mods|=MOD_ALT;if(g_api.GetKeyState(VK_SHIFT)<0)mods|=MOD_SHIFT;if(g_api.GetKeyState(VK_LWIN)<0||g_api.GetKeyState(VK_RWIN)<0)mods|=MOD_WIN;int target=g_capturePad;g_capturePad=-1;finish_hotkey_capture(target,vk,mods);update_selected_text();update_diagnostics();return 0;}
 if(msg==WM_HOTKEY){if(g_capturePad>=0||g_hotkeysSuspended)return 0;int id=(int)wp;if(id>=HOTKEY_BASE&&id<HOTKEY_BASE+PAD_COUNT){trigger_pad(id-HOTKEY_BASE);return 0;}if(id==HOTKEY_STOP){stop_all_playback();set_status_ascii("All soundboard playback stopped by global hotkey.");return 0;}if(id==HOTKEY_STOP+1){toggle_board_mute();update_diagnostics();return 0;}}
 if(msg==WM_DESTROY){save_config();if(pKillTimer){pKillTimer(hwnd,DIAG_TIMER_ID);pKillTimer(hwnd,FAST_TIMER_ID);pKillTimer(hwnd,PLAYHEAD_TIMER_ID);}__atomic_store_n(&g_monitorEnabled,0,__ATOMIC_RELEASE);g_running=0;for(int i=0;i<PAD_COUNT;i++)g_api.UnregisterHotKey(g_main,HOTKEY_BASE+i);g_api.UnregisterHotKey(g_main,HOTKEY_STOP);g_api.UnregisterHotKey(g_main,HOTKEY_STOP+1);if(g_windowBg)g_api.DeleteObject((WL_HGDIOBJ)g_windowBg);if(g_waveBg)g_api.DeleteObject((WL_HGDIOBJ)g_waveBg);if(g_waveSel)g_api.DeleteObject((WL_HGDIOBJ)g_waveSel);if(g_waveBorder)g_api.DeleteObject((WL_HGDIOBJ)g_waveBorder);if(g_controlPanelBg)g_api.DeleteObject((WL_HGDIOBJ)g_controlPanelBg);if(g_controlTrack)g_api.DeleteObject((WL_HGDIOBJ)g_controlTrack);if(g_controlFill)g_api.DeleteObject((WL_HGDIOBJ)g_controlFill);if(g_wavePen)g_api.DeleteObject((WL_HGDIOBJ)g_wavePen);if(g_waveMidPen)g_api.DeleteObject((WL_HGDIOBJ)g_waveMidPen);if(g_playheadPen)g_api.DeleteObject((WL_HGDIOBJ)g_playheadPen);destroy_backbuffer();g_api.PostQuitMessage(0);return 0;}return g_api.DefWindowProcW(hwnd,msg,wp,lp);
}
void WL_CALLBACK entry(void){
 if(!wl_init_gui(&g_api))return;init_optional_apis();if(pSetProcessDpiAwarenessContext)pSetProcessDpiAwarenessContext((void*)(WL_IPTR)-4);else if(pSetProcessDPIAware)pSetProcessDPIAware();
 static const WL_WCHAR MUTEX_NAME[]={'L','o','c','a','l',92,'A','P','O','S','o','u','n','d','b','o','a','r','d','C','o','n','t','r','o','l','l','e','r','_','v','2',0};WL_HANDLE mx=g_api.CreateMutexW(0,WL_TRUE,MUTEX_NAME);if(mx&&g_api.GetLastError()==183){g_api.CloseHandle(mx);g_api.ExitProcess(0);}load_config();WL_HANDLE srv=g_api.CreateThread(0,0,server_thread,0,0,0);if(srv)g_api.CloseHandle(srv);
 WL_WNDCLASSEXW wc;memset(&wc,0,sizeof(wc));wc.cbSize=sizeof(wc);wc.style=CS_DBLCLKS;wc.lpfnWndProc=wndproc;wc.hInstance=(WL_HINSTANCE)g_api.GetModuleHandleW(0);wc.hbrBackground=0;wc.lpszClassName=CLASS_NAME;if(!g_api.RegisterClassExW(&wc)){if(mx)g_api.CloseHandle(mx);mf_shutdown();g_api.ExitProcess(2);}
 int ww=g_savedWindowW+ui_scale(20),wh=g_savedWindowH+ui_scale(80);if(ww<ui_scale(780))ww=ui_scale(780);if(wh<ui_scale(800))wh=ui_scale(800);WL_HWND w=g_api.CreateWindowExW(0,CLASS_NAME,WINDOW_TITLE,WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN,100,30,ww,wh,0,0,wc.hInstance,0);if(!w){if(mx)g_api.CloseHandle(mx);mf_shutdown();g_api.ExitProcess(3);}g_api.ShowWindow(w,SW_SHOW);g_api.UpdateWindow(w);
 WL_MSG m;while(g_api.GetMessageW(&m,0,0,0)>0){g_api.TranslateMessage(&m);g_api.DispatchMessageW(&m);}for(int tries=0;tries<300;tries++){if(__atomic_load_n(&g_connections,__ATOMIC_ACQUIRE)==0&&__atomic_load_n(&g_loadWorkers,__ATOMIC_ACQUIRE)==0&&__atomic_load_n(&g_resampleWorkerRunning,__ATOMIC_ACQUIRE)==0)break;g_api.Sleep(10);}int safe=__atomic_load_n(&g_connections,__ATOMIC_ACQUIRE)==0&&__atomic_load_n(&g_loadWorkers,__ATOMIC_ACQUIRE)==0&&__atomic_load_n(&g_resampleWorkerRunning,__ATOMIC_ACQUIRE)==0;monitor_shutdown();if(safe){for(int i=0;i<PAD_COUNT;i++){Sample*s=0;spin_lock();s=g_pads[i].sample;g_pads[i].sample=0;spin_unlock();sample_release(s);}drain_retired_samples();mf_shutdown();}if(mx)g_api.CloseHandle(mx);g_api.ExitProcess(0);
}