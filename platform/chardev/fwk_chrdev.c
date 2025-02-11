/*
 * Character Device Interface
 *
 * File Name:   fwk_chrdev.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.05.19
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <platform/fwk_chrdev.h>
#include <kernel/mutex.h>

/*!< The globals */
struct fwk_char_device *sgrt_fwk_chrdevs[DEVICE_MAX_NUM];
static struct mutex_lock sgrt_fwk_chrdev_lock = MUTEX_LOCK_INIT();

/*!< The functions */
static struct fwk_char_device *__fwk_register_chrdev(kuint32_t major, kuint32_t baseminor, kuint32_t count, const kchar_t *name);
static struct fwk_char_device *__fwk_unregister_chrdev(kuint32_t major, kuint32_t baseminor, kuint32_t count);

/*!< API function */
/*!
 * @brief   char device initial
 * @param   none
 * @retval  errno
 * @note    none
 */
kint32_t __plat_init fwk_chrdev_init(void)
{
    struct fwk_char_device **sprt_chrdev;
    kusize_t  chrdevMax;
    kuint32_t i;

    sprt_chrdev	= &sgrt_fwk_chrdevs[0];
    chrdevMax 	= ARRAY_SIZE(sgrt_fwk_chrdevs);

    for (i = 0; i < chrdevMax; i++)
        *(sprt_chrdev++) = mrt_nullptr;

    return ER_NORMAL;
}

/*!
 * @brief   char device exit
 * @param   none
 * @retval  errno
 * @note    none
 */
void __plat_exit fwk_chrdev_exit(void)
{
    struct fwk_char_device **sprt_chrdev;
    struct fwk_char_device *sprt_prev;
    struct fwk_char_device *sprt_list;
    kusize_t  chrdevMax;
    kuint32_t i;

    sprt_chrdev	= &sgrt_fwk_chrdevs[0];
    chrdevMax 	= ARRAY_SIZE(sgrt_fwk_chrdevs);

    for (i = 0; i < chrdevMax; i++, sprt_chrdev++)
    {
        mrt_list_delete_all(*sprt_chrdev, sprt_prev, sprt_list);
        *sprt_chrdev = mrt_nullptr;
    }
}

/*!
 * @brief   register char device
 * @param   major, baseminor, count, name
 * @retval  errno
 * @note    none
 */
static struct fwk_char_device *__fwk_register_chrdev(kuint32_t major, kuint32_t baseminor, kuint32_t count, const kchar_t *name)
{
    struct fwk_char_device *sprt_chrdev;
    struct fwk_char_device **sprt_Dst;
    kuint32_t index;
    kusize_t chrdevMax;
    kint32_t i;

    if (!count)
        return mrt_nullptr;

    chrdevMax = ARRAY_SIZE(sgrt_fwk_chrdevs);

    if (!major)
    {
        sprt_Dst = &sgrt_fwk_chrdevs[chrdevMax - 1];

        /*!< 
         * The composite value of the Major + Minor must be less than 2^32, 
         * that is, the number of primary device numbers and secondary device numbers is limited 
         */
        for (i = (chrdevMax - 1); i >= 0; i--)
        {
            if (!(*(sprt_Dst--)))
                break;
        }

        if (i < 0)
            return mrt_nullptr;

        major = i;
    }

    if (major >= chrdevMax)
        return mrt_nullptr;

    index = major % chrdevMax;
    sprt_Dst = &sgrt_fwk_chrdevs[index];

    sprt_chrdev = (struct fwk_char_device *)kmalloc(sizeof(struct fwk_char_device), GFP_KERNEL);
    if (!isValid(sprt_chrdev))
        return mrt_nullptr;

    sprt_chrdev->major		= major;
    sprt_chrdev->baseminor	= baseminor;
    sprt_chrdev->count		= count;
    sprt_chrdev->sprt_next	= mrt_nullptr;
    do_string_n_copy((char *)sprt_chrdev->name, (const char *)name, sizeof(sprt_chrdev->name));

    /*!< If sprt_Temp[index] is empty, sprt_chrdev can be deposited directly */
    if (!(*sprt_Dst))
    {
        *sprt_Dst = sprt_chrdev;
        goto chrget;
    }

    /*!< 
     * If sprt_Temp[index] is not empty, 
     * then insert sprt_chrdev into sprt_Temp[index]->next, and arrange them in the order of baseminor from small to large 
     */
    for (sprt_Dst = sprt_Dst; *sprt_Dst; sprt_Dst = &(*sprt_Dst)->sprt_next)
    {
        /*!<
         * A new device with a smaller secondary device number than an existing device can take the place of the device, 
         * or it may not be possible to replace it because of overlap;
         * This code only provides a rough analysis of where the new device can theoretically be replaced, and is not necessarily quite right
         * If the code is proved to be false after subsequent inspection, the new device is confirmed to have overlapping scopes and will be invalidated
         */
        if (((*sprt_Dst)->baseminor >= baseminor) ||
                (((*sprt_Dst)->baseminor + (*sprt_Dst)->count - 1) >= baseminor))
        {
            break;
        }
    }

    /*!< Error verification: The applied baseminor cannot be duplicated with an existing device */
    if (*sprt_Dst)
    {
        kuint32_t first_minor = (*sprt_Dst)->baseminor;
        kuint32_t last_minor  = (*sprt_Dst)->baseminor + (*sprt_Dst)->count - 1;

        /*!< The new device is theoretically plugged in front of (*sprt_Dst) */
        /*!<
         * 1. baseminor < first_minor:
         *		If the program can be executed at this point, this must be satisfied, because the new device must be plugged in front of (*sprt_Dst)
         * 2. (baseminor + count - 1) >= first_minor:
         *  	Larger than the first_minor of the old device? 
         * 		Then the range of the new device is obviously either partially overlapped with the old device, or even completely covered the old device
         */
        if ((baseminor <= last_minor) && ((baseminor + count - 1) >= first_minor))
            goto fail;
    }

    /*!< If the device is successfully applied, plug the new device in front of (*sprt_Dst) */
    /*!<
     * Presume: b.next = &c
     * If a new device 'd' is inserted, it is located between 'b' and 'c':
     * 		a = &b.next	===> *a is equivalent to &c
     *		d.next	= &c = *a
     *		a is the address of b.next, and d is deposited to overwrite the original stored &c: *a = b.next = &d
     * Our sprt_Dst here is to play the role of 'a'
     */
    sprt_chrdev->sprt_next = (*sprt_Dst);
    *sprt_Dst = sprt_chrdev;

chrget:
    return sprt_chrdev;

fail:
    kfree(sprt_chrdev);

    return mrt_nullptr;
}

/*!
 * @brief   unregister char device
 * @param   major, baseminor, count
 * @retval  char_device *
 * @note    none
 */
static struct fwk_char_device *__fwk_unregister_chrdev(kuint32_t major, kuint32_t baseminor, kuint32_t count)
{
    struct fwk_char_device *sprt_Rlt;
    struct fwk_char_device **sprt_Dst;
    kuint32_t index;
    kusize_t  chrdevMax;

    chrdevMax = ARRAY_SIZE(sgrt_fwk_chrdevs);
    if ((major >= chrdevMax) || (!count))
        return mrt_nullptr;

    index = major % chrdevMax;
    sprt_Dst = &sgrt_fwk_chrdevs[index];

    /*!<
     * The primary device number index has been found, and the secondary device needs to be separated from the linked list
     * If the number of secondary devices and the number of secondary devices can be matched, 
     * the device will be separated and the linked list connection will be restored
     */
    for (sprt_Dst = sprt_Dst, sprt_Rlt = mrt_nullptr; *sprt_Dst; sprt_Dst = &(*sprt_Dst)->sprt_next)
    {
        if (((baseminor == (*sprt_Dst)->baseminor) && (count == (*sprt_Dst)->count)))
        {
            sprt_Rlt = *sprt_Dst;
            *sprt_Dst = (*sprt_Dst)->sprt_next;

            break;
        }
    }

    return sprt_Rlt;
}

/*!
 * @brief   char device allocation
 * @param   devNum, baseminor, count, name
 * @retval  errno
 * @note    none
 */
kint32_t fwk_alloc_chrdev(kuint32_t *devNum, kuint32_t baseminor, kuint32_t count, const kchar_t *name)
{
    struct fwk_char_device *sprt_chrdev;

    mutex_lock(&sgrt_fwk_chrdev_lock);
    sprt_chrdev = __fwk_register_chrdev(0, baseminor, count, name);
    mutex_unlock(&sgrt_fwk_chrdev_lock);

    if (!isValid(sprt_chrdev))
        return -ER_FAULT;

    *devNum	= MKE_DEV_NUM(sprt_chrdev->major, sprt_chrdev->baseminor);

    return ER_NORMAL;
}

/*!
 * @brief   register char device
 * @param   devNum, count, name
 * @retval  errno
 * @note    none
 */
kint32_t fwk_register_chrdev(kuint32_t devNum, kuint32_t count, const kchar_t *name)
{
    struct fwk_char_device *sprt_chrdev;
    kuint32_t major, last;
    kuint32_t n, next;

    last = devNum + count;

    mutex_lock(&sgrt_fwk_chrdev_lock);
    for (n = devNum; n < last; n = next)
    {
        major = GET_DEV_MAJOR(n);
        next = MKE_DEV_NUM(major + 1, 0);
        next = mrt_ret_min2(last, next);

        sprt_chrdev = __fwk_register_chrdev(major, GET_DEV_MINOR(n), next - n, name);
        if (!isValid(sprt_chrdev))
            goto fail;
    }

    mutex_unlock(&sgrt_fwk_chrdev_lock);
    return ER_NORMAL;

fail:
    mutex_unlock(&sgrt_fwk_chrdev_lock);

    /*!<
     * If the registration fails, you will need to cancel the previous registration together
     * The number of devices that have been registered: count = n - devNum
     */
    fwk_unregister_chrdev(devNum, n - devNum);

    return -ER_FAILD;
}

/*!
 * @brief   unregister char device
 * @param   devNum, count
 * @retval  errno
 * @note    none
 */
void fwk_unregister_chrdev(kuint32_t devNum, kuint32_t count)
{
    struct fwk_char_device *sprt_chrdev;
    kuint32_t major, last;
    kuint32_t n, next;

    last = devNum + count;

    mutex_lock(&sgrt_fwk_chrdev_lock);
    for (n = devNum; n < last; n = next)
    {
        major = GET_DEV_MAJOR(n);
        next = MKE_DEV_NUM(major + 1, 0);
        next = mrt_ret_min2(last, next);

        sprt_chrdev = __fwk_unregister_chrdev(major, GET_DEV_MINOR(n), next - n);
        if (isValid(sprt_chrdev))
            kfree(sprt_chrdev);
    }

    mutex_unlock(&sgrt_fwk_chrdev_lock);
}

/*!< end of file */
