/*
 * Platform File System: Block Device
 *
 * File Name:   fwk_blkdev.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.11.03
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_BLKDEV_H
#define __FWK_BLKDEV_H

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/fwk_platdev.h>

struct fwk_inode;
struct fwk_gendisk;

/*!< The defines */
struct fwk_block_device
{
    kchar_t name[DEVICE_NAME_LEN];
    kuint32_t major;

    struct fwk_inode *sprt_inode;
    struct fwk_gendisk *sprt_gdisk;
};

/*!< The globals */

/*!< The functions */
extern kint32_t fwk_register_blkdev(kuint32_t major, const kchar_t *name);
extern void fwk_unregister_blkdev(kuint32_t major, const kchar_t *name);

#ifdef __cplusplus
    }
#endif

#endif /* __FWK_BLKDEV_H */
