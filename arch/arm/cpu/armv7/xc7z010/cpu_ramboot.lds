/*
 * Link Script for XC7Z010
 *
 * File Name:   cpu_ramboot.lds
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.09.12
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)

/*!< ------------------------ core 0: 256MB ------------------------------- */

/*!< Program base address */
/*!< 0x00100000 ~ 0x040fffff (64MB) is used as memory space and dtb */
_PROGRAM_START = DEFINED(CONFIG_PROGRAM_BASE) ? CONFIG_PROGRAM_BASE : 0x04100000;
_PROGRAM_SIZE  = DEFINED(CONFIG_PROGRAM_SIZE) ? CONFIG_PROGRAM_SIZE : 0x0FF00000;

/*!< Heap and Stack size (12.514MB) */
_HEAP_SIZE = DEFINED(CONFIG_HEAP_SIZE) ? CONFIG_HEAP_SIZE : (8 * 1024 * 1024);
_SVC_STACK_SIZE = DEFINED(CONFIG_SVC_STACK_SIZE) ? CONFIG_SVC_STACK_SIZE : (2 * 1024 * 1024);
_SYS_STACK_SIZE = DEFINED(CONFIG_SYS_STACK_SIZE) ? CONFIG_SYS_STACK_SIZE : (2 * 1024 * 1024);
_IRQ_STACK_SIZE = DEFINED(CONFIG_IRQ_STACK_SIZE) ? CONFIG_IRQ_STACK_SIZE : (256 * 1024);
_FIQ_STACK_SIZE = DEFINED(CONFIG_FIQ_STACK_SIZE) ? CONFIG_FIQ_STACK_SIZE : (256 * 1024);
_ABT_STACK_SIZE = DEFINED(CONFIG_ABT_STACK_SIZE) ? CONFIG_ABT_STACK_SIZE : (1024);
_UND_STACK_SIZE = DEFINED(CONFIG_UND_STACK_SIZE) ? CONFIG_UND_STACK_SIZE : (1024);

/*!< memory pool size (32MB) */
_MEM_POOL_SIZE  = DEFINED(CONFIG_MEM_POOL_SIZE) ? CONFIG_MEM_POOL_SIZE : (32 * 1024 * 1024);

/*!< 
 * framebuffer ram (support double buffer with 1080p) 
 * 1080p = 1920 * 1080 * 4 ≈ 8MB, double buffer is 16MB
 * 2K = 2560 * 1440 * 4 = 14MB
 * 4K = 3840 * 2160 * 4 = 31.65MB
 * therefore, support at less 2K
 */
_FB_DRAM_SIZE   = DEFINED(CONFIG_FB_DRAM_SIZE) ? CONFIG_FB_DRAM_SIZE : (16 * 1024 * 1024);

MEMORY
{
    ram_ddr_0 : ORIGIN = 0x04100000, LENGTH = 0x0FF00000
}

ENTRY(_start)
SECTIONS
{
    . = _PROGRAM_START;
    . = ALIGN(16);

    __ram_ddr_start = .;

    .text :
    {
        __vector_table = .;

        KEEP(*(.vectors))
        KEEP(*(.start))
        KEEP(*(.head))
        *(.text)
        *(.text.*)
        *(.gnu.linkonce.t.*)
        *(.plt)
        *(.gnu_warning)
        *(.gcc_execpt_table)
        *(.glue_7)
        *(.glue_7t)
        *(.vfp11_veneer)
        *(.ARM.extab)
        *(.gnu.linkonce.armextab.*)
        *(.note.gnu.build-id)
    } > ram_ddr_0

    .dync_init : 
    {
        . = ALIGN(16);
        __dync_init_start = .;
        *(.dync_init)
        __dync_init_0_start = .;
        KEEP(*(.dync_init.0*))
        __dync_init_1_start = .;
        KEEP(*(.dync_init.1*))
        __dync_init_2_start = .;
        KEEP(*(.dync_init.2*))
        __dync_init_3_start = .;
        KEEP(*(.dync_init.3*))
        __dync_init_4_start = .;
        KEEP(*(.dync_init.4*))
        __dync_init_5_start = .;
        KEEP(*(.dync_init.5*))
        __dync_init_6_start = .;
        KEEP(*(.dync_init.6*))
        __dync_init_7_start = .;
        KEEP(*(.dync_init.7*))
        __dync_init_8_start = .;
        KEEP(*(.dync_init.8*))
        __dync_init_9_start = .;
        KEEP(*(.dync_init.9*))
        __dync_init_9_end = .;
        __dync_init_end = .;
    } > ram_ddr_0

    .dync_exit : 
    {
        . = ALIGN(16);
        __dync_exit_start = .;
        *(.dync_exit)
        __dync_exit_0_start = .;
        KEEP(*(.dync_exit.0*))
        __dync_exit_1_start = .;
        KEEP(*(.dync_exit.1*))
        __dync_exit_2_start = .;
        KEEP(*(.dync_exit.2*))
        __dync_exit_3_start = .;
        KEEP(*(.dync_exit.3*))
        __dync_exit_4_start = .;
        KEEP(*(.dync_exit.4*))
        __dync_exit_5_start = .;
        KEEP(*(.dync_exit.5*))
        __dync_exit_6_start = .;
        KEEP(*(.dync_exit.6*))
        __dync_exit_7_start = .;
        KEEP(*(.dync_exit.7*))
        __dync_exit_8_start = .;
        KEEP(*(.dync_exit.8*))
        __dync_exit_9_start = .;
        KEEP(*(.dync_exit.9*))
        __dync_exit_9_end = .;
        __dync_exit_end = .;
    } > ram_ddr_0

    . = ALIGN(16);
    .rodata :
    {
        __rodata_start = .;
        *(.rodata)
        *(.rodata.*)
        __rodata_end = .;
    } > ram_ddr_0

    . = ALIGN(16);
    .data :
    {
        __data_start = .;
        *(.data)
        *(.data.*)
        __data_end = .;
    } > ram_ddr_0

    .ARM.exidx : 
    {
        __exidx_start = .;
        *(.ARM.exidx*)
        *(.gnu.linkonce.armexidix.*.*)
        __exidx_end = .;
    } > ram_ddr_0

    . = ALIGN(16);
    .bss (NOLOAD) :
    {
        __bss_start = .;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        __bss_end = .;
    } > ram_ddr_0
    
    /*!< heap */
    .heap (NOLOAD) :
    {
        . = ALIGN(16);
        __heap_start = .;
        . += _HEAP_SIZE;
        __heap_end = .;
    } > ram_ddr_0

    /*!< stack */
    .stack (NOLOAD) :
    {
        . = ALIGN(16);
        __stack_start = .;
        __svc_stack_start = .;
        . += _SVC_STACK_SIZE;
        . = ALIGN(16);
        __svc_stack_end = .;
        __sys_stack_start = .;
        . += _SYS_STACK_SIZE;
        . = ALIGN(16);
        __sys_stack_end = .;
        __irq_stack_start = .;
        . += _IRQ_STACK_SIZE;
        . = ALIGN(16);
        __irq_stack_end = .;
        __fiq_stack_start = .;
        . += _FIQ_STACK_SIZE;
        . = ALIGN(16);
        __fiq_stack_end = .;
        __abt_stack_start = .;
        . += _ABT_STACK_SIZE;
        . = ALIGN(16);
        __abt_stack_end = .;
        __und_stack_start = .;
        . += _UND_STACK_SIZE;
        . = ALIGN(16);
        __und_stack_end = .;
        __stack_end = .;
    } > ram_ddr_0

    /*!< memory pool */
    .mem_pool (NOLOAD) :
    {
        . = ALIGN(16);
        __mem_pool_start = .;
        . += _MEM_POOL_SIZE;
        __mem_pool_end = .;
    } > ram_ddr_0

    /*!< DRAM */
    .fb_dram (NOLOAD) :
    {
        . = ALIGN(16);
        __fb_dram_start = .;
        . += _FB_DRAM_SIZE;
        __fb_dram_end = .;
    } > ram_ddr_0

    __ram_ddr_end = .;

    .ARM.attributes 0 : 
    { 
        __ARM_attributes_start = .;
        *(.ARM.attributes)
        __ARM_attributes_end = .;
    }
}
