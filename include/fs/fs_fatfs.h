/*
 * General FatFs Interface
 *
 * File Name:   fs_fatfs.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.11.03
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FS_FATFS_H
#define __FS_FATFS_H

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The includes */
#include <common/generic.h>
#include <common/api_string.h>
#include <common/io_stream.h>
#include <platform/fwk_kobj.h>
#include <platform/block/fwk_blkdev.h>
#include <platform/block/fwk_gendisk.h>
#include <fatfs/ff.h>
#include <fatfs/diskio.h>

/*!< The defines */
#define FATFS_DISK_PATH_LEN                 (10)

/*!< for the whole disk */
typedef struct fatfs_disk
{
    kchar_t diskPath[FATFS_DISK_PATH_LEN];
    kuint8_t path_lenth;

    FATFS sgrt_fatfs;
    struct fwk_kobject *sprt_kobj;

    struct fwk_gendisk sgrt_gdisk;

    kuint16_t disk_number;
    kbool_t is_mounted;

    struct list_head sgrt_link;

} srt_fatfs_disk_t;

#define mrt_fatfs_disk_get(gdisk)   \
            mrt_container_of(gdisk, struct fatfs_disk, sgrt_gdisk)

/*!< The functions */
/*!< sd card */
extern DRESULT fs_sdfatfs_write(kuint8_t physicalDrive, 
                                const kuint8_t *buffer, kuint32_t sector, kuint8_t count);
extern DRESULT fs_sdfatfs_read(kuint8_t physicalDrive, 
                                kuint8_t *buffer, kuint32_t sector, kuint8_t count);
extern DRESULT fs_sdfatfs_ioctl(kuint8_t physicalDrive, kuint8_t command, void *buffer);
extern DSTATUS fs_sdfatfs_status(kuint8_t physicalDrive);
extern DSTATUS fs_sdfatfs_initial(kuint8_t physicalDrive);
extern DSTATUS fs_sdfatfs_release(kuint8_t physicalDrive);

extern struct fatfs_disk *fs_alloc_fatfs(kuint16_t number);
extern kint32_t fs_register_fatfs(struct fatfs_disk *sprt_fdisk);
extern void fs_unregister_fatfs(struct fatfs_disk *sprt_fdisk);

#ifdef __cplusplus
    }
#endif

#endif /* __FS_FATFS_H */
