/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/* ------------------------ System architecture includes ----------------------------- */
#include "arch/sys_arch.h"

/* ------------------------ lwIP includes --------------------------------- */
#include "lwip/opt.h"

#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/sys.h"
#include "lwip/mem.h"
#include "lwip/stats.h"
#include "lwip/init.h"

#include <common/time.h>
#include <kernel/spinlock.h>

/* The globals */
static u32_t _rand_value;
static volatile uint32_t time_now = 0;
static struct timer_list sgrt_lwip_tick_timer;
static DECLARE_SPIN_LOCK(sgrt_lwip_lock);

/************************************************************************
* Generates a pseudo-random number.
* NOTE: Contrubuted by the FNET project.
*************************************************************************/
u32_t lwip_rand(void)
{
    _rand_value = _rand_value * 1103515245u + 12345u;
	return ((u32_t)(_rand_value >> 16u) % (32767u + 1u));
}

void lwip_tick_timeout(kuint32_t args)
{
    struct timer_list *sprt_tim = (struct timer_list *)args;

    time_now++;
    mod_timer(sprt_tim, jiffies + msecs_to_jiffies(1));
}

void lwip_tick_timer_init(void)
{
    /* Set SysTick period to 1 ms and enable its interrupts */
    struct timer_list *sprt_tim = &sgrt_lwip_tick_timer;

    setup_timer(sprt_tim, lwip_tick_timeout, (kuint32_t)sprt_tim);
    sprt_tim->expires = jiffies + msecs_to_jiffies(1);
    add_timer(sprt_tim);
}

/*
This optional function returns the current time in milliseconds (don't care
  for wraparound, this is only used for time diffs).
  Not implementing this function means you cannot use some modules (e.g. TCP
  timestamps, internal timeouts for NO_SYS==1).
  */

u32_t sys_now(void)
{
    return (u32_t)time_now;
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_arch_protect
 *---------------------------------------------------------------------------*
 * Description:
 *      This optional function does a "fast" critical region protection and
 *      returns the previous protection level. This function is only called
 *      during very short critical regions. An embedded system which supports
 *      ISR-based drivers might want to implement this function by disabling
 *      interrupts. Task-based systems might want to implement this by using
 *      a mutex or disabling tasking. This function should support recursive
 *      calls from the same task or interrupt. In other words,
 *      sys_arch_protect() could be called while already protected. In
 *      that case the return value indicates that it is already protected.
 *
 *      sys_arch_protect() is only required if your port is supporting an
 *      operating system.
 * Outputs:
 *      sys_prot_t              -- Previous protection level (not used here)
 *---------------------------------------------------------------------------*/
sys_prot_t sys_arch_protect( void )
{
    spin_lock_irqsave(&sgrt_lwip_lock);
    return sgrt_lwip_lock.flag;
}

/*---------------------------------------------------------------------------*
 * Routine:  sys_arch_unprotect
 *---------------------------------------------------------------------------*
 * Description:
 *      This optional function does a "fast" set of critical region
 *      protection to the value specified by pval. See the documentation for
 *      sys_arch_protect() for more information. This function is only
 *      required if your port is supporting an operating system.
 * Inputs:
 *      sys_prot_t              -- Previous protection level (not used here)
 *---------------------------------------------------------------------------*/
void sys_arch_unprotect( sys_prot_t xValue )
{
    spin_unlock_irqrestore(&sgrt_lwip_lock);
}

/*-------------------------------------------------------------------------*
 * End of File:  sys_arch.c
 *-------------------------------------------------------------------------*/

