/*
 * Platform File System Controller
 *
 * File Name:   fwk_fs.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.05.29
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_FS_H_
#define __FWK_FS_H_

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/fwk_inode.h>
#include <platform/fwk_uaccess.h>

/*!< The defines */
#define RET_INODE_FROM_FILE(file)								((file)->sprt_inode)
#define RET_PRIVATE_DRV_DATA(file)								((file)->private_data)

struct fwk_file
{
	kuint32_t mode;

	struct fwk_inode *sprt_inode;
	struct fwk_file_oprts *sprt_foprts;

	/*!< All open files form a list */
	struct list_head sgrt_list;

	void *private_data;
};

/*!< Device operation API */
struct fwk_file_oprts
{
	kint32_t (*open) (struct fwk_inode *, struct fwk_file *);
	kint32_t (*close) (struct fwk_inode *, struct fwk_file *);
	kssize_t (*write) (struct fwk_file *, const kbuffer_t *, kssize_t);
	kssize_t (*read) (struct fwk_file *, kbuffer_t *, kssize_t);
	kint32_t (*unlocked_ioctl) (struct fwk_file *, kuint32_t, kuaddr_t);
	kint32_t (*compat_ioctl) (struct fwk_file *, kuint32_t, kuaddr_t);
	kint32_t (*mmap) (struct fwk_file *, struct fwk_vm_area *);
};

/*!< The functions */
extern struct fwk_file *fwk_do_filp_open(kchar_t *name, kuint32_t mode);
extern void fwk_do_filp_close(struct fwk_file *sprt_file);

#ifdef __cplusplus
    }
#endif

#endif /*!< __FWK_FS_H_ */
