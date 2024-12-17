/*
 * Context for Thread Defines
 *
 * File Name:   asm_text.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.12.16
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __ASM_CONTEXT_H_
#define __ASM_CONTEXT_H_

/*!< The defines */
#define CONTEXT_ENTRY_OFFSET                0
#define CONTEXT_ARGS_OFFSET                 4
#define CONTEXT_FIRST_OFFSET                8
#define CONTEXT_NEXT_SP                     12
#define CONTEXT_PREV_SP                     16

#define SCHED_FROM_IRQ						0x01
#define SCHED_NOT_FIRST						0x02

#endif /* __ASM_CONTEXT_H_ */
