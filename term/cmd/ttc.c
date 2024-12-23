/*
 * Terminal Core API: Command ttc (terminal thread comunication)
 *
 * File Name:   ttc.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.12.23
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <platform/fwk_basic.h>
#include <term/term.h>
#include <kernel/sched.h>
#include <kernel/thread.h>
#include <kernel/mailbox.h>

/*!< The defines */


/*!< The globals */

 
/*!< The functions */


/*!< API functions */
/*!
 * @brief   cmd 'ttc': excute function
 * @param   sprt_cmd, argc, argv
 * @retval  errno
 * @note    none
 */
static kint32_t term_cmd_ttc_mail(struct term_cmd *sprt_cmd, kint32_t argc, kchar_t **argv)
{
    struct thread *sprt_thread;
    struct mailbox *sprt_tar;
    struct mail sgrt_mail;
    struct mail_msg sgrt_msg[1] = {};
    tid_t tid;

    switch (argc)
    {
        case 2:
            if (!strcmp(argv[1], "--help"))
                sprt_cmd->help();
            else
                goto fail;

            break;

        case 3:
            tid = ascii_to_dec(argv[1]);
            if (tid < 0)
            {
                printk("tid is not correct!\n");
                return -ER_FAULT;
            }

            sprt_thread = get_thread_handle(tid);
            if (!isValid(sprt_thread))
            {
                printk("can not find thread, check the tid please\n");
                return -ER_UNVALID;
            }

            /*!< if destination thread is sleepy, mail should not be sent */
            if (!is_thread_valid(tid))
                break;

            mail_init(mrt_current->sprt_mb, &sgrt_mail);
            
            sgrt_msg[0].buffer = (kuint8_t *)argv[2];
            sgrt_msg[0].size = strlen(argv[2]) + 1;
            sgrt_msg[0].type = NR_MAIL_TYPE_SERIAL;

            sgrt_mail.sprt_msg = &sgrt_msg[0];
            sgrt_mail.num_msgs = 1;

            sprt_tar = sprt_thread->sprt_mb;
            mail_send(sprt_tar->name, &sgrt_mail);

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
 * @brief   cmd 'ttc': help function
 * @param   none
 * @retval  none
 * @note    none
 */
static void term_cmd_ttc_help(void)
{
    printk("usage: ttc [tid] [op]\n");
}

/*!
 * @brief   cmd 'ttc' init and add
 * @param   none
 * @retval  none
 * @note    none
 */
void term_cmd_add_ttc(void)
{
    struct term_cmd *sprt_cmd;

    sprt_cmd = term_cmd_allocate("ttc", GFP_KERNEL);
    if (!isValid(sprt_cmd))
        return;

    sprt_cmd->do_excute = term_cmd_ttc_mail;
    sprt_cmd->help = term_cmd_ttc_help;

    term_cmd_add(sprt_cmd);
}