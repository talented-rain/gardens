/*
 * NetWork Interface
 *
 * File Name:   fwk_lwip.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.11.23
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/fwk_fcntl.h>
#include <platform/net/fwk_if.h>
#include <platform/net/fwk_netdev.h>
#include <platform/net/fwk_socket.h>
#include <platform/net/fwk_netif.h>
#include <platform/net/fwk_ip.h>
#include <platform/net/fwk_arp.h>
#include <platform/net/fwk_ether.h>
#include <platform/net/fwk_icmp.h>
#include <platform/net/fwk_udp.h>
#include <platform/net/fwk_tcp.h>
#include <platform/net/fwk_lwip.h>
#include <kernel/thread.h>
#include <kernel/sched.h>

/*!< The defines */
struct fwk_lwip_data
{
    struct netif sgrt_netif;
    void *ndev;

    tid_t txd;
    struct fwk_sk_buff_head sgrt_txq;
};

/*!< The globals */

/*!< API functions */
/*!
 * @brief   get size according to the proto type
 * @param   proto
 * @retval  size
 * @note    none
 */
static kssize_t lwip_get_ip_proto_size(kuint8_t proto)
{
    kssize_t size = -1;

    switch (proto)
    {
        case NET_IP_PROTO_ICMP:
            size = NET_ICMP_HDR_LEN;
            break;
        case NET_IP_PROTO_UDP:
            size = NET_UDP_HDR_LEN;
            break;
        case NET_IP_PROTO_TCP:
            size = NET_TCP_HDR_LEN;
            break;
        default: break;
    }

    return size;
}

/*!
 * @brief   convert pbuf to skb, and add to tx queue
 * @param   sprt_netif, sprt_buf
 * @retval  errno
 * @note    none
 */
static err_t lwip_lowlevel_output(struct netif *sprt_netif, struct pbuf *sprt_buf)
{
    struct fwk_network_if *sprt_if;
    struct fwk_lwip_data *sprt_data;
    struct fwk_sk_buff *sprt_skb;
    struct pbuf *sprt_per, *sprt_cur;
    struct fwk_eth_hdr *sprt_ethhdr;
    struct fwk_ip_hdr *sprt_iphdr;
    kuint32_t head_len;
    kssize_t transport_len = 0;

    if (!sprt_buf->tot_len)
        return ERR_OK;

    sprt_if = (struct fwk_network_if *)sprt_netif->state;
    sprt_data = (struct fwk_lwip_data *)sprt_if->private_data;

    for (sprt_per = sprt_buf; sprt_per && sprt_per->len;)
    {
        sprt_ethhdr = (struct fwk_eth_hdr *)sprt_per->payload;
    
        head_len = SKB_DATA_HEAD_LEN(NET_ETHER_HDR_LEN);
        sprt_skb = fwk_alloc_skb(sprt_per->len + 2 * head_len, GFP_KERNEL);
        if (!isValid(sprt_skb))
        {
            print_err("%s: allocate skb failed!\n", __FUNCTION__);
            goto END;
        }

        fwk_skb_reserve(sprt_skb, head_len);
        fwk_skb_put(sprt_skb, sprt_per->len);
        sprt_skb->sprt_ndev = sprt_data->ndev;
        sprt_skb->protocol = sprt_ethhdr->h_proto;

        fwk_skb_set_mac_header(sprt_skb, 0);
        fwk_skb_set_network_header(sprt_skb, NET_ETHER_HDR_LEN);

        switch (mrt_htons(sprt_ethhdr->h_proto))
        {
            case NET_ETH_PROTO_IP:
                sprt_iphdr = (struct fwk_ip_hdr *)((void *)sprt_ethhdr + NET_ETHER_HDR_LEN);
                transport_len = lwip_get_ip_proto_size(sprt_iphdr->protocol);
                if (transport_len < 0)
                    goto fail;

                fwk_skb_set_transport_header(sprt_skb, NET_ETHER_HDR_LEN + NET_IP_HDR_LEN);
                break;

            case NET_ETH_PROTO_ARP:
                fwk_skb_set_transport_header(sprt_skb, NET_ETHER_HDR_LEN + NET_ARP_HDR_LEN);
                break;

            default: 
                print_err("%s: unable to recognize network layer protocol (%d)!\n", 
                        __FUNCTION__, mrt_htons(sprt_ethhdr->h_proto));
                goto fail;
        }

        sprt_skb->data_len = sprt_skb->len - (fwk_skb_transport_offset(sprt_skb) + transport_len);

        /*!< copy pbuf to skb */
        memcpy(sprt_skb->data, sprt_per->payload, sprt_skb->len);
        fwk_skb_add_tail(&sprt_data->sgrt_txq, sprt_skb);

        goto END;
        
    fail:
        kfree_skb(sprt_skb);
    END:
        sprt_cur = sprt_per;
        sprt_per = sprt_cur->next;

        /*!< PBUF_RAW will be released by lwip-lib sources code */
        if (sprt_cur->type == PBUF_POOL)
            pbuf_free(sprt_cur);
    }

    return ERR_OK;
}

/*!
 * @brief   lwip enet initialization
 * @param   sprt_netif
 * @retval  errno
 * @note    none
 */
static err_t lwip_enet_init(struct netif *sprt_netif)
{
    struct fwk_network_if *sprt_if;
    struct fwk_ifreq sgrt_ifr;
    kint32_t sockfd;
    kint32_t retval;

    sockfd = NET_SOCKET_GENERIC;
    sprt_if = (struct fwk_network_if *)sprt_netif->state;

    sprt_netif->name[0] = 'e';
    sprt_netif->name[1] = 'n';
    sprt_netif->linkoutput = lwip_lowlevel_output;

#if LWIP_IPV4
    sprt_netif->output = etharp_output;
#endif

#if LWIP_IPV6
    sprt_netif->output_ip6 = ethip6_output;
#endif

    /*!< start hardware */
    if (fwk_netif_open(sprt_if->ifname))
        return ERR_IF;

    strcpy(sgrt_ifr.mrt_ifr_name, sprt_if->ifname);
    retval = virt_ioctl(sockfd, NETWORK_IFR_GET_HWADDR, &sgrt_ifr);
    if (retval)
        return ERR_IF;

    sprt_netif->hwaddr_len = sizeof(sprt_netif->hwaddr);
    kmemcpy(&sprt_netif->hwaddr[0], sgrt_ifr.mrt_ifr_hwaddr.sa_data, sprt_netif->hwaddr_len);

    retval = virt_ioctl(sockfd, NETWORK_IFR_GET_MTU, &sgrt_ifr);
    if (retval)
        return ERR_IF;

    sprt_netif->mtu = sgrt_ifr.mrt_ifr_mtu;
    sprt_netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

    netif_set_link_up(sprt_netif);
    return ERR_OK;
}

/*!
 * @brief   lwip enet tx thread: send skbs one by one
 * @param   args: sprt_netif (private argument)
 * @retval  args
 * @note    none
 */
static void *fwk_lwip_tx_entry(void *args)
{
    struct netif *sprt_netif;
    struct fwk_network_if *sprt_if;
    struct fwk_lwip_data *sprt_data;
    struct fwk_sk_buff *sprt_skb;

    sprt_netif = (struct netif *)args;
    sprt_if = (struct fwk_network_if *)sprt_netif->state;
    sprt_data = (struct fwk_lwip_data *)sprt_if->private_data;

    for (;;)
    {
        while ((sprt_skb = fwk_skb_dequeue(&sprt_data->sgrt_txq)))
            fwk_dev_queue_xmit(sprt_skb);

        msleep(1);
    }

    return args;
}

/*!
 * @brief   lwip start (called by net_link_up())
 * @param   sprt_if
 * @retval  errno
 * @note    none
 */
static kint32_t fwk_lwip_link_up(struct fwk_network_if *sprt_if)
{
    struct fwk_lwip_data *sprt_data;
    ip4_addr_t sgrt_ip, sgrt_gw, sgrt_mask;

    sprt_data = kzalloc(sizeof(*sprt_data), GFP_KERNEL);
    if (!isValid(sprt_data))
        return PTR_ERR(sprt_data);

    sgrt_ip.addr = sprt_if->sgrt_ip.sin_addr.s_addr;
    sgrt_gw.addr = sprt_if->sgrt_gw.sin_addr.s_addr;
    sgrt_mask.addr = sprt_if->sgrt_netmask.sin_addr.s_addr;

    sprt_if->private_data = sprt_data;
    sprt_data->ndev = fwk_ifname_to_ndev(sprt_if->ifname);
    if (!sprt_data->ndev)
        goto fail;

    fwk_skb_list_init(&sprt_data->sgrt_txq);

    /*!< save ip address, and call lwip_enet_init */
    netif_add(&sprt_data->sgrt_netif, 
            &sgrt_ip, &sgrt_mask, &sgrt_gw, 
            sprt_if, lwip_enet_init, ethernet_input);

    /*!< the global variable "netif_default = sprt_netif" */
    netif_set_default(&sprt_data->sgrt_netif);

	/*!< specify that the network if is up */
	netif_set_up(&sprt_data->sgrt_netif);

    sprt_data->txd = kernel_thread_create(-1, mrt_nullptr, fwk_lwip_tx_entry, &sprt_data->sgrt_netif);
    thread_set_priority(mrt_tid_attr(sprt_data->txd), THREAD_PROTY_SOCKTX);

    return ER_NORMAL;

fail:
    kfree(sprt_data);
    return -ER_FAILD;
}

/*!
 * @brief   lwip close (called by net_link_down())
 * @param   sprt_if
 * @retval  errno
 * @note    none
 */
static kint32_t fwk_lwip_link_down(struct fwk_network_if *sprt_if)
{
    fwk_netif_close(sprt_if->ifname);
    return ER_NORMAL;
}

/*!
 * @brief   lwip init for transport layer (called by socket_bind())
 * @param   sprt_socket
 * @retval  errno
 * @note    bind control block with ip/port/...
 */
static kint32_t fwk_lwip_init(struct fwk_network_com *sprt_socket)
{
    ip_addr_t *sprt_ip = (ip_addr_t *)&sprt_socket->sgrt_sin.sin_addr;
    kuint16_t port = sprt_socket->sgrt_sin.sin_port;
    void *pcb;

    switch (sprt_socket->type)
    {
        case NR_SOCK_STREAM:

            break;

        case NR_SOCK_DGRAM:
            pcb = (void *)lwip_udp_raw_bind(sprt_ip, port);
            if (!isValid(pcb))
                goto fail;

            sprt_socket->private_data = pcb;
        
            break;

        default: goto fail;
    }

    return ER_NORMAL;

fail:
    return -ER_FAILD;
}

/*!
 * @brief   corresonding to fwk_lwip_init
 * @param   sprt_socket
 * @retval  none
 * @note    none
 */
static void fwk_lwip_exit(struct fwk_network_com *sprt_socket)
{

}

/*!
 * @brief   listen port
 * @param   sprt_socket
 * @retval  none
 * @note    none
 */
static void fwk_lwip_listen(struct fwk_network_com *sprt_socket)
{
    struct tcp_pcb *sprt_tcp;

    sprt_tcp = (struct tcp_pcb *)sprt_socket->private_data;
    sprt_tcp = tcp_listen(sprt_tcp);
    sprt_socket->private_data = sprt_tcp;
}

/*!
 * @brief   send message (for tcp)
 * @param   sprt_socket, buf, size
 * @retval  size sent
 * @note    none
 */
static kssize_t fwk_lwip_send(struct fwk_network_com *sprt_socket, const void *buf, kssize_t size)
{
    return 0;
}

/*!
 * @brief   send message (for udp)
 * @param   sprt_socket, buf, size
 * @retval  size sent
 * @note    none
 */
static kssize_t fwk_lwip_sendto(struct fwk_network_com *sprt_socket, const void *buf, kssize_t len, 
                        kint32_t flags, const struct fwk_sockaddr *sprt_dest, fwk_socklen_t addrlen)
{
    struct udp_pcb *sprt_upcb;
    struct fwk_sockaddr_in sgrt_saddr;

    sprt_upcb = (struct udp_pcb *)sprt_socket->private_data;
    memcpy(&sgrt_saddr, sprt_dest, addrlen);

    return lwip_udp_raw_sendto(sprt_upcb, (const ip_addr_t *)&sgrt_saddr.sin_addr, 
                            sgrt_saddr.sin_port, buf, len);
}

/*!
 * @brief   recv message (for tcp)
 * @param   sprt_socket, buf, size
 * @retval  size received
 * @note    none
 */
static kssize_t fwk_lwip_recv(struct fwk_network_com *sprt_socket, void *buf, kssize_t size)
{
    return 0;
}

/*!
 * @brief   recv message (for udp)
 * @param   sprt_socket, buf, size
 * @retval  size received
 * @note    none
 */
static kssize_t fwk_lwip_recvfrom(struct fwk_network_com *sprt_socket, void *buf, size_t len, 
                        kint32_t flags, struct fwk_sockaddr *sprt_src, fwk_socklen_t *addrlen)
{
    struct udp_pcb *sprt_upcb;
    struct fwk_sockaddr_in sgrt_saddr;
    kssize_t size;

    sprt_upcb = (struct udp_pcb *)sprt_socket->private_data;
    size = lwip_udp_raw_recvfrom(sprt_upcb, buf, len, 
                            (ip_addr_t *)&sgrt_saddr.sin_addr, &sgrt_saddr.sin_port);

    *addrlen = sizeof(sgrt_saddr);
    memcpy(sprt_src, &sgrt_saddr, *addrlen);

    return size;
}

/*!< network device node operations of lwip interface */
static const struct fwk_network_if_ops sgrt_fwk_lwip_if_oprts =
{
    .init       = fwk_lwip_init,
    .exit       = fwk_lwip_exit,
    .listen     = fwk_lwip_listen,

    .send       = fwk_lwip_send,
    .recv       = fwk_lwip_recv,
    .sendto     = fwk_lwip_sendto,
    .recvfrom   = fwk_lwip_recvfrom,

    .link_up    = fwk_lwip_link_up,
    .link_down  = fwk_lwip_link_down,
};

/*!
 * @brief   deal with per skb received
 * @param   sprt_socket, sprt_skb
 * @retval  errno
 * @note    skb ---> pbuf ---> lwip ---> application layer
 */
static err_t lwip_lowlevel_input(struct netif *sprt_netif, struct fwk_sk_buff *sprt_skb)
{
    struct fwk_eth_hdr *sprt_ethhdr;
    struct pbuf *sprt_buf;
    kint32_t head_len;

    head_len = SKB_DATA_HEAD_LEN(NET_ETHER_HDR_LEN);
    sprt_buf = pbuf_alloc(PBUF_RAW, sprt_skb->len + head_len, PBUF_POOL);
    if (!sprt_buf)
    {
        print_err("%s: allocate lwip pbuf failed!\n", __FUNCTION__);
        goto END;
    }

    pbuf_header(sprt_buf, -head_len);
    pbuf_take(sprt_buf, sprt_skb->data, sprt_skb->len);
    sprt_ethhdr = (struct fwk_eth_hdr *)fwk_skb_mac_header(sprt_skb);

    switch (mrt_htons(sprt_ethhdr->h_proto))
    {
        case NET_ETH_PROTO_IP:
        case NET_ETH_PROTO_ARP:
            if (sprt_netif->input(sprt_buf, sprt_netif) != ERR_OK)
                pbuf_free(sprt_buf);
            break;

        default:
            pbuf_free(sprt_buf);
            break;
    }

END:
    return ERR_OK;
}

/*!
 * @brief   recv callback (called by main loop or rx thread continuously)
 * @param   rxq: global rx queue
 * @retval  none
 * @note    get every skb from rx queue, and deal with it
 */
static void fwk_lwip_input(void *rxq, void *args)
{
    struct fwk_network_if *sprt_if;
    struct fwk_lwip_data *sprt_data;
    struct fwk_sk_buff *sprt_skb;
    struct fwk_sk_buff_head *sprt_rxq;

    sprt_rxq = (struct fwk_sk_buff_head *)rxq;
    while ((sprt_skb = fwk_skb_dequeue(sprt_rxq)))
    {
        sprt_if = mrt_nullptr;
        while ((sprt_if = network_next_node(sprt_if)))
        {
            sprt_data = (struct fwk_lwip_data *)sprt_if->private_data;
            if (sprt_data->ndev != (void *)sprt_skb->sprt_ndev)
                continue;

            lwip_lowlevel_input(&sprt_data->sgrt_netif, sprt_skb);

            /*!< Handle all system timeouts for all core protocols */
            sys_check_timeouts();
        }

        kfree_skb(sprt_skb);
    }
}

/*!< -------------------------------------------------------------------- */
/*!
 * @brief   lwip interface init
 * @param   none
 * @retval  none
 * @note    none
 */
kint32_t __plat_init fwk_lwip_if_init(void)
{
    lwip_init();
    fwk_netif_init(fwk_lwip_input, mrt_nullptr);

    network_set_default_ops(&sgrt_fwk_lwip_if_oprts);
    return ER_NORMAL;
}

IMPORT_PLATFORM_INIT(fwk_lwip_if_init);

/*!< end of file */
