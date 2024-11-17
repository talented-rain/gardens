/*
 * Generic Configuration
 *
 * File Name:   configs.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.12.16
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __CONFIGS_H
#define __CONFIGS_H

/*!< The includes */
#include "mach_configs.h"

#define CONFIG_PRINT_LEVEL                      "7"

/*!< all scheduler will be quited, the kernel will degrade to standlone */
#define CONFIG_SCHDULE                          (1)
/*!< 
 * 0: the pending must be requested by self-thread or others (scheduling to another hightest priority thread);
 * 1: the pending will be requested by timer-out ISR (check priority and time-slice, e.g, thread will be preempted suddenly)
 */
#define CONFIG_PREEMPT                          (1)
/*!< if CONFIG_PREEMPT is diabled, thread can be chosen ===> 0: polling by priority; 1: polling in order */
#define CONFIG_ROLL_POLL                        (1)

#if CONFIG_PREEMPT
/*!< disable priority,  */
#undef  CONFIG_ROLL_POLL
#define CONFIG_ROLL_POLL                        (0)
#endif

/*!< armv7 */
#if defined(CONFIG_ARCH_ARMV7)
    #include <asm/armv7/arch_common.h>
#endif

#endif /* __CONFIGS_H */
