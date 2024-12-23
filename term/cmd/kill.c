/*
 * Terminal Core API: Command kill
 *
 * File Name:   kill.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.12.23
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <platform/fwk_basic.h>
#include <kernel/signal.h>
#include <kernel/sched.h>
#include <term/term.h>

/*!< The defines */


/*!< The globals */


/*!< The functions */

/*!< API functions */
/*!
 * @brief   cmd 'kill': excute function
 * @param   sprt_cmd, argc, argv
 * @retval  errno
 * @note    none
 */
static kint32_t term_cmd_kill_signal(struct term_cmd *sprt_cmd, kint32_t argc, kchar_t **argv)
{
    kint32_t signal;
    tid_t tid;

    switch (argc)
    {
        case 3:
            if (*(argv[1]) != '-')
                goto fail;

            signal = ascii_to_dec(argv[1] + 1);
            if (signal < 0)
                goto fail;

            switch (signal)
            {
                case SIGKILL:
                    tid = ascii_to_dec(argv[2]);
                    if (tid < 0)
                        goto fail;

                    schedule_thread_sleep(tid);
                    break;

                case SIGWAKE:
                    tid = ascii_to_dec(argv[2]);
                    if (tid < 0)
                        goto fail;

                    schedule_thread_wakeup(tid);
                    break;
            }

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
 * @brief   cmd 'kill': help function
 * @param   none
 * @retval  none
 * @note    none
 */
static void term_cmd_kill_help(void)
{
    printk("usage: kill -[signal] [tid]\n");
}

/*!
 * @brief   cmd 'kill' init and add
 * @param   none
 * @retval  none
 * @note    none
 */
void term_cmd_add_kill(void)
{
    struct term_cmd *sprt_cmd;

    sprt_cmd = term_cmd_allocate("kill", GFP_KERNEL);
    if (!isValid(sprt_cmd))
        return;

    sprt_cmd->do_excute = term_cmd_kill_signal;
    sprt_cmd->help = term_cmd_kill_help;

    term_cmd_add(sprt_cmd);
}
