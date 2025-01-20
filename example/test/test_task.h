/*
 * User Thread Instance Tables
 *
 * File Name:   test_task.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.11.21
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __TEST_TASK_H_
#define __TEST_TASK_H_

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The globals */
#include <common/basic_types.h>
#include <common/error_types.h>
#include <common/generic.h>
#include <common/list_types.h>

/*!< The globals */

/*!< The functions */
#ifdef CONFIG_LIGHT_APP
extern kint32_t light_task_init(void);
#else
static inline kint32_t light_task_init(void) { return 0; }
#endif

#ifdef CONFIG_BUTTON_APP
extern kint32_t button_task_init(void);
#else
static inline kint32_t button_task_init(void) { return 0; }
#endif

#ifdef CONFIG_DISPLAY_APP
extern kint32_t display_task_init(void);
#else
static inline kint32_t display_task_init(void) { return 0; }
#endif

#ifdef CONFIG_TSC_APP
extern kint32_t tsc_task_init(void);
#else
static inline kint32_t tsc_task_init(void) { return 0; }
#endif

#ifdef CONFIG_ENV_MONITOR_APP
extern kint32_t env_monitor_init(void);
#else
static inline kint32_t env_monitor_init(void) { return 0; }
#endif

#ifdef CONFIG_CONSOLE_APP
extern kint32_t console_task_init(void);
#else
static inline kint32_t console_task_init(void) { return 0; }
#endif

static inline kint32_t test_task_init(void) { return 0; }

#ifdef __cplusplus
    }
#endif

#endif /* __TEST_TASK_H_ */
