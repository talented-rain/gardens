/*
 * Platform File System Controller
 *
 * File Name:   fwk_fs.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.05.29
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <platform/fwk_fs.h>

/*!< API function */
/*!
 * @brief   fwk_do_filp_open
 * @param   none
 * @retval  none
 * @note    none
 */
struct fwk_file *fwk_do_filp_open(kchar_t *name, kuint32_t mode)
{
    struct fwk_file  *sprt_file;
    struct fwk_inode *sprt_inode;
    kint32_t retval;

    sprt_inode = fwk_inode_find(name);
    if (!isValid(sprt_inode))
        goto fail1;
    
    sprt_file = (struct fwk_file *)kzalloc(sizeof(struct fwk_file), GFP_KERNEL);
    if (!isValid(sprt_file))
        goto fail1;

    sprt_file->mode = mode;
    sprt_file->sprt_inode = sprt_inode;
    sprt_file->sprt_foprts = sprt_inode->sprt_foprts;
    if (sprt_file->sprt_foprts->open)
    {
        retval = sprt_file->sprt_foprts->open(sprt_inode, sprt_file);
        if (retval)
            goto fail2;
    }

    return sprt_file;

fail2:
    kfree(sprt_file);

fail1:
    return mrt_nullptr;
}

/*!
 * @brief   fwk_do_filp_close
 * @param   none
 * @retval  none
 * @note    none
 */
void fwk_do_filp_close(struct fwk_file *sprt_file)
{
    struct fwk_inode *sprt_inode;

    if (!isValid(sprt_file))
        return;

    sprt_inode = sprt_file->sprt_inode;

    /*!< Close device */
    if (sprt_file->sprt_foprts->close)
        sprt_file->sprt_foprts->close(sprt_inode, sprt_file);

    /*!< 
     * Legacy bug: 
     * If the file is not closed after opening, and the inode node is suddenly deleted, 
     * what should we do with the file? How to release the occupied fd? 
     */
    if (sprt_inode->sprt_foprts->close)
        sprt_inode->sprt_foprts->close(sprt_inode, sprt_file);

    kfree(sprt_file);
}

/*!< end of file */
