/*
 * Hardware Abstraction Layer Clock Interface
 *
 * File Name:   fwk_clk.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.05.03
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <platform/of/fwk_of.h>
#include <platform/clk/fwk_clk.h>
#include <platform/clk/fwk_clk_provider.h>
#include <kernel/mutex.h>

/*!< API function */
/*!
 * @brief   initial and record sprt_clk
 * @param   sprt_clk, sprt_hw
 * @retval  sprt_clk
 * @note    none
 */
struct fwk_clk *fwk_clk_config(struct fwk_clk *sprt_clk, struct fwk_clk_hw *sprt_hw, const kchar_t *dev_id, const kchar_t *con_id)
{
    if (!sprt_hw || !sprt_clk)
        return mrt_nullptr;
    
    sprt_clk->dev_id = dev_id;
    sprt_clk->con_id = con_id;
    sprt_clk->max_rate = ~(kutype_t)0;
    sprt_clk->sprt_core = sprt_hw->sprt_core;
    sprt_clk->flags &= ~NR_FWK_CLK_IS_DYNAMIC;

    mutex_lock(&sprt_hw->sprt_core->sgrt_mutex);
    list_head_add_tail(&sprt_hw->sprt_core->sgrt_clks, &sprt_clk->sgrt_link);
    mutex_unlock(&sprt_hw->sprt_core->sgrt_mutex);

    return sprt_clk;
}

/*!
 * @brief   create and initialize sprt_clk
 * @param   sprt_hw, dev_id, con_id
 * @retval  sprt_clk
 * @note    none
 */
struct fwk_clk *fwk_create_clk(struct fwk_clk_hw *sprt_hw, const kchar_t *dev_id, const kchar_t *con_id)
{
    struct fwk_clk *sprt_clk;

    if (!sprt_hw)
        return mrt_nullptr;

    sprt_clk = kzalloc(sizeof(*sprt_clk), GFP_KERNEL);
    if (!isValid(sprt_clk))
        return mrt_nullptr;

    if (fwk_clk_config(sprt_clk, sprt_hw, dev_id, con_id))
        sprt_clk->flags |= NR_FWK_CLK_IS_DYNAMIC;

    return sprt_clk;
}

/*!
 * @brief   initialize sprt_clk
 * @param   sprt_dev, sprt_clk
 * @retval  errno
 * @note    none
 */
kint32_t fwk_init_clk(struct fwk_device *sprt_dev, struct fwk_clk *sprt_clk)
{
    return ER_NORMAL;
}

/*!
 * @brief   destroy sprt_clk
 * @param   sprt_clk
 * @retval  none
 * @note    none
 */
void fwk_free_clk(struct fwk_clk *sprt_clk)
{
    if (!sprt_clk)
        return;

    list_head_del(&sprt_clk->sgrt_link);

    if (sprt_clk->flags & NR_FWK_CLK_IS_DYNAMIC)
        kfree(sprt_clk);
}

/*!
 * @brief   register sprt_clk
 * @param   sprt_dev, sprt_hw
 * @retval  sprt_clk
 * @note    none
 */
struct fwk_clk *fwk_clk_register(struct fwk_device *sprt_dev, struct fwk_clk_hw *sprt_hw)
{
    struct fwk_clk_core *sprt_core;
    const struct fwk_clk_init_data *sprt_init;
    kuint32_t i;

    sprt_init = sprt_hw->sprt_init;
    
    sprt_core = kzalloc(sizeof(*sprt_core), GFP_KERNEL);
    if (!isValid(sprt_core))
        return mrt_nullptr;

    sprt_core->sprt_ops = sprt_init->sprt_ops;
    sprt_core->name = sprt_init->name;
    sprt_core->sprt_hw = sprt_hw;
    sprt_hw->sprt_core = sprt_core;
    init_list_head(&sprt_core->sgrt_clks);
    mutex_init(&sprt_core->sgrt_mutex);

    sprt_core->parent_names = kzalloc(sprt_init->num_parents * sizeof(kchar_t *), GFP_KERNEL);
    if (!isValid(sprt_core->parent_names))
        goto fail1;

    for (i = 0; i < sprt_init->num_parents; i++)
    {
        sprt_core->parent_names[i] = sprt_init->parent_names[i];

        ERR_OUT(!sprt_core->parent_names[i], "parent names is empty!\n");
        if (!sprt_core->parent_names[i])
            goto fail2;
    }

    if (!sprt_hw->sprt_clk)
    {
        sprt_hw->sprt_clk = fwk_create_clk(sprt_hw, mrt_nullptr, mrt_nullptr);
        if (!isValid(sprt_hw->sprt_clk))
            goto fail2;
    }
    else
        fwk_clk_config(sprt_hw->sprt_clk, sprt_hw, mrt_nullptr, mrt_nullptr);

    if (fwk_init_clk(sprt_dev, sprt_hw->sprt_clk))
        goto fail3;

    return sprt_hw->sprt_clk;

fail3:
    fwk_free_clk(sprt_hw->sprt_clk);
    sprt_hw->sprt_clk = mrt_nullptr;
fail2:
    kfree(sprt_core->parent_names);
fail1:
    kfree(sprt_core);

    return mrt_nullptr;
}

/*!
 * @brief   unregister sprt_clk
 * @param   sprt_clk
 * @retval  none
 * @note    none
 */
void fwk_clk_unregister(struct fwk_clk *sprt_clk)
{
    struct fwk_clk_hw *sprt_hw;
    struct fwk_clk *sprt_child, *sprt_temp;

    sprt_hw = fwk_clk_to_hw(sprt_clk);
    mutex_lock(&sprt_hw->sprt_core->sgrt_mutex);

    foreach_list_next_entry_safe(sprt_child, sprt_temp, &sprt_hw->sprt_core->sgrt_clks, sgrt_link)
        fwk_free_clk(sprt_child);

    mutex_unlock(&sprt_hw->sprt_core->sgrt_mutex);

    if (sprt_hw->sprt_core->parent_names)
        kfree(sprt_hw->sprt_core->parent_names);

    kfree(sprt_hw->sprt_core);
    sprt_hw->sprt_core = mrt_nullptr;
}

/*!
 * @brief   parse clk from device_node
 * @param   sprt_dev, name
 * @retval  sprt_clk
 * @note    none
 */
struct fwk_clk *fwk_clk_get(struct fwk_device *sprt_dev, const kchar_t *name)
{
    struct fwk_clk *sprt_clk;
    struct fwk_device_node *sprt_node;
    struct fwk_of_phandle_args sgrt_args;
    kint32_t index = 0;

    sprt_node = sprt_dev->sprt_node;

    if (name)
    {
        index = fwk_of_property_match_string(sprt_node, "clock-names", name);
        if (index < 0)
            return mrt_nullptr;
    }

    if (fwk_of_parse_phandle_with_args(sprt_node, "clocks", "#clock-cells", 0, index, &sgrt_args))
        return mrt_nullptr;

    sprt_clk = fwk_clk_provider_look_up(&sgrt_args);
    if (!isValid(sprt_clk))
        return mrt_nullptr;

    return fwk_create_clk(fwk_clk_to_hw(sprt_clk), mrt_dev_get_name(sprt_dev), name);
}

/*!
 * @brief   release clk
 * @param   sprt_clk
 * @retval  none
 * @note    none
 */
void fwk_clk_put(struct fwk_clk *sprt_clk)
{
    fwk_free_clk(sprt_clk);
}

/*!
 * @brief   enable clk
 * @param   sprt_clk
 * @retval  none
 * @note    none
 */
void fwk_clk_enable(struct fwk_clk *sprt_clk)
{
    const struct fwk_clk_ops *sprt_ops;

    sprt_ops = sprt_clk->sprt_core->sprt_ops;

    if (sprt_ops && sprt_ops->enable)
        sprt_ops->enable(fwk_clk_to_hw(sprt_clk));
}

/*!
 * @brief   disable clk
 * @param   sprt_clk
 * @retval  none
 * @note    none
 */
void fwk_clk_disable(struct fwk_clk *sprt_clk)
{
    const struct fwk_clk_ops *sprt_ops;

    sprt_ops = sprt_clk->sprt_core->sprt_ops;

    if (sprt_ops && sprt_ops->disable)
        sprt_ops->disable(fwk_clk_to_hw(sprt_clk));
}

/*!
 * @brief   prepare clk
 * @param   sprt_clk
 * @retval  none
 * @note    none
 */
void fwk_clk_prepare(struct fwk_clk *sprt_clk)
{
    const struct fwk_clk_ops *sprt_ops;

    sprt_ops = sprt_clk->sprt_core->sprt_ops;

    if (sprt_ops && sprt_ops->prepare)
        sprt_ops->prepare(fwk_clk_to_hw(sprt_clk));
}

/*!
 * @brief   unprepare clk
 * @param   sprt_clk
 * @retval  none
 * @note    none
 */
void fwk_clk_unprepare(struct fwk_clk *sprt_clk)
{
    const struct fwk_clk_ops *sprt_ops;

    sprt_ops = sprt_clk->sprt_core->sprt_ops;

    if (sprt_ops && sprt_ops->unprepare)
        sprt_ops->unprepare(fwk_clk_to_hw(sprt_clk));
}

/*!
 * @brief   check if clk is enabled
 * @param   sprt_clk
 * @retval  enable/disable
 * @note    none
 */
kbool_t fwk_clk_is_enabled(struct fwk_clk *sprt_clk)
{
    const struct fwk_clk_ops *sprt_ops;

    sprt_ops = sprt_clk->sprt_core->sprt_ops;

    if (sprt_ops && sprt_ops->is_enabled)
        return !sprt_ops->is_enabled(fwk_clk_to_hw(sprt_clk));

    return false;
}

/*!
 * @brief   enable clk
 * @param   sprt_clk
 * @retval  none
 * @note    none
 */
void fwk_clk_prepare_enable(struct fwk_clk *sprt_clk)
{
    fwk_clk_prepare(sprt_clk);
    fwk_clk_enable(sprt_clk);
}

/*!
 * @brief   disable clk
 * @param   sprt_clk
 * @retval  none
 * @note    none
 */
void fwk_clk_disable_unprepare(struct fwk_clk *sprt_clk)
{
    fwk_clk_disable(sprt_clk);
    fwk_clk_unprepare(sprt_clk);
}

/*!< end of file */
