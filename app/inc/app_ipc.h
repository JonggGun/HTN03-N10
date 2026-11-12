#ifndef APP_IPC_H
#define APP_IPC_H
#include "FreeRTOS.h"
#include "queue.h"
#include "air_logic.h"
typedef struct { uint16_t adc; uint8_t level; AirThresholds limits; } AirReading;
extern QueueHandle_t xSensorQueue,xDisplayQueue;
typedef struct { volatile uint32_t adc_errors,sensor_dropped,uart_rx_dropped,uart_tx_errors,oled_errors; volatile unsigned sensor_stack,analysis_stack,display_stack; volatile size_t free_heap; } AppDiagnostics;
extern AppDiagnostics g_diag;
BaseType_t app_ipc_init(void);
#endif
