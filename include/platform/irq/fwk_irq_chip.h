/*
 * Interrupt Interface Defines
 *
 * File Name:   fwk_irq_chip.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.05.13
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_IRQ_CHIP_H
#define __FWK_IRQ_CHIP_H

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The includes */
#include <platform/fwk_basic.h>

/*!< The defines */
typedef struct fwk_irq_data srt_fwk_irq_data_t;

typedef struct fwk_irq_chip 
{
	void (*irq_enable) (struct fwk_irq_data *sprt_data);
	void (*irq_disable) (struct fwk_irq_data *sprt_data);
	void (*irq_mask) (struct fwk_irq_data *sprt_data);
	void (*irq_unmask) (struct fwk_irq_data *sprt_data);
	kbool_t (*irq_ack) (struct fwk_irq_data *sprt_data);
	kbool_t (*irq_is_enabled) (struct fwk_irq_data *sprt_data);
	kint32_t (*irq_set_type) (struct fwk_irq_data *sprt_data, kuint32_t type);

} srt_fwk_irq_chip_t;

typedef struct fwk_irq_generic
{
	kuaddr_t manage_reg;
	kuaddr_t status_reg;

	struct fwk_irq_chip sgrt_chip;
	void *private_data;

} srt_fwk_irq_generic_t;

/*!< The globals */
extern struct fwk_irq_chip sgrt_fwk_irq_dummy_chip;
extern struct fwk_irq_chip sgrt_fwk_irq_generic_chip;

/*!< The functions */
extern void fwk_enable_irq(kint32_t irq);
extern void fwk_disable_irq(kint32_t irq);
extern void fwk_irq_set_type(kint32_t irq, kuint32_t flags);
extern kbool_t fwk_irq_is_acked(kint32_t irq);
extern kbool_t fwk_irq_is_enabled(kint32_t irq);

extern void fwk_irq_setup_generic_chip(kint32_t irq_base, kuint32_t irq_max, struct fwk_irq_generic *sprt_gc, void *chip_data);
extern void fwk_irq_shutdown_generic_chip(kint32_t irq_base, kuint32_t irq_max);
extern struct fwk_irq_generic *fwk_irq_get_generic_data(struct fwk_irq_data *sprt_data);

#ifdef __cplusplus
	}
#endif

#endif /* __FWK_IRQ_CHIP_H */
