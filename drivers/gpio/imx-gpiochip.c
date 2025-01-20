/*
 * Template of Character Device : LED
 *
 * File Name:   imx_gpiochip.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.11.26
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/of/fwk_of.h>
#include <platform/of/fwk_of_device.h>
#include <platform/fwk_platdrv.h>
#include <platform/gpio/fwk_gpiochip.h>
#include <platform/gpio/fwk_gpiodesc.h>
#include <platform/irq/fwk_irq_chip.h>
#include <platform/irq/fwk_irq_domain.h>
#include <platform/irq/fwk_irq_types.h>

#include <imx6/imx6ull_periph.h>

/*!< The defines */
typedef struct imx_gpio_port
{
	void *base;
	kint32_t id;
	kint32_t irq_high;
	kint32_t irq_low;
	struct fwk_gpio_chip sgrt_gpiochip;
	struct fwk_irq_domain *sprt_irqdomain;

	kint32_t irq_base;
	struct fwk_irq_generic *sprt_irqgc;

	struct list_head sgrt_link;

} srt_imx_gpio_port_t;

/*!< The globals */
static DECLARE_LIST_HEAD(sgrt_imx_gpio_ports_list);

/*!< API function */
/*!
 * @brief   isr
 * @param   ptrDev
 * @retval  errno
 * @note    none
 */
static irq_return_t imx_gpiochip_driver_isr(void *ptrDev)
{
	struct imx_gpio_port *sprt_port;
	struct fwk_irq_domain *sprt_domain;
	srt_hal_imx_gpio_t *sprt_reg;
	kbool_t irq_ena, irq_sta;
	kuint32_t idx, irq;

	sprt_port = (struct imx_gpio_port *)ptrDev;
	sprt_reg = (srt_hal_imx_gpio_t *)sprt_port->base;
	sprt_domain = sprt_port->sprt_irqdomain;
	
	for (idx = 0; idx < 32; idx++)
	{
		irq_ena = !!mrt_getbitl(mrt_bit(idx), &sprt_reg->IMR);
		irq_sta = !!mrt_getbitl(mrt_bit(idx), &sprt_reg->ISR);

		if (!irq_ena || !irq_sta)
			continue;
		
		irq = fwk_irq_get_by_domain(sprt_domain, idx);
		fwk_do_irq_handler(irq);

		/*!< clear status bit */
		mrt_setbitl(mrt_bit(idx), &sprt_reg->ISR);
	}

	return ER_NORMAL;
}

/*!
 * @brief   set the trigger ways of gpio irq
 * @param   sprt_data, type
 * @retval  errno
 * @note    none
 */
static kint32_t imx_gpiochip_driver_irq_set_type(struct fwk_irq_data *sprt_data, kuint32_t type)
{
	struct imx_gpio_port *sprt_port;
	struct fwk_irq_generic *sprt_gc;
	srt_hal_imx_gpio_t *sprt_reg;
	struct fwk_gpio_desc *sprt_desc;
	kuint32_t offset, bitmask = 0;
	kbool_t is_clear = false, active_low;

	offset = sprt_data->hwirq;
	sprt_gc = fwk_irq_get_generic_data(sprt_data);
	sprt_port = sprt_gc->private_data;
	sprt_reg = (srt_hal_imx_gpio_t *)sprt_port->base;

	sprt_desc = fwk_gpiochip_get_desc(&sprt_port->sgrt_gpiochip, offset);
	if (!sprt_desc)
		return -ER_NODEV;

	active_low = fwk_gpio_desc_check_flags(sprt_desc, NR_FWK_GPIODESC_ACTIVE_LOW);

	switch (type)
	{
		case IRQ_TYPE_EDGE_RISING:
			bitmask = active_low ? HAL_IMX_ICR_FALLING_EDGE : HAL_IMX_ICR_RISING_EDGE;
			break;

		case IRQ_TYPE_EDGE_FALLING:
			bitmask = active_low ? HAL_IMX_ICR_RISING_EDGE : HAL_IMX_ICR_FALLING_EDGE;
			break;

		case IRQ_TYPE_EDGE_BOTH:
			bitmask = HAL_IMX_ICR_MASK + 1;
			break;

		case IRQ_TYPE_LEVEL_LOW:
			bitmask = active_low ? HAL_IMX_ICR_HIGH_LEVEL : HAL_IMX_ICR_LOW_LEVEL;
			break;

		case IRQ_TYPE_LEVEL_HIGH:
			bitmask = active_low ? HAL_IMX_ICR_LOW_LEVEL : HAL_IMX_ICR_HIGH_LEVEL;
			break;

		default:
			return -ER_UNVALID;
	}

	mrt_clrbitl(mrt_bit(offset), &sprt_reg->EDGE_SEL);

	if (bitmask > HAL_IMX_ICR_MASK)
	{
		is_clear = true;
		mrt_setbitl(mrt_bit(offset), &sprt_reg->EDGE_SEL);
	}

	if (offset >= 16)
	{
		offset = (offset - 16) << 1;
		mrt_clrbitl(mrt_bit_nr(HAL_IMX_ICR_MASK, offset), &sprt_reg->ICR2);
		
		if (!is_clear)
			mrt_setbitl(mrt_bit_nr(bitmask, offset), &sprt_reg->ICR2);
	}
	else
	{
		offset <<= 1;
		mrt_clrbitl(mrt_bit_nr(HAL_IMX_ICR_MASK, offset), &sprt_reg->ICR1);
		
		if (!is_clear)
			mrt_setbitl(mrt_bit_nr(bitmask, offset), &sprt_reg->ICR1);
	}

	return ER_NORMAL;
}

/*!
 * @brief   hardware irqnumber translate
 * @param   sprt_domain: parent interrupt controller
 * @param   sprt_intc: parent interrupt controller's device-node
 * @param   intspec: of_handle_args::args[]
 * @param   intsize: property "interrupts" cell number
 * @retval  none
 * @note    none
 */
static kint32_t imx_gpiochip_irq_domain_xlate(struct fwk_irq_domain *sprt_domain, struct fwk_device_node *sprt_intc,
				const kuint32_t *intspec, kuint32_t intsize, kuint32_t *out_hwirq, kuint32_t *out_type)
{
	if (sprt_domain->sprt_node != sprt_intc)
		return -ER_UNVALID;
	if (intsize != 2)
		return -ER_UNVALID;
	if (intspec[0] >= 32)
		return -ER_UNVALID;

	/*!< gpio: 0 ~ 31 */
	*out_hwirq = intspec[0];
    *out_type = intspec[1];

	return ER_NORMAL;
}

static const struct fwk_irq_domain_ops sgrt_imx_gpiochip_hierarchy_ops = 
{
	.xlate = imx_gpiochip_irq_domain_xlate,
	.alloc = mrt_nullptr,
	.free = mrt_nullptr,
};

/*!
 * @brief   gpio chip intc probe
 * @param   sprt_pdev
 * @retval  errno
 * @note    none
 */
static kint32_t imx_gpiochip_driver_probe_intc(struct fwk_platdev *sprt_pdev)
{
	struct imx_gpio_port *sprt_port;
	struct fwk_device_node *sprt_node, *sprt_par;
	struct fwk_irq_domain *sprt_intc;
	struct fwk_irq_domain *sprt_domain;
	kint32_t virq, irq_base;
	struct fwk_irq_generic *sprt_gc;
	struct fwk_irq_chip *sprt_chip;
	srt_hal_imx_gpio_t *sprt_reg;

	sprt_node = sprt_pdev->sgrt_dev.sprt_node;
	sprt_port = fwk_platform_get_drvdata(sprt_pdev);
	sprt_reg = (srt_hal_imx_gpio_t *)sprt_port->base;
	sprt_chip = &sgrt_fwk_irq_generic_chip;

	if (!fwk_of_find_property(sprt_node, "interrupt-controller", mrt_nullptr))
		return -ER_NSUPPORT;

	sprt_par = fwk_of_irq_parent(sprt_node);
	if (!isValid(sprt_par))
		return -ER_NOTFOUND;
	
	sprt_intc = fwk_of_irq_host(sprt_par);
	if (!isValid(sprt_intc))
		return -ER_NOTFOUND;

	sprt_domain = fwk_irq_domain_add_hierarchy(sprt_intc, sprt_node, 
									32, &sgrt_imx_gpiochip_hierarchy_ops, mrt_nullptr);
	if (!isValid(sprt_domain))
		return -ER_FAILD;

	irq_base = sprt_port->id << 5;
	virq = fwk_irq_domain_alloc_irqs(sprt_domain, irq_base, 0, 32);
	if (virq < 0)
		goto fail1;

	sprt_port->irq_base = virq;
	sprt_port->sprt_irqdomain = sprt_domain;

	sprt_gc = kzalloc(sizeof(*sprt_gc), GFP_KERNEL);
	if (!isValid(sprt_gc))
		goto fail2;

	sprt_gc->sgrt_chip.irq_mask = sprt_chip->irq_mask;
	sprt_gc->sgrt_chip.irq_unmask = sprt_chip->irq_unmask;
	sprt_gc->sgrt_chip.irq_ack = sprt_chip->irq_ack;
	sprt_gc->sgrt_chip.irq_set_type = imx_gpiochip_driver_irq_set_type;
	sprt_gc->manage_reg = (kuaddr_t)&sprt_reg->IMR;
	sprt_gc->status_reg = (kuaddr_t)&sprt_reg->ISR;
	sprt_gc->private_data = sprt_port;

	fwk_irq_setup_generic_chip(sprt_port->irq_base, 32, sprt_gc, mrt_nullptr);
	sprt_port->sprt_irqgc = sprt_gc;

	return ER_NORMAL;

fail2:
	fwk_irq_domain_free_irqs(sprt_domain);
	sprt_port->sprt_irqdomain = mrt_nullptr;
fail1:
	fwk_irq_domain_del_hierarchy(sprt_domain);
	kfree(sprt_domain);

	return -ER_ERROR;
}

/*!
 * @brief   gpio chip intc remove
 * @param   sprt_pdev
 * @retval  errno
 * @note    none
 */
static void imx_gpiochip_driver_remove_intc(struct fwk_platdev *sprt_pdev)
{
	struct imx_gpio_port *sprt_port;
	struct fwk_irq_domain *sprt_domain;

	sprt_port = fwk_platform_get_drvdata(sprt_pdev);
	sprt_domain = sprt_port->sprt_irqdomain;
	sprt_port->sprt_irqdomain = mrt_nullptr;

	fwk_irq_shutdown_generic_chip(sprt_port->irq_base, 32);
	kfree(sprt_port->sprt_irqgc);
	sprt_port->sprt_irqgc = mrt_nullptr;
	
	fwk_irq_domain_free_irqs(sprt_domain);
	fwk_irq_domain_del_hierarchy(sprt_domain);
	kfree(sprt_domain);
}

/*!
 * @brief   gpio number to irq number
 * @param   sprt_chip, offset(gpio number)
 * @retval  errno
 * @note    none
 */
static kint32_t imx_gpiochip_driver_to_irq(struct fwk_gpio_chip *sprt_chip, kuint32_t offset)
{
	struct imx_gpio_port *sprt_port;
	struct fwk_irq_domain *sprt_domain;

	if (!sprt_chip || (offset >= sprt_chip->ngpios))
		return -ER_NODEV;

	sprt_port = mrt_container_of(sprt_chip, struct imx_gpio_port, sgrt_gpiochip);
	sprt_domain = sprt_port->sprt_irqdomain;

	return fwk_irq_domain_find_map(sprt_domain, offset, 0);
}

/*!
 * @brief   get gpio direction configuration
 * @param   sprt_chip, offset(gpio number)
 * @retval  errno
 * @note    none
 */
static kint32_t imx_gpiochip_driver_get_direction(struct fwk_gpio_chip *sprt_chip, kuint32_t offset)
{
	struct imx_gpio_port *sprt_port;
	srt_hal_imx_gpio_t *sprt_reg;

	if (!sprt_chip || (offset >= sprt_chip->ngpios))
		return -ER_NODEV;

	sprt_port = mrt_container_of(sprt_chip, struct imx_gpio_port, sgrt_gpiochip);
	sprt_reg = (srt_hal_imx_gpio_t *)sprt_port->base;
	if (!sprt_reg)
		return -ER_NODEV;

    return mrt_getbitl(mrt_bit(offset), &sprt_reg->GDIR) ? FWK_GPIO_DIR_OUT : FWK_GPIO_DIR_IN;
}

/*!
 * @brief   set gpio direction to input
 * @param   sprt_chip, offset(gpio number)
 * @retval  errno
 * @note    none
 */
static kint32_t imx_gpiochip_driver_direction_input(struct fwk_gpio_chip *sprt_chip, kuint32_t offset)
{
	struct imx_gpio_port *sprt_port;
	srt_hal_imx_gpio_t *sprt_reg;

	if (!sprt_chip || (offset >= sprt_chip->ngpios))
		return -ER_NODEV;

	sprt_port = mrt_container_of(sprt_chip, struct imx_gpio_port, sgrt_gpiochip);
	sprt_reg = (srt_hal_imx_gpio_t *)sprt_port->base;
	if (!sprt_reg)
		return -ER_NODEV;
	
	mrt_clrbitl(mrt_bit(offset), &sprt_reg->GDIR);

    return ER_NORMAL;
}

/*!
 * @brief   set gpio direction to output
 * @param   sprt_chip, offset(gpio number)
 * @retval  errno
 * @note    none
 */
static kint32_t imx_gpiochip_driver_direction_output(struct fwk_gpio_chip *sprt_chip, kuint32_t offset, kint32_t value)
{
	struct imx_gpio_port *sprt_port;
	srt_hal_imx_gpio_t *sprt_reg;

	if (!sprt_chip || (offset >= sprt_chip->ngpios))
		return -ER_NODEV;

	sprt_port = mrt_container_of(sprt_chip, struct imx_gpio_port, sgrt_gpiochip);
	sprt_reg = (srt_hal_imx_gpio_t *)sprt_port->base;
	if (!sprt_reg)
		return -ER_NODEV;
	
	mrt_setbitl(mrt_bit(offset), &sprt_reg->GDIR);

	/*!< if it is active high, set low level to initialize gpio */
	if (sprt_chip->set)
		sprt_chip->set(sprt_chip, offset, value);

    return ER_NORMAL;
}

/*!
 * @brief   set gpio direction to input
 * @param   sprt_chip, offset(gpio number)
 * @retval  errno
 * @note    none
 */
static kint32_t imx_gpiochip_driver_get(struct fwk_gpio_chip *sprt_chip, kuint32_t offset)
{
	struct imx_gpio_port *sprt_port;
	srt_hal_imx_gpio_t *sprt_reg;

	if (!sprt_chip || (offset >= sprt_chip->ngpios))
		return -ER_NODEV;

	sprt_port = mrt_container_of(sprt_chip, struct imx_gpio_port, sgrt_gpiochip);
	sprt_reg = (srt_hal_imx_gpio_t *)sprt_port->base;
	if (!sprt_reg)
		return -ER_NODEV;

    return !!mrt_getbitl(mrt_bit(offset), &sprt_reg->DR);
}

/*!
 * @brief   set gpio velue
 * @param   sprt_chip, offset(gpio number), value(0 or 1)
 * @retval  none
 * @note    none
 */
static void imx_gpiochip_driver_set(struct fwk_gpio_chip *sprt_chip, kuint32_t offset, kint32_t value)
{
	struct imx_gpio_port *sprt_port;
	srt_hal_imx_gpio_t *sprt_reg;

	if (!sprt_chip || (offset >= sprt_chip->ngpios) || (value < 0))
		return;

	sprt_port = mrt_container_of(sprt_chip, struct imx_gpio_port, sgrt_gpiochip);
	sprt_reg = (srt_hal_imx_gpio_t *)sprt_port->base;
	if (!sprt_reg)
		return;

	if (value)
		mrt_setbitl(mrt_bit(offset), &sprt_reg->DR);
	else
		mrt_clrbitl(mrt_bit(offset), &sprt_reg->DR);
}

/*!
 * @brief   gpio chip probe
 * @param   sprt_pdev
 * @retval  errno
 * @note    none
 */
static kint32_t imx_gpiochip_driver_probe_gc(struct fwk_platdev *sprt_pdev)
{
	struct imx_gpio_port *sprt_port;
	struct fwk_device_node *sprt_node;
	struct fwk_gpio_chip *sprt_gc;
	kint32_t gpio_base;
	kuint32_t count = 0;

	sprt_node = sprt_pdev->sgrt_dev.sprt_node;
	sprt_port = fwk_platform_get_drvdata(sprt_pdev);

	if (!fwk_of_find_property(sprt_node, "gpio-controller", mrt_nullptr))
		return -ER_NSUPPORT;

	if (fwk_of_property_read_u32(sprt_node, "#gpio-cells", &count))
		return -ER_NSUPPORT;

	sprt_gc = &sprt_port->sgrt_gpiochip;
	gpio_base = sprt_port->id << 5;

	if (fwk_gpiochip_init(sprt_gc, & sprt_pdev->sgrt_dev, gpio_base, 32 >> 3))
		return -ER_FAILD;

	sprt_gc->of_gpio_n_cells = count;
	sprt_gc->to_irq = imx_gpiochip_driver_to_irq;
	sprt_gc->get_direction = imx_gpiochip_driver_get_direction;
	sprt_gc->direction_input = imx_gpiochip_driver_direction_input;
	sprt_gc->direction_output = imx_gpiochip_driver_direction_output;
	sprt_gc->get = imx_gpiochip_driver_get;
	sprt_gc->set = imx_gpiochip_driver_set;

	return fwk_gpiochip_add(sprt_gc);
}

/*!< --------------------------------------------------------------------- */
/*!
 * @brief   imx_gpiochip_driver_probe
 * @param   sprt_dev
 * @retval  errno
 * @note    none
 */
static kint32_t imx_gpiochip_driver_probe(struct fwk_platdev *sprt_pdev)
{
	struct imx_gpio_port *sprt_port;
	struct fwk_device_node *sprt_node;
	srt_hal_imx_gpio_t *sprt_reg;

	sprt_node = sprt_pdev->sgrt_dev.sprt_node;

	sprt_port = kzalloc(sizeof(*sprt_port), GFP_KERNEL);
	if (!isValid(sprt_port))
		return -ER_NOMEM;

	/*!< id = 0 ~ 5, e.g. gpio1 ~ gpio6 */
	sprt_port->id = (sprt_pdev->id < 0) ? fwk_of_get_alias_id(sprt_node) : sprt_pdev->id;
	if (sprt_port->id < 0)
		goto fail1;

	/*!< get gpio register base address */
	sprt_port->base = fwk_of_iomap(sprt_node, 0);
	sprt_port->base = fwk_io_remap(sprt_port->base, ARCH_PER_SIZE);
	if (!isValid(sprt_port->base))
		goto fail1;

	/*!< get gpio interrupt number */
	sprt_port->irq_low = fwk_platform_get_irq(sprt_pdev, 0);
	sprt_port->irq_high = fwk_platform_get_irq(sprt_pdev, 1);

	/*!< request irq (the domain is gpc) */
	if (sprt_port->irq_low >= 0)
	{
		if (fwk_request_irq(sprt_port->irq_low, imx_gpiochip_driver_isr, IRQ_TYPE_NONE, "imx-gpio-low", sprt_port))
			goto fail2;
	}
	
	if (sprt_port->irq_high >= 0)
	{
		if (fwk_request_irq(sprt_port->irq_high, imx_gpiochip_driver_isr, IRQ_TYPE_NONE, "imx-gpio-high", sprt_port))
			goto fail3;
	}

	/*!< initial gpio register */
	sprt_reg = (srt_hal_imx_gpio_t *)sprt_port->base;
	mrt_writel(~0, &sprt_reg->ISR);
	mrt_writel(0, &sprt_reg->IMR);

	fwk_platform_set_drvdata(sprt_pdev, sprt_port);

	/*!< gpio is also a irq-controller */
	if (imx_gpiochip_driver_probe_intc(sprt_pdev))
		goto fail4;

	/*!< set to be a gpio-controller */
	if (imx_gpiochip_driver_probe_gc(sprt_pdev))
		goto fail5;

	list_head_add_tail(&sgrt_imx_gpio_ports_list, &sprt_port->sgrt_link);

	return ER_NORMAL;

fail5:
	imx_gpiochip_driver_remove_intc(sprt_pdev);
fail4:
	fwk_platform_set_drvdata(sprt_pdev, mrt_nullptr);
	if (sprt_port->irq_high >= 0)
		fwk_free_irq(sprt_port->irq_high, sprt_port);
fail3:
	if (sprt_port->irq_low >= 0)
		fwk_free_irq(sprt_port->irq_low, sprt_port);
fail2:
	fwk_io_unmap(sprt_port->base);
fail1:
	kfree(sprt_port);
	return -ER_ERROR;
}

/*!
 * @brief   imx_gpiochip_driver_remove
 * @param   sprt_dev
 * @retval  errno
 * @note    none
 */
static kint32_t imx_gpiochip_driver_remove(struct fwk_platdev *sprt_pdev)
{
	struct imx_gpio_port *sprt_port;

	sprt_port = fwk_platform_get_drvdata(sprt_pdev);

	imx_gpiochip_driver_remove_intc(sprt_pdev);
	fwk_platform_set_drvdata(sprt_pdev, mrt_nullptr);

	if (sprt_port->irq_high >= 0)
		fwk_free_irq(sprt_port->irq_high, sprt_port);

	if (sprt_port->irq_low >= 0)
		fwk_free_irq(sprt_port->irq_low, sprt_port);
	
	fwk_io_unmap(sprt_port->base);
	list_head_del(&sprt_port->sgrt_link);
	kfree(sprt_port);

	return ER_NORMAL;
}

/*!< device id for device-tree */
static const struct fwk_of_device_id sgrt_imx_gpiochip_driver_id[] =
{
	{ .compatible = "fsl,imx6ul-gpio", },
	{},
};

/*!< platform instance */
static struct fwk_platdrv sgrt_imx_gpiochip_platdriver =
{
	.probe	= imx_gpiochip_driver_probe,
	.remove	= imx_gpiochip_driver_remove,
	
	.sgrt_driver =
	{
		.name 	= "fsl, imx6ull, gpio-controller",
		.id 	= -1,
		.sprt_of_match_table = sgrt_imx_gpiochip_driver_id,
	},
};

/*!< --------------------------------------------------------------------- */
/*!
 * @brief   imx_gpiochip_driver_init
 * @param   none
 * @retval  errno
 * @note    none
 */
kint32_t __fwk_init imx_gpiochip_driver_init(void)
{
	return fwk_register_platdriver(&sgrt_imx_gpiochip_platdriver);
}

/*!
 * @brief   imx_gpiochip_driver_exit
 * @param   none
 * @retval  none
 * @note    none
 */
void __fwk_exit imx_gpiochip_driver_exit(void)
{
	fwk_unregister_platdriver(&sgrt_imx_gpiochip_platdriver);
}

IMPORT_PATTERN_INIT(imx_gpiochip_driver_init);
IMPORT_PATTERN_EXIT(imx_gpiochip_driver_exit);

/*!< end of file */
