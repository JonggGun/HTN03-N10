#include "task_display.h"
#include "task.h"
#include "app_ipc.h"
#include "hal_board.h"
#include <stdio.h>
static void display_task(void *arg) {
    AirReading reading;char line[32];int oled_ready=0;TickType_t next_retry=0;
    (void)arg;
    vTaskDelay(pdMS_TO_TICKS(100));
    for(;;) {
        if(xQueueReceive(xDisplayQueue,&reading,pdMS_TO_TICKS(200))==pdPASS) {
            (void)snprintf(line,sizeof(line),"%u,%u\r\n",(unsigned)reading.adc,(unsigned)reading.level);
            hal_uart_write(line);
            if(!oled_ready && (int32_t)(xTaskGetTickCount()-next_retry)>=0) {
                oled_ready=hal_oled_init();
                if(!oled_ready) {g_diag.oled_errors++;next_retry=xTaskGetTickCount()+pdMS_TO_TICKS(2000);}
            }
            if(oled_ready && !hal_oled_show(reading.adc,reading.level,reading.limits.t)) {
                oled_ready=0;g_diag.oled_errors++;next_retry=xTaskGetTickCount()+pdMS_TO_TICKS(2000);
            }
        }
        g_diag.display_stack=uxTaskGetStackHighWaterMark(NULL);
        g_diag.free_heap=xPortGetFreeHeapSize();
    }
}
BaseType_t task_display_create(void) {
    return xTaskCreate(display_task,"Display",DISPLAY_STACK_WORDS,NULL,DISPLAY_PRIORITY,NULL);
}
