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
#include <platform/net/fwk_ip.h>
#include <platform/net/fwk_icmp.h>
#include <platform/net/fwk_udp.h>
#include <platform/net/fwk_tcp.h>

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
    for (kuint32_t idx = 0; idx < lenth; idx++)
        *(buf++) = (kuint8_t)random_val();
}

kuint16_t fwk_ip_check_sum(struct fwk_ip_hdr *sprt_iphdr)
{
    kuint16_t data, idx;
    kuint32_t chksum = 0;
    kuint8_t *msg = (kuint8_t *)sprt_iphdr;

    sprt_iphdr->check = 0;
    for (idx = 0; idx < NET_IP_HDR_LEN; idx += 2) {
        data = (msg[idx] << 8) | msg[idx + 1];
        chksum += data;
    }

    chksum = ((chksum & 0xffff0000) >> 16) + (chksum & 0x0000ffff);
    chksum = 0xffff - chksum;

    return mrt_htons((kuint16_t)chksum);
}

kuint16_t fwk_icmp_check_sum(struct fwk_ip_hdr *sprt_iphdr, kuint8_t *icmp_msg)
{
    kuint16_t data, idx;
    kuint16_t data_len;
    kuint32_t chksum = 0;
    struct fwk_icmp_hdr *sprt_icmphdr;

    data_len = sprt_iphdr->tot_len - NET_IP_HDR_LEN;

    sprt_icmphdr = (struct fwk_icmp_hdr *)icmp_msg;
    sprt_icmphdr->check_sum = 0;

    for (idx = 0; idx < data_len; idx += 2) {
        data = (icmp_msg[idx] << 8) | icmp_msg[idx + 1];
        chksum += data;
    }

    chksum = ((chksum & 0xffff0000) >> 16) + (chksum & 0x0000ffff);
    chksum = 0xffff - chksum;

    return mrt_htons((kuint16_t)chksum);
}

kuint16_t fwk_tcp_check_sum(struct fwk_ip_hdr *sprt_iphdr, kuint8_t *tcp_msg)
{
    kuint16_t data, idx;
    kuint32_t chksum = 0;
    struct fwk_tcp_fakehdr sgrt_fhdr;
    struct fwk_tcp_hdr *sprt_tcphdr;
    kuint8_t *fake_msg;

    sgrt_fhdr.saddr = sprt_iphdr->saddr;
    sgrt_fhdr.daddr = sprt_iphdr->daddr;
    sgrt_fhdr.proto = sprt_iphdr->protocol;
    sgrt_fhdr.zero = 0;
    sgrt_fhdr.len = sprt_iphdr->tot_len - NET_IP_HDR_LEN;

    fake_msg = (kuint8_t *)&sgrt_fhdr;
    for (idx = 0; idx < NET_TCP_FAKE_HDR_LEN; idx += 2) {
        data = (fake_msg[idx] << 8) | fake_msg[idx + 1];
        chksum += data;
    }

    sprt_tcphdr = (struct fwk_tcp_hdr *)tcp_msg;
    sprt_tcphdr->check_sum = 0;

    for (idx = 0; idx < sgrt_fhdr.len; idx += 2) {
        data = (tcp_msg[idx] << 8) | tcp_msg[idx + 1];
        chksum += data;
    }

    chksum = ((chksum & 0xffff0000) >> 16) + (chksum & 0x0000ffff);
    chksum = 0xffff - chksum;

    return mrt_htons((kuint16_t)chksum);
}

kuint16_t fwk_udp_check_sum(struct fwk_ip_hdr *sprt_iphdr, kuint8_t *udp_msg)
{
    kuint16_t data, idx;
    kuint32_t chksum = 0;
    struct fwk_udp_fakehdr sgrt_fhdr;
    struct fwk_udp_hdr *sprt_udphdr;
    kuint8_t *fake_msg;

    u32_set(&sgrt_fhdr.saddr, &sprt_iphdr->saddr);
    u32_set(&sgrt_fhdr.daddr, &sprt_iphdr->daddr);
    u8_set(&sgrt_fhdr.proto, &sprt_iphdr->protocol);
    u16_set(&sgrt_fhdr.len, &sprt_iphdr->tot_len);

    sgrt_fhdr.zero = 0;
    sgrt_fhdr.len -= NET_IP_HDR_LEN;

    fake_msg = (kuint8_t *)&sgrt_fhdr;
    for (idx = 0; idx < NET_UDP_FAKE_HDR_LEN; idx += 2) {
        data = (fake_msg[idx] << 8) | fake_msg[idx + 1];
        chksum += data;
    }

    sprt_udphdr = (struct fwk_udp_hdr *)udp_msg;
    sprt_udphdr->check_sum = 0;

    for (idx = 0; idx < sgrt_fhdr.len; idx += 2) {
        data = (udp_msg[idx] << 8) | udp_msg[idx + 1];
        chksum += data;
    }

    chksum = ((chksum & 0xffff0000) >> 16) + (chksum & 0x0000ffff);
    chksum = 0xffff - chksum;

    return mrt_htons((kuint16_t)chksum);
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
