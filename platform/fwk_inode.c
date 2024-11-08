/*
 * Platform File System: Inode
 *
 * File Name:   fwk_inode.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.05.29
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/fwk_inode.h>

/*!< API function */
/*!
 * @brief   fwk_chrdev_open
 * @param   none
 * @retval  none
 * @note    none
 */
static kint32_t fwk_chrdev_open(struct fwk_inode *sprt_inode, struct fwk_file *sprt_file)
{
    struct fwk_cdev *sprt_cdev;
    kuint32_t devNum;

    devNum = sprt_inode->r_dev;
/*
    if (devNum < DEVICE_MAJOR_BASE)
        goto fail;
*/

    sprt_cdev = (struct fwk_cdev *)fwk_kobjmap_lookup(sprt_fwk_chrdev_map, devNum);
    if (!isValid(sprt_cdev))
        goto fail;

    sprt_inode->sprt_cdev = sprt_cdev;

    /*!< Replace the device operate function */
    sprt_file->sprt_foprts = sprt_cdev->sprt_oprts;
    if (sprt_file->sprt_foprts->open)
        return sprt_file->sprt_foprts->open(sprt_inode, sprt_file);

    return ER_NORMAL;

fail:
    return -ER_UNVALID;
}

/*!
 * @brief   fwk_chrdev_close
 * @param   none
 * @retval  none
 * @note    none
 */
static kint32_t fwk_chrdev_close(struct fwk_inode *sprt_inode, struct fwk_file *sprt_file)
{
    sprt_inode->sprt_cdev = mrt_nullptr;

    return ER_NORMAL;
}

static struct fwk_file_oprts sgrt_fwk_inode_def_chrfoprts =
{
    .open	= fwk_chrdev_open,
    .close	= fwk_chrdev_close,
};

/*!
 * @brief   fwk_blkdev_open
 * @param   none
 * @retval  none
 * @note    none
 */
static kint32_t fwk_blkdev_open(struct fwk_inode *sprt_inode, struct fwk_file *sprt_file)
{
    struct fwk_block_device *sprt_blkdev;
    kuint32_t devNum;

    devNum = sprt_inode->r_dev;

    sprt_blkdev = (struct fwk_block_device *)fwk_kobjmap_lookup(sprt_fwk_blkdev_map, devNum);
    if (!isValid(sprt_blkdev))
        goto fail;

    sprt_inode->sprt_blkdev = sprt_blkdev;
    sprt_blkdev->sprt_inode = sprt_inode;

    return ER_NORMAL;

fail:
    return -ER_UNVALID;
}

/*!
 * @brief   fwk_blkdev_close
 * @param   none
 * @retval  none
 * @note    none
 */
static kint32_t fwk_blkdev_close(struct fwk_inode *sprt_inode, struct fwk_file *sprt_file)
{
    struct fwk_block_device *sprt_blkdev;

    sprt_blkdev = sprt_inode->sprt_blkdev;
    sprt_blkdev->sprt_inode = mrt_nullptr;
    sprt_inode->sprt_blkdev = mrt_nullptr;

    return ER_NORMAL;
}

static struct fwk_file_oprts sgrt_fwk_inode_def_blkfoprts =
{
    .open	= fwk_blkdev_open,
    .close	= fwk_blkdev_close,
};

/*!
 * @brief   fwk_netdev_open
 * @param   none
 * @retval  none
 * @note    none
 */
static kint32_t fwk_netdev_open(struct fwk_inode *sprt_inode, struct fwk_file *sprt_file)
{
    return ER_NORMAL;
}

/*!
 * @brief   fwk_netdev_close
 * @param   none
 * @retval  none
 * @note    none
 */
static kint32_t fwk_netdev_close(struct fwk_inode *sprt_inode, struct fwk_file *sprt_file)
{
    return ER_NORMAL;
}

static struct fwk_file_oprts sgrt_fwk_inode_def_netfoprts =
{
    .open	= fwk_netdev_open,
    .close	= fwk_netdev_close,
};

/*!
 * @brief   set the operation functions to inode
 * @param   sprt_node, type, devNum
 * @retval  errno
 * @note    none
 */
kint32_t fwk_inode_set_ops(struct fwk_inode *sprt_inode, kuint32_t type, kint32_t devNum)
{
    if (!sprt_inode)
        return -ER_NOMEM;

    sprt_inode->r_dev = devNum;

    switch(type)
    {
        case NR_TYPE_CHRDEV:
            sprt_inode->sprt_foprts	= &sgrt_fwk_inode_def_chrfoprts;
            break;

        case NR_TYPE_BLKDEV:
            sprt_inode->sprt_foprts	= &sgrt_fwk_inode_def_blkfoprts;
            break;

        case NR_TYPE_NETDEV:
            sprt_inode->sprt_foprts	= &sgrt_fwk_inode_def_netfoprts;
            break;

        default:
            break;
    };

    return ER_NORMAL;
}

/*!
 * @brief   fwk_mk_inode
 * @param   none
 * @retval  none
 * @note    none
 */
struct fwk_inode *fwk_mk_inode(struct fwk_kobject *sprt_kobj, kuint32_t type, kint32_t devNum)
{
    struct fwk_inode *sprt_inode;

    if (!sprt_kobj)
        return ERR_PTR(-ER_ERROR);

    sprt_inode = (struct fwk_inode *)kzalloc(sizeof(struct fwk_inode), GFP_KERNEL);
    if (!isValid(sprt_inode))
        return ERR_PTR(-ER_NOMEM);

    sprt_inode->name = sprt_kobj->name;
    sprt_inode->type = sprt_kobj->is_dir ? INODE_TYPE_DIR : INODE_TYPE_FILE;
    sprt_inode->sprt_kobj = sprt_kobj;

    fwk_inode_set_ops(sprt_inode, type, devNum);

    return sprt_inode;
}

/*!
 * @brief   fwk_rm_inode
 * @param   none
 * @retval  none
 * @note    none
 */
void fwk_rm_inode(struct fwk_inode *sprt_inode)
{
    if (!sprt_inode)
        return;

    /*!< Free up resources */
    kfree(sprt_inode);
}

/*!
 * @brief   fwk_inode_find
 * @param   none
 * @retval  none
 * @note    none
 */
struct fwk_inode *fwk_inode_find(kchar_t *name)
{
    struct fwk_kobject *sprt_kobj;

    if (!name || !(*name))
        return mrt_nullptr;

    sprt_kobj = fwk_find_kobject_by_path(mrt_nullptr, name);
    if (!isValid(sprt_kobj))
        return ERR_PTR(-ER_NOTFOUND);

    return sprt_kobj->sprt_inode;
}

/*!
 * @brief   fwk_inode_find
 * @param   none
 * @retval  none
 * @note    none
 */
struct fwk_inode *fwk_inode_find_disk(const kchar_t *name)
{
    struct fwk_kobject *sprt_kobj = mrt_nullptr;
    kchar_t *disk_name;
    kuint32_t lenth, mark = 0;

    if (!name || (*name != '/'))
        return mrt_nullptr;

    lenth = strlen(name);
    if (*(name + lenth - 1) == '/')
        return mrt_nullptr;

    disk_name = (kchar_t *)kmalloc(lenth + 1, GFP_KERNEL);
    if (!isValid(disk_name))
        return mrt_nullptr;

    strcpy(disk_name, name);

    while (lenth > 1)
    {
        lenth--;
        if (*(disk_name + lenth - 1) != '/')
        {
            mark = 0;
            continue;
        }

        /*!< if '/' appears 2 times continually, it is unvalid */
        if (mark)
            break;
        
        mark = 1;
        *(disk_name + lenth) = '\0';

        sprt_kobj = fwk_find_kobject_by_path(sprt_kobj, disk_name);
        if (!isValid(sprt_kobj))
            continue;

        if (sprt_kobj->is_disk)
            goto succ;
    }

    sprt_kobj = mrt_nullptr;

succ:
    kfree(disk_name);
    return (isValid(sprt_kobj) ? sprt_kobj->sprt_inode : mrt_nullptr);
}

/*!< end of file */
