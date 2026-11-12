#ifndef AIR_LOGIC_H
#define AIR_LOGIC_H
#include <stdint.h>
#include "app_cfg.h"
typedef struct { uint16_t t[3]; } AirThresholds;
typedef struct { uint16_t values[FILTER_SIZE]; uint32_t sum; unsigned pos,count; } AirFilter;
uint16_t air_filter_push(AirFilter *f,uint16_t value);
uint8_t air_level(uint16_t adc,uint8_t old,const AirThresholds *t,int hysteresis);
/* 0 invalid, 1 GET, 2 successful SET; failed SET never mutates thresholds. */
int air_command(const char *line,AirThresholds *t);
#endif
