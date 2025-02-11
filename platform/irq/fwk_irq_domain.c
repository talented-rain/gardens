/*
 * Interrupt Interface Defines
 *
 * File Name:   fwk_irq_domain.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.01.07
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <platform/irq/fwk_irq_domain.h>
#include <platform/irq/fwk_irq_types.h>
#include <platform/of/fwk_of.h>
#include <kernel/rw_lock.h>

/*!< The globals */
static DECLARE_LIST_HEAD(sgrt_fwk_irq_domain);
static struct rw_lock sgrt_irq_domain_lock = RW_LOCK_INIT();

/*!< API function */
/*!
 * @brief   get irq domain
 * @param   sprt_node
 * @retval  sprt_domain
 * @note    none
 */
struct fwk_irq_domain *fwk_of_irq_host(struct fwk_device_node *sprt_node)
{
	struct fwk_irq_domain *sprt_domain, *found = mrt_nullptr;

	rd_lock(&sgrt_irq_domain_lock);

	foreach_list_next_entry(sprt_domain, &sgrt_fwk_irq_domain, sgrt_link)
	{
		if (sprt_node == sprt_domain->sprt_node)
		{
			found = sprt_domain;
			break;
		}
	}

	rd_unlock(&sgrt_irq_domain_lock);
	return found;
}

/*!
 * @brief   Get the irq value
 * @param   none
 * @retval  none
 * @note    none
 */
kint32_t fwk_of_irq_parse_one(struct fwk_device_node *sprt_node, kuint32_t index, struct fwk_of_phandle_args *sprt_irq)
{
	struct fwk_device_node *sprt_parent;
	kuint32_t i, cells;
	kuint32_t lenth, *ptr_value;
	kint32_t retval;

	if ((!isValid(sprt_node)) || (!sprt_irq))
		return -ER_NULLPTR;

	ptr_value = (kuint32_t *)fwk_of_get_property(sprt_node, "interrupts", &lenth);
	lenth /= sizeof(kuint32_t);

	/* how many value per group */
	cells = fwk_of_n_irq_cells(sprt_node);
	if ((!cells) || (cells >= FWK_OF_MAX_PHANDLE_ARGS))
		return -ER_FAULT;

	if ((!ptr_value) || (((index + 1) * cells) > lenth))
		return -ER_FAULT;

	for (i = 0; i < cells; i++)
	{
		retval = fwk_of_property_read_u32_index(sprt_node, "interrupts", (index * cells) + i, &sprt_irq->args[i]);
		if (retval < 0)
			return -ER_FAULT;
	}

	/* for intc, parent == null; but it should not to be translated */
	sprt_parent = fwk_of_irq_parent(sprt_node);
	if (!isValid(sprt_parent))
		return -ER_FAULT;

	sprt_irq->args_count = i;
	sprt_irq->sprt_node = sprt_parent;

	return ER_NORMAL;
}

/*!
 * @brief   find irq number
 * @param   none
 * @retval  none
 * @note    none
 */
kint32_t fwk_irq_find_mapping(struct fwk_irq_domain *sprt_domain, kuint32_t type, kint32_t hwirq)
{
	return fwk_irq_domain_find_map(sprt_domain, hwirq, type);
}

/*!
 * @brief   Get the virtual-irq number (soft irq number)
 * @param   none
 * @retval  none
 * @note    none
 */
kint32_t fwk_irq_create_of_mapping(struct fwk_of_phandle_args *sprt_irq)
{
	struct fwk_irq_domain *sprt_domain;
	kint32_t virq;
	kuint32_t hwirq, type;
	kint32_t retval;

	if ((!sprt_irq) || !isValid(sprt_irq->sprt_node))
		return -ER_NOMEM;
	
	sprt_domain = fwk_of_irq_host(sprt_irq->sprt_node);
	if (!isValid(sprt_domain) || (!sprt_domain->sprt_ops->xlate))
		return -ER_NOTFOUND;

	/* get hwirq/type form sprt_irq->args[] */
	retval = sprt_domain->sprt_ops->xlate(sprt_domain, sprt_irq->sprt_node, 
										sprt_irq->args, sprt_irq->args_count, &hwirq, &type);
	if (retval < 0)
		return retval;

	virq = fwk_irq_find_mapping(sprt_domain, type, hwirq);
	if (virq >= 0)
		return virq;

	/* map one virtual irq number */
	virq = fwk_irq_domain_alloc_irqs(sprt_domain, -1, hwirq, 1);
	if (virq < 0)
		return virq;

	if (sprt_domain->sprt_ops->alloc)
		sprt_domain->sprt_ops->alloc(sprt_domain, virq, 1, mrt_nullptr);
	
	fwk_irq_desc_set_type(virq, type);

	return virq;
}

/*!
 * @brief   Get the irq value
 * @param   none
 * @retval  none
 * @note    none
 */
kint32_t fwk_irq_of_parse_and_map(struct fwk_device_node *sprt_node, kuint32_t index)
{
	struct fwk_of_phandle_args sgrt_old;
	kint32_t retval;

	retval = fwk_of_irq_parse_one(sprt_node, index, &sgrt_old);
	if (retval < 0)
		return -ER_FAILD;

	return fwk_irq_create_of_mapping(&sgrt_old);
}

/*!
 * @brief   Add new IRQ domain
 * @param   none
 * @retval  none
 * @note    none
 */
struct fwk_irq_domain *fwk_irq_domain_add_linear(struct fwk_device_node *sprt_node, kuint32_t size,
					 							const struct fwk_irq_domain_ops *ops, void *host_data)
{
	struct fwk_irq_domain *sprt_domain;

	/*!< extra size, with hard interrupt number as index and soft interrupt number as value */
	sprt_domain = (struct fwk_irq_domain *)kzalloc(sizeof(struct fwk_irq_domain) + sizeof(kint32_t) * size, GFP_KERNEL);
	if (!isValid(sprt_domain))
		return mrt_nullptr;

	sprt_domain->sprt_ops = ops;
	sprt_domain->sprt_node = sprt_node;
	sprt_domain->host_data = host_data;
	sprt_domain->hwirq_max = size;

	for (kuint32_t i = 0; i < size; i++)
		sprt_domain->revmap[i] = -1;

	wr_lock(&sgrt_irq_domain_lock);
	list_head_add_tail(&sgrt_fwk_irq_domain, &sprt_domain->sgrt_link);
	wr_unlock(&sgrt_irq_domain_lock);

	return sprt_domain;
}

/*!
 * @brief   Add new IRQ domain
 * @param   none
 * @retval  none
 * @note    none
 */
struct fwk_irq_domain *fwk_irq_domain_add_hierarchy(struct fwk_irq_domain *sprt_parent, struct fwk_device_node *sprt_node, 
												kuint32_t size, const struct fwk_irq_domain_ops *ops, void *host_data)
{
	struct fwk_irq_domain *sprt_domain;

	sprt_domain = fwk_irq_domain_add_linear(sprt_node, size, ops, host_data);
	if (isValid(sprt_domain))
	{
		sprt_domain->sprt_parent = sprt_parent;
		sprt_domain->name = sprt_node->name;
	}

	return sprt_domain;
}

/*!
 * @brief   delete sprt_domain
 * @param   sprt_domain
 * @retval  none
 * @note    none
 */
void fwk_irq_domain_del_hierarchy(struct fwk_irq_domain *sprt_domain)
{
	if (!sprt_domain)
		return;

	fwk_irq_domain_free_irqs(sprt_domain);

	wr_lock(&sgrt_irq_domain_lock);
	list_head_del(&sprt_domain->sgrt_link);
	wr_unlock(&sgrt_irq_domain_lock);
}

/*!
 * @brief   find sprt_domain by name
 * @param   name, hwirq
 * @retval  sprt_domain
 * @note    none
 */
struct fwk_irq_domain *fwk_irq_get_domain_by_name(kchar_t *name, kint32_t hwirq)
{
	struct fwk_irq_domain *sprt_domain;
	kint32_t retval;

	rd_lock(&sgrt_irq_domain_lock);

	foreach_list_next_entry(sprt_domain, &sgrt_fwk_irq_domain, sgrt_link)
	{
		retval = sprt_domain->name && (!strcmp(sprt_domain->name, name));
		if (retval && (hwirq < sprt_domain->hwirq_max))
		{
			rd_unlock(&sgrt_irq_domain_lock);
			return sprt_domain;
		}
	}

	rd_unlock(&sgrt_irq_domain_lock);
	return mrt_nullptr;
}

/*!
 * @brief   get virtual irq by hwirq
 * @param   sprt_domain, hwirq
 * @retval  virtual irq number
 * @note    none
 */
kint32_t fwk_irq_get_by_domain(struct fwk_irq_domain *sprt_domain, kint32_t hwirq)
{
	return sprt_domain ? sprt_domain->revmap[hwirq] : -1;
}

/*!
 * @brief   get sprt_domain by name
 * @param   name, hwirq
 * @retval  sprt_domain
 * @note    none
 */
kint32_t fwk_irq_get_by_domain_name(kchar_t *name, kint32_t hwirq)
{
	struct fwk_irq_domain *sprt_domain;

	sprt_domain = fwk_irq_get_domain_by_name(name, hwirq);
	return fwk_irq_get_by_domain(sprt_domain, hwirq);
}

/* end of file */
