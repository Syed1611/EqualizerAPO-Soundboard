/* Exercise the production registration/retry and dispatch code with mock Win32 APIs. */
#include <stdio.h>
#include <stdlib.h>
#include "../src/controller.c"
static int blocked=1,regCalls,unregCalls;
static WL_BOOL WL_WINAPI mock_register(WL_HWND h,WL_I32 id,WL_UINT mods,WL_UINT vk){(void)h;(void)id;(void)mods;(void)vk;regCalls++;return !blocked;}
static WL_BOOL WL_WINAPI mock_unregister(WL_HWND h,WL_I32 id){(void)h;(void)id;unregCalls++;return 1;}
static void check(int ok,const char*s){if(!ok){fprintf(stderr,"FAIL %s\n",s);exit(1);}puts(s);}
static Sample sample;
int main(void){
 g_api.RegisterHotKey=mock_register;g_api.UnregisterHotKey=mock_unregister;
 int p=19;g_pads[p].vk=0x76;g_pads[p].mods=MOD_CONTROL;g_stopVk=0x77;g_stopMods=MOD_ALT;
 check(!register_pad_hotkey(p)&&g_hotkeyState[p]==-1,"Failed Windows registration is recorded");
 check(!register_stop_hotkey()&&g_stopHotkeyState==-1,"Stop registration failure is recorded");
 blocked=0;g_capturePad=p;int before=regCalls;
 check(!retry_hotkeys()&&regCalls==before,"No retries during capture");
 g_capturePad=-1;g_hotkeysSuspended=1;check(!retry_hotkeys()&&regCalls==before,"No retries during suspension");
 g_hotkeysSuspended=0;check(retry_hotkeys()==2&&g_hotkeyState[p]==1&&g_stopHotkeyState==1,"Rejected registrations recover after conflict clears");
 before=regCalls;int unreg=unregCalls;check(!retry_hotkeys()&&regCalls==before&&unregCalls==unreg,"Successful registrations not churned");
 g_connections=1;g_protocolStatus=1;g_pads[p].sample=&sample;WL_U32 lp=(0x76u<<16)|MOD_CONTROL;
 dispatch_global_hotkey(HOTKEY_BASE+p,lp);check(g_triggerSerial[p]==1,"Page 2 global hotkey reaches actual playback serial");
 dispatch_global_hotkey(HOTKEY_BASE+p,(0x75u<<16)|MOD_CONTROL);check(g_triggerSerial[p]==1,"Stale key ignored");
 dispatch_global_hotkey(HOTKEY_BASE+p,(0x76u<<16)|MOD_ALT);check(g_triggerSerial[p]==1,"Wrong modifiers ignored");
 g_capturePad=p;dispatch_global_hotkey(HOTKEY_BASE+p,lp);g_capturePad=-1;check(g_triggerSerial[p]==1,"Capture cannot trigger playback");
 g_boardMuted=1;dispatch_global_hotkey(HOTKEY_BASE+p,lp);g_boardMuted=0;check(g_triggerSerial[p]==1,"Muted playback ignored");
 g_protocolStatus=0;dispatch_global_hotkey(HOTKEY_BASE+p,lp);check(g_triggerSerial[p]==1,"Disconnected playback not queued unexpectedly");g_protocolStatus=1;
 g_pads[p].sample=0;dispatch_global_hotkey(HOTKEY_BASE+p,lp);check(g_triggerSerial[p]==1,"Missing audio ignored");g_pads[p].sample=&sample;
 dispatch_global_hotkey(HOTKEY_BASE+p,lp);check(g_triggerSerial[p]==2,"Next hotkey works after reconnect");
 dispatch_global_hotkey(HOTKEY_STOP,(0x77u<<16)|MOD_ALT);check(g_stopSerial==1,"Stop hotkey reaches playback stop serial");
 return 0;
}
