/*
 * Platform File System: GenDisk
 *
 * File Name:   fwk_gendisk.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.11.03
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_GENDISK_H_
#define __FWK_GENDISK_H_

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/fwk_platdev.h>
#include <platform/block/fwk_blkdev.h>

struct fs_stream;

/*!< The defines */
typedef struct fwk_gendisk
{
    kint32_t major;
    kint32_t first_minor;
    kint32_t minors;
    kchar_t disk_name[DEVICE_NAME_LEN];

    kint32_t (*mount)(struct fwk_gendisk *sprt_disk);
    kint32_t (*unmount)(struct fwk_gendisk *sprt_disk);
    kint32_t (*mkfs)(struct fwk_gendisk *sprt_disk);
    kint32_t (*mkdir)(struct fwk_gendisk *sprt_disk, const kchar_t *dir_name);
    kint32_t (*rmdir)(struct fwk_gendisk *sprt_disk, const kchar_t *dir_name);

    struct fwk_device *sprt_devfs;
    struct fwk_block_device_oprts *sprt_bops;

    struct fwk_block_device *sprt_blkdev;

} srt_fwk_gendisk_t;

struct fwk_block_device_oprts
{
    kint32_t (*open) (struct fwk_block_device *sprt_blkdev, struct fs_stream *sprt_file);
    kint32_t (*close) (struct fwk_block_device *sprt_blkdev, struct fs_stream *sprt_file);

    kssize_t (*write) (struct fs_stream *sprt_file, const void *buffer, kuint32_t size, kuint32_t offset);
    kssize_t (*read) (struct fs_stream *sprt_file, void *buffer, kuint32_t size, kuint32_t offset);
    kint32_t (*lseek) (struct fs_stream *sprt_file, kuint32_t offset);
    kssize_t (*fsize) (struct fs_stream *sprt_file);
    kssize_t (*fpos) (struct fs_stream *sprt_file);
};

/*!< The globals */

/*!< The functions */
extern kint32_t fwk_gendisk_init(struct fwk_gendisk *sprt_gdisk, 
                                            const struct fwk_block_device_oprts *sprt_oprts);
extern struct fwk_gendisk *fwk_alloc_gendisk(kint32_t minors, 
                                            const struct fwk_block_device_oprts *sprt_oprts);
extern kint32_t fwk_add_gendisk(struct fwk_gendisk *sprt_gdisk);
extern kint32_t fwk_del_gendisk(struct fwk_gendisk *sprt_gdisk);

#ifdef __cplusplus
    }
#endif

#endif /* __FWK_GENDISK_H_ */
