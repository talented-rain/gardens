/*
 * Assembly File About Thread Scheduler
 *
 * File Name:   context.S
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.03.31
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

#include <common/linkage.h>
#include <configs/mach_configs.h>

    .text
    .arm

/* -------------------------------------------------------------------------------
 * Global Variables Defines 
 * -----------------------------------------------------------------------------*/
g_asm_preempt_cnt: .word g_sched_preempt_cnt
g_asm_context_info: .word 0x00000000

.macro _set_preempt_cnt op
    push { r1, r2 }
    ldr r2, g_asm_preempt_cnt
    ldr r1, [r2]
    \op r1, #1
    str r1, [r2]
    pop { r1, r2 }
.endm
    
/* -------------------------------------------------------------------------------
 * Save The Current Filed Context 
 * -----------------------------------------------------------------------------*/
.macro _generic_save_params
    sub sp, sp, #ARCH_FRAME_SIZE                    @ sp: current position
    stmia sp, { r0 - r12 }                          @ push r0 ~ r12 to sp (basic address)
    add r8, sp, #ARCH_OFFSET_PC                     @ r8 = offset of pc, it is also lr reg's top address
    stmdb r8, { sp, lr }                            @ save lr to stack first, and then save sp
.endm

.macro _context_save_params
    add r2, r1, #ARCH_OFFSET_R3                     @ r1: specified sp
    stmia r2, { r3 - r12 }                          @ push r3 ~ r12 to sp (basic address)
    mov r9, r1                                      @ r9: prev sp (frame register base)
    pop { r1, r2 }
    stmia r9, { r0 - r2 }

    add r8, r9, #ARCH_OFFSET_PC                     @ r8 = offset of pc, it is also lr reg's top address
    stmdb r8, { sp, lr }                            @ save lr to stack first, and then save sp
.endm

/* -------------------------------------------------------------------------------
 * Restore The Current Filed Context 
 * -----------------------------------------------------------------------------*/
.macro _generic_restore_params
    add r8, sp, #ARCH_OFFSET_LR                     @ r8 = offset of pc, it is also lr reg's top address
    ldr lr, [r8]
    ldmia sp, { r0 - r12 }
    add sp, sp, #ARCH_FRAME_SIZE
.endm

.macro _context_restore_params
    add r0, r1, #ARCH_OFFSET_R1                     @ pointer to r1
    ldmia r0, { r1 - r12 }                          @ pop r1 ~ r12
    sub r0, #ARCH_OFFSET_R1
    ldr lr, [r0, #ARCH_OFFSET_LR]
    ldr r0, [r0]
.endm

/* -------------------------------------------------------------------------------
 * Entry
 * -----------------------------------------------------------------------------*/
ENTRY(__switch_to)
__switch_to:
    _set_preempt_cnt add                            @ preempt_disable

    push { r1, r2 }
    ldr r2, [r0, #8]                                @ r1: first
    ldr r1, [r2]

    cmp r1, #0                                      @ if r1 == 0, current thread is scheduled by "start_kernel"
    bne 2f

1:
    pop { r1, r2 }
    _generic_save_params

    str lr, [r8]                                    @ manually save lr(pc - 4) to stack (offset pc)
    mrs r5, spsr                                    @ get spsr
    str r5, [r7]                                    @ manually save spsr to stack (offset psr)
    b _switch_restore

2:
    ldr r2, [r0, #16]                               @ r1: prev sp (frame register base)
    ldr r1, [r2]
    
    _context_save_params
    str lr, [r8]                                    @ manually save lr(pc - 4) to stack (offset pc)

    add r7, r9, #ARCH_OFFSET_PSR                    @ r7 = offset of spsr
    mrs r5, spsr                                    @ get spsr
    str r5, [r7]                                    @ manually save spsr to stack (offset psr)

    add r4, r9, #ARCH_FRAME_SIZE
    mov r3, #0
    str r3, [r4]

_switch_restore:
    ldr r3, [r0, #12]                               @ r1: next sp (frame register base)
    ldr r1, [r3]
    ldr sp, [r1, #ARCH_OFFSET_SP]
    cmp sp, #0
    moveq sp, r1                                    @ first schedule, sp = frame register base    

    ldr r2, [r0]                                    @ address of entry
    mov r7, #0                                      @ initial

    ldr r3, [r1, #ARCH_OFFSET_PC]                   @ r3: reload lr, lr = entry
    cmp r3, #0
    itt eq
    ldr r3, [r2]                                    @ first schedule, lr = entry
    mov r7, #1                                      @ mask first
    push { r3, r7 }                                 @ push for pc, and mask first

    ldr r7, [r0, #8]                                @ r2: first
    ldr r2, [r7]                                    @ if r2 == 0, current thread is scheduled by "start_kernel"

1:
    push { r0 - r3, r7 }                            @ r1: next sp; r2: first; r7: address of [first]
    ldr r1, =__real_thread_init_before              @ init thread before running
    blx r1

    pop { r0 - r3, r7 }
    cmp r2, #0                                      @ check if is scheduled for the first time
    beq 4f

2:
    add r4, r1, #ARCH_OFFSET_PC                     @ if (g_asm_context_info != 0), it will return to irq ---> svc, push lr and spsr for "rfeia sp!"
    ldmia r4, { r5, r6 }                            @ r5 = pc, r6 = spsr
    cmp r6, #0
    orreq r6, #ARCH_SVC_MODE
    msr spsr_cxsf, r6                               @ restore spsr

    mov r4, #0
    str r4, g_asm_context_info                      @ clear
    
    add r4, r1, #ARCH_FRAME_SIZE
    ldr r3, [r4]
    cmp r3, #0
    beq 3f                                          @ case 1: scheduling is from thread calling, return to thread

    pop { r3, r7 }                                  @ lr_svc (r3) will be abandoned
    push { r5, r6 }                                 @ r5 = lr, r6 = spsr

    cmp r7, #1                                      @ first scheduled
    ittt eq
    ldr r4, [r0, #4]
    ldr r0, [r4]                                    @ r0: args
    b _schedule_first_irq

    _context_restore_params                         @ restore context

_schedule_first_irq:
    _set_preempt_cnt sub
    b _schedule_over                                @ case 2: scheduling is from IRQ, need to return to IRQ;

3:
    ldmia sp, { r2 }
    cmp r2, #1                                      @ first scheduled
    beq 4f

    pop { r2 }
    _context_restore_params                         @ restore context
    push { r0 }
    mrs r0, spsr
    msr cpsr, r0                                    @ restore cpsr
    pop { r0 }
    b 5f

4:
    pop { r2 }
    mov r2, #1
    str r2, [r7]                                    @ set first to true

    ldr r7, [r0, #4]                                @ r0: args
    ldr r0, [r7]
    
5:
    _set_preempt_cnt sub                            @ preempt_enable
    pop { pc }

ENDPROC(__switch_to)

ENTRY(__schedule_before)
__schedule_before:
    push { r0 }
    ldr r0, g_asm_preempt_cnt
    ldr r0, [r0]                                    @ check is preempt locked
    cmp r0, #0
    pop { r0 }
    bne _schedule_over

    _set_preempt_cnt add                            @ preempt_disable

    _generic_save_params
    ldr r0, =__schedule_thread
    blx r0                                          @ schedule threads
    str r0, g_asm_context_info
    _generic_restore_params

1:
    push { r0 }
    push { r1, r2 }
    ldr r0, g_asm_context_info                      @ get context_info from "__schedule_thread"
    ldr r2, [r0, #16]                               @ r1: prev sp (frame register base)
    ldr r1, [r2]

    _context_save_params
    pop { r4 }                                      @ r4 = r0
    str r4, [r9]                                    @ r9: prev sp (frame register base); resave r4 (r0), r5 (r1), r6 (r2)

    pop { r5, r6 }                                  @ r5 = lr_irq, r6 = spsr_irq
    stmia r8, { r5, r6 }
    add r4, r9, #ARCH_OFFSET_SP
    str sp, [r4]                                    @ save sp_svc

    add r4, r9, #ARCH_FRAME_SIZE
    mov r3, #1
    str r3, [r4]                                    @ save scheduling source

    b _switch_restore

_schedule_over:
    rfeia sp!                                       @ if scheduling source is irq, return to kill interrupt

ENDPROC(__schedule_before)

/* end of file */
