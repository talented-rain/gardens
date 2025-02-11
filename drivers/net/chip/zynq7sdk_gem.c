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

#include <kernel/sched.h>
#include <kernel/sleep.h>

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
typedef enum 
{
    NR_ETH_LINK_UNDEFINED = 0,
    NR_ETH_LINK_UP,
    NR_ETH_LINK_DOWN,
    NR_ETH_LINK_NEGOTIATING

} nrt_link_status_t;

#define XEMACPS_IS_ETH_0(x)                     ((x)->Config.BaseAddress == XPAR_PS7_ETHERNET_0_BASEADDR)

struct xsdk_gem_phy
{
    kint32_t link_speed;
    kuint32_t phymapemac0[32];
    kuint32_t phymapemac1[32];
    kuint32_t phyaddrforemac;

    nrt_link_status_t eth_link_status;
};

struct xsdk_gem_drv_data
{
    void *base;
    kint32_t irq;

    kuint16_t hwaddr[NET_MAC_ETH_ALEN];

    xemacpsif_s sgrt_xemacif;
    XEmacPs_Config sgrt_config;

    struct xsdk_gem_phy sgrt_phy;
};

#define XSDK_GEM_DRIVER_NAME                    "gem0"

/*!< phy */
#define PHY_DETECT_REG                          1
#define PHY_IDENTIFIER_1_REG                    2
#define PHY_IDENTIFIER_2_REG                    3
#define PHY_DETECT_MASK                         0x1808
#define PHY_MARVELL_IDENTIFIER                  0x0141
#define PHY_TI_IDENTIFIER                       0x2000
#define PHY_REALTEK_IDENTIFIER                  0x001c
#define PHY_XILINX_PCS_PMA_ID1                  0x0174
#define PHY_XILINX_PCS_PMA_ID2                  0x0C00

#define PHY_REGCR                               0x0D
#define PHY_ADDAR                               0x0E
#define PHY_RGMIIDCTL                           0x86
#define PHY_RGMIICTL                            0x32
#define PHY_STS                                 0x11
#define PHY_TI_CR                               0x10
#define PHY_TI_CFG4                             0x31

#define PHY_REGCR_ADDR                          0x001F
#define PHY_REGCR_DATA                          0x401F
#define PHY_TI_CRVAL                            0x5048
#define PHY_TI_CFG4RESVDBIT7                    0x80

/*!< Bd Ring */
#define BD_ALIGNMENT                            (XEMACPS_DMABD_MINIMUM_ALIGNMENT << 1)

#define XNET_CONFIG_N_TX_DESC                   512
#define XNET_CONFIG_N_RX_DESC                   512

/*!< The globals */
static kuint8_t bd_space[0x100000] __align(0x100000);

/*!< The functions */

/*!< API function */
static kint32_t xsdk_get_ti_phy_speed(XEmacPs *sprt_xemacps, kuint32_t phy_addr)
{
    kuint16_t control;
    kuint16_t status;
    kuint16_t status_speed;
    kuint32_t timeout_counter = 0;
    kuint32_t phyregtemp;
    int i;
    kint32_t RetStatus;

    print_debug("Start PHY autonegotiation \r\n");

    XEmacPs_PhyRead(sprt_xemacps, phy_addr, 0x1F, (kuint16_t *)&phyregtemp);
    phyregtemp |= 0x4000;
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, 0x1F, phyregtemp);

    RetStatus = XEmacPs_PhyRead(sprt_xemacps, phy_addr, 0x1F, (kuint16_t *)&phyregtemp);
    if (RetStatus) 
    {
        print_debug("Error during sw reset \n\r");
        return RetStatus;
    }

    XEmacPs_PhyRead(sprt_xemacps, phy_addr, 0, (kuint16_t *)&phyregtemp);
    phyregtemp |= 0x8000;
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, 0, phyregtemp);

    /*!< Delay */
    sleep(2);

    RetStatus = XEmacPs_PhyRead(sprt_xemacps, phy_addr, 0, (kuint16_t *)&phyregtemp);
    if (RetStatus)
    {
        print_debug("Error during reset \n\r");
        return RetStatus;
    }

    /*!< FIFO depth */
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, PHY_TI_CR, PHY_TI_CRVAL);
    RetStatus = XEmacPs_PhyRead(sprt_xemacps, phy_addr, PHY_TI_CR, (kuint16_t *)&phyregtemp);
    if (RetStatus)
    {
        print_debug("Error writing to 0x10 \n\r");
        return RetStatus;
    }

    /*!< TX/RX tuning */
    /*!< Write to PHY_RGMIIDCTL */
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, PHY_REGCR, PHY_REGCR_ADDR);
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, PHY_ADDAR, PHY_RGMIIDCTL);
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, PHY_REGCR, PHY_REGCR_DATA);
    RetStatus = XEmacPs_PhyWrite(sprt_xemacps, phy_addr, PHY_ADDAR, 0xA8);
    if (RetStatus)
    {
        print_debug("Error in tuning");
        return RetStatus;
    }

    /*!< Read PHY_RGMIIDCTL */
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, PHY_REGCR, PHY_REGCR_ADDR);
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, PHY_ADDAR, PHY_RGMIIDCTL);
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, PHY_REGCR, PHY_REGCR_DATA);
    RetStatus = XEmacPs_PhyRead(sprt_xemacps, phy_addr, PHY_ADDAR, (kuint16_t *)&phyregtemp);
    if (RetStatus)
    {
        print_debug("Error in tuning");
        return RetStatus;
    }

    /*!< Write PHY_RGMIICTL */
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, PHY_REGCR, PHY_REGCR_ADDR);
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, PHY_ADDAR, PHY_RGMIICTL);
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, PHY_REGCR, PHY_REGCR_DATA);
    RetStatus = XEmacPs_PhyWrite(sprt_xemacps, phy_addr, PHY_ADDAR, 0xD3);
    if (RetStatus)
    {
        print_debug("Error in tuning");
        return RetStatus;
    }

    /*!< Read PHY_RGMIICTL */
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, PHY_REGCR, PHY_REGCR_ADDR);
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, PHY_ADDAR, PHY_RGMIICTL);
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, PHY_REGCR, PHY_REGCR_DATA);
    RetStatus = XEmacPs_PhyRead(sprt_xemacps, phy_addr, PHY_ADDAR, (kuint16_t *)&phyregtemp);
    if (RetStatus)
    {
        print_debug("Error in tuning");
        return RetStatus;
    }

    /*!< SW workaround for unstable link when RX_CTRL is not STRAP MODE 3 or 4 */
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, PHY_REGCR, PHY_REGCR_ADDR);
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, PHY_ADDAR, PHY_TI_CFG4);
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, PHY_REGCR, PHY_REGCR_DATA);

    RetStatus = XEmacPs_PhyRead(sprt_xemacps, phy_addr, PHY_ADDAR, (kuint16_t *)&phyregtemp);
    phyregtemp &= ~(PHY_TI_CFG4RESVDBIT7);
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, PHY_REGCR, PHY_REGCR_ADDR);
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, PHY_ADDAR, PHY_TI_CFG4);
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, PHY_REGCR, PHY_REGCR_DATA);
    RetStatus = XEmacPs_PhyWrite(sprt_xemacps, phy_addr, PHY_ADDAR, phyregtemp);

    XEmacPs_PhyRead(sprt_xemacps, phy_addr, IEEE_AUTONEGO_ADVERTISE_REG, &control);
    control |= IEEE_ASYMMETRIC_PAUSE_MASK;
    control |= IEEE_PAUSE_MASK;
    control |= ADVERTISE_100;
    control |= ADVERTISE_10;
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, IEEE_AUTONEGO_ADVERTISE_REG, control);

    XEmacPs_PhyRead(sprt_xemacps, phy_addr, IEEE_1000_ADVERTISE_REG_OFFSET, &control);
    control |= ADVERTISE_1000;
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, IEEE_1000_ADVERTISE_REG_OFFSET, control);

    XEmacPs_PhyRead(sprt_xemacps, phy_addr, IEEE_CONTROL_REG_OFFSET, &control);
    control |= IEEE_CTRL_AUTONEGOTIATE_ENABLE;
    control |= IEEE_STAT_AUTONEGOTIATE_RESTART;
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, IEEE_CONTROL_REG_OFFSET, control);

    XEmacPs_PhyRead(sprt_xemacps, phy_addr, IEEE_CONTROL_REG_OFFSET, &control);
    XEmacPs_PhyRead(sprt_xemacps, phy_addr, IEEE_STATUS_REG_OFFSET, &status);

    print_debug("Waiting for PHY to complete autonegotiation.\r\n");

    while (!(status & IEEE_STAT_AUTONEGOTIATE_COMPLETE)) 
    {
        sleep(1);
        timeout_counter++;

        if (timeout_counter == 30) 
        {
            print_debug("Auto negotiation error \r\n");
            return -ER_TIMEOUT;
        }

        XEmacPs_PhyRead(sprt_xemacps, phy_addr, IEEE_STATUS_REG_OFFSET, &status);
    }

    print_debug("autonegotiation complete \r\n");

    XEmacPs_PhyRead(sprt_xemacps, phy_addr, PHY_STS, &status_speed);
    if ((status_speed & 0xC000) == 0x8000)
        return 1000;
    if ((status_speed & 0xC000) == 0x4000)
        return 100;

    return 10;
}

static kint32_t xsdk_get_realtek_phy_speed(XEmacPs *sprt_xemacps, kuint32_t phy_addr)
{
    kuint16_t control;
    kuint16_t status;
    kuint16_t status_speed;
    kuint32_t timeout_counter = 0;
    kuint32_t temp_speed;

    print_debug("Start PHY autonegotiation \r\n");

    XEmacPs_PhyRead(sprt_xemacps, phy_addr, IEEE_AUTONEGO_ADVERTISE_REG, &control);
    control |= IEEE_ASYMMETRIC_PAUSE_MASK;
    control |= IEEE_PAUSE_MASK;
    control |= ADVERTISE_100;
    control |= ADVERTISE_10;
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, IEEE_AUTONEGO_ADVERTISE_REG, control);

    XEmacPs_PhyRead(sprt_xemacps, phy_addr, IEEE_1000_ADVERTISE_REG_OFFSET, &control);
    control |= ADVERTISE_1000;
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, IEEE_1000_ADVERTISE_REG_OFFSET, control);

    XEmacPs_PhyRead(sprt_xemacps, phy_addr, IEEE_CONTROL_REG_OFFSET, &control);
    control |= IEEE_CTRL_AUTONEGOTIATE_ENABLE;
    control |= IEEE_STAT_AUTONEGOTIATE_RESTART;
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, IEEE_CONTROL_REG_OFFSET, control);

    XEmacPs_PhyRead(sprt_xemacps, phy_addr, IEEE_CONTROL_REG_OFFSET, &control);
    control |= IEEE_CTRL_RESET_MASK;
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, IEEE_CONTROL_REG_OFFSET, control);

    while (1) 
    {
        XEmacPs_PhyRead(sprt_xemacps, phy_addr, IEEE_CONTROL_REG_OFFSET, &control);
        if (!(control & IEEE_CTRL_RESET_MASK))        
            break;
    }

    XEmacPs_PhyRead(sprt_xemacps, phy_addr, IEEE_STATUS_REG_OFFSET, &status);

    print_debug("Waiting for PHY to complete autonegotiation.\r\n");

    while ( !(status & IEEE_STAT_AUTONEGOTIATE_COMPLETE) ) 
    {
        sleep(1);
        timeout_counter++;

        if (timeout_counter == 30) 
        {
            print_debug("Auto negotiation error \r\n");
            return -ER_TIMEOUT;
        }

        XEmacPs_PhyRead(sprt_xemacps, phy_addr, IEEE_STATUS_REG_OFFSET, &status);
    }
    print_debug("autonegotiation complete \r\n");

    XEmacPs_PhyRead(sprt_xemacps, phy_addr,IEEE_SPECIFIC_STATUS_REG, &status_speed);
    if (status_speed & 0x400) 
    {
        temp_speed = status_speed & IEEE_SPEED_MASK;

        if (temp_speed == IEEE_SPEED_1000)
            return 1000;
        if(temp_speed == IEEE_SPEED_100)
            return 100;

        return 10;
    }

    return -ER_ERROR;
}

static kint32_t xsdk_get_marvell_phy_speed(XEmacPs *sprt_xemacps, kuint32_t phy_addr)
{
    kuint16_t temp;
    kuint16_t control;
    kuint16_t status;
    kuint16_t status_speed;
    kuint32_t timeout_counter = 0;
    kuint32_t temp_speed;

    print_debug("Start PHY autonegotiation \r\n");

    XEmacPs_PhyWrite(sprt_xemacps,phy_addr, IEEE_PAGE_ADDRESS_REGISTER, 2);
    XEmacPs_PhyRead(sprt_xemacps, phy_addr, IEEE_CONTROL_REG_MAC, &control);
    control |= IEEE_RGMII_TXRX_CLOCK_DELAYED_MASK;
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, IEEE_CONTROL_REG_MAC, control);

    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, IEEE_PAGE_ADDRESS_REGISTER, 0);

    XEmacPs_PhyRead(sprt_xemacps, phy_addr, IEEE_AUTONEGO_ADVERTISE_REG, &control);
    control |= IEEE_ASYMMETRIC_PAUSE_MASK;
    control |= IEEE_PAUSE_MASK;
    control |= ADVERTISE_100;
    control |= ADVERTISE_10;
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, IEEE_AUTONEGO_ADVERTISE_REG, control);

    XEmacPs_PhyRead(sprt_xemacps, phy_addr, IEEE_1000_ADVERTISE_REG_OFFSET, &control);
    control |= ADVERTISE_1000;
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, IEEE_1000_ADVERTISE_REG_OFFSET, control);

    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, IEEE_PAGE_ADDRESS_REGISTER, 0);
    XEmacPs_PhyRead(sprt_xemacps, phy_addr, IEEE_COPPER_SPECIFIC_CONTROL_REG, &control);
    /*!< max number of gigabit attempts */
    control |= (7 << 12);
    /*!< enable downshift */
    control |= (1 << 11);
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, IEEE_COPPER_SPECIFIC_CONTROL_REG, control);
    XEmacPs_PhyRead(sprt_xemacps, phy_addr, IEEE_CONTROL_REG_OFFSET, &control);
    control |= IEEE_CTRL_AUTONEGOTIATE_ENABLE;
    control |= IEEE_STAT_AUTONEGOTIATE_RESTART;
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, IEEE_CONTROL_REG_OFFSET, control);

    XEmacPs_PhyRead(sprt_xemacps, phy_addr, IEEE_CONTROL_REG_OFFSET, &control);
    control |= IEEE_CTRL_RESET_MASK;
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, IEEE_CONTROL_REG_OFFSET, control);

    while (1) 
    {
        XEmacPs_PhyRead(sprt_xemacps, phy_addr, IEEE_CONTROL_REG_OFFSET, &control);
        if (!(control & IEEE_CTRL_RESET_MASK))
            break;
    }

    XEmacPs_PhyRead(sprt_xemacps, phy_addr, IEEE_STATUS_REG_OFFSET, &status);

    print_debug("Waiting for PHY to complete autonegotiation.\r\n");

    while (!(status & IEEE_STAT_AUTONEGOTIATE_COMPLETE)) 
    {
        sleep(1);
        XEmacPs_PhyRead(sprt_xemacps, 
                phy_addr, IEEE_COPPER_SPECIFIC_STATUS_REG_2, &temp);
        timeout_counter++;

        if (timeout_counter == 30) 
        {
            print_debug("Auto negotiation error \r\n");
            return -ER_TIMEOUT;
        }

        XEmacPs_PhyRead(sprt_xemacps, phy_addr, IEEE_STATUS_REG_OFFSET, &status);
    }
    print_debug("autonegotiation complete \r\n");

    XEmacPs_PhyRead(sprt_xemacps, phy_addr,IEEE_SPECIFIC_STATUS_REG, &status_speed);
    if (status_speed & 0x400) 
    {
        temp_speed = status_speed & IEEE_SPEED_MASK;

        if (temp_speed == IEEE_SPEED_1000)
            return 1000;
        if(temp_speed == IEEE_SPEED_100)
            return 100;
        
        return 10;
    }

    return -ER_ERROR;
}

static kint32_t xsdk_gem_phy_ieee_speed(XEmacPs *sprt_emacps, kuint32_t phy_addr)
{
    kuint16_t phy_identity;
    kint32_t RetStatus;

    XEmacPs_PhyRead(sprt_emacps, phy_addr, PHY_IDENTIFIER_1_REG, &phy_identity);

    if (phy_identity == PHY_TI_IDENTIFIER)
        RetStatus = XEmacPs_GetTiPhySpeed(sprt_emacps, phy_addr);
    else if (phy_identity == PHY_REALTEK_IDENTIFIER)
        RetStatus = XEmacPs_GetRealtekPhySpeed(sprt_emacps, phy_addr);
    else
        RetStatus = XEmacPs_GetMarvellPhySpeed(sprt_emacps, phy_addr);

    return RetStatus;
}

static void xsdk_gem_phy_detect(XEmacPs *sprt_emacps, kuint32_t *phymapemac0, kuint32_t *phymapemac1)
{
    kuint16_t phy_reg;
    kuint32_t phy_addr;
    kuint32_t *phymapemac;

    phymapemac = XEMACPS_IS_ETH_0(sprt_emacps) ? phymapemac0 : phymapemac1;

    for (phy_addr = 31; phy_addr > 0; phy_addr--) 
    {
        XEmacPs_PhyRead(sprt_emacps, phy_addr, PHY_DETECT_REG, &phy_reg);

        if ((phy_reg != 0xFFFF) &&
            ((phy_reg & PHY_DETECT_MASK) == PHY_DETECT_MASK)) 
        {
            /*!< Found a valid PHY address */
            print_debug("XEmacPs %s: PHY detected at address %d.\r\n", __FUNCTION__, phy_addr);

            phymapemac[phy_addr] = true;
            XEmacPs_PhyRead(sprt_emacps, phy_addr, PHY_IDENTIFIER_1_REG, &phy_reg);

            if ((phy_reg != PHY_MARVELL_IDENTIFIER) &&
                (phy_reg != PHY_TI_IDENTIFIER) &&
                (phy_reg != PHY_REALTEK_IDENTIFIER))
                print_debug("WARNING: Not a Marvell or TI or Realtek Ethernet PHY. Please verify the initialization sequence\r\n");
        }
    }
}

static void xsdk_gem_emac_init(struct xsdk_gem_drv_data *sprt_data)
{
    XEmacPs *sprt_emacps;
    struct xsdk_gem_phy *sprt_phy;
    kint32_t status;
    kuint32_t i;
    kbool_t phyfoundforemac0 = false;
    kbool_t phyfoundforemac1 = false;
    kuint32_t link_speed;

    sprt_emacps = &sprt_data->sgrt_xemacif.sgrt_emacps;
    sprt_phy = &sprt_data->sgrt_phy;

    /*!< set mac address */
    status = XEmacPs_SetMacAddress(sprt_emacps, sprt_data->hwaddr, 1);
    if (status)
        print_debug("In %s: Emac Mac Address set failed...\r\n", __func__);

    XEmacPs_SetMdioDivisor(sprt_emacps, MDC_DIV_224);
    xsdk_gem_phy_detect(sprt_emacps, &sprt_phy->phymapemac0[0], &sprt_phy->phymapemac1[0]);

    for (i = 31; i > 0; i--) 
    {
        if (XEMACPS_IS_ETH_0(sprt_emacps)) 
        {
            if (sprt_phy->phymapemac0[i] == true) 
            {
                link_speed = xsdk_gem_phy_ieee_speed(sprt_emacps, i);
                sprt_phy->link_speed = XEmacPs_PhySetup(sprt_emacps, i, link_speed);
                phyfoundforemac0 = true;
                sprt_phy->phyaddrforemac = i;
            }
        } 
        else
        {
            if (sprt_phy->phymapemac1[i] == true) 
            {
                link_speed = xsdk_gem_phy_ieee_speed(sprt_emacps, i);
                sprt_phy->link_speed = XEmacPs_PhySetup(sprt_emacps, i, link_speed);
                phyfoundforemac1 = true;
                sprt_phy->phyaddrforemac = i;
            }
        }
    }

    /*!< If no PHY was detected, use broadcast PHY address of 0 */
    if (XEMACPS_IS_ETH_0(sprt_emacps)) 
    {
        if (phyfoundforemac0 == false)
        {
            link_speed = xsdk_gem_phy_ieee_speed(sprt_emacps, 0);
            sprt_phy->link_speed = XEmacPs_PhySetup(sprt_emacps, 0, link_speed);
        }
    }
    else
    {
        if (phyfoundforemac1 == false)
        {
            link_speed = xsdk_gem_phy_ieee_speed(sprt_emacps, 0);
            sprt_phy->link_speed = XEmacPs_PhySetup(sprt_emacps, 0, link_speed);
        }
    }

    if (sprt_phy->link_speed < 0) 
    {
        sprt_phy->eth_link_status = NR_ETH_LINK_DOWN;
        print_debug("Assert due to phy setup failure \n\r", __func__);
    } 
    else
        sprt_phy->eth_link_status = NR_ETH_LINK_UP;

    XEmacPs_SetOperatingSpeed(sprt_emacps, sprt_phy->link_speed);

    /*!< Setting the operating speed of the MAC needs a delay. */
}

static void xsdk_gem_dma_tx_descs_clean(struct xsdk_gem_drv_data *sprt_data)
{
    XEmacPs_Bd sgrt_bd;
    XEmacPs_BdRing *sprt_txbdring;
    xemacpsif_s *sprt_xemacif = &sprt_data->sgrt_xemacif;

    sprt_txbdring = &XEmacPs_GetTxRing(&sprt_xemacif->sgrt_emacps);

    XEmacPs_BdClear(&sgrt_bd);
    XEmacPs_BdSetStatus(&sgrt_bd, XEMACPS_TXBUF_USED_MASK);

    /*!< Create the TxBD ring */
    XEmacPs_BdRingCreate(sprt_txbdring, (kuint32_t)sprt_xemacif->tx_bdspace,
            (kuint32_t) sprt_xemacif->tx_bdspace, BD_ALIGNMENT, XNET_CONFIG_N_TX_DESC);
    XEmacPs_BdRingClone(sprt_txbdring, &sgrt_bd, XEMACPS_SEND);
}

static kint32_t xsdk_gem_dma_init(struct xsdk_gem_drv_data *sprt_data)
{
    
}

static kint32_t xsdk_gem_ndo_init(struct fwk_net_device *sprt_ndev)
{
    struct xsdk_gem_drv_data *sprt_data;
    xemacpsif_s *sprt_xemacif;
    XEmacPs_Config *sprt_config;
    struct xsdk_gem_phy *sprt_phy;
    kint32_t retval;

    sprt_data = (struct xsdk_gem_drv_data *)fwk_netdev_priv(sprt_ndev);
    
    sprt_xemacif = &sprt_data->sgrt_xemacif;
    sprt_xemacif->sprt_txq = mrt_nullptr;
    sprt_xemacif->sprt_rxq = pq_queue_create(NR_PQ_DROP, 4096);
    if (!sprt_xemacif->sprt_rxq)
        return -ER_NOMEM;

    sprt_config = &sprt_data->sgrt_config;
    sprt_config->DeviceId = XPAR_PS7_ETHERNET_0_DEVICE_ID;
    sprt_config->BaseAddress = (kuint32_t)sprt_data->base;
    sprt_config->IsCacheCoherent = XPAR_PS7_ETHERNET_0_IS_CACHE_COHERENT;

    retval = XEmacPs_CfgInitialize(&sprt_xemacif->sgrt_emacps, sprt_config, sprt_config->BaseAddress);
    if (retval)
        return retval;

    sprt_phy = &sprt_data->sgrt_phy;
    sprt_phy->link_speed = 100;
    sprt_phy->eth_link_status = NR_ETH_LINK_UNDEFINED;
    sprt_phy->phyaddrforemac = 0;
    memset(&sprt_phy->phymapemac0[0], 0, sizeof(sprt_phy->phymapemac0));
    memset(&sprt_phy->phymapemac1[0], 0, sizeof(sprt_phy->phymapemac1));



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
    sprt_data->base = fwk_io_remap(base, ARCH_PER_SIZE);
    sprt_data->irq = fwk_platform_get_irq(sprt_pdev, 0);

    fwk_of_property_read_u16_array(sprt_phy, "local-mac-address", sprt_data->hwaddr, NET_MAC_ETH_ALEN);
    return ER_NORMAL;
}

/*!
 * @brief   xsdk_gem_driver_probe
 * @param   sprt_pdev
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
 * @param   sprt_pdev
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
