/*
 * Interrupt Interface Defines
 *
 * File Name:   fwk_irq_desc.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.03.23
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/of/fwk_of.h>
#include <platform/irq/fwk_irq_domain.h>
#include <platform/irq/fwk_irq_types.h>
#include <platform/irq/fwk_irq_chip.h>

/*!< The defines */
#define FWK_IRQ_DESC_RADIXTREE				(1)
#define FWK_IRQ_DESC_MAX					(1024)

/*!< The globals */
static DECLARE_RADIX_TREE(sgrt_fwk_irq_radix_tree, default_malloc, kfree);
static kuint32_t g_fwk_allocated_irqs[mrt_num_align(FWK_IRQ_DESC_MAX, RET_BITS_PER_INT) / RET_BITS_PER_INT] = { 0 };

/*!< API functions */
/*!
 * @brief   allocate irq_desc
 * @param   gfp
 * @retval  none
 * @note    none
 */
static struct fwk_irq_desc *fwk_allocate_irq_desc(ert_fwk_mempool_t gfp)
{
	struct fwk_irq_desc *sprt_desc;

	sprt_desc = (struct fwk_irq_desc *)kzalloc(sizeof(*sprt_desc), gfp);
	if (!isValid(sprt_desc))
		return mrt_nullptr;

	init_list_head(&sprt_desc->sgrt_action);
		
	return sprt_desc;
}

/*!
 * @brief   find irq number that is not used from bitmap
 * @param   bitmap, irq_base, total_bits, nr_irqs
 * @retval  irq number base
 * @note    none
 */
static kint32_t fwk_irq_bitmap_find_areas(kuint32_t *bitmap, kint32_t irq_base, kuint32_t total_bits, kuint32_t nr_irqs)
{
	kuint32_t index = 0, temp = 0;

	index = (irq_base >= 0) ? irq_base : 0;

	do 
	{
		if ((index + nr_irqs) >= total_bits)
			return -ER_MORE;

		/*!< get the first non-zero bit  */
		index = bitmap_find_first_zero_bit(bitmap, index, total_bits);
		if (index < 0)
			return -ER_NOTFOUND;

		/*!< base on index, get nr_irqs consecutive non-zero bits; if success, return 0; otherwise, return index that causing failed */
		temp = bitmap_find_nr_zero_bit(bitmap, index, total_bits, nr_irqs);
		if (temp)
		{
			index = temp + 1;
			continue;
		}

	} while (temp);

	return index;
}

/*!
 * @brief   allocate and initialize irq_data
 * @param   sprt_domain, virq, hwirq, nr_irqs
 * @retval  virtual irq number or errno
 * @note    none
 */
static kint32_t irq_domain_alloc_irq_data(struct fwk_irq_domain *sprt_domain, kuint32_t virq, kuint32_t hwirq, kuint32_t nr_irqs)
{
	struct fwk_irq_data *sprt_data;

	for (kuint32_t i = 0; i < nr_irqs; i++)
	{
		sprt_data = fwk_irq_get_data(virq + i);

		if (!isValid(sprt_data))
			return -ER_NOTFOUND;

		sprt_data->sprt_domain = sprt_domain;
		sprt_data->irq = virq + i;
		sprt_data->hwirq = hwirq + i;
		sprt_data->sprt_chip = &sgrt_fwk_irq_dummy_chip;
		sprt_data->mask = 0;
		sprt_domain->revmap[hwirq + i] = virq + i;
	}

	return virq;
}

/*!
 * @brief   Get the irq value
 * @param   none
 * @retval  none
 * @note    none
 */
static kint32_t fwk_irq_domain_alloc_descs(kint32_t irq_base, kuint32_t nr_irqs)
{
	struct fwk_irq_desc *sprt_desc;
	kuint32_t virq;

	virq = fwk_irq_bitmap_find_areas(g_fwk_allocated_irqs, irq_base, FWK_IRQ_DESC_MAX, nr_irqs);
	if (virq < 0)
		return virq;

	/*!< create desc for every irq */
	for (kuint32_t i = 0; i < nr_irqs; i++)
	{
		sprt_desc = fwk_allocate_irq_desc(GFP_KERNEL);
		if (!isValid(sprt_desc))
			return -ER_NOMEM;

		sprt_desc->irq = virq + i;
		radix_tree_add(&sgrt_fwk_irq_radix_tree, virq + i, &sprt_desc->sgrt_radix);
	}

	bitmap_set_nr_bit_valid(g_fwk_allocated_irqs, virq, FWK_IRQ_DESC_MAX, nr_irqs);

	return virq;
}

/*!< ----------------------------------------------------------- */
/*!
 * @brief   irq ---> irq_desc
 * @param   irq
 * @retval  irq_desc
 * @note    none
 */
struct fwk_irq_desc *fwk_irq_to_desc(kuint32_t virq)
{
	if (virq < 0)
		return mrt_nullptr;
	
	return radix_tree_next_entry(&sgrt_fwk_irq_radix_tree, struct fwk_irq_desc, sgrt_radix, virq);
}

/*!
 * @brief   irq_data ---> irq_desc
 * @param   irq_data
 * @retval  irq_desc
 * @note    none
 */
struct fwk_irq_desc *fwk_irq_data_to_desc(struct fwk_irq_data *sprt_data)
{
	return sprt_data ? mrt_container_of(sprt_data, struct fwk_irq_desc, sgrt_data) : mrt_nullptr;
}

/*!
 * @brief   irq ---> irq_data
 * @param   irq
 * @retval  irq_data
 * @note    none
 */
struct fwk_irq_data *fwk_irq_get_data(kuint32_t virq)
{
	struct fwk_irq_desc *sprt_desc = fwk_irq_to_desc(virq);
	return (sprt_desc) ? &sprt_desc->sgrt_data : mrt_nullptr;
}

/*!
 * @brief   get irq_data by hard irq
 * @param   sprt_domain, hwirq
 * @retval  irq_data
 * @note    none
 */
struct fwk_irq_data *fwk_irq_domain_get_data(struct fwk_irq_domain *sprt_domain, kuint32_t hwirq)
{
	kint32_t virq;
	struct fwk_irq_data *sprt_data;

	if (!isValid(sprt_domain))
		goto fail;

	virq = sprt_domain->revmap[hwirq];
	if (virq < 0)
		goto fail;

	sprt_data = fwk_irq_get_data(virq);
	if (isValid(sprt_data))
		return ((sprt_data->hwirq == hwirq) && (sprt_data->sprt_domain == sprt_domain)) ? sprt_data : mrt_nullptr;

fail:
	return mrt_nullptr;
}

/*!
 * @brief   get irq_data by hard irq
 * @param   sprt_domain, hwirq
 * @retval  irq_data
 * @note    none
 */
kint32_t fwk_irq_domain_find_map(struct fwk_irq_domain *sprt_domain, kuint32_t hwirq, kuint32_t type)
{
	struct fwk_irq_desc *sprt_desc;
	struct fwk_irq_data *sprt_data;

	sprt_data = fwk_irq_domain_get_data(sprt_domain, hwirq);
	if (!isValid(sprt_data))
		return -ER_UNVALID;

	if (!type)
		goto END;

	sprt_desc = fwk_irq_data_to_desc(sprt_data);
	type &= IRQ_TYPE_SENSE_MASK;
	
	if (!(sprt_desc->flags & type))
		sprt_desc->flags |= type;

END:
	return sprt_data->irq;
}

/*!
 * @brief   set irq type
 * @param   virq, type
 * @retval  none
 * @note    none
 */
void fwk_irq_desc_set_type(kuint32_t virq, kuint32_t type)
{
	struct fwk_irq_desc *sprt_desc = fwk_irq_to_desc(virq);

	if (!isValid(sprt_desc))
		return;

	sprt_desc->flags |= type;
}

/*!
 * @brief   allocate multiple irq numbers
 * @param   sprt_domain, irq_base, hwirq, nr_irqs
 * @retval  irq base allocated
 * @note    none
 */
kint32_t fwk_irq_domain_alloc_irqs(struct fwk_irq_domain *sprt_domain, kint32_t irq_base, kuint32_t hwirq, kuint32_t nr_irqs)
{
	kint32_t virq;

	virq = fwk_irq_domain_alloc_descs(irq_base, nr_irqs);
	if (virq < 0)
		return virq;
	
	return irq_domain_alloc_irq_data(sprt_domain, virq, hwirq, nr_irqs);
}

/*!
 * @brief   release irq
 * @param   irq
 * @retval  none
 * @note    none
 */
void fwk_irq_desc_free(kint32_t irq)
{
	struct fwk_irq_desc *sprt_desc;
	struct fwk_irq_data *sprt_data;

	sprt_desc = fwk_irq_to_desc(irq);
	sprt_data = &sprt_desc->sgrt_data;

	bitmap_set_nr_bit_zero(g_fwk_allocated_irqs, sprt_data->irq, FWK_IRQ_DESC_MAX, 1);
	fwk_destroy_irq_action(sprt_data->irq);
	radix_tree_del(&sgrt_fwk_irq_radix_tree, sprt_data->irq);

	kfree(sprt_desc);
}

/*!
 * @brief   release multiple irq numbers
 * @param   sprt_domain
 * @retval  none
 * @note    none
 */
void fwk_irq_domain_free_irqs(struct fwk_irq_domain *sprt_domain)
{
	kuint32_t idx;

	for (idx = 0; idx < sprt_domain->hwirq_max; idx++)
	{
		if (sprt_domain->revmap[idx] < 0)
			continue;

		fwk_irq_desc_free(sprt_domain->revmap[idx]);
		sprt_domain->revmap[idx] = -1;
	}
}

/*!
 * @brief   parse and map irq
 * @param   sprt_node, index
 * @retval  irq number (hwirq ---> virtual irq)
 * @note    none
 */
kint32_t fwk_of_irq_get(struct fwk_device_node *sprt_node, kuint32_t index)
{
	return fwk_irq_of_parse_and_map(sprt_node, index);
}


/* end of file */
