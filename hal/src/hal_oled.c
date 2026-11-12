#include "hal_board.h"
#include "app_cfg.h"
#include <stdio.h>
#include <string.h>
static const char glyph_chars[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ :-";
static const uint8_t glyphs[][5]={
{62,81,73,69,62},{0,66,127,64,0},{66,97,81,73,70},{33,65,69,75,49},{24,20,18,127,16},
{39,69,69,69,57},{60,74,73,73,48},{1,113,9,5,3},{54,73,73,73,54},{6,73,73,41,30},
{126,17,17,17,126},{127,73,73,73,54},{62,65,65,65,34},{127,65,65,34,28},{127,73,73,73,65},
{127,9,9,9,1},{62,65,73,73,122},{127,8,8,8,127},{0,65,127,65,0},{32,64,65,63,1},
{127,8,20,34,65},{127,64,64,64,64},{127,2,12,2,127},{127,4,8,16,127},{62,65,65,65,62},
{127,9,9,9,6},{62,65,81,33,94},{127,9,25,41,70},{38,73,73,73,50},{1,1,127,1,1},
{63,64,64,64,63},{31,32,64,32,31},{63,64,56,64,63},{99,20,8,20,99},{7,8,112,8,7},{97,81,73,69,67},
{0,0,0,0,0},{0,54,54,0,0},{8,8,8,8,8}};
static int page_text(unsigned page,const char *s) {
    uint8_t data[128]={0};uint8_t cmd[3];unsigned x=0,k;
    cmd[0]=(uint8_t)(0xB0u+page);cmd[1]=0u;cmd[2]=0x10u;
    while(*s && x+5u<128u) {
        const char *p=strchr(glyph_chars,*s++);
        if(p) for(k=0;k<5u;k++) data[x+k]=glyphs[p-glyph_chars][k];
        x+=6u;
    }
    return hal_i2c_write(OLED_ADDRESS,0,cmd,3) && hal_i2c_write(OLED_ADDRESS,0x40,data,128);
}
int hal_oled_init(void) {
    static const uint8_t cmd[]={0xAE,0xD5,0x80,0xA8,0x3F,0xD3,0x00,0x40,0x8D,0x14,0x20,0x02,
        0xA1,0xC8,0xDA,0x12,0x81,0x7F,0xD9,0xF1,0xDB,0x40,0xA4,0xA6,0xAF};
    unsigned i;
    if(!hal_i2c_write(OLED_ADDRESS,0,cmd,sizeof(cmd))) return 0;
    for(i=0;i<8u;i++) if(!page_text(i,"")) return 0;
    return 1;
}
int hal_oled_show(uint16_t adc,uint8_t level,const uint16_t thresholds[3]) {
    static const char *names[]={"TOT","TRUNG BINH","KEM","NGUY HAI"};
    char line[22];unsigned i;
    if(!page_text(0,APP_DEMO_MODE ? "DEMO - NOT SENSOR" : "AIR QUALITY")) return 0;
    (void)snprintf(line,sizeof(line),"ADC: %u",(unsigned)adc);
    if(!page_text(2,line)||!page_text(3,names[level<4u?level:0u])) return 0;
    for(i=0;i<3u;i++) {
        (void)snprintf(line,sizeof(line),"T%u: %u",i+1u,(unsigned)thresholds[i]);
        if(!page_text(5u+i,line)) return 0;
    }
    return 1;
}
