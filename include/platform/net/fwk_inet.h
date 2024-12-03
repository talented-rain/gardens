/*
 * Hardware Abstraction Layer Net Interface
 *
 * File Name:   fwk_inet.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.12.03
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_INET_H_
#define __FWK_INET_H_

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/net/fwk_if.h>
#include <platform/net/fwk_skbuff.h>

/*!< The defines */


/*!< The functions */
TARGET_EXT kuint32_t fwk_inet_addr(const kchar_t *addr);

TARGET_EXT kint32_t fwk_netif_open(const kchar_t *name);
TARGET_EXT kint32_t fwk_netif_close(const kchar_t *name);
TARGET_EXT kint32_t fwk_netif_ioctl(kuint32_t request, kuaddr_t args);
TARGET_EXT kint32_t fwk_netif_rx(struct fwk_sk_buff *sprt_skb);

TARGET_EXT kint32_t fwk_dev_queue_xmit(struct fwk_sk_buff *sprt_skb);
TARGET_EXT struct pq_queue *fwk_dev_queue_poll(void);

#endif /*!< __FWK_INET_H_ */
