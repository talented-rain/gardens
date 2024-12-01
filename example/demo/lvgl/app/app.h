/*
 * User Thread Instance Tables
 *
 * File Name:   app.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.11.17
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __LVGL_APP_H_
#define __LVGL_APP_H_

/*!< The globals */
#include <common/basic_types.h>
#include <common/error_types.h>
#include <common/generic.h>
#include <common/list_types.h>
#include <kernel/instance.h>
#include <lvgl/lvgl.h>

/*!< The defines */

/*!< The globals */

/*!< The functions */
TARGET_EXT void lvgl_task_startup(void *args);
TARGET_EXT void lvgl_task(void *args);

#endif /* __LVGL_APP_H_ */
