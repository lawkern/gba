/* (c) copyright 2025 Lawrence D. Kern /////////////////////////////////////// */

OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)

MEMORY
{
    ewram (w!x) : ORIGIN = 0x02000000, LENGTH = 256K
    iwram (w!x) : ORIGIN = 0x03000000, LENGTH = 32K
    rom (rx)    : ORIGIN = 0x08000000, LENGTH = 32M
}

SECTIONS
{
    .text : {
        KEEP(*(.gba_header))
        *(.text .text*)
        *(.rodata .rodata*)
        . = ALIGN(4);
    } > rom

    .data : {
        *(.data .data*)
        . = ALIGN(4);
    } > iwram AT > rom

    __iwram_data_start = LOADADDR(.data);
    __iwram_data_end   = LOADADDR(.data) + SIZEOF(.data);
    __iwram_data_dest  = ADDR(.data);

    .bss : {
        *(.bss .bss*)
        . = ALIGN(4);
    } > iwram

    __bss_start = ADDR(.bss);
    __bss_end   = ADDR(.bss) + SIZEOF(.bss);
}
