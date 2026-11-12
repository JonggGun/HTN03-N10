ENTRY(Reset_Handler)
MEMORY { FLASH(rx): ORIGIN = 0x08000000, LENGTH = 64K
         RAM(rwx): ORIGIN = 0x20000000, LENGTH = 19K }
SECTIONS {
 .isr_vector : { KEEP(*(.isr_vector)) } > FLASH
 .text : { *(.text*) *(.rodata*) } > FLASH
 .ARM.exidx : { *(.ARM.exidx*) } > FLASH
 _sidata = LOADADDR(.data);
 .data : { _sdata = .; *(.data*) . = ALIGN(4); _edata = .; } > RAM AT> FLASH
 .bss : { _sbss = .; *(.bss*) *(COMMON) . = ALIGN(4); _ebss = .; end = .; } > RAM
 /DISCARD/ : { *(.comment*) *(.note*) }
}
