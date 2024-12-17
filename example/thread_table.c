/*
 * User Thread Instance Tables
 *
 * File Name:   thread_table.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.04.02
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The globals */
#include "thread_table.h"

/*!< The defines */

/*!< The globals */
real_thread_init_t g_thread_init_tables[] =
{
    /*!< test app */
    mrt_test_app_list,
    
    /*!< applications */
//  lvgl_task_init,
//  lwip_task_init,
    
    /*!< end */
    mrt_nullptr,
};


/*!< end of file */
