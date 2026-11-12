#include "hal_board.h"
#include "stm32_regs.h"
#include "semphr.h"
static SemaphoreHandle_t adc_mutex;
int hal_adc_init(void) {
    uint32_t n;
    adc_mutex=xSemaphoreCreateMutex();
    if(!adc_mutex) return 0;
    GPIOA->CRL &= ~15u;
    ADC1->CR1=0u; ADC1->CR2=1u;
    ADC1->SMPR2=7u; /* channel 0, 239.5 cycles, ADC clock = 4 MHz */
    ADC1->SQR1=0u; ADC1->SQR2=0u; ADC1->SQR3=0u;
    /* Startup-only settling; never called from a task. */
    for(n=0;n<1000u;n++) __asm volatile("nop");
    ADC1->CR2 |= 1u<<3;
    for(n=0;n<100000u && (ADC1->CR2&(1u<<3));n++) {}
    if(ADC1->CR2&(1u<<3)) return 0;
    ADC1->CR2 |= 1u<<2;
    for(n=0;n<100000u && (ADC1->CR2&(1u<<2));n++) {}
    if(ADC1->CR2&(1u<<2)) return 0;
    ADC1->CR2 |= (7u<<17)|(1u<<20);
    return 1;
}
int hal_adc_read(uint16_t *value) {
    uint32_t n; int ok=0;
    if(xSemaphoreTake(adc_mutex,pdMS_TO_TICKS(5))!=pdTRUE) return 0;
    ADC1->SR=0u; ADC1->CR2|=1u<<22;
    for(n=0;n<20000u;n++) if(ADC1->SR&2u) { *value=(uint16_t)ADC1->DR;ok=1;break; }
    xSemaphoreGive(adc_mutex);return ok;
}
/* Single owner: AnalysisTask. */
void hal_alarm_set(int active) {
    if(active) GPIOB->BSRR=(1u<<12)|(1u<<13);
    else GPIOB->BRR=(1u<<12)|(1u<<13);
}
