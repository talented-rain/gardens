/*
 * Template of Character Device (Use Xilinx SDK): LED
 *
 * File Name:   zynq7sdk_led.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.10.27
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
#include <platform/fwk_platdrv.h>
#include <platform/fwk_uaccess.h>
#include <platform/gpio/fwk_gpiodesc.h>

#include <asm/zynq7/zynq7_periph.h>
#include <asm/zynq7/xparameters.h>

/*!< The defines */
struct sdk_led_drv_data
{
    kchar_t *ptrName;
    kuint32_t major;
    kuint32_t minor;
    XGpioPs sgrt_gpio;
    kuint32_t pin;
    kbool_t isActiveL;

    struct fwk_cdev *sprt_cdev;
    struct fwk_device *sprt_idev;

    void *ptrData;
};

#define SDK_LED_DRIVER_NAME								"ledgpio"
#define SDK_LED_DRIVER_MAJOR							(NR_LED_MAJOR)

/*!< The functions */
static void sdk_led_set_value(struct sdk_led_drv_data *sprt_data, kbool_t value);

/*!< API function */
/*!
 * @brief   sdk_led_driver_open
 * @param   sprt_inode, sprt_file
 * @retval  errno
 * @note    none
 */
static kint32_t sdk_led_driver_open(struct fwk_inode *sprt_inode, struct fwk_file *sprt_file)
{
    struct sdk_led_drv_data *sprt_data;

    sprt_data = sprt_inode->sprt_cdev->privData;
    sprt_file->private_data = sprt_data;

    return 0;
}

/*!
 * @brief   sdk_led_driver_close
 * @param   sprt_inode, sprt_file
 * @retval  errno
 * @note    none
 */
static kint32_t sdk_led_driver_close(struct fwk_inode *sprt_inode, struct fwk_file *sprt_file)
{
    sprt_file->private_data = mrt_nullptr;

    return 0;
}

/*!
 * @brief   sdk_led_driver_write
 * @param   sprt_file, ptrBuffer, size
 * @retval  errno
 * @note    none
 */
static kssize_t sdk_led_driver_write(struct fwk_file *sprt_file, const kbuffer_t *ptrBuffer, kssize_t size)
{
    struct sdk_led_drv_data *sprt_data;
    kuint8_t value;

    sprt_data = (struct sdk_led_drv_data *)sprt_file->private_data;

    fwk_copy_from_user(&value, ptrBuffer, 1);
    sdk_led_set_value(sprt_data, !!value);

    return 0;
}

/*!
 * @brief   sdk_led_driver_read
 * @param   sprt_file, ptrBuffer, size
 * @retval  errno
 * @note    none
 */
static kssize_t sdk_led_driver_read(struct fwk_file *sprt_file, kbuffer_t *ptrBuffer, kssize_t size)
{
    return 0;
}

/*!< led-template driver operation */
const struct fwk_file_oprts sgrt_sdk_led_driver_oprts =
{
    .open	= sdk_led_driver_open,
    .close	= sdk_led_driver_close,
    .write	= sdk_led_driver_write,
    .read	= sdk_led_driver_read,
};

/*!
 * @brief   open or close led
 * @param   sprt_data, value (true: open led; false: close led)
 * @retval  errno
 * @note    none
 */
static void sdk_led_set_value(struct sdk_led_drv_data *sprt_data, kbool_t value)
{
    XGpioPs_WritePin(&sprt_data->sgrt_gpio, 
                    XGPIOPS_BANK_PIN(0, sprt_data->pin), 
                    sprt_data->isActiveL ? (!value) : value);
}

/*!
 * @brief   configure gpio pin property
 * @param   sprt_pdev, sprt_data
 * @retval  errno
 * @note    none
 */
static kint32_t sdk_led_configure(struct fwk_platdev *sprt_pdev, struct sdk_led_drv_data *sprt_data)
{
    struct fwk_device_node *sprt_node, *sprt_parent;
    struct fwk_of_phandle_args sgrt_args;
    XGpioPs *sprt_gpio;
    XGpioPs_Config sgrt_cfg;
    void *addr;
    kint32_t retval;

    sprt_gpio = &sprt_data->sgrt_gpio;
    sprt_node = sprt_pdev->sgrt_dev.sprt_node;
    if (!isValid(sprt_node))
        return PTR_ERR(sprt_node);

    retval = fwk_of_parse_phandle_with_args(sprt_node, 
                                    "led-gpios", "#gpio-cells", 2, 0, &sgrt_args); 
    if (retval || (!isValid(sgrt_args.sprt_node)))
        return retval;

    /*!< get gpio-controller*/
    sprt_parent = sgrt_args.sprt_node;
    addr = fwk_of_iomap(sprt_parent, 0);

    sgrt_cfg.DeviceId = 0;
    sgrt_cfg.BaseAddr = (kuint32_t)fwk_io_remap(addr);
    sprt_data->pin = sgrt_args.args[0];
    sprt_data->isActiveL = !!(sgrt_args.args[1] & GPIO_ACTIVE_LOW);

    /*!< config */
    XGpioPs_CfgInitialize(sprt_gpio, &sgrt_cfg, sgrt_cfg.BaseAddr);
    XGpioPs_SetDirectionPin(sprt_gpio, XGPIOPS_BANK_PIN(0, sprt_data->pin), XGPIOPS_PIN_DIR_OUTPUT);
    XGpioPs_SetOutputEnablePin(sprt_gpio, XGPIOPS_BANK_PIN(0, sprt_data->pin), true);

    /*!< set default level */
    sdk_led_set_value(sprt_data, false);

    return 0;
}

/*!< --------------------------------------------------------------------- */
/*!
 * @brief   sdk_led_driver_probe
 * @param   sprt_dev
 * @retval  errno
 * @note    none
 */
static kint32_t sdk_led_driver_probe(struct fwk_platdev *sprt_pdev)
{
    struct sdk_led_drv_data *sprt_data;
    struct fwk_cdev *sprt_cdev;
    struct fwk_device *sprt_idev;
    kuint32_t devnum;
    kint32_t retval;

    devnum = MKE_DEV_NUM(SDK_LED_DRIVER_MAJOR, 0);
    retval = fwk_register_chrdev(devnum, 1, SDK_LED_DRIVER_NAME);
    if (retval < 0)
        return -ER_FAILD;

    sprt_cdev = fwk_cdev_alloc(&sgrt_sdk_led_driver_oprts);
    if (!isValid(sprt_cdev))
        goto fail1;

    retval = fwk_cdev_add(sprt_cdev, devnum, 1);
    if (retval < 0)
        goto fail2;

    sprt_idev = fwk_device_create(NR_TYPE_CHRDEV, devnum, SDK_LED_DRIVER_NAME);
    if (!isValid(sprt_idev))
        goto fail3;
    
    sprt_data = (struct sdk_led_drv_data *)kzalloc(sizeof(struct sdk_led_drv_data), GFP_KERNEL);
    if (!isValid(sprt_data))
        goto fail4;

    if (sdk_led_configure(sprt_pdev, sprt_data))
        goto fail5;

    sprt_data->ptrName = SDK_LED_DRIVER_NAME;
    sprt_data->major = GET_DEV_MAJOR(devnum);
    sprt_data->minor = GET_DEV_MINOR(devnum);
    sprt_data->sprt_cdev = sprt_cdev;
    sprt_data->sprt_idev = sprt_idev;
    sprt_idev->sprt_parent = &sprt_pdev->sgrt_dev;

    sprt_cdev->privData = sprt_data;
    fwk_platform_set_drvdata(sprt_pdev, sprt_data);

    print_info("register a new chardevice (LED)\n");

    return ER_NORMAL;

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
 * @brief   sdk_led_driver_remove
 * @param   sprt_dev
 * @retval  errno
 * @note    none
 */
static kint32_t sdk_led_driver_remove(struct fwk_platdev *sprt_pdev)
{
    struct sdk_led_drv_data *sprt_data;
    kuint32_t devnum;

    sprt_data = (struct sdk_led_drv_data *)fwk_platform_get_drvdata(sprt_pdev);
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
static const struct fwk_of_device_id sgrt_sdk_led_driver_id[] =
{
    { .compatible = "xlnx,z7-lite,ledgpio", },
    {},
};

/*!< platform instance */
static struct fwk_platdrv sgrt_sdk_led_platdriver =
{
    .probe	= sdk_led_driver_probe,
    .remove	= sdk_led_driver_remove,
    
    .sgrt_driver =
    {
        .name 	= SDK_LED_DRIVER_NAME,
        .id 	= -1,
        .sprt_of_match_table = sgrt_sdk_led_driver_id,
    },
};

/*!< --------------------------------------------------------------------- */
/*!
 * @brief   sdk_led_driver_init
 * @param   none
 * @retval  errno
 * @note    none
 */
kint32_t __fwk_init sdk_led_driver_init(void)
{
    return fwk_register_platdriver(&sgrt_sdk_led_platdriver);
}

/*!
 * @brief   sdk_led_driver_exit
 * @param   none
 * @retval  none
 * @note    none
 */
void __fwk_exit sdk_led_driver_exit(void)
{
    fwk_unregister_platdriver(&sgrt_sdk_led_platdriver);
}

IMPORT_DRIVER_INIT(sdk_led_driver_init);
IMPORT_DRIVER_EXIT(sdk_led_driver_exit);

/*!< end of file */
