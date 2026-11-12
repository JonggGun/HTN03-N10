#include "stm32_regs.h"
/* Reset clock HSI 8 MHz; no external crystal required. Called before RTOS. */
void bsp_init(void) {
    uint32_t n;
    RCC_CR |= 1u;
    for(n=0;n<100000u && !(RCC_CR & 2u);n++) {}
    if(!(RCC_CR & 2u)) bsp_panic();
    RCC_CFGR = 0u;
    for(n=0;n<100000u && (RCC_CFGR & 12u);n++) {}
    if(RCC_CFGR & 12u) bsp_panic();
    REG32(0xE000ED08u) = 0x08000000u; /* VTOR */
    REG32(0xE000ED0Cu) = 0x05FA0300u; /* 4 preemption bits */
    RCC_APB2ENR |= (1u<<2)|(1u<<3)|(1u<<4)|(1u<<14)|(1u<<9)|1u;
    RCC_APB1ENR |= 1u<<21;
    GPIOB->CRH = (GPIOB->CRH & ~0x00FF0000u) | 0x00220000u;
    GPIOB->BRR = (1u<<12)|(1u<<13);
    GPIOC->CRH = (GPIOC->CRH & ~(15u<<20)) | (2u<<20);
    GPIOC->BSRR = 1u<<13;
}
void bsp_panic(void) {
    __asm volatile("cpsid i" ::: "memory");
    RCC_APB2ENR |= 1u<<4;
    GPIOC->CRH = (GPIOC->CRH & ~(15u<<20)) | (2u<<20);
    GPIOC->BRR = 1u<<13; /* Onboard active-low LED solid = fatal error. */
    for(;;) { __asm volatile("nop"); }
}
