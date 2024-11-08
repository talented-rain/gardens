/*
 * General FatFs Interface
 *
 * File Name:   fs_fatfs.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.11.03
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

#include <platform/fwk_mempool.h>
#include <platform/fwk_inode.h>
#include <platform/fwk_fs.h>
#include <platform/block/fwk_gendisk.h>
#include <fs/fs_intr.h>

/*!< The defines */

/*!< The globals */

/*!< API function */
/*!
 * @brief   open file
 * @param   name, mode
 * @retval  file pointer
 * @note    open file (block device)
 */
struct fs_stream *file_open(const kchar_t *name, kuint32_t mode)
{
    struct fwk_inode *sprt_inode;
    struct fs_stream *sprt_fs;
    struct fwk_file sgrt_file;
    struct fwk_block_device *sprt_blkdev;
    kint32_t retval;

    sprt_inode = fwk_inode_find_disk(name);
    if (!isValid(sprt_inode))
        return ERR_PTR(-ER_NOTFOUND);

    sprt_fs = (struct fs_stream *)kzalloc(sizeof(*sprt_fs), GFP_KERNEL);
    if (!isValid(sprt_fs))
        return ERR_PTR(-ER_NOMEM);

    sprt_fs->full_name = (kchar_t *)name;
    sprt_fs->mode = mode;
    sprt_fs->sprt_dnode = sprt_inode;

    sgrt_file.sprt_foprts = sprt_inode->sprt_foprts;
    if (sgrt_file.sprt_foprts->open)
    {
        sgrt_file.mode = mode;
        retval = sgrt_file.sprt_foprts->open(sprt_inode, &sgrt_file);
        if (retval)
            goto fail1;
    }

    sprt_blkdev = sprt_inode->sprt_blkdev;
    sprt_fs->sprt_bops = sprt_blkdev->sprt_gdisk->sprt_bops;

    if (sprt_fs->sprt_bops->open)
    {
        retval = sprt_fs->sprt_bops->open(sprt_blkdev, sprt_fs);
        if (retval)
            goto fail2;
    }

    return sprt_fs;

fail2:
    if (sgrt_file.sprt_foprts->close)
        sgrt_file.sprt_foprts->close(sprt_inode, &sgrt_file);

fail1:
    kfree(sprt_fs);
    return ERR_PTR(-ER_FAILD);
}

/*!
 * @brief   close file
 * @param   sprt_fs
 * @retval  none
 * @note    close file (block device)
 */
void file_close(struct fs_stream *sprt_fs)
{
    struct fwk_inode *sprt_dnode;
    struct fwk_block_device *sprt_blkdev;
    struct fwk_file sgrt_file;
    kint32_t retval;

    sprt_dnode = sprt_fs->sprt_dnode;
    sprt_blkdev = sprt_dnode->sprt_blkdev;

    if (sprt_fs->sprt_bops->close)
    {
        retval = sprt_fs->sprt_bops->close(sprt_blkdev, sprt_fs);
        if (retval)
            return;
    }

    sgrt_file.sprt_foprts = sprt_dnode->sprt_foprts;

    if (sgrt_file.sprt_foprts->close)
        sgrt_file.sprt_foprts->close(sprt_dnode, &sgrt_file);

    kfree(sprt_fs);
}

/*!
 * @brief   write file
 * @param   sprt_fs, buf, size
 * @retval  size of data written
 * @note    write buf to sprt_fs
 */
kssize_t file_write(struct fs_stream *sprt_fs, const void *buf, kusize_t size)
{
    if (!sprt_fs->sprt_bops->write)
        return -ER_ERROR;

    return sprt_fs->sprt_bops->write(sprt_fs, buf, size, 0);
}

/*!
 * @brief   read file
 * @param   sprt_fs, buf, size
 * @retval  size of data read
 * @note    read buf from sprt_fs
 */
kssize_t file_read(struct fs_stream *sprt_fs, void *buf, kusize_t size)
{
    if (!sprt_fs->sprt_bops->read)
        return -ER_ERROR;

    return sprt_fs->sprt_bops->read(sprt_fs, buf, size, 0);
}

/*!
 * @brief   get file size
 * @param   sprt_fs
 * @retval  size of file data
 * @note    none
 */
kssize_t file_size(struct fs_stream *sprt_fs)
{
    if (!sprt_fs->sprt_bops->fsize)
        return -ER_ERROR;

    return sprt_fs->sprt_bops->fsize(sprt_fs);
}

/*!
 * @brief   locate to offset
 * @param   sprt_fs, offset
 * @retval  errno
 * @note    set offset
 */
kint32_t file_lseek(struct fs_stream *sprt_fs, kuint32_t offset)
{
    if (!sprt_fs->sprt_bops->lseek)
        return -ER_ERROR;

    return sprt_fs->sprt_bops->lseek(sprt_fs, offset);
}

/* end of file */