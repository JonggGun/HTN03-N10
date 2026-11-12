#include "hal_board.h"
#include "stm32_regs.h"
#include "semphr.h"
static SemaphoreHandle_t bus_mutex;
static void bus_reset(void) {
    RCC_APB1RSTR|=1u<<21;RCC_APB1RSTR&=~(1u<<21);
    I2C1->CR2=8u;I2C1->CCR=40u;I2C1->TRISE=9u;
    I2C1->OAR1=1u<<14;I2C1->CR1=1u;
}
int hal_i2c_init(void) {
    GPIOB->CRL=(GPIOB->CRL&0x00FFFFFFu)|0xFF000000u;
    bus_mutex=xSemaphoreCreateMutex();bus_reset();return bus_mutex!=NULL;
}
static int wait_flag(uint32_t flag) {
    uint32_t n;
    for(n=0;n<20000u;n++) {
        uint32_t sr=I2C1->SR1;
        if(sr&0x0F00u) return 0;
        if(sr&flag) return 1;
    }
    return 0;
}
int hal_i2c_write(uint8_t address,uint8_t control,const uint8_t *data,size_t n) {
    uint32_t k;volatile uint32_t discard;size_t i;int ok=0;
    if(xSemaphoreTake(bus_mutex,pdMS_TO_TICKS(30))!=pdTRUE) return 0;
    for(k=0;k<20000u && (I2C1->SR2&2u);k++) {}
    if(I2C1->SR2&2u) goto done;
    I2C1->CR1 |= 1u<<8;
    if(!wait_flag(1u)) goto done;
    I2C1->DR=(uint32_t)address<<1;
    if(!wait_flag(2u)) goto done;
    discard=I2C1->SR1;discard=I2C1->SR2;(void)discard;
    if(!wait_flag(1u<<7)) goto done;
    I2C1->DR=control;
    for(i=0;i<n;i++) {
        if(!wait_flag(1u<<7)) goto done;
        I2C1->DR=data[i];
    }
    if(!wait_flag(1u<<2)) goto done;
    ok=1;
done:
    I2C1->CR1 |= 1u<<9;
    if(!ok) bus_reset();
    xSemaphoreGive(bus_mutex);return ok;
}
