/*
 * Interrupt Interface Defines
 *
 * File Name:   fwk_irq_types.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.11.14
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/irq/fwk_irq_types.h>
#include <platform/irq/fwk_irq_chip.h>
#include <platform/irq/fwk_irq_domain.h>
#include <platform/of/fwk_of.h>

/*!< The globals */

/*!< API function */
/*!
 * @brief  fwk_default_irq_isr
 * @param  none
 * @retval none
 * @note   default irq handler
 */
static irq_return_t fwk_default_irq_isr(void *ptrDev)
{
    return ER_NORMAL;
}

/*!
 * @brief  fwk_request_irq
 * @param  none
 * @retval none
 * @note   irq register
 */
void *fwk_find_irq_action(kint32_t irq, const kchar_t *name, void *ptrDev)
{
    struct fwk_irq_desc *sprt_desc;
    struct fwk_irq_action *sprt_action;
    kint32_t retval = 0;

    sprt_desc = fwk_irq_to_desc(irq);
    if (!isValid(sprt_desc))
        return mrt_nullptr;

    spin_lock_irqsave(&sprt_desc->sgrt_lock);

    foreach_list_next_entry(sprt_action, &sprt_desc->sgrt_action, sgrt_link)
    {
        if (name)
            retval = strncmp(sprt_action->name, name, FWK_IRQ_DESC_NAME_LENTH);

        if (!retval && (sprt_action->ptrArgs == ptrDev))
        {
            spin_unlock_irqrestore(&sprt_desc->sgrt_lock);
            return sprt_action;
        }
    }

    spin_unlock_irqrestore(&sprt_desc->sgrt_lock);
    return mrt_nullptr;
}

/*!
 * @brief  fwk_request_irq
 * @param  none
 * @retval none
 * @note   irq register
 */
kint32_t fwk_request_irq(kint32_t irq, irq_handler_t handler, kuint32_t flags, const kchar_t *name, void *ptrDev)
{
    struct fwk_irq_desc *sprt_desc;
    struct fwk_irq_action *sprt_action;
    kuint32_t len = kstrlen(name);

    if ((!name) || (!ptrDev))
        return -ER_FAULT;

    if (fwk_find_irq_action(irq, name, ptrDev))
        return -ER_EXISTED;

    sprt_desc = fwk_irq_to_desc(irq);
    if (!isValid(sprt_desc))
        return -ER_NOMEM;

    sprt_action = (struct fwk_irq_action *)kzalloc(sizeof(*sprt_action), GFP_KERNEL);
    if (!isValid(sprt_action))
        return -ER_NOMEM;

    sprt_action->handler = handler ? handler : fwk_default_irq_isr;
    sprt_action->flags = flags;
    sprt_action->ptrArgs = ptrDev;

    if (len >= sizeof(sprt_action->name))
        goto fail;
    
    kstrcpy(sprt_action->name, name);
    
    fwk_irq_set_type(irq, flags);

    spin_lock_irqsave(&sprt_desc->sgrt_lock);
    list_head_add_tail(&sprt_desc->sgrt_action, &sprt_action->sgrt_link);
    spin_unlock_irqrestore(&sprt_desc->sgrt_lock);

    fwk_enable_irq(irq);

    return ER_NORMAL;

fail:
    kfree(sprt_action);
    return -ER_CHECKERR;
}

/*!
 * @brief  fwk_free_irq
 * @param  none
 * @retval none
 * @note   irq unregister
 */
void fwk_free_irq(kint32_t irq, void *ptrDev)
{
    struct fwk_irq_desc *sprt_desc;
    struct fwk_irq_action *sprt_action;

    if ((irq < 0) || (!ptrDev))
        return;

    sprt_desc = fwk_irq_to_desc(irq);
    if (!isValid(sprt_desc))
        return;

    fwk_disable_irq(irq);

    sprt_action = fwk_find_irq_action(irq, mrt_nullptr, ptrDev);
    if (isValid(sprt_action))
    {
        spin_lock_irqsave(&sprt_desc->sgrt_lock);
        list_head_del(&sprt_action->sgrt_link);
        spin_unlock_irqrestore(&sprt_desc->sgrt_lock);

        kfree(sprt_action);
    }
}

/*!
 * @brief   destroy the content of irq_desc
 * @param   irq
 * @retval  none
 * @note    none
 */
void fwk_destroy_irq_action(kint32_t irq)
{
    struct fwk_irq_desc *sprt_desc;
    struct fwk_irq_action *sprt_action, *sprt_temp;

    sprt_desc = fwk_irq_to_desc(irq);
    if (!isValid(sprt_desc))
        return;

    spin_lock_irqsave(&sprt_desc->sgrt_lock);

    foreach_list_next_entry_safe(sprt_action, sprt_temp, &sprt_desc->sgrt_action, sgrt_link)
    {
        list_head_del(&sprt_action->sgrt_link);
        kfree(sprt_action);
    }

    spin_unlock_irqrestore(&sprt_desc->sgrt_lock);
}

/*!
 * @brief   fwk_do_irq_handler
 * @param   none
 * @retval  none
 * @note    excute irq handler
 */
void fwk_do_irq_handler(kint32_t softIrq)
{
    struct fwk_irq_desc *sprt_desc;
    struct fwk_irq_action *sprt_action;
    kint32_t retval;

    if (softIrq < 0)
        return;

    sprt_desc = fwk_irq_to_desc(softIrq);
    if (!isValid(sprt_desc))
        return;
        
    foreach_list_next_entry(sprt_action, &sprt_desc->sgrt_action, sgrt_link)
    {
        retval = sprt_action->handler ? sprt_action->handler(sprt_action->ptrArgs) : -1;
        switch (retval)
        {
            case 0:
                break;

            case 1:
                break;

            default:
                break;
        }
    }
}

/*!
 * @brief   fwk_handle_softirq
 * @param   none
 * @retval  none
 * @note    excute irq handler
 */
void fwk_handle_softirq(kint32_t softIrq, kuint32_t event)
{
    switch (event)
    {
        case SWI_EVENT_SCHEDULED:
            print_info("trigger NR_EVENT_SCHEDULED \n");
            break;
        
        case SWI_EVENT_SYSCALL:
            print_info("trigger NR_EVENT_SYSCALL \n");
            break;

        default: break;
    }
}

/* end of file */
