/* Permanent 16 HWND grid: page switches update mapping, not HWND visibility. */
#include <stdio.h>
#include <stdlib.h>
#include "../src/controller.c"
static void yes(int x,const char*t){if(!x){fprintf(stderr,"FAIL %s\n",t);exit(1);}puts(t);}
int main(void){
 g_activeTab=1;g_padPage=0;
 for(int i=0;i<32;i++){yes(visible_pad_slot(i)==(i<16?i:-1),"Page 1 pad/slot mapping");}
 swap_pad_page(1);yes(g_padPage==1,"Page 2 is selected");
 for(int i=0;i<32;i++){yes(visible_pad_slot(i)==(i>=16?i-16:-1),"Page 2 pad/slot mapping");}
 swap_pad_page(0);yes(g_padPage==0,"Returning to Page 1 changes only mapping");
 for(int i=0;i<16;i++)yes(visible_pad_slot(i)==i,"Persistent pad window indices");
 puts("ALL R2 PAGE VIRTUALIZATION TESTS PASSED");return 0;
}
