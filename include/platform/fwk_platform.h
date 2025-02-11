/*
 * Platform Bus Interface of Hardware Abstraction Layer
 *
 * File Name:   fwk_platform.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.05.25
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_BUSTYPE_H_
#define __FWK_BUSTYPE_H_

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/fwk_kobj.h>
#include <platform/of/fwk_of.h>
#include <platform/irq/fwk_irq_types.h>
#include <kernel/rw_lock.h>

/*!< The defines */
struct fwk_pinctrl_dev_info;
struct fwk_dev_pm_ops;

typedef struct fwk_SysPrivate
{
    struct fwk_bus_type *sprt_bus;

    /*!< Device list */
    struct list_head sgrt_list_devices;
    struct rw_lock sgrt_device_lock;

    /*!< Driver list */
    struct list_head sgrt_list_drivers;
    struct rw_lock sgrt_driver_lock;
    
} srt_fwk_SysPrivate_t;

typedef struct fwk_device
{
    kchar_t *init_name;
    struct fwk_device *sprt_parent;

    struct fwk_bus_type *sprt_bus;
    struct fwk_device_type *sprt_type;
    struct list_head sgrt_link;
    struct list_head sgrt_leaf;

    struct fwk_driver *sprt_driver;
    struct fwk_kobject sgrt_kobj;

    struct fwk_device_node *sprt_node;
    kint32_t (*release)	(struct fwk_device *sprt_dev);

    struct fwk_pinctrl_dev_info *sprt_pctlinfo;
    void *privData;

} srt_fwk_device_t;

typedef struct fwk_driver
{
    kchar_t *name;
    kint32_t id;

    kint32_t matches;

    const struct fwk_of_device_id *sprt_of_match_table;

    struct fwk_bus_type *sprt_bus;
    struct list_head sgrt_link;

    kint32_t (*probe)	(struct fwk_device *sprt_dev);
    kint32_t (*remove)	(struct fwk_device *sprt_dev);

    struct fwk_device_oprts *sprt_oprts;

} srt_fwk_driver_t;

typedef struct fwk_bus_type
{
    kchar_t *name;

    kint32_t (*match)	(struct fwk_device *sprt_dev, struct fwk_driver *sprt_driver);
    kint32_t (*probe)	(struct fwk_device *sprt_dev);
    kint32_t (*remove)	(struct fwk_device *sprt_dev);

    struct fwk_SysPrivate *sprt_SysPriv;

} srt_fwk_bus_type_t;

typedef struct fwk_device_type 
{
    const kchar_t *name;
    void (*release)(struct fwk_device *sprt_dev);

    const struct fwk_dev_pm_ops *sprt_pm;

} srt_fwk_device_type_t;

#define FWK_GET_BUS_DEVICE(bus)								(&(bus)->sprt_SysPriv->sgrt_list_devices)
#define FWK_GET_BUS_DRIVER(bus)								(&(bus)->sprt_SysPriv->sgrt_list_drivers)

#define FWK_INIT_BUS_DEVICE_LIST(parent, list, bus)	\
{	\
    parent 	= FWK_GET_BUS_DEVICE(bus);	\
    list	= parent;	\
}
#define FWK_INIT_BUS_DRIVER_LIST(parent, list, bus)	\
{	\
    parent 	= FWK_GET_BUS_DRIVER(bus);	\
    list	= parent;	\
}
#define FWK_NEXT_DEVICE(parent, list)						mrt_list_head_parent(parent, list, struct fwk_device, sgrt_link)
#define FWK_NEXT_DRIVER(parent, list)						mrt_list_head_parent(parent, list, struct fwk_driver, sgrt_link)

#define __BUS_DEVICE_RD_LOCK(bus)                           rd_lock(&(bus)->sprt_SysPriv->sgrt_device_lock)
#define __BUS_DEVICE_RD_UNLOCK(bus)                         rd_unlock(&(bus)->sprt_SysPriv->sgrt_device_lock)
#define __BUS_DEVICE_WR_LOCK(bus)                           wr_lock(&(bus)->sprt_SysPriv->sgrt_device_lock)
#define __BUS_DEVICE_WR_UNLOCK(bus)                         wr_unlock(&(bus)->sprt_SysPriv->sgrt_device_lock)

#define __BUS_DRIVER_RD_LOCK(bus)                           rd_lock(&(bus)->sprt_SysPriv->sgrt_driver_lock)
#define __BUS_DRIVER_RD_UNLOCK(bus)                         rd_unlock(&(bus)->sprt_SysPriv->sgrt_driver_lock)
#define __BUS_DRIVER_WR_LOCK(bus)                           wr_lock(&(bus)->sprt_SysPriv->sgrt_driver_lock)
#define __BUS_DRIVER_WR_UNLOCK(bus)                         wr_unlock(&(bus)->sprt_SysPriv->sgrt_driver_lock)

/*!< The globals */
extern struct fwk_bus_type sgrt_fwk_platform_bus_type;

/*!< The functions */
extern kint32_t fwk_device_driver_probe(struct fwk_device *sprt_dev);
extern kint32_t fwk_device_driver_remove(struct fwk_device *sprt_dev);
extern kint32_t fwk_device_driver_match(struct fwk_device *sprt_dev, struct fwk_bus_type *sprt_bus_type, void *ptr_data);

extern kint32_t fwk_device_initial(struct fwk_device *sprt_dev);
extern struct fwk_device *fwk_device_create(kuint32_t type, kuint32_t devNum, kchar_t *fmt, ...);
extern kint32_t fwk_device_destroy(struct fwk_device *sprt_dev);

/*!< API functions */
/*!
 * @brief   get device name
 * @param   sprt_dev
 * @retval  name
 * @note    none
 */
static inline kchar_t *fwk_dev_get_name(struct fwk_device *sprt_dev)
{
    if (sprt_dev->init_name)
        return sprt_dev->init_name;

    return fwk_kobject_get_name(&sprt_dev->sgrt_kobj);
}

/*!
 * @brief   set device name
 * @param   sprt_dev, name
 * @retval  none
 * @note    none
 */
static inline void fwk_dev_set_name(struct fwk_device *sprt_dev, kchar_t *name, ...)
{
    va_list sprt_list;

    va_start(sprt_list, name);
    fwk_kobject_set_name_args(&sprt_dev->sgrt_kobj, name, sprt_list);
    va_end(sprt_list);
}

/*!
 * @brief   release device name
 * @param   sprt_dev
 * @retval  none
 * @note    none
 */
static inline void fwk_dev_del_name(struct fwk_device *sprt_dev)
{
    fwk_kobject_del_name(&sprt_dev->sgrt_kobj);
}

#define mrt_dev_get_name(dev)								fwk_dev_get_name(dev)
#define mrt_dev_set_name(dev, fmt, ...)						fwk_dev_set_name(dev, fmt, ##__VA_ARGS__)
#define mrt_dev_del_name(dev)								fwk_dev_del_name(dev)

#ifdef __cplusplus
    }
#endif

#endif /*!< __FWK_BUSTYPE_H_ */
