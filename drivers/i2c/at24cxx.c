/*
 * Template of EEPROM Driver : at24cxx
 *
 * File Name:   at24cxx.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.06.07
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/of/fwk_of.h>
#include <platform/of/fwk_of_device.h>
#include <platform/fwk_platdrv.h>
#include <platform/fwk_uaccess.h>
#include <platform/gpio/fwk_gpiodesc.h>
#include <platform/i2c/fwk_i2c_dev.h>
#include <platform/i2c/fwk_i2c_core.h>
#include <platform/i2c/fwk_i2c_algo.h>
#include <platform/fwk_cdev.h>
#include <platform/fwk_chrdev.h>
#include <platform/fwk_inode.h>
#include <platform/fwk_fs.h>
#include <platform/fwk_fcntl.h>
#include <platform/i2c/fwk_eeprom.h>
#include <kernel/sleep.h>
#include <kernel/spinlock.h>

/*!< The defines */
#define AT24CXX_DRVIVER_MAJOR                       (222)

#define AT24C01_PAGE_SIZE                           (8)
#define AT24C01_PAGE_COUNT                          (16)

/*!<
 * at24c01: 1kbits = 128bytes
 * at24cxx: 2kbits = 256bytes
 */
#define AT24CXX_PAGE_OFFSET(addr, size)             ((addr) / (size))

typedef struct at24cxx_drv_info
{
    kchar_t *name;
    struct fwk_i2c_client *sprt_client;

    kint32_t devnum;
    struct fwk_cdev *sprt_cdev;
    struct fwk_device *sprt_idev;

    kuint32_t page_size;
    kuint32_t total_size;

} at24cxx_drv_info_t;

/*!< API function */
/*!
 * @brief   at24cxx write
 * @param   sprt_info
 * @retval  errno
 * @note    none
 */
static kint32_t at24cxx_write_eeprom(struct at24cxx_drv_info *sprt_info, kuint8_t *buffer, kssize_t size)
{
    struct fwk_i2c_msg sgrt_msgs;

    /*!< keep a certain interval between two writes */
    msleep(100);

    sgrt_msgs.addr = sprt_info->sprt_client->addr;
    sgrt_msgs.flags = 0;
    sgrt_msgs.ptr_buf = buffer;
    sgrt_msgs.len = size;

    return fwk_i2c_transfer(sprt_info->sprt_client, &sgrt_msgs, 1);
}

/*!
 * @brief   at24cxx read
 * @param   sprt_info
 * @retval  errno
 * @note    none
 */
static kint32_t at24cxx_read_eeprom(struct at24cxx_drv_info *sprt_info, kuint8_t *buffer, kssize_t size)
{
    struct fwk_i2c_msg sgrt_msgs[2];

    sgrt_msgs[0].addr = sprt_info->sprt_client->addr;
    sgrt_msgs[0].flags = 0;
    sgrt_msgs[0].ptr_buf = buffer;
    sgrt_msgs[0].len = 1;

    sgrt_msgs[1].addr = sprt_info->sprt_client->addr;
    sgrt_msgs[1].flags |= FWK_I2C_M_RD;
    sgrt_msgs[1].ptr_buf = buffer + 1;
    sgrt_msgs[1].len = size - 1;

    return fwk_i2c_transfer(sprt_info->sprt_client, &sgrt_msgs[0], ARRAY_SIZE(sgrt_msgs));
}

/*!
 * @brief   at24cxx write
 * @param   sprt_info
 * @retval  errno
 * @note    none
 */
static kint32_t __at24cxx_driver_write(struct at24cxx_drv_info *sprt_info, struct fwk_eeprom *sprt_eep)
{
    kuint8_t *buffer, *ptr_from;
    kuint32_t end_addr, last_left, last_page;
    kuint32_t first_page, first_left, pages;
    kuint32_t idx;
    kint32_t retval = ER_NORMAL;

    if (!sprt_eep->buf || !sprt_eep->size)
        return -ER_EMPTY;

    end_addr = sprt_eep->addr + sprt_eep->size;
    if (end_addr >= sprt_info->total_size)
        return -ER_MORE;

    buffer = kmalloc(sprt_eep->size + 1, GFP_KERNEL);
    if (!isValid(buffer))
        return -ER_NOMEM;

    ptr_from = sprt_eep->buf;
    first_left = last_left = 0;

    /*!< get the page offset of the address will be read */
    last_page = AT24CXX_PAGE_OFFSET(end_addr, sprt_info->page_size);
    last_left = end_addr - last_page * sprt_info->page_size;

    first_page = AT24CXX_PAGE_OFFSET(sprt_eep->addr, sprt_info->page_size);
    if (first_page != last_page)
        first_left = (first_page + 1) * sprt_info->page_size - sprt_eep->addr;

    pages = (last_page > (first_page + 1)) ? (last_page - first_page - 1) : 0;
    
    if (first_left)
    {
        fwk_copy_from_user(buffer + 1, ptr_from, first_left);

        *buffer = (kuint8_t)sprt_eep->addr + (kuint8_t)(ptr_from - sprt_eep->buf);
        retval = at24cxx_write_eeprom(sprt_info, buffer, first_left + 1);
        if (retval)
            goto END;

        ptr_from += first_left;
    }

    for (idx = 0; idx < pages; idx++)
    {
        ptr_from += idx * sprt_info->page_size;
        fwk_copy_from_user(buffer + 1, ptr_from, sprt_info->page_size);

        *buffer = (kuint8_t)sprt_eep->addr + (kuint8_t)(ptr_from - sprt_eep->buf);
        retval = at24cxx_write_eeprom(sprt_info, buffer, sprt_info->page_size + 1);
        if (retval)
            goto END;
    }
    ptr_from += idx * sprt_info->page_size;

    if (last_left)
    {
        fwk_copy_from_user(buffer + 1, ptr_from, last_left);

        *buffer = (kuint8_t)sprt_eep->addr + (kuint8_t)(ptr_from - sprt_eep->buf);
        retval = at24cxx_write_eeprom(sprt_info, buffer, last_left + 1);
        if (retval)
            goto END;
    }

END:
    kfree(buffer);
    return retval ? retval : sprt_eep->size;
}

/*!
 * @brief   at24cxx read
 * @param   sprt_info
 * @retval  errno
 * @note    none
 */
static kint32_t __at24cxx_driver_read(struct at24cxx_drv_info *sprt_info, struct fwk_eeprom *sprt_eep)
{
    kuint8_t *buffer, *ptr_to;
    kuint32_t end_addr, last_left, last_page;
    kuint32_t first_page, first_left, pages;
    kuint32_t idx;
    kint32_t retval = ER_NORMAL;

    if (!sprt_eep->buf || !sprt_eep->size)
        return -ER_EMPTY;

    end_addr = sprt_eep->addr + sprt_eep->size;
    if (end_addr >= sprt_info->total_size)
        return -ER_MORE;

    buffer = kmalloc(sprt_eep->size + 1, GFP_KERNEL);
    if (!isValid(buffer))
        return -ER_NOMEM;

    ptr_to = sprt_eep->buf;
    first_left = last_left = 0;

    /*!< get the page offset of the address will be read */
    last_page = AT24CXX_PAGE_OFFSET(end_addr, sprt_info->page_size);
    last_left = end_addr - last_page * sprt_info->page_size;

    first_page = AT24CXX_PAGE_OFFSET(sprt_eep->addr, sprt_info->page_size);
    if (first_page != last_page)
        first_left = (first_page + 1) * sprt_info->page_size - sprt_eep->addr;

    pages = (last_page > (first_page + 1)) ? (last_page - first_page - 1) : 0;
    
    if (first_left)
    {
        *buffer = (kuint8_t)sprt_eep->addr + (kuint8_t)(ptr_to - sprt_eep->buf);
        retval = at24cxx_read_eeprom(sprt_info, buffer, first_left + 1);
        if (retval)
            goto END;

        fwk_copy_to_user(ptr_to, buffer + 1, first_left);
        ptr_to += first_left;
    }

    for (idx = 0; idx < pages; idx++)
    {
        ptr_to += idx * sprt_info->page_size;

        *buffer = (kuint8_t)sprt_eep->addr + (kuint8_t)(ptr_to - sprt_eep->buf);
        retval = at24cxx_read_eeprom(sprt_info, buffer, sprt_info->page_size + 1);
        if (retval)
            goto END;

        fwk_copy_to_user(ptr_to, buffer + 1, sprt_info->page_size);
    }
    ptr_to += idx * sprt_info->page_size;

    if (last_left)
    {
        *buffer = (kuint8_t)sprt_eep->addr + (kuint8_t)(ptr_to - sprt_eep->buf);
        retval = at24cxx_read_eeprom(sprt_info, buffer, last_left + 1);
        if (retval)
            goto END;

        fwk_copy_to_user(ptr_to, buffer + 1, last_left);
    }

END:
    kfree(buffer);
    return retval ? retval : sprt_eep->size;
}

/*!
 * @brief   driver open
 * @param   sprt_inode, sprt_file
 * @retval  errno
 * @note    none
 */
static kint32_t at24cxx_driver_open(struct fwk_inode *sprt_inode, struct fwk_file *sprt_file)
{
    struct at24cxx_drv_info *sprt_info;

    sprt_info = sprt_inode->sprt_cdev->privData;
    sprt_file->private_data = sprt_info;

    return ER_NORMAL;
}

/*!
 * @brief   driver close
 * @param   sprt_inode, sprt_file
 * @retval  errno
 * @note    none
 */
static kint32_t at24cxx_driver_close(struct fwk_inode *sprt_inode, struct fwk_file *sprt_file)
{
    sprt_file->private_data = mrt_nullptr;

    return ER_NORMAL;
}

/*!
 * @brief   driver ioctl
 * @param   sprt_file, cmd, args
 * @retval  errno
 * @note    none
 */
static kint32_t at24cxx_driver_ioctl(struct fwk_file *sprt_file, kuint32_t cmd, kuaddr_t args)
{
    struct at24cxx_drv_info *sprt_info;
    struct fwk_eeprom *sprt_eep;
    kint32_t retval = ER_NORMAL;
    
    sprt_info = sprt_file->private_data;
    sprt_eep = (struct fwk_eeprom *)args;

    switch (cmd)
    {
        case FWK_EEPROM_READ:
            retval = __at24cxx_driver_read(sprt_info, sprt_eep);
            break;

        case FWK_EEPROM_WRITE:
            retval = __at24cxx_driver_write(sprt_info, sprt_eep);
            break;

        default: break;
    }

    return (retval < 0) ? retval : ER_NORMAL;
}

static const struct fwk_file_oprts sgrt_at24cxx_driver_oprts =
{
    .open = at24cxx_driver_open,
    .close = at24cxx_driver_close,
    .unlocked_ioctl = at24cxx_driver_ioctl,
};

/*!< --------------------------------------------------------------------- */
/*!
 * @brief   at24cxx_driver_probe
 * @param   sprt_client, sprt_id
 * @retval  errno
 * @note    none
 */
static kint32_t at24cxx_driver_probe(struct fwk_i2c_client *sprt_client, const struct fwk_i2c_device_id *sprt_id)
{
    struct at24cxx_drv_info *sprt_info;
    struct fwk_device_node *sprt_node;
    struct fwk_device *sprt_idev;
    kint32_t devnum;
    kuint32_t pages, page_size;

    sprt_node = sprt_client->sgrt_dev.sprt_node;

    sprt_info = kzalloc(sizeof(*sprt_info), GFP_KERNEL);
    if (!isValid(sprt_info))
        return -ER_NOMEM;

    devnum = MKE_DEV_NUM(AT24CXX_DRVIVER_MAJOR, 0);
    sprt_info->devnum = devnum;
    sprt_info->name = "at24cxx";
    sprt_info->sprt_client = sprt_client;
    
    if (fwk_of_property_read_u32(sprt_node, "page_size", &page_size))
        page_size = AT24C01_PAGE_SIZE;

    if (fwk_of_property_read_u32(sprt_node, "pages", &pages))
        pages = AT24C01_PAGE_COUNT;

    sprt_info->page_size  = page_size;
    sprt_info->total_size = page_size * pages;

    if (fwk_register_chrdev(devnum, 1, sprt_info->name))
        goto fail1;

    sprt_info->sprt_cdev = fwk_cdev_alloc(&sgrt_at24cxx_driver_oprts);
    if (!isValid(sprt_info->sprt_cdev))
        goto fail2;

    if (fwk_cdev_add(sprt_info->sprt_cdev, devnum, 1))
        goto fail3;

    sprt_idev = fwk_device_create(NR_TYPE_CHRDEV, devnum, "%s", sprt_node->name);
    if (!isValid(sprt_idev))
        goto fail4;

    sprt_info->sprt_idev = sprt_idev;
    sprt_info->sprt_cdev->privData = sprt_info;
    fwk_i2c_set_client_data(sprt_client, sprt_info);

	return ER_NORMAL;
    
fail4:
    fwk_cdev_del(sprt_info->sprt_cdev);
fail3:
    kfree(sprt_info->sprt_cdev);
fail2:
    fwk_unregister_chrdev(devnum, 0);
fail1:
    kfree(sprt_info);

    return -ER_ERROR;
}

/*!
 * @brief   at24cxx_driver_remove
 * @param   sprt_client
 * @retval  errno
 * @note    none
 */
static kint32_t at24cxx_driver_remove(struct fwk_i2c_client *sprt_client)
{
    struct at24cxx_drv_info *sprt_info;

    sprt_info = fwk_i2c_get_client_data(sprt_client);

    fwk_device_destroy(sprt_info->sprt_idev);
    fwk_cdev_del(sprt_info->sprt_cdev);
    kfree(sprt_info->sprt_cdev);
    fwk_unregister_chrdev(sprt_info->devnum, 0);
    kfree(sprt_info);
    fwk_i2c_set_client_data(sprt_client, mrt_nullptr);

    return ER_NORMAL;
}

static const struct fwk_i2c_device_id sgrt_at24cxx_driver_ids[] =
{
    { .name = "at24cxx", .driver_data = -1 },
    {},
};

/*!< device id for device-tree */
static const struct fwk_of_device_id sgrt_at24cxx_driver_dts[] =
{
	{ .compatible = "atmel,at24cxx", },
	{},
};

/*!< platform instance */
static struct fwk_i2c_driver sgrt_at24cxx_driver =
{
	.probe	= at24cxx_driver_probe,
	.remove	= at24cxx_driver_remove,

    .sprt_id_table = sgrt_at24cxx_driver_ids,
	
	.sgrt_driver =
	{
		.name 	= "at24cxx,eeprom",
		.id 	= -1,
		.sprt_of_match_table = sgrt_at24cxx_driver_dts,
	},
};

/*!< --------------------------------------------------------------------- */
/*!
 * @brief   at24cxx_driver_init
 * @param   none
 * @retval  errno
 * @note    none
 */
kint32_t __fwk_init at24cxx_driver_init(void)
{
	return fwk_i2c_add_driver(&sgrt_at24cxx_driver);
}

/*!
 * @brief   at24cxx_driver_exit
 * @param   none
 * @retval  none
 * @note    none
 */
void __fwk_exit at24cxx_driver_exit(void)
{
	fwk_i2c_del_driver(&sgrt_at24cxx_driver);
}

IMPORT_DRIVER_INIT(at24cxx_driver_init);
IMPORT_DRIVER_EXIT(at24cxx_driver_exit);

/*!< end of file */
