/*
 * ARMv7 Configuration
 *
 * File Name:   arch_common.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.12.16
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __ARCH_COMMON_H
#define __ARCH_COMMON_H

/*!< The includes */
#include "asm_config.h"
#include "gcc_config.h"
#include "gic_basic.h"

/*!< The defines */
#if (defined(CONFIG_OF))
    #define CPU_INTC_COMPATIBLE                             "arm,cortex-a7-gic"
#endif

#define mrt_enable_irq(irqNumber)                           local_irq_enable(irqNumber)
#define mrt_disable_irq(irqNumber)                          local_irq_disable(irqNumber)
#define mrt_get_irq_pri(irqNumber)                          local_irq_get_priority(irqNumber)
#define mrt_set_irq_pri(irqNumber, pri)                     local_irq_set_priority(irqNumber, pri)

/*!< API function */
/*!
 * @brief   enable irq
 * @param   none
 * @retval  none
 * @note    GIC Interrupt
 */
static inline void hw_enable_irq(kint32_t irqNumber)
{
    mrt_enable_irq(irqNumber);
}

/*!
 * @brief   disable irq
 * @param   none
 * @retval  none
 * @note    GIC Interrupt
 */
static inline void hw_disable_irq(kint32_t irqNumber)
{
    mrt_disable_irq(irqNumber);
}

/*!
 * @brief   get irq priority
 * @param   none
 * @retval  none
 * @note    GIC Interrupt
 */
static inline kuint32_t get_irq_priority(kint32_t irqNumber)
{
    return mrt_get_irq_pri(irqNumber);
}

/*!
 * @brief   set irq priority
 * @param   none
 * @retval  none
 * @note    GIC Interrupt
 */
static inline void set_irq_priority(kint32_t irqNumber, kuint32_t priority)
{
    mrt_set_irq_pri(irqNumber, priority);
}


#endif /* __ARCH_COMMON_H */
