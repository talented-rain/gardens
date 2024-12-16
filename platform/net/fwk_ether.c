/*
 * NetWork Interface
 *
 * File Name:   fwk_ether.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.12.09
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/net/fwk_if.h>
#include <platform/net/fwk_ether.h>
#include <platform/net/fwk_netdev.h>
#include <platform/net/fwk_skbuff.h>

/*!< The defines */

/*!< The globals */


/*!< API functions */
kint16_t fwk_eth_type_trans(struct fwk_sk_buff *sprt_skb, struct fwk_net_device *sprt_ndev)
{
    struct fwk_eth_hdr *sprt_ethdr;
    kuint16_t proto;

    sprt_ethdr = (struct fwk_eth_hdr *)sprt_skb->data;
    sprt_skb->sprt_ndev = sprt_ndev;

//  fwk_skb_reset_mac_header(sprt_skb);
//  fwk_skb_pull(sprt_skb, NET_ETHER_HDR_LEN);
    
    proto = sprt_ethdr->h_proto & mrt_htons(0xFF00);
    if (proto >= mrt_htons(NET_ETH_PROTO_802_3_MIN))
        return sprt_ethdr->h_proto;

    return NET_ETH_PROTO_802_2;
}

/*!< end of file */
