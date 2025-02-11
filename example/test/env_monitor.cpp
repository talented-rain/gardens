/*
 * User Thread Instance (environment monitor task) Interface
 *
 * File Name:   env_monitor.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.06.06
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
#include <platform/i2c/fwk_eeprom.h>
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
#define ENV_TASK_STACK_SIZE                         THREAD_STACK_HALF(1)    /*!< 1/2 page (2kbytes) */

/*!< The globals */

/*!< API functions */
/*!
 * @brief  environment monitor task
 * @param  none
 * @retval none
 * @note   monitor environment
 */
static void *env_monitor_entry(void *args)
{
    kint32_t fd, eep_fd;
    kuint32_t info[3] = {};
    struct fwk_eeprom sgrt_eep;
    kssize_t retval;

    do 
    {
        fd = virt_open("/dev/ap3216c", O_RDWR);
        if (fd < 0)
            msleep(200);

    } while (fd < 0);

    sgrt_eep.addr = 0x26;
    sgrt_eep.offset = 0;
    sgrt_eep.buf = (kuint8_t *)&info[0];
    sgrt_eep.size = sizeof(info);

    for (;;)
    {
        retval = virt_read(fd, info, sizeof(info));
        if (retval < 0)
            goto END;

        eep_fd = virt_open("/dev/at24c02", O_RDWR);
        if (eep_fd < 0)
            goto END;

        retval = virt_ioctl(eep_fd, FWK_EEPROM_WRITE, &sgrt_eep);
        if (retval < 0)
        {
            virt_close(eep_fd);
            goto END;
        }

        memset(info, 0, sizeof(info));
        retval = virt_ioctl(eep_fd, FWK_EEPROM_READ, &sgrt_eep);
        if (retval < 0)
        {
            virt_close(eep_fd);
            goto END;
        }

        cout << __FUNCTION__ 
             << ": ir: "    << info[0] 
             << ", als: "   << info[1] 
             << ", ps: "    << info[2] 
             << endl;
        
        virt_close(eep_fd);
        
END:
        msleep(200);
    }

    virt_close(fd);
    return args;
}

/*!
 * @brief	create environment monitor task
 * @param  	none
 * @retval 	error code
 * @note   	none
 */
kint32_t env_monitor_init(void)
{
    static kuint8_t g_env_monitor_stack[ENV_TASK_STACK_SIZE];

    crt_task_t *cprt_task = new crt_task_t("env_monitor task", 
                                            env_monitor_entry, 
                                            g_env_monitor_stack, 
                                            sizeof(g_env_monitor_stack));
    if (!cprt_task)
        return -ER_FAILD;

    struct mailbox &sgrt_mb = cprt_task->get_mailbox();
    mailbox_init(&sgrt_mb, cprt_task->get_self(), "env_monitor-task-mailbox");

    return ER_NORMAL;
}

/*!< end of file */
