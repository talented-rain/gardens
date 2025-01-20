/*
 * Hardware Abstraction Layer Net Interface
 *
 * File Name:   fwk_arp.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.12.15
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_ARP_H_
#define __FWK_ARP_H_

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The includes */
#include <platform/net/fwk_if.h>

/*!< The defines */
#define NET_ARP_HDR_LEN                                 (28)

/*!< proto */
#define NET_ARPHRD_NETROM                               0       /*!< from KA9Q: NET/ROM pseudo */
#define NET_ARPHRD_ETHER                                1       /*!< Ethernet 10Mbps */
#define NET_ARPHRD_EETHER                               2       /*!< Experimental Ethernet */
#define NET_ARPHRD_AX25                                 3       /*!< AX.25 Level 2 */
#define NET_ARPHRD_PRONET                               4       /*!< PROnet token ring */
#define NET_ARPHRD_CHAOS                                5       /*!< Chaosnet */
#define NET_ARPHRD_IEEE802                              6       /*!< IEEE 802.2 Ethernet/TR/TB */
#define NET_ARPHRD_ARCNET                               7       /*!< ARCnet */
#define NET_ARPHRD_APPLETLK                             8       /*!< APPLEtalk */
#define NET_ARPHRD_DLCI                                 15      /*!< Frame Relay DLCI  */
#define NET_ARPHRD_ATM                                  19      /*!< ATM */
#define NET_ARPHRD_METRICOM                             23      /*!< Metricom STRIP (new IANA id) */
#define NET_ARPHRD_IEEE1394                             24      /*!< IEEE 1394 IPv4 - RFC 2734 */
#define NET_ARPHRD_EUI64                                27      /*!< EUI-64 */
#define NET_ARPHRD_INFINIBAND                           32      /*!< InfiniBand */

/*!< opcode */
#define NET_ARPOP_REQUEST                               1       /*!< ARP request */
#define NET_ARPOP_REPLY                                 2       /*!< ARP reply */
#define NET_ARPOP_RREQUEST                              3       /*!< RARP request */
#define NET_ARPOP_RREPLY                                4       /*!< RARP reply */
#define NET_ARPOP_InREQUEST                             8       /*!< InARP request */
#define NET_ARPOP_InREPLY                               9       /*!< InARP reply */
#define NET_ARPOP_NAK                                   10      /*!< (ATM)ARP NAK */

/*!< ARP information Header */
struct fwk_arp_hdr {
    kuint16_t ha_type;                                  /*!< format of hardware address, such as: NET_ARPHRD_ETHER */
    kuint16_t proto_type;                               /*!< format of protocol address, such as: NET_ETH_PROTO_IP */
    kuint8_t haddr_len;                                 /*!< length of hardware address, such as: NET_MAC_ETH_ALEN */
    kuint8_t proto_len;                                 /*!< length of protocol address, such as: 4 */
    kuint16_t opcode;                                   /*!< command, NET_ARPOP_REQUEST or NET_ARPOP_REPLY */

    kuint8_t mac_src[NET_MAC_ETH_ALEN];                 /*!< sender MAC address, NET_MAC_ETH_ALEN = 6 */
    kuint32_t ip_src;

    /*!< for sender, mac_dst will be 0; but replier will set it */
    kuint8_t mac_dst[NET_MAC_ETH_ALEN];                 /*!< target MAC address, NET_MAC_ETH_ALEN = 6 */
    kuint32_t ip_dst;

} __packed;

#ifdef __cplusplus
    }
#endif

#endif /*!< __FWK_ARP_H_ */
