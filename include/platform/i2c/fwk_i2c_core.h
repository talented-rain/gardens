/*
 * Hardware Abstraction Layer I2C Interface
 *
 * File Name:   fwk_i2c_core.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.07.16
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_I2C_CORE_H_
#define __FWK_I2C_CORE_H_

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/fwk_platform.h>
#include <kernel/rw_lock.h>

#include "fwk_i2c_algo.h"

/*!< The defines */
struct fwk_i2c_client;

typedef struct fwk_i2c_adapter
{
	kuint32_t id;
	const struct fwk_i2c_algo *sprt_algo; 				/*!< the algorithm to access the bus */
	void *algo_data;

	kint32_t timeout;		         	 				/*!< in jiffies */
	kint32_t retries;
	struct fwk_device sgrt_dev;		         			/*!< the adapter device */

	kint32_t nr;
	kchar_t name[48];

	struct list_head sgrt_clients;
	struct rw_lock sgrt_lock;

} srt_fwk_i2c_adapter_t;

/*!< The functions */
extern kint32_t fwk_i2c_transfer(struct fwk_i2c_client *sprt_client, struct fwk_i2c_msg *sprt_msgs, kint32_t num);

/*!< API functions */
/*!
 * @brief   save driver data to i2c device
 * @param   sprt_adap, data
 * @retval  none
 * @note    none
 */
static inline void fwk_i2c_adapter_set_drvdata(struct fwk_i2c_adapter *sprt_adap, void *data)
{
	sprt_adap->sgrt_dev.privData = data;
}

/*!
 * @brief   get driver data from i2c device
 * @param   sprt_adap
 * @retval  none
 * @note    none
 */
static inline void *fwk_i2c_adapter_get_drvdata(struct fwk_i2c_adapter *sprt_adap)
{
	return sprt_adap->sgrt_dev.privData;
}

#ifdef __cplusplus
    }
#endif

#endif /*!< __FWK_I2C_CORE_H_ */
