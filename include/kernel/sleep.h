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

/*!< The includes */
#include <kernel/kernel.h>

/*!< The defines */

/*!< The functions */
TARGET_EXT void schedule_timeout(kutime_t count);
TARGET_EXT void schedule_delay(kuint32_t seconds);
TARGET_EXT void schedule_delay_ms(kuint32_t milseconds);
TARGET_EXT void schedule_delay_us(kuint32_t useconds);

#endif /* __KERNEL_SLEEP_H_ */
