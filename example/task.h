/*
 * User Thread Instance Tables
 *
 * File Name:   task.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.04.02
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __TASK_TABLE_H_
#define __TASK_TABLE_H_

#ifdef __cplusplus

/*!< The globals */
#include <common/basic_types.h>
#include <common/error_types.h>
#include <common/generic.h>
#include <common/list_types.h>
#include <common/libcxplus.h>
#include <kernel/instance.h>
#include <kernel/kernel.h>
#include <kernel/sched.h>
#include <kernel/thread.h>
#include <kernel/sleep.h>
#include <kernel/mutex.h>
#include <kernel/mailbox.h>

/*!< The defines */
/*!< ------------------------------------------------------------- */
BEGIN_NAMESPACE(tsk)
/*!< ------------------------------------------------------------- */

typedef void *(*task_entry)(void *);

class crt_task_t {
public:
    crt_task_t(const kchar_t *name, void *(*task_entry)(void *), kuint8_t *stack = mrt_nullptr, 
            kuint32_t size = 0, kuint32_t prio = THREAD_PROTY_DEFAULT, kuint32_t tslice = THREAD_TIME_DEFUALT);
    ~crt_task_t();

    tid_t get_self(void)
    {
        return tid;
    }

    struct mailbox &get_mailbox(void)
    {
        return sgrt_mb;
    }

private:
    tid_t tid;
    struct thread_attr sgrt_attr;
    struct mailbox sgrt_mb;

    kuint8_t *stack_base;
    kuint32_t stack_size;
    kbool_t isdync;
};

/*!< ------------------------------------------------------------- */
END_NAMESPACE(tsk)
/*!< ------------------------------------------------------------- */

/*!< The globals */

/*!< The functions */

#endif

#endif /* __TASK_TABLE_H_ */
