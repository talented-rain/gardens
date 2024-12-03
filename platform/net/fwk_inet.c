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

/*!< The defines */

/*!< The globals */


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

kint32_t fwk_netif_open(const kchar_t *name)
{
    struct fwk_net_device *sprt_ndev;
    const struct fwk_netdev_ops *sprt_ops;

    sprt_ndev = fwk_find_netdevice(name);
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

    sprt_ndev = fwk_find_netdevice(name);
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
    sprt_ndev = fwk_find_netdevice(sprt_ifr->mrt_ifr_name);
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
            break;

        case NETWORK_IFR_GET_MTU:

            break;

        default:
            return -ER_UNVALID;
    }

out:
    return ER_NORMAL;
}

static kint32_t __fwk_dev_queue_xmit(struct fwk_sk_buff *sprt_skb)
{
    struct fwk_net_device *sprt_ndev = sprt_skb->sprt_netdev;
    const struct fwk_netdev_ops *sprt_ops = sprt_ndev->sprt_netdev_oprts;
    struct fwk_sk_buff *sprt_per;
    netdev_tx_t retval, total_size = 0;

    if (!sprt_ops->ndo_start_xmit)
        return -ER_TRXERR;
    
    for (sprt_per = sprt_skb; sprt_per; sprt_per = sprt_skb->sprt_next)
    {
        retval = sprt_ops->ndo_start_xmit(sprt_per, sprt_ndev);
        if (retval < 0)
            return -ER_SEND_FAILD;

        total_size += retval;
    }

    return total_size;
}

kint32_t fwk_dev_queue_xmit(struct fwk_sk_buff *sprt_skb)
{
    return __fwk_dev_queue_xmit(sprt_skb);
}

kint32_t fwk_netif_rx(struct fwk_sk_buff *sprt_skb)
{
    return 0;
}

struct pq_queue *fwk_dev_queue_poll(void)
{
    return mrt_nullptr;
}

/*!< end of file */
