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
/*!
 * @brief   convert ip string to integer
 * @param   ip addr
 * @retval  ip
 * @note    example:
 *          input format can be: 
 *              "192.168.253.231"
 *              "0xC0.0xA8.0xFD.0xE7"
 *              "0xc0.0xa8.0xfd.0xe7"
 *          output format:
 *              (192 << 24) | (168 << 16) | (253 << 8) | 231
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

/*!
 * @brief   set random value
 * @param   buf (destination value)
 * @retval  none
 * @note    none
 */
void fwk_inet_random_addr(kuint8_t *buf, kusize_t lenth)
{
    for (kuint32_t idx = 0; idx < lenth; idx++)
        *(buf++) = (kuint8_t)random_val();
}

/*!
 * @brief   prepare to calculate the checksum (ICMP)
 * @param   msg (icmp message)
 * @retval  check sum
 * @note    none
 */
static kuint32_t __fwk_icmp_csum_before(kuint8_t *msg)
{
    struct fwk_icmp_hdr *sprt_icmphdr;

    sprt_icmphdr = (struct fwk_icmp_hdr *)msg;
    sprt_icmphdr->check_sum = 0;

    return 0;
}

/*!
 * @brief   prepare to calculate the checksum (TCP/UDP)
 * @param   sprt_iphdr, msg (tcp/udp message)
 * @retval  check sum
 * @note    none
 */
static kuint32_t __fwk_tcp_udp_csum_before(struct fwk_ip_hdr *sprt_iphdr, kuint8_t *msg)
{
    kuint16_t data, idx;
    kuint16_t data_len;
    kuint32_t chksum = 0;
    struct fwk_ip_fakehdr sgrt_fhdr;
    kuint8_t *fake_msg;

    data_len = mrt_htons(sprt_iphdr->tot_len) - sprt_iphdr->ihl * 4;

    sgrt_fhdr.saddr = sprt_iphdr->saddr;
    sgrt_fhdr.daddr = sprt_iphdr->daddr;
    sgrt_fhdr.proto = sprt_iphdr->protocol;
    sgrt_fhdr.zero = 0;

    sgrt_fhdr.len = mrt_htons(data_len);
    fake_msg = (kuint8_t *)&sgrt_fhdr;

    for (idx = 0; idx < sizeof(sgrt_fhdr); idx += 2) 
    {
        data = (fake_msg[idx] << 8) | fake_msg[idx + 1];
        chksum += data;
    }

    if (sprt_iphdr->protocol == NET_IP_PROTO_TCP)
    {
        struct fwk_tcp_hdr *sprt_tcphdr = (struct fwk_tcp_hdr *)msg;
        sprt_tcphdr->check_sum = 0;
    }
    else
    {
        struct fwk_udp_hdr *sprt_udphdr = (struct fwk_udp_hdr *)msg;
        sprt_udphdr->check_sum = 0;
    }

    return chksum;
}

/*!
 * @brief   prepare to calculate the checksum (IP)
 * @param   sprt_iphdr
 * @retval  check sum
 * @note    none
 */
kuint16_t fwk_ip_network_csum(struct fwk_ip_hdr *sprt_iphdr)
{
    kuint16_t data, idx;
    kuint32_t chksum = 0;
    kuint8_t *msg = (kuint8_t *)sprt_iphdr;

    sprt_iphdr->check = 0;
    for (idx = 0; idx < (sprt_iphdr->ihl * 4); idx += 2) 
    {
        data = (msg[idx] << 8) | msg[idx + 1];
        chksum += data;
    }

    chksum = ((chksum & 0xffff0000) >> 16) + (chksum & 0x0000ffff);
    return mrt_htons((kuint16_t)(~chksum));
}

/*!
 * @brief   prepare to calculate the checksum (ICMP/UDP/TCP)
 * @param   sprt_iphdr, msg (tcp/udp message)
 * @retval  check sum
 * @note    none
 */
kuint16_t fwk_ip_transport_csum(struct fwk_ip_hdr *sprt_iphdr, kuint8_t *msg)
{
    kuint16_t data, idx;
    kuint16_t data_len;
    kuint32_t chksum = 0;
    kbool_t is_odd;

    switch (sprt_iphdr->protocol)
    {
        case NET_IP_PROTO_UDP:
        case NET_IP_PROTO_TCP:
            chksum = __fwk_tcp_udp_csum_before(sprt_iphdr, msg);
            break;

        case NET_IP_PROTO_ICMP:
            chksum = __fwk_icmp_csum_before(msg);
            break;

        default: break;
    }

    data_len = mrt_htons(sprt_iphdr->tot_len) - sprt_iphdr->ihl * 4;
    is_odd = !!(data_len & 0x01);

    data_len &= ~(kuint16_t)0x01;
    for (idx = 0; idx < data_len; idx += 2) 
    {
        data = (msg[idx] << 8) | msg[idx + 1];
        chksum += data;
    }

    if (is_odd)
        chksum += msg[data_len];

    chksum = ((chksum & 0xffff0000) >> 16) + (chksum & 0x0000ffff);
    return mrt_htons((kuint16_t)(~chksum));
}

/*!
 * @brief   prepare to calculate the checksum
 * @param   sprt_iphdr, offset (0 or sprt_iphdr->ihl)
 * @retval  check sum
 * @note    none
 */
kuint16_t fwk_ip_slow_csum(struct fwk_ip_hdr *sprt_iphdr, kuint16_t offset)
{
    if (offset < sprt_iphdr->ihl)
        return fwk_ip_network_csum(sprt_iphdr);

    return fwk_ip_transport_csum(sprt_iphdr, ((kuint8_t *)sprt_iphdr) + (sprt_iphdr->ihl * 4));
}

/*!
 * @brief   get global rx list
 * @param   none
 * @retval  rx list
 * @note    none
 */
struct fwk_sk_buff_head *fwk_netif_rxq_get(void)
{
    return &sgrt_fwk_skb_rx_lists;
}

/*!< -------------------------------------------------------------------- */
/*!
 * @brief   network open (called by net_link_up())
 * @param   name
 * @retval  errno
 * @note    name ---> sprt_ndev ---> driver_open
 */
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

/*!
 * @brief   network close (called by net_link_down())
 * @param   name
 * @retval  errno
 * @note    name ---> sprt_ndev ---> driver_stop
 */
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

/*!
 * @brief   network ioctl (called by virt_ioctl())
 * @param   request (cmd), args (address of buffer)
 * @retval  errno
 * @note    none
 */
kint32_t fwk_netif_ioctl(kuint32_t request, kuaddr_t args)
{
    struct fwk_ifreq *sprt_ifr;
    struct fwk_net_device *sprt_ndev;
    const struct fwk_netdev_ops *sprt_ops;

    sprt_ifr = (struct fwk_ifreq *)args;
    sprt_ndev = fwk_ifname_to_ndev(sprt_ifr->mrt_ifr_name);
    if (!isValid(sprt_ndev))
        return PTR_ERR(sprt_ndev);

    /*!< if sprt_ops->ndo_do_ioctl defined, use sprt_ops->ndo_do_ioctl */
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

/*!
 * @brief   send skb
 * @param   sprt_skb
 * @retval  size sent
 * @note    none
 */
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

/*!
 * @brief   send skb
 * @param   sprt_skb
 * @retval  size sent
 * @note    none
 */
kint32_t fwk_dev_queue_xmit(struct fwk_sk_buff *sprt_skb)
{
    return __fwk_dev_queue_xmit(sprt_skb);
}

/*!
 * @brief   add skb received to global rx list
 * @param   sprt_skb
 * @retval  errno
 * @note    the function will wake up rx_thread
 */
kint32_t fwk_netif_rx(struct fwk_sk_buff *sprt_skb)
{
    if (!fwk_skb_enqueue(fwk_netif_rxq_get(), sprt_skb))
        schedule_thread_wakeup(THREAD_TID_SOCKRX);

    return ER_NORMAL;
}

/*!< ----------------------------------------------------------------------- */
struct fwk_netif_tcb
{
    void *args;
    void (*pfunc_rx)(void *rxq, void *args);
};

/*!
 * @brief   rx thread
 * @param   args (for callback function)
 * @retval  args
 * @note    if skb_list is empty, sleep all the time
 */
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

/*!
 * @brief   netif initalization
 * @param   pfunc_rx: callback
 * @param   args (for callback function)
 * @retval  none
 * @note    create rx thread (only one for per cpu)
 */
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

    kernel_thread_create(THREAD_TID_SOCKRX, mrt_nullptr, fwk_netif_rx_entry, sprt_tcb);
    thread_set_priority(mrt_tid_attr(THREAD_TID_SOCKRX), THREAD_PROTY_SOCKRX);
}

/*!< end of file */
