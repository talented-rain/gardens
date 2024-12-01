/*
 * User Thread Instance Tables
 *
 * File Name:   test.app.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.11.21
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __TEST_APP_H_
#define __TEST_APP_H_

/*!< The globals */
#include <common/basic_types.h>
#include <common/error_types.h>
#include <common/generic.h>
#include <common/list_types.h>

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

static inline kint32_t test_app_init(void) { return 0; }

/*!< The defines */
#define mrt_test_app_list   \
            console_app_init,   \
            light_app_init, \
            button_app_init,    \
            /* display_app_init, */  \
            tsc_app_init,   \
            env_monitor_init,   \
            \
            test_app_init

#endif /* __TEST_APP_H_ */
