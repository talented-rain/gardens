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
#include <kernel/asm_text.h>

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

.macro _context_save_params
    add r2, r1, #ARCH_OFFSET_R3                     @ r1: specified sp
    stmia r2, { r3 - r12 }                          @ push r3 ~ r12 to sp (basic address)

    mov r9, lr
    add r2, r1, #ARCH_OFFSET_LR
    stmia r2, { r9, lr }                            @ save lr
.endm

.macro _context_restore_params
    add r2, r1, #ARCH_OFFSET_R3                     @ pointer to r3
    ldmia r2, { r3 - r12 }                          @ pop r3 ~ r12 from sp
    ldr lr, [r1, #ARCH_OFFSET_LR]
.endm

/* -------------------------------------------------------------------------------
 * -------------------------------------------------------------------------------
 *                      Entry1: enter from "schedule_thread"
 * -------------------------------------------------------------------------------
 * -----------------------------------------------------------------------------*/
ENTRY(__switch_to)
__switch_to:
    _set_preempt_cnt add

    push { r1, r2 }
    str r0, g_asm_context_info

    ldr r2, [r0, #CONTEXT_FIRST_OFFSET]
    ldr r1, [r2]
    cmp r1, #0
    beq _context_save_first

1:
    ldr r2, [r0, #CONTEXT_PREV_SP]                  @ r1: prev sp (frame register base)
    ldr r1, [r2]

    push { r3 }
    add r2, r1, #ARCH_FRAME_SIZE
    ldr r3, [r2]
    bic r3, #SCHED_FROM_IRQ
    str r3, [r2]                                    @ save scheduling source
    pop { r3 }

    b _context_save

/*!< scheduled for first time */
_context_save_first:
    mov r1, #1
    str r1, [r2]                                    @ set first to true

    pop { r1, r2}

    sub sp, sp, #ARCH_FRAME_SIZE                    @ sp: current position
    stmia sp, { r0 - r12 }                          @ push r0 ~ r12 to sp (basic address)
    add r8, sp, #ARCH_OFFSET_PC                     @ r8 = offset of pc, it is also lr reg's top address
    stmdb r8, { sp, lr }                            @ save lr to stack first, and then save sp
    str lr, [r8]                                    @ manually save lr (pc - 4) to stack (offset pc)

    add r8, sp, #ARCH_OFFSET_PSR                    @ offset psr
    mrs r5, spsr                                    @ get spsr
    str r5, [r8]                                    @ manually save spsr to stack (offset psr)

    b _sched_first_ready

/* -------------------------------------------------------------------------------
 * save curent context
 * -----------------------------------------------------------------------------*/
_context_save:
    _context_save_params                            @ save r3 ~ r12, sp, lr
    mov r9, r1                                      @ r9: prev sp (frame register base)
    pop { r1, r2 }
    stmia r9, { r0 - r2 }

    add r8, r9, #ARCH_OFFSET_PSR                    @ r8 = offset of spsr
    mrs r5, spsr                                    @ get spsr
    str r5, [r8]                                    @ manually save spsr to stack (offset psr)

    add r8, r9, #ARCH_FRAME_SIZE                    @ r8 = &flags
    ldr r7, [r8]                                    @ r7 = flags
    orr r6, r7, #SCHED_NOT_FIRST                    @ r6 = flags | 0x02
    str r6, [r8]                                    @ initial flags (marking the previous thread will no longer be scheduled for the first time)  

    str sp, [r9, #ARCH_OFFSET_SP]                   @ save sp
    b _context_restore

/* -------------------------------------------------------------------------------
 * restore next context
 * -----------------------------------------------------------------------------*/
_context_restore:
    ldr r0, g_asm_context_info                      @ get context_info from "__schedule_thread"

    ldr r2, [r0, #CONTEXT_NEXT_SP]                  @ r2: next sp (frame register base)
    ldr r1, [r2]

    ldr sp, [r1, #ARCH_OFFSET_SP]
    cmp sp, #0
    beq _sched_first_ready                          @ first schedule    

    _context_restore_params
    push { lr }                                     @ save lr

    ldr r2, [r1, #ARCH_OFFSET_PSR]
    push { r2 }                                     @ save spsr

    add r2, r1, #ARCH_OFFSET_R2                     @ pointer to r2
    ldmdb r2, { r0, r1 }
    ldr r2, [r2]

    b _sched_init_before

/* -------------------------------------------------------------------------------
 * schedule preparation (first time scheduling)
 * -----------------------------------------------------------------------------*/
_sched_first_ready:
    ldr r1, [r0, #CONTEXT_NEXT_SP]                  @ r1: next sp (frame register base)
    ldr r8, [r1]
    mov sp, r8                                      @ first schedule, sp = frame register base    

    ldr r1, [r0, #CONTEXT_ENTRY_OFFSET]             @ address of entry
    ldr r8, [r1]                                    @ first schedule, lr = entry
    mrs r7, cpsr
    push { r7, r8 }                                 @ push { cpsr, lr } ===> pop { spsr }, pop { pc }

    ldr r1, [r0, #CONTEXT_ARGS_OFFSET]              @ args
    ldr r8, [r1]
    mov r0, r8                                      @ r0 = args

    b _sched_init_before

/* -------------------------------------------------------------------------------
 * save curent context
 * -----------------------------------------------------------------------------*/
_sched_init_before:
    push { r0 - r3, r12 }                           @ r1: next sp; r2: first; r7: address of [first]
    ldr r1, =__real_thread_init_before              @ init thread before running
    blx r1

1:
    ldr r0, g_asm_context_info                      @ get context_info from "__schedule_thread"
    ldr r1, [r0, #CONTEXT_NEXT_SP]                  @ r1: next sp (frame register base)
    ldr r2, [r1]

    add r3, r2, #ARCH_FRAME_SIZE                    @ r8 = &flags
    ldr r12, [r3]                                   @ r7 = flags
    and r12, #SCHED_FROM_IRQ
    cmp r12, #0
    beq 3f                                          @ judge if is from irq_handler

2:
    pop { r0 - r3, r12 }
    pop { lr }                                      @ spsr will be abandoned
    pop { lr }                                      @ lr_svc will be used by current thread

    b _switch_to_irq                                @ ===> return irq_handler

3:
    pop { r0 - r3, r12 }
    push { r12 }
    ldr r12, [sp, #4]
    msr spsr_cxsf, r12                              @ spsr will be used by current thread
    pop { r12 }
    add sp, #4

    b _switch_to_next                               @ ===> return thread

/* -------------------------------------------------------------------------------
 * schedule new thread
 * -----------------------------------------------------------------------------*/
_switch_to_next:
    _set_preempt_cnt sub                            @ preempt_enable
    pop { pc }

ENDPROC(__switch_to)


/* -------------------------------------------------------------------------------
 * -------------------------------------------------------------------------------
 *                      Entry2: enter from "irq_handler"
 * -------------------------------------------------------------------------------
 * -----------------------------------------------------------------------------*/
ENTRY(__schedule_before)
__schedule_before:
    push { r0 }
    ldr r0, g_asm_preempt_cnt
    ldr r0, [r0]                                    @ check is preempt locked
    cmp r0, #0
    pop { r0 }
    bne _schedule_over

    _set_preempt_cnt add                            @ preempt_disable

    push { r0 - r2 }
    push { lr }                                     @ important: save current thread's lr, which is not equaled to lr_irq !!!
    push { r3 - r12 }

    ldr r1, =__schedule_thread
    blx r1                                          @ schedule threads
    str r0, g_asm_context_info

    pop { r3 - r12 }
    pop { lr }                                      @ get lr_svc

    add r2, sp, #12                                 @ pointer to lr_irq
    ldr r1, [r2, #4]                                @ r1 = spsr_irq (lr_irq can be kept in sp all the time, does not need to be taken out)                    
    cmp r1, #0
    orreq r1, #ARCH_SVC_MODE
    msr spsr_cxsf, r1

1:
    ldr r2, [r0, #CONTEXT_PREV_SP]                  @ r1: prev sp (frame register base)
    ldr r1, [r2]

    push { r3 }
    add r2, r1, #ARCH_FRAME_SIZE
    ldr r3, [r2]
    orr r3, #SCHED_FROM_IRQ
    str r3, [r2]                                    @ save scheduling source
    pop { r3 }

    pop { r0 }
    b _context_save

/* -------------------------------------------------------------------------------
 * schedule new thread (by IRQ)
 * -----------------------------------------------------------------------------*/
_switch_to_irq:
    _set_preempt_cnt sub                            @ preempt_enable

/* -------------------------------------------------------------------------------
 * schedule start
 * -----------------------------------------------------------------------------*/
_schedule_over:
    rfeia sp!                                       @ if scheduling source is irq, return to kill interrupt

ENDPROC(__schedule_before)

/* end of file */
