/*
 * Character Device Interface
 *
 * File Name:   fwk_chrdev.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.05.19
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_CHRDEV_H_
#define __FWK_CHRDEV_H_

/*!< The includes */
#include <platform/fwk_basic.h>

/*!< The defines */
struct fwk_char_device
{
	kuint32_t major;
	kuint32_t baseminor;
	kuint32_t count;
	kint8_t name[DEVICE_NAME_LEN];

	/*!< All devices with the same primary device number form a list */
	struct fwk_char_device *sprt_next;
};

/*!< The globals */
TARGET_EXT struct fwk_char_device *sgrt_fwk_chrdevs[];

/*!< The functions */
/*!< -------------------------------------------------------------- */
TARGET_EXT kint32_t fwk_chrdev_init(void);
TARGET_EXT void fwk_chrdev_exit(void);

/*!< -------------------------------------------------------------- */
TARGET_EXT kint32_t fwk_alloc_chrdev(kuint32_t *devNum, kuint32_t baseminor, kuint32_t count, const kchar_t *name);
TARGET_EXT kint32_t fwk_register_chrdev(kuint32_t devNum, kuint32_t count, const kchar_t *name);
TARGET_EXT void fwk_unregister_chrdev(kuint32_t devNum, kuint32_t count);

#endif /*!< __FWK_CHRDEV_H_ */
