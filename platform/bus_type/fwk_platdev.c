/*
 * Platform Bus Interface of Hardware Abstraction Layer
 *
 * File Name:   fwk_platdev.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.05.23
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <platform/fwk_platform.h>
#include <platform/fwk_platdev.h>

/*!< The globals */
static struct fwk_device_type sgrt_fwk_platform_dev_type =
{
	.name = "platform-type",
};

static DECLARE_LIST_HEAD(sgrt_fwk_devices);

/*!< The functions */
static kint32_t fwk_device_attach(struct fwk_device *sprt_dev, struct fwk_bus_type *sprt_bus_type);
static kint32_t fwk_device_detach(struct fwk_device *sprt_dev);
static kint32_t fwk_device_to_bus(struct fwk_device *sprt_dev, struct fwk_bus_type *sprt_bus_type);
static kint32_t fwk_bus_del_device(struct fwk_device *sprt_dev, struct fwk_bus_type *sprt_bus_type);

/*!< API function */
/*!
 * @brief   Register Platform Device
 * @param   sprt_platdev
 * @retval  Register Result
 * @note    Should be used at initcall
 */
kint32_t fwk_register_platdevice(struct fwk_platdev *sprt_platdev)
{
	sprt_platdev->sgrt_dev.sprt_bus = &sgrt_fwk_platform_bus_type;
	sprt_platdev->sgrt_dev.sprt_type = &sgrt_fwk_platform_dev_type;

	return fwk_device_add(&sprt_platdev->sgrt_dev);
}

/*!
 * @brief   Unregister Platform Device
 * @param   sprt_platdev
 * @retval  Unregister Result
 * @note    Should be used at exitcall
 */
kint32_t fwk_unregister_platdevice(struct fwk_platdev *sprt_platdev)
{
	return fwk_device_del(&sprt_platdev->sgrt_dev);
}

/*!< -------------------------------------------------------------------------- */
/*!<							Device Local Interface							*/
/*!< -------------------------------------------------------------------------- */
static kint32_t fwk_device_find(struct fwk_device *sprt_dev)
{
	struct fwk_device *sprt_leaf;

	foreach_list_next_entry(sprt_leaf, &sgrt_fwk_devices, sgrt_leaf)
	{
		if (sprt_leaf == sprt_dev)
			return ER_NORMAL;

		if (!strcmp(mrt_dev_get_name(sprt_leaf), mrt_dev_get_name(sprt_dev)))
			return ER_NORMAL;
	}

	return -ER_NOTFOUND;
}

/*!
 * @brief   Device Match Interface
 * @param   device, platform-bus
 * @retval  errno
 * @note    Device actively matches driver
 */
static kint32_t fwk_device_attach(struct fwk_device *sprt_dev, struct fwk_bus_type *sprt_bus_type)
{
	struct fwk_driver *sprt_driver;
	kint32_t retval;

	DECLARE_LIST_HEAD_PTR(sprt_list);
	DECLARE_LIST_HEAD_PTR(sprt_parent);

	/*!< sprt_driver is not null, maybe this device has been matched to driver */
	if (sprt_dev->sprt_driver)
		return ER_NORMAL;

	/*!< check if "match" function defines in platform-bus */
	if (!sprt_bus_type->match)
		return -ER_NSUPPORT;

	FWK_INIT_BUS_DRIVER_LIST(sprt_parent, sprt_list, sprt_bus_type);

	/*!< get driver from bus one after another */
	while ((sprt_driver = FWK_NEXT_DRIVER(sprt_parent, sprt_list)))
	{
		/*!< try to attach this driver */
		retval = fwk_device_driver_match(sprt_dev, sprt_bus_type, sprt_driver);
		if (!retval || (retval == -ER_PERMIT))
			return ER_NORMAL;
	}

	return -ER_PERMIT;
}

/*!
 * @brief   Device dissolve the relationship with driver 
 * @param   device
 * @retval  errno
 * @note    Device actively leaves driver
 */
static kint32_t fwk_device_detach(struct fwk_device *sprt_dev)
{
	struct fwk_driver *sprt_driver;

	/*!< sprt_driver is null, no driver has been mathced */
	if (!sprt_dev->sprt_driver)
		return ER_NORMAL;

	sprt_driver	= sprt_dev->sprt_driver;

	/*!< prepare to separate */
	fwk_device_driver_remove(sprt_dev);

	/*!< do separattion */
	sprt_dev->sprt_driver = mrt_nullptr;
	sprt_driver->matches--;

	return ER_NORMAL;
}

/*!
 * @brief   Add new device to bus
 * @param   device, platform-bus
 * @retval  errno
 * @note    After adding finished, excute device-driver matching
 */
static kint32_t fwk_device_to_bus(struct fwk_device *sprt_dev, struct fwk_bus_type *sprt_bus_type)
{
	kint32_t retval;

	/*!< add to list tail */
	list_head_add_tail(FWK_GET_BUS_DEVICE(sprt_bus_type), &sprt_dev->sgrt_link);

	/*!< do device-driver matching */
	retval = fwk_device_attach(sprt_dev, sprt_bus_type);
	return (!retval || (retval == -ER_PERMIT)) ? ER_NORMAL : retval;
}

/*!
 * @brief   delete device on bus
 * @param   device, platform-bus
 * @retval  errno
 * @note    none
 */
static kint32_t fwk_bus_del_device(struct fwk_device *sprt_dev, struct fwk_bus_type *sprt_bus_type)
{
	/*!< do detaching before deleting */
	fwk_device_detach(sprt_dev);

	/*!< delete device */
	list_head_del_safe(FWK_GET_BUS_DEVICE(sprt_bus_type), &sprt_dev->sgrt_link);

	return ER_NORMAL;
}

/*!
 * @brief   Register device
 * @param   device
 * @retval  errno
 * @note    none
 */
kint32_t fwk_device_add(struct fwk_device *sprt_dev)
{
	struct fwk_bus_type *sprt_bus_type;
	kint32_t retval;

	sprt_bus_type = sprt_dev->sprt_bus;

	if (!fwk_device_find(sprt_dev))
		return -ER_EXISTED;

	/*!< platform-bus is not exsisted */
	if (sprt_bus_type)
	{
		/*!< device list is not exsisted */
		if (!sprt_bus_type->sprt_SysPriv)
			goto fail;

		/*!< fisrt register */
		sprt_dev->sprt_driver = mrt_nullptr;

		/*!< add device to device list (platform-bus) */
		retval = fwk_device_to_bus(sprt_dev, sprt_bus_type);
		if (retval)
			return retval;
	}

	list_head_add_tail(&sgrt_fwk_devices, &sprt_dev->sgrt_leaf);
	return ER_NORMAL;

fail:
	return -ER_ERROR;
}

/*!
 * @brief   Unregister device
 * @param   device
 * @retval  errno
 * @note    none
 */
kint32_t fwk_device_del(struct fwk_device *sprt_dev)
{
	struct fwk_bus_type *sprt_bus_type;
	kint32_t retval;

	sprt_bus_type = sprt_dev->sprt_bus;

	if (fwk_device_find(sprt_dev))
		return -ER_NOTFOUND;

	/*!< platform-bus is not exsisted */
	if (sprt_bus_type)
	{
		/*!< device list is not exsisted */
		if (!sprt_bus_type->sprt_SysPriv)
			goto fail;

		/*!< delete device on the bus */
		retval = fwk_bus_del_device(sprt_dev, sprt_bus_type);
		if (retval)
			return retval;
	}

	mrt_dev_del_name(sprt_dev);
	list_head_del(&sprt_dev->sgrt_leaf);
	return ER_NORMAL;

fail:
	return -ER_ERROR;
}

/* end of file */
