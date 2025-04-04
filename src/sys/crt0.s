/* (c) copyright 2025 Lawrence D. Kern /////////////////////////////////////// */

    .section .gba_header
    .arm

header:
    b start_vector
    .fill 188, 1, 0  @ Logo data is inserted here
    .byte 0x30, 0x31
    .byte 0x96, 0x00 @ Checksum is inserted here
    .byte 0
    .fill 12, 1, 0

    .align
    .text
    .arm

start_vector:
    b initialize_system

initialize_system:
    @ Set up IRQ stack
    mov r0, #0x12
    msr cpsr, r0
    ldr sp, =0x03007FA0

    @ Set up system stack
    mov r0, #0x1F
    msr cpsr, r0
    ldr sp, =0x03007F00

    @ Set up IWRAM
    ldr r0, =__iwram_data_start
    ldr r1, =__iwram_data_dest
    ldr r2, =__iwram_data_end
    cmp r0, r2
    beq .skip_data_copy

    .data_copy_loop:
    ldmia r0!, {r3}
    stmia r1!, {r3}
    cmp r0, r2
    blt .data_copy_loop

    .skip_data_copy:

    @ Set up BSS
    ldr r0, =__bss_start
    ldr r1, =__bss_end
    mov r2, #0

    .bss_clear_loop:
    cmp r0, r1
    strlt r2, [r0], #4
    blt .bss_clear_loop

    @ Program entry point
    ldr r0, =main
    bx r0

    .loop:
    b .loop
