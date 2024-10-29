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
real_thread_init_t g_real_thread_init_tables[] =
{
    console_app_init,
    light_app_init,
    button_app_init,
    display_app_init,
    tsc_app_init,
    env_monitor_init,
    
    mrt_nullptr,
};


/*!< end of file */
