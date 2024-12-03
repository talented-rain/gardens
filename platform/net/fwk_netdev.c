/*
 * Hardware Abstraction Layer Net Interface
 *
 * File Name:   fwk_netdev.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.07.06
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/net/fwk_netdev.h>
#include <platform/fwk_platform.h>
#include <platform/fwk_platdev.h>

/*!< The defines */
#define NETDEV_IF_INS_MAX                   ((kuint32_t)256U)

/*!< The globals */
static DECLARE_LIST_HEAD(sgrt_fwk_net_device_list);
static kuint32_t g_fwk_allocated_ins[mrt_num_align(NETDEV_IF_INS_MAX, RET_BITS_PER_INT) / RET_BITS_PER_INT] = { 0 };

/*!< API function */
static kint32_t fwk_net_validate_name(struct fwk_net_device *sprt_ndev)
{
    kchar_t new_name[NET_IFNAME_SIZE];
    kchar_t *name = sprt_ndev->name;
    kchar_t *p;
    kint32_t index = sprt_ndev->ifindex;

    p = kstrchr(name, '%');
    if (!p)
        return ER_NORMAL;

    *(name + (kuint32_t)(p - name)) = '\0';
    if (!(*(name + 1)) || ((*(name + 1)) != 'd'))
        goto END;

    if (index < 0)
    {
        index = bitmap_find_first_zero_bit(g_fwk_allocated_ins, 0, NETDEV_IF_INS_MAX);
        if (index < 0)
            goto END;

        bitmap_set_nr_bit_valid(g_fwk_allocated_ins, index, NETDEV_IF_INS_MAX, 1);
        sprt_ndev->ifindex = index;
        sprt_ndev->priv_flags |= NR_NETDEV_PRIV_AINDEX;
    }

    sprintk(new_name, "%s%d", name, index);
    strncpy(name, new_name, NET_IFNAME_SIZE);

END:
    return ER_NORMAL;
}

static void fwk_net_invalidate_name(struct fwk_net_device *sprt_ndev)
{
    kchar_t old_name[NET_IFNAME_SIZE];
    kchar_t *name = sprt_ndev->name;
    kchar_t *p;
    kint32_t index = sprt_ndev->ifindex;

    if (sprt_ndev->priv_flags & NR_NETDEV_PRIV_AINDEX)
    {
        memset(old_name, 0, NET_IFNAME_SIZE);
        sprintk(old_name, "%d", index);

        p = name + strlen(name) - strlen(old_name);
        *p = '%';
        *(p + 1) = '\0';

        bitmap_set_nr_bit_zero(g_fwk_allocated_ins, index, NETDEV_IF_INS_MAX, 1);
        sprt_ndev->ifindex = -1;
        sprt_ndev->priv_flags &= (~NR_NETDEV_PRIV_AINDEX);
    }
}

/*!
 * @brief   Allocate network device
 * @param   none
 * @retval  none
 * @note    none
 */
struct fwk_net_device *fwk_alloc_netdev_mq(kint32_t sizeof_priv, const kchar_t *name,
                                    void (*setup) (struct fwk_net_device *sprt_ndev), kuint32_t txqs)
{
    struct fwk_net_device *sprt_netdev;
    struct fwk_netdev_queue *sprt_tx;
    kint32_t alloc_size = 0;

    if (sizeof_priv > 0)
    {
        alloc_size = mrt_align(sizeof(*sprt_netdev), sizeof(kutype_t));
        alloc_size += sizeof_priv;
    }

    sprt_netdev = (struct fwk_net_device *)kzalloc(alloc_size, GFP_KERNEL);
    if (!isValid(sprt_netdev))
        return ERR_PTR(-ER_NOMEM);

    sprt_tx = (struct fwk_netdev_queue *)kzalloc(txqs * sizeof(*sprt_tx), GFP_KERNEL);
    if (!isValid(sprt_tx))
    {
        kfree(sprt_netdev);
        return ERR_PTR(-ER_NOMEM);
    }

    /*!< register send queue */
    sprt_netdev->sprt_tx = sprt_tx;
    sprt_netdev->num_tx_queues = txqs;
    sprt_netdev->real_num_tx_queues = txqs;
    sprt_netdev->private_data = ((void *)sprt_netdev) + mrt_align(sizeof(*sprt_netdev), sizeof(kutype_t));
    sprt_netdev->ifindex = -1;

    /*!< initial recieve queue */
    memset(&sprt_netdev->sgrt_rcu, 0, sizeof(struct fwk_netdev_queue));
    kstrncpy(sprt_netdev->name, name, NET_IFNAME_SIZE);
    init_list_head(&sprt_netdev->sgrt_link);

    if (setup)
        setup(sprt_netdev);

    return sprt_netdev;
}

/*!
 * @brief   release network device
 * @param   none
 * @retval  none
 * @note    none
 */
void fwk_free_netdev(struct fwk_net_device *sprt_ndev)
{
    if (!isValid(sprt_ndev))
        return;

    /*!< release send queue first */
    if (isValid(sprt_ndev->sprt_tx))
        kfree(sprt_ndev->sprt_tx);

    kfree(sprt_ndev);
}

struct fwk_net_device *fwk_find_netdevice(const kchar_t *name)
{
    struct fwk_net_device *sprt_ndev;

    foreach_list_next_entry(sprt_ndev, &sgrt_fwk_net_device_list, sgrt_link)
    {
        if (!strcmp(sprt_ndev->name, name))
            return sprt_ndev;
    }

    return mrt_nullptr;
}

kint32_t fwk_register_netdevice(struct fwk_net_device *sprt_ndev)
{
    struct fwk_device *sprt_dev;
    kint32_t retval;

    if (!sprt_ndev)
        return -ER_NODEV;

    if (!mrt_list_head_empty(&sprt_ndev->sgrt_link))
        return -ER_CHECKERR;

    fwk_net_validate_name(sprt_ndev);
    if (fwk_find_netdevice(sprt_ndev->name))
        return -ER_EXISTED;

    if (sprt_ndev->sprt_netdev_oprts->ndo_init)
    {
        retval = sprt_ndev->sprt_netdev_oprts->ndo_init(sprt_ndev);
        if (retval)
            return retval;
    }

    sprt_dev = &sprt_ndev->sgrt_dev;
    if (fwk_device_initial(sprt_dev))
        goto fail1;
    
    mrt_dev_set_name(sprt_dev, "%s", sprt_ndev->name);
    if (fwk_device_add(sprt_dev))
        goto fail2;

    list_head_add_tail(&sgrt_fwk_net_device_list, &sprt_ndev->sgrt_link);
    return ER_NORMAL;

fail2:
    mrt_dev_del_name(sprt_dev);

fail1:
    if (sprt_ndev->sprt_netdev_oprts->ndo_uninit)
        sprt_ndev->sprt_netdev_oprts->ndo_uninit(sprt_ndev);

    return -ER_FAILD;
}

kint32_t fwk_unregister_netdevice(struct fwk_net_device *sprt_ndev)
{
    struct fwk_device *sprt_dev;

    if (!sprt_ndev)
        return -ER_NODEV;

    if (mrt_list_head_empty(&sprt_ndev->sgrt_link))
        return -ER_CHECKERR;

    if (!fwk_find_netdevice(sprt_ndev->name))
        return -ER_NODEV;

    sprt_dev = &sprt_ndev->sgrt_dev;
    mrt_dev_del_name(sprt_dev);
    if (sprt_ndev->sprt_netdev_oprts->ndo_uninit)
        sprt_ndev->sprt_netdev_oprts->ndo_uninit(sprt_ndev);

    fwk_net_invalidate_name(sprt_ndev);
    list_head_del(&sprt_ndev->sgrt_link);

    return ER_NORMAL;
}

/*!< end of file */
