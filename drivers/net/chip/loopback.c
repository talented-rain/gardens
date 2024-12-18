/*
 * Loopback Ethernet Device
 *
 * File Name:   loopback.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.12.10
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <platform/fwk_basic.h>

#include <platform/net/fwk_if.h>
#include <platform/net/fwk_netdev.h>
#include <platform/net/fwk_ip.h>
#include <platform/net/fwk_arp.h>
#include <platform/net/fwk_skbuff.h>
#include <platform/net/fwk_ether.h>
#include <platform/net/fwk_icmp.h>
#include <platform/net/fwk_udp.h>

/*!< The defines */
struct loopback_drv_data
{
    struct fwk_device sgrt_dev;
    struct fwk_net_device *sprt_ndev;
};

#define LOOPBACK_MAC_ADDR                           { 0x00, 0x0a, 0x35, 0x00, 0x01, 0x22 }

/*!< The globals */
static struct loopback_drv_data sgrt_loopback_drv_data;

/*!< The functions */

/*!< API function */
static kssize_t loopback_driver_recv(struct fwk_net_device *sprt_ndev, void *buffer, kssize_t len)
{
    struct fwk_sk_buff *sprt_skb;
    void *data;

    sprt_skb = fwk_alloc_skb(len + 2 * ARCH_PER_SIZE, GFP_KERNEL);
    if (!isValid(sprt_skb))
        return -ER_NOMEM;

    fwk_skb_reserve(sprt_skb, ARCH_PER_SIZE);
    data = fwk_skb_put(sprt_skb, len);
    if (!isValid(data))
        goto fail;

    memcpy(data, buffer, sprt_skb->len);
    sprt_skb->protocol = fwk_eth_type_trans(sprt_skb, sprt_ndev);
    sprt_skb->sprt_ndev = sprt_ndev;

    fwk_skb_set_mac_header(sprt_skb, 0);
    fwk_skb_set_network_header(sprt_skb, NET_ETHER_HDR_LEN);

    /*!< commit to rx queue */
    fwk_netif_rx(sprt_skb);

    sprt_ndev->last_rx = jiffies;
    sprt_ndev->sgrt_stats.rx_packets++;
    sprt_ndev->sgrt_stats.rx_bytes += sprt_skb->len;

    return sprt_skb->len;

fail:
    kfree_skb(sprt_skb);
    return -ER_FAILD;
}

static kssize_t loopback_driver_recycle(struct fwk_net_device *sprt_ndev, void *buffer, kssize_t len)
{
    struct fwk_eth_hdr *sprt_ethdr;
    struct fwk_ip_hdr *sprt_iphdr;
    struct fwk_arp_hdr *sprt_arphdr;
    struct fwk_icmp_hdr *sprt_icmphdr;
    struct fwk_udp_hdr *sprt_udphdr;

    kuint8_t mac_address[NET_MAC_ETH_ALEN];
    kuint32_t ipaddr;
    kuint16_t port;
    kssize_t recv_len;

    sprt_ethdr = (struct fwk_eth_hdr *)buffer;

    /*!< swap source and destination mac address */
    kmemcpy(mac_address, sprt_ethdr->h_dest, NET_MAC_ETH_ALEN);
    kmemcpy(sprt_ethdr->h_dest, sprt_ethdr->h_source, NET_MAC_ETH_ALEN);
    kmemcpy(sprt_ethdr->h_source, mac_address, NET_MAC_ETH_ALEN);

    switch (mrt_htons(sprt_ethdr->h_proto))
    {
        case NET_ETH_PROTO_IP:
            sprt_iphdr = (struct fwk_ip_hdr *)(buffer + sizeof(*sprt_ethdr));

            switch (sprt_iphdr->protocol)
            {
                case NET_IP_PROTO_ICMP:
                    /*!< ICMP <0x00: rely; 0x08: ping> */
                    sprt_icmphdr = (struct fwk_icmp_hdr *)((kuint8_t *)sprt_iphdr + sizeof(*sprt_iphdr));

                    sprt_icmphdr->type = NET_PROTO_ICMP_ER;
                    sprt_icmphdr->check_sum = fwk_icmp_check_sum(sprt_iphdr, (kuint8_t *)sprt_icmphdr);

                    break;

                case NET_IP_PROTO_UDP:
                    sprt_udphdr = (struct fwk_udp_hdr *)((kuint8_t *)sprt_iphdr + sizeof(*sprt_iphdr));
                    
                    u16_set(&port, &sprt_udphdr->dst_port);
                    u16_set(&sprt_udphdr->src_port, &sprt_udphdr->dst_port);
                    u16_set(&sprt_udphdr->dst_port, &port);

                    sprt_udphdr->check_sum = fwk_udp_check_sum(sprt_iphdr, (kuint8_t *)sprt_udphdr);

                    break;

                /*!< tcp requires establishing connection first, not suitable for loopback */
                default:
                    return -ER_UNVALID;
            }

            /*!< swap source and destination ip address */
            u32_set(&ipaddr, &sprt_iphdr->daddr);
            u32_set(&sprt_iphdr->daddr, &sprt_iphdr->saddr);
            u32_set(&sprt_iphdr->saddr, &ipaddr);

            sprt_iphdr->check = fwk_ip_check_sum(sprt_iphdr);

            break;

        case NET_ETH_PROTO_ARP:
            sprt_arphdr = (struct fwk_arp_hdr *)(buffer + sizeof(*sprt_ethdr));

            kmemcpy(mac_address, sprt_arphdr->mac_src, NET_MAC_ETH_ALEN);
            kmemcpy(sprt_arphdr->mac_src, sprt_ndev->dev_addr, NET_MAC_ETH_ALEN);
            kmemcpy(sprt_arphdr->mac_dst, mac_address, NET_MAC_ETH_ALEN);

            /*!< swap source and destination ip address */
            u32_set(&ipaddr, &sprt_arphdr->ip_dst);
            u32_set(&sprt_arphdr->ip_dst, &sprt_arphdr->ip_src);
            u32_set(&sprt_arphdr->ip_src, &ipaddr);

            sprt_arphdr->opcode = mrt_htons(NET_ARPOP_REPLY);

            break;

        default: 
            return -ER_UNVALID;
    }

    recv_len = loopback_driver_recv(sprt_ndev, buffer, len);
    return (recv_len > 0) ? len : (-ER_TRXERR);
}

static kssize_t loopback_driver_send(struct fwk_net_device *sprt_ndev, void *buffer, kssize_t len)
{
    /*!< loopback */
    return loopback_driver_recycle(sprt_ndev, buffer, len);
}

static kint32_t loopback_init(struct fwk_net_device *sprt_ndev)
{
    kuint8_t mac_address[NET_MAC_ETH_ALEN] = LOOPBACK_MAC_ADDR;

    memcpy(sprt_ndev->dev_addr, mac_address, NET_MAC_ETH_ALEN);
    return ER_NORMAL;
}

static kint32_t loopback_open(struct fwk_net_device *sprt_ndev)
{
    sprt_ndev->sgrt_stats.tx_bytes = 0;
    sprt_ndev->sgrt_stats.tx_packets = 0;
    sprt_ndev->sgrt_stats.rx_bytes = 0;
    sprt_ndev->sgrt_stats.rx_packets = 0;

    fwk_netif_start_queue(sprt_ndev);
    return ER_NORMAL;
}

static kint32_t loopback_stop(struct fwk_net_device *sprt_ndev)
{
    fwk_netif_stop_queue(sprt_ndev);

    sprt_ndev->sgrt_stats.tx_bytes = 0;
    sprt_ndev->sgrt_stats.tx_packets = 0;
    sprt_ndev->sgrt_stats.rx_bytes = 0;
    sprt_ndev->sgrt_stats.rx_packets = 0;

    return ER_NORMAL;
}

static netdev_tx_t loopback_start_xmit(struct fwk_sk_buff *sprt_skb, struct fwk_net_device *sprt_ndev)
{
    netdev_tx_t size;

    fwk_netif_stop_queue(sprt_ndev);
    sprt_ndev->sgrt_stats.tx_packets++;
    sprt_ndev->sgrt_stats.tx_bytes += sprt_skb->len;

    size = loopback_driver_send(sprt_ndev, sprt_skb->data, sprt_skb->len);
    kfree_skb(sprt_skb);

    fwk_netif_wake_queue(sprt_ndev);
    return size;
}

static const struct fwk_netdev_ops sgrt_loopback_drv_oprts =
{
    .ndo_init = loopback_init,
    .ndo_open = loopback_open,
    .ndo_stop = loopback_stop,
    .ndo_start_xmit = loopback_start_xmit,
};

static void loopback_driver_setup(struct fwk_net_device *sprt_ndev)
{
    sprt_ndev->mtu = 1500;
    sprt_ndev->sprt_netdev_oprts = &sgrt_loopback_drv_oprts;

    fwk_eth_random_addr(sprt_ndev->dev_addr);
    fwk_eth_broadcast_addr(sprt_ndev->broadcast);

    sprt_ndev->tx_queue_len = 1000;
    sprt_ndev->hard_header_len = NET_ETHER_HDR_LEN;
    sprt_ndev->min_header_len = NET_ETHER_HDR_LEN;
}

/*!
 * @brief   loopback_driver_probe
 * @param   sprt_data
 * @retval  errno
 * @note    none
 */
static kint32_t loopback_driver_probe(struct loopback_drv_data *sprt_data)
{
    struct fwk_net_device *sprt_ndev;
    kint32_t retval;

    sprt_ndev = fwk_alloc_netdev(0, "lo", loopback_driver_setup);
    if (!isValid(sprt_ndev))
        return -ER_FAILD;
    
    sprt_ndev->private_data = sprt_data;
    sprt_data->sprt_ndev = sprt_ndev;

    retval = fwk_register_netdevice(sprt_ndev);
    if (retval)
        goto fail;

    print_info("register a new netdevice (Loopback)\n");
    return ER_NORMAL;

fail:
    fwk_free_netdev(sprt_ndev);
    return -ER_FAILD;
}

/*!
 * @brief   loopback_driver_remove
 * @param   sprt_data
 * @retval  errno
 * @note    none
 */
static kint32_t loopback_driver_remove(struct loopback_drv_data *sprt_data)
{
    struct fwk_net_device *sprt_ndev;

    sprt_ndev = sprt_data->sprt_ndev;
    if (!isValid(sprt_ndev))
        return -ER_NULLPTR;

    fwk_unregister_netdevice(sprt_ndev);
    fwk_free_netdev(sprt_ndev);

    return ER_NORMAL;
}

/*!< --------------------------------------------------------------------- */
/*!
 * @brief   loopback_driver_init
 * @param   none
 * @retval  errno
 * @note    none
 */
kint32_t __fwk_init loopback_driver_init(void)
{
    return loopback_driver_probe(&sgrt_loopback_drv_data);
}

/*!
 * @brief   loopback_driver_exit
 * @param   none
 * @retval  none
 * @note    none
 */
void __fwk_exit loopback_driver_exit(void)
{
    loopback_driver_remove(&sgrt_loopback_drv_data);
}

IMPORT_DRIVER_INIT(loopback_driver_init);
IMPORT_DRIVER_EXIT(loopback_driver_exit);

/*!< end of file */
