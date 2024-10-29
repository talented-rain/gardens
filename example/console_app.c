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

#include "thread_table.h"

/*!< The defines */
#define CONSOLE_APP_THREAD_STACK_SIZE                       REAL_THREAD_STACK_HALF(1)    /*!< 1/2 page (1kbytes) */

/*!< The globals */
static real_thread_t g_console_app_tid;
static struct real_thread_attr sgrt_console_app_attr;
static kuint32_t g_console_app_stack[CONSOLE_APP_THREAD_STACK_SIZE];
static struct mailbox sgrt_console_app_mailbox;
static kuint8_t g_console_recv_buf[1024];

/*!< API functions */
/*!
 * @brief  console recieve task
 * @param  none
 * @retval none
 * @note   none
 */
static void *console_app_entry(void *args)
{
    struct mailbox *sprt_mb = &sgrt_console_app_mailbox;
    struct mail *sprt_mail = mrt_nullptr;
    struct mail_msg sgrt_msg[1] = {};
    kuint8_t *ptr_buf;
    kusize_t buf_size;
    static kuint8_t status = 0;
    kint32_t retval = -1;

    real_thread_set_name(__FUNCTION__);

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

        if (!kstrncmp((kchar_t *)ptr_buf, "led1 on", 7))
            status = 1;
        else if (!kstrncmp((kchar_t *)ptr_buf, "led1 off", 8))
            status = 0;
        else
            continue;

        if (sprt_mail)
            mail_destroy(sprt_mb, sprt_mail);

        sprt_mail = mail_create(sprt_mb);
        if (!isValid(sprt_mail))
        {
            sprt_mail = mrt_nullptr;
            continue;
        }
        
        sgrt_msg[0].buffer = &status;
        sgrt_msg[0].size = 1;
        sgrt_msg[0].type = NR_MAIL_TYPE_SERIAL;

        sprt_mail->sprt_msg = &sgrt_msg[0];
        sprt_mail->num_msgs = 1;

        mail_send("light-app-mailbox", sprt_mail);
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
    struct real_thread_attr *sprt_attr = &sgrt_console_app_attr;
    kint32_t retval;

	sprt_attr->detachstate = REAL_THREAD_CREATE_JOINABLE;
	sprt_attr->inheritsched	= REAL_THREAD_INHERIT_SCHED;
	sprt_attr->schedpolicy = REAL_THREAD_SCHED_FIFO;

    /*!< thread stack */
	real_thread_set_stack(sprt_attr, mrt_nullptr, g_console_app_stack, sizeof(g_console_app_stack));
    /*!< lowest priority */
	real_thread_set_priority(sprt_attr, REAL_THREAD_PROTY_DEFAULT);
    /*!< default time slice */
    real_thread_set_time_slice(sprt_attr, REAL_THREAD_TIME_DEFUALT);

    /*!< register thread */
    retval = real_thread_create(&g_console_app_tid, sprt_attr, console_app_entry, mrt_nullptr);
    return (retval < 0) ? retval : 0;
}

/*!< end of file */
