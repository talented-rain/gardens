/*
 * Assembly Header Defines For ARM Cortex-A7
 *
 * File Name:   gic_basic.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.11.15
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __GIC_BASIC_H
#define __GIC_BASIC_H

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The includes */
#include <common/generic.h>
#include <common/io_stream.h>
#include <boot/boot_text.h>
#include "asm_config.h"
#include "gcc_config.h"

/*!< The defines */
#define GIC_SPI                                     0
#define GIC_PPI                                     1

typedef struct gic_common
{
    void *dest_base;
    void *cpu_base;

    kuint32_t gic_irqs;
    void *sprt_domain;

} srt_gic_t;

#define GIC_MAX_NR                                  (1)
#define GIC_NULL                                    ((srt_gic_t *)0)

/*!< Number of Bits used for Priority Levels */
#define __GIC_PRIO_BITS                             5   
#define __GIC_MAX_IRQS                              1020

typedef struct gic_dist
{
    /*!< distributor */
//  kuint32_t RESERVED0[1024];

#define GIC_DIST_HEAD_OFFSET                        (1024 << 2)

    kuint32_t D_CTLR;                               /*!< Offset: 0x1000 (R/W) Distributor Control Register */
    kuint32_t D_TYPER;                              /*!< Offset: 0x1004 (R/ )  Interrupt Controller Type Register */
    kuint32_t D_IIDR;                               /*!< Offset: 0x1008 (R/ )  Distributor Implementer Identification Register */
    kuint32_t RESERVED1[29];
    kuint32_t D_IGROUPR[16];                        /*!< Offset: 0x1080 - 0x0BC (R/W) Interrupt Group Registers */
    kuint32_t RESERVED2[16];
    kuint32_t D_ISENABLER[16];                      /*!< Offset: 0x1100 - 0x13C (R/W) Interrupt Set-Enable Registers */
    kuint32_t RESERVED3[16];
    kuint32_t D_ICENABLER[16];                      /*!< Offset: 0x1180 - 0x1BC (R/W) Interrupt Clear-Enable Registers */
    kuint32_t RESERVED4[16];
    kuint32_t D_ISPENDR[16];                        /*!< Offset: 0x1200 - 0x23C (R/W) Interrupt Set-Pending Registers */
    kuint32_t RESERVED5[16];
    kuint32_t D_ICPENDR[16];                        /*!< Offset: 0x1280 - 0x2BC (R/W) Interrupt Clear-Pending Registers */
    kuint32_t RESERVED6[16];
    kuint32_t D_ISACTIVER[16];                      /*!< Offset: 0x1300 - 0x33C (R/W) Interrupt Set-Active Registers */
    kuint32_t RESERVED7[16];
    kuint32_t D_ICACTIVER[16];                      /*!< Offset: 0x1380 - 0x3BC (R/W) Interrupt Clear-Active Registers */
    kuint32_t RESERVED8[16];
    kuint8_t  D_IPRIORITYR[512];                    /*!< Offset: 0x1400 - 0x5FC (R/W) Interrupt Priority Registers */
    kuint32_t RESERVED9[128];
    kuint8_t  D_ITARGETSR[512];                     /*!< Offset: 0x1800 - 0x9FC (R/W) Interrupt Targets Registers */
    kuint32_t RESERVED10[128];
    kuint32_t D_ICFGR[32];                          /*!< Offset: 0x1C00 - 0xC7C (R/W) Interrupt configuration registers */
    kuint32_t RESERVED11[32];
    kuint32_t D_PPISR;                              /*!< Offset: 0x1D00 (R/ ) Private Peripheral Interrupt Status Register */
    kuint32_t D_SPISR[15];                          /*!< Offset: 0x1D04 - 0xD3C (R/ ) Shared Peripheral Interrupt Status Registers */
    kuint32_t RESERVED12[112];
    kuint32_t D_SGIR;                               /*!< Offset: 0x1F00 ( /W) Software Generated Interrupt Register */
    kuint32_t RESERVED13[3];
    kuint8_t  D_CPENDSGIR[16];                      /*!< Offset: 0x1F10 - 0xF1C (R/W) SGI Clear-Pending Registers */
    kuint8_t  D_SPENDSGIR[16];                      /*!< Offset: 0x1F20 - 0xF2C (R/W) SGI Set-Pending Registers */
    kuint32_t RESERVED14[40];
    kuint32_t D_PIDR4;                              /*!< Offset: 0x1FD0 (R/ ) Peripheral ID4 Register */
    kuint32_t D_PIDR5;                              /*!< Offset: 0x1FD4 (R/ ) Peripheral ID5 Register */
    kuint32_t D_PIDR6;                              /*!< Offset: 0x1FD8 (R/ ) Peripheral ID6 Register */
    kuint32_t D_PIDR7;                              /*!< Offset: 0x1FDC (R/ ) Peripheral ID7 Register */
    kuint32_t D_PIDR0;                              /*!< Offset: 0x1FE0 (R/ ) Peripheral ID0 Register */
    kuint32_t D_PIDR1;                              /*!< Offset: 0x1FE4 (R/ ) Peripheral ID1 Register */
    kuint32_t D_PIDR2;                              /*!< Offset: 0x1FE8 (R/ ) Peripheral ID2 Register */
    kuint32_t D_PIDR3;                              /*!< Offset: 0x1FEC (R/ ) Peripheral ID3 Register */
    kuint32_t D_CIDR0;                              /*!< Offset: 0x1FF0 (R/ ) Component ID0 Register */
    kuint32_t D_CIDR1;                              /*!< Offset: 0x1FF4 (R/ ) Component ID1 Register */
    kuint32_t D_CIDR2;                              /*!< Offset: 0x1FF8 (R/ ) Component ID2 Register */
    kuint32_t D_CIDR3;                              /*!< Offset: 0x1FFC (R/ ) Component ID3 Register */

} srt_gic_dist_t;

typedef struct gic_cpu
{
    /*!< cpu interface */
    kuint32_t C_CTLR;                               /*!< Offset: 0x2000 (R/W) CPU Interface Control Register */
    kuint32_t C_PMR;                                /*!< Offset: 0x2004 (R/W) Interrupt Priority Mask Register */
    kuint32_t C_BPR;                                /*!< Offset: 0x2008 (R/W) Binary Point Register */
    kuint32_t C_IAR;                                /*!< Offset: 0x200C (R/ ) Interrupt Acknowledge Register */
    kuint32_t C_EOIR;                               /*!< Offset: 0x2010 ( /W) End Of Interrupt Register */
    kuint32_t C_RPR;                                /*!< Offset: 0x2014 (R/ ) Running Priority Register */
    kuint32_t C_HPPIR;                              /*!< Offset: 0x2018 (R/ ) Highest Priority Pending Interrupt Register */
    kuint32_t C_ABPR;                               /*!< Offset: 0x201C (R/W) Aliased Binary Point Register */
    kuint32_t C_AIAR;                               /*!< Offset: 0x2020 (R/ ) Aliased Interrupt Acknowledge Register */
    kuint32_t C_AEOIR;                              /*!< Offset: 0x2024 ( /W) Aliased End Of Interrupt Register */
    kuint32_t C_AHPPIR;                             /*!< Offset: 0x2028 (R/ ) Aliased Highest Priority Pending Interrupt Register */
    kuint32_t RESERVED15[41];
    kuint32_t C_APR0;                               /*!< Offset: 0x20D0 (R/W) Active Priority Register */
    kuint32_t RESERVED16[3];
    kuint32_t C_NSAPR0;                             /*!< Offset: 0x20E0 (R/W) Non-secure Active Priority Register */
    kuint32_t RESERVED17[6];
    kuint32_t C_IIDR;                               /*!< Offset: 0x20FC (R/ ) CPU Interface Identification Register */
    kuint32_t RESERVED18[960];
    kuint32_t C_DIR;                                /*!< Offset: 0x3000 ( /W) Deactivate Interrupt Register */

} srt_gic_cpu_t;

#define __mrt_get_gic_distributor() \
    (srt_gic_dist_t *)(mrt_mask(__get_cp15_cbar(), 0xffff0000U) + GIC_DIST_HEAD_OFFSET)
#define __mrt_get_gic_interface()   \
    (srt_gic_cpu_t *)(mrt_mask(__get_cp15_cbar(), 0xffff0000U) + GIC_DIST_HEAD_OFFSET + sizeof(srt_gic_dist_t))

#define mrt_get_gic_distributor(gic)  \
    (((gic) && ((gic)->dest_base)) ? (srt_gic_dist_t *)((gic)->dest_base) : __mrt_get_gic_distributor())
#define mrt_get_gic_interface(gic)    \
    (((gic) && ((gic)->cpu_base))  ? (srt_gic_cpu_t *)((gic)->cpu_base)  : __mrt_get_gic_interface())

/* The functions */
extern srt_gic_t *fwk_get_gic_data(kuint32_t gic_nr);
extern kint32_t fwk_gic_to_actual_irq(kint32_t hwirq);
extern kint32_t fwk_gpc_to_gic_irq(kint32_t virq);

static inline void hw_enable_irq(kint32_t hwirq)
{
    srt_gic_t *sprt_gic = fwk_get_gic_data(0);
    srt_gic_dist_t *sprt_dist;

    sprt_dist = mrt_get_gic_distributor(sprt_gic);
    mrt_setbit_towords(hwirq, &sprt_dist->D_ISENABLER);
}

static inline void hw_disable_irq(kint32_t hwirq)
{
    srt_gic_t *sprt_gic = fwk_get_gic_data(0);
    srt_gic_dist_t *sprt_dist;

    sprt_dist = mrt_get_gic_distributor(sprt_gic);
    mrt_setbit_towords(hwirq, &sprt_dist->D_ICENABLER);
}

/*!
 * @brief   gic enbale irq
 * @param   none
 * @retval  none
 * @note    enable IRQ: group0
 */
static inline void local_irq_enable(kint32_t irq_number)
{
    kint32_t hwirq;

    hwirq = fwk_gpc_to_gic_irq(irq_number);
    if (hwirq < 0)
        return;

    hw_enable_irq(hwirq);
}

/*!
 * @brief   gic disable irq
 * @param   none
 * @retval  none
 * @note    disable IRQ: group0
 */
static inline void local_irq_disable(kint32_t irq_number)
{
    kint32_t hwirq;

    hwirq = fwk_gpc_to_gic_irq(irq_number);
    if (hwirq < 0)
        return;

    hw_disable_irq(hwirq);
}

/*!
 * @brief   gic acknowledge irq
 * @param   none
 * @retval  none
 * @note    return IRQ number (and CPU source in SGI case)
 */
static inline kint32_t hw_irq_acknowledge(void)
{
    srt_gic_t *sprt_gic = fwk_get_gic_data(0);
    srt_gic_cpu_t *sprt_cpu;

    sprt_cpu = mrt_get_gic_interface(sprt_gic);
    return mrt_mask(sprt_cpu->C_IAR, 0x1fffU);
}

/*!
 * @brief   gic deactivate irq
 * @param   none
 * @retval  none
 * @note    value should be got from gic_acknowledge_irq()
 */
static inline void hw_irq_deactivate(kuint32_t value)
{
    srt_gic_t *sprt_gic = fwk_get_gic_data(0);
    srt_gic_cpu_t *sprt_cpu;

    sprt_cpu = mrt_get_gic_interface(sprt_gic);
    mrt_writel(value, &sprt_cpu->C_EOIR);
}

/*!
 * @brief   gic get running priority
 * @param   none
 * @retval  none
 * @note    get current interrupt priority
 */
static inline kuint32_t hw_irq_get_running_priority(void)
{
    srt_gic_t *sprt_gic = fwk_get_gic_data(0);
    srt_gic_cpu_t *sprt_cpu;

    sprt_cpu = mrt_get_gic_interface(sprt_gic);
    return mrt_mask(sprt_cpu->C_RPR, 0xffU);
}

/*!
 * @brief   gic set priority grouping
 * @param   none
 * @retval  none
 * @note    configure priority group
 */
static inline void hw_irq_set_priority_grouping(kuint32_t priorityGroup)
{
    srt_gic_t *sprt_gic = fwk_get_gic_data(0);
    srt_gic_cpu_t *sprt_cpu;

    sprt_cpu = mrt_get_gic_interface(sprt_gic);
    mrt_writel(mrt_mask(priorityGroup, 0x7U), &sprt_cpu->C_BPR);
}

/*!
 * @brief   gic get priority grouping
 * @param   none
 * @retval  none
 * @note    get priority group
 */
static inline kuint32_t hw_irq_get_priority_grouping(void)
{
    srt_gic_t *sprt_gic = fwk_get_gic_data(0);
    srt_gic_cpu_t *sprt_cpu;

    sprt_cpu = mrt_get_gic_interface(sprt_gic);
    return mrt_mask(sprt_cpu->C_BPR, 0x7U);
}

/*!
 * @brief   gic set priority
 * @param   none
 * @retval  none
 * @note    set "irq_number" interrupt priority
 */
static inline void hw_irq_set_priority(kuint32_t irq_number, kuint32_t priority)
{
    srt_gic_t *sprt_gic = fwk_get_gic_data(0);
    srt_gic_dist_t *sprt_dist;
    kint32_t hwirq;

    hwirq = fwk_gpc_to_gic_irq(irq_number);
    if (hwirq < 0)
        return;

    sprt_dist = mrt_get_gic_distributor(sprt_gic);
    mrt_writeb(mrt_bit_mask(priority, 0xffU, 8U - __GIC_PRIO_BITS), &sprt_dist->D_IPRIORITYR[hwirq]);
}

/*!
 * @brief   gic get priority
 * @param   none
 * @retval  none
 * @note    get "irq_number" interrupt priority
 */
static inline kuint32_t hw_irq_get_priority(kuint32_t irq_number)
{
    srt_gic_t *sprt_gic = fwk_get_gic_data(0);
    srt_gic_dist_t *sprt_dist;
    kint32_t hwirq;

    hwirq = fwk_gpc_to_gic_irq(irq_number);
    if (hwirq < 0)
        return 0;

    sprt_dist = mrt_get_gic_distributor(sprt_gic);
    return (kuint32_t)mrt_getbit_u8(0xffU, 8U - __GIC_PRIO_BITS, &sprt_dist->D_IPRIORITYR[hwirq]);
}

#ifdef __cplusplus
    }
#endif

#endif /* __GIC_BASIC_H */
