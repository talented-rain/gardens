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
typedef enum {
    NR_ETH_LINK_UNDEFINED = 0,
    NR_ETH_LINK_UP,
    NR_ETH_LINK_DOWN,
    NR_ETH_LINK_NEGOTIATING

} nrt_link_status_t;

#define XEMACPS_IS_ETH_0(x)                     ((x)->Config.BaseAddress == XPAR_PS7_ETHERNET_0_BASEADDR)

/*!< Bd Ring */
#define BD_ALIGNMENT                            (XEMACPS_DMABD_MINIMUM_ALIGNMENT << 1)

#define BD_SIZE                                 sizeof(XEmacPs_Bd)
#define XNET_TX_DESC_NUM                        (512)
#define XNET_RX_DESC_NUM                        (512)
#define XNET_TX_DESC_SIZE                       (XNET_TX_DESC_NUM * BD_SIZE)
#define XNET_RX_DESC_SIZE                       (XNET_RX_DESC_NUM * BD_SIZE)

#define XEMACPS_BD_TO_INDEX(sprt_ring, sprt_bd) \
        (((kuint32_t)(sprt_bd) - (kuint32_t)(sprt_ring)->BaseBdAddr) / (sprt_ring)->Separation)

struct xsdk_gem_phy {
    kint32_t link_speed;
    kuint32_t phymapemac0[32];
    kuint32_t phymapemac1[32];
    kuint32_t phyaddrforemac;

    nrt_link_status_t eth_link_status;
};

struct xsdk_gem_drv_data {
    void *base;
    kint32_t irq;

    kuint16_t hwaddr[NET_MAC_ETH_ALEN];
    struct fwk_net_device *sprt_ndev;

    xemacpsif_s sgrt_xemacif;
    XEmacPs_Config sgrt_config;

    struct xsdk_gem_phy sgrt_phy;

    struct fwk_sk_buff *sprt_rxskbs[XNET_RX_DESC_NUM];
    struct fwk_sk_buff *sprt_txskbs[XNET_TX_DESC_NUM];
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

/*!< The globals */

/*!< The functions */

/*!< API function */
static kint32_t xsdk_get_ti_phy_speed(XEmacPs *sprt_xemacps, kuint32_t phy_addr)
{
    kuint16_t control;
    kuint16_t status;
    kuint16_t status_speed;
    kuint32_t timeout_counter = 0;
    kuint32_t phyregtemp;
    kint32_t RetStatus;

    print_debug("Start PHY autonegotiation \r\n");

    XEmacPs_PhyRead(sprt_xemacps, phy_addr, 0x1F, (kuint16_t *)&phyregtemp);
    phyregtemp |= 0x4000;
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, 0x1F, phyregtemp);

    RetStatus = XEmacPs_PhyRead(sprt_xemacps, phy_addr, 0x1F, (kuint16_t *)&phyregtemp);
    if (RetStatus) {
        print_debug("Error during sw reset \n\r");
        return RetStatus;
    }

    XEmacPs_PhyRead(sprt_xemacps, phy_addr, 0, (kuint16_t *)&phyregtemp);
    phyregtemp |= 0x8000;
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, 0, phyregtemp);

    /*!< Delay */
    sleep(2);

    RetStatus = XEmacPs_PhyRead(sprt_xemacps, phy_addr, 0, (kuint16_t *)&phyregtemp);
    if (RetStatus) {
        print_debug("Error during reset \n\r");
        return RetStatus;
    }

    /*!< FIFO depth */
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, PHY_TI_CR, PHY_TI_CRVAL);
    RetStatus = XEmacPs_PhyRead(sprt_xemacps, phy_addr, PHY_TI_CR, (kuint16_t *)&phyregtemp);
    if (RetStatus) {
        print_debug("Error writing to 0x10 \n\r");
        return RetStatus;
    }

    /*!< TX/RX tuning */
    /*!< Write to PHY_RGMIIDCTL */
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, PHY_REGCR, PHY_REGCR_ADDR);
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, PHY_ADDAR, PHY_RGMIIDCTL);
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, PHY_REGCR, PHY_REGCR_DATA);

    RetStatus = XEmacPs_PhyWrite(sprt_xemacps, phy_addr, PHY_ADDAR, 0xA8);
    if (RetStatus) {
        print_debug("Error in tuning");
        return RetStatus;
    }

    /*!< Read PHY_RGMIIDCTL */
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, PHY_REGCR, PHY_REGCR_ADDR);
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, PHY_ADDAR, PHY_RGMIIDCTL);
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, PHY_REGCR, PHY_REGCR_DATA);
    
    RetStatus = XEmacPs_PhyRead(sprt_xemacps, phy_addr, PHY_ADDAR, (kuint16_t *)&phyregtemp);
    if (RetStatus) {
        print_debug("Error in tuning");
        return RetStatus;
    }

    /*!< Write PHY_RGMIICTL */
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, PHY_REGCR, PHY_REGCR_ADDR);
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, PHY_ADDAR, PHY_RGMIICTL);
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, PHY_REGCR, PHY_REGCR_DATA);
    
    RetStatus = XEmacPs_PhyWrite(sprt_xemacps, phy_addr, PHY_ADDAR, 0xD3);
    if (RetStatus) {
        print_debug("Error in tuning");
        return RetStatus;
    }

    /*!< Read PHY_RGMIICTL */
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, PHY_REGCR, PHY_REGCR_ADDR);
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, PHY_ADDAR, PHY_RGMIICTL);
    XEmacPs_PhyWrite(sprt_xemacps, phy_addr, PHY_REGCR, PHY_REGCR_DATA);
    
    RetStatus = XEmacPs_PhyRead(sprt_xemacps, phy_addr, PHY_ADDAR, (kuint16_t *)&phyregtemp);
    if (RetStatus) {
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

    while (!(status & IEEE_STAT_AUTONEGOTIATE_COMPLETE)) {
        sleep(1);
        timeout_counter++;

        if (timeout_counter == 30) {
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

    while (1) {
        XEmacPs_PhyRead(sprt_xemacps, phy_addr, IEEE_CONTROL_REG_OFFSET, &control);
        if (!(control & IEEE_CTRL_RESET_MASK))        
            break;
    }

    XEmacPs_PhyRead(sprt_xemacps, phy_addr, IEEE_STATUS_REG_OFFSET, &status);

    print_debug("Waiting for PHY to complete autonegotiation.\r\n");

    while (!(status & IEEE_STAT_AUTONEGOTIATE_COMPLETE)) {
        sleep(1);
        timeout_counter++;

        if (timeout_counter == 30) {
            print_debug("Auto negotiation error \r\n");
            return -ER_TIMEOUT;
        }

        XEmacPs_PhyRead(sprt_xemacps, phy_addr, IEEE_STATUS_REG_OFFSET, &status);
    }
    print_debug("autonegotiation complete \r\n");

    XEmacPs_PhyRead(sprt_xemacps, phy_addr,IEEE_SPECIFIC_STATUS_REG, &status_speed);
    if (status_speed & 0x400) {
        temp_speed = status_speed & IEEE_SPEED_MASK;

        if (temp_speed == IEEE_SPEED_1000)
            return 1000;
        if (temp_speed == IEEE_SPEED_100)
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

    while (1) {
        XEmacPs_PhyRead(sprt_xemacps, phy_addr, IEEE_CONTROL_REG_OFFSET, &control);
        if (!(control & IEEE_CTRL_RESET_MASK))
            break;
    }

    XEmacPs_PhyRead(sprt_xemacps, phy_addr, IEEE_STATUS_REG_OFFSET, &status);

    print_debug("Waiting for PHY to complete autonegotiation.\r\n");

    while (!(status & IEEE_STAT_AUTONEGOTIATE_COMPLETE)) {
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
    if (status_speed & 0x400) {
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
        RetStatus = xsdk_get_ti_phy_speed(sprt_emacps, phy_addr);
    else if (phy_identity == PHY_REALTEK_IDENTIFIER)
        RetStatus = xsdk_get_realtek_phy_speed(sprt_emacps, phy_addr);
    else
        RetStatus = xsdk_get_marvell_phy_speed(sprt_emacps, phy_addr);

    return RetStatus;
}

static void xsdk_gem_phy_detect(XEmacPs *sprt_emacps, kuint32_t *phymapemac0, kuint32_t *phymapemac1)
{
    kuint16_t phy_reg;
    kuint32_t phy_addr;
    kuint32_t *phymapemac;

    phymapemac = XEMACPS_IS_ETH_0(sprt_emacps) ? phymapemac0 : phymapemac1;

    for (phy_addr = 31; phy_addr > 0; phy_addr--) {
        XEmacPs_PhyRead(sprt_emacps, phy_addr, PHY_DETECT_REG, &phy_reg);

        if ((phy_reg != 0xFFFF) &&
            ((phy_reg & PHY_DETECT_MASK) == PHY_DETECT_MASK)) {
            
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

static void xsdk_gem_start_xmit(xemacpsif_s *sprt_xemacif)
{
    kuint32_t reg;

    /*!< Start transmit */
    reg  = XEmacPs_ReadReg(sprt_xemacif->sgrt_emacps.Config.BaseAddress, XEMACPS_NWCTRL_OFFSET);
    reg |= XEMACPS_NWCTRL_STARTTX_MASK;
    XEmacPs_WriteReg(sprt_xemacif->sgrt_emacps.Config.BaseAddress, XEMACPS_NWCTRL_OFFSET, reg);
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

    for (i = 31; i > 0; i--) {
        if (XEMACPS_IS_ETH_0(sprt_emacps)) {
            if (sprt_phy->phymapemac0[i] == true) {
                link_speed = xsdk_gem_phy_ieee_speed(sprt_emacps, i);
                sprt_phy->link_speed = XEmacPs_PhySetup(sprt_emacps, i, link_speed);
                phyfoundforemac0 = true;
                sprt_phy->phyaddrforemac = i;
            }
        } 
        else {
            if (sprt_phy->phymapemac1[i] == true) {
                link_speed = xsdk_gem_phy_ieee_speed(sprt_emacps, i);
                sprt_phy->link_speed = XEmacPs_PhySetup(sprt_emacps, i, link_speed);
                phyfoundforemac1 = true;
                sprt_phy->phyaddrforemac = i;
            }
        }
    }

    /*!< If no PHY was detected, use broadcast PHY address of 0 */
    if (XEMACPS_IS_ETH_0(sprt_emacps)) {
        if (phyfoundforemac0 == false) {
            link_speed = xsdk_gem_phy_ieee_speed(sprt_emacps, 0);
            sprt_phy->link_speed = XEmacPs_PhySetup(sprt_emacps, 0, link_speed);
        }
    }
    else {
        if (phyfoundforemac1 == false) {
            link_speed = xsdk_gem_phy_ieee_speed(sprt_emacps, 0);
            sprt_phy->link_speed = XEmacPs_PhySetup(sprt_emacps, 0, link_speed);
        }
    }

    if (sprt_phy->link_speed < 0) {
        sprt_phy->eth_link_status = NR_ETH_LINK_DOWN;
        print_debug("Assert due to phy setup failure \n\r", __func__);
    } 
    else {
        sprt_phy->eth_link_status = NR_ETH_LINK_UP;
    }

    XEmacPs_SetOperatingSpeed(sprt_emacps, sprt_phy->link_speed);

    /*!< Setting the operating speed of the MAC needs a delay. */
}

/*!<  */
static void xsdk_gem_dma_tx_descs_clean(struct xsdk_gem_drv_data *sprt_data)
{
    XEmacPs_Bd sgrt_bd;
    XEmacPs_BdRing *sprt_txbdring;
    xemacpsif_s *sprt_xemacif = &sprt_data->sgrt_xemacif;

    sprt_txbdring = &XEmacPs_GetTxRing(&sprt_xemacif->sgrt_emacps);

    XEmacPs_BdClear(&sgrt_bd);
    XEmacPs_BdSetStatus(&sgrt_bd, XEMACPS_TXBUF_USED_MASK);

    /*!< Create the TxBD ring */
    XEmacPs_BdRingInitial(sprt_txbdring, (kuint32_t)sprt_xemacif->tx_bdspace,
            (kuint32_t)sprt_xemacif->tx_bdspace, BD_ALIGNMENT, XNET_TX_DESC_NUM);
    XEmacPs_BdRingClone(sprt_txbdring, &sgrt_bd, XEMACPS_SEND);
}

static kint32_t xsdk_gem_get_tx_freecnt(struct xsdk_gem_drv_data *sprt_data)
{
    xemacpsif_s *sprt_xemacif = &sprt_data->sgrt_xemacif;
    XEmacPs_BdRing *sprt_txbdring;
    kint32_t freecnt = 0;

    sprt_txbdring = &XEmacPs_GetTxRing(&sprt_xemacif->sgrt_emacps);

    /*!< tx space is available as long as there are valid BD's */
    freecnt = XEmacPs_BdRingGetFreeCnt(sprt_txbdring);
    return freecnt;
}

static kint32_t xsdk_gem_setup_rxbd(struct xsdk_gem_drv_data *sprt_data, nrt_gfp_t mask)
{
    xemacpsif_s *sprt_xemacif = &sprt_data->sgrt_xemacif;
    XEmacPs_BdRing *sprt_rxring;
    struct fwk_sk_buff *sprt_skb;
    kuint32_t freebds;
    kuint32_t bdindex;
    XEmacPs_Bd *sprt_rxbd;
    kuint32_t *temp;
    void *buffer;
    kint32_t status;

    sprt_rxring = &XEmacPs_GetRxRing(&sprt_xemacif->sgrt_emacps);
    freebds = XEmacPs_BdRingGetFreeCnt(sprt_rxring);

    while (freebds--) {
        sprt_skb = fwk_alloc_skb(XEMACPS_MAX_FRAME_SIZE + SKB_DATA_HEAD_LEN(NET_ETHER_HDR_LEN), mask);
        if (!isValid(sprt_skb)) {
            print_err("%s: unable to alloc sk_buff\n", __func__);
            return -ER_NOMEM;
        }

        fwk_skb_reserve(sprt_skb, SKB_DATA_HEAD_LEN(NET_ETHER_HDR_LEN));
        buffer = fwk_skb_put(sprt_skb, XEMACPS_MAX_FRAME_SIZE);
        if (!isValid(buffer)) {
            fwk_free_skb(sprt_skb);
            return -ER_NOMEM;
        }

        /*!< free_BD ---> pre_BD */
        status = XEmacPs_BdRingAlloc(sprt_rxring, 1, &sprt_rxbd);
        if (status) {
            fwk_free_skb(sprt_skb);
            print_err("%s: Error allocating RxBD\n", __FUNCTION__);

            return -ER_BUSY;
        }

        /*!< pre_BD ---> work_BD */
        status = XEmacPs_BdRingToHw(sprt_rxring, 1, sprt_rxbd);
        if (status) {
            fwk_free_skb(sprt_skb);
            XEmacPs_BdRingUnAlloc(sprt_rxring, 1, sprt_rxbd);
            print_err("%s: Error: committing RxBD to HW\n", __FUNCTION__);

            return -ER_BUSY;
        }

        bdindex = XEMACPS_BD_TO_INDEX(sprt_rxring, sprt_rxbd);
        temp = (kuint32_t *)sprt_rxbd;

        *temp = *(temp + 1) = 0;
        if (bdindex == (XNET_RX_DESC_NUM - 1))
            *temp = 0x00000002;

        mrt_dsb();

        if (!sprt_xemacif->sgrt_emacps.Config.IsCacheCoherent)
            Xil_DCacheInvalidateRange((kuaddr_t)buffer, XEMACPS_MAX_FRAME_SIZE);

        XEmacPs_BdSetAddressRx(sprt_rxbd, buffer);
        sprt_data->sprt_rxskbs[bdindex] = sprt_skb;
    }

    return ER_NORMAL;
}

static kint32_t xsdk_gem_dma_init(struct xsdk_gem_drv_data *sprt_data)
{
    xemacpsif_s *sprt_xemacif = &sprt_data->sgrt_xemacif;
    XEmacPs_BdRing *sprt_rxring, *sprt_txring;
    kuint32_t gige_version;
    void *bd_space;
    XEmacPs_Bd sgrt_bdtemp;
    kint32_t status;

    sprt_rxring = &XEmacPs_GetRxRing(&sprt_xemacif->sgrt_emacps);
    sprt_txring = &XEmacPs_GetTxRing(&sprt_xemacif->sgrt_emacps);

    /*!< allocate 8K for Rx bds, and 8K for Tx bds */
    bd_space = kmalloc((XNET_RX_DESC_SIZE + XNET_TX_DESC_SIZE) * 2, GFP_KERNEL);
    if (!isValid(bd_space))
        return -ER_NOMEM;

    sprt_xemacif->rx_bdspace = bd_space;
    sprt_xemacif->tx_bdspace = bd_space + (XNET_RX_DESC_SIZE << 1);

    /*!<
     * Setup RxBD space.
     *
     * Setup a BD template for the Rx channel. This template will be copied to
     * every RxBD. We will not have to explicitly set these again.
     */
    XEmacPs_BdClear(&sgrt_bdtemp);

    /*!< Create the RxBD ring */
    status = XEmacPs_BdRingCreate(sprt_rxring, (kuint32_t)sprt_xemacif->rx_bdspace,
                (kuint32_t)sprt_xemacif->rx_bdspace, BD_ALIGNMENT, XNET_RX_DESC_NUM);
    if (status) {
        print_err("Error setting up RxBD space\r\n");
        goto fail;
    }

    /*!< initial sprt_rxring with sgrt_bdtemp */
    status = XEmacPs_BdRingClone(sprt_rxring, &sgrt_bdtemp, XEMACPS_RECV);
    if (status) {
        print_err("Error initializing RxBD space\r\n");
        goto fail;
    }

    /*!< Setup TxBD */
    XEmacPs_BdClear(&sgrt_bdtemp);
    XEmacPs_BdSetStatus(&sgrt_bdtemp, XEMACPS_TXBUF_USED_MASK);

    /*!< Create the TxBD ring */
    status = XEmacPs_BdRingCreate(sprt_txring, (kuint32_t)sprt_xemacif->tx_bdspace,
                (kuint32_t)sprt_xemacif->tx_bdspace, BD_ALIGNMENT, XNET_TX_DESC_NUM);
    if (status)
        goto fail;

    /*!< We reuse the bd template, as the same one will work for both rx and tx. */
    status = XEmacPs_BdRingClone(sprt_txring, &sgrt_bdtemp, XEMACPS_SEND);
    if (status)
        goto fail;

    /*!< 
     * Build Rx buffer
     * Method 1: define an array with 1500 * 512 bytes; but it needs to be copied to sk_buff again
     * Method 2: allocate a sk_buff in advance, and the received packets are stored directly to skb->data by DMA 
     */
    xsdk_gem_setup_rxbd(sprt_data, GFP_KERNEL);
    XEmacPs_SetQueuePtr(&sprt_xemacif->sgrt_emacps, sprt_xemacif->sgrt_emacps.RxBdRing.BaseBdAddr, 0, XEMACPS_RECV);

    gige_version = (XEmacPs_ReadReg(sprt_xemacif->sgrt_emacps.Config.BaseAddress, 0xFC) >> 16) & 0xfff;
    if (gige_version > 2) {
        XEmacPs_Bd *sprt_rxbd_terminate, *sprt_txbd_terminate;
        void *bd_terminate;

        /*!< allocate 8K for Rx bds, and 8K for Tx bds */
        bd_terminate = kmalloc((XNET_RX_DESC_SIZE + XNET_TX_DESC_SIZE) * 2, GFP_KERNEL);
        if (!isValid(bd_terminate))
            goto out;

        sprt_rxbd_terminate = (XEmacPs_Bd *)bd_terminate;
        sprt_txbd_terminate = (XEmacPs_Bd *)(bd_terminate + (XNET_RX_DESC_SIZE << 1));

        XEmacPs_SetQueuePtr(&sprt_xemacif->sgrt_emacps, sprt_xemacif->sgrt_emacps.TxBdRing.BaseBdAddr, 1, XEMACPS_SEND);

        /*!<
         * This version of GEM supports priority queuing and the current
         * dirver is using tx priority queue 1 and normal rx queue for
         * packet transmit and receive. The below code ensure that the
         * other queue pointers are parked to known state for avoiding
         * the controller to malfunction by fetching the descriptors
         * from these queues.
         */
        XEmacPs_BdClear(sprt_rxbd_terminate);
        XEmacPs_BdSetAddressRx(sprt_rxbd_terminate, (XEMACPS_RXBUF_NEW_MASK | XEMACPS_RXBUF_WRAP_MASK));
        XEmacPs_WriteReg(sprt_xemacif->sgrt_emacps.Config.BaseAddress, XEMACPS_RXQ1BASE_OFFSET, (kuint32_t)sprt_rxbd_terminate);

        XEmacPs_BdClear(sprt_txbd_terminate);
        XEmacPs_BdSetStatus(sprt_txbd_terminate, (XEMACPS_TXBUF_USED_MASK | XEMACPS_TXBUF_WRAP_MASK));
        XEmacPs_WriteReg(sprt_xemacif->sgrt_emacps.Config.BaseAddress, XEMACPS_TXQBASE_OFFSET, (kuint32_t)sprt_txbd_terminate);
    }
    else {
        XEmacPs_SetQueuePtr(&sprt_xemacif->sgrt_emacps, sprt_xemacif->sgrt_emacps.TxBdRing.BaseBdAddr, 0, XEMACPS_SEND);
    }

out:
    return ER_NORMAL;

fail:
    kfree(bd_space);
    memset(sprt_rxring, 0, sizeof(*sprt_rxring));
    memset(sprt_txring, 0, sizeof(*sprt_txring));
    sprt_xemacif->rx_bdspace = sprt_xemacif->tx_bdspace = mrt_nullptr;

    return -ER_FAILD;
}

static void xsdk_gem_send_bd(struct xsdk_gem_drv_data *sprt_data, struct fwk_sk_buff *sprt_skb)
{
    struct fwk_net_device *sprt_ndev = sprt_data->sprt_ndev;
    xemacpsif_s *sprt_xemacif = &sprt_data->sgrt_xemacif;
    XEmacPs_BdRing *sprt_txring;
    XEmacPs_Bd *sprt_txbd;
    kuint32_t bdindex;
    kint32_t status;

    sprt_txring = &XEmacPs_GetTxRing(&sprt_xemacif->sgrt_emacps);

    /*!< free-bd ---> pre-bd */
    status = XEmacPs_BdRingAlloc(sprt_txring, 1, &sprt_txbd);
    if (status) {
        print_err("%s: error allocating TxBD\r\n", __func__);
        goto fail;
    }

    bdindex = XEMACPS_BD_TO_INDEX(sprt_txring, sprt_txbd);
    if (sprt_data->sprt_txskbs[bdindex]) {
        print_err("%s: last skb is still existed! bdindex is invalid\n", __func__);
        goto fail;
    }

    /*!< 
     * Send the data from the skb to the interface, one skb at a time. 
     * The size of the data in each skb is kept in the skb->len variable. 
     */
    if (!sprt_xemacif->sgrt_emacps.Config.IsCacheCoherent)
        Xil_DCacheFlushRange((kuint32_t)sprt_skb->data, (kuint32_t)sprt_skb->len);

    XEmacPs_BdSetAddressTx(sprt_txbd, (kuint32_t)sprt_skb->data);

    if (sprt_skb->len > sprt_ndev->mtu)
        XEmacPs_BdSetLength(sprt_txbd, sprt_ndev->mtu & 0x3fff);
    else
        XEmacPs_BdSetLength(sprt_txbd, sprt_skb->len & 0x3fff);

    sprt_data->sprt_txskbs[bdindex] = sprt_skb;
    XEmacPs_BdSetLast(sprt_txbd);
    XEmacPs_BdClearTxUsed(sprt_txbd);
    mrt_dsb();

    status = XEmacPs_BdRingToHw(sprt_txring, 1, sprt_txbd);
    if (status) {
        print_debug("sgsend: Error submitting TxBD\r\n");
        goto fail;
    }

    /*!< Start transmit */
    xsdk_gem_start_xmit(sprt_xemacif);
    return;

fail:
    return;
}

static void xsdk_gem_sent_complete(struct xsdk_gem_drv_data *sprt_data)
{
    xemacpsif_s *sprt_xemacif = &sprt_data->sgrt_xemacif;
    XEmacPs_BdRing *sprt_txring;
    XEmacPs_Bd *sprt_txbd, *sprt_curbd;
    struct fwk_sk_buff *sprt_skb;
    kint32_t n_bds = 0, n_bds_freed;
    kint32_t bdindex;
    kuint32_t *temp;
    kint32_t status;

    sprt_txring = &XEmacPs_GetTxRing(&sprt_xemacif->sgrt_emacps);

    /*!< 
     * work-bd ---> post-bd
     * sprt_txbd: the first work-bd
     * n_bds: the number of post-bds
     */
    while ((n_bds = XEmacPs_BdRingFromHwTx(sprt_txring, 
                        XNET_TX_DESC_NUM, &sprt_txbd))) {
        n_bds_freed = n_bds;
        sprt_curbd = sprt_txbd;

        while (n_bds_freed--) {
            bdindex = XEMACPS_BD_TO_INDEX(sprt_txring, sprt_curbd);
            temp = (kuint32_t *)sprt_curbd;

            *temp = *(temp + 1) = 0;
            if (bdindex == (XNET_TX_DESC_NUM - 1))
                *temp = 0xc0000000;
            else
                *temp = 0x80000000;

            mrt_dsb();

            sprt_skb = sprt_data->sprt_txskbs[bdindex];
            if (sprt_skb)
                fwk_free_skb(sprt_skb);

            sprt_data->sprt_txskbs[bdindex] = mrt_nullptr;
            sprt_curbd = XEmacPs_BdRingNext(sprt_txring, sprt_curbd);
        }

        status = XEmacPs_BdRingFree(sprt_txring, n_bds, sprt_txbd);
        if (status)
            print_debug("Failure while freeing in Tx Done ISR\r\n");
    }
}

static void xsdk_gem_free_txbuf_any(struct xsdk_gem_drv_data *sprt_data)
{
    struct fwk_sk_buff **sprt_skb;
    kint32_t index;

    for (index = 0; index < XNET_TX_DESC_NUM; index++) {
        sprt_skb = &sprt_data->sprt_txskbs[index];

        if (*sprt_skb) {
            fwk_free_skb(*sprt_skb);
            *sprt_skb = mrt_nullptr;
        }
    }
}

static void xsdk_gem_free_rxbuf_any(struct xsdk_gem_drv_data *sprt_data)
{
    struct fwk_sk_buff **sprt_skb;
    kint32_t index;

    for (index = 0; index < XNET_RX_DESC_NUM; index++) {
        sprt_skb = &sprt_data->sprt_rxskbs[index];

        if (*sprt_skb) {
            fwk_free_skb(*sprt_skb);
            *sprt_skb = mrt_nullptr;
        }
    }
}

static void xsdk_gem_send_handler(struct xsdk_gem_drv_data *sprt_data)
{
    xemacpsif_s *sprt_xemacif = &sprt_data->sgrt_xemacif;
    kuint32_t reg;

    /*!< Clear interrupt status bit */
    reg = XEmacPs_ReadReg(sprt_xemacif->sgrt_emacps.Config.BaseAddress, XEMACPS_TXSR_OFFSET);
    XEmacPs_WriteReg(sprt_xemacif->sgrt_emacps.Config.BaseAddress, XEMACPS_TXSR_OFFSET, reg);

    /*!< If Transmit done interrupt is asserted, process completed BD's */
    xsdk_gem_sent_complete(sprt_data);
}

static void xsdk_gem_recv_handler(struct xsdk_gem_drv_data *sprt_data)
{
    xemacpsif_s *sprt_xemacif = &sprt_data->sgrt_xemacif;
    XEmacPs_BdRing *sprt_rxring;
    struct fwk_sk_buff *sprt_skb;
    struct fwk_net_device *sprt_ndev = sprt_data->sprt_ndev;
    kuint32_t reg, tempcntr;
    kuint32_t gige_version;
    kuint32_t n_bds;
    XEmacPs_Bd *sprt_rxbd;
    XEmacPs_Bd *sprt_curbd;
    kuint32_t bdindex;
    kuint32_t count;

    /*!<
     * If Reception done interrupt is asserted, call RX call back function
     * to handle the processed BDs and then raise the according flag.
     */
    reg = XEmacPs_ReadReg(sprt_xemacif->sgrt_emacps.Config.BaseAddress, XEMACPS_RXSR_OFFSET);
    XEmacPs_WriteReg(sprt_xemacif->sgrt_emacps.Config.BaseAddress, XEMACPS_RXSR_OFFSET, reg);

    gige_version = ((XEmacPs_ReadReg(sprt_xemacif->sgrt_emacps.Config.BaseAddress, 0xFC)) >> 16) & 0xFFF;
    if (gige_version == 2) {
        tempcntr = XEmacPs_ReadReg(sprt_xemacif->sgrt_emacps.Config.BaseAddress, XEMACPS_RXCNT_OFFSET);
        
        if ((!tempcntr) && (!sprt_xemacif->last_rx_frms_cntr)) {
            reg = XEmacPs_ReadReg(sprt_xemacif->sgrt_emacps.Config.BaseAddress, XEMACPS_NWCTRL_OFFSET);
            reg &= ~XEMACPS_NWCTRL_RXEN_MASK;
            XEmacPs_WriteReg(sprt_xemacif->sgrt_emacps.Config.BaseAddress, XEMACPS_NWCTRL_OFFSET, reg);

            reg = XEmacPs_ReadReg(sprt_xemacif->sgrt_emacps.Config.BaseAddress, XEMACPS_NWCTRL_OFFSET);
            reg |= XEMACPS_NWCTRL_RXEN_MASK;
            XEmacPs_WriteReg(sprt_xemacif->sgrt_emacps.Config.BaseAddress, XEMACPS_NWCTRL_OFFSET, reg);
        }

        sprt_xemacif->last_rx_frms_cntr = tempcntr;
    }

    /*!< deal with rx buffer */
    sprt_rxring = &XEmacPs_GetRxRing(&sprt_xemacif->sgrt_emacps);

    /*!< work-BD ---> post-BD */
    while ((n_bds = XEmacPs_BdRingFromHwRx(sprt_rxring, 
                        XNET_RX_DESC_NUM, &sprt_rxbd))) {
        for (count = 0, sprt_curbd = sprt_rxbd; count < n_bds; count++) {
            bdindex = XEMACPS_BD_TO_INDEX(sprt_rxring, sprt_curbd);
            sprt_skb = sprt_data->sprt_rxskbs[bdindex];

            /*!< Adjust the buffer size to the actual number of bytes received. */
            sprt_skb->len = XEmacPs_BdGetLength(sprt_curbd);
            sprt_skb->tail = sprt_skb->data + sprt_skb->len;
            sprt_skb->protocol = fwk_eth_type_trans(sprt_skb, sprt_ndev);
            sprt_skb->sprt_ndev = sprt_ndev;

            sprt_ndev->sgrt_stats.rx_packets++;
            sprt_ndev->sgrt_stats.rx_bytes += sprt_skb->len;

            fwk_netif_rx(sprt_skb);
            sprt_data->sprt_rxskbs[bdindex] = mrt_nullptr;

            sprt_curbd = XEmacPs_BdRingNext(sprt_rxring, sprt_curbd);
        }

        /*!< free up the BD's: post-BD ---> free-BD */
        XEmacPs_BdRingFree(sprt_rxring, n_bds, sprt_rxbd);
        xsdk_gem_setup_rxbd(sprt_data, GFP_ATOMIC);
    }
}

static void xsdk_gem_error_handler(struct xsdk_gem_drv_data *sprt_data)
{
    xemacpsif_s *sprt_xemacif = &sprt_data->sgrt_xemacif;
    kuint32_t reg;
    kint32_t retval;

    xsdk_gem_free_txbuf_any(sprt_data);
    xsdk_gem_free_rxbuf_any(sprt_data);

    retval = XEmacPs_CfgInitialize(&sprt_xemacif->sgrt_emacps, 
                        &sprt_data->sgrt_config, sprt_data->sgrt_config.BaseAddress);
    if (retval)
        print_err("%s: EmacPs Configuration Failed....\r\n", __func__);

    /*!< set mac address */
    retval = XEmacPs_SetMacAddress(&sprt_xemacif->sgrt_emacps, sprt_data->hwaddr, 1);
    if (retval)
        print_err("%s: Emac Mac Address set failed...\r\n",__func__);

    XEmacPs_SetOperatingSpeed(&sprt_xemacif->sgrt_emacps, sprt_data->sgrt_phy.link_speed);

    /*!< Setting the operating speed of the MAC needs a delay */
//  sleep(2);

    reg = XEmacPs_ReadReg(sprt_xemacif->sgrt_emacps.Config.BaseAddress, XEMACPS_DMACR_OFFSET);
    reg |= 0x01000000;
    XEmacPs_WriteReg(sprt_xemacif->sgrt_emacps.Config.BaseAddress, XEMACPS_DMACR_OFFSET, reg);

    xsdk_gem_dma_init(sprt_data);
    XEmacPs_Start(&sprt_xemacif->sgrt_emacps);
}

static void xsdk_gem_send_error(struct xsdk_gem_drv_data *sprt_data)
{
    xemacpsif_s *sprt_xemacif = &sprt_data->sgrt_xemacif;
    kuint32_t reg;

    reg = XEmacPs_ReadReg(sprt_xemacif->sgrt_emacps.Config.BaseAddress, XEMACPS_NWCTRL_OFFSET);
    reg &= ~XEMACPS_NWCTRL_TXEN_MASK;
    XEmacPs_WriteReg(sprt_xemacif->sgrt_emacps.Config.BaseAddress, XEMACPS_NWCTRL_OFFSET, reg);

    xsdk_gem_free_txbuf_any(sprt_data);
    xsdk_gem_dma_tx_descs_clean(sprt_data);

    reg = XEmacPs_ReadReg(sprt_xemacif->sgrt_emacps.Config.BaseAddress, XEMACPS_NWCTRL_OFFSET);
    reg |= XEMACPS_NWCTRL_TXEN_MASK;
    XEmacPs_WriteReg(sprt_xemacif->sgrt_emacps.Config.BaseAddress, XEMACPS_NWCTRL_OFFSET, reg);
}

static void xsdk_gem_tx_error_handler(struct xsdk_gem_drv_data *sprt_data, kuint32_t regVal)
{
    if (!regVal)
        return;

    if (regVal & XEMACPS_TXSR_HRESPNOK_MASK) {
        print_debug("Transmit DMA error\r\n");
        xsdk_gem_error_handler(sprt_data);
    }

    if (regVal & XEMACPS_TXSR_URUN_MASK) {
        print_debug("Transmit under run\r\n");
        xsdk_gem_send_error(sprt_data);
    }

    if (regVal & XEMACPS_TXSR_BUFEXH_MASK) {
        print_debug("Transmit buffer exhausted\r\n");
        xsdk_gem_send_error(sprt_data);
    }

    if (regVal & XEMACPS_TXSR_RXOVR_MASK) {
        print_debug("Transmit retry excessed limits\r\n");
        xsdk_gem_send_error(sprt_data);
    }

    if (regVal & XEMACPS_TXSR_FRAMERX_MASK) {
        print_debug("Transmit collision\r\n");
        xsdk_gem_sent_complete(sprt_data);
    }
}

static void xsdk_gem_rx_error_handler(struct xsdk_gem_drv_data *sprt_data, kuint32_t regVal)
{
    if (!regVal)
        return;

    if (regVal & XEMACPS_RXSR_HRESPNOK_MASK) {
        print_debug("Receive DMA error\r\n");
        xsdk_gem_error_handler(sprt_data);
    }

    if (regVal & XEMACPS_RXSR_RXOVR_MASK) {
        print_debug("Receive over run\r\n");
        xsdk_gem_recv_handler(sprt_data);
        xsdk_gem_setup_rxbd(sprt_data, GFP_ATOMIC);
    } 

    if (regVal & XEMACPS_RXSR_BUFFNA_MASK) {
        print_debug("Receive buffer not available\r\n");
        xsdk_gem_recv_handler(sprt_data);
        xsdk_gem_setup_rxbd(sprt_data, GFP_ATOMIC);
    }
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
    sprt_xemacif->sprt_rxq = mrt_nullptr;

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

    xsdk_gem_emac_init(sprt_data);
    xsdk_gem_dma_init(sprt_data);

    return ER_NORMAL;
}

static void xsdk_gem_ndo_uninit(struct fwk_net_device *sprt_ndev)
{

}

static kint32_t xsdk_gem_ndo_open(struct fwk_net_device *sprt_ndev)
{
    struct xsdk_gem_drv_data *sprt_data;
    xemacpsif_s *sprt_xemacif;

    sprt_data = (struct xsdk_gem_drv_data *)fwk_netdev_priv(sprt_ndev);
    sprt_xemacif = &sprt_data->sgrt_xemacif;

    if (XEmacPs_Start(&sprt_xemacif->sgrt_emacps))
        return -ER_FAILD;

    fwk_enable_irq(sprt_data->irq);
    return ER_NORMAL;
}

static kint32_t xsdk_gem_ndo_stop(struct fwk_net_device *sprt_ndev)
{
    struct xsdk_gem_drv_data *sprt_data;

    sprt_data = (struct xsdk_gem_drv_data *)fwk_netdev_priv(sprt_ndev);
    fwk_disable_irq(sprt_data->irq);

    return ER_NORMAL;
}

static netdev_tx_t xsdk_gem_ndo_start_xmit(struct fwk_sk_buff *sprt_skb, struct fwk_net_device *sprt_ndev)
{
    struct xsdk_gem_drv_data *sprt_data;
    netdev_tx_t size = 0;

    sprt_data = (struct xsdk_gem_drv_data *)fwk_netdev_priv(sprt_ndev);

    /*!< prohibit sending (lock sk_buff) */
    fwk_netif_stop_queue(sprt_ndev);
    sprt_ndev->sgrt_stats.tx_packets++;
    sprt_ndev->sgrt_stats.tx_bytes += sprt_skb->len;

    /*!< the number of idle BDs is seriously insufficient */
    if (xsdk_gem_get_tx_freecnt(sprt_data) <= 5)
        xsdk_gem_sent_complete(sprt_data);

    if (xsdk_gem_get_tx_freecnt(sprt_data)) {
        size = sprt_skb->len;
        xsdk_gem_send_bd(sprt_data, sprt_skb);
    }

    fwk_free_skb(sprt_skb);
    fwk_netif_wake_queue(sprt_ndev);

    return size;
}

static const struct fwk_netdev_ops sgrt_xsdk_gem_drv_oprts =
{
    .ndo_init = xsdk_gem_ndo_init,
    .ndo_uninit = xsdk_gem_ndo_uninit,
    .ndo_open = xsdk_gem_ndo_open,
    .ndo_stop = xsdk_gem_ndo_stop,
    .ndo_start_xmit = xsdk_gem_ndo_start_xmit,
};

static void xsdk_gem_driver_setup(struct fwk_net_device *sprt_ndev)
{
    struct xsdk_gem_drv_data *sprt_data;

    sprt_data = (struct xsdk_gem_drv_data *)fwk_netdev_priv(sprt_ndev);
    sprt_data->sprt_ndev = sprt_ndev;

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
    struct xsdk_gem_drv_data *sprt_data;
    XEmacPs *sprt_emacps;
    kuint32_t RegISR;
    kuint32_t RegSR;
    kuint32_t RegCtrl;
    kuint32_t RegQ1ISR = 0U;

    sprt_data = (struct xsdk_gem_drv_data *)args;
    sprt_emacps = &sprt_data->sgrt_xemacif.sgrt_emacps;

    /*!< 
     * This ISR will try to handle as many interrupts as it can in a single
     * call. However, in most of the places where the user's error handler
     * is called, this ISR exits because it is expected that the user will
     * reset the device in nearly all instances.
     */
    RegISR = XEmacPs_ReadReg(sprt_emacps->Config.BaseAddress, XEMACPS_ISR_OFFSET);

    /*!< Read Transmit Q1 ISR */
    if (sprt_emacps->Version > 2)
        RegQ1ISR = XEmacPs_ReadReg(sprt_emacps->Config.BaseAddress, XEMACPS_INTQ1_STS_OFFSET);

    /*!< ---------------------------------------------------------------------------- */
    /*!< Receive complete interrupt */
    if (RegISR & XEMACPS_IXR_FRAMERX_MASK) {
        /*!< 
         * Clear RX status register RX complete indication but preserve
         * error bits if there is any 
         */
        XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, XEMACPS_RXSR_OFFSET,
                   ((kuint32_t)XEMACPS_RXSR_FRAMERX_MASK | (kuint32_t)XEMACPS_RXSR_BUFFNA_MASK));
        xsdk_gem_recv_handler(sprt_data);
    }

    /*!< Receive error conditions interrupt */
    if (RegISR & XEMACPS_IXR_RX_ERR_MASK) {

        /*!< Clear RX status register */
        RegSR = XEmacPs_ReadReg(sprt_emacps->Config.BaseAddress, XEMACPS_RXSR_OFFSET);
        XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, XEMACPS_RXSR_OFFSET, RegSR);

        /*!< 
         * Fix for CR # 692702. Write to bit 18 of net_ctrl
         * register to flush a packet out of Rx SRAM upon
         * an error for receive buffer not available. 
         */
        if ((RegISR & XEMACPS_IXR_RXUSED_MASK) != 0x00000000U) {
            RegCtrl = XEmacPs_ReadReg(sprt_emacps->Config.BaseAddress, XEMACPS_NWCTRL_OFFSET);
            RegCtrl |= (kuint32_t)XEMACPS_NWCTRL_FLUSH_DPRAM_MASK;
            XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, XEMACPS_NWCTRL_OFFSET, RegCtrl);
        }

        if (RegSR)
            xsdk_gem_rx_error_handler(sprt_data, RegSR);
    }

    /*!< ---------------------------------------------------------------------------- */
    /*!< Transmit Q1 complete interrupt */
    if (sprt_emacps->Version > 2) {
        if (RegQ1ISR & XEMACPS_INTQ1SR_TXCOMPL_MASK) {

            /*!< Clear TX status register TX complete indication but preserve
            * error bits if there is any */
            XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, XEMACPS_INTQ1_STS_OFFSET, XEMACPS_INTQ1SR_TXCOMPL_MASK);
            XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, XEMACPS_TXSR_OFFSET,
                    ((kuint32_t)XEMACPS_TXSR_TXCOMPL_MASK | (kuint32_t)XEMACPS_TXSR_USEDREAD_MASK));
            
            xsdk_gem_send_handler(sprt_data);

            /*!< Transmit Q1 error conditions interrupt */
            if (RegQ1ISR & XEMACPS_INTQ1SR_TXERR_MASK) {

                /*!< Clear Interrupt Q1 status register */
                XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, XEMACPS_INTQ1_STS_OFFSET, RegQ1ISR);
                xsdk_gem_tx_error_handler(sprt_data, RegQ1ISR);
            }
        }
    }

    /*!< Transmit complete interrupt */
    if (RegISR & XEMACPS_IXR_TXCOMPL_MASK) {
        /*!< 
         * Clear TX status register TX complete indication but preserve
         * error bits if there is any 
         */
        XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, XEMACPS_TXSR_OFFSET,
                   ((kuint32_t)XEMACPS_TXSR_TXCOMPL_MASK | (kuint32_t)XEMACPS_TXSR_USEDREAD_MASK));
        
        xsdk_gem_send_handler(sprt_data);
    }
    else if (RegISR & XEMACPS_IXR_TX_ERR_MASK) {
        /*!< Clear TX status register */
        RegSR = XEmacPs_ReadReg(sprt_emacps->Config.BaseAddress, XEMACPS_TXSR_OFFSET);
        XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, XEMACPS_TXSR_OFFSET, RegSR);

        xsdk_gem_tx_error_handler(sprt_data, RegSR);
    }

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
    retval = fwk_request_irq(sprt_data->irq, xsdk_gem_driver_isr, 0, XSDK_GEM_DRIVER_NAME, sprt_data);
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
