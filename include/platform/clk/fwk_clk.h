/*
 * Hardware Abstraction Layer Gpio Interface
 *
 * File Name:   fwk_clk.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.05.03
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_CLK_H_
#define __FWK_CLK_H_

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/fwk_platform.h>
#include <platform/of/fwk_of.h>

/*!< The defines */
struct mutex_lock;

enum __ERT_FWK_CLK_FLAG
{
	NR_FWK_CLK_IS_DYNAMIC = mrt_bit(0),
};

typedef struct fwk_clk
{
	const kchar_t *dev_id;
	const kchar_t *con_id;
	kuint32_t max_rate;
	kuint32_t min_rate;

	struct fwk_clk_core *sprt_core;
	struct list_head sgrt_link;

	kuint8_t flags;

} srt_fwk_clk_t;

typedef struct fwk_clk_core
{
	const kchar_t	*name;
	const struct fwk_clk_ops *sprt_ops;
	struct fwk_clk_hw *sprt_hw;

	struct fwk_clk_core	*sprt_parent;

	const kchar_t	**parent_names;
	struct fwk_clk_core	**sprt_parents;

	kuint32_t rate;

	kuint32_t flags;
	kuint32_t enable_count;
	kuint32_t prepare_count;

	struct list_head sgrt_clks;
	struct mutex_lock sgrt_mutex;

} srt_fwk_clk_core_t;

typedef struct fwk_clk_init_data 
{
	const kchar_t *name;
	const struct fwk_clk_ops *sprt_ops;

	const kchar_t **parent_names;
	kuint8_t num_parents;

	kuint32_t flags;

} srt_fwk_clk_init_data_t;

typedef struct fwk_clk_hw
{
	struct fwk_clk_core *sprt_core;
	struct fwk_clk *sprt_clk;
	const struct fwk_clk_init_data *sprt_init;

} srt_fwk_clk_hw_t;

typedef struct fwk_clk_ops
{
	kint32_t	(*prepare) (struct fwk_clk_hw *sprt_hw);
	void		(*unprepare) (struct fwk_clk_hw *sprt_hw);
	kint32_t	(*is_prepared) (struct fwk_clk_hw *sprt_hw);
	void		(*unprepare_unused) (struct fwk_clk_hw *sprt_hw);
	kint32_t	(*enable) (struct fwk_clk_hw *sprt_hw);
	void		(*disable) (struct fwk_clk_hw *sprt_hw);
	kint32_t	(*is_enabled) (struct fwk_clk_hw *sprt_hw);
	void		(*disable_unused) (struct fwk_clk_hw *sprt_hw);

	kint32_t	(*round_rate) (struct fwk_clk_hw *sprt_hw, kuint32_t rate, kuint32_t *parent_rate);

	kint32_t	(*set_parent) (struct fwk_clk_hw *sprt_hw, kuint8_t index);
	kuint8_t	(*get_parent) (struct fwk_clk_hw *sprt_hw);
	kint32_t	(*set_rate) (struct fwk_clk_hw *sprt_hw, kuint32_t rate, kuint32_t parent_rate);

	kint32_t	(*get_phase) (struct fwk_clk_hw *sprt_hw);
	kint32_t	(*set_phase) (struct fwk_clk_hw *sprt_hw, kint32_t degrees);
	void		(*init) (struct fwk_clk_hw *sprt_hw);

} srt_fwk_clk_ops_t;

/*!< The functions */
extern struct fwk_clk *fwk_clk_register(struct fwk_device *sprt_dev, struct fwk_clk_hw *sprt_hw);
extern void fwk_clk_unregister(struct fwk_clk *sprt_clk);

extern struct fwk_clk *fwk_clk_config(struct fwk_clk *sprt_clk, 
									struct fwk_clk_hw *sprt_hw, const kchar_t *dev_id, const kchar_t *con_id);
extern struct fwk_clk *fwk_create_clk(struct fwk_clk_hw *sprt_hw, const kchar_t *dev_id, const kchar_t *con_id);
extern kint32_t fwk_init_clk(struct fwk_device *sprt_dev, struct fwk_clk *sprt_clk);

extern struct fwk_clk *fwk_clk_get(struct fwk_device *sprt_dev, const kchar_t *name);
extern void fwk_clk_put(struct fwk_clk *sprt_clk);
extern void fwk_clk_enable(struct fwk_clk *sprt_clk);
extern void fwk_clk_disable(struct fwk_clk *sprt_clk);
extern void fwk_clk_prepare(struct fwk_clk *sprt_clk);
extern void fwk_clk_unprepare(struct fwk_clk *sprt_clk);
extern kbool_t fwk_clk_is_enabled(struct fwk_clk *sprt_clk);
extern void fwk_clk_prepare_enable(struct fwk_clk *sprt_clk);
extern void fwk_clk_disable_unprepare(struct fwk_clk *sprt_clk);

/*!< API functions */
/*!
 * @brief   sprt_clk ---> fwk_clk_hw
 * @param   sprt_clk
 * @retval  sprt_hw
 * @note    none
 */
static inline struct fwk_clk_hw *fwk_clk_to_hw(struct fwk_clk *sprt_clk)
{
    return sprt_clk ? sprt_clk->sprt_core->sprt_hw : mrt_nullptr;
}

#ifdef __cplusplus
    }
#endif

#endif /*!< __FWK_CLK_H_ */
