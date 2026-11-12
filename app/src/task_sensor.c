#include "task_sensor.h"
#include "task.h"
#include "app_ipc.h"
#include "hal_board.h"
static void sensor_task(void *arg) {
    AirFilter filter={0};TickType_t last=xTaskGetTickCount();uint16_t raw,filtered;
#if APP_DEMO_MODE
    unsigned demo=0;
#endif
    (void)arg;
    for(;;) {
#if APP_DEMO_MODE
        demo=(demo+71u)%8190u; raw=(uint16_t)(demo<=4095u?demo:8190u-demo);
#else
        if(!hal_adc_read(&raw)) {g_diag.adc_errors++;goto next;}
#endif
        filtered=air_filter_push(&filter,raw);
        if(xQueueSend(xSensorQueue,&filtered,0)!=pdPASS) g_diag.sensor_dropped++;
#if !APP_DEMO_MODE
next:
#endif
        g_diag.sensor_stack=uxTaskGetStackHighWaterMark(NULL);
        vTaskDelayUntil(&last,pdMS_TO_TICKS(SENSOR_PERIOD_MS));
    }
}
BaseType_t task_sensor_create(void) {
    return xTaskCreate(sensor_task,"AirSensor",SENSOR_STACK_WORDS,NULL,SENSOR_PRIORITY,NULL);
}
