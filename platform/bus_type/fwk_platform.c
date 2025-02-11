/*
 * Platform Bus Interface of Hardware Abstraction Layer
 *
 * File Name:   fwk_platform.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.05.25
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <platform/fwk_platform.h>
#include <platform/fwk_platdev.h>
#include <platform/fwk_platdrv.h>
#include <platform/fwk_pinctrl.h>
#include <platform/fwk_inode.h>

/*!<
 * One device can only be matched with one driver
 * One driver can be used for multiple devices
 */

/*!< API function */
/*!
 * @brief   Match device and driver
 * @param   device, driver
 * @retval  errno
 * @note    none
 */
static kint32_t fwk_platform_match(struct fwk_device *sprt_dev, struct fwk_driver *sprt_driver)
{
    struct fwk_platdev *sprt_platdev;
    struct fwk_platdrv *sprt_platdrv;
    struct fwk_id_table *sprt_idTable;
    struct fwk_device_node *sprt_np;
    kuint32_t idTable_cnt;

    sprt_platdev = mrt_container_of(sprt_dev, struct fwk_platdev, sgrt_dev);
    sprt_platdrv = mrt_container_of(sprt_driver, struct fwk_platdrv, sgrt_driver);

    /*!<
     * Devices cannot have the same name as each other; Drivers cannot have the same name from one drive to another
     * Therefore, if you match the device name, it is not possible to match one driver with multiple devices
     * This function should be done by idTable
     */

    /*!< Matching priority 1: override, where override is set, all other matching methods will be invalidated */
    if (sprt_platdev->driver_override)
        return ((!strcmp(sprt_platdev->driver_override, sprt_driver->name)) ? ER_NORMAL : (-ER_FAILD));

    /*!< Match Priority 2: Device Tree */
    sprt_np = fwk_of_node_try_matches(sprt_dev->sprt_node, sprt_driver->sprt_of_match_table, mrt_nullptr);
    if (isValid(sprt_np) && (sprt_dev->sprt_node == sprt_np))
        return ER_NORMAL;

    /*!< Match Priority 3: idTable */
    for (idTable_cnt = 0; idTable_cnt < sprt_platdrv->num_idTable; idTable_cnt++)
    {
        sprt_idTable = sprt_platdrv->sprt_idTable + idTable_cnt;

        if (!strcmp((char *)sprt_platdev->name, (char *)sprt_idTable->name))
            return ER_NORMAL;
    }

    /*!< Match Priority 4: Name */
    /*!< 
     * Each device is not allowed to have the same name;
     * So you can exit immediately after finding it, and there will be no more devices with the same name in the future */
    if (!strcmp((char *)sprt_platdev->name, (char *)sprt_driver->name))
        return ER_NORMAL;

    return -ER_NOTFOUND;
}

/*!
 * @brief   Probe after matching successfully
 * @param   device
 * @retval  errno
 * @note    none
 */
static kint32_t fwk_platform_probe(struct fwk_device *sprt_dev)
{
    struct fwk_driver  *sprt_driver;
    struct fwk_platdev *sprt_platdev;
    struct fwk_platdrv *sprt_platdrv;

    sprt_driver	= sprt_dev->sprt_driver;
    if (!sprt_driver)
        return -ER_NOTFOUND;

    sprt_platdev = mrt_container_of(sprt_dev, struct fwk_platdev, sgrt_dev);
    sprt_platdrv = mrt_container_of(sprt_driver, struct fwk_platdrv, sgrt_driver);

    if ((!sprt_platdrv->probe) || (0 > sprt_platdrv->probe(sprt_platdev)))
    {
        print_warn("device driver probe anomaly, driver is: %s\n", sprt_platdrv->sgrt_driver.name);
        return -ER_PERMIT;
    }

    return ER_NORMAL;
}

/*!
 * @brief   Remove on bus
 * @param   device
 * @retval  errno
 * @note    none
 */
static kint32_t fwk_platform_remove(struct fwk_device *sprt_dev)
{
    struct fwk_driver  *sprt_driver;
    struct fwk_platdev *sprt_platdev;
    struct fwk_platdrv *sprt_platdrv;

    sprt_driver	= sprt_dev->sprt_driver;
    if (!sprt_driver)
        return -ER_NOTFOUND;

    sprt_platdev = mrt_container_of(sprt_dev, struct fwk_platdev, sgrt_dev);
    sprt_platdrv = mrt_container_of(sprt_driver, struct fwk_platdrv, sgrt_driver);

    if ((!sprt_platdrv->remove) || (0 > sprt_platdrv->remove(sprt_platdev)))
        return -ER_PERMIT;

    return ER_NORMAL;
}

static struct fwk_SysPrivate sgrt_fwk_platform_SysPriv =
{
    .sprt_bus = &sgrt_fwk_platform_bus_type,

    .sgrt_list_devices	= LIST_HEAD_INIT(&sgrt_fwk_platform_SysPriv.sgrt_list_devices),
    .sgrt_device_lock   = RW_LOCK_INIT(),

    .sgrt_list_drivers	= LIST_HEAD_INIT(&sgrt_fwk_platform_SysPriv.sgrt_list_drivers),
    .sgrt_driver_lock   = RW_LOCK_INIT(),
};

struct fwk_bus_type sgrt_fwk_platform_bus_type =
{
    .name	= "platform",

    .match	= fwk_platform_match,
    .probe	= fwk_platform_probe,
    .remove	= fwk_platform_remove,

    .sprt_SysPriv = &sgrt_fwk_platform_SysPriv,
};

/*!
 * @brief   Probe operation
 * @param   none
 * @retval  none
 * @note    none
 */
kint32_t fwk_device_driver_probe(struct fwk_device *sprt_dev)
{
    struct fwk_bus_type *sprt_bus_type;
    kint32_t retval;

    sprt_bus_type = sprt_dev->sprt_bus;

    /*!< bind pinctrl before probe */
    if (fwk_pinctrl_bind_pins(sprt_dev))
        print_warn("bind pinctrl faild, device is: %s\n", mrt_dev_get_name(sprt_dev));

    if (sprt_bus_type->probe)
    {
        retval = sprt_bus_type->probe(sprt_dev);
        if (retval)
        {
            fwk_pinctrl_unbind_pins(sprt_dev);
            print_err("probe device and driver failed! device is: %s\n", sprt_dev->init_name);

            return retval;
        }

        return ER_NORMAL;
    }

    return -ER_NSUPPORT;
}

/*!
 * @brief   Remove operation
 * @param   device
 * @retval  errno
 * @note    none
 */
kint32_t fwk_device_driver_remove(struct fwk_device *sprt_dev)
{
    struct fwk_bus_type *sprt_bus_type = sprt_dev->sprt_bus;

    return sprt_bus_type->remove ? sprt_bus_type->remove(sprt_dev) : (-ER_NSUPPORT);
}

/*!
 * @brief   Match device and driver
 * @param   device, platform-bus, driver
 * @retval  errno
 * @note    none
 */
kint32_t fwk_device_driver_match(struct fwk_device *sprt_dev, struct fwk_bus_type *sprt_bus_type, void *ptr_data)
{
    struct fwk_driver *sprt_driver;
    kint32_t retval;

    sprt_driver	= (struct fwk_driver *)ptr_data;
    retval = sprt_bus_type->match(sprt_dev, sprt_driver);
    if (retval < 0)
        return -ER_NOTFOUND;

    /*!< 
     * Save the device to indicate that the driver has been matched;
     * You only need to operate the device to obtain the driver's information 
     */
    sprt_dev->sprt_driver = sprt_driver;

    return fwk_device_driver_probe(sprt_dev);
}

/*!
 * @brief   device initialization
 * @param   sprt_dev
 * @retval  errno
 * @note    none
 */
kint32_t fwk_device_initial(struct fwk_device *sprt_dev)
{
    struct fwk_kobject *sprt_kobj;

    init_list_head(&sprt_dev->sgrt_leaf);
    init_list_head(&sprt_dev->sgrt_link);
    fwk_kobject_init(&sprt_dev->sgrt_kobj);

    sprt_kobj = fwk_find_kobject_by_path(mrt_nullptr, FWK_PATH_SYS_DEVICE);
    if (!isValid(sprt_kobj))
        return PTR_ERR(sprt_kobj);

    sprt_dev->sgrt_kobj.sprt_kset = mrt_fwk_kset_get(sprt_kobj);
    if (!isValid(sprt_dev->sgrt_kobj.sprt_kset))
        return PTR_ERR(sprt_dev->sgrt_kobj.sprt_kset);

    return ER_NORMAL;
}

/*!
 * @brief   fwk_device_create
 * @param   none
 * @retval  none
 * @note    none
 */
struct fwk_device *fwk_device_create(kuint32_t type, kuint32_t devNum, kchar_t *fmt, ...)
{
    struct fwk_kobject *sprt_kobj;
    struct fwk_kobject *sprt_parent;
    struct fwk_device *sprt_dev;
    kchar_t *name;
    va_list sprt_valist;

    if (!fmt)
        return ERR_PTR(-ER_ERROR);

    sprt_parent = fwk_find_kobject_by_path(mrt_nullptr, FWK_PATH_DEVICE);
    if (!isValid(sprt_parent))
        return ERR_PTR(-ER_NOTFOUND);

    va_start(sprt_valist, fmt);
    name = vasprintk_safe(fmt, mrt_nullptr, sprt_valist);
    va_end(sprt_valist);

    if (!name)
        return ERR_PTR(-ER_NOMEM);

    sprt_kobj = fwk_kobject_populate(sprt_parent, name);
    if (!isValid(sprt_kobj))
        goto fail1;

    if (sprt_kobj->sprt_inode->type == INODE_TYPE_FILE)
        fwk_inode_set_ops(sprt_kobj->sprt_inode, type, devNum);

    sprt_dev = kzalloc(sizeof(*sprt_dev), GFP_KERNEL);
    if (!isValid(sprt_dev))
        goto fail2;

    if (fwk_device_initial(sprt_dev))
        goto fail3;

    mrt_dev_set_name(sprt_dev, "%s", name);
    if (fwk_device_add(sprt_dev))
        goto fail4;

    kfree(name);
    return sprt_dev;

fail4:
    mrt_dev_del_name(sprt_dev);
fail3:
    kfree(sprt_dev);
fail2:
    fwk_kset_kobject_remove(sprt_kobj);
fail1:
    kfree(name);
    return ERR_PTR(-ER_FAILD);
}

/*!
 * @brief   fwk_device_destroy
 * @param   none
 * @retval  none
 * @note    none
 */
kint32_t fwk_device_destroy(struct fwk_device *sprt_dev)
{
    struct fwk_kobject *sprt_parent;
    struct fwk_kobject *sprt_kobj;

    if (!sprt_dev)
        return -ER_NOMEM;

    sprt_parent = fwk_find_kobject_by_path(mrt_nullptr, FWK_PATH_DEVICE);
    if (!isValid(sprt_parent))
        return -ER_NOTFOUND;

    sprt_kobj = fwk_find_kobject_by_path(sprt_parent, mrt_dev_get_name(sprt_dev));
    if (!isValid(sprt_kobj))
        return -ER_NOTFOUND;

    fwk_device_del(sprt_dev);
    fwk_kset_kobject_remove(sprt_kobj);

    return ER_NORMAL;
}

/*!< end of file */
