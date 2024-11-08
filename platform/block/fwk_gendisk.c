/*
 * Block Device Interface
 *
 * File Name:   fwk_blkdevice.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.11.03
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/fwk_inode.h>
#include <platform/fwk_fs.h>
#include <platform/block/fwk_gendisk.h>
#include <platform/fwk_platdrv.h>

/*!< The defines */

/*!< The globals */

/*!< API function */
/*!
 * @brief   gendisk initial
 * @param   sprt_gdisk, sprt_oprts
 * @retval  errno
 * @note    none
 */
kint32_t fwk_gendisk_init(struct fwk_gendisk *sprt_gdisk, const struct fwk_block_device_oprts *sprt_oprts)
{
    if (!isValid(sprt_gdisk))
        return -ER_NOMEM;

    memset(sprt_gdisk, 0, sizeof(*sprt_gdisk));
    sprt_gdisk->sprt_bops = (struct fwk_block_device_oprts *)sprt_oprts;

    return ER_NORMAL;
}

/*!
 * @brief   gendisk alloc
 * @param   sprt_oprts
 * @retval  errno
 * @note    none
 */
struct fwk_gendisk *fwk_alloc_gendisk(kint32_t minors, const struct fwk_block_device_oprts *sprt_oprts)
{
    struct fwk_gendisk *sprt_gdisk;

    sprt_gdisk = (struct fwk_gendisk *)kzalloc(sizeof(*sprt_gdisk), GFP_KERNEL);
    if (!isValid(sprt_gdisk))
        return ERR_PTR(-ER_FAILD);

    if (fwk_gendisk_init(sprt_gdisk, sprt_oprts))
    {
        kfree(sprt_gdisk);
        return ERR_PTR(-ER_FAILD);
    }

    sprt_gdisk->minors = minors;
    return sprt_gdisk;
}

/*!
 * @brief   gendisk add
 * @param   sprt_gdisk
 * @retval  errno
 * @note    none
 */
kint32_t fwk_add_gendisk(struct fwk_gendisk *sprt_gdisk)
{
    struct fwk_block_device *sprt_blkdev;
    struct fwk_device *sprt_dev;
    kuint32_t devNum;
    kint32_t retval;

	if (!isValid(sprt_gdisk))
		return -ER_NOMEM;

    sprt_blkdev = (struct fwk_block_device *)kzalloc(sizeof(*sprt_blkdev), GFP_KERNEL);
    if (!isValid(sprt_blkdev))
        return -ER_FAILD;

    sprt_blkdev->sprt_gdisk = sprt_gdisk;
    sprt_blkdev->major = sprt_gdisk->major;
    sprt_gdisk->sprt_blkdev = sprt_blkdev;

	devNum = MKE_DEV_NUM(sprt_gdisk->major, sprt_gdisk->first_minor);
	retval = fwk_kobj_map(sprt_fwk_blkdev_map, devNum, sprt_gdisk->minors, sprt_blkdev);
    if (retval)
        goto fail1;

    if (sprt_gdisk->mount)
    {
        retval = sprt_gdisk->mount(sprt_gdisk);
        if (retval)
            goto fail2;

        return ER_NORMAL;
    }

    sprt_dev = fwk_device_create(NR_TYPE_BLKDEV, devNum, sprt_gdisk->disk_name);
    if (!isValid(sprt_dev))
        goto fail2;

    sprt_gdisk->sprt_devfs = sprt_dev;
    return ER_NORMAL;

fail2:
    fwk_kobj_unmap(sprt_fwk_blkdev_map, devNum, sprt_gdisk->minors);
fail1:
    kfree(sprt_blkdev);
    sprt_gdisk->sprt_blkdev = mrt_nullptr;

    return -ER_FAILD;
}

/*!
 * @brief   gendisk delete
 * @param   sprt_gdisk
 * @retval  errno
 * @note    none
 */
kint32_t fwk_del_gendisk(struct fwk_gendisk *sprt_gdisk)
{
    kuint32_t devNum;

	if (!isValid(sprt_gdisk))
		return -ER_NOMEM;

    devNum = MKE_DEV_NUM(sprt_gdisk->major, sprt_gdisk->first_minor);
	fwk_kobj_unmap(sprt_fwk_blkdev_map, devNum, sprt_gdisk->minors);

    if (sprt_gdisk->unmount)
        sprt_gdisk->unmount(sprt_gdisk);
    else
    {
        fwk_device_destroy(sprt_gdisk->sprt_devfs);
        sprt_gdisk->sprt_devfs = mrt_nullptr;
    }

    kfree(sprt_gdisk->sprt_blkdev);
    sprt_gdisk->sprt_blkdev = mrt_nullptr;

	return ER_NORMAL;
}

/* end of file */
