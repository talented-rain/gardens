/*
 * Terminal Core API: Command ts
 *
 * File Name:   ts.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.12.24
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <platform/fwk_basic.h>
#include <kernel/signal.h>
#include <kernel/sched.h>
#include <kernel/spinlock.h>
#include <term/term.h>

/*!< The defines */


/*!< The globals */


/*!< The functions */

/*!< API functions */
/*!
 * @brief   display title
 * @param   none
 * @retval  none
 * @note    none
 */
static void term_cmd_ts_title(void)
{
    printk("tid     stack_size     priority  slice   status      name\n");
    printk("----------------------------------------------------------\n");
    printk("status: (1)running, (2)ready, (3)suspend, (4)sleep\n");
    printk("----------------------------------------------------------\n");
}

/*!
 * @brief   show thread info
 * @param   tid, stack_size, ...
 * @retval  none
 * @note    none
 */
static void term_cmd_ts_format(tid_t tid, kuint32_t stack_size, kuint32_t prio, 
                        kutime_t expires, kuint32_t status, kchar_t *name)
{
    printk("%d      %d (Bytes)      %d      %d (ms)", 
            tid,    stack_size,     prio,   expires);
    printk("    %d      %s\n", status,  name);
}

/*!
 * @brief   cmd 'ts': excute function
 * @param   sprt_cmd, argc, argv
 * @retval  errno
 * @note    none
 */
static kint32_t term_cmd_task_show(struct term_cmd *sprt_cmd, kint32_t argc, kchar_t **argv)
{
    struct thread *sprt_thread;
    struct thread_attr *sprt_attr;
    struct spin_lock *sprt_lock;

    switch (argc)
    {
        case 1:
            sprt_lock = scheduler_lock();
            spin_lock_irqsave(sprt_lock);

            term_cmd_ts_title();

            /*!< 1. running */
            sprt_thread = mrt_current;
            sprt_attr = sprt_thread->sprt_attr;
            term_cmd_ts_format(sprt_thread->tid, sprt_attr->stacksize, sprt_attr->sgrt_param.sched_curpriority,
                        thread_get_sched_msecs(sprt_attr), sprt_thread->status, sprt_thread->name);
            
            /*!< 2. ready */
            sprt_thread = mrt_nullptr;
            while ((sprt_thread = next_ready_thread(sprt_thread)))
            {
                sprt_attr = sprt_thread->sprt_attr;
                term_cmd_ts_format(sprt_thread->tid, sprt_attr->stacksize, sprt_attr->sgrt_param.sched_curpriority,
                        thread_get_sched_msecs(sprt_attr), sprt_thread->status, sprt_thread->name);
            }

            /*!< 3. suspend */
            sprt_thread = mrt_nullptr;
            while ((sprt_thread = next_suspend_thread(sprt_thread)))
            {
                sprt_attr = sprt_thread->sprt_attr;
                term_cmd_ts_format(sprt_thread->tid, sprt_attr->stacksize, sprt_attr->sgrt_param.sched_curpriority,
                        thread_get_sched_msecs(sprt_attr), sprt_thread->status, sprt_thread->name);
            }

            /*!< 4. sleep */
            sprt_thread = mrt_nullptr;
            while ((sprt_thread = next_sleep_thread(sprt_thread)))
            {
                sprt_attr = sprt_thread->sprt_attr;
                term_cmd_ts_format(sprt_thread->tid, sprt_attr->stacksize, sprt_attr->sgrt_param.sched_curpriority,
                        thread_get_sched_msecs(sprt_attr), sprt_thread->status, sprt_thread->name);
            }

            spin_unlock_irqrestore(sprt_lock);
            break;

        case 2:
            if (!strcmp(argv[1], "--help"))
                sprt_cmd->help();
            else
                goto fail;

            break;

        default: 
            goto fail;
    }

    return ER_NORMAL;

fail:
    printk("argument error, try entering \'%s --help\' to get usage\n", argv[0]);
    return -ER_FAULT;
}

/*!
 * @brief   cmd 'ts': help function
 * @param   none
 * @retval  none
 * @note    none
 */
static void term_cmd_ts_help(void)
{
    printk("usage: ts\n");
}

/*!
 * @brief   cmd 'ts' init and add
 * @param   none
 * @retval  none
 * @note    none
 */
void term_cmd_add_ts(void)
{
    struct term_cmd *sprt_cmd;

    sprt_cmd = term_cmd_allocate("ts", GFP_KERNEL);
    if (!isValid(sprt_cmd))
        return;

    sprt_cmd->do_excute = term_cmd_task_show;
    sprt_cmd->help = term_cmd_ts_help;

    term_cmd_add(sprt_cmd);
}
