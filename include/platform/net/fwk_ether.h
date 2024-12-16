/*
 * Hardware Abstraction Layer Net Interface
 *
 * File Name:   fwk_ether.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.07.06
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_ETHER_H_
#define __FWK_ETHER_H_

/*!< The includes */
#include <platform/net/fwk_if.h>
#include <platform/net/fwk_netif.h>
#include <platform/net/fwk_netdev.h>

/*!< The defines */
struct fwk_sk_buff;

#define NET_ETHER_HDR_LEN                       (14)

#define NET_ETH_PROTO_LOOP                      (0x0060)
#define NET_ETH_PROTO_PUP                       (0x0200)
#define NET_ETH_PROTO_PUPAT                     (0x0201)
#define NET_ETH_PROTO_TSN                       (0x22F0)

#define NET_ETH_PROTO_802_3	                    (0x0001)    /*!< Dummy type for 802.3 frames */
#define NET_ETH_PROTO_AX25	                    (0x0002)    /*!< Dummy protocol id for AX.25 */
#define NET_ETH_PROTO_ALL	                    (0x0003)    /*!< Every packet (be careful!!!) */
#define NET_ETH_PROTO_802_2	                    (0x0004)    /*!< 802.2 frames */
#define NET_ETH_PROTO_SNAP	                    (0x0005)    /*!< Internal only */
#define NET_ETH_PROTO_DDCMP                     (0x0006)    /*!< DEC DDCMP: Internal only */
#define NET_ETH_PROTO_WAN_PPP                   (0x0007)    /*!< Dummy type for WAN PPP frames*/
#define NET_ETH_PROTO_PPP_MP                    (0x0008)    /*!< Dummy type for PPP MP frames */
#define NET_ETH_PROTO_LOCALTALK                 (0x0009)    /*!< Localtalk pseudo type */
#define NET_ETH_PROTO_CAN	                    (0x000C)    /*!< CAN: Controller Area Network */
#define NET_ETH_PROTO_CANFD	                    (0x000D)    /*!< CANFD: CAN flexible data rate*/
#define NET_ETH_PROTO_PPPTALK	                (0x0010)    /*!< Dummy type for Atalk over PPP*/
#define NET_ETH_PROTO_TR_802_2	                (0x0011)    /*!< 802.2 frames */
#define NET_ETH_PROTO_MOBITEX	                (0x0015)    /*!< Mobitex (kaz@cafe.net)	*/
#define NET_ETH_PROTO_CONTROL	                (0x0016)    /*!< Card specific control frames */
#define NET_ETH_PROTO_IRDA	                    (0x0017)    /*!< Linux-IrDA */
#define NET_ETH_PROTO_ECONET	                (0x0018)    /*!< Acorn Econet */
#define NET_ETH_PROTO_HDLC	                    (0x0019)    /*!< HDLC frames */
#define NET_ETH_PROTO_ARCNET	                (0x001A)    /*!< 1A for ArcNet :-) */
#define NET_ETH_PROTO_DSA	                    (0x001B)    /*!< Distributed Switch Arch */
#define NET_ETH_PROTO_TRAILER	                (0x001C)    /*!< Trailer switch tagging	*/
#define NET_ETH_PROTO_PHONET	                (0x00F5)    /*!< Nokia Phonet frames */
#define NET_ETH_PROTO_IEEE802154                (0x00F6)    /*!< IEEE802.15.4 frame */
#define NET_ETH_PROTO_CAIF	                    (0x00F7)    /*!< ST-Ericsson CAIF protocol */
#define NET_ETH_PROTO_XDSA	                    (0x00F8)    /*!< Multiplexed DSA protocol */
#define NET_ETH_PROTO_802_3_MIN	                (0x0600)

#define NET_ETH_PROTO_IP                        (0x0800)    /*!< Internet protocol v4 */
#define NET_ETH_PROTO_ARP                       (0x0806)    /*!< Address resolution protocol */
#define NET_ETH_PROTO_WOL                       (0x0842)    /*!< Wake on lan */
#define NET_ETH_PROTO_RARP                      (0x8035)    /*!< RARP */
#define NET_ETH_PROTO_VLAN                      (0x8100)    /*!< Virtual local area network */ 
#define NET_ETH_PROTO_IPV6                      (0x86dd)    /*!< Internet protocol v6 */ 
#define NET_ETH_PROTO_PPPOEDISC                 (0x8863)    /*!< PPP Over Ethernet Discovery Stage */
#define NET_ETH_PROTO_PPPOE                     (0x8864)    /*!< PPP Over Ethernet Session Stage */
#define NET_ETH_PROTO_JUMBO                     (0x8870)    /*!< Jumbo Frames */  
#define NET_ETH_PROTO_PROFINET                  (0x8892)    /*!< Process field network */ 
#define NET_ETH_PROTO_ETHERCAT                  (0x88a4)    /*!< Ethernet for control automation technology */ 
#define NET_ETH_PROTO_LLDP                      (0x88cc)    /*!< Link layer discovery protocol */ 
#define NET_ETH_PROTO_SERCOS                    (0x88cd)    /*!< Serial real-time communication system */ 
#define NET_ETH_PROTO_MRP                       (0x88e3)    /*!< Media redundancy protocol */
#define NET_ETH_PROTO_PTP                       (0x88f7)    /*!< Precision time protocol */
#define NET_ETH_PROTO_QINQ                      (0x9100)    /*!< Q-in-Q, 802.1ad */

/*!< EthernetII information Header */
struct fwk_eth_hdr
{
    kuint8_t h_dest[NET_MAC_ETH_ALEN];                      /*!< destination MAC address, ETH_ALEN = 6 */
    kuint8_t h_source[NET_MAC_ETH_ALEN];                    /*!< source MAC address, ETH_ALEN = 6 */

    kuint16_t h_proto;                                      /*!< network layer protocol type(IP, ARP, ...) */

} __packed;

/*!< The functions */
TARGET_EXT kint16_t fwk_eth_type_trans(struct fwk_sk_buff *sprt_skb, struct fwk_net_device *sprt_ndev);

/*!< API functions */
static inline void fwk_eth_random_addr(kuint8_t *addr)
{
    fwk_inet_random_addr(addr, NET_MAC_ETH_ALEN);
    addr[0] &= 0xfe;
    addr[0] |= 0x02;
}

static inline void fwk_eth_broadcast_addr(kuint8_t *addr)
{
    memset(addr, 0xff, NET_MAC_ETH_ALEN);
}

#endif /*!< __FWK_ETHER_H_ */
