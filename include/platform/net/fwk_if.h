/*
 * Hardware Abstraction Layer Net Interface
 *
 * File Name:   fwk_if.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.07.06
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_IF_H_
#define __FWK_IF_H_

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The includes */
#include <platform/fwk_basic.h>

/*!< The defines */
#define NET_UAPI_DEF_IF_IFREQ									1
#define NET_UAPI_DEF_IF_IFMAP									1

#define NET_IFNAME_SIZE											(32)
#define NET_MAX_ADDR_LEN										(32)

#define NET_MAC_ETH_ALEN										(6)

#define NET_PROTO_ETH_P_IP    									0x0800		/*!< IP Protocol */
#define NET_PROTO_ETH_P_ARP   									0x0806		/*!< Address Resolution Protocol */
#define NET_PROTO_ETH_P_RARP  									0x8035		/*!< Reverse Address Resolution Protocol */
#define NET_PROTO_ETH_P_IPV6  									0x86DD		/*!< IPV6 Protocol */

typedef kuint16_t fwk_sa_family_t;
typedef kssize_t fwk_socklen_t;

struct fwk_sockaddr
{
    fwk_sa_family_t sa_family;
    kint8_t sa_data[14];													/*!< Address data.  */

#define FWK_SOCKADDR_SIZE                                       (16)
};

struct fwk_in_addr
{
    kuint32_t s_addr;
};

/*!< ip, port and so on */
struct fwk_sockaddr_in 
{
    fwk_sa_family_t	sin_family;                                             /*!< Address family */
    kuint16_t		sin_port;                                               /*!< Port number */
    struct fwk_in_addr sin_addr;                                            /*!< Internet address */

    /*!< Pad to size of `struct sockaddr'. */
    kuint8_t zero[FWK_SOCKADDR_SIZE - sizeof(fwk_sa_family_t) - sizeof(kuint16_t) - sizeof(struct fwk_in_addr)];
};

/*!< network device node */
struct fwk_network_if
{
	kchar_t ifname[NET_IFNAME_SIZE];

    struct fwk_sockaddr_in sgrt_ip;
    struct fwk_sockaddr_in sgrt_gw;
    struct fwk_sockaddr_in sgrt_netmask;

    struct fwk_network_if_ops *sprt_oprts;

    struct list_head sgrt_link;
    void *private_data;
};

/*!< sockets descripter */
struct fwk_network_com
{
    kint32_t domain;
    kint32_t type;
    kint32_t protocol;

    struct fwk_sockaddr_in sgrt_sin;
    void *private_data;
};

struct fwk_network_object
{
    struct fwk_network_com sgrt_socket;
    struct fwk_network_if *sprt_if;

    struct radix_link sgrt_radix;
};

/*!< network node: operations */
struct fwk_network_if_ops
{
    kint32_t (*init)(struct fwk_network_com *sprt_socket);
    void (*exit)(struct fwk_network_com *sprt_socket);

    void (*listen)(struct fwk_network_com *sprt_socket);

    kssize_t (*send)(struct fwk_network_com *sprt_socket, const void *buf, kssize_t size);
    kssize_t (*recv)(struct fwk_network_com *sprt_socket, void *buf, kssize_t size);
    kssize_t (*sendto)(struct fwk_network_com *sprt_socket, const void *buf, kssize_t len, 
                        kint32_t flags, const struct fwk_sockaddr *sprt_dest, fwk_socklen_t addrlen);
    kssize_t (*recvfrom)(struct fwk_network_com *sprt_socket, void *buf, size_t len, 
                        kint32_t flags, struct fwk_sockaddr *sprt_src, fwk_socklen_t *addrlen);

    kint32_t (*link_up)(struct fwk_network_if *sprt_if);
    kint32_t (*link_down)(struct fwk_network_if *sprt_if);
};

struct fwk_ifmap
{
    kuaddr_t mem_start;
    kuaddr_t mem_end;
    kuint16_t base_addr;
    kuint8_t irq;
    kuint8_t dma;
    kuint8_t port;
    /*!< 3 bytes spare */
};

struct fwk_ifreq
{
#define IFHWADDRLEN	6
    union
    {
        kchar_t ifrn_name[NET_IFNAME_SIZE];									/*!< if name, e.g. "en0" */

    } urt_fwk_ifr_ifrn;

    union
    {
        struct fwk_sockaddr ifru_addr;
        struct fwk_sockaddr ifru_dstaddr;
        struct fwk_sockaddr ifru_broadaddr;
        struct fwk_sockaddr ifru_netmask;
        struct fwk_sockaddr ifru_hwaddr;
        kint16_t ifru_flags;
        kint32_t ifru_ivalue;
        kint32_t ifru_mtu;
        struct fwk_ifmap ifru_map;
        kint8_t ifru_slave[NET_IFNAME_SIZE];								/*!< Just fits the size */
        kchar_t ifru_newname[NET_IFNAME_SIZE];
        void *ifru_data;
//		struct if_settings ifru_settings;

    } urt_fwk_ifr_ifru;
};

#define mrt_ifr_name			urt_fwk_ifr_ifrn.ifrn_name					/*!< interface name */
#define mrt_ifr_hwaddr			urt_fwk_ifr_ifru.ifru_hwaddr				/*!< MAC address */
#define	mrt_ifr_addr			urt_fwk_ifr_ifru.ifru_addr					/*!< address */
#define	mrt_ifr_dstaddr			urt_fwk_ifr_ifru.ifru_dstaddr				/*!< other end of p-p lnk	*/
#define	mrt_ifr_broadaddr		urt_fwk_ifr_ifru.ifru_broadaddr				/*!< broadcast address */
#define	mrt_ifr_netmask			urt_fwk_ifr_ifru.ifru_netmask				/*!< interface net mask */
#define	mrt_ifr_flags			urt_fwk_ifr_ifru.ifru_flags					/*!< flags */
#define	mrt_ifr_metric			urt_fwk_ifr_ifru.ifru_ivalue				/*!< metric */
#define	mrt_ifr_mtu				urt_fwk_ifr_ifru.ifru_mtu					/*!< mtu */
#define mrt_ifr_map				urt_fwk_ifr_ifru.ifru_map					/*!< device map */
#define mrt_ifr_slave			urt_fwk_ifr_ifru.ifru_slave					/*!< slave device */
#define	mrt_ifr_data			urt_fwk_ifr_ifru.ifru_data					/*!< for use by interface	*/
#define mrt_ifr_ifindex			urt_fwk_ifr_ifru.ifru_ivalue				/*!< interface index */
#define mrt_ifr_bandwidth		urt_fwk_ifr_ifru.ifru_ivalue    			/*!< link bandwidth */
#define mrt_ifr_qlen			urt_fwk_ifr_ifru.ifru_ivalue				/*!< Queue length */
#define mrt_ifr_newname			urt_fwk_ifr_ifru.ifru_newname				/*!< New name	*/
// #define mrt_ifr_settings		urt_fwk_ifr_ifru.ifru_settings				/*!< Device/proto settings */

enum __ERT_NETWORK_IFR
{
    NR_NETWORK_IFR_NAME = 0,
    NR_NETWORK_IFR_HWADDR,
    NR_NETWORK_IFR_ADDR,
    NR_NETWORK_IFR_DSTADDR,
    NR_NETWORK_IFR_BROADADDR,
    NR_NETWORK_IFR_NETMASK,
    NR_NETWORK_IFR_FLAGS,
    NR_NETWORK_IFR_METRIC,
    NR_NETWORK_IFR_MTU,
    NR_NETWORK_IFR_MAP,
    NR_NETWORK_IFR_SLAVE,
    NR_NETWORK_IFR_DATA,
    NR_NETWORK_IFR_IFINDEX,
    NR_NETWORK_IFR_BANDWIDTH,
    NR_NETWORK_IFR_QLEN,
};

#define NETWORK_IFR_GET_NAME    FWK_IOR('n', NR_NETWORK_IFR_NAME, struct fwk_ifreq)
#define NETWORK_IFR_GET_HWADDR  FWK_IOR('n', NR_NETWORK_IFR_HWADDR, struct fwk_ifreq)
#define NETWORK_IFR_GET_MTU     FWK_IOR('n', NR_NETWORK_IFR_MTU, struct fwk_ifreq)

/*!< The globals */
extern struct fwk_network_if_ops *sprt_fwk_network_if_oprts;

/*!< The functions */
extern struct fwk_network_if *network_find_node(const kchar_t *name, struct fwk_sockaddr_in *sprt_ip);
extern struct fwk_network_if *network_next_node(struct fwk_network_if *sprt_if);

/*!< API functions */
/*!
 * @brief   configure default if operations
 * @param   sprt_oprts
 * @retval  none
 * @note    none
 */
static inline void network_set_default_ops(const struct fwk_network_if_ops *sprt_oprts)
{
    sprt_fwk_network_if_oprts = (struct fwk_network_if_ops *)sprt_oprts;
}

#ifdef __cplusplus
    }
#endif

#endif /*!< _FWK_MAC_H_ */
