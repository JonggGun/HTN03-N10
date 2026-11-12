#include "air_logic.h"
#include <string.h>
uint16_t air_filter_push(AirFilter *f,uint16_t value) {
    if(f->count == FILTER_SIZE) f->sum -= f->values[f->pos];
    else f->count++;
    f->values[f->pos] = value; f->sum += value;
    f->pos = (f->pos+1u)%FILTER_SIZE;
    return (uint16_t)(f->sum/f->count);
}
uint8_t air_level(uint16_t adc,uint8_t old,const AirThresholds *t,int hysteresis) {
    uint8_t level = (old<=3u && hysteresis) ? old : 0u;
    while(level<3u && adc>=t->t[level]) level++;
    while(level>0u) {
        unsigned edge = t->t[level-1u];
        unsigned h = hysteresis ? HYSTERESIS_ADC : 0u;
        /* For very low valid thresholds, cap hysteresis so ADC=0 can clear. */
        if(h>=edge) h=edge-1u;
        if(adc >= edge-h) break;
        level--;
    }
    return level;
}
int air_command(const char *line,AirThresholds *t) {
    AirThresholds next = *t;
    const char *p;
    unsigned value=0u,index;
    if(strcmp(line,"GET")==0) return 1;
    if(strncmp(line,"SET T",5)!=0) return 0;
    if(line[5]<'1'||line[5]>'3'||line[6]!=' ') return 0;
    index=(unsigned)(line[5]-'1'); p=line+7;
    if(*p<'0'||*p>'9') return 0;
    while(*p>='0'&&*p<='9') {
        value=value*10u+(unsigned)(*p++-'0');
        if(value>4095u) return 0;
    }
    if(*p!='\0') return 0;
    next.t[index]=(uint16_t)value;
    if(next.t[0]==0u || next.t[0]>=next.t[1] || next.t[1]>=next.t[2]) return 0;
    *t=next;return 2;
}
