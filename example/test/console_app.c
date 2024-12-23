/*
 * User Thread Instance (light task) Interface
 *
 * File Name:   console_app.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.04.01
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The globals */
#include <common/basic_types.h>
#include <common/error_types.h>
#include <common/generic.h>
#include <common/io_stream.h>
#include <platform/fwk_fcntl.h>
#include <kernel/kernel.h>
#include <kernel/sched.h>
#include <kernel/thread.h>
#include <kernel/sleep.h>
#include <kernel/mutex.h>
#include <kernel/mailbox.h>

#include "test_app.h"

/*!< The defines */
#define CONSOLE_APP_THREAD_STACK_SIZE                       THREAD_STACK_HALF(1)    /*!< 1/2 page (1kbytes) */

/*!< The globals */
static tid_t g_console_app_tid;
static struct thread_attr sgrt_console_app_attr;
static kuint32_t g_console_app_stack[CONSOLE_APP_THREAD_STACK_SIZE];
static struct mailbox sgrt_console_app_mailbox;
static kuint8_t g_console_recv_buf[1024];

/*!< API functions */
/*!
 * @brief  send mail to light thread
 * @param  sprt_mb, command_line
 * @retval none
 * @note   none
 */
static void command_mail_to_light(struct mailbox *sprt_mb, kuint8_t *command_line)
{
    struct mail sgrt_mail;
    struct mail_msg sgrt_msg[1] = {};
    kuint8_t status = 0;

    if (!kstrncmp((kchar_t *)command_line, "led1 on", 7))
        status = 1;
    else if (!kstrncmp((kchar_t *)command_line, "led1 off", 8))
        status = 0;
    else
        return;

    mail_init(sprt_mb, &sgrt_mail);
    
    sgrt_msg[0].buffer = &status;
    sgrt_msg[0].size = 1;
    sgrt_msg[0].type = NR_MAIL_TYPE_SERIAL;

    sgrt_mail.sprt_msg = &sgrt_msg[0];
    sgrt_mail.num_msgs = 1;

    mail_send("light-app-mailbox", &sgrt_mail);
}

/*!
 * @brief  send mail to display thread
 * @param  sprt_mb, command_line
 * @retval none
 * @note   none
 */
static void command_mail_to_display(struct mailbox *sprt_mb, kuint8_t *command_line)
{
    struct mail sgrt_mail;
    struct mail_msg sgrt_msg[1] = {};
    kuint8_t status = 0;

    if (!kstrncmp((kchar_t *)command_line, "page up", 9))
        status = 1;
    else if (!kstrncmp((kchar_t *)command_line, "page down", 8))
        status = 2;
    else
        return;

    mail_init(sprt_mb, &sgrt_mail);
    
    sgrt_msg[0].buffer = &status;
    sgrt_msg[0].size = 1;
    sgrt_msg[0].type = NR_MAIL_TYPE_SERIAL;

    sgrt_mail.sprt_msg = &sgrt_msg[0];
    sgrt_mail.num_msgs = 1;

    mail_send("display-app-mailbox", &sgrt_mail);
}

/*!
 * @brief  console recieve task
 * @param  none
 * @retval none
 * @note   none
 */
static void *console_app_entry(void *args)
{
    struct mailbox *sprt_mb = &sgrt_console_app_mailbox;
    kuint8_t *ptr_buf;
    kusize_t buf_size;
    kint32_t retval = -1;

    ptr_buf = &g_console_recv_buf[0];
    buf_size = sizeof(g_console_recv_buf);

    mailbox_init(sprt_mb, mrt_current->tid, "console-app-mailbox");

    for (;;)
    {       
        do {
            /*!< read command line */
            retval = io_getstr(ptr_buf, buf_size);

        } while (retval <= 0);

        print_info("recv command line, data is: %s\n", ptr_buf);

        command_mail_to_light(sprt_mb, ptr_buf);
        command_mail_to_display(sprt_mb, ptr_buf);
    }

    return args;
}

/*!
 * @brief	create console app task
 * @param  	none
 * @retval 	error code
 * @note   	none
 */
kint32_t console_app_init(void)
{
    struct thread_attr *sprt_attr = &sgrt_console_app_attr;
    kint32_t retval;

    sprt_attr->detachstate = THREAD_CREATE_JOINABLE;
    sprt_attr->inheritsched	= THREAD_INHERIT_SCHED;
    sprt_attr->schedpolicy = THREAD_SCHED_FIFO;

    /*!< thread stack */
    thread_set_stack(sprt_attr, mrt_nullptr, g_console_app_stack, sizeof(g_console_app_stack));
    /*!< lowest priority */
    thread_set_priority(sprt_attr, THREAD_PROTY_DEFAULT);
    /*!< default time slice */
    thread_set_time_slice(sprt_attr, THREAD_TIME_DEFUALT);

    /*!< register thread */
    retval = thread_create(&g_console_app_tid, sprt_attr, console_app_entry, mrt_nullptr);
    if (!retval)
        thread_set_name(g_console_app_tid, "console_app_entry");

    return retval;
}

/*!< end of file */
