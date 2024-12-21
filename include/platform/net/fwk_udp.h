/*
 * Hardware Abstraction Layer Net Interface
 *
 * File Name:   fwk_udp.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.12.07
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_UDP_H_
#define __FWK_UDP_H_

/*!< The includes */
#include <platform/net/fwk_if.h>
#include <platform/net/fwk_ip.h>

/*!< The defines */
#define NET_UDP_HDR_LEN                         (8)
#define NET_UDP_FAKE_HDR_LEN                    (NET_IP_FAKE_HDR_LEN)

struct fwk_udp_hdr
{
    kuint16_t src_port;
    kuint16_t dst_port;
    kuint16_t hdr_len;
    kuint16_t check_sum;
    
} __packed;

#endif /*!< __FWK_UDP_H_ */
