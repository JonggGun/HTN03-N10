#include "app_ipc.h"
QueueHandle_t xSensorQueue,xDisplayQueue;
AppDiagnostics g_diag;
BaseType_t app_ipc_init(void) {
    xSensorQueue=xQueueCreate(8,sizeof(uint16_t));
    xDisplayQueue=xQueueCreate(1,sizeof(AirReading));
    return (xSensorQueue && xDisplayQueue) ? pdPASS : pdFAIL;
}
