/*
 * Main Initialization
 *
 * File Name:   main.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.09.10
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <configs/configs.h>
#include <common/basic_types.h>
#include <common/error_types.h>
#include <common/generic.h>
#include <common/atomic_types.h>
#include <common/io_stream.h>
#include <common/time.h>
#include <boot/implicit_call.h>
#include <boot/board_init.h>
#include <platform/of/fwk_of.h>
#include <platform/of/fwk_of_device.h>
#include <platform/fwk_fcntl.h>
#include <platform/irq/fwk_irq.h>
#include <kernel/sched.h>
#include <kernel/thread.h>
#include <kernel/instance.h>
#include <fs/fs_intr.h>
#include <fs/fs_fatfs.h>

/*!< The globals */
static struct tag_params *sprt_tag_params;

/*!< The defines */
#define mrt_tag_params_get()    \
({    \
    struct tag_params *sprt_param;  \
    __asm__ __volatile__ (  \
        " str r2, [%0]  \n\t"   \
        : \
        : "r"(&sprt_param)   \
        : "cc","memory" \
    );  \
    sprt_param; \
})

/*!< API functions */
/*!
 * @brief  populate params
 * @param  sprt_params
 * @retval none
 * @note   none
 */
void setup_tag_params(struct tag_params *sprt_params)
{
    while (sprt_params->sgrt_hdr.type != (-1))
    {
        switch (sprt_params->sgrt_hdr.type)
        {
            case TAG_PARAM_VIDEO:
                sprt_fwk_video_params = &sprt_params->u.sgrt_vdp;
                break;
            case TAG_PARAM_FDT:
                sprt_fwk_fdt_params = &sprt_params->u.sgrt_fdt;
                break;

            default: break;
        }

        sprt_params = TAG_PARAM_NEXT(sprt_params);
    }
}

/*!
 * @brief  setup machine
 * @param  none
 * @retval none
 * @note   cpu param populate
 */
void setup_machine(struct tag_params *sprt_params)
{
    setup_tag_params(sprt_params);

    /*!< build device-tree */
    setup_machine_fdt(sprt_fwk_fdt_params);
}

/*!
 * @brief  start_kernel
 * @param  none
 * @retval none
 * @note   kernel main
 */
void start_kernel(void)
{
    /*!< disable interrupt */
    mrt_disable_cpu_irq();
    sprt_tag_params = mrt_tag_params_get();

    /*!< initial memory pool */
    fwk_mempool_initial();
    print_info("\nstart kernel ...... \n");

    /*!< populate params from bootloader */
    setup_machine(sprt_tag_params);

    /*!< board initcall */
    if (run_machine_initcall())
        goto fail;

    /*!< initial irq */
    initIRQ();

    /*!< systick init */
    board_init_systick();

    /*!< file system */
    if (filesystem_initcall())
        goto fail;

    /*!< populate device node after initializing hardware */
    if (fwk_of_platform_populate_init())
        goto fail;

    /* platform initcall */
    if (run_platform_initcall())
        goto fail;

    /*!< enable interrupt */
    mrt_enable_cpu_irq();

#if CONFIG_SCHDULE
    /*!< create thread */
    if (kthread_init())
        goto fail;

    rest_init();
    print_info("initial system finished, start scheduler now\n");

    /*!< start */
    schedule_thread();

#endif

fail:
    print_info("start kernel failed!\n");
    mrt_assert(false);
}

/* end of file */
