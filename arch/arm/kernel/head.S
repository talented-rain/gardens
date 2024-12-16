/*
 * ARM V7 Kernel Boot Assembly File
 *
 * File Name:   head.S
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
    .section ".head", "ax"

    .global kernel_entry

/*!<
 * it can set cpu mode by changing the lower 5 bits of the cpsr register:
 *  USE: 0x10;  FIQ: 0x11;  IRQ: 0x12;  SVC: 0x13   MON: 0x16
 *  ABT: 0x17;  HYP: 0x1a;  UND: 0x1b;  SYS: 0x1f
 */

kernel_entry:
    mov r4, r0                                      @ save variable
    mov r5, r1
    mov r6, r2

    cpsid i                                         @ close IRQ
    cpsid f                                         @ close FIQ

    b _kernel_reset

/*!< ----------------------------------------------------------- */
/*!< Defines variable for stack; It needs combine board_init_r.c to analysis */
/*!< ----------------------------------------------------------- */
.global _UND_MODE_STACK_BASE
_UND_MODE_STACK_BASE:
    .word 0x0123457c

.global _ABT_MODE_STACK_BASE
_ABT_MODE_STACK_BASE:
    .word 0x01234578

.global _FIQ_MODE_STACK_BASE
_FIQ_MODE_STACK_BASE:
    .word 0x01234574

.global _IRQ_MODE_STACK_BASE
_IRQ_MODE_STACK_BASE:
    .word 0x01234570

.global _SYS_MODE_STACK_BASE
_SYS_MODE_STACK_BASE:
    .word 0x0123456c

.global _SVC_MODE_STACK_BASE
_SVC_MODE_STACK_BASE:
    .word 0x01234568

/*!< ----------------------------------------------------------- */
ENTRY(_kernel_reset)
_kernel_reset:
    cps #ARCH_IRQ_MODE                              @ set the cpu to IRQ mode
    ldr sp, _IRQ_MODE_STACK_BASE                    @ set IRQ mode stack
    mrs r0, cpsr                                    @ read cpsr register to r0
    bic r0, r0, #CPSR_BIT_E                         @ set to littile endian
    msr spsr_fsxc, r0

    /*!< Jtag can not run with another mode */
@#if !defined(CONFIG_DEBUG_JTAG)

    cps #ARCH_FIQ_MODE                              @ set the cpu to FIQ mode
    ldr sp, _FIQ_MODE_STACK_BASE                    @ set FIQ mode stack
    mrs r0, cpsr                                    @ read cpsr register to r0
    bic r0, r0, #CPSR_BIT_E                         @ set to littile endian
    msr spsr_fsxc, r0

    cps #ARCH_UND_MODE                              @ set the cpu to UND mode
    ldr sp, _UND_MODE_STACK_BASE                    @ set UND mode stack
    mrs r0, cpsr                                    @ read cpsr register to r0
    bic r0, r0, #CPSR_BIT_E                         @ set to littile endian
    msr spsr_fsxc, r0

    cps #ARCH_SYS_MODE                              @ set the cpu to SYS mode
    ldr sp, _SYS_MODE_STACK_BASE                    @ set SYS mode stack
    mrs r0, cpsr                                    @ read cpsr register to r0
    bic r0, r0, #CPSR_BIT_E                         @ set to littile endian
    msr spsr_fsxc, r0

@#endif

    cps #ARCH_ABT_MODE                              @ set the cpu to ABT mode
    ldr sp, _ABT_MODE_STACK_BASE                    @ set ABT mode stack
    mrs r0, cpsr                                    @ read cpsr register to r0
    bic r0, r0, #CPSR_BIT_E                         @ set to littile endian
    msr spsr_fsxc, r0

    /*!< switch to SVC mode */
    /*!< kernel will run in SVC mode to initial all resources */
    cps #ARCH_SVC_MODE                              @ set the cpu to SVC mode
    ldr sp, _SVC_MODE_STACK_BASE                    @ set SVC mode stack
    mrs r0, cpsr                                    @ read cpsr register to r0
    bic r0, r0, #CPSR_BIT_E                         @ set to littile endian
    bic r0, r0, #CPSR_BIT_A                         @ enable abort
@   orr r0, r0, #CPSR_BIT_A                         @ disable abort
    msr cpsr, r0

    READ_CP15_SCTLR(r0)
    orr r0, r0, #CP15_SCTLR_BIT_C                   @ enable D-Cache
    orr r0, r0, #CP15_SCTLR_BIT_I                   @ enable I-Cache
    orr r0, r0, #CP15_SCTLR_BIT_Z                   @ enable branch prediction
    bic r0, r0, #CP15_SCTLR_BIT_A                   @ disable alignment check
    WRITE_CP15_SCTLR(r0)

    b _kernel_start                                 @ jump to head-common.S (configure mmu)

ENDPROC(_kernel_reset)

/* end of file */
