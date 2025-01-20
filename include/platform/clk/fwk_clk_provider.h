/*
 * Hardware Abstraction Layer Clock Interface
 *
 * File Name:   fwk_clk_provider.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.05.06
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_CLK_PROVIDER_H_
#define __FWK_CLK_PROVIDER_H_

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The includes */
#include <platform/fwk_basic.h>
#include "fwk_clk.h"

/*!< The defines */
typedef struct fwk_clk_one_cell
{
	struct fwk_clk *sprt_clks;
	kuint32_t clks_size;

} srt_fwk_clk_one_cell_t;

/*!< The functions */
extern struct fwk_clk *fwk_of_clk_src_onecell_get(struct fwk_of_phandle_args *sprt_args, void *data);
extern kint32_t fwk_clk_add_provider(struct fwk_device_node *sprt_node, 
                    struct fwk_clk *(*get)(struct fwk_of_phandle_args *, void *), void *data);
extern void fwk_clk_del_provider(struct fwk_device_node *sprt_node);
extern struct fwk_clk *fwk_clk_provider_look_up(struct fwk_of_phandle_args *sprt_args);

#ifdef __cplusplus
    }
#endif

#endif /*!< __FWK_CLK_PROVIDER_H_ */
