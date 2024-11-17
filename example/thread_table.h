/*
 * User Thread Instance Tables
 *
 * File Name:   thread_table.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.04.02
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __THREAD_TABLE_H_
#define __THREAD_TABLE_H_

/*!< The globals */
#include <common/basic_types.h>
#include <common/error_types.h>
#include <common/generic.h>
#include <common/list_types.h>
#include <kernel/instance.h>

/*!< The defines */

/*!< The globals */

/*!< The functions */
#ifdef CONFIG_LIGHT_APP
TARGET_EXT kint32_t light_app_init(void);
#else
static inline kint32_t light_app_init(void) { return 0; }
#endif

#ifdef CONFIG_BUTTON_APP
TARGET_EXT kint32_t button_app_init(void);
#else
static inline kint32_t button_app_init(void) { return 0; }
#endif

#ifdef CONFIG_DISPLAY_APP
TARGET_EXT kint32_t display_app_init(void);
#else
static inline kint32_t display_app_init(void) { return 0; }
#endif

#ifdef CONFIG_TSC_APP
TARGET_EXT kint32_t tsc_app_init(void);
#else
static inline kint32_t tsc_app_init(void) { return 0; }
#endif

#ifdef CONFIG_ENV_MONITOR_APP
TARGET_EXT kint32_t env_monitor_init(void);
#else
static inline kint32_t env_monitor_init(void) { return 0; }
#endif

#ifdef CONFIG_CONSOLE_APP
TARGET_EXT kint32_t console_app_init(void);
#else
static inline kint32_t console_app_init(void) { return 0; }
#endif

#endif /* __THREAD_TABLE_H_ */
