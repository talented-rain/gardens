/*
 * Hardware Abstraction Layer I2C-EEPROM Interface
 *
 * File Name:   fwk_eeprom.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.06.07
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_EEPROM_H_
#define __FWK_EEPROM_H_

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The includes */
#include <platform/fwk_basic.h>

/*!< The defines */
#define FWK_EEPROM_READ							FWK_IOR('r', 1, kuint8_t)
#define FWK_EEPROM_WRITE						FWK_IOR('w', 1, kuint8_t)

struct fwk_eeprom
{
	kuaddr_t addr;
	kuint32_t offset;

	kuint8_t *buf;
	kusize_t size;
};

#ifdef __cplusplus
    }
#endif

#endif /*!< __FWK_EEPROM_H_ */
