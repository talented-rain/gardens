/*
 * ZYNQ7 Board Terminal Initial
 *
 * File Name:   zynq7_console.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.06.22
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <common/time.h>
#include <platform/of/fwk_of.h>
#include <platform/of/fwk_of_device.h>
#include <platform/irq/fwk_irq_types.h>
#include "zynq7_common.h"

/*!< The defines */
#define ZYNQ7_SCUTIMER0_FREQ_HZ                 (XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ / 2)

/*!< The globals */
static XScuTimer sgrt_ps7_xscu_timer_data;

#ifdef CONFIG_OF
static const struct fwk_of_device_id sgrt_zynq7_systick_ids[] =
{
	{ .compatible = "arm,cortex-a9-twd-timer" },
	{},
};
#endif

/*!< The functions */
irq_return_t zynq7_systick_isr(void *ptrDev);

/*!< API function */
/*!
 * @brief   initial and start serial
 * @param   none
 * @retval  none
 * @note    none
 */
void zynq7_systick_init(void)
{
    struct fwk_device_node *sprt_node;
    XScuTimer *sprt_timer;
    XScuTimer_Config *sprt_cfg, sgrt_cfg;
    kuaddr_t irq;
    kint32_t retval;

    sprt_timer = &sgrt_ps7_xscu_timer_data;

#ifdef CONFIG_OF
    sprt_cfg = &sgrt_cfg;

    sprt_node = fwk_of_find_matching_node_and_match(mrt_nullptr, sgrt_zynq7_systick_ids, mrt_nullptr);
    if (!isValid(sprt_node))
        return;

    sprt_cfg->DeviceId = XPAR_PS7_SCUTIMER_0_DEVICE_ID;
    sprt_cfg->BaseAddr = (kuaddr_t)fwk_of_iomap(sprt_node, 0);
    irq = fwk_of_irq_get(sprt_node, 0);

#else
    sprt_cfg = XScuTimer_LookupConfig(XPAR_PS7_SCUTIMER_0_DEVICE_ID);
    if (!isValid(sprt_cfg))
        return;

    irq = XPAR_SCUTIMER_INTR - 16;

#endif

    retval = XScuTimer_CfgInitialize(sprt_timer, sprt_cfg, sprt_cfg->BaseAddr);
    if (retval)
        return;

    XScuTimer_DisableInterrupt(sprt_timer);

    /*!< enable interrupt */
    retval = fwk_request_irq(irq, zynq7_systick_isr, 0, "zynq7-systick", sprt_timer);
    if (!retval)
    {
        XScuTimer_EnableInterrupt(sprt_timer);
    }

    /*!< 10ms */
    XScuTimer_LoadTimer(sprt_timer, ZYNQ7_SCUTIMER0_FREQ_HZ / TICK_HZ);
    XScuTimer_EnableAutoReload(sprt_timer);
    XScuTimer_Start(sprt_timer);
}

/*!
 * @brief   zynq7 systick irq handler
 * @param   none
 * @retval  none
 * @note    increase jiffies
 */
irq_return_t zynq7_systick_isr(void *ptrDev)
{
    XScuTimer *sprt_timer = (XScuTimer *)ptrDev;

    if (XScuTimer_GetInterruptStatus(sprt_timer))
    {
        /*!< reset jiffies when counter over */
		get_time_counter();
		do_timer_event();

        XScuTimer_ClearInterruptStatus(sprt_timer);
    }

    return ER_NORMAL;
}

/* end of file*/
