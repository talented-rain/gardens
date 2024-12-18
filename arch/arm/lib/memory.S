/*
 * Assembly File For Memory Operation
 *
 * File Name:   memory.S
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.11.12
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

#include <common/linkage.h>
#include <configs/mach_configs.h>

    .text
    .arm

/*!
 * @param:  r0 (&start_addr), r12 (start_addr)
 * @param:  r1 (end_addr)
 * @param:  r2 (data)
 */
ENTRY(__memset_ex)
__memset_ex:
    push {r11, r12, lr}
    ldr r12, [r0]
    nop
    sub r11, r1, #16

1:
    cmp r12, r1
    bhs 2f      
    str r2, [r12]
    add r12, #4
    cmp r12, r11
    bhs 3f
    b 1b

3:
    b 1b

2:
    nop
    str r12, [r0]
    pop {r11, r12, lr}
    mov pc, lr

ENDPROC(__memset_ex)

/*  end of file */
