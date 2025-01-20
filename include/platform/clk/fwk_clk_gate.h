/*
 * Hardware Abstraction Layer Gpio Interface
 *
 * File Name:   fwk_clk_gate.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.05.03
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_CLK_GATE_H_
#define __FWK_CLK_GATE_H_

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The includes */
#include <platform/fwk_basic.h>
#include "fwk_clk.h"

/*!< The defines */
typedef struct fwk_clk_gate 
{
	struct fwk_clk_hw sgrt_hw;
	void *reg;
	kuint8_t bit_idx;
	kuint8_t flags;

} srt_fwk_clk_gate_t;

/*!< The functions */
extern struct fwk_clk *fwk_clk_gate_register(struct fwk_clk *sprt_clk, const struct fwk_clk_ops *sprt_ops,
                                const kchar_t *name, const kchar_t *parent, void *reg, kuint8_t shift);
extern void fwk_clk_gate_unregister(struct fwk_clk *sprt_clk);

#ifdef __cplusplus
    }
#endif

#endif /*!< __FWK_CLK_GATE_H_ */
