/*
 * Hardware Abstraction Layer Net Interface
 *
 * File Name:   fwk_socket.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.12.03
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_SOCKET_H_
#define __FWK_SOCKET_H_

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/net/fwk_if.h>

/*!< The defines */
#define NET_SOCKETS_MAX                             (2048)
#define NET_SOCKETS_NUM                             (NET_SOCKETS_MAX - 1)
#define NET_SOCKET_GENERIC                          (NET_SOCKETS_NUM)

enum __ERT_NET_SOCK_TYPE
{
    NR_SOCK_STREAM = 1,
    NR_SOCK_DGRAM = 2,
    NR_SOCK_RAW = 3,

    NR_SOCK_PACKET = 10,
};

#define NET_AF_UNSPEC                               0
#define NET_AF_UNIX                                 1
#define NET_AF_LOCAL                                1
#define NET_AF_INET                                 2
#define NET_AF_DRIDGE                               7
#define NET_AF_INET6                                10
#define NET_AF_NETBEUI                              13
#define NET_AF_KEY                                  15
#define NET_AF_NETLINK                              16
#define NET_AF_ROUTE                                NET_AF_NETLINK
#define NET_AF_PACKET                               17
#define NET_AF_CAN                                  29
#define NET_AF_NFC                                  39

#define NET_PF_UNSPEC                               NET_AF_UNSPEC
#define NET_PF_UNIX                                 NET_AF_UNIX
#define NET_PF_LOCAL                                NET_AF_LOCAL
#define NET_PF_INET                                 NET_AF_INET
#define NET_PF_DRIDGE                               NET_AF_DRIDGE
#define NET_PF_INET6                                NET_AF_INET6
#define NET_PF_NETBEUI                              NET_AF_NETBEUI
#define NET_PF_KEY                                  NET_AF_KEY
#define NET_PF_NETLINK                              NET_AF_NETLINK
#define NET_PF_ROUTE                                NET_AF_ROUTE
#define NET_PF_PACKET                               NET_AF_PACKET
#define NET_PF_CAN                                  NET_AF_CAN
#define NET_PF_NFC                                  NET_AF_NFC

#define NET_MSG_OOB                                 1
#define NET_MSG_PEEK                                2
#define NET_MSG_DONTROUTE                           4
#define NET_MSG_TRYHARD                             4                       /*!< Synonym for MSG_DONTROUTE for DECnet */
#define NET_MSG_CTRUNC                              8
#define NET_MSG_PROBE                               0x10                    /*!< Do not send. Only probe path f.e. for MTU */
#define NET_MSG_TRUNC                               0x20
#define NET_MSG_DONTWAIT                            0x40                    /*!< Nonblocking io		 */
#define NET_MSG_EOR                                 0x80                    /*!< End of record */
#define NET_MSG_WAITALL                             0x100                   /*!< Wait for a full request */
#define NET_MSG_FIN                                 0x200
#define NET_MSG_SYN                                 0x400               
#define NET_MSG_CONFIRM                             0x800                   /*!< Confirm path validity */
#define NET_MSG_RST                                 0x1000
#define NET_MSG_ERRQUEUE                            0x2000                  /*!< Fetch message from error queue */
#define NET_MSG_NOSIGNAL                            0x4000                  /*!< Do not generate SIGPIPE */
#define NET_MSG_MORE                                0x8000                  /*!< Sender will send more */
#define NET_MSG_WAITFORONE                          0x10000                 /*!< recvmmsg(): block until 1+ packets avail */
#define NET_MSG_SENDPAGE_NOTLAST                    0x20000                 /*!< sendpage() internal : not the last page */
#define NET_MSG_BATCH                               0x40000                 /*!< sendmmsg(): more messages coming */
#define NET_MSG_EOF                                 MSG_FIN

/*!< The functions */
extern kint32_t net_link_up(const kchar_t *name, struct fwk_sockaddr_in *sprt_ip, 
                            struct fwk_sockaddr_in *sprt_gw, struct fwk_sockaddr_in *sprt_mask);
extern kint32_t net_link_down(const kchar_t *name);
extern kint32_t net_socket(kint32_t domain, kint32_t type, kint32_t protocol);

extern kint32_t socket_bind(kint32_t sockfd, const struct fwk_sockaddr *sprt_addr, fwk_socklen_t addrlen);
extern kssize_t socket_sendto(kint32_t sockfd, const void *buf, kssize_t len, 
                            kint32_t flags, const struct fwk_sockaddr *sprt_dest, fwk_socklen_t addrlen);
extern kssize_t socket_recvfrom(kint32_t sockfd, void *buf, size_t len, 
                            kint32_t flags, struct fwk_sockaddr *sprt_src, fwk_socklen_t *addrlen);

extern kint32_t network_set_ip(const kchar_t *name, struct fwk_sockaddr_in *sprt_ip);
extern kint32_t network_socket(kint32_t domain, kint32_t type, kint32_t protocol);
extern void network_close(kint32_t sockfd);
extern kint32_t network_bind(kint32_t sockfd, const struct fwk_sockaddr *addr, fwk_socklen_t addrlen);
extern kint32_t network_accept(kint32_t sockfd, struct fwk_sockaddr *addr, fwk_socklen_t *addrlen);
extern kssize_t network_sendto(kint32_t sockfd, const void *buf, kssize_t len, 
                            kint32_t flags, const struct fwk_sockaddr *sprt_dest, fwk_socklen_t addrlen);
extern kssize_t network_recvfrom(kint32_t sockfd, void *buf, size_t len, 
                            kint32_t flags, struct fwk_sockaddr *sprt_src, fwk_socklen_t *addrlen);

#ifdef __cplusplus
    }
#endif

#endif /*!< __FWK_SOCKET_H_ */
