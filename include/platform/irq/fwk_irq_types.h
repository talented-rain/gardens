/*
 * Interrupt Interface Type Defines
 *
 * File Name:   fwk_irq_types.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.11.14
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_IRQ_TYPES_H
#define __FWK_IRQ_TYPES_H

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The includes */
#include <common/generic.h>
#include <common/list_types.h>

#include "fwk_irq_domain.h"
#include "fwk_irq_chip.h"

/*!< The defines */
#define	SWI_EVENT_SCHEDULED						0x00000001
#define	SWI_EVENT_SYSCALL						0x00000002

#define	_SWI_EVENT_SCHEDULED					" swi 0x00000001 "
#define	_SWI_EVENT_SYSCALL						" swi 0x00000002 "

#define mrt_rasie_schedule_event()	\
	do {	\
		__asm__ __volatile__ (	\
			_SWI_EVENT_SCHEDULED : : );	\
	} while (0)

#define FWK_IRQ_DESC_NAME_LENTH					(16)

typedef	kint32_t irq_return_t;
typedef kint32_t (*irq_handler_t)(void *ptrDev);

#define IRQ_TYPE_NONE							0x00000000
#define IRQ_TYPE_EDGE_RISING					0x00000001
#define IRQ_TYPE_EDGE_FALLING					0x00000002
#define IRQ_TYPE_EDGE_BOTH						(IRQ_TYPE_EDGE_FALLING | IRQ_TYPE_EDGE_RISING)
#define IRQ_TYPE_LEVEL_HIGH						0x00000004
#define IRQ_TYPE_LEVEL_LOW						0x00000008
#define IRQ_TYPE_LEVEL_MASK						(IRQ_TYPE_LEVEL_LOW | IRQ_TYPE_LEVEL_HIGH)
#define IRQ_TYPE_SENSE_MASK						(IRQ_TYPE_EDGE_BOTH | IRQ_TYPE_LEVEL_MASK)

typedef struct fwk_irq_data
{
	unsigned int irq;
	unsigned long hwirq;
	unsigned int node;
	struct fwk_irq_chip *sprt_chip;
	struct fwk_irq_domain *sprt_domain;

	struct fwk_irq_data *sprt_parent;
	void *handler_data;
	void *chip_data;

	kuint32_t mask;

} srt_fwk_irq_data_t;

typedef struct fwk_irq_action
{
	kchar_t name[FWK_IRQ_DESC_NAME_LENTH];
	irq_handler_t handler;
	kuint32_t flags;
	void *ptrArgs;

	struct list_head sgrt_link;

} srt_fwk_irq_action_t;

typedef struct fwk_irq_desc
{
	kint32_t irq;

	kuint32_t flags;
	kchar_t irq_name[FWK_IRQ_DESC_NAME_LENTH];
	struct list_head sgrt_action;;

	struct list_head sgrt_link;
	struct radix_link sgrt_radix;
	
	struct fwk_irq_data sgrt_data;

} srt_fwk_irq_desc_t;

/*!< The functions */
extern struct fwk_irq_desc *fwk_irq_to_desc(kuint32_t virq);
extern struct fwk_irq_desc *fwk_irq_data_to_desc(struct fwk_irq_data *sprt_data);
extern struct fwk_irq_data *fwk_irq_get_data(kuint32_t virq);
extern struct fwk_irq_data *fwk_irq_domain_get_data(struct fwk_irq_domain *sprt_domain, kuint32_t hwirq);
extern void fwk_irq_desc_set_type(kuint32_t virq, kuint32_t type);
extern kint32_t fwk_irq_domain_find_map(struct fwk_irq_domain *sprt_domain, kuint32_t hwirq, kuint32_t type);
extern kint32_t fwk_irq_domain_alloc_irqs(struct fwk_irq_domain *sprt_domain, kint32_t irq_base, kuint32_t hwirq, kuint32_t nr_irqs);
extern void fwk_irq_desc_free(kint32_t irq);
extern void fwk_irq_domain_free_irqs(struct fwk_irq_domain *sprt_domain);
extern kint32_t fwk_of_irq_get(struct fwk_device_node *sprt_node, kuint32_t index);

extern void *fwk_find_irq_action(kint32_t irq, const kchar_t *name, void *ptrDev);
extern kint32_t fwk_request_irq(kint32_t irq, irq_handler_t handler, kuint32_t flags, const kchar_t *name, void *ptrDev);
extern void fwk_free_irq(kint32_t irq, void *ptrDev);
extern void fwk_destroy_irq_action(kint32_t irq);
extern void fwk_do_irq_handler(kint32_t softIrq);
extern void fwk_handle_softirq(kint32_t softIrq, kuint32_t event);

#ifdef __cplusplus
	}
#endif

#endif /* __FWK_IRQ_TYPES_H */
