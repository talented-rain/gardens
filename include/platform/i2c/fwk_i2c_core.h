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

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/fwk_platform.h>

#include "fwk_i2c_algo.h"

/*!< The defines */
struct fwk_i2c_client *sprt_client;

typedef struct fwk_i2c_adapter
{
	kuint32_t id;
	const struct fwk_i2c_algo *sprt_algo; 				/*!< the algorithm to access the bus */
	void *algo_data;

	ksint32_t timeout;		         	 				/*!< in jiffies */
	ksint32_t retries;
	struct fwk_device sgrt_dev;		         			/*!< the adapter device */

	ksint32_t nr;
	ksint8_t name[48];

	struct list_head sgrt_clients;

} srt_fwk_i2c_adapter_t;

/*!< The functions */
TARGET_EXT ksint32_t fwk_i2c_transfer(struct fwk_i2c_client *sprt_client, struct fwk_i2c_msg *sprt_msgs, ksint32_t num);

/*!< API functions */
static inline void fwk_i2c_adapter_set_drvdata(struct fwk_i2c_adapter *sprt_adap, void *data)
{
	sprt_adap->sgrt_dev.privData = data;
}

static inline void *fwk_i2c_adapter_get_drvdata(struct fwk_i2c_adapter *sprt_adap)
{
	return sprt_adap->sgrt_dev.privData;
}

#endif /*!< __FWK_I2C_CORE_H_ */
