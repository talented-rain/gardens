/*
 * General SD Card FatFs Interface
 *
 * File Name:   fwk_sdfatfs.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.11.11
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#include <platform/fwk_mempool.h>
#include <rootfs/rfs_disk.h>
#include "ff.h"
#include "diskio.h"

/*!< The defines */
#define RFS_FATFS_DISK_MAX_SS								_MAX_SS

/*!< The globals */
static FATFS sgrt_rfs_fatfs = {0};

/*!< API function */
/*!
 * @brief   rfs_fatfs_disk_mount
 * @param   none
 * @retval  none
 * @note    mount sdcard disk by fatfs
 */
static kint32_t rfs_fatfs_disk_mount(struct rfs_disk *sprt_disk)
{
    /*!< Multiple mounts are not allowed */
    if (sprt_disk->mnt_status)
        return ER_NORMAL;

    /*!< FRESULT f_mount (FATFS* fs, const TCHAR* path, BYTE opt) */
    if (FR_OK != f_mount((FATFS *)sprt_disk->sprt_fs, (const TCHAR *)&sprt_disk->diskPath[0], 1))
        goto fail;

#if (_FS_RPATH >= 2U)
    
    if (FR_OK != f_chdrive((const TCHAR *)&sprt_disk->diskPath[0]))
        goto fail;
#endif

    sprt_disk->mnt_status = true;

    return ER_NORMAL;

fail:
    return -ER_UNVALID;
}

/*!
 * @brief   rfs_fatfs_disk_unmount
 * @param   none
 * @retval  none
 * @note    unmount sdcard disk by fatfs
 */
static kint32_t rfs_fatfs_disk_unmount(struct rfs_disk *sprt_disk)
{
    /*!< if not mounted ? */
    if (!sprt_disk->mnt_status)
        goto fail;

    /*!< FRESULT f_mount (FATFS* fs, const TCHAR* path, BYTE opt) */
    if (FR_OK != f_mount(mrt_nullptr, (const TCHAR *)&sprt_disk->diskPath[0], 0))
        goto fail;

    memset(sprt_disk->sprt_fs, 0, sizeof(FATFS));
    sprt_disk->mnt_status = false;

    return ER_NORMAL;

fail:
    return -ER_UNVALID;
}

/*!
 * @brief   rfs_fatfs_disk_mkfs
 * @param   none
 * @retval  none
 * @note    format sdcard disk by fatfs
 */
static kint32_t rfs_fatfs_disk_mkfs(struct rfs_disk *sprt_disk)
{
    kuint8_t work[RFS_FATFS_DISK_MAX_SS];

    if (!sprt_disk->mnt_status)
        goto fail;

    /*!< FRESULT f_mkfs (const TCHAR* path, BYTE opt, DWORD au, void* work, UINT len) */
    if (FR_OK != f_mkfs((const TCHAR *)&sprt_disk->diskPath[0], FM_FAT32, 0, (void *)work, sizeof(work)))
        goto fail;

    return ER_NORMAL;

fail:
    return -ER_UNVALID;
}

/*!
 * @brief   rfs_fatfs_disk_set_diskpath
 * @param   none
 * @retval  none
 * @note    set disk drive letter
 */
static void rfs_fatfs_disk_set_diskpath(struct rfs_disk *sprt_disk, kuint16_t drvNumber)
{
    sprintk(&sprt_disk->diskPath[0], "%d:/\0", drvNumber);
}

/*!
 * @brief   rfs_fatfs_disk_file_start
 * @param   none
 * @retval  none
 * @note    start sdcard disk detect
 */
static kint32_t rfs_fatfs_disk_file_start(struct rfs_disk_file *sprt_this, 
                                            const kchar_t *ptrName, kuint8_t flags, kuaddr_t bufferAddr, kuint32_t size)
{
    kint32_t retval;

    if (!isValid(sprt_this))
        return -ER_NULLPTR;

    /*!< check if disk is mounted */
    if (!sprt_this->is_mounted(sprt_this))
        return -ER_NREADY;

    if (!ptrName)
        return -ER_FAULT;

    sprt_this->set_fileName(sprt_this, ptrName);

    /*!< open file */
    retval = sprt_this->open(sprt_this, flags);
    if (retval)
        return -ER_FAILD;

    /*!< read file directely after openning file */
    sprt_this->fileSize	= sprt_this->fsize(sprt_this);
    if (sprt_this->fileSize > 0)
    {
        if (bufferAddr && (sprt_this->fileSize < size))
        {
            sprt_this->recvBuffer = (kuint8_t *)bufferAddr;
            sprt_this->isBufferDync = false;
        }
        else
        {
            sprt_this->recvBuffer = (kuint8_t *)kzalloc((sprt_this->fileSize + 4) * sizeof(kuint8_t), GFP_KERNEL);
            sprt_this->isBufferDync	= true;
        }
    }

    if (isValid(sprt_this->recvBuffer))
    {
        retval = sprt_this->read(sprt_this, (void *)sprt_this->recvBuffer, sprt_this->fileSize, 0);
        if (retval <= 0)
        {
            sprt_this->exit(sprt_this);
            return -ER_FAILD;
        }
    }

    sprt_this->close(sprt_this);

    return ER_NORMAL;
}

/*!
 * @brief   rfs_fatfs_disk_file_release
 * @param   none
 * @retval  none
 * @note    release sdcard disk
 */
static void rfs_fatfs_disk_file_release(struct rfs_disk_file *sprt_this)
{
    sprt_this->set_fileName(sprt_this, mrt_nullptr);
    if (sprt_this->ptrFilePath)
    {
        kfree(sprt_this->ptrFilePath);
        sprt_this->ptrFilePath = mrt_nullptr;
    }

    sprt_this->close(sprt_this);
    sprt_this->fileSize	= 0;

    if (isValid(sprt_this->recvBuffer) && sprt_this->isBufferDync)
    {
        kfree(sprt_this->recvBuffer);
        sprt_this->recvBuffer = mrt_nullptr;
    }

    sprt_this->isBufferDync	= false;
}

/*!
 * @brief   rfs_fatfs_disk_set_filepath
 * @param   none
 * @retval  none
 * @note    file name, and file path
 */
static void rfs_fatfs_disk_set_filepath(struct rfs_disk_file *sprt_this, const kchar_t *ptrFileName)
{
#if (IT_FALSE)
    kuint32_t iPathLenth, iNameOffset;

    iPathLenth = ptrFileName ? get_string_lenth(ptrFileName) : 0;
    if (!iPathLenth)
        return;

    iNameOffset = (kuint32_t)(seek_char_in_string(ptrFileName, '/') - ptrFileName);
    iPathLenth -= iNameOffset;

    if (sprt_this->ptrFilePath)
        kfree(sprt_this->ptrFilePath);

    /*!< 
     * for "123.txt",  iNameOffset = 6, iPathLenth = 1;
     * for "/123.txt", iNameOffset = 7, iPathLenth = 1
     * 
     * Whether it is "123.txt" or "/123.txt", *ptrFilePath is '/'
     */
    if (1 == iPathLenth)
        sprt_this->ptrFilePath = mrt_nullptr;
    else
    {
        sprt_this->ptrFilePath = (kuint8_t *)kmalloc(iPathLenth, GFP_KERNEL);
        if (isValid(sprt_this->ptrFilePath))
        {
            /*!<
            * for "abc/123.txt",     iNameOffset = 7, iPathLenth = 4;
            * for "xyz/abc/123.txt", iNameOffset = 7, iPathLenth = 8;
            */
            memcpy(sprt_this->ptrFilePath, ptrFileName, (iPathLenth - 1) * sizeof(kuint8_t));
            *(sprt_this->ptrFilePath + iPathLenth - 1) = '\0';
        }
    }

    if (!iNameOffset)
    {
        sprt_this->ptrFileName = mrt_nullptr;
        return;
    }

    /*!< 
     * for "123.txt",  iPathLenth = 0, sprt_this->ptrFileName = (kuint8_t *)ptrFileName;
     * for "/123.txt", iPathLenth = 1, sprt_this->ptrFileName = (kuint8_t *)ptrFileName + 1
     */
    if ((1 == iPathLenth) && ('/' != *ptrFileName))
        iPathLenth = 0;
    
    sprt_this->ptrFileName = ((kuint8_t *)ptrFileName) + iPathLenth;

#else
    sprt_this->ptrFileName = (kuint8_t *)ptrFileName;
    sprt_this->ptrFilePath = mrt_nullptr;
#endif
}

/*!
 * @brief   rfs_fatfs_disk_is_mounted
 * @param   none
 * @retval  none
 * @note    check is disk mounted
 */
static kbool_t rfs_fatfs_disk_is_mounted(struct rfs_disk_file *sprt_this)
{
    return (isValid(sprt_this->sprt_disk) ? sprt_this->sprt_disk->mnt_status : false);
}

/*!
 * @brief   rfs_fatfs_disk_file_is_opened
 * @param   none
 * @retval  none
 * @note    check is file opened
 */
static kbool_t rfs_fatfs_disk_file_is_opened(struct rfs_disk_file *sprt_this)
{
    return sprt_this->opened;
}

/*!
 * @brief   rfs_fatfs_disk_mkdir
 * @param   none
 * @retval  none
 * @note    create folders
 */
static kint32_t rfs_fatfs_disk_mkdir(const kchar_t *ptrDirName)
{
    FRESULT retval;

    retval = f_mkdir((const TCHAR *)ptrDirName);
    if (retval && (retval != FR_EXIST))
        return -ER_NSUPPORT;

    return ER_NORMAL;
}

/*!
 * @brief   rfs_fatfs_disk_open
 * @param   none
 * @retval  none
 * @note    open file which name is sprt_this->ptrFileName
 */
static kint32_t rfs_fatfs_disk_open(struct rfs_disk_file *sprt_this, kuint8_t flags)
{
    FIL *sprt_file;
    kuint8_t mode = 0;

    /*!< disk is not mounted */
    if (!sprt_this->is_mounted(sprt_this))
        return -ER_NULLPTR;

    if ((!sprt_this->ptrFileName) || isValid(sprt_this->sprt_file))
        goto fail1;

    /*!< struct FIL hope to satisfy x bytes alignment, it is not suggest to use "new" */
    sprt_file = (FIL *)kzalloc(sizeof(FIL), GFP_KERNEL);
    if (!isValid(sprt_file))
        goto fail1;

    /*!<
     * FA_OPEN_ALWAYS(0x10) has a bug: if file has exsited, it will clear all content
     * FA_OPEN_APPEND(0x30) include FA_OPEN_APPEND, it's bug is as same to FA_OPEN_ALWAYS
     */
    mode |= (NR_RFS_DISK_OpenExsiting  	& flags) ? FA_OPEN_EXISTING : 0;
    mode |= (NR_RFS_DISK_OpenCreate 	& flags) ? FA_CREATE_NEW 	: 0;
    mode |= (NR_RFS_DISK_OpenWrite  	& flags) ? FA_WRITE 		: 0;
    mode |= (NR_RFS_DISK_OpenRead  		& flags) ? FA_READ 			: 0;
    mode |= (NR_RFS_DISK_AppendEnd  	& flags) ? FA_OPEN_APPEND 	: 0;
    mode |= (NR_RFS_DISK_OpenClear  	& flags) ? FA_OPEN_ALWAYS 	: 0;

    /*!< FRESULT f_open (FIL* fp, const TCHAR* path, BYTE mode); */
    if (f_open(sprt_file, (const TCHAR *)sprt_this->ptrFileName, mode))
        goto fail2;

    sprt_this->opened = true;
    sprt_this->mode	= flags;
    sprt_this->sprt_file = sprt_file;

    return ER_NORMAL;

fail2:
    kfree(sprt_file);

fail1:
    return -ER_UNVALID;
}

/*!
 * @brief   rfs_fatfs_disk_lseek
 * @param   none
 * @retval  none
 * @note    file offset/seek
 */
static kint32_t rfs_fatfs_disk_lseek(struct rfs_disk_file *sprt_this, kuint32_t offset)
{
    if (!isValid(sprt_this->sprt_file))
        return -ER_NULLPTR;

    /*!< FRESULT f_lseek (FIL* fp, FSIZE_t ofs); */
    if (f_lseek((FIL *)sprt_this->sprt_file, (FSIZE_t)offset))
        return -ER_FAILD;

    return ER_NORMAL;
}

/*!
 * @brief   rfs_fatfs_disk_write
 * @param   none
 * @retval  none
 * @note    write data to disk
 */
static kusize_t rfs_fatfs_disk_write(struct rfs_disk_file *sprt_this, const void *ptrBuffer, kuint32_t size, kuint32_t offset)
{
    kuint32_t bytes_of_write = 0;

    if (!isValid(sprt_this->sprt_file))
        goto fail;

    if (mrt_isBitResetb(NR_RFS_DISK_OpenWrite, &sprt_this->mode))
        goto fail;

    f_lseek((FIL *)sprt_this->sprt_file, (FSIZE_t)offset);

    /*!< FRESULT f_write (FIL* fp, const void* buff, UINT btw, UINT* bw); */
    if (f_write((FIL *)sprt_this->sprt_file, ptrBuffer, (UINT)size, (UINT *)&bytes_of_write))
        goto fail;

    /*!< FRESULT f_sync (FIL* fp); */
    f_sync((FIL *)sprt_this->sprt_file);

    return (kusize_t)bytes_of_write;

fail:
    return 0;
}

/*!
 * @brief   rfs_fatfs_disk_read
 * @param   none
 * @retval  none
 * @note    read data from disk
 */
static kusize_t rfs_fatfs_disk_read(struct rfs_disk_file *sprt_this, void *ptrBuffer, kuint32_t size, kuint32_t offset)
{
    kuint32_t bytes_of_read = 0;

    if (!isValid(sprt_this->sprt_file))
        goto fail;

    if (mrt_isBitResetb(NR_RFS_DISK_OpenRead, &sprt_this->mode))
        goto fail;

    f_lseek((FIL *)sprt_this->sprt_file, (FSIZE_t)offset);

    /*!< FRESULT f_write (FIL* fp, const void* buff, UINT btw, UINT* bw); */
    if (f_read((FIL *)sprt_this->sprt_file, ptrBuffer, (UINT)size, (UINT *)&bytes_of_read))
        goto fail;

    return bytes_of_read;

fail:
    return 0;
}

/*!
 * @brief   rfs_fatfs_disk_get_filesize
 * @param   none
 * @retval  none
 * @note    get file size
 */
static kusize_t rfs_fatfs_disk_get_filesize(struct rfs_disk_file *sprt_this)
{
    FIL *sprt_file = (FIL *)sprt_this->sprt_file;

    if (!isValid(sprt_this->sprt_file))
        return 0;

    return f_size(sprt_file);
}

/*!
 * @brief   rfs_fatfs_disk_close
 * @param   none
 * @retval  none
 * @note    close file
 */
static kint32_t rfs_fatfs_disk_close(struct rfs_disk_file *sprt_this)
{
    /*!< no file is opened */
    if (!sprt_this->is_opened(sprt_this))
        return -ER_NREADY;

    if (!isValid(sprt_this->sprt_file))
        return -ER_NULLPTR;

    /* FRESULT f_close (FIL* fp); */
    if (f_close((FIL *)sprt_this->sprt_file))
        return -ER_FAILD;

    sprt_this->opened = false;
    sprt_this->mode	= 0;

    kfree(sprt_this->sprt_file);
    sprt_this->sprt_file = mrt_nullptr;

    return ER_NORMAL;
}

/*!< important property, directelly accessing is not allowed */
static struct rfs_disk sgrt_rfs_fatfs_disk =
{
    .set_diskPath	= rfs_fatfs_disk_set_diskpath,
    .mount 			= rfs_fatfs_disk_mount,
    .unmount 		= rfs_fatfs_disk_unmount,
    .mkfs 			= rfs_fatfs_disk_mkfs,
    .mkdir          = rfs_fatfs_disk_mkdir,

    .sprt_fs		= &sgrt_rfs_fatfs,
    .diskPath		= {0},
    .mnt_status		= false,
};

/*!< sdcard disk interface */
static struct rfs_disk_file sgrt_rfs_fatfs_file =
{
    .init			= rfs_fatfs_disk_file_start,
    .exit			= rfs_fatfs_disk_file_release,

    .set_fileName 	= rfs_fatfs_disk_set_filepath,

    .is_mounted 	= rfs_fatfs_disk_is_mounted,
    .is_opened 		= rfs_fatfs_disk_file_is_opened,

    .open 			= rfs_fatfs_disk_open,
    .lseek 			= rfs_fatfs_disk_lseek,
    .write 			= rfs_fatfs_disk_write,
    .read 			= rfs_fatfs_disk_read,
    .fsize 			= rfs_fatfs_disk_get_filesize,
    .close 			= rfs_fatfs_disk_close,

    .sprt_disk		= &sgrt_rfs_fatfs_disk,

    .fileSize		= 0,
    .recvBuffer		= mrt_nullptr,
    .sprt_file		= mrt_nullptr,
    .mode			= 0,
    .opened			= false,
    .isBufferDync   = false,
    .isSelfDync     = false,
};

/*!
 * @brief   rfs_fatfs_file_initial
 * @param   none
 * @retval  none
 * @note    disk interface initial
 */
void rfs_fatfs_file_initial(struct rfs_disk_file *sprt_file)
{
    if (isValid(sprt_file))
        memcpy(sprt_file, &sgrt_rfs_fatfs_file, sizeof(struct rfs_disk_file));
}

/*!
 * @brief   rfs_fatfs_file_allocate
 * @param   none
 * @retval  none
 * @note    disk interface allocate
 */
void *rfs_fatfs_file_allocate(void)
{
    struct rfs_disk_file *sprt_file;

    sprt_file = (struct rfs_disk_file *)kmalloc(sizeof(struct rfs_disk_file), GFP_KERNEL);
    if (isValid(sprt_file))
    {
        rfs_fatfs_file_initial(sprt_file);
        sprt_file->isSelfDync = true;
    }

    return sprt_file;
}

/*!
 * @brief   rfs_fatfs_file_free
 * @param   none
 * @retval  none
 * @note    disk interface free
 */
void rfs_fatfs_file_free(struct rfs_disk_file *sprt_file)
{
    if (isValid(sprt_file) && sprt_file->isSelfDync)
        kfree(sprt_file);
}

/*!
 * @brief   rfs_fatfs_disk_create
 * @param   none
 * @retval  none
 * @note    disk create
 */
kint32_t rfs_fatfs_disk_create(struct rfs_disk *sprt_disk, kuint16_t drvNumber)
{
    if (!sprt_disk->mount)
        return -ER_UNVALID;

    /*!< register disk drive number before mountint disk */
    sprt_disk->set_diskPath(sprt_disk, drvNumber);

    /*!< mount SD Card disk */
    return sprt_disk->mount(sprt_disk);
}

/*!
 * @brief   rfs_fatfs_disk_destroy
 * @param   none
 * @retval  none
 * @note    disk release
 */
void rfs_fatfs_disk_destroy(struct rfs_disk *sprt_disk)
{
    if (!sprt_disk->unmount)
        return;

    /*!< unmount SD Card disk */
    if (!sprt_disk->unmount(sprt_disk))
    {
        /*!< unregister disk drive number after unmounting */
        memset(&sprt_disk->diskPath[0], 0, sizeof(sprt_disk->diskPath));
    }
}

/*!< end of file */
