/*
 * Template of Character Device : LED
 *
 * File Name:   extkey.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.05.13
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/fwk_cdev.h>
#include <platform/fwk_chrdev.h>
#include <platform/fwk_inode.h>
#include <platform/fwk_fs.h>
#include <platform/fwk_fcntl.h>
#include <platform/of/fwk_of.h>
#include <platform/of/fwk_of_device.h>
#include <platform/fwk_platdrv.h>
#include <platform/fwk_uaccess.h>
#include <platform/gpio/fwk_gpiodesc.h>
#include <kernel/wait.h>

#include <imx6/imx6ull_periph.h>

/*!< The defines */
struct extkey_drv_data
{
	kchar_t *ptrName;
	kuint32_t major;
	kuint32_t minor;
	struct fwk_gpio_desc *sprt_gdesc;

	struct fwk_cdev *sprt_cdev;
	struct fwk_device *sprt_idev;
	kint32_t irq;

	kbool_t wake;
	struct wait_queue_head sgrt_wqh;

	void *ptrData;
};

#define KEY_DRIVER_NAME								"extkey"
#define KEY_DRIVER_MAJOR							(NR_KEY_MAJOR)

/*!< API function */
static irq_return_t extkey_driver_isr(void *ptrDev)
{
	struct extkey_drv_data *sprt_data;

	sprt_data = (struct extkey_drv_data *)ptrDev;

	sprt_data->wake = true;
	wake_up(&sprt_data->sgrt_wqh);

	return 0;
}

/*!
 * @brief   extkey_driver_open
 * @param   sprt_inode, sprt_file
 * @retval  errno
 * @note    none
 */
static kint32_t extkey_driver_open(struct fwk_inode *sprt_inode, struct fwk_file *sprt_file)
{
	struct extkey_drv_data *sprt_data;

	sprt_data = sprt_inode->sprt_cdev->privData;
	sprt_file->private_data = sprt_data;

	fwk_enable_irq(sprt_data->irq);

	return 0;
}

/*!
 * @brief   extkey_driver_close
 * @param   sprt_inode, sprt_file
 * @retval  errno
 * @note    none
 */
static kint32_t extkey_driver_close(struct fwk_inode *sprt_inode, struct fwk_file *sprt_file)
{
	struct extkey_drv_data *sprt_data;

	sprt_data = sprt_file->private_data;
	fwk_disable_irq(sprt_data->irq);
	
	sprt_file->private_data = mrt_nullptr;

	return 0;
}

/*!
 * @brief   extkey_driver_write
 * @param   sprt_file, ptrBuffer, size
 * @retval  errno
 * @note    none
 */
static kssize_t extkey_driver_write(struct fwk_file *sprt_file, const kbuffer_t *ptrBuffer, kssize_t size)
{
	return 0;
}

/*!
 * @brief   extkey_driver_read
 * @param   sprt_file, ptrBuffer, size
 * @retval  errno
 * @note    none
 */
static kssize_t extkey_driver_read(struct fwk_file *sprt_file, kbuffer_t *ptrBuffer, kssize_t size)
{
	struct extkey_drv_data *sprt_data;
	kuint8_t value;

	sprt_data = (struct extkey_drv_data *)sprt_file->private_data;
	
	if (!(sprt_file->mode & O_NONBLOCK))
		wait_event(&sprt_data->sgrt_wqh, sprt_data->wake);
	else
	{
		if (!sprt_data->wake)
			return -ER_NREADY;
	}
	
	value = fwk_gpio_get_value(sprt_data->sprt_gdesc);
	fwk_copy_to_user(ptrBuffer, &value, 1);

	sprt_data->wake = false;
	
	return 0;
}

/*!< extkey-template driver operation */
const struct fwk_file_oprts sgrt_extkey_driver_oprts =
{
	.open	= extkey_driver_open,
	.close	= extkey_driver_close,
	.write	= extkey_driver_write,
	.read	= extkey_driver_read,
};

/*!< --------------------------------------------------------------------- */
/*!
 * @brief   extkey_driver_probe
 * @param   sprt_dev
 * @retval  errno
 * @note    none
 */
static kint32_t extkey_driver_probe(struct fwk_platdev *sprt_pdev)
{
	struct extkey_drv_data *sprt_data;
	struct fwk_gpio_desc *sprt_gdesc;
	struct fwk_cdev *sprt_cdev;
	struct fwk_device *sprt_idev;
	kuint32_t devnum;
	kint32_t irq;
	kint32_t retval;

	sprt_gdesc = fwk_gpio_desc_get(&sprt_pdev->sgrt_dev, "key0", 0);
	if (!isValid(sprt_gdesc))
		return -ER_NODEV;

	fwk_gpio_set_direction_input(sprt_gdesc);

	irq = fwk_gpio_desc_to_irq(sprt_gdesc);
	if (irq < 0)
		goto fail1;

	devnum = MKE_DEV_NUM(KEY_DRIVER_MAJOR, 0);
	retval = fwk_register_chrdev(devnum, 1, KEY_DRIVER_NAME);
	if (retval < 0)
		goto fail1;

	sprt_cdev = fwk_cdev_alloc(&sgrt_extkey_driver_oprts);
	if (!isValid(sprt_cdev))
		goto fail2;

	retval = fwk_cdev_add(sprt_cdev, devnum, 1);
	if (retval < 0)
		goto fail3;

	sprt_idev = fwk_device_create(NR_TYPE_CHRDEV, devnum, "%s%d", "input/event", 0);
	if (!isValid(sprt_idev))
		goto fail4;
	
	sprt_data = (struct extkey_drv_data *)kzalloc(sizeof(struct extkey_drv_data), GFP_KERNEL);
	if (!isValid(sprt_data))
		goto fail5;

	sprt_data->ptrName = KEY_DRIVER_NAME;
	sprt_data->major = GET_DEV_MAJOR(devnum);
	sprt_data->minor = GET_DEV_MINOR(devnum);
	sprt_data->sprt_cdev = sprt_cdev;
	sprt_data->sprt_idev = sprt_idev;
	sprt_data->sprt_gdesc = sprt_gdesc;
	sprt_data->irq = irq;
	sprt_cdev->privData = sprt_data;
	init_waitqueue_head(&sprt_data->sgrt_wqh);

	if (fwk_request_irq(irq, extkey_driver_isr, IRQ_TYPE_EDGE_RISING | IRQ_TYPE_EDGE_FALLING, KEY_DRIVER_NAME, sprt_data))
		goto fail6;

	fwk_platform_set_drvdata(sprt_pdev, sprt_data);
	fwk_disable_irq(irq);

	return ER_NORMAL;

fail6:
	kfree(sprt_data);
fail5:
	fwk_device_destroy(sprt_idev);
fail4:
	fwk_cdev_del(sprt_cdev);
fail3:
	kfree(sprt_cdev);
fail2:
	fwk_unregister_chrdev(devnum, 1);
fail1:
	fwk_gpio_desc_put(sprt_gdesc);

	return -ER_FAILD;
}

/*!
 * @brief   extkey_driver_remove
 * @param   sprt_dev
 * @retval  errno
 * @note    none
 */
static kint32_t extkey_driver_remove(struct fwk_platdev *sprt_pdev)
{
	struct extkey_drv_data *sprt_data;
	kuint32_t devnum;

	sprt_data = (struct extkey_drv_data *)fwk_platform_get_drvdata(sprt_pdev);
	if (!isValid(sprt_data))
		return -ER_NULLPTR;

	devnum = MKE_DEV_NUM(sprt_data->major, sprt_data->minor);

	fwk_device_destroy(sprt_data->sprt_idev);
	fwk_cdev_del(sprt_data->sprt_cdev);
	kfree(sprt_data->sprt_cdev);
	fwk_unregister_chrdev(devnum, 1);

	fwk_gpio_desc_put(sprt_data->sprt_gdesc);

	kfree(sprt_data);
	fwk_platform_set_drvdata(sprt_pdev, mrt_nullptr);

	return ER_NORMAL;
}

/*!< device id for device-tree */
static const struct fwk_of_device_id sgrt_extkey_driver_id[] =
{
	{ .compatible = "fsl,extern-key", },
	{},
};

/*!< platform instance */
static struct fwk_platdrv sgrt_extkey_platdriver =
{
	.probe	= extkey_driver_probe,
	.remove	= extkey_driver_remove,
	
	.sgrt_driver =
	{
		.name 	= KEY_DRIVER_NAME,
		.id 	= -1,
		.sprt_of_match_table = sgrt_extkey_driver_id,
	},
};

/*!< --------------------------------------------------------------------- */
/*!
 * @brief   extkey_driver_init
 * @param   none
 * @retval  errno
 * @note    none
 */
kint32_t __fwk_init extkey_driver_init(void)
{
	return fwk_register_platdriver(&sgrt_extkey_platdriver);
}

/*!
 * @brief   extkey_driver_exit
 * @param   none
 * @retval  none
 * @note    none
 */
void __fwk_exit extkey_driver_exit(void)
{
	fwk_unregister_platdriver(&sgrt_extkey_platdriver);
}

IMPORT_DRIVER_INIT(extkey_driver_init);
IMPORT_DRIVER_EXIT(extkey_driver_exit);

/*!< end of file */
