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

#include "test_app.h"

/*!< The defines */
#define ENVAPP_THREAD_STACK_SIZE                          REAL_THREAD_STACK_HALF(1)    /*!< 1/2 page (2kbytes) */

/*!< The globals */
static real_thread_t g_env_monitor_tid;
static struct real_thread_attr sgrt_env_monitor_attr;
static kuint32_t g_env_monitor_stack[ENVAPP_THREAD_STACK_SIZE];

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
            schedule_delay_ms(200);

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

        print_info("%s: ir: %d, als: %d, ps: %d\n", __FUNCTION__, info[0], info[1], info[2]);
        virt_close(eep_fd);
        
END:
        schedule_delay_ms(200);
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
    struct real_thread_attr *sprt_attr = &sgrt_env_monitor_attr;
    kint32_t retval;

	sprt_attr->detachstate = REAL_THREAD_CREATE_JOINABLE;
	sprt_attr->inheritsched	= REAL_THREAD_INHERIT_SCHED;
	sprt_attr->schedpolicy = REAL_THREAD_SCHED_FIFO;

    /*!< thread stack */
	real_thread_set_stack(sprt_attr, mrt_nullptr, g_env_monitor_stack, sizeof(g_env_monitor_stack));
    /*!< lowest priority */
	real_thread_set_priority(sprt_attr, REAL_THREAD_PROTY_DEFAULT);
    /*!< default time slice */
    real_thread_set_time_slice(sprt_attr, REAL_THREAD_TIME_DEFUALT);

    /*!< register thread */
    retval = real_thread_create(&g_env_monitor_tid, sprt_attr, env_monitor_entry, mrt_nullptr);
    if (!retval)
        real_thread_set_name(g_env_monitor_tid, "env_monitor_entry");

    return retval;
}

/*!< end of file */
