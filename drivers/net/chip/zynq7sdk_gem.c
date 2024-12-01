/*
 * ZYNQ eMAC Of PS Driver
 *
 * File Name:   zynq7sdk_gem.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.11.30
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/fwk_cdev.h>
#include <platform/fwk_chrdev.h>
#include <platform/fwk_inode.h>
#include <platform/fwk_fs.h>
#include <platform/of/fwk_of.h>
#include <platform/of/fwk_of_device.h>
#include <platform/fwk_platdev.h>
#include <platform/fwk_platdrv.h>
#include <platform/fwk_uaccess.h>

#include <platform/net/fwk_if.h>

#include <zynq7/zynq7_periph.h>
#include <zynq7/xemac/xemacpsif.h>
#include <zynq7/xemac/xemacps.h>
#include <zynq7/xemac/xemac_ieee_reg.h>

/*!< The defines */
struct xsdk_gem_drv_data
{
    void *base;
    kint32_t irq;

    kuint32_t major;
    kuint32_t minor;
    struct fwk_cdev *sprt_cdev;
    struct fwk_device *sprt_idev;

    xemacpsif_s *sprt_emacif;
    XEmacPs_Config *sprt_config;
};

#define XSDK_GEM_DRIVER_NAME								"gem0"

/*!< The globals */

/*!< The functions */

/*!< API function */
/*!
 * @brief   xsdk_gem_driver_open
 * @param   sprt_inode, sprt_file
 * @retval  errno
 * @note    none
 */
static kint32_t xsdk_gem_driver_open(struct fwk_inode *sprt_inode, struct fwk_file *sprt_file)
{
    struct xsdk_gem_drv_data *sprt_data;

    sprt_data = sprt_inode->sprt_cdev->privData;
    sprt_file->private_data = sprt_data;



    return 0;
}

/*!
 * @brief   xsdk_gem_driver_close
 * @param   sprt_inode, sprt_file
 * @retval  errno
 * @note    none
 */
static kint32_t xsdk_gem_driver_close(struct fwk_inode *sprt_inode, struct fwk_file *sprt_file)
{
    sprt_file->private_data = mrt_nullptr;

    return 0;
}

/*!
 * @brief   xsdk_gem_driver_write
 * @param   sprt_file, ptrBuffer, size
 * @retval  errno
 * @note    none
 */
static kssize_t xsdk_gem_driver_write(struct fwk_file *sprt_file, const kbuffer_t *ptrBuffer, kssize_t size)
{
    return 0;
}

/*!
 * @brief   xsdk_gem_driver_read
 * @param   sprt_file, ptrBuffer, size
 * @retval  errno
 * @note    none
 */
static kssize_t xsdk_gem_driver_read(struct fwk_file *sprt_file, kbuffer_t *ptrBuffer, kssize_t size)
{
    return 0;
}

/*!< driver operation */
const struct fwk_file_oprts sgrt_xsdk_gem_driver_oprts =
{
    .open	= xsdk_gem_driver_open,
    .close	= xsdk_gem_driver_close,
    .write	= xsdk_gem_driver_write,
    .read	= xsdk_gem_driver_read,
};

static irq_return_t xsdk_gem_driver_isr(void *args)
{
    return 0;
}

/*!
 * @brief   configure property
 * @param   sprt_pdev, sprt_data
 * @retval  errno
 * @note    none
 */
static kint32_t xsdk_gem_probe_dt(struct fwk_platdev *sprt_pdev, struct xsdk_gem_drv_data *sprt_data)
{
    struct fwk_device_node *sprt_node, *sprt_phy;
    struct fwk_platdev *sprt_platnew;
    kuint32_t phandle;
    void *base;

    sprt_node = sprt_pdev->sgrt_dev.sprt_node;
    if (!isValid(sprt_node))
        return PTR_ERR(sprt_node);

    fwk_of_property_read_u32(sprt_node, "phy-handle", &phandle);
    sprt_phy = fwk_of_find_node_by_phandle(mrt_nullptr, phandle);
    if (!isValid(sprt_phy))
        return PTR_ERR(sprt_phy);

    base = (void *)fwk_platform_get_address(sprt_pdev, 0);
    sprt_data->base = fwk_io_remap(base);
    sprt_data->irq = fwk_platform_get_irq(sprt_pdev, 0);

    sprt_platnew = fwk_platdevice_alloc("xsdk,gem-phy", -1);
    if (!isValid(sprt_platnew))
        return PTR_ERR(sprt_platnew);

    sprt_platnew->sgrt_dev.sprt_node = sprt_phy;
    sprt_platnew->sgrt_dev.sprt_parent = &sprt_pdev->sgrt_dev;

    return fwk_platdevice_add(sprt_platnew);
}

/*!
 * @brief   xsdk_gem_driver_probe
 * @param   sprt_dev
 * @retval  errno
 * @note    none
 */
static kint32_t xsdk_gem_driver_probe(struct fwk_platdev *sprt_pdev)
{
    struct xsdk_gem_drv_data *sprt_data;
    struct fwk_cdev *sprt_cdev;
    struct fwk_device *sprt_idev;
    kuint32_t devnum;
    kint32_t retval;

    retval = fwk_alloc_chrdev(&devnum, 0, 1, XSDK_GEM_DRIVER_NAME);
    if (retval)
        return -ER_FAILD;

    sprt_cdev = fwk_cdev_alloc(&sgrt_xsdk_gem_driver_oprts);
    if (!isValid(sprt_cdev))
        goto fail1;

    retval = fwk_cdev_add(sprt_cdev, devnum, 1);
    if (retval < 0)
        goto fail2;

    sprt_idev = fwk_device_create(NR_TYPE_CHRDEV, devnum, XSDK_GEM_DRIVER_NAME);
    if (!isValid(sprt_idev))
        goto fail3;
    
    sprt_data = (struct xsdk_gem_drv_data *)kzalloc(sizeof(struct xsdk_gem_drv_data), GFP_KERNEL);
    if (!isValid(sprt_data))
        goto fail4;

    sprt_data->major = GET_DEV_MAJOR(devnum);
    sprt_data->minor = GET_DEV_MINOR(devnum);
    sprt_data->sprt_cdev = sprt_cdev;
    sprt_data->sprt_idev = sprt_idev;
    sprt_idev->sprt_parent = &sprt_pdev->sgrt_dev;

    if (xsdk_gem_probe_dt(sprt_pdev, sprt_data))
        goto fail5;

    sprt_cdev->privData = sprt_data;
    fwk_platform_set_drvdata(sprt_pdev, sprt_data);

    retval = fwk_request_irq(sprt_data->irq, xsdk_gem_driver_isr, 0, XSDK_GEM_DRIVER_NAME, sprt_data);
    if (retval)
        goto fail6;

    fwk_disable_irq(sprt_data->irq);
    print_info("register a new chardevice (GEM)\n");

    return ER_NORMAL;

fail6:
    fwk_platform_set_drvdata(sprt_pdev, mrt_nullptr);
fail5:
    kfree(sprt_data);
fail4:
    fwk_device_destroy(sprt_idev);
fail3:
    fwk_cdev_del(sprt_cdev);
fail2:
    kfree(sprt_cdev);
fail1:
    fwk_unregister_chrdev(devnum, 1);

    return -ER_FAILD;
}

/*!
 * @brief   xsdk_gem_driver_remove
 * @param   sprt_dev
 * @retval  errno
 * @note    none
 */
static kint32_t xsdk_gem_driver_remove(struct fwk_platdev *sprt_pdev)
{
    struct xsdk_gem_drv_data *sprt_data;
    kuint32_t devnum;

    sprt_data = (struct xsdk_gem_drv_data *)fwk_platform_get_drvdata(sprt_pdev);
    if (!isValid(sprt_data))
        return -ER_NULLPTR;

    devnum = MKE_DEV_NUM(sprt_data->major, sprt_data->minor);

    fwk_device_destroy(sprt_data->sprt_idev);
    fwk_cdev_del(sprt_data->sprt_cdev);
    kfree(sprt_data->sprt_cdev);
    fwk_unregister_chrdev(devnum, 1);

    kfree(sprt_data);
    fwk_platform_set_drvdata(sprt_pdev, mrt_nullptr);

    return ER_NORMAL;
}

/*!< device id for device-tree */
static const struct fwk_of_device_id sgrt_xsdk_gem_driver_id[] =
{
    { .compatible = "cdns,zynq-gem", },
    {},
};

/*!< platform instance */
static struct fwk_platdrv sgrt_xsdk_gem_platdriver =
{
    .probe	= xsdk_gem_driver_probe,
    .remove	= xsdk_gem_driver_remove,
    
    .sgrt_driver =
    {
        .name 	= XSDK_GEM_DRIVER_NAME,
        .id 	= -1,
        .sprt_of_match_table = sgrt_xsdk_gem_driver_id,
    },
};

/*!< --------------------------------------------------------------------- */
/*!
 * @brief   xsdk_gem_driver_init
 * @param   none
 * @retval  errno
 * @note    none
 */
kint32_t __fwk_init xsdk_gem_driver_init(void)
{
    return fwk_register_platdriver(&sgrt_xsdk_gem_platdriver);
}

/*!
 * @brief   xsdk_gem_driver_exit
 * @param   none
 * @retval  none
 * @note    none
 */
void __fwk_exit xsdk_gem_driver_exit(void)
{
    fwk_unregister_platdriver(&sgrt_xsdk_gem_platdriver);
}

IMPORT_DRIVER_INIT(xsdk_gem_driver_init);
IMPORT_DRIVER_EXIT(xsdk_gem_driver_exit);

/*!< end of file */
