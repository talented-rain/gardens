/*
 * Kernel Time Defines
 *
 * File Name:   sleep.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.04.01
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __KERNEL_SLEEP_H_
#define __KERNEL_SLEEP_H_

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The includes */
#include <kernel/kernel.h>

/*!< The defines */

/*!< The functions */
extern void schedule_timeout(kutime_t count);

extern kuint32_t sleep(kuint32_t seconds);
extern kuint32_t msleep(kuint32_t milseconds);
extern kint32_t usleep(kuint32_t useconds);

#ifdef __cplusplus
    }
#endif

#endif /* __KERNEL_SLEEP_H_ */
