/*
 * User Thread Instance (light task) Interface
 *
 * File Name:   light_app.cpp
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

#include "../task.h"
#include "test_task.h"

using namespace tsk;
using namespace stream;

/*!< The defines */
#define LIGHT_TASK_STACK_SIZE                   THREAD_STACK_HALF(1)    /*!< 1/2 page (1kbytes) */

/*!< The globals */

/*!< API functions */
/*!
 * @brief  light task
 * @param  none
 * @retval none
 * @note   trun on/off led by timer
 */
static void *light_task_entry(void *args)
{
    crt_task_t *cprt_this = (crt_task_t *)args;
    kbool_t status = 0;
    kint32_t fd;
    struct mailbox &sgrt_mb = cprt_this->get_mailbox();
    struct mail *sprt_mail;
    tid_t tid = cprt_this->get_self();
    
    cout << __FUNCTION__ << " is running, which tid is: " << tid << endl;

    for (;;)
    {        
        fd = virt_open("/dev/ledgpio", O_RDWR);
        if (fd < 0)
            goto END1;
        
        sprt_mail = mail_recv(&sgrt_mb, 0);
        if (!isValid(sprt_mail))
            goto END2;

        if (sprt_mail->sprt_msg->type == NR_MAIL_TYPE_SERIAL)
        {
            kchar_t *buffer = (kchar_t *)sprt_mail->sprt_msg[0].buffer;

            if (!kstrncmp(buffer, "on", 2))
                status = 1;
            else if (!kstrncmp(buffer, "off", 3))
                status = 0;
        }

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
kint32_t light_task_init(void)
{
    static kuint8_t g_light_task_stack[LIGHT_TASK_STACK_SIZE];

    crt_task_t *cprt_task = new crt_task_t("light task", 
                                            light_task_entry, 
                                            g_light_task_stack, 
                                            sizeof(g_light_task_stack));
    if (!cprt_task)
        return -ER_FAILD;

    struct mailbox &sgrt_mb = cprt_task->get_mailbox();
    mailbox_init(&sgrt_mb, cprt_task->get_self(), "light-task-mailbox");

    return ER_NORMAL;
}

/*!< end of file */
