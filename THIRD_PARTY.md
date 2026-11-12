# Third party source

FreeRTOS Kernel V11.1.0 is included as a minimal source subset from:
https://github.com/FreeRTOS/FreeRTOS-Kernel/tree/V11.1.0

Included files: tasks.c, queue.c, list.c, include headers, portable/GCC/ARM_CM3, portable/MemMang/heap_4.c and LICENSE.md. Original copyright notices are retained. These third party files are unmodified. Board support, application code, Python monitor, tests and instructions were created for this project.

No STM32 HAL, CMSIS device pack source, Keil compiler or proprietary binary is redistributed. The device pack and ARM Compiler 6 must be installed in the user's Keil environment. Driver register definitions follow STM32 RM0008. OLED driver assumes the SSD1306 128x64 page-addressing command set.
