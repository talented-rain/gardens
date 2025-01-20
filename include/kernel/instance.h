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

#ifndef __KERNEL_INSTANCE_H_
#define __KERNEL_INSTANCE_H_

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The includes */
#include <kernel/kernel.h>
#include <kernel/thread.h>

/*!< The defines */

/*!< The functions */
/*!< kernel */
extern kint32_t rest_init(void);
extern kint32_t kthread_init(void);
extern kint32_t kworker_init(void);
extern kint32_t term_init(void);

/*!< application */
extern kint32_t init_proc_init(void);

extern kint32_t main(kint32_t argc, kchar_t **argv);

#ifdef __cplusplus
    }
#endif

#endif /* __KERNEL_CONTEXT_H_ */
