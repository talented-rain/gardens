/*
 * Interrupt Interface Defines
 *
 * File Name:   irq.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.03.23
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <configs/configs.h>
#include <asm/armv7/ca7_gic.h>

#include <platform/fwk_basic.h>
#include <platform/irq/fwk_irq_domain.h>
#include <platform/irq/fwk_irq.h>
#include <platform/irq/fwk_irq_types.h>
#include <platform/irq/fwk_irq_chip.h>
#include <platform/of/fwk_of.h>

/*!< The defines */
#define CA7_MAX_GPC_NR                  (128)

/*!< The functions */
static kint32_t fwk_gic_of_init(struct fwk_device_node *sprt_node, struct fwk_device_node *sprt_parent);
static kint32_t fwk_gpc_of_init(struct fwk_device_node *sprt_node, struct fwk_device_node *sprt_parent);

/*!< The globals */
static kuint32_t g_iHal_gic_cnts = 0;
static srt_ca7_gic_t sgrt_gic_global_data[CA7_MAX_GIC_NR] = {0};

const struct fwk_of_device_id sgrt_fwk_irq_intcs_table[] =
{
    { .compatible = "arm,cortex-a7-gic", .data = fwk_gic_of_init },
    { .compatible = "arm,cortex-a9-gic", .data = fwk_gic_of_init },

    { .compatible = "fsl,imx6ul-gpc", .data = fwk_gpc_of_init },
    { .compatible = mrt_nullptr, .data = mrt_nullptr },
};

static kbool_t g_isIntcGICDirect = true;

/*!< API function */
/*!
 * @brief   initial IRQ
 * @param   none
 * @retval  none
 * @note    none
 */
void __plat_init initIRQ(void)
{
#if CONFIG_OF
    fwk_of_irq_init(sgrt_fwk_irq_intcs_table);

#else
    local_irq_initial(CA7_GIC_NULL);

#endif

    print_info("initial irq finished\n");
}

/*!
 * @brief   return gic data
 * @param   gic_nr
 * @retval  none
 * @note    none
 */
srt_ca7_gic_t *fwk_get_gic_data(kuint32_t gic_nr)
{
    return (gic_nr >= CA7_MAX_GIC_NR) ? mrt_nullptr : &sgrt_gic_global_data[gic_nr];
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
static kint32_t gic_irq_domain_xlate(struct fwk_irq_domain *sprt_domain, struct fwk_device_node *sprt_intc,
				const kuint32_t *intspec, kuint32_t intsize, kuint32_t *out_hwirq, kuint32_t *out_type)
{
	if (sprt_domain->sprt_node != sprt_intc)
		return -ER_UNVALID;
	if (intsize < 3)
		return -ER_UNVALID;

	*out_hwirq = intspec[1];

	/* skip over PPI, which is 0 ~ 15 */
//	*out_hwirq += 16;

	/* GIC_SGI: 1; GIC_SPI: 0 ===> skip over SGI */
	if (!intspec[0])
		*out_hwirq += 16;

    /* interrupt trigger type */
	*out_type = intspec[2] & IRQ_TYPE_SENSE_MASK;

	return ER_NORMAL;
}

/*!
 * @brief   enable IRQ
 * @param   sprt_data
 * @retval  none
 * @note    none
 */
static void gic_irq_chip_enable(struct fwk_irq_data *sprt_data)
{
    srt_ca7_gic_t *sprt_gic = fwk_get_gic_data(0);
    srt_ca7_gic_des_t *sprt_dest;

    if (!sprt_data || (sprt_data->hwirq < 0))
        return;

    sprt_dest = mrt_get_gic_destributor(sprt_gic);
    mrt_setbit_towords(sprt_data->hwirq + sprt_data->sprt_domain->hwirq, 
                    &sprt_dest->D_ISENABLER);
}

/*!
 * @brief   disable IRQ
 * @param   sprt_data
 * @retval  none
 * @note    none
 */
static void gic_irq_chip_disable(struct fwk_irq_data *sprt_data)
{
    srt_ca7_gic_t *sprt_gic = fwk_get_gic_data(0);
    srt_ca7_gic_des_t *sprt_dest;

    if (!sprt_data || (sprt_data->hwirq < 0))
        return;

    sprt_dest = mrt_get_gic_destributor(sprt_gic);
    mrt_setbit_towords(sprt_data->hwirq + sprt_data->sprt_domain->hwirq, 
                    &sprt_dest->D_ICENABLER);
}

/*!
 * @brief   get IRQ status
 * @param   sprt_data
 * @retval  none
 * @note    none
 */
static kbool_t gic_irq_chip_ack(struct fwk_irq_data *sprt_data)
{
    srt_ca7_gic_t *sprt_gic = fwk_get_gic_data(0);
    srt_ca7_gic_cpu_t *sprt_cpu;
    kuint32_t hwirq;

    if (!sprt_data || (sprt_data->hwirq < 0))
        return false;

    sprt_cpu = mrt_get_gic_interface(sprt_gic);
    hwirq = mrt_mask(sprt_cpu->C_IAR, 0x1fffU);

    return !!(hwirq & mrt_bit(sprt_data->hwirq + sprt_data->sprt_domain->hwirq));
}

/*!
 * @brief   check if IRQ is enabled
 * @param   sprt_data
 * @retval  none
 * @note    none
 */
static kbool_t gic_irq_chip_is_enabled(struct fwk_irq_data *sprt_data)
{
    srt_ca7_gic_t *sprt_gic = fwk_get_gic_data(0);
    srt_ca7_gic_des_t *sprt_dest;

    if (!sprt_data || (sprt_data->hwirq < 0))
        return false;

    sprt_dest = mrt_get_gic_destributor(sprt_gic);
    return !!mrt_getbit_fromwords(sprt_data->hwirq + sprt_data->sprt_domain->hwirq, 
                                &sprt_dest->D_ISENABLER);
}

/*!
 * @brief   allocate irq_domain for gic
 * @param   sprt_domain
 * @retval  error code
 * @note    none
 */
kint32_t gic_irq_domain_alloc(struct fwk_irq_domain *sprt_domain, kuint32_t virq, kuint32_t nr_irqs, void *arg)
{
    struct fwk_irq_generic *sprt_gc;

    if (!sprt_domain)
        return -ER_NODEV;

	sprt_gc = kzalloc(sizeof(*sprt_gc), GFP_KERNEL);
	if (!isValid(sprt_gc))
        return -ER_NOMEM;

    sprt_gc->sgrt_chip.irq_enable = gic_irq_chip_enable;
    sprt_gc->sgrt_chip.irq_disable = gic_irq_chip_disable;
    sprt_gc->sgrt_chip.irq_ack = gic_irq_chip_ack;
    sprt_gc->sgrt_chip.irq_is_enabled = gic_irq_chip_is_enabled;

    fwk_irq_setup_generic_chip(virq, nr_irqs, sprt_gc, mrt_nullptr);

    return ER_NORMAL;
}

/*!
 * @brief   free irq_domain of gpc
 * @param   sprt_domain
 * @retval  none
 * @note    none
 */
void gic_irq_domain_free(struct fwk_irq_domain *sprt_domain, kuint32_t virq, kuint32_t nr_irqs)
{
    struct fwk_irq_generic *sprt_gc;
    struct fwk_irq_data *sprt_data;

    sprt_data = fwk_irq_get_data(virq);
    if (!isValid(sprt_data))
        return;
    
    sprt_gc = fwk_irq_get_generic_data(sprt_data);
    fwk_irq_shutdown_generic_chip(virq, nr_irqs);

    kfree(sprt_gc);
}

static const struct fwk_irq_domain_ops sgrt_gic_domain_hierarchy_ops = 
{
	.xlate = gic_irq_domain_xlate,
	.alloc = gic_irq_domain_alloc,
	.free = gic_irq_domain_free,
};

/*!
 * @brief   initial GIC interrupt
 * @param   none
 * @retval  none
 * @note    For simplicity, we only use group0 of GIC
 */
static void fwk_gic_initial(srt_ca7_gic_t *sprt_gic)
{
    srt_ca7_gic_des_t *sprt_dest;
    srt_ca7_gic_cpu_t *sprt_cpu;
    kuint32_t i;
    kuint32_t irqRegs;

    sprt_gic->dest_base = fwk_io_remap(sprt_gic->dest_base);
    if (!isValid(sprt_gic->dest_base))
        return;

    sprt_gic->cpu_base = fwk_io_remap(sprt_gic->cpu_base);
    if (!isValid(sprt_gic->cpu_base))
        return;

    sprt_dest = mrt_get_gic_destributor(sprt_gic);
    sprt_cpu = mrt_get_gic_interface(sprt_gic);

    irqRegs = mrt_mask(sprt_dest->D_TYPER, 0x1fU) + 1;

    if (isValid(sprt_gic))
    {
        /*!< irq number = ((sprt_dest->D_TYPER & 0x1fU) + 1) * 32 */
        sprt_gic->gic_irqs = irqRegs << 5;
        if (sprt_gic->gic_irqs > __CA7_GIC_MAX_IRQS)
            sprt_gic->gic_irqs = __CA7_GIC_MAX_IRQS;
    }

    /*!< On POR, all SPI is in group 0, level-sensitive and using 1-N model */

    /*!< Disable all PPI, SGI and SPI */
    for (i = 0; i < irqRegs; i++)
        mrt_writel(0xffffffffU, &sprt_dest->D_ICENABLER[i]);

    /*!< Make all interrupts have higher priority */
    mrt_writel(mrt_bit_mask(0xffU, 0xffU, 8 - __CA7_GIC_PRIO_BITS), &sprt_cpu->C_PMR);

    /*!< No subpriority, all priority level allows preemption */
    mrt_writel(7 - __CA7_GIC_PRIO_BITS, &sprt_cpu->C_BPR);

    /*!< Enable group0 distribution */
    mrt_writel(1U, &sprt_dest->D_CTLR);

    /*!< Enable group0 signaling */
    mrt_writel(1U, &sprt_cpu->C_CTLR);

    /*!< if program has run to main, do not set VBAR again */
//  __set_cp15_vbar(VECTOR_TABLE_BASE);
}

/*!
 * @brief   initial GIC
 * @param   sprt_node: current interrupt-controller
 * @retval  none
 * @note    none
 */
static void fwk_gic_init_bases(kuint32_t gic_nr, kuint32_t irq_start,
			                void *dest_base, void *cpu_base,
			                kuint32_t percpu_offset, struct fwk_device_node *sprt_node)
{
    srt_ca7_gic_t *sprt_gic;
    struct fwk_irq_domain *sprt_domain;

    sprt_gic = fwk_get_gic_data(gic_nr);
    if (!isValid(sprt_gic))
        return;

    sprt_gic->dest_base = dest_base;
    sprt_gic->cpu_base = cpu_base;

    /*!< Initial GIC */
    fwk_gic_initial(sprt_gic);

    /*!< sprt_gic->gic_irqs will be get on local_irq_initial */
    if (!sprt_gic->gic_irqs)
        print_err("Get IRQ Controller Number failed!\n");

    if (!isValid(sprt_node))
        return;
    
    sprt_domain = fwk_irq_domain_add_hierarchy(mrt_nullptr, sprt_node, 
                            sprt_gic->gic_irqs, &sgrt_gic_domain_hierarchy_ops, sprt_gic);
    sprt_domain->hwirq = 16;
    sprt_gic->sprt_domain = sprt_domain;
}

/*!
 * @brief   initial GIC
 * @param   sprt_node: current interrupt-controller
 * @param   sprt_parent: interrupt-controller parent
 * @retval  none
 * @note    none
 */
kint32_t fwk_gic_of_init(struct fwk_device_node *sprt_node, struct fwk_device_node *sprt_parent)
{
    kuint32_t destributor = 0, cpu_interface = 0;

    if (isValid(sprt_parent))
        return -ER_UNVALID;

    fwk_of_property_read_u32_index(sprt_node, "reg", 0, &destributor);
    fwk_of_property_read_u32_index(sprt_node, "reg", 2, &cpu_interface);

    if ((!destributor) || (!cpu_interface))
        return -ER_NOTFOUND;

    fwk_gic_init_bases(g_iHal_gic_cnts, -1, (void *)destributor, (void *)cpu_interface, 0, sprt_node);
    g_iHal_gic_cnts++;

    return ER_NORMAL;
}

/*!
 * @brief   convert GIC to GPC
 * @param   hwirq
 * @retval  none
 * @note    none
 */
kint32_t fwk_gic_to_actual_irq(kint32_t hwirq)
{
    kchar_t *intc_name = "interrupt-controller";
    kuint32_t irq_base = 16;

    if (!g_isIntcGICDirect)
    {
        irq_base = 32;
        intc_name = "gpc";
    }

    hwirq -= irq_base;
    if (hwirq < 0)
        return hwirq;

    return fwk_irq_get_by_domain_name(intc_name, hwirq);
}

/*!
 * @brief   convert GPC to GIC
 * @param   virq
 * @retval  none
 * @note    none
 */
kint32_t fwk_gpc_to_gic_irq(kint32_t virq)
{
    struct fwk_irq_data *sprt_data;

    sprt_data = fwk_irq_get_data(virq);
    return isValid(sprt_data) ? (sprt_data->hwirq + 32) : -1;
}

/*!< --------------------------------------------------------------------------- */
/*!
 * @brief   hardware irqnumber translate
 * @param   sprt_domain: parent interrupt controller
 * @param   sprt_intc: parent interrupt controller's device-node
 * @param   intspec: of_handle_args::args[]
 * @param   intsize: property "interrupts" cell number
 * @retval  none
 * @note    none
 */
static kint32_t gpc_irq_domain_xlate(struct fwk_irq_domain *sprt_domain, struct fwk_device_node *sprt_intc,
				const kuint32_t *intspec, kuint32_t intsize, kuint32_t *out_hwirq, kuint32_t *out_type)
{
	if (sprt_domain->sprt_node != sprt_intc)
		return -ER_UNVALID;
	if (intsize != 3)
		return -ER_UNVALID;

    /*!< not allow GIC_SGI */
    if (intspec[0] != 0)
        return -ER_UNVALID;

	*out_hwirq = intspec[1];
    *out_type = intspec[2];

	return ER_NORMAL;
}

static const struct fwk_irq_domain_ops sgrt_gpc_domain_hierarchy_ops = 
{
	.xlate = gpc_irq_domain_xlate,
	.alloc = gic_irq_domain_alloc,
	.free = gic_irq_domain_free,
};

/*!
 * @brief   initial GPC
 * @param   sprt_node: current interrupt-controller
 * @param   sprt_parent: interrupt-controller parent
 * @retval  none
 * @note    none
 */
kint32_t fwk_gpc_of_init(struct fwk_device_node *sprt_node, struct fwk_device_node *sprt_parent)
{
    struct fwk_irq_domain *sprt_domain, *sprt_par;

    if (!isValid(sprt_parent) || !isValid(sprt_node))
        return -ER_UNVALID;

    /*! get gic */
    sprt_par = fwk_of_irq_host(sprt_parent);
    if (!isValid(sprt_par))
        return -ER_UNVALID;

    sprt_domain = fwk_irq_domain_add_hierarchy(sprt_par, sprt_node, CA7_MAX_GPC_NR, &sgrt_gpc_domain_hierarchy_ops, mrt_nullptr);
    if (!isValid(sprt_domain))
        return -ER_FAILD;

    sprt_domain->hwirq = 32;
    g_isIntcGICDirect = false;

    return ER_NORMAL;
}

/* end of file */
