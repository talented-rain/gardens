/*
 * Gerneral Interface : kobjmap
 *
 * File Name:   kobjmap.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.06.10
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/fwk_inode.h>
#include <platform/fwk_fs.h>
#include <platform/fwk_kobj.h>

/*!< The defines */


/*!< API function */
/*!
 * @brief   kobjmap_init
 * @param   none
 * @retval  errno
 * @note    none
 */
kint32_t __fwk_init kobjmap_init(void)
{
	return fwk_kobjmap_init();
}

/*!
 * @brief   kobjmap_exit
 * @param   none
 * @retval  none
 * @note    none
 */
void __fwk_exit kobjmap_exit(void)
{
	fwk_kobjmap_del();
}

IMPORT_EARLY_INIT(kobjmap_init);
IMPORT_EARLY_EXIT(kobjmap_exit);

/*!< end of file */
