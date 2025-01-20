/*
 * Power Management Defines
 *
 * File Name:   fwk_pm.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.05.08
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_PM_H_
#define __FWK_PM_H_

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/fwk_platform.h>

/*!< The defines */
struct fwk_dev_pm_ops 
{
	kint32_t (*prepare)(struct fwk_device *sprt_dev);
	void (*complete)(struct fwk_device *sprt_dev);
	kint32_t (*suspend)(struct fwk_device *sprt_dev);
	kint32_t (*resume)(struct fwk_device *sprt_dev);
	kint32_t (*freeze)(struct fwk_device *sprt_dev);
	kint32_t (*thaw)(struct fwk_device *sprt_dev);
	kint32_t (*poweroff)(struct fwk_device *sprt_dev);
	kint32_t (*restore)(struct fwk_device *sprt_dev);
	kint32_t (*suspend_late)(struct fwk_device *sprt_dev);
	kint32_t (*resume_early)(struct fwk_device *sprt_dev);
	kint32_t (*freeze_late)(struct fwk_device *sprt_dev);
	kint32_t (*thaw_early)(struct fwk_device *sprt_dev);
	kint32_t (*poweroff_late)(struct fwk_device *sprt_dev);
	kint32_t (*restore_early)(struct fwk_device *sprt_dev);
	kint32_t (*suspend_noirq)(struct fwk_device *sprt_dev);
	kint32_t (*resume_noirq)(struct fwk_device *sprt_dev);
	kint32_t (*freeze_noirq)(struct fwk_device *sprt_dev);
	kint32_t (*thaw_noirq)(struct fwk_device *sprt_dev);
	kint32_t (*poweroff_noirq)(struct fwk_device *sprt_dev);
	kint32_t (*restore_noirq)(struct fwk_device *sprt_dev);
	kint32_t (*runtime_suspend)(struct fwk_device *sprt_dev);
	kint32_t (*runtime_resume)(struct fwk_device *sprt_dev);
	kint32_t (*runtime_idle)(struct fwk_device *sprt_dev);
};

#ifdef __cplusplus
    }
#endif

#endif /*!< __FWK_PM_H_ */
