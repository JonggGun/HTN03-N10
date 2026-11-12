#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "air_logic.h"
int main(void) {
    AirThresholds t={{1000,1800,2600}},copy;AirFilter f={0};unsigned i;
    assert(air_level(999,0,&t,0)==0);assert(air_level(1000,0,&t,0)==1);
    assert(air_level(1800,0,&t,0)==2);assert(air_level(2600,0,&t,0)==3);
    assert(air_level(2550,3,&t,1)==3);assert(air_level(2549,3,&t,1)==2);
    assert(air_level(0,3,&t,1)==0);assert(air_level(4095,0,&t,1)==3);
    assert(air_filter_push(&f,100)==100);assert(air_filter_push(&f,300)==200);
    for(i=0;i<30;i++) air_filter_push(&f,4095);
    assert(air_filter_push(&f,4095)==4095);
    assert(air_command("GET",&t)==1);assert(air_command("SET T1 1100",&t)==2);
    copy=t;
    {const char *bad[]={"SET T1 0","SET T2 1100","SET T3 4096","SET T3 -1","SET T4 2000","SET T1 123x","SET T1 99999999999999999","SET T1 ","SET T","GET junk",""};
    for(i=0;i<sizeof(bad)/sizeof(bad[0]);i++){assert(air_command(bad[i],&t)==0);assert(memcmp(&copy,&t,sizeof(t))==0);}}
    assert(air_command("SET T3 4095",&t)==2);
    t.t[0]=1;t.t[1]=2;t.t[2]=3;assert(air_level(0,3,&t,1)==0);
    puts("C tests PASS: boundaries, hysteresis, filter, commands, no mutation on ERR");return 0;
}
