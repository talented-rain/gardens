/*
 * Gerneral Interface : SysFs
 *
 * File Name:   sysfs.c
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
#include <platform/fwk_fcntl.h>
#include <platform/fwk_kobj.h>

/*!< The globals */
struct fwk_kset *sprt_sys_fs;
struct fwk_kset *sprt_devices_fs;
struct fwk_kobject *sprt_kobj_in;
struct fwk_kobject *sprt_kobj_out;
struct fwk_kobject *sprt_kobj_err;

/*!< API function */
/*!
 * @brief   create and initialize /dev/stdin, ...
 * @param   none
 * @retval  none
 * @note    none
 */
kint32_t devfs_stdio_init(struct fwk_kset *sprt_head)
{
	kint32_t retval;

	sprt_kobj_in = fwk_kobject_create();
	if (!isValid(sprt_kobj_in))
		return -ER_NOMEM;

	sprt_kobj_out = fwk_kobject_create();
	if (!isValid(sprt_kobj_out))
	{
		retval = PTR_ERR(sprt_kobj_out);
		goto fail1;
	}

	sprt_kobj_err = fwk_kobject_create();
	if (!isValid(sprt_kobj_err))
	{
		retval = PTR_ERR(sprt_kobj_out);
		goto fail2;
	}

	sprt_kobj_in->sprt_kset  = sprt_head;
	sprt_kobj_out->sprt_kset = sprt_head;
	sprt_kobj_err->sprt_kset = sprt_head;

	retval = fwk_kobject_add(sprt_kobj_in, &sprt_head->sgrt_kobj, "stdin");
	if (retval)
		goto fail3;

	retval = fwk_kobject_add(sprt_kobj_out, &sprt_head->sgrt_kobj, "stdout");
	if (retval)
		goto fail4;

	retval = fwk_kobject_add(sprt_kobj_err, &sprt_head->sgrt_kobj, "stderr");
	if (retval)
		goto fail5;

	return ER_NORMAL;

fail5:
	fwk_kobject_del(sprt_kobj_out);
fail4:
	fwk_kobject_del(sprt_kobj_in);
fail3:
	kfree(sprt_kobj_err);
fail2:
	kfree(sprt_kobj_out);
fail1:
	kfree(sprt_kobj_in);

	return retval;
}

/*!
 * @brief   destroy dev
 * @param   none
 * @retval  none
 * @note    none
 */
void devfs_stdio_destroy(void)
{
	fwk_kobject_destroy(sprt_kobj_err);
	fwk_kobject_destroy(sprt_kobj_out);
	fwk_kobject_destroy(sprt_kobj_in);

	sprt_kobj_in = sprt_kobj_out = sprt_kobj_err = mrt_nullptr;
}

/*!
 * @brief   sysfs_init
 * @param   none
 * @retval  errno
 * @note    none
 */
kint32_t __fwk_init sysfs_init(void)
{
	kint32_t retval;

	/*!< create root path */
	if (fwk_kobject_root_init())
		return -ER_FAILD;

	sprt_sys_fs = fwk_kset_create_and_register("sys", mrt_nullptr);
	if (!isValid(sprt_sys_fs))
		return -ER_NOMEM;

	sprt_devices_fs = fwk_kset_create_and_register("dev", mrt_nullptr);
	if (!isValid(sprt_devices_fs))
		goto fail1;

	retval = devfs_stdio_init(sprt_devices_fs);
	if (retval)
		goto fail2;

	/*!< initialize file decriptor */
	if (fwk_file_system_init())
		goto fail3;

	return ER_NORMAL;

fail3:
	devfs_stdio_destroy();
fail2:
	fwk_kset_unregister(sprt_devices_fs);
	kfree(sprt_devices_fs);
fail1:
	fwk_kset_unregister(sprt_sys_fs);
	kfree(sprt_sys_fs);

	return -ER_FAILD;
}

/*!
 * @brief   sysfs_exit
 * @param   none
 * @retval  none
 * @note    none
 */
void __fwk_exit sysfs_exit(void)
{
	/*!< do nothing */
}

IMPORT_EARLY_INIT(sysfs_init);
IMPORT_EARLY_EXIT(sysfs_exit);

/*!< end of file */
