#include "stm32_regs.h"
#include "FreeRTOS.h"
#include "task.h"
#include "app_ipc.h"
#include "hal_board.h"
#include "task_sensor.h"
#include "task_analysis.h"
#include "task_display.h"
int main(void) {
    bsp_init();
    if(!hal_adc_init() || !hal_uart_init() || !hal_i2c_init()) bsp_panic();
    if(app_ipc_init()!=pdPASS) bsp_panic();
    if(task_sensor_create()!=pdPASS || task_analysis_create()!=pdPASS || task_display_create()!=pdPASS) bsp_panic();
    vTaskStartScheduler();bsp_panic();return 0;
}
void vApplicationMallocFailedHook(void) { bsp_panic(); }
void vApplicationStackOverflowHook(TaskHandle_t task,char *name) { (void)task;(void)name;bsp_panic(); }
