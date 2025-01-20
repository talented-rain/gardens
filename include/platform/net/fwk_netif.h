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

#ifdef __cplusplus
    extern "C" {
#endif

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
extern kuint32_t fwk_inet_addr(const kchar_t *addr);
extern void fwk_inet_random_addr(kuint8_t *buf, kusize_t lenth);
extern kuint16_t fwk_ip_network_csum(struct fwk_ip_hdr *sprt_iphdr);
extern kuint16_t fwk_ip_transport_csum(struct fwk_ip_hdr *sprt_iphdr, kuint8_t *msg);
extern kuint16_t fwk_ip_slow_csum(struct fwk_ip_hdr *sprt_iphdr, kuint16_t offset);

extern struct fwk_sk_buff_head *fwk_netif_rxq_get(void);

extern kint32_t fwk_netif_open(const kchar_t *name);
extern kint32_t fwk_netif_close(const kchar_t *name);
extern kint32_t fwk_netif_ioctl(kuint32_t request, kuaddr_t args);
extern kint32_t fwk_netif_rx(struct fwk_sk_buff *sprt_skb);
extern kint32_t fwk_dev_queue_xmit(struct fwk_sk_buff *sprt_skb);

extern void fwk_netif_init(void (*pfunc_rx)(void *rxq, void *args), void *args);

/*!< API functions */
/*!
 * @brief   start tx queue
 * @param   sprt_ndev
 * @retval  none
 * @note    none
 */
static inline void fwk_netif_start_queue(struct fwk_net_device *sprt_ndev)
{

}

/*!
 * @brief   stop tx queue
 * @param   sprt_ndev
 * @retval  none
 * @note    none
 */
static inline void fwk_netif_stop_queue(struct fwk_net_device *sprt_ndev)
{

}

/*!
 * @brief   restart tx queue
 * @param   sprt_ndev
 * @retval  none
 * @note    none
 */
static inline void fwk_netif_wake_queue(struct fwk_net_device *sprt_ndev)
{

}

#ifdef __cplusplus
    }
#endif

#endif /*!< __FWK_NETIF_H_ */
