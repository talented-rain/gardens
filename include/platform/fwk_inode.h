/*
 * Platform File System: Inode
 *
 * File Name:   fwk_inode.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.05.29
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_INODE_H_
#define __FWK_INODE_H_

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/fwk_fs.h>
#include <platform/fwk_cdev.h>
#include <platform/block/fwk_gendisk.h>
#include <platform/fwk_kobj.h>

/*!< The defines */
#define RET_INODE_MAJOR(inode)							GET_DEV_MAJOR((inode)->r_dev)
#define RET_INODE_MINOR(inode)							GET_DEV_MINOR((inode)->r_dev)

#define INODE_TYPE_FILE									(0)
#define INODE_TYPE_DIR									(1)

struct fwk_inode
{
	kchar_t *name;
	kuint8_t type;
	kuint32_t r_dev;

	struct fwk_file_oprts *sprt_foprts;
	struct fwk_kobject *sprt_kobj;

	union
	{
		struct fwk_cdev *sprt_cdev;
		struct fwk_block_device *sprt_blkdev;
	};
};

#define FWK_PATH_ROOT									"/"
#define FWK_PATH_DEVICE									"/dev/"
#define FWK_PATH_SYSTEM									"/sys/"

#define FWK_PATH_CHARDEV								FWK_PATH_DEVICE"char/"
#define FWK_PATH_BLOCKDEV								FWK_PATH_DEVICE"block/"

// #define FWK_PATH_SYS_DEVICE							FWK_PATH_SYSTEM"/devices/"
#define FWK_PATH_SYS_DEVICE								FWK_PATH_SYSTEM
#define FWK_PATH_SYS_DRIVER								FWK_PATH_SYSTEM"drivers/"

/*!< The functions */
extern kint32_t fwk_inode_set_ops(struct fwk_inode *sprt_inode, kuint32_t type, kint32_t devNum);
extern struct fwk_inode *fwk_mk_inode(struct fwk_kobject *sprt_kobj, kuint32_t type, kint32_t devNum);
extern void fwk_rm_inode(struct fwk_inode *sprt_inode);
extern struct fwk_inode *fwk_inode_find(kchar_t *name);
extern struct fwk_inode *fwk_inode_find_disk(const kchar_t *name);

#ifdef __cplusplus
    }
#endif

#endif /*!< __FWK_INODE_H_ */
