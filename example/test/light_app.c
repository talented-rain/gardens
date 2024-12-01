/*
 * User Thread Instance (light task) Interface
 *
 * File Name:   light_app.c
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
#define LIGHTAPP_THREAD_STACK_SIZE                          REAL_THREAD_STACK_HALF(1)    /*!< 1/2 page (1kbytes) */

/*!< The globals */
static real_thread_t g_light_app_tid;
static struct real_thread_attr sgrt_light_app_attr;
static kuint32_t g_light_app_stack[LIGHTAPP_THREAD_STACK_SIZE];
static struct mailbox sgrt_light_app_mailbox;

/*!< API functions */
/*!
 * @brief  light task
 * @param  none
 * @retval none
 * @note   trun on/off led by timer
 */
static void *light_app_entry(void *args)
{
    kbool_t status = 0;
    kint32_t fd;
    struct mailbox *sprt_mb = &sgrt_light_app_mailbox;
    struct mail *sprt_mail;

    mailbox_init(&sgrt_light_app_mailbox, mrt_current->tid, "light-app-mailbox");

    for (;;)
    {       
//      print_info("%s is running, which tid is: %d\n", __FUNCTION__, mrt_current->tid);
        
        fd = virt_open("/dev/ledgpio", O_RDWR);
        if (fd < 0)
            goto END1;
        
        sprt_mail = mail_recv(sprt_mb, 0);
        if (!isValid(sprt_mail))
            goto END2;

        status = *sprt_mail->sprt_msg->buffer;
        virt_write(fd, &status, 1);

        mail_recv_finish(sprt_mail);

END2:
        virt_close(fd);
END1:
        schedule_delay_ms(200);
    }

    return args;
}

/*!
 * @brief	create light app task
 * @param  	none
 * @retval 	error code
 * @note   	none
 */
kint32_t light_app_init(void)
{
    struct real_thread_attr *sprt_attr = &sgrt_light_app_attr;
    kint32_t retval;

	sprt_attr->detachstate = REAL_THREAD_CREATE_JOINABLE;
	sprt_attr->inheritsched	= REAL_THREAD_INHERIT_SCHED;
	sprt_attr->schedpolicy = REAL_THREAD_SCHED_FIFO;

    /*!< thread stack */
	real_thread_set_stack(sprt_attr, mrt_nullptr, g_light_app_stack, sizeof(g_light_app_stack));
    /*!< lowest priority */
	real_thread_set_priority(sprt_attr, REAL_THREAD_PROTY_DEFAULT);
    /*!< default time slice */
    real_thread_set_time_slice(sprt_attr, REAL_THREAD_TIME_DEFUALT);

    /*!< register thread */
    retval = real_thread_create(&g_light_app_tid, sprt_attr, light_app_entry, mrt_nullptr);
    if (!retval)
        real_thread_set_name(g_light_app_tid, "light_app_entry");

    return retval;
}

/*!< end of file */
