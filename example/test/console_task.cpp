/*
 * User Thread Instance (light task) Interface
 *
 * File Name:   console_task.c
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
#define CONSOLE_TASK_STACK_SIZE                       THREAD_STACK_HALF(1)    /*!< 1/2 page (1kbytes) */

/*!< The globals */

/*!< API functions */
/*!
 * @brief  send mail to light thread
 * @param  cprt_this, command_line
 * @retval none
 * @note   none
 */
static void command_mail_to_light(crt_task_t *cprt_this, kuint8_t *command_line)
{
    struct mailbox &sgrt_mb = cprt_this->get_mailbox();
    struct mail sgrt_mail;
    struct mail_msg sgrt_msg[1] = {};
    kuint8_t status = 0;

    if (!kstrncmp((kchar_t *)command_line, "led1 on", 7))
        status = 1;
    else if (!kstrncmp((kchar_t *)command_line, "led1 off", 8))
        status = 0;
    else
        return;

    mail_init(&sgrt_mb, &sgrt_mail);
    
    sgrt_msg[0].buffer = &status;
    sgrt_msg[0].size = 1;
    sgrt_msg[0].type = NR_MAIL_TYPE_SERIAL;

    sgrt_mail.sprt_msg = &sgrt_msg[0];
    sgrt_mail.num_msgs = 1;

    mail_send("light-app-mailbox", &sgrt_mail);
}

/*!
 * @brief  send mail to display thread
 * @param  cprt_this, command_line
 * @retval none
 * @note   none
 */
static void command_mail_to_display(crt_task_t *cprt_this, kuint8_t *command_line)
{
    struct mailbox &sgrt_mb = cprt_this->get_mailbox();
    struct mail sgrt_mail;
    struct mail_msg sgrt_msg[1] = {};
    kuint8_t status = 0;

    if (!kstrncmp((kchar_t *)command_line, "page up", 9))
        status = 1;
    else if (!kstrncmp((kchar_t *)command_line, "page down", 8))
        status = 2;
    else
        return;

    mail_init(&sgrt_mb, &sgrt_mail);
    
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
static void *console_task_entry(void *args)
{
    static kuint8_t g_console_recv_buf[1024];
    
    crt_task_t *cprt_this = (crt_task_t *)args;
    string cgrt_str(&g_console_recv_buf[0], sizeof(g_console_recv_buf));

    for (;;)
    {       
        do {
            /*!< read command line */
            cin >> cgrt_str;

        } while (cgrt_str.real_size <= 0);

        cout << "recv command line, data is: " << cgrt_str.get_buf() << endl;

        command_mail_to_light(cprt_this, cgrt_str.get_buf());
        command_mail_to_display(cprt_this, cgrt_str.get_buf());
    }

    return args;
}

/*!
 * @brief	create console app task
 * @param  	none
 * @retval 	error code
 * @note   	none
 */
kint32_t console_task_init(void)
{
    static kuint8_t g_console_task_stack[CONSOLE_TASK_STACK_SIZE];

    crt_task_t *cprt_task = new crt_task_t("console task", 
                                            console_task_entry, 
                                            g_console_task_stack, 
                                            sizeof(g_console_task_stack));
    if (!cprt_task)
        return -ER_FAILD;

    struct mailbox &sgrt_mb = cprt_task->get_mailbox();
    mailbox_init(&sgrt_mb, cprt_task->get_self(), "console-task-mailbox");

    return ER_NORMAL;
}

/*!< end of file */
