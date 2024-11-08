/*
 * ARM V7 Boot Assembly File
 *
 * File Name:   start.S
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.09.09
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#include <common/linkage.h>
#include <configs/mach_configs.h>

    .text
    .arm
    .section ".start", "ax"

    .global _reset

/*!<
 * it can set cpu mode by changing the lower 5 bits of the cpsr register:
 *  USE: 0x10;  FIQ: 0x11;  IRQ: 0x12;  SVC: 0x13   MON: 0x16
 *  ABT: 0x17;  HYP: 0x1a;  UND: 0x1b;  SYS: 0x1f
 */

_reset:
    /*!< Disable interrupts (fiq & irq), and set the cpu to SVC mode */
    mrs r0, cpsr                                    @ read cpsr register to r0
    bic r0, r0, #0x1f                               @ clear mode bits
    orr r0, r0, #ARCH_SVC_MODE                      @ set the cpu to SVC mode
    orr r0, r0, #CPSR_BIT_F                         @ disable fiq
    orr r0, r0, #CPSR_BIT_I                         @ disable irq
    msr cpsr, r0                                    @ change cpsr

    READ_CP15_SCTLR(r0)                             @ read cp15-c1(SCTLR)
    bic r0, #CP15_SCTLR_BIT_V                       @ set VBAR relocate ---> V = 0
    WRITE_CP15_SCTLR(r0)                            @ change cp15-c1

    ldr r0, =_start                                 @ verctor table base address
    WRITE_CP15_VBAR(r0)                             @ set VBAR

    bl cpu_init_cp15                                @ initial cp15
    bl cpu_init_crit                                @ initial low level configure, like clk, sdram, ...

	mov	r0, r0
	mrc	p15, 0, r1, c1, c0, 2                       @ read cp access control register (CACR) into r1
	orr	r1, r1, #(0xf << 20)                        @ enable full access for p10 & p11
	mcr	p15, 0, r1, c1, c0, 2                       @ write back into CACR

    /*!< enable vfp */
	fmrx r1, fpexc                                  @ read the exception register
	orr r1, r1, #0x40000000                         @ set VFP enable bit, leave the others in orig state
	fmxr fpexc, r1                                  @ write back the exception register

    b _main                                         @ jump to crt0.S

/*!< ----------------------------------------------------------- */
/*!< cp15 initial */
ENTRY(cpu_init_cp15)
cpu_init_cp15:
    mov r12, lr

    mov r0, #0x00
    WRITE_CP15_TLBS(r0)                             @ invalidate TLBs
    WRITE_CP15_ICACHE(r0)                           @ invalidate I-Cache
    WRITE_CP15_BP(r0)                               @ invalidate BP array
    WRITE_CP15_DSB(r0)                              @ invalidate DSB
    WRITE_CP15_ISB(r0)                              @ invalidate ISB

    READ_CP15_SCTLR(r0)
    bic r0, r0, #CP15_SCTLR_BIT_M                   @ disable mmu
    bic r0, r0, #CP15_SCTLR_BIT_C                   @ disable D-Cache consistency
    bic r0, r0, #CP15_SCTLR_BIT_I                   @ disable I-Cache
    orr r0, r0, #CP15_SCTLR_BIT_A                   @ enable alignment check
    orr r0, r0, #CP15_SCTLR_BIT_Z                   @ enable branch prediction
    WRITE_CP15_SCTLR(r0)

    mov pc, r12

ENDPROC(cpu_init_cp15)

/*!< ----------------------------------------------------------- */
/*!< lowlevel init */
ENTRY(cpu_init_crit)
cpu_init_crit:
    b lowlevel_init                                 @ jump to lowlevel_init.S

ENDPROC(cpu_init_crit)

/* end of file */
