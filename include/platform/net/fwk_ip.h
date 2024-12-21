/*
 * Hardware Abstraction Layer Net Interface
 *
 * File Name:   fwk_ip.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.07.06
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_IP_H_
#define __FWK_IP_H_

/*!< The includes */
#include <platform/net/fwk_if.h>

/*!< The defines */
/*!< IP Version */
#define NET_IPV4_FLAG										(0x04)

#define NET_IP_HDR_LEN                                      (20)
#define NET_IP_FAKE_HDR_LEN                                 (12)

/*!< Trasport Protocol */
#define NET_IP_PROTO_ICMP                                   (0x01)
#define NET_IP_PROTO_IGMP                                   (0x02)
#define NET_IP_PROTO_UDP                                    (0x11)
#define NET_IP_PROTO_UDPLITE                                (0x88)
#define NET_IP_PROTO_TCP                                    (0x06)

struct fwk_ip_fakehdr
{
    kuint32_t saddr;                                        /*!< source ip address */
    kuint32_t daddr;                                        /*!< destination IP address */
    kuint8_t zero;                                          /*!< 0 */
    kuint8_t proto;                                         /*!< NET_IP_PROTO_UDP/NET_IP_PROTO_TCP */
    kuint16_t len;                                          /*!< NET_UDP_HDR_LEN/NET_TCP_HDR_LEN + data_len */
    
} __packed;

/*!< IP information Header */
struct fwk_ip_hdr {
#if CONFIG_LITTILE_ENDIAN                       			/*!< Little-endian mode, low address storage low bytes */
    kuint8_t ihl : 4,                                       /*!< Low address, so here is the low 4 bits, the length of the IP frame is stored: ihl * 4, generally ihl = 0x5, and the calculation result is exactly 20 */
			 version : 4;                                   /*!< High address, so here is 4 bits high, storing the IP protocol version, if IPv4, then version = 0x4*/
#else                       			                    /*!< Big-endian mode, low addresses store high bytes; In contrast to the little-endian, the version is reversed with ihl */
    kuint8_t version : 4,
  			 ihl : 4;
#endif

    kuint8_t	tos;                                        /*!< service type */
    kuint16_t	tot_len;                                    /*!< frame lenth */
    kuint16_t	id;                                         /*!< identifier */
    kuint16_t	frag_off;                                   /*!< the upper 4 bits are flags, and the lower 12 bits are the offsets */
    kuint8_t	ttl;                                        /*!< survival */
    kuint8_t	protocol;                                   /*!< the upper layer (transport layer) protocol, in the case of TCP, then protocol = 0x06 */
    kuint16_t	check;                                      /*!< CRC */
    kuint32_t	saddr;                                      /*!< source ip address */
    kuint32_t	daddr;                                      /*!< destination IP address */

} __packed;

#endif /*!< __FWK_IP_H_ */
