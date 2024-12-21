/*
 * Platform Object Maps Defines
 *
 * File Name:   fwk_kbojmap.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.05.18
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <platform/fwk_kobj.h>

/*!< The globals */
struct fwk_kobj_map *sprt_fwk_chrdev_map;
struct fwk_kobj_map *sprt_fwk_blkdev_map;

/*!< API function */
/*!
 * @brief   fwk_kobjmap_init
 * @param   none
 * @retval  none
 * @note    none
 */
kint32_t __plat_init fwk_kobjmap_init(void)
{
    kuint32_t i;

    sprt_fwk_chrdev_map = (struct fwk_kobj_map *)kzalloc(sizeof(struct fwk_kobj_map), GFP_KERNEL);
    if (!isValid(sprt_fwk_chrdev_map))
        goto fail1;

    sprt_fwk_blkdev_map = (struct fwk_kobj_map *)kzalloc(sizeof(struct fwk_kobj_map), GFP_KERNEL);
    if (!isValid(sprt_fwk_blkdev_map))
        goto fail2;

    /*!< Initialize the character device matching table */
    for (i = 0; i < ARRAY_SIZE(sprt_fwk_chrdev_map->sprt_probes); i++)
        sprt_fwk_chrdev_map->sprt_probes[i] = mrt_nullptr;

    /*!< Initialize the block device matching table */
    for (i = 0; i < ARRAY_SIZE(sprt_fwk_blkdev_map->sprt_probes); i++)
        sprt_fwk_blkdev_map->sprt_probes[i] = mrt_nullptr;

    return ER_NORMAL;

fail2:
    kfree(sprt_fwk_chrdev_map);
    sprt_fwk_chrdev_map = mrt_nullptr;
fail1:
    return -ER_NOMEM;
}

/*!
 * @brief   fwk_kobjmap_del
 * @param   none
 * @retval  none
 * @note    none
 */
void __plat_exit fwk_kobjmap_del(void)
{
    struct fwk_probes *sprt_prev;
    struct fwk_probes *sprt_list;
    kusize_t mapsize;
    kuint32_t i;

    /*!< Destroy the character device matching table */
    mapsize	= isValid(sprt_fwk_chrdev_map) ? ARRAY_SIZE(sprt_fwk_chrdev_map->sprt_probes) : 0;
    for (i = 0; i < mapsize; i++)
    {
        mrt_list_delete_all(sprt_fwk_chrdev_map->sprt_probes[i],
                                        sprt_prev, sprt_list);
        sprt_fwk_chrdev_map->sprt_probes[i] = mrt_nullptr;
    }
    kfree(sprt_fwk_chrdev_map);
    sprt_fwk_chrdev_map = mrt_nullptr;

    /*!< Destroy the block device matching table */
    mapsize	= isValid(sprt_fwk_blkdev_map) ? ARRAY_SIZE(sprt_fwk_blkdev_map->sprt_probes) : 0;
    for (i = 0; i < mapsize; i++)
    {
        mrt_list_delete_all(sprt_fwk_blkdev_map->sprt_probes[i],
                                        sprt_prev, sprt_list);
        sprt_fwk_blkdev_map->sprt_probes[i] = mrt_nullptr;
    }
    kfree(sprt_fwk_blkdev_map);
    sprt_fwk_blkdev_map = mrt_nullptr;
}

/*!
 * @brief   fwk_kobj_map
 * @param   none
 * @retval  none
 * @note    none
 */
kint32_t fwk_kobj_map(struct fwk_kobj_map *domain, kuint32_t devNum, kuint32_t range, void *data)
{
    struct fwk_probes *sprt_probe;
    kuint32_t major;
    kuint32_t majorCnt;
    kuint32_t i;
    kusize_t probeMax;

    if (!isValid(domain) || (!isValid(data)))
        return -ER_FAULT;

    /*!< The maximum number of primary devices that can be supported */
    probeMax = ARRAY_SIZE(domain->sprt_probes);

    /*!< The number of master devices, that is, the number of array members occupied by probes */
    major = GET_DEV_MAJOR(devNum);
    major = mrt_ret_min2(probeMax, major);
    majorCnt = GET_DEV_MAJOR(devNum + range) - major + 1;
    majorCnt = CMP_GT2(major + majorCnt, probeMax, probeMax - major, majorCnt);

    sprt_probe = (struct fwk_probes *)kzalloc(sizeof(struct fwk_probes) * majorCnt, GFP_KERNEL);
    if (!isValid(sprt_probe))
        return -ER_NOMEM;

    for (i = 0; i < majorCnt; i++)
    {
        struct fwk_probes *sprt_Temp;
        struct fwk_probes **sprt_Dst;
        kuint32_t index;

        index = (major + i) % probeMax;
        sprt_Dst = &domain->sprt_probes[index];
        sprt_Temp = &sprt_probe[i];

        /*!<
         * Regardless of whether the master devices are the same, as long as the same driver calls this API, the data of the master devices is the same
         * Each sprt_Temp belongs to the same (devNum ~ devNum + range) range, and the data is the same; and:
         * 		1) kobj doesn't need to worry about invalid or duplicate device numbers, as this step has already been done in the fwk_register_chrdev
         * 		2) Apply for a sprt_Temp according to the main device number, the purpose is to fill the probes array, 
         * 		so as to facilitate the quick location of the master device number;
         * 		3) When looking for a device, you just need to determine if the device number is located in (devNum ~ devNum + range), 
         * 		you don't have to think about anything else, because:
         * 			fwk_register_chrdev have ensured that this range of device numbers cannot be illegal!
         */
        sprt_Temp->data = data;
        sprt_Temp->devNum = devNum;
        sprt_Temp->range = range;

        /*!< Sorting: Sort by range, from smallest to largest */
        /*!< Actually, the ranking here doesn't make much sense, and it's okay not to rank */
        while ((isValid(*sprt_Dst)) && ((*sprt_Dst)->range < range))
            sprt_Dst = &(*(sprt_Dst))->sprt_next;

        sprt_Temp->sprt_next = *sprt_Dst;
        *sprt_Dst = sprt_Temp;
    }

    return ER_NORMAL;
}

/*!
 * @brief   fwk_kobj_unmap
 * @param   none
 * @retval  none
 * @note    none
 */
kint32_t fwk_kobj_unmap(struct fwk_kobj_map *domain, kuint32_t devNum, kuint32_t range)
{
    struct fwk_probes *sprt_Rlt;
    kuint32_t major;
    kuint32_t majorCnt;
    kuint32_t i;
    kusize_t probeMax;

    if (!isValid(domain))
        return -ER_FAULT;

    /*!< The maximum number of primary devices that can be supported */
    probeMax = ARRAY_SIZE(domain->sprt_probes);

    /*!< The number of master devices, that is, the number of array members occupied by probes */
    major = GET_DEV_MAJOR(devNum);
    major = mrt_ret_min2(probeMax, major);
    majorCnt = GET_DEV_MAJOR(devNum + range) - major + 1;
    majorCnt = CMP_GT2(major + majorCnt, probeMax, probeMax - major, majorCnt);;

    for (i = 0, sprt_Rlt = mrt_nullptr; i < majorCnt; i++)
    {
        struct fwk_probes *sprt_Temp;
        struct fwk_probes **sprt_Dst;
        kuint32_t index;

        index = (major + i) % probeMax;
        sprt_Dst = &domain->sprt_probes[index];

        while (*sprt_Dst)
        {
            if (((*sprt_Dst)->devNum == devNum) && ((*sprt_Dst)->range == range))
            {
                /*!< Delete the list node */
                sprt_Temp = *sprt_Dst;
                *sprt_Dst = sprt_Temp->sprt_next;

                sprt_Rlt = sprt_Rlt ? sprt_Rlt : sprt_Temp;
                break;
            }

            sprt_Dst = &(*(sprt_Dst))->sprt_next;
        }
    }

    if (sprt_Rlt)
        kfree(sprt_Rlt);

    return ER_NORMAL;
}

/*!
 * @brief   fwk_kobjmap_lookup
 * @param   none
 * @retval  none
 * @note    none
 */
void *fwk_kobjmap_lookup(struct fwk_kobj_map *domain, kuint32_t devNum)
{
    struct fwk_probes *sprt_Temp;
    kuint32_t index;
    void *data;
    kusize_t probeMax;

    if (!isValid(domain))
        return mrt_nullptr;

    /*!< The maximum number of primary devices that can be supported */
    probeMax = ARRAY_SIZE(domain->sprt_probes);
    index = GET_DEV_MAJOR(devNum);

    for (sprt_Temp = domain->sprt_probes[index % probeMax]; sprt_Temp; sprt_Temp = sprt_Temp->sprt_next)
    {
        /*!< Already found? */
        /*!< It is determined by the range of device numbers */
        if ((devNum >= sprt_Temp->devNum) && (devNum <= (sprt_Temp->devNum + sprt_Temp->range - 1)))
            break;
    }

    data = sprt_Temp ? sprt_Temp->data : mrt_nullptr;

    return (data);
}


/*!< end of file */
