/*
 * Hardware Abstraction Layer Gpio Interface
 *
 * File Name:   fwk_gpiochip.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.04.27
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_GPIOCHIP_H_
#define __FWK_GPIOCHIP_H_

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/fwk_platform.h>
#include <platform/of/fwk_of.h>

/*!< The defines */
typedef struct fwk_gpio_desc srt_fwk_gpio_desc_t;
typedef struct fwk_gpio_chip srt_fwk_gpio_chip_t;
typedef struct fwk_gpio_node_prop srt_fwk_gpio_node_prop_t;

typedef struct fwk_gpio_chip
{
    const kchar_t *name;
    struct fwk_device *sprt_dev;
    kint32_t base;
    kuint32_t ngpios;
    struct fwk_gpio_desc *sprt_desc;
    struct list_head sgrt_link;
	struct list_head sgrt_pin_ranges;

	kint32_t   (*request) (struct fwk_gpio_chip *sprt_chip, kuint32_t offset);
	void       (*free) (struct fwk_gpio_chip *sprt_chip, kuint32_t offset);
	kint32_t   (*get_direction) (struct fwk_gpio_chip *sprt_chip, kuint32_t offset);
	kint32_t   (*direction_input) (struct fwk_gpio_chip *sprt_chip, kuint32_t offset);
	kint32_t   (*direction_output) (struct fwk_gpio_chip *sprt_chip, kuint32_t offset, kint32_t value);
	kint32_t   (*get) (struct fwk_gpio_chip *sprt_chip, kuint32_t offset);
	void       (*set) (struct fwk_gpio_chip *sprt_chip, kuint32_t offset, kint32_t value);

	kint32_t   (*to_irq) (struct fwk_gpio_chip *sprt_chip, kuint32_t offset);

	kuint32_t of_gpio_n_cells;
	kint32_t (*of_xlate)(struct fwk_gpio_chip *sprt_chip,
							const struct fwk_of_phandle_args *sprt_spec, kuint32_t *flags);

} srt_fwk_gpio_chip_t;

/*!< The functions */
extern kint32_t fwk_gpiochip_init(struct fwk_gpio_chip *sprt_chip, struct fwk_device *sprt_dev, kuint32_t base, kuint32_t size);
extern kint32_t fwk_gpiochip_add(struct fwk_gpio_chip *sprt_chip);
extern void fwk_gpiochip_del(struct fwk_gpio_chip *sprt_chip);
extern struct fwk_gpio_desc *fwk_gpiochip_and_desc_find(struct fwk_gpio_node_prop *sprt_data, struct fwk_of_phandle_args *sprt_spec);

#ifdef __cplusplus
    }
#endif

#endif /*!< __FWK_GPIOCHIP_H_ */
