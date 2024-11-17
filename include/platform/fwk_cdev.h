/*
 * Character Device Interface
 *
 * File Name:   fwk_cdev.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.05.18
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_CDEV_H_
#define __FWK_CDEV_H_

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/fwk_fs.h>
#include <platform/fwk_kobj.h>

/*!< The defines */
struct fwk_cdev
{
    kuint32_t devNum;
    kuint32_t count;

    struct fwk_file_oprts *sprt_oprts;
    struct fwk_cdev *sprt_next;

    void *privData;
};

/*!< The functions */
TARGET_EXT kint32_t fwk_cdev_init(struct fwk_cdev *sprt_cdev, const struct fwk_file_oprts *sprt_oprts);
TARGET_EXT struct fwk_cdev *fwk_cdev_alloc(const struct fwk_file_oprts *sprt_oprts);
TARGET_EXT kint32_t fwk_cdev_add(struct fwk_cdev *sprt_cdev, kuint32_t devNum, kuint32_t count);
TARGET_EXT kint32_t fwk_cdev_del(struct fwk_cdev *sprt_cdev);

#endif /*!< __FWK_CDEV_H_ */
