#include <stdio.h>
#include <stdlib.h>
#include "../src/controller.c"
static void check(int ok,const char*s){if(!ok){fprintf(stderr,"FAIL: %s\n",s);exit(1);}puts(s);}
static PipeHello incoming;static PipeAck outgoing;static int reads,writes,failwrite;
static WL_BOOL WL_WINAPI rd(WL_HANDLE h,void*b,WL_DWORD n,WL_DWORD*done,void*ov){(void)h;(void)ov;if(reads++==0&&n==sizeof incoming){memcpy(b,&incoming,n);*done=n;return 1;}*done=0;return 0;}
static WL_BOOL WL_WINAPI wr(WL_HANDLE h,const void*b,WL_DWORD n,WL_DWORD*done,void*ov){(void)h;(void)ov;if(failwrite){*done=0;return 0;}if(n!=sizeof outgoing)abort();memcpy(&outgoing,b,n);++writes;*done=n;return 1;}
static WL_BOOL WL_WINAPI closed(WL_HANDLE h){(void)h;return 1;}
static void* WL_WINAPI heap(void){return 0;}
static WL_BOOL WL_WINAPI freemem(void*h,WL_DWORD f,void*p){(void)h;(void)f;free(p);return 1;}
static void attempt(PipeHello hi,int accept){incoming=hi;reads=writes=0;Client*c=calloc(1,sizeof *c);if(!c)abort();c->pipe=(WL_HANDLE)1;client_thread(c);check(writes==accept,"Actual client handshake acceptance");if(accept)check(outgoing.magic==M_ACK&&outgoing.version==3&&outgoing.build==hi.build,"ACK matches connecting DLL build");check(!g_connections,"Client count cleaned up");}
int main(void){g_api.ReadFile=rd;g_api.WriteFile=wr;g_api.CloseHandle=closed;g_api.DisconnectNamedPipe=closed;g_api.GetProcessHeap=heap;g_api.HeapFree=freemem;
check(sizeof(PipeHello)==20&&sizeof(PipeAck)==16&&sizeof(PipeRequest)==36&&sizeof(PipeAudio)==16,"Wire ABI sizes unchanged");
PipeHello h={M_HELLO,3,100218,48000,2};attempt(h,1);h.build=100219;attempt(h,1);h.build=100220;attempt(h,1);h.build=BUILD_VERSION;attempt(h,1);h.build=100217;attempt(h,0);check(g_protocolStatus==-1&&g_protocolBadBuild==100217,"Rejected build recorded");h.build=100219;h.version=2;attempt(h,0);check(g_protocolBadVersion==2,"Rejected protocol recorded");h.version=3;h.channels=1;attempt(h,0);h.channels=2;h.sampleRate=7999;attempt(h,0);h.sampleRate=384001;attempt(h,0);h.sampleRate=48000;attempt(h,1);check(g_protocolStatus==0&&g_protocolReadyCount==0,"Successful handshake clears rejection and disconnects");check(g_protocolPeerBuild==100219,"Last accepted DLL build remains available after disconnect");
r20_protocol_accept(&h);PipeHello bad=h;bad.build=7;r20_protocol_reject(&bad);check(g_protocolStatus==1&&g_protocolReadyCount==1,"Invalid peer cannot disrupt valid peer");r20_protocol_disconnect();check(g_protocolStatus==0,"Last valid peer disconnect clears ready status");failwrite=1;attempt(h,0);check(g_protocolReadyCount==0,"Failed ACK never counts as accepted");puts("PASS R20 IPC hotfix");return 0;}
