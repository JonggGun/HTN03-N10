#ifndef HAL_BOARD_H
#define HAL_BOARD_H
#include <stdint.h>
#include <stddef.h>
#include "FreeRTOS.h"
int hal_adc_init(void);
int hal_adc_read(uint16_t *value);
void hal_alarm_set(int active);
int hal_uart_init(void);
void hal_uart_start_rx(void);
int hal_uart_get(char *ch);
/* RX error flag; caller must discard its partial command and wait for newline. */
int hal_uart_rx_error(void);
int hal_uart_write(const char *text);
int hal_i2c_init(void);
int hal_i2c_write(uint8_t address,uint8_t control,const uint8_t *data,size_t n);
int hal_oled_init(void);
int hal_oled_show(uint16_t adc,uint8_t level,const uint16_t thresholds[3]);
#endif
