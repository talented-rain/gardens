/*
 * Platform File System Controller
 *
 * File Name:   fwk_fcntl.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.05.28
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_FCNTL_H_
#define __FWK_FCNTL_H_

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/fwk_fs.h>
#include <kernel/mutex.h>

/*!< The defines */
/*!< The maximum number of default file descriptors that can be assigned */
#define FILE_DESC_NUM_MAX									(32)

/*!< The maximum number of extension file descriptors that can be assigned */
#define FILE_DESC_EXP_NUM									(128)

/*!< socket descriptors base */
#define NETWORK_SOCKETS_BASE                        		(FILE_DESC_NUM_MAX + FILE_DESC_EXP_NUM)

#define FILE_DESC_OVER_BASE(fd)								(fd < DEVICE_MAJOR_BASE)

struct fwk_file_table
{
	kint32_t max_fdarr;										/*!< size of fd_array */
	kint32_t max_fds; 										/*!< The maximum number of current file objects = max_fdarr + array number of fds */
	kint32_t max_fdset;										/*!< The maximum number of current file descriptors */
	kint32_t ref_fdarr; 									/*!< The number of descriptors that have been assigned on fd_array */

	struct fwk_file **fds;
	struct fwk_file *fd_array[FILE_DESC_NUM_MAX];

	struct mutex_lock sgrt_mutex;
};

/*!< for open mode */
#define O_ACCMODE											00000004
#define O_RDONLY											00000001
#define O_WRONLY											00000002
#define O_RDWR												00000003

/*!< not fcntl */
#ifndef O_CREAT
#define O_CREAT												00000100
#endif

/*!< not fcntl */
#ifndef O_EXCL
#define O_EXCL												00000200
#endif

/*!< not fcntl */
#ifndef O_NOCTTY
#define O_NOCTTY											00000400
#endif

/*!< not fcntl */
#ifndef O_TRUNC
#define O_TRUNC												00001000
#endif

#ifndef O_APPEND
#define O_APPEND											00002000
#endif

#ifndef O_NONBLOCK
#define O_NONBLOCK											00004000
#endif

/*!< used to be O_SYNC, see below */
#ifndef O_DSYNC
#define O_DSYNC												00010000
#endif

/*!< fcntl, for BSD compatibility */
#ifndef FASYNC
#define FASYNC												00020000
#endif

/*!< direct disk access hint */
#ifndef O_DIRECT
#define O_DIRECT											00040000
#endif

#ifndef O_LARGEFILE
#define O_LARGEFILE											00100000
#endif

/*!< must be a directory */
#ifndef O_DIRECTORY
#define O_DIRECTORY											00200000
#endif

/*!< don't follow links */
#ifndef O_NOFOLLOW
#define O_NOFOLLOW											00400000
#endif

#ifndef O_NOATIME
#define O_NOATIME											01000000
#endif

/*!< set close_on_exec */
#ifndef O_CLOEXEC
#define O_CLOEXEC											02000000
#endif

#ifndef O_PATH
#define O_PATH												010000000
#endif

#ifndef O_NDELAY
#define O_NDELAY											O_NONBLOCK
#endif

#define F_DUPFD												0
#define F_GETFD												1
#define F_SETFD												2
#define F_GETFL												3
#define F_SETFL												4

#ifndef F_GETLK
#define F_GETLK												5
#define F_SETLK												6
#define F_SETLKW											7
#endif

#ifndef F_SETOWN
#define F_SETOWN											8
#define F_GETOWN											9
#endif

#ifndef F_SETSIG
#define F_SETSIG											10
#define F_GETSIG											11
#endif

/*!< The functions */
/*!< -------------------------------------------------------------- */
TARGET_EXT kint32_t fwk_file_system_init(void);

/*!< -------------------------------------------------------------- */
TARGET_EXT kint32_t virt_open(const kchar_t *dev, kuint32_t mode);
TARGET_EXT void virt_close(kint32_t fd);
TARGET_EXT kssize_t virt_write(kint32_t fd, const void *buf, kusize_t size);
TARGET_EXT kssize_t virt_read(kint32_t fd, void *buf, kusize_t size);
TARGET_EXT kssize_t virt_ioctl(kint32_t fd, kuint32_t request, ...);
TARGET_EXT void *virt_mmap(void *addr, kusize_t length, kint32_t prot, kint32_t flags, kint32_t fd, kuint32_t offset);
TARGET_EXT kint32_t virt_munmap(void *addr, kusize_t length);

/*!< API function */
/*!
 * @brief   Get avaliable index
 * @param   none
 * @retval  none
 * @note    none
 */
static inline kint32_t fwk_get_fd_available(struct fwk_file_table *sprt_table)
{
	return (sprt_table->ref_fdarr < sprt_table->max_fdarr) ? (sprt_table->ref_fdarr) : (-1);
}

/*!
 * @brief   fwk_fdtable_get
 * @param   none
 * @retval  none
 * @note    none
 */
static inline void fwk_fdtable_get(struct fwk_file_table *sprt_table)
{
	sprt_table->ref_fdarr = (sprt_table->ref_fdarr < sprt_table->max_fdarr) ? (sprt_table->ref_fdarr + 1) : sprt_table->max_fdarr;
}

/*!
 * @brief   fwk_fdtable_put
 * @param   none
 * @retval  none
 * @note    none
 */
static inline void fwk_fdtable_put(struct fwk_file_table *sprt_table)
{
	sprt_table->ref_fdarr = (sprt_table->ref_fdarr > DEVICE_MAJOR_BASE) ? (sprt_table->ref_fdarr - 1) : DEVICE_MAJOR_BASE;
}

#endif /*!< __FWK_FCNTL_H_ */
