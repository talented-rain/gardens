/*
 * ARM V7 Interrupt API Function
 *
 * File Name:   interrupt.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.09.10
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <configs/configs.h>
#include <common/generic.h>
#include <common/io_stream.h>
#include <asm/interrupt.h>
#include <platform/irq/fwk_irq_types.h>

/*!< API function */
/*!
 * @brief   exec_fiq_handler
 * @param   none
 * @retval  none
 * @note    FIQ exception
 */
void exec_fiq_handler(void)
{
    
}

/*!
 * @brief   exec_irq_handler
 * @param   none
 * @retval  none
 * @note    IRQ exception
 */
void exec_irq_handler(void)
{
    kint32_t hardirq, softIrq;

    /*!< read IAR, enable IRQ */
    hardirq = hw_irq_acknowledge();

    /*!< find system soft IRQn, and excute IRQ handler */
    softIrq = fwk_gic_to_actual_irq(hardirq);
    fwk_do_irq_handler(softIrq);

    /*!< write IAR, disable IRQ */
    hw_irq_deactivate(hardirq);
}

/*!
 * @brief   exec_software_irq_handler
 * @param   none
 * @retval  none
 * @note    SWI exception
 */
void exec_software_irq_handler(void)
{
    kint32_t hardirq;
    kuint32_t event = 0;

    __asm__ __volatile__ (
        " mov %0, r12 " 
        : "=&r"(event) 
    );

    /*!< read IAR, enable IRQ */
    hardirq = hw_irq_acknowledge();
//  fwk_handle_softirq(hardirq, event);

    /*!< write IAR, disable IRQ */
    hw_irq_deactivate(hardirq);
}

/* end of file*/
