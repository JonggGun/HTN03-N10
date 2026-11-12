#include "hal_board.h"
#include "stm32_regs.h"
#include "semphr.h"
#include "queue.h"
#include "app_ipc.h"
static QueueHandle_t rx_queue;
static SemaphoreHandle_t tx_mutex;
static volatile unsigned rx_error;
int hal_uart_init(void) {
    rx_queue=xQueueCreate(128,sizeof(char));tx_mutex=xSemaphoreCreateMutex();
    if(!rx_queue||!tx_mutex) return 0;
    GPIOA->CRH=(GPIOA->CRH & ~0xFF0u)|0x4B0u; /* PA9 AF pushpull, PA10 input */
    UART1->BRR=69u; /* 8 MHz / 115200 rounded, 115942 baud */
    UART1->CR2=0u;UART1->CR3=0u;
    UART1->CR1=(1u<<13)|(1u<<3)|(1u<<2);
    return 1;
}
void hal_uart_start_rx(void) {
    volatile uint32_t discard=UART1->SR;discard=UART1->DR;(void)discard;
    ((volatile uint8_t *)0xE000E400u)[37]=6u<<4;
    REG32(0xE000E284u)=1u<<5; /* clear pending IRQ 37 */
    REG32(0xE000E104u)=1u<<5;
    UART1->CR1 |= 1u<<5;
}
void USART1_IRQHandler(void) {
    BaseType_t wake=pdFALSE;
    uint32_t sr=UART1->SR;
    if(sr & ((1u<<5)|15u)) {
        char ch=(char)UART1->DR; /* SR then DR clears receive errors */
        if(sr&15u) { rx_error=1u;g_diag.uart_rx_dropped++; }
        else if(xQueueSendFromISR(rx_queue,&ch,&wake)!=pdPASS) { rx_error=1u;g_diag.uart_rx_dropped++; }
    }
    portYIELD_FROM_ISR(wake);
}
int hal_uart_get(char *ch) { return xQueueReceive(rx_queue,ch,0)==pdPASS; }
int hal_uart_rx_error(void) {
    unsigned e;
    taskENTER_CRITICAL(); e=rx_error;rx_error=0u;taskEXIT_CRITICAL();
    if(e) xQueueReset(rx_queue);
    return (int)e;
}
int hal_uart_write(const char *text) {
    uint32_t n;int ok=1;
    if(xSemaphoreTake(tx_mutex,pdMS_TO_TICKS(30))!=pdTRUE) {g_diag.uart_tx_errors++;return 0;}
    while(*text) {
        for(n=0;n<20000u && !(UART1->SR&(1u<<7));n++) {}
        if(!(UART1->SR&(1u<<7))) {ok=0;break;}
        UART1->DR=(uint8_t)*text++;
    }
    xSemaphoreGive(tx_mutex);if(!ok) g_diag.uart_tx_errors++;
    return ok;
}
