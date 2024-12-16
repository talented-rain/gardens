/*
 * Hardware Abstraction Layer Net Interface
 *
 * File Name:   fwk_lwip.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.12.07
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_LWIP_H_
#define __FWK_LWIP_H_

/*!< The includes */
#include <platform/net/fwk_if.h>
#include <platform/net/fwk_ip.h>

#include <lwip/port/lwipopts.h>
#include <lwip/netif.h>
#include <lwip/ip.h>
#include <lwip/priv/tcp_priv.h>
#include <lwip/init.h>
#include <lwip/inet.h>
#include <lwip/etharp.h>
#include <netif/ethernet.h>
#include <lwip/udp.h>
#include <lwip/tcp.h>
#include <lwip/timeouts.h>
#include <lwip/contrib/apps/udpecho_raw/udpecho_raw.h>

/*!< The functions */
TARGET_EXT kssize_t lwip_udp_raw_recvfrom(struct udp_pcb *sprt_upcb, void *buf, 
                                        kusize_t size, ip_addr_t *sprt_src, u16_t *port);
TARGET_EXT kssize_t lwip_udp_raw_sendto(struct udp_pcb *sprt_upcb, const ip_addr_t *sprt_dest, 
                                        u16_t dest_port, const void *buf, kusize_t size);
TARGET_EXT struct udp_pcb *lwip_udp_raw_bind(const ip_addr_t *sprt_ip, u16_t port);

#endif /*!< __FWK_LWIP_H_ */
