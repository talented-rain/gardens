/*
 * ZYNQ eMAC Of PS Driver
 *
 * File Name:   zynq7sdk_gem.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.11.30
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/of/fwk_of.h>
#include <platform/of/fwk_of_device.h>
#include <platform/fwk_platdev.h>
#include <platform/fwk_platdrv.h>
#include <platform/fwk_uaccess.h>

#include <platform/net/fwk_if.h>
#include <platform/net/fwk_netdev.h>
#include <platform/net/fwk_ip.h>
#include <platform/net/fwk_skbuff.h>
#include <platform/net/fwk_ether.h>

#include <zynq7/zynq7_periph.h>
#include <zynq7/xemac/xemacpsif.h>
#include <zynq7/xemac/xemacps.h>
#include <zynq7/xemac/xemac_ieee_reg.h>

/*!< The defines */
struct xsdk_gem_drv_data
{
    void *base;
    kint32_t irq;

    kuint16_t hwaddr[NET_MAC_ETH_ALEN];

    struct xemac_s sgrt_xemac;
    xemacpsif_s sgrt_xemacpsif;
    XEmacPs_Config sgrt_config;
};

#define XSDK_GEM_DRIVER_NAME                        "gem0"

/*!< The globals */

/*!< The functions */

/*!< API function */
static kint32_t xsdk_gem_dma_init(struct xemac_s *sprt_xemac)
{
    xemacpsif_s *sprt_emcpsif;
    kuint32_t version;

    sprt_emcpsif = (xemacpsif_s *)sprt_xemac->state;
    version = ((XEmacPs_ReadReg(sprt_emcpsif->emacps.Config.BaseAddress, 0xFC)) >> 16) & 0xFFF;
    
}

static kint32_t xsdk_gem_ndo_init(struct fwk_net_device *sprt_ndev)
{
    struct xemac_s *sprt_xemac;
    xemacpsif_s *sprt_emcpsif;
    XEmacPs_Config *sprt_config;
    struct xsdk_gem_drv_data *sprt_data;
    kint32_t retval;

    sprt_data = (struct xsdk_gem_drv_data *)fwk_netdev_priv(sprt_ndev);
    sprt_xemac = &sprt_data->sgrt_xemac;
    sprt_emcpsif = &sprt_data->sgrt_xemacpsif;

    sprt_xemac->state = (void *)sprt_emcpsif;
    sprt_xemac->topology_index = 0;

    sprt_config = &sprt_data->sgrt_config;
    sprt_config->DeviceId = 0;
    sprt_config->BaseAddress = (kuaddr_t)sprt_data->base;
    sprt_config->IsCacheCoherent = XPAR_PS7_ETHERNET_0_IS_CACHE_COHERENT;

    retval = XEmacPs_CfgInitialize(&sprt_emcpsif->emacps, sprt_config, sprt_config->BaseAddress);
    if (retval)
        return retval;

    for (kint32_t idx = 0; idx < NET_MAC_ETH_ALEN; idx++)
        sprt_ndev->dev_addr[idx] = (kuint8_t)sprt_data->hwaddr[idx];

    XEmacPs_Init(&sprt_emcpsif->emacps, sprt_ndev->dev_addr);
    XEmacPsIf_SetupIsr(sprt_xemac);
    

    return ER_NORMAL;
}

static void xsdk_gem_ndo_uninit(struct fwk_net_device *sprt_ndev)
{

}

static kint32_t xsdk_gem_ndo_open(struct fwk_net_device *sprt_ndev)
{
    return ER_NORMAL;
}

static kint32_t xsdk_gem_ndo_stop(struct fwk_net_device *sprt_ndev)
{
    return ER_NORMAL;
}

static netdev_tx_t xsdk_gem_ndo_start_xmit(struct fwk_sk_buff *sprt_skb, struct fwk_net_device *sprt_ndev)
{
    return 0;
}

static void xsdk_gem_ndo_set_rx_mode(struct fwk_net_device *sprt_ndev)
{
    
}

static kint32_t xsdk_gem_ndo_set_mac_address(struct fwk_net_device *sprt_ndev, void *ptr_addr)
{
    return ER_NORMAL;
}

static kint32_t xsdk_gem_ndo_do_ioctl(struct fwk_net_device *sprt_ndev, struct fwk_ifreq *sprt_ifr, kint32_t cmd)
{
    return ER_NORMAL;
}

static void xsdk_gem_ndo_tx_timeout(struct fwk_net_device *sprt_ndev)
{

}

static struct fwk_netdev_stats *xsdk_gem_ndo_get_stats(struct fwk_net_device *sprt_ndev)
{
    return mrt_nullptr;
}

static kint32_t xsdk_gem_ndo_add_slave(struct fwk_net_device *sprt_ndev, struct fwk_net_device *sprt_slave_dev)
{
    return ER_NORMAL;
}

static kint32_t xsdk_gem_ndo_del_slave(struct fwk_net_device *sprt_ndev, struct fwk_net_device *sprt_slave_dev)
{
    return ER_NORMAL;
}

static kint32_t xsdk_gem_ndo_set_tx_maxrate(struct fwk_net_device *sprt_ndev, kint32_t queue_index, kuint32_t maxrate)
{
    return ER_NORMAL;
}

static const struct fwk_netdev_ops sgrt_xsdk_gem_drv_oprts =
{
    .ndo_init = xsdk_gem_ndo_init,
    .ndo_uninit = xsdk_gem_ndo_uninit,
    .ndo_open = xsdk_gem_ndo_open,
    .ndo_stop = xsdk_gem_ndo_stop,
    .ndo_start_xmit = xsdk_gem_ndo_start_xmit,
    .ndo_set_rx_mode = xsdk_gem_ndo_set_rx_mode,
    .ndo_set_mac_address = xsdk_gem_ndo_set_mac_address,
    .ndo_do_ioctl = xsdk_gem_ndo_do_ioctl,
    .ndo_tx_timeout = xsdk_gem_ndo_tx_timeout,
    .ndo_get_stats = xsdk_gem_ndo_get_stats,
    .ndo_add_slave = xsdk_gem_ndo_add_slave,
    .ndo_del_slave = xsdk_gem_ndo_del_slave,
    .ndo_set_tx_maxrate = xsdk_gem_ndo_set_tx_maxrate,
};

static void xsdk_gem_driver_setup(struct fwk_net_device *sprt_ndev)
{
    struct xsdk_gem_drv_data *sprt_data;

    sprt_ndev->mtu = XEMACPS_MTU;
    sprt_ndev->sprt_netdev_oprts = &sgrt_xsdk_gem_drv_oprts;
    fwk_eth_random_addr(sprt_ndev->dev_addr);

    fwk_eth_broadcast_addr(sprt_ndev->broadcast);
    sprt_ndev->tx_queue_len = 1000;
    sprt_ndev->hard_header_len = NET_ETHER_HDR_LEN;
    sprt_ndev->min_header_len = NET_ETHER_HDR_LEN;
}

static irq_return_t xsdk_gem_driver_isr(void *args)
{


    return 0;
}

/*!
 * @brief   configure property
 * @param   sprt_pdev, sprt_data
 * @retval  errno
 * @note    none
 */
static kint32_t xsdk_gem_driver_probe_dt(struct fwk_platdev *sprt_pdev, struct xsdk_gem_drv_data *sprt_data)
{
    struct fwk_device_node *sprt_node, *sprt_phy;
    kuint32_t phandle;
    void *base;

    sprt_node = sprt_pdev->sgrt_dev.sprt_node;
    if (!isValid(sprt_node))
        return PTR_ERR(sprt_node);

    fwk_of_property_read_u32(sprt_node, "phy-handle", &phandle);
    sprt_phy = fwk_of_find_node_by_phandle(mrt_nullptr, phandle);
    if (!isValid(sprt_phy))
        return PTR_ERR(sprt_phy);

    base = (void *)fwk_platform_get_address(sprt_pdev, 0);
    sprt_data->base = fwk_io_remap(base);
    sprt_data->irq = fwk_platform_get_irq(sprt_pdev, 0);

    fwk_of_property_read_u16_array(sprt_phy, "local-mac-address", sprt_data->hwaddr, NET_MAC_ETH_ALEN);
    return ER_NORMAL;
}

/*!
 * @brief   xsdk_gem_driver_probe
 * @param   sprt_dev
 * @retval  errno
 * @note    none
 */
static kint32_t xsdk_gem_driver_probe(struct fwk_platdev *sprt_pdev)
{
    struct xsdk_gem_drv_data *sprt_data;
    struct fwk_net_device *sprt_ndev;
    kint32_t retval;

    sprt_ndev = fwk_alloc_netdev(sizeof(*sprt_data), "eth%d", xsdk_gem_driver_setup);
    if (!isValid(sprt_ndev))
        return -ER_FAILD;
    
    sprt_data = (struct xsdk_gem_drv_data *)fwk_netdev_priv(sprt_ndev);
    if (!isValid(sprt_data))
        goto fail1;

    if (xsdk_gem_driver_probe_dt(sprt_pdev, sprt_data))
        goto fail1;

    fwk_platform_set_drvdata(sprt_pdev, sprt_data);
    retval = fwk_request_irq(sprt_data->irq, xsdk_gem_driver_isr, 0, XSDK_GEM_DRIVER_NAME, sprt_ndev);
    if (retval)
        goto fail2;

    fwk_disable_irq(sprt_data->irq);
    retval = fwk_register_netdevice(sprt_ndev);
    if (retval)
        goto fail3;

    print_info("register a new netdevice (GEM)\n");
    return ER_NORMAL;

fail3:
    fwk_free_irq(sprt_data->irq, sprt_ndev);
fail2:
    fwk_platform_set_drvdata(sprt_pdev, mrt_nullptr);
fail1:
    fwk_free_netdev(sprt_ndev);
    return -ER_FAILD;
}

/*!
 * @brief   xsdk_gem_driver_remove
 * @param   sprt_dev
 * @retval  errno
 * @note    none
 */
static kint32_t xsdk_gem_driver_remove(struct fwk_platdev *sprt_pdev)
{
    struct fwk_net_device *sprt_ndev;
    struct xsdk_gem_drv_data *sprt_data;

    sprt_ndev = (struct fwk_net_device *)fwk_platform_get_drvdata(sprt_pdev);
    if (!isValid(sprt_ndev))
        return -ER_NULLPTR;

    sprt_data = (struct xsdk_gem_drv_data *)fwk_netdev_priv(sprt_ndev);

    fwk_unregister_netdevice(sprt_ndev);
    fwk_free_irq(sprt_data->irq, sprt_ndev);
    fwk_platform_set_drvdata(sprt_pdev, mrt_nullptr);
    fwk_free_netdev(sprt_ndev);

    return ER_NORMAL;
}

/*!< device id for device-tree */
static const struct fwk_of_device_id sgrt_xsdk_gem_driver_id[] =
{
    { .compatible = "cdns,zynq-gem", },
    {},
};

/*!< platform instance */
static struct fwk_platdrv sgrt_xsdk_gem_platdriver =
{
    .probe	= xsdk_gem_driver_probe,
    .remove	= xsdk_gem_driver_remove,
    
    .sgrt_driver =
    {
        .name 	= XSDK_GEM_DRIVER_NAME,
        .id 	= -1,
        .sprt_of_match_table = sgrt_xsdk_gem_driver_id,
    },
};

/*!< --------------------------------------------------------------------- */
/*!
 * @brief   xsdk_gem_driver_init
 * @param   none
 * @retval  errno
 * @note    none
 */
kint32_t __fwk_init xsdk_gem_driver_init(void)
{
    return fwk_register_platdriver(&sgrt_xsdk_gem_platdriver);
}

/*!
 * @brief   xsdk_gem_driver_exit
 * @param   none
 * @retval  none
 * @note    none
 */
void __fwk_exit xsdk_gem_driver_exit(void)
{
    fwk_unregister_platdriver(&sgrt_xsdk_gem_platdriver);
}

IMPORT_DRIVER_INIT(xsdk_gem_driver_init);
IMPORT_DRIVER_EXIT(xsdk_gem_driver_exit);

/*!< end of file */
