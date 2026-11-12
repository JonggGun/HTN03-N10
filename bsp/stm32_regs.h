#ifndef STM32_REGS_H
#define STM32_REGS_H
#include <stdint.h>
#define REG32(a) (*(volatile uint32_t *)(a))
#define RCC_CR REG32(0x40021000u)
#define RCC_CFGR REG32(0x40021004u)
#define RCC_APB2ENR REG32(0x40021018u)
#define RCC_APB1ENR REG32(0x4002101Cu)
#define RCC_APB1RSTR REG32(0x40021010u)
typedef struct { volatile uint32_t CRL,CRH,IDR,ODR,BSRR,BRR,LCKR; } GPIO_Regs;
#define GPIOA ((GPIO_Regs *)0x40010800u)
#define GPIOB ((GPIO_Regs *)0x40010C00u)
#define GPIOC ((GPIO_Regs *)0x40011000u)
typedef struct { volatile uint32_t SR,DR,BRR,CR1,CR2,CR3,GTPR; } UART_Regs;
#define UART1 ((UART_Regs *)0x40013800u)
typedef struct { volatile uint32_t CR1,CR2,OAR1,OAR2,DR,SR1,SR2,CCR,TRISE; } I2C_Regs;
#define I2C1 ((I2C_Regs *)0x40005400u)
typedef struct { volatile uint32_t SR,CR1,CR2,SMPR1,SMPR2,JOFR[4],HTR,LTR,SQR1,SQR2,SQR3,JSQR,JDR[4],DR; } ADC_Regs;
#define ADC1 ((ADC_Regs *)0x40012400u)
void bsp_init(void);
void bsp_panic(void);
#endif
