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
kint32_t fdt_boot_initial(void)
{
    struct fs_stream sgrt_file;
    struct fatfs_disk *sprt_fdisk;
    struct fwk_gendisk *sprt_gdisk;
    struct fwk_block_device sgrt_blkdev;
    kint32_t retval;

    if ((PROGRAM_RAM_START <= (CONFIG_DEVICE_TREE_BASE + CONFIG_FDT_MAX_SIZE - 1)))
        return RET_BOOT_ERR;

    sprt_fdisk = fs_alloc_fatfs(SDDISK);
    if (!isValid(sprt_fdisk))
        return RET_BOOT_ERR;

    sprt_gdisk = &sprt_fdisk->sgrt_gdisk;
    sgrt_blkdev.sprt_gdisk = sprt_gdisk;

    retval = sprt_gdisk->mount(sprt_gdisk);
    if (retval)
        goto fail1;

    sgrt_file.full_name = "/boot/firmware.dtb";
    sgrt_file.mode = O_RDONLY;
    retval = sprt_gdisk->sprt_bops->open(&sgrt_blkdev, &sgrt_file);
    if (retval)
        goto fail2;

    retval = sprt_gdisk->sprt_bops->read(&sgrt_file, 
                                    (void *)CONFIG_DEVICE_TREE_BASE, CONFIG_FDT_MAX_SIZE, 0);
    if (retval <= 0)
    {
        print_err("can not read device-tree file!\n");
        retval = -ER_EMPTY;
    }

    sprt_gdisk->sprt_bops->close(&sgrt_blkdev, &sgrt_file);

fail2:
    sprt_gdisk->unmount(sprt_gdisk);
fail1:
    kfree(sprt_fdisk);
    return retval ? RET_BOOT_ERR : RET_BOOT_PASS;
}

/*!
 * @brief   boot_main_loop
 * @param   none
 * @retval  none
 * @note    initial heap and stack
 */
kint32_t boot_main_loop(void)
{
    for (;;)
    {
        break;
    }

    /*!< device-tree prepare */
    fdt_boot_initial();

    /*!< Destroy Memory Pool of Bootloader */
    memory_block_self_destroy();

    return RET_BOOT_PASS;
}

/*!
 * @brief   jump_to_kernel
 * @param   gd
 * @retval  none
 * @note    jump to assembly entry: kernel_entry
 */
void jump_to_kernel(srt_gd_t *sprt_gd)
{
    void (*kernel_entry)(kint32_t zero, kint32_t arch, kuint32_t params);

    kernel_entry = (void (*)(kint32_t, kint32_t, kuint32_t))sprt_gd->os_entry;

    /*!< jump to head.S */
    kernel_entry(0, 0, 0);
}

/*!< board init queue lately */
board_init_t board_init_sequence_r[] =
{
    /*!< boot initial */
    system_boot_initial,

    /*!< peripheral initial*/
	board_init_light,

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
    srt_gd_t *sprt_gd;

    sprt_gd = board_get_gd();

    /*!< save kernel entry address */
    __asm__ __volatile__ (
        " mov %0, r10 \n"
        : "=&r"(sprt_gd->os_entry)
    );

    /*!< initial */
    if (board_initcall_run_list(board_init_sequence_r))
        for (;;);

    /*!< if initialize finished, start kernel */
    jump_to_kernel(sprt_gd);
}

/* end of file */
