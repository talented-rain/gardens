/*
 * Hardware Abstraction Layer Net Interface
 *
 * File Name:   fwk_icmp.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.12.07
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_ICMP_H_
#define __FWK_ICMP_H_

/*!< The includes */
#include <platform/net/fwk_if.h>
#include <platform/net/fwk_ip.h>

/*!< The defines */
#define NET_ICMP_HDR_LEN                        (8)

/*!< type */
#define NET_PROTO_ICMP_ER                       (0x00)      /*!< echo reply */
#define NET_PROTO_ICMP_DUR                      (0x03)      /*!< destination unreachable */
#define NET_PROTO_ICMP_SQ                       (0x04)      /*!< source quench */
#define NET_PROTO_ICMP_RD                       (0x05)      /*!< redirect */
#define NET_PROTO_ICMP_ECHO                     (0x08)      /*!< echo (ping) */
#define NET_PROTO_ICMP_TE                       (0x0b)      /*!< time exceeded */
#define NET_PROTO_ICMP_PP                       (0x0c)      /*!< parameter problem */
#define NET_PROTO_ICMP_TS                       (0x0d)      /*!< timestamp */
#define NET_PROTO_ICMP_TSR                      (0x0e)      /*!< timestamp reply */
#define NET_PROTO_ICMP_IRQ                      (0x0f)      /*!< information request */
#define NET_PROTO_ICMP_IR                       (0x10)      /*!< information reply */
#define NET_PROTO_ICMP_AM                       (0x12)      /*!< address mask request */
#define NET_PROTO_ICMP_AMR                      (0x13)      /*!< address mask reply */

struct fwk_icmp_echo
{
    kuint16_t id;
    kuint16_t seq_no;
};

struct fwk_icmp_hdr
{
    kuint8_t type;
    kuint8_t code;
    kuint16_t check_sum;

    union {
        kuint32_t info;
        struct fwk_icmp_echo sgrt_echo;
    } u;
    
} __packed;

#endif /*!< __FWK_ICMP_H_ */
