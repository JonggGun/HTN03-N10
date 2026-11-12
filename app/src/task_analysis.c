#include "task_analysis.h"
#include "task.h"
#include "app_ipc.h"
#include "hal_board.h"
#include <stdio.h>
static void analysis_task(void *arg) {
    AirReading reading={0,0,{{DEFAULT_T1,DEFAULT_T2,DEFAULT_T3}}};
    char command[40],reply[64],ch;unsigned used=0,budget;int drop=0,valid=0;
    (void)arg;hal_uart_start_rx();
    for(;;) {
        if(hal_uart_rx_error()) {drop=1;used=0;}
        /* Bounded RX work so a continuous stream cannot starve ADC analysis. */
        for(budget=0;budget<64u && hal_uart_get(&ch);budget++) {
            if(ch=='\r') continue;
            if(ch=='\n') {
                int result;
                if(hal_uart_rx_error()) drop=1;
                command[used]='\0';
                result=drop ? 0 : air_command(command,&reading.limits);
                if(result==1) {
                    (void)snprintf(reply,sizeof(reply),"THR,%u,%u,%u\r\n",
                        (unsigned)reading.limits.t[0],(unsigned)reading.limits.t[1],(unsigned)reading.limits.t[2]);
                    hal_uart_write(reply);
                } else {
                    hal_uart_write(result==2 ? "OK\r\n" : "ERR\r\n");
                    if(result==2 && valid) {
                        reading.level=air_level(reading.adc,0,&reading.limits,0);
                        hal_alarm_set(reading.level==3u);xQueueOverwrite(xDisplayQueue,&reading);
                    }
                }
                used=0;drop=0;
            } else if((unsigned char)ch<32u || (unsigned char)ch>126u) {drop=1;}
            else if(!drop) {
                if(used+1u<sizeof(command)) command[used++]=ch;
                else drop=1;
            }
        }
        if(xQueueReceive(xSensorQueue,&reading.adc,pdMS_TO_TICKS(20))==pdPASS) {
            reading.level=air_level(reading.adc,reading.level,&reading.limits,valid);
            valid=1;hal_alarm_set(reading.level==3u);
            xQueueOverwrite(xDisplayQueue,&reading);
        }
        g_diag.analysis_stack=uxTaskGetStackHighWaterMark(NULL);
    }
}
BaseType_t task_analysis_create(void) {
    return xTaskCreate(analysis_task,"Analysis",ANALYSIS_STACK_WORDS,NULL,ANALYSIS_PRIORITY,NULL);
}
