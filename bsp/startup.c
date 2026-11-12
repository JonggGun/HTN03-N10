#include <stdint.h>
extern int main(void);
extern void bsp_panic(void);
extern void SVC_Handler(void),PendSV_Handler(void),SysTick_Handler(void),USART1_IRQHandler(void);
void Default_Handler(void) { bsp_panic(); }
#if defined(__ARMCC_VERSION)
/* Absolute symbols required by MicroLIB; reserved by the scatter file. */
__asm(".global __initial_sp\n.equ __initial_sp, 0x20005000\n"
      ".global __heap_base\n.equ __heap_base, 0x20004B00\n"
      ".global __heap_limit\n.equ __heap_limit, 0x20004C00\n");
extern void __main(void);
__attribute__((naked,noreturn)) void Reset_Handler(void) { __asm volatile("b __main"); }
#else
extern uint32_t _sidata,_sdata,_edata,_sbss,_ebss;
void Reset_Handler(void) {
    uint32_t *src=&_sidata,*dst;
    for(dst=&_sdata;dst<&_edata;) *dst++=*src++;
    for(dst=&_sbss;dst<&_ebss;) *dst++=0u;
    (void)main();for(;;) {}
}
#endif
#define D Default_Handler
__attribute__((used,section(".isr_vector"),aligned(256)))
void (*const vectors[76])(void) = {
    (void (*)(void))0x20005000u,Reset_Handler,D,D,D,D,D,0,0,0,0,SVC_Handler,D,0,PendSV_Handler,SysTick_Handler,
    D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,
    USART1_IRQHandler, /* External interrupt 37, vector 53 */
    D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D
};
