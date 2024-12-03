/*
 * Hardware Abstraction Layer Net Interface
 *
 * File Name:   fwk_network.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.12.03
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_NETWORK_H_
#define __FWK_NETWORK_H_

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/net/fwk_if.h>

/*!< The defines */
#define NETWORK_SOCKETS_MAX                         (2048)
#define NETWORK_SOCKETS_NUM                         (NETWORK_SOCKETS_MAX - 1)
#define NETWORK_SOCKETS_GENERIC                     (NETWORK_SOCKETS_NUM)

/*!< The functions */
TARGET_EXT kint32_t virt_socket(kint32_t domain, kint32_t type, kint32_t protocol);

TARGET_EXT kint32_t network_link_up(const kchar_t *name, struct fwk_sockaddr_in *sprt_ip, 
                            struct fwk_sockaddr_in *sprt_gw, struct fwk_sockaddr_in *sprt_mask);
TARGET_EXT kint32_t network_link_down(const kchar_t *name);
TARGET_EXT kint32_t network_set_ip(const kchar_t *name, struct fwk_sockaddr_in *sprt_ip);

TARGET_EXT kint32_t network_socket(kint32_t domain, kint32_t type, kint32_t protocol);
TARGET_EXT void network_close(kint32_t sockfd);
TARGET_EXT kint32_t network_bind(kint32_t sockfd, const struct fwk_sockaddr *addr, fwk_socklen_t addrlen);
TARGET_EXT kint32_t network_accept(kint32_t sockfd, struct fwk_sockaddr *addr, fwk_socklen_t *addrlen);

#endif /*!< __FWK_NETWORK_H_ */
