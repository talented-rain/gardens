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

#include "test/test_app.h"

/*!< The defines */

/*!< The globals */

/*!< The functions */
TARGET_EXT kint32_t lvgl_task_init(void);
TARGET_EXT kint32_t lwip_task_init(void);

#endif /* __THREAD_TABLE_H_ */
