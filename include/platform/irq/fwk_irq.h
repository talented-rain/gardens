/*
 * Interrupt Interface Defines
 *
 * File Name:   fwk_irq.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.01.10
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_IRQ_H
#define __FWK_IRQ_H

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The includes */
#include <platform/fwk_basic.h>

/*!< The defines */

/*!< The functions */
extern void initIRQ(void);
extern void fwk_of_irq_init(const struct fwk_of_device_id *sprt_matches);

#ifdef __cplusplus
	}
#endif

#endif /* __FWK_IRQ_H */
