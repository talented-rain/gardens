/*
 * NetWork Interface
 *
 * File Name:   fwk_socket.c
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
#include <platform/net/fwk_netif.h>
#include <platform/net/fwk_socket.h>
#include <kernel/mutex.h>

/*!< The defines */

/*!< The globals */
struct fwk_network_if_ops *sprt_fwk_network_if_oprts = mrt_nullptr;

static struct mutex_lock sgrt_socket_mutex = MUTEX_LOCK_INIT();
static DECLARE_LIST_HEAD(sgrt_fwk_network_nodes);
static DECLARE_RADIX_TREE(sgrt_sockets_radix_tree, default_malloc, kfree);
static kuint32_t g_allocated_sockets[mrt_align(NET_SOCKETS_MAX, RET_BITS_PER_INT) / RET_BITS_PER_INT] = { 0 };

#define mrt_socket_to_object(sockfd)    \
            radix_tree_next_entry(&sgrt_sockets_radix_tree, struct fwk_network_object, sgrt_radix, sockfd)

/*!< API functions */
struct fwk_network_if *network_find_node(const kchar_t *name, struct fwk_sockaddr_in *sprt_ip)
{
    struct fwk_network_if *sprt_if;

    foreach_list_next_entry(sprt_if, &sgrt_fwk_network_nodes, sgrt_link)
    {
        if (name && (!strcmp(sprt_if->ifname, name)))
            return sprt_if;

        if ((sprt_ip) && 
            (sprt_if->sgrt_ip.sin_addr.s_addr == sprt_ip->sin_addr.s_addr))
            return sprt_if;
    }

    return mrt_nullptr;
}

struct fwk_network_if *network_next_node(struct fwk_network_if *sprt_if)
{
    if (!sprt_if)
        return mrt_list_first_valid_entry(&sgrt_fwk_network_nodes, struct fwk_network_if, sgrt_link);
    if (mrt_list_head_until(sprt_if, &sgrt_fwk_network_nodes, sgrt_link))
        return mrt_nullptr;

    return mrt_list_next_entry(sprt_if, sgrt_link);
}

kint32_t network_link_up(const kchar_t *name, struct fwk_sockaddr_in *sprt_ip, 
                    struct fwk_sockaddr_in *sprt_gw, struct fwk_sockaddr_in *sprt_mask)
{
    struct fwk_network_if *sprt_if;
    struct fwk_network_if_ops *sprt_ops;

    sprt_ops = sprt_fwk_network_if_oprts;

    if ((!sprt_ops) || 
        (!sprt_ops->link_up) ||
        (!sprt_ops->link_down) ||
        (!sprt_ops->recv) ||
        (!sprt_ops->send))
        return -ER_NSUPPORT;

    /*!< ip is exsisted */
    if (network_find_node(name, sprt_ip))
        return -ER_EXISTED;

    sprt_if = kzalloc(sizeof(*sprt_if), GFP_KERNEL);
    if (!isValid(sprt_if))
        return PTR_ERR(sprt_if);

    if (sprt_ip)
        memcpy(&sprt_if->sgrt_ip, sprt_ip, sizeof(*sprt_ip));
    if (sprt_gw)
        memcpy(&sprt_if->sgrt_gw, sprt_gw, sizeof(*sprt_gw));
    if (sprt_mask)
        memcpy(&sprt_if->sgrt_netmask, sprt_mask, sizeof(*sprt_mask));

    strcpy(sprt_if->ifname, name);
    sprt_if->sprt_oprts = sprt_ops;

    if (sprt_if->sprt_oprts->link_up(sprt_if))
        goto fail;

    list_head_add_tail(&sgrt_fwk_network_nodes, &sprt_if->sgrt_link);
    return ER_NORMAL;
    
fail:
    kfree(sprt_if);
    return -ER_FAILD;
}

kint32_t network_set_ip(const kchar_t *name, struct fwk_sockaddr_in *sprt_ip)
{
    struct fwk_network_if *sprt_if;

    if ((!name) || (!sprt_ip))
        return -ER_UNVALID;

    sprt_if = network_find_node(name, mrt_nullptr);
    if (!sprt_if)
        return -ER_NODEV;

    if (network_find_node(mrt_nullptr, sprt_ip))
        return -ER_EXISTED;

    memcpy(&sprt_if->sgrt_ip, sprt_ip, sizeof(*sprt_ip));
    return ER_NORMAL;
}

kint32_t network_link_down(const kchar_t *name)
{
    struct fwk_network_if *sprt_if;

    sprt_if = network_find_node(name, mrt_nullptr);
    if (!sprt_if)
        return -ER_NODEV;

    if (sprt_if->sprt_oprts->link_down(sprt_if))
        return -ER_FAILD;

    fwk_netif_close(name);
    list_head_del(&sprt_if->sgrt_link);
    kfree(sprt_if);

    return ER_NORMAL;
}

kint32_t network_socket(kint32_t domain, kint32_t type, kint32_t protocol)
{
    struct fwk_network_object *sprt_obj;
    struct fwk_network_com *sprt_socket;
    struct radix_tree *sprt_rtree;
    kint32_t index;

    mutex_lock(&sgrt_socket_mutex);
    index = bitmap_find_first_zero_bit(g_allocated_sockets, 0, NET_SOCKETS_NUM);
    if (index < 0)
    {
        mutex_unlock(&sgrt_socket_mutex);
        return -ER_FULL;
    }

    bitmap_set_nr_bit_valid(g_allocated_sockets, index, NET_SOCKETS_NUM, 1);
    mutex_unlock(&sgrt_socket_mutex);

    sprt_obj = kzalloc(sizeof(*sprt_obj), GFP_KERNEL);
    if (!isValid(sprt_obj))
    {
        mutex_lock(&sgrt_socket_mutex);
        bitmap_set_nr_bit_zero(g_allocated_sockets, index, NET_SOCKETS_NUM, 1);
        mutex_unlock(&sgrt_socket_mutex);

        return PTR_ERR(sprt_obj);
    }

    sprt_rtree  = &sgrt_sockets_radix_tree;
    sprt_socket = &sprt_obj->sgrt_socket;
    
    sprt_socket->domain = domain;
    sprt_socket->type = type;
    sprt_socket->protocol = protocol;

    spin_lock(&sprt_rtree->sgrt_lock);
    radix_tree_add(sprt_rtree, index, &sprt_obj->sgrt_radix);
    spin_unlock(&sprt_rtree->sgrt_lock);
    
    return (index + NETWORK_SOCKETS_BASE);
}

void network_close(kint32_t sockfd)
{
    struct fwk_network_object *sprt_obj;
    struct fwk_network_if *sprt_if;
    struct radix_tree *sprt_rtree;
    kint32_t index;

    index = sockfd - NETWORK_SOCKETS_BASE;
    if ((index < 0) ||
        (index >= NET_SOCKETS_NUM))
        return;

    sprt_obj = mrt_socket_to_object(index);
    if (sprt_obj)
    {
        sprt_if = sprt_obj->sprt_if;
        sprt_rtree = &sgrt_sockets_radix_tree;

        if (sprt_if && sprt_if->sprt_oprts->exit)
           sprt_if->sprt_oprts->exit(&sprt_obj->sgrt_socket);

        spin_lock(&sprt_rtree->sgrt_lock);
        radix_tree_del(&sgrt_sockets_radix_tree, index);
        spin_unlock(&sprt_rtree->sgrt_lock);

        mutex_lock(&sgrt_socket_mutex);
        bitmap_set_nr_bit_zero(g_allocated_sockets, index, NET_SOCKETS_NUM, 1);
        mutex_unlock(&sgrt_socket_mutex);

        kfree(sprt_obj);
    }
}

kint32_t network_bind(kint32_t sockfd, const struct fwk_sockaddr *sprt_addr, fwk_socklen_t addrlen)
{
    struct fwk_network_object *sprt_obj;
    struct fwk_network_if *sprt_if;
    struct fwk_network_com *sprt_socket;
    struct fwk_sockaddr_in *sprt_sin;
    kint32_t index;

    index = sockfd - NETWORK_SOCKETS_BASE;
    if ((index < 0) ||
        (index >= NET_SOCKETS_NUM))
        return -ER_UNVALID;

    sprt_obj = mrt_socket_to_object(index);
    if (!sprt_obj)
        return -ER_EMPTY;

    sprt_sin = (struct fwk_sockaddr_in *)sprt_addr;
    sprt_if = network_find_node(mrt_nullptr, sprt_sin);
    if (!sprt_if)
        return -ER_IOERR;

    sprt_obj->sprt_if = sprt_if;
    sprt_socket = &sprt_obj->sgrt_socket;
    if (sprt_socket->domain != sprt_sin->sin_family)
        return -ER_CHECKERR;

    memcpy(&sprt_socket->sgrt_sin, sprt_sin, addrlen);

    if (sprt_if->sprt_oprts->init)
        sprt_if->sprt_oprts->init(sprt_socket);

    return 0;
}

kint32_t network_accept(kint32_t sockfd, struct fwk_sockaddr *sprt_addr, fwk_socklen_t *addrlen)
{
    kint32_t index;

    index = sockfd - NETWORK_SOCKETS_BASE;
    if ((index < 0) ||
        (index >= NET_SOCKETS_NUM))
        return -ER_UNVALID;

    return 0;
}

kssize_t network_sendto(kint32_t sockfd, const void *buf, kssize_t len, 
                        kint32_t flags, const struct fwk_sockaddr *sprt_dest, fwk_socklen_t addrlen)
{
    struct fwk_network_object *sprt_obj;
    struct fwk_network_if *sprt_if;
    kint32_t index;

    index = sockfd - NETWORK_SOCKETS_BASE;
    if ((index < 0) ||
        (index >= NET_SOCKETS_NUM))
        return -ER_UNVALID;

    sprt_obj = mrt_socket_to_object(index);
    if (!sprt_obj)
        return -ER_EMPTY;

    sprt_if = sprt_obj->sprt_if;
    if (sprt_if->sprt_oprts->sendto)
        return sprt_if->sprt_oprts->sendto(&sprt_obj->sgrt_socket, buf, len, flags, sprt_dest, addrlen);

    return -ER_FORBID;
}

kssize_t network_recvfrom(kint32_t sockfd, void *buf, size_t len, 
                        kint32_t flags, struct fwk_sockaddr *sprt_src, fwk_socklen_t *addrlen)
{
    struct fwk_network_object *sprt_obj;
    struct fwk_network_if *sprt_if;
    kint32_t index;

    index = sockfd - NETWORK_SOCKETS_BASE;
    if ((index < 0) ||
        (index >= NET_SOCKETS_NUM))
        return -ER_UNVALID;

    sprt_obj = mrt_socket_to_object(index);
    if (!sprt_obj)
        return -ER_EMPTY;

    sprt_if = sprt_obj->sprt_if;
    if (sprt_if->sprt_oprts->recvfrom)
        return sprt_if->sprt_oprts->recvfrom(&sprt_obj->sgrt_socket, buf, len, flags, sprt_src, addrlen);

    return -ER_FORBID;
}

/*!< ----------------------------------------------------------------- */
/*!
 * @brief   virt_socket
 * @param   none
 * @retval  none
 * @note    The interface is provided for use by the application layer
 */
kint32_t virt_socket(kint32_t domain, kint32_t type, kint32_t protocol)
{
    return network_socket(domain, type, protocol);
}

kint32_t virt_bind(kint32_t sockfd, const struct fwk_sockaddr *sprt_addr, fwk_socklen_t addrlen)
{
    return network_bind(sockfd, sprt_addr, addrlen);
}

kssize_t virt_sendto(kint32_t sockfd, const void *buf, kssize_t len, 
                        kint32_t flags, const struct fwk_sockaddr *sprt_dest, fwk_socklen_t addrlen)
{
    return network_sendto(sockfd, buf, len, flags, sprt_dest, addrlen);
}

kssize_t virt_recvfrom(kint32_t sockfd, void *buf, size_t len, 
                        kint32_t flags, struct fwk_sockaddr *sprt_src, fwk_socklen_t *addrlen)
{
    return network_recvfrom(sockfd, buf, len, flags, sprt_src, addrlen);
}

/*!< end of file */
