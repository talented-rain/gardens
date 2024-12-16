/*
 * User Thread Instance (button task) Interface
 *
 * File Name:   button_app.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.05.25
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
#define BUTTONAPP_THREAD_STACK_SIZE                          REAL_THREAD_STACK_HALF(1)    /*!< 1/2 page (1kbytes) */

/*!< The globals */
static tid_t g_button_app_tid;
static struct real_thread_attr sgrt_button_app_attr;
static kuint32_t g_button_app_stack[BUTTONAPP_THREAD_STACK_SIZE];
static struct mailbox sgrt_button_app_mailbox;

/*!< API functions */
/*!
 * @brief  light task
 * @param  none
 * @retval none
 * @note   trun on/off led by timer
 */
static void *button_app_entry(void *args)
{
    kuint8_t status = 0, last_status = 0;
    kint32_t fd;
    struct mailbox *sprt_mb = &sgrt_button_app_mailbox;
    struct mail *sprt_mail = mrt_nullptr;
    struct mail_msg sgrt_msg[1] = {};
    kssize_t retval;

    mailbox_init(sprt_mb, mrt_current->tid, "button-app-mailbox");

    do {
        fd = virt_open("/dev/input/event0", O_RDONLY);
        if (mrt_unlikely(fd < 0))
            schedule_delay_ms(200);

    } while (fd < 0);

    for (;;)
    {
        retval = virt_read(fd, &status, 1);
        if ((retval < 0) || (status == last_status))
            goto END;
        
        if (sprt_mail)
            mail_destroy(sprt_mb, sprt_mail);

        sprt_mail = mail_create(sprt_mb);
        if (!isValid(sprt_mail))
        {
            sprt_mail = mrt_nullptr;
            goto END;
        }

        sgrt_msg[0].buffer = &status;
        sgrt_msg[0].size = 1;
        sgrt_msg[0].type = NR_MAIL_TYPE_KEY;

        sprt_mail->sprt_msg = &sgrt_msg[0];
        sprt_mail->num_msgs = 1;

        mail_send("light-app-mailbox", sprt_mail);
        last_status = status;

END:
        schedule_delay_ms(200);
    }

    virt_close(fd);
    return args;
}

/*!
 * @brief	create light app task
 * @param  	none
 * @retval 	error code
 * @note   	none
 */
kint32_t button_app_init(void)
{
    struct real_thread_attr *sprt_attr = &sgrt_button_app_attr;
    kint32_t retval;

	sprt_attr->detachstate = REAL_THREAD_CREATE_JOINABLE;
	sprt_attr->inheritsched	= REAL_THREAD_INHERIT_SCHED;
	sprt_attr->schedpolicy = REAL_THREAD_SCHED_FIFO;

    /*!< thread stack */
	real_thread_set_stack(sprt_attr, mrt_nullptr, g_button_app_stack, sizeof(g_button_app_stack));
    /*!< lowest priority */
	real_thread_set_priority(sprt_attr, __THREAD_HIGHER_DEFAULT(1));
    /*!< default time slice */
    real_thread_set_time_slice(sprt_attr, REAL_THREAD_TIME_DEFUALT);

    /*!< register thread */
    retval = real_thread_create(&g_button_app_tid, sprt_attr, button_app_entry, mrt_nullptr);
    if (!retval)
        real_thread_set_name(g_button_app_tid, "button_app_entry");

    return retval;
}

/*!< end of file */
