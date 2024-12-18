/*
 * Hardware Abstraction Layer Net Interface
 *
 * File Name:   fwk_netif.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.12.03
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_NETIF_H_
#define __FWK_NETIF_H_

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/net/fwk_if.h>
#include <platform/net/fwk_netdev.h>
#include <platform/net/fwk_skbuff.h>

/*!< The defines */
struct fwk_ip_hdr;

#define mrt_htons(x)                mrt_cpu_to_be16(x)
#define mrt_ntohs(x)                mrt_be16_to_cpu(x)
#define mrt_htonl(x)                mrt_cpu_to_be32(x)
#define mrt_ntohl(x)                mrt_be32_to_cpu(x)

/*!< The functions */
TARGET_EXT kuint32_t fwk_inet_addr(const kchar_t *addr);
TARGET_EXT void fwk_inet_random_addr(kuint8_t *buf, kusize_t lenth);
TARGET_EXT kuint16_t fwk_ip_check_sum(struct fwk_ip_hdr *sprt_iphdr);
TARGET_EXT kuint16_t fwk_icmp_check_sum(struct fwk_ip_hdr *sprt_iphdr, kuint8_t *icmp_msg);
TARGET_EXT kuint16_t fwk_tcp_check_sum(struct fwk_ip_hdr *sprt_iphdr, kuint8_t *tcp_msg);
TARGET_EXT kuint16_t fwk_udp_check_sum(struct fwk_ip_hdr *sprt_iphdr, kuint8_t *udp_msg);

TARGET_EXT struct fwk_sk_buff_head *fwk_netif_rxq_get(void);

TARGET_EXT kint32_t fwk_netif_open(const kchar_t *name);
TARGET_EXT kint32_t fwk_netif_close(const kchar_t *name);
TARGET_EXT kint32_t fwk_netif_ioctl(kuint32_t request, kuaddr_t args);
TARGET_EXT kint32_t fwk_netif_rx(struct fwk_sk_buff *sprt_skb);
TARGET_EXT kint32_t fwk_dev_queue_xmit(struct fwk_sk_buff *sprt_skb);

TARGET_EXT void fwk_netif_init(void (*pfunc_rx)(void *rxq, void *args), void *args);

/*!< API functions */
static inline void fwk_netif_start_queue(struct fwk_net_device *sprt_ndev)
{

}

static inline void fwk_netif_stop_queue(struct fwk_net_device *sprt_ndev)
{

}

static inline void fwk_netif_wake_queue(struct fwk_net_device *sprt_ndev)
{

}

#endif /*!< __FWK_NETIF_H_ */
