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
#include <platform/net/fwk_if.h>
#include <platform/fwk_fcntl.h>
#include <platform/net/fwk_netdev.h>
#include <platform/net/fwk_network.h>
#include <platform/net/fwk_inet.h>

#include <lwip/port/lwipopts.h>
#include <lwip/netif.h>
#include <lwip/ip.h>
#include <lwip/priv/tcp_priv.h>
#include <lwip/init.h>
#include <lwip/inet.h>
#include <lwip/etharp.h>
#include <netif/ethernet.h>
#include <lwip/udp.h>
#include <lwip/timeouts.h>
#include <lwip/contrib/apps/udpecho_raw/udpecho_raw.h>

/*!< The defines */
struct fwk_lwip_data
{
    struct netif sgrt_netif;

    kuint8_t tx_buffer[1518];
    kuint8_t rx_buffer[1518];
};

/*!< The globals */


/*!< API functions */
static err_t lwip_lowlevel_output(struct netif *sprt_netif, struct pbuf *sprt_buf)
{
    struct fwk_network_if *sprt_if;
    struct fwk_lwip_data *sprt_data;
    struct fwk_sk_buff sgrt_skb;
    kssize_t size;

    sprt_if = (struct fwk_network_if *)sprt_netif->state;
    sprt_data = (struct fwk_lwip_data *)sprt_if->private_data;

    /*!< get data buffer from sprt_buf */
    pbuf_copy_partial(sprt_buf, sprt_data->tx_buffer, sprt_buf->tot_len, 0);
    if (!sprt_buf->tot_len)
        return ERR_OK;

    sgrt_skb.ptr_data = sprt_data->tx_buffer;
    size = fwk_dev_queue_xmit(&sgrt_skb);
    if (size <= 0)
        return ERR_IF;

    return ERR_OK;
}

static err_t lwip_lowlevel_input(struct netif *sprt_netif)
{
    struct fwk_network_if *sprt_if;
    struct fwk_lwip_data *sprt_data;
    struct pbuf *sprt_buf;
    struct pq_queue *sprt_pq;
    kssize_t size = 0;

    sprt_if = (struct fwk_network_if *)sprt_netif->state;
    sprt_data = (struct fwk_lwip_data *)sprt_if->private_data;

    sprt_pq = fwk_dev_queue_poll();
    if (!sprt_pq)
        return ERR_IF;

    sprt_buf = pbuf_alloc(PBUF_RAW, size, PBUF_POOL);
    if (NULL == sprt_buf)
        return ERR_MEM;

    pbuf_take(sprt_buf, sprt_data->rx_buffer, size);
    if (sprt_netif->input(sprt_buf, sprt_netif) != ERR_OK)
    {
        pbuf_free(sprt_buf);
        return ERR_IF;
    }

    return ERR_OK;
}

static err_t lwip_enet_init(struct netif *sprt_netif)
{
    struct fwk_network_if *sprt_if;
    struct fwk_ifreq sgrt_ifr;
    kint32_t sockfd;
    kint32_t retval;

    sockfd = NETWORK_SOCKETS_GENERIC;
    sprt_if = (struct fwk_network_if *)sprt_netif->state;

    sprt_netif->name[0] = 'e';
    sprt_netif->name[1] = '0';
    sprt_netif->linkoutput = lwip_lowlevel_output;

#if LWIP_IPV4
    sprt_netif->output = etharp_output;
#endif

#if LWIP_IPV6
    sprt_netif->output_ip6 = ethip6_output;
#endif

    strcpy(sgrt_ifr.mrt_ifr_name, sprt_if->ifname);
    retval = virt_ioctl(sockfd, NETWORK_IFR_GET_HWADDR, &sgrt_ifr);
    if (retval)
        return ERR_IF;

    sprt_netif->hwaddr_len = sizeof(sprt_netif->hwaddr);
    memcpy(&sprt_netif->hwaddr[0], &sgrt_ifr.mrt_ifr_hwaddr, sprt_netif->hwaddr_len);

    retval = virt_ioctl(sockfd, NETWORK_IFR_GET_MTU, &sgrt_ifr);
    if (retval)
        return ERR_IF;

    sprt_netif->mtu = sgrt_ifr.mrt_ifr_mtu;
    sprt_netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

    netif_set_link_up(sprt_netif);
    return ERR_OK;
}

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

    /*!< save ip address, and call lwip_enet_init */
    netif_add(&sprt_data->sgrt_netif, 
            &sgrt_ip, &sgrt_mask, &sgrt_gw, 
            sprt_if, lwip_enet_init, ethernet_input);

    /*!< the global variable "netif_default = sprt_netif" */
    netif_set_default(&sprt_data->sgrt_netif);

	/*!< specify that the network if is up */
	netif_set_up(&sprt_data->sgrt_netif);

    return ER_NORMAL;
}

static kint32_t fwk_lwip_link_down(struct fwk_network_if *sprt_if)
{
    return ER_NORMAL;
}

static kint32_t fwk_lwip_init(struct fwk_network_com *sprt_socket)
{
    return ER_NORMAL;
}

static void fwk_lwip_exit(struct fwk_network_com *sprt_socket)
{

}

static kssize_t fwk_lwip_send(struct fwk_network_com *sprt_socket, const void *buf, kssize_t size)
{
    return 0;
}

static kssize_t fwk_lwip_recv(struct fwk_network_com *sprt_socket, void *buf, kssize_t size)
{
    struct fwk_network_object *sprt_nobj;
    struct fwk_network_if *sprt_if;
    struct fwk_lwip_data *sprt_data;

    sprt_nobj = mrt_container_of(sprt_socket, struct fwk_network_object, sgrt_socket);
    sprt_if = sprt_nobj->sprt_if;
    if (!sprt_if)
        return -ER_NULLPTR;

    sprt_data = (struct fwk_lwip_data *)sprt_if->private_data;
    lwip_lowlevel_input(&sprt_data->sgrt_netif);

    /*!< Handle all system timeouts for all core protocols */
    sys_check_timeouts();

    return 0;
}

static const struct fwk_network_if_ops sgrt_fwk_lwip_if_oprts =
{
    .init = fwk_lwip_init,
    .exit = fwk_lwip_exit,
    .send = fwk_lwip_send,
    .recv = fwk_lwip_recv,

    .link_up = fwk_lwip_link_up,
    .link_down = fwk_lwip_link_down,
};

kint32_t __plat_init fwk_lwip_if_init(void)
{
    lwip_init();

    sprt_fwk_network_if_oprts = (struct fwk_network_if_ops *)(&sgrt_fwk_lwip_if_oprts);
    return ER_NORMAL;
}

IMPORT_PLATFORM_INIT(fwk_lwip_if_init);

/*!< end of file */
