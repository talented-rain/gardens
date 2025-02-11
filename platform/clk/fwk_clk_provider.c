/*
 * Hardware Abstraction Layer Clock Interface
 *
 * File Name:   fwk_clk_gate.c
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
#include <kernel/rw_lock.h>

/*!< The defines */
typedef struct fwk_of_clk_provider
{
	struct list_head sgrt_link;

	struct fwk_device_node *sprt_node;
	struct fwk_clk *(*get)(struct fwk_of_phandle_args *sprt_args, void *data);
	void *data;

} srt_fwk_of_clk_provider_t;

/*!< The globals */
static DECLARE_LIST_HEAD(sgrt_fwk_clk_providers);
static struct rw_lock sgrt_clk_providers_lock = RW_LOCK_INIT();

/*!< API function */
/*!
 * @brief   get sprt_clk from data
 * @param   sprt_args, data
 * @retval  sprt_clk
 * @note    none
 */
struct fwk_clk *fwk_of_clk_src_onecell_get(struct fwk_of_phandle_args *sprt_args, void *data)
{
    struct fwk_clk_one_cell *sprt_cell;
    kuint32_t index;

    if (!sprt_args || !data)
        return mrt_nullptr;

    sprt_cell = (struct fwk_clk_one_cell *)data;
    index = sprt_args->args[0];

    return sprt_cell->sprt_clks ? &sprt_cell->sprt_clks[index] : mrt_nullptr;
}

/*!
 * @brief   add a new clk provider (data)
 * @param   sprt_node, data
 * @retval  errno
 * @note    none
 */
kint32_t fwk_clk_add_provider(struct fwk_device_node *sprt_node, 
                    struct fwk_clk *(*get)(struct fwk_of_phandle_args *, void *), void *data)
{
    struct fwk_of_clk_provider *sprt_provider;

    if (!sprt_node || !get)
        return -ER_NODEV;

    sprt_provider = kzalloc(sizeof(*sprt_provider), GFP_KERNEL);
    if (!isValid(sprt_provider))
        return -ER_NOMEM;

    sprt_provider->sprt_node = sprt_node;
    sprt_provider->get = get;
    sprt_provider->data = data;

    wr_lock(&sgrt_clk_providers_lock);
    list_head_add_tail(&sgrt_fwk_clk_providers, &sprt_provider->sgrt_link);
    wr_unlock(&sgrt_clk_providers_lock);

    return ER_NORMAL;
}

/*!
 * @brief   remove and destroy clk a provider
 * @param   sprt_node
 * @retval  none
 * @note    none
 */
void fwk_clk_del_provider(struct fwk_device_node *sprt_node)
{
    struct fwk_of_clk_provider *sprt_provider;

    if (!sprt_node)
        return;

    wr_lock(&sgrt_clk_providers_lock);

    foreach_list_next_entry(sprt_provider, &sgrt_fwk_clk_providers, sgrt_link)
    {
        if (sprt_provider->sprt_node == sprt_node)
        {
            list_head_del(&sprt_provider->sgrt_link);
            kfree(sprt_provider);
        }
    }

    wr_unlock(&sgrt_clk_providers_lock);
}

/*!
 * @brief   look up a provider
 * @param   sprt_args
 * @retval  sprt_clk
 * @note    none
 */
struct fwk_clk *fwk_clk_provider_look_up(struct fwk_of_phandle_args *sprt_args)
{
    struct fwk_of_clk_provider *sprt_provider;

    if (!sprt_args)
        return mrt_nullptr;

    rd_lock(&sgrt_clk_providers_lock);

    foreach_list_next_entry(sprt_provider, &sgrt_fwk_clk_providers, sgrt_link)
    {
        if (sprt_provider->sprt_node == sprt_args->sprt_node)
        {
            rd_unlock(&sgrt_clk_providers_lock);
            return sprt_provider->get(sprt_args, sprt_provider->data);
        }
    }

    rd_unlock(&sgrt_clk_providers_lock);
    return mrt_nullptr;
}

/*!< end of file */
