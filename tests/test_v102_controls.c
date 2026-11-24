#include <stdio.h>
#include <stdlib.h>
#include "../src/controller.c"
static void ok(int state,const char*name){if(!state){fprintf(stderr,"FAIL: %s\n",name);exit(1);}printf("PASS: %s\n",name);}
int main(void){
 g_activeTab=1;g_padPage=0;
 swap_pad_page(1);ok(g_padPage==1,"page switching keeps selected page 2");
 swap_pad_page(0);ok(g_padPage==0,"page switching restores page 1");
 for(int page=0;page<2;page++)for(int i=0;i<32;i++)
  ok(page_shows_pad(page,i)==(i/16==page),"page button index matches correct pad");
 g_selected=18;g_fxMiniBars[0]=(WL_RECT){100,100,301,107};
 ApoFxConfig c=apo_fx_preset(1);g_fxSettings[18]=c;
 WL_U64 serial=g_fxSerial[18],restart=g_fxRestartSerial[18];
 fx_set_bar_from_x(0,100,0);ok(g_fxSettings[18].echoMs==50,"echo mini bar minimum is 50ms");
 fx_set_bar_from_x(0,300,0);ok(g_fxSettings[18].echoMs==650,"echo mini bar maximum is 650ms");
 ok(g_fxSerial[18]>serial,"changing mini bar updates audio parameter serial");
 ok(g_fxRestartSerial[18]==restart,"dragging effect bar does not clear effect tails");
 ok(fx_row_hit(200,105)==0,"mini bar hit-testing works");
 puts("ALL v1.0.2 PAGE AND MINI-BAR CHECKS PASSED");return 0;
}
