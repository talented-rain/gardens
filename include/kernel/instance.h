/*
 * Kernel Thread Instance Defines
 *
 * File Name:   instance.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.04.01
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __KEL_INSTANCE_H_
#define __KEL_INSTANCE_H_

/*!< The includes */
#include <kernel/kernel.h>
#include <kernel/thread.h>

/*!< The defines */
typedef kint32_t (*real_thread_init_t)(void);

/*!< The globals */
TARGET_EXT real_thread_init_t g_thread_init_tables[];

/*!< The functions */
TARGET_EXT kint32_t rest_init(void);
TARGET_EXT kint32_t kthread_init(void);
TARGET_EXT kint32_t kworker_init(void);

TARGET_EXT kint32_t init_proc_init(void);

/*!< API functions */
/*!
 * @brief	init thread tables
 * @param  	none
 * @retval 	none
 * @note   	only for user thread; called by init_proc
 */
static inline void init_real_thread_table(void)
{
    const real_thread_init_t *pFunc_init;
    
    for (pFunc_init = g_thread_init_tables; (*pFunc_init); pFunc_init++)
    {
        /*!< ignore returns error */
        (*pFunc_init)();
    }
}

#endif /* __KEL_CONTEXT_H_ */
