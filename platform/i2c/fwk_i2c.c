/*
 * Hardware Abstraction Layer I2C Interface
 *
 * File Name:   fwk_i2c.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.05.07
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/of/fwk_of.h>
#include <platform/of/fwk_of_device.h>
#include <platform/fwk_platform.h>
#include <platform/fwk_platdev.h>
#include <platform/fwk_platdrv.h>
#include <platform/i2c/fwk_i2c_dev.h>
#include <platform/i2c/fwk_i2c_core.h>
#include <platform/i2c/fwk_i2c_algo.h>

/*!< The globals */
extern struct fwk_bus_type sgrt_fwk_i2c_bus_type;
extern struct fwk_device_type sgrt_fwk_i2c_adapter_type;
extern struct fwk_device_type sgrt_fwk_i2c_client_type;

/*!< API function */
/*!
 * @brief   get the id_table that is matched
 * @param   sprt_idTable, sprt_client
 * @retval  id_table
 * @note    none
 */
static const struct fwk_i2c_device_id *fwk_i2c_match_id(const struct fwk_i2c_device_id *sprt_idTable, struct fwk_i2c_client *sprt_client)
{
    while (sprt_idTable->name && *(sprt_idTable->name))
    {
        if (!strcmp((char *)sprt_client->name, (char *)sprt_idTable->name))
            return sprt_idTable;

        sprt_idTable++;
    }

    return mrt_nullptr;
}

/*!
 * @brief   Match device and driver
 * @param   device, driver
 * @retval  errno
 * @note    none
 */
static kint32_t fwk_i2c_device_match(struct fwk_device *sprt_dev, struct fwk_driver *sprt_drv)
{
    struct fwk_i2c_client *sprt_client;
    struct fwk_i2c_driver *sprt_driver;
    struct fwk_device_node *sprt_np;

    if (sprt_dev->sprt_type != &sgrt_fwk_i2c_client_type)
        return -ER_CHECKERR;

    sprt_client = mrt_container_of(sprt_dev, struct fwk_i2c_client, sgrt_dev);
    sprt_driver = mrt_container_of(sprt_drv, struct fwk_i2c_driver, sgrt_driver);

    /*!<
     * Devices cannot have the same name as each other; Drivers cannot have the same name from one drive to another
     * Therefore, if you match the device name, it is not possible to match one driver with multiple devices
     * This function should be done by idTable
     */

    /*!< Match Priority 1: Device Tree */
    sprt_np = fwk_of_node_try_matches(sprt_dev->sprt_node, sprt_drv->sprt_of_match_table, mrt_nullptr);
    if (isValid(sprt_np) && (sprt_dev->sprt_node == sprt_np))
        return ER_NORMAL;

    /*!< Match Priority 2: idTable */
    if (sprt_driver->sprt_id_table)
        return fwk_i2c_match_id(sprt_driver->sprt_id_table, sprt_client) ? ER_NORMAL : -ER_NOTFOUND;

    return -ER_NOTFOUND;
}

/*!
 * @brief   Probe after matching successfully
 * @param   device
 * @retval  errno
 * @note    none
 */
static kint32_t fwk_i2c_device_probe(struct fwk_device *sprt_dev)
{
    struct fwk_driver  *sprt_drv;
    struct fwk_i2c_client *sprt_client;
    struct fwk_i2c_driver *sprt_driver;
    const struct fwk_i2c_device_id *sprt_idTable;

    sprt_drv = sprt_dev->sprt_driver;
    if (!sprt_drv)
        return -ER_FAULT;

    sprt_client = mrt_container_of(sprt_dev, struct fwk_i2c_client, sgrt_dev);
    sprt_driver = mrt_container_of(sprt_drv, struct fwk_i2c_driver, sgrt_driver);
    sprt_idTable = fwk_i2c_match_id(sprt_driver->sprt_id_table, sprt_client);

    /*!< get irq */
    if (!sprt_client->irq)
    {
        sprt_client->irq = fwk_of_irq_get(sprt_dev->sprt_node, 0);
        if (sprt_client->irq < 0)
            sprt_client->irq = -1;
    }

    if (sprt_driver->probe)
        return sprt_driver->probe(sprt_client, sprt_idTable) ? -ER_PERMIT : ER_NORMAL;

    return ER_NORMAL;
}

/*!
 * @brief   Remove on bus
 * @param   device
 * @retval  errno
 * @note    none
 */
static kint32_t fwk_i2c_device_remove(struct fwk_device *sprt_dev)
{
    struct fwk_driver  *sprt_drv;
    struct fwk_i2c_client *sprt_client;
    struct fwk_i2c_driver *sprt_driver;

    sprt_drv = sprt_dev->sprt_driver;
    if (!sprt_drv)
        return -ER_FAULT;

    sprt_client = mrt_container_of(sprt_dev, struct fwk_i2c_client, sgrt_dev);
    sprt_driver = mrt_container_of(sprt_drv, struct fwk_i2c_driver, sgrt_driver);

    if ((!sprt_driver->remove) || (0 > sprt_driver->remove(sprt_client)))
        return -ER_PERMIT;

    return ER_NORMAL;
}

static struct fwk_SysPrivate sgrt_fwk_i2c_device_SysPriv =
{
    .sprt_bus = &sgrt_fwk_i2c_bus_type,

    .sgrt_list_devices	= LIST_HEAD_INIT(&sgrt_fwk_i2c_device_SysPriv.sgrt_list_devices),
    .sgrt_device_lock   = RW_LOCK_INIT(),

    .sgrt_list_drivers	= LIST_HEAD_INIT(&sgrt_fwk_i2c_device_SysPriv.sgrt_list_drivers),
    .sgrt_driver_lock   = RW_LOCK_INIT(),
};

struct fwk_bus_type sgrt_fwk_i2c_bus_type =
{
    .name	= "i2c-bus",

    .match	= fwk_i2c_device_match,
    .probe	= fwk_i2c_device_probe,
    .remove	= fwk_i2c_device_remove,

    .sprt_SysPriv = &sgrt_fwk_i2c_device_SysPriv,
};

struct fwk_device_type sgrt_fwk_i2c_adapter_type =
{
    .name = "i2c-adapter-type",
};

struct fwk_device_type sgrt_fwk_i2c_client_type =
{
    .name = "i2c-client-type",
};

/*!
 * @brief   register i2c client
 * @param   sprt_client
 * @retval  errno
 * @note    none
 */
kint32_t fwk_register_i2c_device(struct fwk_i2c_client *sprt_client)
{
    sprt_client->sgrt_dev.sprt_bus = &sgrt_fwk_i2c_bus_type;

    return fwk_device_add(&sprt_client->sgrt_dev);
}

/*!
 * @brief   unregister i2c client
 * @param   sprt_client
 * @retval  errno
 * @note    none
 */
kint32_t fwk_unregister_i2c_device(struct fwk_i2c_client *sprt_client)
{
    return fwk_device_del(&sprt_client->sgrt_dev);
}

/*!
 * @brief   register i2c driver
 * @param   sprt_driver
 * @retval  errno
 * @note    none
 */
kint32_t fwk_i2c_register_driver(struct fwk_i2c_driver *sprt_driver)
{
    sprt_driver->sgrt_driver.sprt_bus = &sgrt_fwk_i2c_bus_type;

    return fwk_driver_register(&sprt_driver->sgrt_driver);
}

/*!
 * @brief   unregister i2c driver
 * @param   sprt_driver
 * @retval  errno
 * @note    none
 */
kint32_t fwk_i2c_unregister_driver(struct fwk_i2c_driver *sprt_driver)
{
    return fwk_driver_unregister(&sprt_driver->sgrt_driver);
}

/*!
 * @brief   create and register i2c client
 * @param   sprt_adap, sprt_info
 * @retval  sprt_client
 * @note    none
 */
struct fwk_i2c_client *fwk_i2c_new_device(struct fwk_i2c_adapter *sprt_adap, struct fwk_i2c_board_info const *sprt_info)
{
    struct fwk_i2c_client *sprt_client;
    kint32_t retval;

    sprt_client = kzalloc(sizeof(*sprt_client), GFP_KERNEL);
    if (!isValid(sprt_client))
        return mrt_nullptr;

    sprt_client->sprt_adapter = sprt_adap;
    sprt_client->irq = sprt_info->irq;
    sprt_client->addr = sprt_info->addr;
    sprt_client->flags = sprt_info->flags;
    kstrlcpy(sprt_client->name, sprt_info->type, sizeof(sprt_client->name));

    sprt_client->sgrt_dev.sprt_parent = &sprt_adap->sgrt_dev;
    sprt_client->sgrt_dev.sprt_bus = &sgrt_fwk_i2c_bus_type;
    sprt_client->sgrt_dev.sprt_type = &sgrt_fwk_i2c_client_type;
    sprt_client->sgrt_dev.sprt_node = sprt_info->sprt_node;
    mrt_dev_set_name(&sprt_client->sgrt_dev, "%d-%d", 
                sprt_adap->nr, sprt_client->addr | ((sprt_client->flags & FWK_I2C_M_TEN) ? 0xa000 : 0));

    /*!< verify machine address */
    if (sprt_client->flags & FWK_I2C_M_TEN)
    {
        /*!< 10bits, 0x000 ~ 0x3ff */
        if (sprt_client->addr > 0x3ff)
            goto fail;
    }
    else
    {
        /*!< 7bits, 0x01 ~ 0x7f */
        if (!sprt_client->addr || (sprt_client->addr > 0x7f))
            goto fail;
    }

    retval = fwk_device_add(&sprt_client->sgrt_dev);
    if (retval)
        goto fail;

    wr_lock(&sprt_adap->sgrt_lock);
    list_head_add_tail(&sprt_adap->sgrt_clients, &sprt_client->sgrt_link);
    wr_unlock(&sprt_adap->sgrt_lock);

    return sprt_client;

fail:
    mrt_dev_del_name(&sprt_client->sgrt_dev);
    kfree(sprt_client);

    return mrt_nullptr;
}

/*!
 * @brief   unregister i2c client
 * @param   sprt_client
 * @retval  errno
 * @note    none
 */
static void __fwk_i2c_unregister_device(struct fwk_i2c_client *sprt_client)
{
    list_head_del(&sprt_client->sgrt_link);
    fwk_device_del(&sprt_client->sgrt_dev);
}

/*!
 * @brief   unregister i2c client
 * @param   sprt_client
 * @retval  errno
 * @note    none
 */
void fwk_i2c_unregister_device(struct fwk_i2c_client *sprt_client)
{
    wr_lock(&sprt_client->sprt_adapter->sgrt_lock);
    __fwk_i2c_unregister_device(sprt_client);
    wr_unlock(&sprt_client->sprt_adapter->sgrt_lock);
}

/*!
 * @brief   register the i2c client in sprt_adap
 * @param   sprt_adap, sprt_node
 * @retval  sprt_client
 * @note    none
 */
static struct fwk_i2c_client *fwk_of_i2c_register_device(struct fwk_i2c_adapter *sprt_adap, struct fwk_device_node *sprt_node)
{
    struct fwk_i2c_board_info sgrt_bi = {};
    kuint32_t reg = 0;

    if (fwk_of_modalias_node(sprt_node, sgrt_bi.type, sizeof(sgrt_bi.type)))
        return ERR_PTR(-ER_NOTFOUND);

    if (fwk_of_property_read_u32(sprt_node, "reg", &reg))
        return ERR_PTR(-ER_NOTFOUND);

    if (!reg)
        return ERR_PTR(-ER_EMPTY);

    sgrt_bi.sprt_node = sprt_node;
    sgrt_bi.addr = (kuint16_t)reg;

    return fwk_i2c_new_device(sprt_adap, &sgrt_bi);
}

/*!
 * @brief   register all i2c clients in sprt_adap
 * @param   sprt_adap
 * @retval  none
 * @note    none
 */
void fwk_of_i2c_register_devices(struct fwk_i2c_adapter *sprt_adap)
{
    struct fwk_device_node *sprt_node;

    if (!sprt_adap->sgrt_dev.sprt_node)
        return;

    foreach_fwk_of_child(sprt_adap->sgrt_dev.sprt_node, sprt_node)
    {
        if (fwk_of_device_is_avaliable(sprt_node))
            fwk_of_i2c_register_device(sprt_adap, sprt_node);
    }
}

/*!
 * @brief   register i2c adapter
 * @param   sprt_adap
 * @retval  errno
 * @note    none
 */
kint32_t fwk_i2c_register_adapter(struct fwk_i2c_adapter *sprt_adap)
{
    kint32_t retval;

    if (!sprt_adap->sprt_algo || !(*sprt_adap->name))
        return -ER_FAULT;

    if (!sprt_adap->timeout)
        sprt_adap->timeout = TICK_HZ;

    init_list_head(&sprt_adap->sgrt_clients);
    mrt_dev_set_name(&sprt_adap->sgrt_dev, "i2c-%d", sprt_adap->nr);

    sprt_adap->sgrt_dev.sprt_bus = &sgrt_fwk_i2c_bus_type;
    sprt_adap->sgrt_dev.sprt_type = &sgrt_fwk_i2c_adapter_type;

    rw_lock_init(&sprt_adap->sgrt_lock);

    retval = fwk_device_add(&sprt_adap->sgrt_dev);
    if (retval)
        goto fail;

    fwk_of_i2c_register_devices(sprt_adap);

    return ER_NORMAL;

fail:
    mrt_dev_del_name(&sprt_adap->sgrt_dev);
    return retval;
}

/*!
 * @brief   register i2c adapter
 * @param   sprt_adap
 * @retval  errno
 * @note    none
 */
kint32_t fwk_i2c_add_adapter(struct fwk_i2c_adapter *sprt_adap)
{
    if (sprt_adap->id < 0)
    {
        sprt_adap->nr = fwk_of_get_alias_id(sprt_adap->sgrt_dev.sprt_node);
        if (sprt_adap->nr < 0)
            return sprt_adap->nr;
    }
    else
        sprt_adap->nr = sprt_adap->id;

    return fwk_i2c_register_adapter(sprt_adap);
}

/*!
 * @brief   unregister i2c adapter
 * @param   sprt_adap
 * @retval  none
 * @note    none
 */
void fwk_i2c_del_adapter(struct fwk_i2c_adapter *sprt_adap)
{
    struct fwk_i2c_client *sprt_client, *sprt_temp;

    wr_lock(&sprt_adap->sgrt_lock);

    foreach_list_next_entry_safe(sprt_client, sprt_temp, &sprt_adap->sgrt_clients, sgrt_link)
    {
        __fwk_i2c_unregister_device(sprt_client);
        kfree(sprt_client);
    }

    wr_unlock(&sprt_adap->sgrt_lock);
    fwk_device_del(&sprt_adap->sgrt_dev);
}

/*!
 * @brief   i2c transfer
 * @param   sprt_client, sprt_msgs, num (the number of sprt_msgs)
 * @retval  errno
 * @note    none
 */
kint32_t fwk_i2c_transfer(struct fwk_i2c_client *sprt_client, struct fwk_i2c_msg *sprt_msgs, kint32_t num)
{
    const struct fwk_i2c_algo *sprt_algo = sprt_client->sprt_adapter->sprt_algo;

    if (sprt_algo->master_xfer)
        return sprt_algo->master_xfer(sprt_client->sprt_adapter, sprt_msgs, num);

    return -ER_NSUPPORT;
}

/*!< end of file */
