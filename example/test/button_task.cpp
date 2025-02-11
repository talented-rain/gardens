/*
 * User Thread Instance (button task) Interface
 *
 * File Name:   button_task.c
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

#include "../task.h"
#include "test_task.h"

using namespace tsk;
using namespace bsc;

/*!< The defines */
#define BUTTON_TASK_STACK_SIZE                          THREAD_STACK_HALF(1)    /*!< 1/2 page (1kbytes) */

/*!< The globals */

/*!< API functions */
/*!
 * @brief  button task
 * @param  none
 * @retval none
 * @note   trun on/off led by timer
 */
static void *button_task_entry(void *args)
{
    crt_task_t *cprt_this = (crt_task_t *)args;
    kuint8_t status = 0, last_status = 0;
    kint32_t fd;
    struct mailbox &sgrt_mb = cprt_this->get_mailbox();
    struct mail *sprt_mail = mrt_nullptr;
    struct mail_msg sgrt_msg[1] = {};
    kssize_t retval;

    do {
        fd = virt_open("/dev/input/event0", O_RDONLY);
        if (mrt_unlikely(fd < 0))
            msleep(200);

    } while (fd < 0);

    for (;;)
    {
        retval = virt_read(fd, &status, 1);
        if ((retval < 0) || (status == last_status))
            goto END;
        
        if (sprt_mail)
            mail_destroy(&sgrt_mb, sprt_mail);

        sprt_mail = mail_create(&sgrt_mb);
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
        msleep(200);
    }

    virt_close(fd);
    return args;
}

/*!
 * @brief	create button app task
 * @param  	none
 * @retval 	error code
 * @note   	none
 */
kint32_t button_task_init(void)
{
    static kuint8_t g_button_task_stack[BUTTON_TASK_STACK_SIZE];

    crt_task_t *cprt_task = new crt_task_t("button task", 
                                            button_task_entry, 
                                            g_button_task_stack, 
                                            sizeof(g_button_task_stack),
                                            __THREAD_HIGHER_DEFAULT(1));
    if (!cprt_task)
        return -ER_FAILD;

    struct mailbox &sgrt_mb = cprt_task->get_mailbox();
    mailbox_init(&sgrt_mb, cprt_task->get_self(), "button-task-mailbox");

    return ER_NORMAL;
}

/*!< end of file */
