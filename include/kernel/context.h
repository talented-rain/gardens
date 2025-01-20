/*
 * Context for Thread Defines
 *
 * File Name:   context.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.08.04
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __KERNEL_CONTEXT_H_
#define __KERNEL_CONTEXT_H_

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The globals */
#include <kernel/kernel.h>

/*!< The defines */
struct scheduler_context_regs
{
	kuaddr_t r0;
	kuaddr_t r1;
	kuaddr_t r2;
	kuaddr_t r3;
	kuaddr_t r4;
	kuaddr_t r5;
	kuaddr_t r6;
	kuaddr_t r7;
	kuaddr_t r8;
	kuaddr_t r9;
	kuaddr_t r10;
	kuaddr_t r11;
	kuaddr_t r12;
	kuaddr_t sp;
	kuaddr_t lr;
	kuaddr_t pc;
	kuaddr_t psr;

	kuaddr_t flags;							/*!< save scheduling reason (1: scheduled by preempt; 0: scheduled by self) */
};

struct scheduler_context
{
	kuaddr_t entry;							/*!< if (*flags) is 0, entry will be run */
	kuaddr_t args;
	kuaddr_t first;							/*!< 0: first run; 1: other */

	kuaddr_t next_sp;
	kuaddr_t prev_sp;
};

/*!< The functions */
extern void __switch_to(kuaddr_t info);

/*!< API functions */
/*!
 * @brief	switch context
 * @param  	sprt_info: prev and next thread information (stack, entry, ...)
 * @retval 	none
 * @note   	jump to asm
 */
static inline void context_switch(struct scheduler_context *sprt_info)
{
	__switch_to((kuaddr_t)sprt_info);
}

#ifdef __cplusplus
    }
#endif

#endif /* __KERNEL_CONTEXT_H_ */
