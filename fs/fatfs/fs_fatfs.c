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
#include <platform/fwk_fcntl.h>
#include <platform/mmc/fwk_sdcard.h>
#include <fs/fs_intr.h>
#include <fs/fs_fatfs.h>

/*!< The defines */
#define FATFS_DISK_MAX_SS								_MAX_SS

#define FATFS_BLK_DEVICE_MAJOR                          10
#define FATFS_BLK_DEVICE_MINOR                          0

/*!< The globals */
static DECLARE_LIST_HEAD(sgrt_fatfs_drvList);

/*!< The functions */
static kint32_t fatfs_disk_mount(struct fwk_gendisk *sprt_gdisk);
static kint32_t fatfs_disk_unmount(struct fwk_gendisk *sprt_gdisk);
static kint32_t fatfs_disk_format(struct fwk_gendisk *sprt_gdisk);
static kint32_t fatfs_disk_mkdir(struct fwk_gendisk *sprt_gdisk, const kchar_t *dir_name);

/*!< API function */
/*!
 * @brief   mount fatfs
 * @param   sprt_gdisk
 * @retval  errno
 * @note    mount disk by fatfs
 */
static kint32_t fatfs_disk_mount(struct fwk_gendisk *sprt_gdisk)
{
    struct fatfs_disk *sprt_fdisk;
    struct fwk_kobject *sprt_kobj;
    kchar_t name[FATFS_DISK_PATH_LEN * 2] = {0};
    kuint8_t lenth;

    sprt_fdisk = mrt_fatfs_disk_get(sprt_gdisk);

    /*!< Multiple mounts are not allowed */
    if (sprt_fdisk->is_mounted)
        return -ER_EXISTED;

    if (!sprt_fdisk->diskPath[0])
        return -ER_UNVALID;

    /*!< FRESULT f_mount (FATFS* fs, const TCHAR* path, BYTE opt) */
    if (FR_OK != f_mount(&sprt_fdisk->sgrt_fatfs, (const TCHAR *)sprt_fdisk->diskPath, 1))
        return -ER_FAILD;

#if (_FS_RPATH >= 2U)
    if (FR_OK != f_chdrive((const TCHAR *)sprt_fdisk->diskPath))
        goto fail;
#endif

    print_info("mount fatfs disk \"%s\" successfully\n", sprt_fdisk->diskPath);

    lenth = sprintk(name, "/media/FAT32_%d/\0", sprt_fdisk->disk_number);
    sprt_kobj = fwk_kobject_populate(mrt_nullptr, (const kchar_t *)name);
    if (!isValid(sprt_kobj))
    {
        if (PTR_ERR(sprt_kobj) != (-ER_FORBID))
        {
            print_err("populate disk path \"%s\" failed!\n", name);
            goto fail;
        }

        sprt_fdisk->path_lenth = 0;
    }
    else
    {
        fwk_inode_set_ops(sprt_kobj->sprt_inode, NR_TYPE_BLKDEV, 
                        MKE_DEV_NUM(sprt_gdisk->major, sprt_gdisk->first_minor));

        sprt_kobj->is_disk = true;
        sprt_fdisk->sprt_kobj = sprt_kobj;
        fwk_kobject_get(sprt_kobj);

        sprt_fdisk->path_lenth = lenth;
    }

    sprt_fdisk->is_mounted = true;
    return ER_NORMAL;

fail:
    fatfs_disk_unmount(sprt_gdisk);
    return -ER_FAILD;
}

/*!
 * @brief   unmount fatfs
 * @param   sprt_gdisk
 * @retval  errno
 * @note    unmount disk by fatfs
 */
static kint32_t fatfs_disk_unmount(struct fwk_gendisk *sprt_gdisk)
{
    struct fatfs_disk *sprt_fdisk;

    sprt_fdisk = mrt_fatfs_disk_get(sprt_gdisk);
    if (!sprt_fdisk->is_mounted)
        return ER_NORMAL;

    /*!< FRESULT f_mount (FATFS* fs, const TCHAR* path, BYTE opt) */
    if (FR_OK != f_mount(mrt_nullptr, sprt_fdisk->diskPath, 0))
    {
        print_err("unmount fatfs disk \"%s\" failed!\n", sprt_fdisk->diskPath);
        return -ER_FAILD;
    }

    if (sprt_fdisk->sprt_kobj)
        fwk_kset_kobject_remove(sprt_fdisk->sprt_kobj);

    sprt_fdisk->is_mounted = false;
    sprt_fdisk->path_lenth = 0;

    memset(&sprt_fdisk->sgrt_fatfs, 0, sizeof(sprt_fdisk->sgrt_fatfs));
    print_info("unmount fatfs disk \"%s\"\n", sprt_fdisk->diskPath);

    return ER_NORMAL;
}

/*!
 * @brief   format disk
 * @param   sprt_gdisk
 * @retval  errno
 * @note    format and clear disk
 */
static kint32_t fatfs_disk_format(struct fwk_gendisk *sprt_gdisk)
{
    struct fatfs_disk *sprt_fdisk;
    kuint8_t work[FATFS_DISK_MAX_SS];

    sprt_fdisk = mrt_fatfs_disk_get(sprt_gdisk);
    if (sprt_fdisk->is_mounted)
        fatfs_disk_unmount(sprt_gdisk);

    /*!< FRESULT f_mkfs (const TCHAR* path, BYTE opt, DWORD au, void* work, UINT len) */
    if (FR_OK != f_mkfs(sprt_fdisk->diskPath, FM_FAT32, 0, (void *)work, sizeof(work)))
        return -ER_FAILD;

    return fatfs_disk_mount(sprt_gdisk);
}

/*!
 * @brief   create directory
 * @param   sprt_gdisk, dir_name (directory name)
 * @retval  errno
 * @note    mkdir
 */
static kint32_t fatfs_disk_mkdir(struct fwk_gendisk *sprt_gdisk, const kchar_t *dir_name)
{
    struct fatfs_disk *sprt_fdisk;
    FRESULT retval;

    sprt_fdisk = mrt_fatfs_disk_get(sprt_gdisk);
    if (!sprt_fdisk->is_mounted)
        return -ER_NREADY;
    
    retval = f_mkdir(dir_name);
    if (retval && (retval != FR_EXIST))
        return -ER_FAILD;

    return ER_NORMAL;
}

/*!
 * @brief   delete directory
 * @param   sprt_gdisk, dir_name (directory name)
 * @retval  errno
 * @note    rmdir
 */
static kint32_t fatfs_disk_rmdir(struct fwk_gendisk *sprt_gdisk, const kchar_t *dir_name)
{
    struct fatfs_disk *sprt_fdisk;
    FRESULT retval;

    sprt_fdisk = mrt_fatfs_disk_get(sprt_gdisk);
    if (!sprt_fdisk->is_mounted)
        return -ER_NREADY;
    
    retval = f_unlink(dir_name);
    if (retval != FR_OK)
        return -ER_FAILD;

    return ER_NORMAL;
}

/*!
 * @brief   open file in disk
 * @param   sprt_blkdev, sprt_file
 * @retval  errno
 * @note    open file and return file pointer
 */
static kint32_t fatfs_file_open(struct fwk_block_device *sprt_blkdev, struct fs_stream *sprt_file)
{
    struct fatfs_disk *sprt_fdisk;
    struct fwk_gendisk *sprt_gdisk;
    FIL *sprt_fp;
    kchar_t *name;
    kuint8_t mode = 0;

    sprt_gdisk = sprt_blkdev->sprt_gdisk;
    sprt_fdisk = mrt_fatfs_disk_get(sprt_gdisk);

    name = kstrcat(sprt_file->full_name, sprt_fdisk->path_lenth);
    if ((*name == '\0') ||
        ((*name == '/' ) && (*(name + 1) == '\0')))
        return -ER_UNVALID;
    
    /*!< struct FIL hope to satisfy x bytes alignment, it is not suggest to use "new" */
    sprt_fp = (FIL *)kzalloc(sizeof(FIL), GFP_KERNEL);
    if (!isValid(sprt_fp))
        return -ER_NOMEM;

    /*!<
     * FA_OPEN_ALWAYS(0x10) has a bug: if file has exsited, it will clear all content
     * FA_OPEN_APPEND(0x30) include FA_OPEN_APPEND, it's bug is as same to FA_OPEN_ALWAYS
     */
    mode |= (sprt_file->mode & O_CREAT ) ? FA_CREATE_NEW    : 0;
    mode |= (sprt_file->mode & O_WRONLY) ? FA_WRITE         : 0;
    mode |= (sprt_file->mode & O_RDONLY) ? FA_READ          : 0;
    mode |= (sprt_file->mode & O_APPEND) ? FA_OPEN_APPEND   : 0;
    mode |= (sprt_file->mode & O_EXCL  ) ? FA_OPEN_ALWAYS   : 0;
    if (!mode)
        mode = FA_OPEN_EXISTING;

    /*!< FRESULT f_open (FIL* fp, const TCHAR* path, BYTE mode); */
    if (f_open(sprt_fp, (const TCHAR *)name, mode))
        goto fail;

    sprt_file->private_data = sprt_fp;
    return ER_NORMAL;

fail:
    kfree(sprt_fp);
    return -ER_FAILD;
}

/*!
 * @brief   close file
 * @param   sprt_blkdev, sprt_file
 * @retval  errno
 * @note    close file and release file pointer
 */
static kint32_t fatfs_file_close(struct fwk_block_device *sprt_blkdev, struct fs_stream *sprt_file)
{
    FIL *sprt_fp;

    sprt_fp = (FIL *)sprt_file->private_data;
    if (!sprt_fp)
        return ER_NORMAL;

    /* FRESULT f_close (FIL* fp); */
    if (f_close(sprt_fp))
        return -ER_FAILD;

    kfree(sprt_fp);
    sprt_file->private_data = mrt_nullptr;

    return ER_NORMAL;
}

/*!
 * @brief   locate file offset
 * @param   sprt_file, offset
 * @retval  errno
 * @note    set file pointer add offset
 */
static kint32_t fatfs_file_lssek(struct fs_stream *sprt_file, kuint32_t offset)
{
    FIL *sprt_fp;

    sprt_fp = (FIL *)sprt_file->private_data;
    if (!sprt_fp)
        return -ER_NODEV;

    /*!< FRESULT f_lseek (FIL* fp, FSIZE_t ofs); */
    if (f_lseek(sprt_fp, (FSIZE_t)offset))
        return -ER_FAILD;

    return ER_NORMAL;
}

/*!
 * @brief   write data to disk
 * @param   sprt_file, buffer, size, offset
 * @retval  errno
 * @note    write file
 */
static kssize_t fatfs_file_write(struct fs_stream *sprt_file, const void *buffer, kuint32_t size, kuint32_t offset)
{
    FIL *sprt_fp;
    kusize_t bytes = 0;

    sprt_fp = (FIL *)sprt_file->private_data;
    if (!sprt_fp)
        return -ER_NODEV;

    if (!(sprt_file->mode & O_WRONLY))
        return -ER_FORBID;

    if (offset)
        f_lseek(sprt_fp, (FSIZE_t)offset);

    /*!< FRESULT f_write (FIL* fp, const void* buff, UINT btw, UINT* bw); */
    if (f_write(sprt_fp, buffer, (UINT)size, (UINT *)&bytes))
        return -ER_FAILD;

    /*!< FRESULT f_sync (FIL* fp); */
    f_sync(sprt_fp);
    return bytes;
}

/*!
 * @brief   read data from disk
 * @param   sprt_file, buffer, size, offset
 * @retval  errno
 * @note    read file
 */
static kssize_t fatfs_file_read(struct fs_stream *sprt_file, void *buffer, kuint32_t size, kuint32_t offset)
{
    FIL *sprt_fp;
    kusize_t bytes = 0;

    sprt_fp = (FIL *)sprt_file->private_data;
    if (!sprt_fp)
        return -ER_NODEV;

    if (!(sprt_file->mode & O_RDONLY))
        return -ER_FORBID;

    if (offset)
        f_lseek(sprt_fp, (FSIZE_t)offset);

    /*!< FRESULT f_write (FIL* fp, const void* buff, UINT btw, UINT* bw); */
    if (f_read(sprt_fp, buffer, (UINT)size, (UINT *)&bytes))
        return -ER_FAILD;

    return bytes;
}

/*!
 * @brief   get file size
 * @param   sprt_file
 * @retval  size
 * @note    none
 */
static kssize_t fatfs_file_get_size(struct fs_stream *sprt_file)
{
    FIL *sprt_fp;

    sprt_fp = (FIL *)sprt_file->private_data;
    if (!sprt_fp)
        return -ER_NODEV;

    return f_size(sprt_fp);
}

/*!
 * @brief   get file read/write pointer
 * @param   sprt_file
 * @retval  size
 * @note    none
 */
static kssize_t fatfs_file_tell(struct fs_stream *sprt_file)
{
    FIL *sprt_fp;

    sprt_fp = (FIL *)sprt_file->private_data;
    if (!sprt_fp)
        return -ER_NODEV;

    return f_tell(sprt_fp);
}

/*!< fatfs operations */
static const struct fwk_block_device_oprts sgrt_fatfs_bops =
{  
    .open   = fatfs_file_open,
    .close  = fatfs_file_close,
    .write  = fatfs_file_write,
    .read   = fatfs_file_read,
    .lseek  = fatfs_file_lssek,
    .fsize  = fatfs_file_get_size,
    .fpos   = fatfs_file_tell,
};

/*!
 * @brief   fatfs create and initial
 * @param   number: disk type
 * @retval  errno
 * @note    register block device and gendisk
 */
struct fatfs_disk *fs_alloc_fatfs(kuint16_t number)
{
    struct fatfs_disk *sprt_fdisk;
    struct fwk_gendisk *sprt_gdisk;

    sprt_fdisk = (struct fatfs_disk *)kzalloc(sizeof(*sprt_fdisk), GFP_KERNEL);
    if (!isValid(sprt_fdisk))
        return ERR_PTR(-ER_NOMEM);

    sprt_gdisk = &sprt_fdisk->sgrt_gdisk;
    fwk_gendisk_init(sprt_gdisk, &sgrt_fatfs_bops);

    sprintk(sprt_fdisk->diskPath, "%d:/\0", number);
    sprt_fdisk->disk_number = number;

    sprt_gdisk->mount = fatfs_disk_mount;
    sprt_gdisk->unmount = fatfs_disk_unmount;
    sprt_gdisk->mkfs = fatfs_disk_format;
    sprt_gdisk->mkdir = fatfs_disk_mkdir;
    sprt_gdisk->rmdir = fatfs_disk_rmdir;

    return sprt_fdisk;
}

/*!
 * @brief   fatfs create and initial
 * @param   number: disk type
 * @retval  errno
 * @note    register block device and gendisk
 */
kint32_t fs_register_fatfs(struct fatfs_disk *sprt_fdisk)
{
    struct fwk_gendisk *sprt_gdisk;
    kint32_t retval;

    if (!isValid(sprt_fdisk))
        return -ER_NODEV;
    
    sprt_gdisk = &sprt_fdisk->sgrt_gdisk;

    sprt_gdisk->major = FATFS_BLK_DEVICE_MAJOR;
    sprt_gdisk->first_minor = sprt_fdisk->disk_number;
    sprt_gdisk->minors = 1;

    retval = fwk_add_gendisk(sprt_gdisk);
    if (retval)
    {
        print_err("add new gendisk mmc \"%s\" failed\n", sprt_fdisk->diskPath);
        return -ER_FAILD;
    }

    list_head_add_tail(&sgrt_fatfs_drvList, &sprt_fdisk->sgrt_link);

    return ER_NORMAL;
}

/*!
 * @brief   fatfs create and initial
 * @param   number: disk type
 * @retval  errno
 * @note    register block device and gendisk
 */
void fs_unregister_fatfs(struct fatfs_disk *sprt_fdisk)
{
    fwk_del_gendisk(&sprt_fdisk->sgrt_gdisk);
    list_head_del(&sprt_fdisk->sgrt_link);
}

/*!< ------------------------------------------------------------- */
/*!
 * @brief   fs_fatfs_init
 * @param   none
 * @retval  none
 * @note    fatfs init
 */
kint32_t __plat_init fs_fatfs_init(void)
{
    kint32_t retval;

    retval = fwk_register_blkdev(FATFS_BLK_DEVICE_MAJOR, "mmc");
    if (retval)
    {
        print_err("register mmc fatfs block device failed!\n");
        return retval;
    }

    print_info("register mmc fatfs block device successfully\n");
    return ER_NORMAL;
}

/*!
 * @brief   fs_fatfs_exit
 * @param   none
 * @retval  none
 * @note    fatfs exit
 */
void __plat_exit fs_fatfs_exit(void)
{
    struct fatfs_disk *sprt_fdisk, *sprt_temp;

    fwk_unregister_blkdev(FATFS_BLK_DEVICE_MAJOR, "mmc");
    print_info("unregistered mmc fatfs block device\n");

    foreach_list_next_entry_safe(sprt_fdisk, sprt_temp, &sgrt_fatfs_drvList, sgrt_link)
        fs_unregister_fatfs(sprt_fdisk);
}

IMPORT_ROOTFS_INIT(fs_fatfs_init);
IMPORT_ROOTFS_EXIT(fs_fatfs_exit);
