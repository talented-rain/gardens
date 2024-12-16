/*
 * Platform File System Controller
 *
 * File Name:   fwk_fcntl.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.05.28
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/fwk_fs.h>
#include <platform/fwk_kobj.h>
#include <platform/fwk_cdev.h>
#include <platform/fwk_inode.h>
#include <platform/fwk_fcntl.h>
#include <platform/net/fwk_netif.h>
#include <platform/net/fwk_socket.h>
#include <kernel/spinlock.h>

/*!< The globals */
static struct fwk_file_table sgrt_fwk_file_table =
{
    .max_fdarr	= 0,
    .max_fds	= FILE_DESC_NUM_MAX,
    .max_fdset	= -1,
    .ref_fdarr	= 0,

    .fds		= mrt_nullptr,
    .fd_array	= { mrt_nullptr },

    .sgrt_mutex	= MUTEX_LOCK_INIT(),
};

static struct fwk_file sgrt_fwk_file_stdio[DEVICE_MAJOR_BASE] =
{
    {
        .sprt_inode		= mrt_nullptr,
        .sprt_foprts	= mrt_nullptr,
    },
    {
        .sprt_inode		= mrt_nullptr,
        .sprt_foprts	= mrt_nullptr,
    },
    {
        .sprt_inode		= mrt_nullptr,
        .sprt_foprts	= mrt_nullptr,
    },
};

/*!< API function */
/*!
 * @brief   fwk_file_system_init
 * @param   none
 * @retval  none
 * @note    none
 */
kint32_t __plat_init fwk_file_system_init(void)
{
    struct fwk_file_table *sprt_table;
    kusize_t  num_farray;
    kuint32_t fileCnt;

    sprt_table = &sgrt_fwk_file_table;
    num_farray = ARRAY_SIZE(sprt_table->fd_array);

    if (!sprt_table->max_fds)
        sprt_table->max_fds	= num_farray;

    sprt_table->max_fdarr = mrt_ret_min2(sprt_table->max_fds, num_farray);
    num_farray = ARRAY_SIZE(sgrt_fwk_file_stdio);

    if (sprt_table->max_fdarr < num_farray)
        return -ER_MORE;

    /*!< Occupy the top three */
    for (fileCnt = 0; fileCnt < num_farray; fileCnt++)
        sprt_table->fd_array[fileCnt] = &sgrt_fwk_file_stdio[fileCnt];

    sprt_table->max_fds		= sprt_table->max_fdarr;
    sprt_table->ref_fdarr	= fileCnt;
    sprt_table->max_fdset 	= CMP_GT2(num_farray, 0, num_farray - 1, sprt_table->max_fdset);

    mutex_init(&sprt_table->sgrt_mutex);

    return ER_NORMAL;
}

/*!
 * @brief   fwk_get_expand_fdtable
 * @param   none
 * @retval  none
 * @note    none
 */
static kint32_t fwk_get_expand_fdtable(struct fwk_file_table *sprt_table, kuint32_t flags)
{
    kint32_t index;

    /*!< Created for the first time */
    if (!isValid(sprt_table->fds))
        sprt_table->fds	= (struct fwk_file **)kcalloc(sizeof(struct fwk_file), FILE_DESC_EXP_NUM, GFP_KERNEL);

    /*!< It still doesn't exist after it was created */
    if (!isValid(sprt_table->fds))
        goto fail;

    /*!< Fetch an unused index from fds */
    for (index = 0; index < FILE_DESC_EXP_NUM; index++)
    {
        if (!sprt_table->fds[index])
            goto fdget;
    }

    /*!< Not found */
    goto fail;

fdget:
    index += sprt_table->max_fdarr;
    sprt_table->max_fdset = mrt_ret_max2(sprt_table->max_fdset, index);
    sprt_table->max_fds	= sprt_table->max_fdarr	+ FILE_DESC_EXP_NUM;

    return index;

fail:
    return -ER_ERROR;
}

/*!
 * @brief   fwk_put_expand_fdtable
 * @param   none
 * @retval  none
 * @note    none
 */
static void fwk_put_expand_fdtable(struct fwk_file_table *sprt_table, kint32_t fd)
{
    kint32_t index;

    /*!< The array does not exist */
    if (!isValid(sprt_table->fds) || (fd < sprt_table->max_fdarr))
        return;

    index = fd - sprt_table->max_fdarr;
    sprt_table->fds[index] = mrt_nullptr;

    sprt_table->max_fdset = CMP_GTS(sprt_table->max_fdset, fd, fd - 1);

    /*!< The extended array has all been emptied. Free up space and save memory */
    if (!index)
    {
        kfree(sprt_table->fds);

        sprt_table->fds 	= mrt_nullptr;
        sprt_table->max_fds = sprt_table->max_fdarr;
    }
}

/*!
 * @brief   fwk_get_unused_fd_flags
 * @param   none
 * @retval  none
 * @note    none
 */
static kint32_t fwk_get_unused_fd_flags(kuint32_t flags)
{
    struct fwk_file_table *sprt_table;
    kint32_t index;

    sprt_table = &sgrt_fwk_file_table;

    /*!< Check if fd_array are fully assigned */
    index = fwk_get_fd_available(sprt_table);
    if (index < 0)
        goto expand;

    mutex_lock(&sprt_table->sgrt_mutex);

    /*!< Priority is given to backward allocation */
    index = CMP_LTS(sprt_table->max_fdset + 1, sprt_table->max_fdarr, -1);
    if (index >= 0)
    {
        sprt_table->max_fdset++;
        goto fdget;
    }

    /*!< Fetch an unused index from the file_array */
    for (index = 0; index < sprt_table->max_fdset; index++)
    {
        if (!sprt_table->fd_array[index])
            goto fdget;
    }

    mutex_unlock(&sprt_table->sgrt_mutex);

    /*!< Fail: fwk_get_fd_available check failed || RET_MIN_SUPER fail to get || for loop calls the overrun */
    return -ER_MORE;

fdget:
    fwk_fdtable_get(sprt_table);
    mutex_unlock(&sprt_table->sgrt_mutex);

    return index;

expand:
    /*!< fd has run out */
    index = fwk_get_expand_fdtable(sprt_table, flags);
    mutex_unlock(&sprt_table->sgrt_mutex);

    return index;
}

/*!
 * @brief   fwk_put_used_fd_flags
 * @param   none
 * @retval  none
 * @note    none
 */
static void fwk_put_used_fd_flags(kint32_t fd)
{
    struct fwk_file_table *sprt_table;
    kint32_t index;

    if (FILE_DESC_OVER_BASE(fd))
        return;

    sprt_table = &sgrt_fwk_file_table;
    index = fd;

    if (index >= sprt_table->max_fdarr)
        goto expand;

    mutex_lock(&sprt_table->sgrt_mutex);

    sprt_table->fd_array[index]	= mrt_nullptr;
    sprt_table->max_fdset = CMP_GTS(sprt_table->max_fdset, fd, fd - 1);
    fwk_fdtable_put(sprt_table);

    mutex_unlock(&sprt_table->sgrt_mutex);

    return;

expand:
    mutex_lock(&sprt_table->sgrt_mutex);
    /*!< fd has run out */
    fwk_put_expand_fdtable(sprt_table, fd);
    mutex_unlock(&sprt_table->sgrt_mutex);
}

/*!
 * @brief   fwk_fd_install
 * @param   none
 * @retval  none
 * @note    none
 */
static kint32_t fwk_fd_install(kint32_t fd, struct fwk_file *sprt_file)
{
    struct fwk_file_table *sprt_table;
    struct fwk_file **sprt_fdt;
    kint32_t index;

    if (FILE_DESC_OVER_BASE(fd) || !isValid(sprt_file))
        return -ER_UNVALID;

    sprt_table	= &sgrt_fwk_file_table;
    sprt_fdt	= sprt_table->fd_array;
    index		= fd;

    if (fd >= sprt_table->max_fdarr)
    {
        sprt_fdt = sprt_table->fds;
        index = fd - sprt_table->max_fdarr;
    }

    if (sprt_fdt[index])
        return -ER_UNVALID;

    mutex_lock(&sprt_table->sgrt_mutex);
    sprt_fdt[index] = sprt_file;
    mutex_unlock(&sprt_table->sgrt_mutex);

    return ER_NORMAL;
}

/*!
 * @brief   fwk_fd_to_file
 * @param   none
 * @retval  none
 * @note    none
 */
static struct fwk_file *fwk_fd_to_file(kint32_t fd)
{
    struct fwk_file_table *sprt_table;
    struct fwk_file  **sprt_fdt;
    kint32_t index;

    if (FILE_DESC_OVER_BASE(fd))
        return mrt_nullptr;

    sprt_table	= &sgrt_fwk_file_table;
    sprt_fdt	= sprt_table->fd_array;
    index		= fd;

    if (fd >= sprt_table->max_fdarr)
    {
        sprt_fdt = sprt_table->fds;
        index = fd - sprt_table->max_fdarr;
    }

    return sprt_fdt[index];
}

/*!
 * @brief   fwk_do_open
 * @param   none
 * @retval  none
 * @note    none
 */
static kint32_t fwk_do_open(const kchar_t *dev, kuint32_t mode)
{
    struct fwk_file  *sprt_file;
    kint32_t fd;
    kint32_t retval;

    fd = fwk_get_unused_fd_flags(mode);
    if (fd < 0)
        goto fail1;

    sprt_file = fwk_do_filp_open((kchar_t *)dev, mode);
    if (!isValid(sprt_file))
        goto fail2;

    retval = fwk_fd_install(fd, sprt_file);
    if (retval)
        goto fail3;

    return fd;

fail3:
    fwk_do_filp_close(sprt_file);
fail2:
    fwk_put_used_fd_flags(fd);
fail1:
    return -ER_FAULT;
}

/*!
 * @brief   fwk_do_close
 * @param   none
 * @retval  none
 * @note    none
 */
static void fwk_do_close(kint32_t fd)
{
    struct fwk_file *sprt_file;

    sprt_file = fwk_fd_to_file(fd);
    if (!isValid(sprt_file))
        return;

    fwk_do_filp_close(sprt_file);
    fwk_put_used_fd_flags(fd);
}

/*!
 * @brief   fwk_do_write
 * @param   none
 * @retval  none
 * @note    none
 */
static kssize_t fwk_do_write(kint32_t fd, const void *buf, kusize_t size)
{
    struct fwk_file *sprt_file;
    kint32_t retval;

    if (fd < 0)
        return -ER_ERROR;

    sprt_file = fwk_fd_to_file(fd);
    if (!isValid(sprt_file))
        return -ER_ERROR;

    if ((sprt_file->mode & O_WRONLY) != O_WRONLY)
        return -ER_FORBID;

    if (sprt_file->sprt_foprts->write)
    {
        retval = sprt_file->sprt_foprts->write(sprt_file, (const kbuffer_t *)buf, size);
        if (!retval)
            return size;
    }

    return -ER_ERROR;
}

/*!
 * @brief   fwk_do_read
 * @param   none
 * @retval  none
 * @note    none
 */
static kssize_t fwk_do_read(kint32_t fd, void *buf, kusize_t size)
{
    struct fwk_file *sprt_file;
    kint32_t retval;

    if (fd < 0)
        return -ER_ERROR;

    sprt_file = fwk_fd_to_file(fd);
    if (!isValid(sprt_file))
        return -ER_ERROR;

    if ((sprt_file->mode & O_RDONLY) != O_RDONLY)
        return -ER_FORBID;

    if (sprt_file->sprt_foprts->read)
    {
        retval = sprt_file->sprt_foprts->read(sprt_file, (kbuffer_t *)buf, size);
        if (!retval)
            return size;
        else if (retval > 0)
            return retval;
    }

    return -ER_ERROR;
}

/*!
 * @brief   fwk_do_ioctl
 * @param   none
 * @retval  none
 * @note    none
 */
static kint32_t fwk_do_ioctl(kint32_t fd, kuint32_t request, kuaddr_t args)
{
    struct fwk_file *sprt_file;
    kint32_t retval;

    if (fd < 0)
        return -ER_ERROR;

    sprt_file = fwk_fd_to_file(fd);
    if (!isValid(sprt_file))
        return -ER_ERROR;

    if (sprt_file->sprt_foprts->unlocked_ioctl)
    {
        retval = sprt_file->sprt_foprts->unlocked_ioctl(sprt_file, request, args);
        if (!retval)
            return ER_NORMAL;
    }

    return -ER_ERROR;
}

/*!
 * @brief   fwk_do_mmap
 * @param   none
 * @retval  none
 * @note    none
 */
static void *fwk_do_mmap(void *addr, kusize_t length, kint32_t prot, kint32_t flags, kint32_t fd, kuint32_t offset)
{
    struct fwk_vm_area sgrt_vm;
    struct fwk_file *sprt_file;
    kint32_t retval;

    if (fd < 0)
        return mrt_nullptr;

    sprt_file = fwk_fd_to_file(fd);
    if (!isValid(sprt_file))
        return mrt_nullptr;

    sgrt_vm.offset = offset;
    if (sprt_file->sprt_foprts->mmap)
    {
        retval = sprt_file->sprt_foprts->mmap(sprt_file, &sgrt_vm);
        if (retval)
            return mrt_nullptr;
    }

    if (length > sgrt_vm.size)
        return mrt_nullptr;
    
    if (!isValid(addr))
        return (void *)sgrt_vm.virt_addr;

    return mrt_nullptr;
}

/*!< ------------------------------------------------------------ */
/*!
 * @brief   virt_open
 * @param   none
 * @retval  none
 * @note    The interface is provided for use by the application layer
 */
kint32_t virt_open(const kchar_t *dev, kuint32_t mode)
{
    return fwk_do_open(dev, mode);
}

/*!
 * @brief   virt_close
 * @param   none
 * @retval  none
 * @note    The interface is provided for use by the application layer
 */
void virt_close(kint32_t fd)
{
    if (fd >= NETWORK_SOCKETS_BASE)
        network_close(fd);

    return fwk_do_close(fd);
}

/*!
 * @brief   virt_write
 * @param   none
 * @retval  none
 * @note    The interface is provided for use by the application layer
 */
kssize_t virt_write(kint32_t fd, const void *buf, kusize_t size)
{
    if (fd >= NETWORK_SOCKETS_BASE)
        return ER_NORMAL;

    return fwk_do_write(fd, buf, size);
}

/*!
 * @brief   virt_read
 * @param   none
 * @retval  none
 * @note    The interface is provided for use by the application layer
 */
kssize_t virt_read(kint32_t fd, void *buf, kusize_t size)
{
    if (fd >= NETWORK_SOCKETS_BASE)
        return ER_NORMAL;

    return fwk_do_read(fd, buf, size);
}

/*!
 * @brief   virt_ioctl
 * @param   none
 * @retval  none
 * @note    The interface is provided for use by the application layer
 */
kint32_t virt_ioctl(kint32_t fd, kuint32_t request, ...)
{
    va_list ptr_list;
    kuaddr_t args;
    kint32_t retval = ER_NORMAL;

    va_start(ptr_list, request);
    args = (kuaddr_t)va_arg(ptr_list, void *);

    if (fd >= NETWORK_SOCKETS_BASE)
        retval = fwk_netif_ioctl(request, args);
    else
        retval = fwk_do_ioctl(fd, request, args);

    va_end(ptr_list);
    return retval;
}

/*!
 * @brief   virt_mmap
 * @param   none
 * @retval  none
 * @note    The interface is provided for use by the application layer
 */
void *virt_mmap(void *addr, kusize_t length, kint32_t prot, kint32_t flags, kint32_t fd, kuint32_t offset)
{
    return fwk_do_mmap(addr, length, prot, flags, fd, offset);
}

/*!
 * @brief   virt_munmap
 * @param   none
 * @retval  none
 * @note    The interface is provided for use by the application layer
 */
kint32_t virt_munmap(void *addr, kusize_t length)
{
    return 0;
}

/*!< end of file */
