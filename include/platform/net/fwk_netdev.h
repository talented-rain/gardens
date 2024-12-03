/*
 * Hardware Abstraction Layer Net Interface
 *
 * File Name:   fwk_netdev.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.07.06
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_NETDEV_H_
#define __FWK_NETDEV_H_

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/net/fwk_if.h>
#include <platform/fwk_platform.h>

/*!< The defines */
typedef kint32_t netdev_tx_t;
struct fwk_sk_buff;

struct fwk_netdev_stats
{
    kuint64_t rx_packets;
    kuint64_t tx_packets;
    kuint64_t rx_bytes;
    kuint64_t tx_bytes;
    kuint64_t rx_errors;
    kuint64_t tx_errors;
    kuint64_t rx_dropped;
    kuint64_t tx_dropped;
    kuint64_t multicast;
    kuint64_t collisions;
    kuint64_t rx_length_errors;
    kuint64_t rx_over_errors;
    kuint64_t rx_crc_errors;
    kuint64_t rx_frame_errors;
    kuint64_t rx_fifo_errors;
    kuint64_t rx_missed_errors;
    kuint64_t tx_aborted_errors;
    kuint64_t tx_carrier_errors;
    kuint64_t tx_fifo_errors;
    kuint64_t tx_heartbeat_errors;
    kuint64_t tx_window_errors;
    kuint64_t rx_compressed;
    kuint64_t tx_compressed;
};

struct fwk_netdev_queue
{
    struct fwk_net_device *sprt_netdev;								/*!< destination/source network device */
    kuint64_t tx_maxrate;
    kuint64_t trans_timeout;										/*!< statistics on the number of times the queue times out */
    kuint64_t trans_start;											/*!< The time of the last sent */
    kuint64_t state;												/*!< state */
};

enum __ERT_FWK_NETDEVICE_PRIV
{
    NR_NETDEV_PRIV_AINDEX = mrt_bit(0),
};

struct fwk_net_device
{
    kchar_t name[NET_IFNAME_SIZE];									/*!< network device name */
    kchar_t *ptr_ifalias;											/*!< network device aliases */

    kuint64_t state;												/*!< network device interface state */
    kint32_t ifindex;												/*!< network Device Interface Index Value: Network device identifier */

    struct list_head sgrt_link;										/*!< network device list */
    struct fwk_netdev_stats sgrt_states;							/*!< statistics on network device interfaces */
    const struct fwk_netdev_ops *sprt_netdev_oprts;					/*!< network device operation API */
    const struct fwk_ethtool_ops *sprt_ethtool_oprts;				/*!< ether tool operation API */

    kuint32_t flags;												/*!< Network device Interface identifier */
    kuint16_t priv_flags;											/*!< Network device Interface identifier; However, it is not visible to user space */

    kuint32_t mtu;													/*!< maximum transmission unit of the network device interface */
    kuint16_t type;													/*!< interface hardware type */
    kuint16_t hard_header_len;										/*!< hardware interface header lenth */
    kuint16_t min_header_len;										/*!< minimum hardware header length */

    kuint32_t uc_promisc;											/*!< unicast mode of the network device interface */
    kuint32_t promiscuity;											/*!< promiscuous mode of the network device interface */
    kuint32_t allmulti;												/*!< full multicast mode for the network device interface */

    kuint8_t *ptr_dev_addr;											/*!< MAC address of the network device interface */

    kuint8_t broadcast[NET_MAX_ADDR_LEN];							/*!< hardware broadcast address */
    struct fwk_netdev_queue sgrt_rcu;								/*!< packet receive queue for network device interfaces */
    struct fwk_netdev_queue *sprt_tx;								/*!< packet send queue for network device interfaces */

    kuint32_t num_tx_queues;										/*!< number of TX queues allocated at alloc_netdev_mq() time */
    kuint32_t real_num_tx_queues;									/*!< number of TX queues currently active in device */
    kuint64_t tx_queue_len;											/*!< max frame per queue allowned */

    struct fwk_device sgrt_dev;
//	struct fwk_phy_device *sprt_phydev;
    void *private_data;
};

struct fwk_netdev_ops
{
    kint32_t (*ndo_init) (struct fwk_net_device *sprt_ndev);
    void (*ndo_uninit) (struct fwk_net_device *sprt_ndev);
    kint32_t (*ndo_open) (struct fwk_net_device *sprt_ndev);
    kint32_t (*ndo_stop) (struct fwk_net_device *sprt_ndev);
    netdev_tx_t (*ndo_start_xmit) (struct fwk_sk_buff *sprt_skb, struct fwk_net_device *sprt_ndev);
    void (*ndo_set_rx_mode) (struct fwk_net_device *sprt_ndev);
    kint32_t (*ndo_set_mac_address) (struct fwk_net_device *sprt_ndev, void *ptr_addr);
    kint32_t (*ndo_do_ioctl) (struct fwk_net_device *sprt_ndev, struct fwk_ifreq *sprt_ifr, kint32_t cmd);
    void (*ndo_tx_timeout) (struct fwk_net_device *sprt_ndev);
    struct fwk_netdev_stats* (*ndo_get_stats) (struct fwk_net_device *sprt_ndev);
    kint32_t (*ndo_add_slave) (struct fwk_net_device *sprt_ndev, struct fwk_net_device *sprt_slave_dev);
    kint32_t (*ndo_del_slave) (struct fwk_net_device *sprt_ndev, struct fwk_net_device *sprt_slave_dev);
    kint32_t (*ndo_set_tx_maxrate) (struct fwk_net_device *sprt_ndev, kint32_t queue_index, kuint32_t maxrate);
};

struct fwk_ethtool_ops
{
    kint32_t (*begin) (struct fwk_net_device *);
    void (*complete) (struct fwk_net_device *);
};

/*!< The functions */
TARGET_EXT struct fwk_net_device *fwk_alloc_netdev_mq(kint32_t sizeof_priv, const kchar_t *name,
                                                    void (*setup) (struct fwk_net_device *sprt_ndev), kuint32_t queue_count);
TARGET_EXT void fwk_free_netdev(struct fwk_net_device *sprt_ndev);
TARGET_EXT struct fwk_net_device *fwk_find_netdevice(const kchar_t *name);
TARGET_EXT kint32_t fwk_register_netdevice(struct fwk_net_device *sprt_ndev);
TARGET_EXT kint32_t fwk_unregister_netdevice(struct fwk_net_device *sprt_ndev);

#define fwk_alloc_netdev(sizeof_priv, name, setup)	\
                fwk_alloc_netdev_mq(sizeof_priv, name, setup, 1)
#define fwk_netdev_priv(sprt_ndev)				((sprt_ndev)->private_data)

#endif /*!< __FWK_NETDEV_H_ */
