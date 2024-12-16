/*
 * NetWork Interface
 *
 * File Name:   fwk_lwip.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.11.23
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/net/fwk_if.h>
#include <platform/net/fwk_netdev.h>
#include <platform/net/fwk_skbuff.h>
#include <platform/net/fwk_netif.h>

#include <kernel/thread.h>
#include <kernel/sched.h>

/*!< The defines */

/*!< The globals */
static struct fwk_sk_buff_head sgrt_fwk_skb_rx_lists;

/*!< API functions */
/*!< 
 * input format can be: 
 *      "192.168.253.231"
 *      "0xC0.0xA8.0xFD.0xE7"
 *      "0xc0.0xa8.0xfd.0xe7"
 * output format:
 *      (192 << 24) | (168 << 16) | (253 << 8) | 231
 */
kuint32_t fwk_inet_addr(const kchar_t *addr)
{
    kchar_t temp[12];
    kchar_t *p, *str;
    kuint8_t len = 0;
    kuint32_t val = 0, offset = 3, byte = 0;

    str = (kchar_t *)addr;
    for (p = str; str && (*str); p++) 
    {
        len = (kuint8_t)(p - str);

        if ((*p == '.') || (*p == '\0')) 
        {
            if (!len)
                goto fail;

            strncpy(temp, str, len);
            str = p + 1;
            temp[len] = '\0';

            byte = ascii_to_dec(temp);
            /*!< 0 ~ 255 */
            if (byte > 255)
                goto fail;

            val |= (byte << (8 * offset));
            if (!offset) 
            {
                /*!< avoid the 4th '.' exsits*/
                if (*p)
                    goto fail;

                break;
            }

            if (!(*p) || !(*str))
                goto fail;

            offset--;
            continue;
        }

        if (len > sizeof(temp))
            goto fail;
    }

    return val;

fail:
    print_err("%s: input argument error!\n", __FUNCTION__);
    return 0;
}

void fwk_inet_random_addr(kuint8_t *buf, kusize_t lenth)
{
    for (kuint32_t idx; idx < lenth; idx++)
        *(buf++) = (kuint8_t)random_val();
}

struct fwk_sk_buff_head *fwk_netif_rxq_get(void)
{
    return &sgrt_fwk_skb_rx_lists;
}

kint32_t fwk_netif_open(const kchar_t *name)
{
    struct fwk_net_device *sprt_ndev;
    const struct fwk_netdev_ops *sprt_ops;

    sprt_ndev = fwk_ifname_to_ndev(name);
    if (!isValid(sprt_ndev))
        return PTR_ERR(sprt_ndev);

    sprt_ops = sprt_ndev->sprt_netdev_oprts;
    if (sprt_ops->ndo_open)
    {
        if (sprt_ops->ndo_open(sprt_ndev))
            return -ER_FAILD;
    }

    return ER_NORMAL;
}

kint32_t fwk_netif_close(const kchar_t *name)
{
    struct fwk_net_device *sprt_ndev;
    const struct fwk_netdev_ops *sprt_ops;

    sprt_ndev = fwk_ifname_to_ndev(name);
    if (!isValid(sprt_ndev))
        return PTR_ERR(sprt_ndev);

    sprt_ops = sprt_ndev->sprt_netdev_oprts;
    if (sprt_ops->ndo_stop)
    {
        if (sprt_ops->ndo_stop(sprt_ndev))
            return -ER_FAILD;
    }

    return ER_NORMAL;
}

kint32_t fwk_netif_ioctl(kuint32_t request, kuaddr_t args)
{
    struct fwk_ifreq *sprt_ifr;
    struct fwk_net_device *sprt_ndev;
    const struct fwk_netdev_ops *sprt_ops;

    sprt_ifr = (struct fwk_ifreq *)args;
    sprt_ndev = fwk_ifname_to_ndev(sprt_ifr->mrt_ifr_name);
    if (!isValid(sprt_ndev))
        return PTR_ERR(sprt_ndev);

    sprt_ops = sprt_ndev->sprt_netdev_oprts;
    if (sprt_ops->ndo_do_ioctl)
    {
        if (!sprt_ops->ndo_do_ioctl(sprt_ndev, sprt_ifr, (kint32_t)request))
            goto out;
    }

    /*!< default operations */
    switch (request)
    {
        case NETWORK_IFR_GET_HWADDR:
            memcpy(sprt_ifr->mrt_ifr_hwaddr.sa_data, sprt_ndev->dev_addr, NET_MAC_ETH_ALEN);
            break;

        case NETWORK_IFR_GET_MTU:
            sprt_ifr->mrt_ifr_mtu = sprt_ndev->mtu;
            break;

        default:
            return -ER_UNVALID;
    }

out:
    return ER_NORMAL;
}

static netdev_tx_t __fwk_dev_queue_xmit(struct fwk_sk_buff *sprt_skb)
{
    struct fwk_net_device *sprt_ndev;
    const struct fwk_netdev_ops *sprt_ops;
    netdev_tx_t retval;
    
    sprt_ndev = sprt_skb->sprt_ndev;
    sprt_ops = sprt_ndev->sprt_netdev_oprts;

    if (!sprt_ops->ndo_start_xmit)
        return -ER_TRXERR;

    retval = sprt_ops->ndo_start_xmit(sprt_skb, sprt_ndev);
    return retval;
}

kint32_t fwk_dev_queue_xmit(struct fwk_sk_buff *sprt_skb)
{
    return __fwk_dev_queue_xmit(sprt_skb);
}

kint32_t fwk_netif_rx(struct fwk_sk_buff *sprt_skb)
{
    if (!fwk_skb_enqueue(fwk_netif_rxq_get(), sprt_skb))
        schedule_thread_wakeup(REAL_THREAD_TID_SOCKRX);

    return ER_NORMAL;
}

/*!< ----------------------------------------------------------------------- */
struct fwk_netif_tcb
{
    void *args;
    void (*pfunc_rx)(void *rxq, void *args);
};

static void *fwk_netif_rx_entry(void *args)
{
    struct fwk_netif_tcb *sprt_tcb;
    struct fwk_sk_buff_head *sprt_head;

    sprt_head = fwk_netif_rxq_get();
    sprt_tcb = (struct fwk_netif_tcb *)args;

    for (;;)
    {
        if (mrt_skbuff_list_empty(sprt_head))
            goto END;

        if (sprt_tcb->pfunc_rx)
            sprt_tcb->pfunc_rx(sprt_head, sprt_tcb->args);

    END:
        schedule_self_suspend();
    }

    return args;
}

void fwk_netif_init(void (*pfunc_rx)(void *rxq, void *args), void *args)
{
    struct fwk_netif_tcb *sprt_tcb;
    struct fwk_sk_buff_head *sprt_head;

    sprt_tcb = kmalloc(sizeof(*sprt_tcb), GFP_KERNEL);
    if (!isValid(sprt_tcb))
        return;

    sprt_tcb->args = args;
    sprt_tcb->pfunc_rx = pfunc_rx;

    sprt_head = fwk_netif_rxq_get();
    fwk_skb_list_init(sprt_head);

    kernel_thread_create(REAL_THREAD_TID_SOCKRX, mrt_nullptr, fwk_netif_rx_entry, sprt_tcb);
    real_thread_set_priority(mrt_tid_attr(REAL_THREAD_TID_SOCKRX), REAL_THREAD_PROTY_SOCKRX);
}

/*!< end of file */
