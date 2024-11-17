/*
 * IMX6ULL Board General Initial
 *
 * File Name:   board_r.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.09.10
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes*/
#include <boot/board_init.h>
#include <platform/fwk_fcntl.h>
#include <fs/fs_fatfs.h>
#include <fs/fs_intr.h>

/*!< The defines */
struct boot_images
{
    kuaddr_t os_entry;

    struct global_data *sprt_gd;
    struct tag_params *sprt_params;
    struct m_area sgrt_fdt;

    struct fatfs_disk *sprt_fdisk;
};

/*!< The globals */
struct boot_images sgrt_boot_images;

/*!< API function */
/*!
 * @brief   system_boot_initial
 * @param   none
 * @retval  none
 * @note    initial heap and stack
 */
kint32_t system_boot_initial(void)
{
#if CONFIG_STACK_WITH_LDS
    _SVC_MODE_STACK_BASE = SVC_MODE_STACK_BASE;
    _SYS_MODE_STACK_BASE = SYS_MODE_STACK_BASE;
    _FIQ_MODE_STACK_BASE = FIQ_MODE_STACK_BASE;
    _IRQ_MODE_STACK_BASE = IRQ_MODE_STACK_BASE;
    _ABT_MODE_STACK_BASE = ABT_MODE_STACK_BASE;
    _UND_MODE_STACK_BASE = UND_MODE_STACK_BASE;

#else
    _SVC_MODE_STACK_BASE = CONFIG_SVC_STACK_BASE;
    _SYS_MODE_STACK_BASE = CONFIG_SYS_STACK_BASE;
    _FIQ_MODE_STACK_BASE = CONFIG_FIQ_STACK_BASE;
    _IRQ_MODE_STACK_BASE = CONFIG_IRQ_STACK_BASE;
    _ABT_MODE_STACK_BASE = CONFIG_ABT_STACK_BASE;
    _UND_MODE_STACK_BASE = CONFIG_UND_STACK_BASE;
#endif

    return RET_BOOT_PASS;
}

/*!
 * @brief   fdt_boot_initial
 * @param   none
 * @retval  none
 * @note    copy dtb to ram
 */
kint32_t fix_data_mempool_initial(void)
{
    kint32_t retval;

    retval = memory_block_self_defines(GFP_FIXED, 
                                CONFIG_B2K_FIXDATA_BASE, CONFIG_B2K_FIXDATA_SIZE);
    return retval ? RET_BOOT_PASS : RET_BOOT_ERR;
}

/*!
 * @brief   extdisk_mount
 * @param   none
 * @retval  none
 * @note    mount disk
 */
kint32_t extdisk_mount(struct boot_images *sprt_image)
{
    struct fatfs_disk *sprt_fdisk;
    struct fwk_gendisk *sprt_gdisk;
    kint32_t retval;

    sprt_fdisk = fs_alloc_fatfs(SDDISK);
    if (!isValid(sprt_fdisk))
        return RET_BOOT_ERR;

    sprt_gdisk = &sprt_fdisk->sgrt_gdisk;
    retval = sprt_gdisk->mount(sprt_gdisk);
    if (retval)
        return RET_BOOT_ERR;

    sprt_image->sprt_fdisk = sprt_fdisk;
    return RET_BOOT_PASS;
}

/*!
 * @brief   extdisk_mount
 * @param   none
 * @retval  none
 * @note    mount disk
 */
void extdisk_unmount(struct boot_images *sprt_image)
{
    struct fatfs_disk *sprt_fdisk;
    struct fwk_gendisk *sprt_gdisk;

    sprt_fdisk = sprt_image->sprt_fdisk;
    if (!isValid(sprt_fdisk))
        return;

    sprt_gdisk = &sprt_fdisk->sgrt_gdisk;
    sprt_gdisk->unmount(sprt_gdisk);
    kfree(sprt_fdisk);

    sprt_image->sprt_fdisk = mrt_nullptr;
}

/*!
 * @brief   prepare video params
 * @param   sprt_image, sprt_params
 * @retval  none
 * @note    fill sprt_params
 */
kint32_t tag_params_video(struct boot_images *sprt_image, struct tag_params *sprt_params)
{
    struct fatfs_disk *sprt_fdisk;
    struct fwk_gendisk *sprt_gdisk;
    struct fs_stream sgrt_file;
    struct fwk_block_device sgrt_blkdev;
    struct video_params *sprt_vdp;
    void *fontlib;
    kssize_t size;

    sprt_fdisk = sprt_image->sprt_fdisk;
    if (!isValid(sprt_fdisk))
        return RET_BOOT_ERR;

    sprt_gdisk = &sprt_fdisk->sgrt_gdisk;
    sgrt_blkdev.sprt_gdisk = sprt_gdisk;

    sprt_vdp = &sprt_params->u.sgrt_vdp;
    sprt_params->sgrt_hdr.size = sizeof(*sprt_vdp);
    sprt_params->sgrt_hdr.type = TAG_PARAM_VIDEO;

    sgrt_file.full_name = "/lib/font/HZK16";
    sgrt_file.mode = O_RDONLY;
    if (!sprt_gdisk->sprt_bops->open(&sgrt_blkdev, &sgrt_file))
    {
        size = sprt_gdisk->sprt_bops->fsize(&sgrt_file);
        if (size <= 0)
            goto END;

        fontlib = kmalloc(mrt_align(size, 8), GFP_FIXED);
        if (!isValid(fontlib))
            goto END;
        
        size = sprt_gdisk->sprt_bops->read(&sgrt_file, fontlib, size, 0);
        if (size > 0)
        {
            sprt_vdp->sgrt_hz16x16.base = fontlib;
            sprt_vdp->sgrt_hz16x16.size = size;
            sprt_vdp->sgrt_hz16x16.offset = 0;
        }
    }

END:
    sprt_gdisk->sprt_bops->close(&sgrt_blkdev, &sgrt_file);
    return RET_BOOT_PASS;
}

/*!
 * @brief   prepare params for kernel
 * @param   sprt_image
 * @retval  none
 * @note    fill sprt_params
 */
kint32_t boot_params_prep(struct boot_images *sprt_image)
{
    struct global_data *sprt_gd;
    struct tag_params *sprt_params;
    kint32_t retval;

    sprt_gd = sprt_image->sprt_gd;
    sprt_image->os_entry = sprt_gd->os_entry;

    sprt_params = (struct tag_params *)CONFIG_B2K_PARAM_BASE;
    sprt_image->sprt_params = sprt_params;
    
    sprt_params->sgrt_hdr.size = 0;
    sprt_params->sgrt_hdr.type = 0;

    /*!< ---------------------------------------------------------*/
    sprt_params = TAG_PARAM_NEXT(sprt_params);
    retval = tag_params_video(sprt_image, sprt_params);
    if (retval)
        goto fail;

    /*!< ---------------------------------------------------------*/
    sprt_params = TAG_PARAM_NEXT(sprt_params);
    sprt_params->sgrt_hdr.size = sizeof(sprt_params->u.sgrt_fdt);
    sprt_params->sgrt_hdr.type = TAG_PARAM_FDT;
    memcpy(&sprt_params->u.sgrt_fdt.sgrt_fdt, &sprt_image->sgrt_fdt, sizeof(struct m_area));

    /*!< ---------------------------------------------------------*/
    sprt_params = TAG_PARAM_NEXT(sprt_params);
    sprt_params->sgrt_hdr.size = 0;
    sprt_params->sgrt_hdr.type = -1;

    return RET_BOOT_PASS;

fail:
    return RET_BOOT_ERR;
}

/*!
 * @brief   fdt_boot_initial
 * @param   none
 * @retval  none
 * @note    copy dtb to ram
 */
kint32_t fdt_boot_initial(struct boot_images *sprt_image)
{
    struct fs_stream sgrt_file;
    struct fatfs_disk *sprt_fdisk;
    struct fwk_gendisk *sprt_gdisk;
    struct fwk_block_device sgrt_blkdev;
    kssize_t size;
    kint32_t retval;

    if ((PROGRAM_RAM_START <= (CONFIG_DEVICE_TREE_BASE + CONFIG_FDT_MAX_SIZE - 1)))
        return RET_BOOT_ERR;

    sprt_fdisk = sprt_image->sprt_fdisk;
    if (!isValid(sprt_fdisk))
        return RET_BOOT_ERR;

    sprt_gdisk = &sprt_fdisk->sgrt_gdisk;
    sgrt_blkdev.sprt_gdisk = sprt_gdisk;
    sgrt_file.full_name = "/boot/firmware.dtb";
    sgrt_file.mode = O_RDONLY;
    retval = sprt_gdisk->sprt_bops->open(&sgrt_blkdev, &sgrt_file);
    if (retval)
        return RET_BOOT_ERR;

    size = sprt_gdisk->sprt_bops->read(&sgrt_file, 
                                    (void *)CONFIG_DEVICE_TREE_BASE, CONFIG_FDT_MAX_SIZE, 0);
    sprt_gdisk->sprt_bops->close(&sgrt_blkdev, &sgrt_file);

    if (size <= 0)
    {
        print_err("can not read device-tree file!\n");
        return RET_BOOT_ERR;
    }

    sprt_image->sgrt_fdt.base = (void *)CONFIG_DEVICE_TREE_BASE;
    sprt_image->sgrt_fdt.size = size;

    return RET_BOOT_PASS;
}

/*!
 * @brief   jump_to_kernel
 * @param   gd
 * @retval  none
 * @note    jump to assembly entry: kernel_entry
 */
void jump_to_kernel(struct boot_images *sprt_image)
{
    kuint32_t r2;
    void (*kernel_entry)(kint32_t zero, kint32_t arch, kuint32_t params);

    r2 = (kuint32_t)sprt_image->sprt_params;
    kernel_entry = (void (*)(kint32_t, kint32_t, kuint32_t))sprt_image->os_entry;

    /*!< jump to head.S */
    kernel_entry(0, 0, r2);
}

/*!
 * @brief   boot_main_loop
 * @param   none
 * @retval  none
 * @note    initial heap and stack
 */
kint32_t boot_main_loop(void)
{
    struct boot_images *sprt_image;
    kint32_t retval;

    sprt_image = &sgrt_boot_images;
    if (extdisk_mount(sprt_image))
        return RET_BOOT_ERR;

    for (;;)
    {
        break;
    }

    /*!< device-tree prepare */
    retval = fdt_boot_initial(sprt_image);
    if (retval)
        goto exit;

    retval = boot_params_prep(sprt_image);
    if (retval)
        goto exit;

exit:
    extdisk_unmount(sprt_image);

    /*!< Destroy Memory Pool of Bootloader */
    memory_block_self_destroy(-1);

    return retval ? RET_BOOT_ERR : RET_BOOT_PASS;
}

/*!< board init queue lately */
board_init_t board_init_sequence_r[] =
{
    /*!< boot initial */
    system_boot_initial,

    /*!< fix memory initial */
    fix_data_mempool_initial,

    /*!< peripheral initial*/
//	board_init_light,

    /*!< boot main loop */
    boot_main_loop,

    mrt_nullptr,
};

/*!
 * @brief   board_init_r
 * @param   none
 * @retval  none
 * @note    board late initial
 */
void board_init_r(void)
{
    struct boot_images *sprt_image;
    srt_gd_t *sprt_gd;

    sprt_gd = board_get_gd();

    /*!< save kernel entry address */
    __asm__ __volatile__ (
        " mov %0, r10 \n"
        : "=&r"(sprt_gd->os_entry)
    );

    sprt_image = &sgrt_boot_images;
    sprt_image->sprt_gd = sprt_gd;

    /*!< initial */
    if (board_initcall_run_list(board_init_sequence_r))
        mrt_assert(false);

    /*!< if initialize finished, start kernel */
    jump_to_kernel(sprt_image);
}

/* end of file */
