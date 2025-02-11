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
#include <kernel/mutex.h>

/*!< The defines */
struct fwk_block_major_name
{
    kchar_t name[DEVICE_NAME_LEN];
    kint32_t major;
};

/*!< The globals */
struct fwk_block_major_name *sgrt_block_major_name[DEVICE_MAX_NUM];
static struct mutex_lock sgrt_blkdev_mutex = MUTEX_LOCK_INIT();

/*!< API function */
/*!
 * @brief   register block device
 * @param   major, name
 * @retval  errno
 * @note    none
 */
static struct fwk_block_major_name *__fwk_register_blkdev(kuint32_t major, const kchar_t *name)
{
    struct fwk_block_major_name *sprt_blkdev;
    struct fwk_block_major_name **sprt_Dst;
    kuint32_t index;
    kusize_t blkdevMax;
    kint32_t i;

    blkdevMax = ARRAY_SIZE(sgrt_block_major_name);

    if (!major)
    {
        sprt_Dst = &sgrt_block_major_name[blkdevMax - 1];

        /*!< 
         * The composite value of the Major + Minor must be less than 2^32, 
         * that is, the number of primary device numbers and secondary device numbers is limited 
         */
        for (i = (blkdevMax - 1); i >= 0; i--)
        {
            if (!(*(sprt_Dst--)))
                break;
        }

        if (i < 0)
            return ERR_PTR(-ER_EXISTED);

        major = i;
    }

    if (major >= blkdevMax)
        return ERR_PTR(-ER_UNVALID);

    index = major % blkdevMax;
    sprt_Dst = &sgrt_block_major_name[index];

    /*!< If sprt_Temp[index] is empty, sprt_blkdev can be deposited directly */
    if (*sprt_Dst)
        return ERR_PTR(-ER_EXISTED);

    sprt_blkdev = (struct fwk_block_major_name *)kmalloc(sizeof(*sprt_blkdev), GFP_KERNEL);
    if (!isValid(sprt_blkdev))
        return ERR_PTR(-ER_FAILD);

    sprt_blkdev->major = major;
    strncpy(sprt_blkdev->name, (const char *)name, sizeof(sprt_blkdev->name));
    *sprt_Dst = sprt_blkdev;

    return sprt_blkdev;
}

/*!
 * @brief   unregister block device
 * @param   major, name
 * @retval  errno
 * @note    none
 */
static struct fwk_block_major_name *__fwk_unregister_blkdev(kuint32_t major, const kchar_t *name)
{
    struct fwk_block_major_name *sprt_Rlt;
    struct fwk_block_major_name **sprt_Dst;
    kuint32_t index;
    kusize_t  blkdevMax;

    blkdevMax = ARRAY_SIZE(sgrt_block_major_name);
    if (major >= blkdevMax)
        return ERR_PTR(-ER_UNVALID);

    index = major % blkdevMax;
    sprt_Dst = &sgrt_block_major_name[index];
    if (!(*sprt_Dst))
        return ERR_PTR(-ER_EMPTY);

    sprt_Rlt = *sprt_Dst;
    if (strcmp(sprt_Rlt->name, name))
        return ERR_PTR(-ER_CHECKERR);

    *sprt_Dst = mrt_nullptr;

    return sprt_Rlt;
}

/*!
 * @brief   register block device
 * @param   major, name
 * @retval  errno
 * @note    none
 */
kint32_t fwk_register_blkdev(kuint32_t major, const kchar_t *name)
{
    struct fwk_block_major_name *sprt_blkdev;

    mutex_lock(&sgrt_blkdev_mutex);

    sprt_blkdev = __fwk_register_blkdev(major, name);
    if (!isValid(sprt_blkdev))
    {
        mutex_unlock(&sgrt_blkdev_mutex);
        print_err("register block device \"%s\" failed!\n", name);

        return -ER_FAILD;
    }

    mutex_unlock(&sgrt_blkdev_mutex);
    return ER_NORMAL;
}

/*!
 * @brief   unregister block device
 * @param   major, name
 * @retval  errno
 * @note    none
 */
void fwk_unregister_blkdev(kuint32_t major, const kchar_t *name)
{
    mutex_lock(&sgrt_blkdev_mutex);
    __fwk_unregister_blkdev(major, name);
    mutex_unlock(&sgrt_blkdev_mutex);
}

/* end of file */
