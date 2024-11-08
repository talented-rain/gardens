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
#include <platform/mmc/fwk_sdcard.h>
#include <fs/fs_fatfs.h>

/*!< The globals */
static struct fwk_sdcard sgrt_fwk_sddisk;
static struct fatfs_disk *sprt_fatfs_sddisk;

/*!< API function */
/*!
 * @brief   fs_sdfatfs_write
 * @param   none
 * @retval  none
 * @note    write SD Card by Fatfs
 */
DRESULT fs_sdfatfs_write(kuint8_t physicalDrive, const kuint8_t *buffer, kuint32_t sector, kuint8_t count)
{
    if (physicalDrive != SDDISK)
        return RES_PARERR;

    if (!fwk_sdcard_rw_blocks(&sgrt_fwk_sddisk, (void *)buffer, sector, count, NR_SdCard_WriteToCard))
        return RES_ERROR;

    return RES_OK;
}

/*!
 * @brief   fs_sdfatfs_read
 * @param   none
 * @retval  none
 * @note    read SD Card by Fatfs
 */
DRESULT fs_sdfatfs_read(kuint8_t physicalDrive, kuint8_t *buffer, kuint32_t sector, kuint8_t count)
{
    if (physicalDrive != SDDISK)
        return RES_PARERR;

    if (!fwk_sdcard_rw_blocks(&sgrt_fwk_sddisk, buffer, sector, count, NR_SdCard_ReadToHost))
        return RES_ERROR;

    return RES_OK;
}

/*!
 * @brief   fs_sdfatfs_ioctl
 * @param   none
 * @retval  none
 * @note    control SD Card by Fatfs
 */
DRESULT fs_sdfatfs_ioctl(kuint8_t physicalDrive, kuint8_t command, void *buffer)
{
    DRESULT result = RES_OK;

    if (physicalDrive != SDDISK)
        return RES_PARERR;

    switch (command)
    {
        case GET_SECTOR_COUNT:
            if (buffer)
                *(kuint32_t *)buffer = sgrt_fwk_sddisk.blockCount;
            else
                result = RES_PARERR;
            break;

        case GET_SECTOR_SIZE:
            if (buffer)
                *(kuint32_t *)buffer = sgrt_fwk_sddisk.blockSize;
            else
                result = RES_PARERR;
            break;

        case GET_BLOCK_SIZE:
            if (buffer)
                *(kuint32_t *)buffer = sgrt_fwk_sddisk.sgrt_csd.eraseSectorSize;
            else
                result = RES_PARERR;
            break;

        case CTRL_SYNC:
            result = RES_OK;
            break;
        default:
            result = RES_PARERR;
            break;
    }

    return result;
}

/*!
 * @brief   fs_sdfatfs_status
 * @param   none
 * @retval  none
 * @note    check if SD Card
 */
DSTATUS fs_sdfatfs_status(kuint8_t physicalDrive)
{
    if (physicalDrive != SDDISK)
        return STA_NOINIT;

	if (mrt_isBitResetl(NR_SdCard_Transfer_State, &sgrt_fwk_sddisk.mode))
		return STA_NOINIT;

    return 0;
}

/*!
 * @brief   fs_sdfatfs_initial
 * @param   none
 * @retval  none
 * @note    initial SD Card by Fatfs
 */
DSTATUS fs_sdfatfs_initial(kuint8_t physicalDrive)
{
    struct fwk_sdcard *sprt_card;
    kint32_t iRetval;

    if (physicalDrive != SDDISK)
        return STA_NOINIT;

    /*!< allocate sdcard structure or get host */
    sprt_card = fwk_sdcard_allocate_device(&sgrt_fwk_sddisk);
    if (!isValid(sprt_card))
        return STA_NOINIT;

    /*!< detect and initial */
    iRetval = fwk_sdcard_initial_device(sprt_card);
    if (iRetval)
    {
       fwk_sdcard_free_device(sprt_card);
       return STA_NOINIT;
    }

    return 0;
}

/*!
 * @brief   fs_sdfatfs_release
 * @param   none
 * @retval  none
 * @note    release SD Card by Fatfs
 */
DSTATUS fs_sdfatfs_release(kuint8_t physicalDrive)
{
    if (physicalDrive != SDDISK)
        return STA_NOINIT;

	fwk_sdcard_inactive_device(&sgrt_fwk_sddisk);

	return 0;
}

/*!< ------------------------------------------------------------- */
/*!
 * @brief   fs_sdfatfs_init
 * @param   none
 * @retval  none
 * @note    sd fatfs init
 */
kint32_t __plat_init fs_sdfatfs_init(void)
{
    struct fatfs_disk *sprt_fdisk;
    struct fwk_sdcard *sprt_card;
    kint32_t retval = 0;

    sprt_fdisk = fs_alloc_fatfs(SDDISK);
    if (!isValid(sprt_fdisk))
        return -ER_NOMEM;

    retval = fs_register_fatfs(sprt_fdisk);
    if (retval)
    {
        sprt_card = &sgrt_fwk_sddisk;
        if (!sprt_card->sgrt_if.sprt_host)
            goto fail;

        if (fwk_sdcard_is_insert(sprt_card))
        {
            print_err("sd card detected, but initialize fatfs failed!\n");
            goto fail;
        }

        print_warn("sd card not detected, please insert your card\n");
        return ER_NORMAL;
    }

    sprt_fatfs_sddisk = sprt_fdisk;

    print_info("sd card detected, and initialize fatfs successfully!\n");
    return ER_NORMAL;

fail:
    kfree(sprt_fdisk);
    return retval;
}

/*!
 * @brief   fs_sdfatfs_exit
 * @param   none
 * @retval  none
 * @note    sd fatfs exit
 */
void __plat_exit fs_sdfatfs_exit(void)
{
    struct fatfs_disk *sprt_fdisk;

    sprt_fdisk = sprt_fatfs_sddisk;
    sprt_fatfs_sddisk = mrt_nullptr;

    fs_unregister_fatfs(sprt_fdisk);
    sprt_fdisk = mrt_nullptr;
}

IMPORT_ROOTFS_INIT(fs_sdfatfs_init);
IMPORT_ROOTFS_EXIT(fs_sdfatfs_exit);

/*!< end of file */
