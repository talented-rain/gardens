/*
 * ZYNQ eMAC Of PS Interface
 *
 * File Name:   zynq7_emacps.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.11.24
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <common/generic.h>
#include <common/queue.h>

#include <boot/board_init.h>
#include <asm/armv7/gcc_config.h>

#include <zynq7/zynq7_periph.h>
#include <zynq7/xemac/xemacpsif.h>
#include <zynq7/xemac/xemacps.h>
#include <zynq7/xemac/xemac_ieee_reg.h>

#include <lwip/port/lwipopts.h>
#include <lwip/netif.h>
#include <lwip/ip.h>

/*!< The defines */
struct XEthernetPs
{
    kuaddr_t Mac_BaseAddr;
    struct xemac_s *xemac;
    xemacpsif_s *xemacpsif;
    XEmacPs_Config *mac_config;
};

#define XNET_CONFIG_INCLUDE_GEM                 1
#define XNET_CONFIG_EMAC_NUMBER                 0
#define XNET_CONFIG_N_TX_DESC                   512
#define XNET_CONFIG_N_RX_DESC                   512

#define CONFIG_LINKSPEED_AUTODETECT	            1

#define PHY_DETECT_REG                          1
#define PHY_IDENTIFIER_1_REG                    2
#define PHY_IDENTIFIER_2_REG                    3
#define PHY_DETECT_MASK                         0x1808
#define PHY_MARVELL_IDENTIFIER                  0x0141
#define PHY_TI_IDENTIFIER                       0x2000
#define PHY_REALTEK_IDENTIFIER                  0x001c
#define PHY_XILINX_PCS_PMA_ID1                  0x0174
#define PHY_XILINX_PCS_PMA_ID2                  0x0C00

#define XEMACPS_GMII2RGMII_SPEED1000_FD         0x140
#define XEMACPS_GMII2RGMII_SPEED100_FD          0x2100
#define XEMACPS_GMII2RGMII_SPEED10_FD           0x100
#define XEMACPS_GMII2RGMII_REG_NUM              0x10

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

/*!< Frequency setting */
#define SLCR_LOCK_ADDR                          (XPS_SYS_CTRL_BASEADDR + 0x4)
#define SLCR_UNLOCK_ADDR                        (XPS_SYS_CTRL_BASEADDR + 0x8)
#define SLCR_GEM0_CLK_CTRL_ADDR                 (XPS_SYS_CTRL_BASEADDR + 0x140)
#define SLCR_GEM1_CLK_CTRL_ADDR                 (XPS_SYS_CTRL_BASEADDR + 0x144)
#define SLCR_GEM_SRCSEL_EMIO                    0x40
#define SLCR_LOCK_KEY_VALUE                     0x767B
#define SLCR_UNLOCK_KEY_VALUE                   0xDF0D
#define SLCR_ADDR_GEM_RST_CTRL                  (XPS_SYS_CTRL_BASEADDR + 0x214)
#define EMACPS_SLCR_DIV_MASK                    0xFC0FC0FF

struct xtopology_t 
{
    kuaddr_t emac_baseaddr;
    kuaddr_t intc_baseaddr;
    kuaddr_t intc_emac_intr;	                /*!< valid only for xemac_type_xps_emaclite */
    kuaddr_t scugic_baseaddr;                   /*!< valid only for Zynq */
    kuaddr_t scugic_emac_intr;                  /*!< valid only for GEM */
};

enum ethernet_link_status {
    ETH_LINK_UNDEFINED = 0,
    ETH_LINK_UP,
    ETH_LINK_DOWN,
    ETH_LINK_NEGOTIATING
};

/*!< The globals */
static struct xtopology_t xtopology[] = {
    {
        .emac_baseaddr = 0xE000B000,
        .intc_baseaddr = 0x0,
        .intc_emac_intr = 0x0,
        .scugic_baseaddr = 0xF8F00100,
        .scugic_emac_intr = 0x36,
    },
};
const kint32_t xtopology_n_emacs = ARRAY_SIZE(xtopology);

static XEmacPs_Config XEmacPs_ConfigTable[XPAR_XEMACPS_NUM_INSTANCES] =
{
    {
        XPAR_PS7_ETHERNET_0_DEVICE_ID,
        XPAR_PS7_ETHERNET_0_BASEADDR,
        XPAR_PS7_ETHERNET_0_IS_CACHE_COHERENT
    }
};
#define XEMACPS_IS_ETH_0(x)                  ((x)->Config.BaseAddress == XPAR_PS7_ETHERNET_0_BASEADDR)

static kint32_t link_speed = 100;
static kuint32_t phymapemac0[32];
static kuint32_t phymapemac1[32];
static kuint32_t phyaddrforemac;
enum ethernet_link_status eth_link_status = ETH_LINK_UNDEFINED;

static kuint32_t tx_pbufs_storage[4 * XNET_CONFIG_N_TX_DESC];
static kuint32_t rx_pbufs_storage[4 * XNET_CONFIG_N_RX_DESC];

static volatile kuint32_t bd_space_index = 0;
static volatile kuint32_t bd_space_attr_set = 0;

static struct netif *sprt_zynq7_netif;
static XEmacPs_Config *sprt_zynq7_machconfig;

/*!< The functions */

/*!< API functions */
void XEmacPs_Start(XEmacPs *sprt_emacps)
{
    kuint32_t Reg;

    /*!< Assert bad arguments and conditions */
    if ((!sprt_emacps) ||
        (!sprt_emacps->IsReady))
        return;

    /*!< Start DMA */
    /*!< 
     * When starting the DMA channels, both transmit and receive sides
     * need an initialized BD list.
     */
    if (sprt_emacps->Version == 2) 
    {
        if ((!sprt_emacps->RxBdRing.BaseBdAddr) ||
            (!sprt_emacps->TxBdRing.BaseBdAddr))
            return;

        XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, XEMACPS_RXQBASE_OFFSET,
                        sprt_emacps->RxBdRing.BaseBdAddr);

        XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, XEMACPS_TXQBASE_OFFSET,
                        sprt_emacps->TxBdRing.BaseBdAddr);
    }

    /*!< clear any existed int status */
    XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, XEMACPS_ISR_OFFSET, XEMACPS_IXR_ALL_MASK);

    /*!< Enable transmitter if not already enabled */
    if ((sprt_emacps->Options & (kuint32_t)XEMACPS_TRANSMITTER_ENABLE_OPTION)!=0x00000000U) 
    {
        Reg = XEmacPs_ReadReg(sprt_emacps->Config.BaseAddress, XEMACPS_NWCTRL_OFFSET);
        if (!(Reg & XEMACPS_NWCTRL_TXEN_MASK))
        {
            XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress,
                        XEMACPS_NWCTRL_OFFSET,
                        Reg | (kuint32_t)XEMACPS_NWCTRL_TXEN_MASK);
        }
    }

    /*!< Enable receiver if not already enabled */
    if ((sprt_emacps->Options & XEMACPS_RECEIVER_ENABLE_OPTION) != 0x00000000U) 
    {
        Reg = XEmacPs_ReadReg(sprt_emacps->Config.BaseAddress, XEMACPS_NWCTRL_OFFSET);
        if (!(Reg & XEMACPS_NWCTRL_RXEN_MASK)) 
        {
            XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress,
                        XEMACPS_NWCTRL_OFFSET,
                        Reg | (kuint32_t)XEMACPS_NWCTRL_RXEN_MASK);
        }
    }

    /*!< Enable TX and RX interrupts */
    XEmacPs_IntEnable(sprt_emacps, 
            (XEMACPS_IXR_TX_ERR_MASK | XEMACPS_IXR_RX_ERR_MASK | (kuint32_t)XEMACPS_IXR_FRAMERX_MASK | (kuint32_t)XEMACPS_IXR_TXCOMPL_MASK));

    /*!< Enable TX Q1 Interrupts */
    if (sprt_emacps->Version > 2)
        XEmacPs_IntQ1Enable(sprt_emacps, XEMACPS_INTQ1_IXR_ALL_MASK);

    /*!< Mark as started */
    sprt_emacps->IsStarted = true;
}

void XEmacPs_Stop(XEmacPs *sprt_emacps)
{
    kuint32_t Reg;

    if ((!sprt_emacps) ||
        (!sprt_emacps->IsReady))
        return;

    /*!< Disable all interrupts */
    XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, 
                        XEMACPS_IDR_OFFSET, XEMACPS_IXR_ALL_MASK);

    /*!< Disable the receiver & transmitter */
    Reg = XEmacPs_ReadReg(sprt_emacps->Config.BaseAddress, XEMACPS_NWCTRL_OFFSET);
    Reg &= (kuint32_t)(~XEMACPS_NWCTRL_RXEN_MASK);
    Reg &= (kuint32_t)(~XEMACPS_NWCTRL_TXEN_MASK);
    XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, XEMACPS_NWCTRL_OFFSET, Reg);

    /*!< Mark as stopped */
    sprt_emacps->IsStarted = false;
}

void XEmacPs_SetQueuePtr(XEmacPs *sprt_emacps, kuint32_t QPtr, kuint8_t QueueNum, kuint16_t Direction)
{
    /*!< Assert bad arguments and conditions */
    if ((!sprt_emacps) ||
        (!sprt_emacps->IsReady))
        return;

    /*!< If already started, then there is nothing to do */
    if (sprt_emacps->IsStarted)
         return;

    if (QueueNum == 0x00U) 
    {
        if (Direction == XEMACPS_SEND)
            XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, 
                            XEMACPS_TXQBASE_OFFSET, (QPtr & ULONG64_LO_MASK));
        else
            XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, 
                            XEMACPS_RXQBASE_OFFSET, (QPtr & ULONG64_LO_MASK));
    }
    else
        XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, 
                            XEMACPS_TXQ1BASE_OFFSET, (QPtr & ULONG64_LO_MASK));
}

void XEmacPs_ClearHash(XEmacPs *sprt_emacps)
{
    if ((!sprt_emacps) ||
        (!sprt_emacps->IsReady))
        return;

    XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress,
                    XEMACPS_HASHL_OFFSET, 0x0U);

    /*!< write bits [63:32] in TOP */
    XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress,
                    XEMACPS_HASHH_OFFSET, 0x0U);
}

kint32_t XEmacPs_SetMacAddress(XEmacPs *sprt_emacps, void *AddressPtr, kuint8_t Index)
{
    kuint32_t MacAddr;
    kuint8_t *Aptr = (kuint8_t *)AddressPtr;
    kuint8_t IndexLoc = Index;

    if ((!sprt_emacps) ||
        (!Aptr) ||
        (!sprt_emacps->IsReady) ||
        ((IndexLoc <= (kuint8_t)XEMACPS_MAX_MAC_ADDR) && (IndexLoc > 0x00U)))
        return -ER_NREADY;

    /*!< Be sure device has been stopped */
    if (sprt_emacps->IsStarted)
        return -ER_LOCKED;

    /*!< Index ranges 1 to 4, for offset calculation is 0 to 3. */
    IndexLoc--;

    /*!< Set the MAC bits [31:0] in BOT */
    MacAddr = *(Aptr);
    MacAddr |= ((kuint32_t)(*(Aptr + 1)) << 8U);
    MacAddr |= ((kuint32_t)(*(Aptr + 2)) << 16U);
    MacAddr |= ((kuint32_t)(*(Aptr + 3)) << 24U);
    XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress,
                ((kuint32_t)XEMACPS_LADDR1L_OFFSET + ((kuint32_t)IndexLoc * (kuint32_t)8)), MacAddr);

    /*!< There are reserved bits in TOP so don't affect them */
    MacAddr = XEmacPs_ReadReg(sprt_emacps->Config.BaseAddress,
                    ((kuint32_t)XEMACPS_LADDR1H_OFFSET + ((kuint32_t)IndexLoc * (kuint32_t)8)));

    MacAddr &= (kuint32_t)(~XEMACPS_LADDR_MACH_MASK);

    /*!< Set MAC bits [47:32] in TOP */
    MacAddr |= (kuint32_t)(*(Aptr + 4));
    MacAddr |= (kuint32_t)(*(Aptr + 5)) << 8U;

    XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress,
                ((kuint32_t)XEMACPS_LADDR1H_OFFSET + ((kuint32_t)IndexLoc * (kuint32_t)8)), MacAddr);

    return ER_NORMAL;
}

kint32_t XEmacPs_SetTypeIdCheck(XEmacPs *sprt_emacps, kuint32_t Id_Check, kuint8_t Index)
{
    kuint8_t IndexLoc = Index;

    if ((!sprt_emacps) ||
        (!sprt_emacps->IsReady) ||
        ((IndexLoc <= (kuint8_t)XEMACPS_MAX_TYPE_ID) && (IndexLoc > 0x00U)))
        return -ER_NREADY;

    /*!< Be sure device has been stopped */
    if (sprt_emacps->IsStarted)
        return -ER_LOCKED;

    /*!< Index ranges 1 to 4, for offset calculation is 0 to 3. */
    IndexLoc--;

    /*!< Set the ID bits in MATCHx register */
    XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress,
                ((kuint32_t)XEMACPS_MATCH1_OFFSET + ((kuint32_t)IndexLoc * (kuint32_t)4)), Id_Check);

    return ER_NORMAL;
}

kint32_t XEmacPs_SetOptions(XEmacPs *sprt_emacps, kuint32_t Options)
{
    kuint32_t Reg;		                                /*!< Generic register contents */
    kuint32_t RegNetCfg;		                            /*!< Reflects original contents of NET_CONFIG */
    kuint32_t RegNewNetCfg;	                            /*!< Reflects new contents of NET_CONFIG */

    if ((!sprt_emacps) ||
        (!sprt_emacps->IsReady))
        return -ER_NREADY;

    /*!< Be sure device has been stopped */
    if (sprt_emacps->IsStarted)
        return -ER_LOCKED;

    /*!< 
     * Many of these options will change the NET_CONFIG registers.
     * To reduce the amount of IO to the device, group these options here
     * and change them all at once.
     */

    /*!< Grab current register contents */
    RegNetCfg = XEmacPs_ReadReg(sprt_emacps->Config.BaseAddress, XEMACPS_NWCFG_OFFSET);
    RegNewNetCfg = RegNetCfg;

    /*!< It is configured to max 1536. */
    if ((Options & XEMACPS_FRAME1536_OPTION) != 0x00000000U)
        RegNewNetCfg |= (XEMACPS_NWCFG_1536RXEN_MASK);

    /*!< Turn on VLAN packet only, only VLAN tagged will be accepted */
    if ((Options & XEMACPS_VLAN_OPTION) != 0x00000000U)
        RegNewNetCfg |= XEMACPS_NWCFG_NVLANDISC_MASK;

    /*!< Turn on FCS stripping on receive packets */
    if ((Options & XEMACPS_FCS_STRIP_OPTION) != 0x00000000U)
        RegNewNetCfg |= XEMACPS_NWCFG_FCSREM_MASK;

    /*!< Turn on length/type field checking on receive packets */
    if ((Options & XEMACPS_LENTYPE_ERR_OPTION) != 0x00000000U)
        RegNewNetCfg |= XEMACPS_NWCFG_LENERRDSCRD_MASK;

    /*!< Turn on flow control */
    if ((Options & XEMACPS_FLOW_CONTROL_OPTION) != 0x00000000U)
        RegNewNetCfg |= XEMACPS_NWCFG_PAUSEEN_MASK;

    /*!< Turn on promiscuous frame filtering (all frames are received) */
    if ((Options & XEMACPS_PROMISC_OPTION) != 0x00000000U)
        RegNewNetCfg |= XEMACPS_NWCFG_COPYALLEN_MASK;

    /*!< Allow broadcast address reception */
    if ((Options & XEMACPS_BROADCAST_OPTION) != 0x00000000U)
        RegNewNetCfg &= (kuint32_t)(~XEMACPS_NWCFG_BCASTDI_MASK);

    /*!< Allow multicast address filtering */
    if ((Options & XEMACPS_MULTICAST_OPTION) != 0x00000000U)
        RegNewNetCfg |= XEMACPS_NWCFG_MCASTHASHEN_MASK;

    /*!< enable RX checksum offload */
    if ((Options & XEMACPS_RX_CHKSUM_ENABLE_OPTION) != 0x00000000U)
        RegNewNetCfg |= XEMACPS_NWCFG_RXCHKSUMEN_MASK;

    /*!< Enable jumbo frames */
    if (((Options & XEMACPS_JUMBO_ENABLE_OPTION) != 0x00000000U) &&
        (sprt_emacps->Version > 2)) 
    {
        RegNewNetCfg |= XEMACPS_NWCFG_JUMBO_MASK;
        XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress,
                XEMACPS_JUMBOMAXLEN_OFFSET, XEMACPS_RX_BUF_SIZE_JUMBO);
        
        Reg = XEmacPs_ReadReg(sprt_emacps->Config.BaseAddress, XEMACPS_DMACR_OFFSET);
        Reg &= ~XEMACPS_DMACR_RXBUF_MASK;
        Reg |= (((((kuint32_t)XEMACPS_RX_BUF_SIZE_JUMBO / (kuint32_t)XEMACPS_RX_BUF_UNIT) +
                (((((kuint32_t)XEMACPS_RX_BUF_SIZE_JUMBO %
                (kuint32_t)XEMACPS_RX_BUF_UNIT))!=(kuint32_t)0) ? 1U : 0U)) <<
                (kuint32_t)(XEMACPS_DMACR_RXBUF_SHIFT)) &
                (kuint32_t)(XEMACPS_DMACR_RXBUF_MASK));

        XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, XEMACPS_DMACR_OFFSET, Reg);

        sprt_emacps->MaxMtuSize = XEMACPS_MTU_JUMBO;
        sprt_emacps->MaxFrameSize = XEMACPS_MTU_JUMBO + XEMACPS_HDR_SIZE + XEMACPS_TRL_SIZE;
        sprt_emacps->MaxVlanFrameSize = sprt_emacps->MaxFrameSize + XEMACPS_HDR_VLAN_SIZE;
        sprt_emacps->RxBufMask = XEMACPS_RXBUF_LEN_JUMBO_MASK;
    }

    if (((Options & XEMACPS_SGMII_ENABLE_OPTION) != 0x00000000U) &&
        (sprt_emacps->Version > 2)) 
        RegNewNetCfg |= (XEMACPS_NWCFG_SGMIIEN_MASK | XEMACPS_NWCFG_PCSSEL_MASK);

    /*!< Officially change the NET_CONFIG registers if it needs to be modified. */
    if (RegNetCfg != RegNewNetCfg)
        XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, XEMACPS_NWCFG_OFFSET, RegNewNetCfg);

    /*!< Enable TX checksum offload */
    if ((Options & XEMACPS_TX_CHKSUM_ENABLE_OPTION) != 0x00000000U) 
    {
        Reg = XEmacPs_ReadReg(sprt_emacps->Config.BaseAddress, XEMACPS_DMACR_OFFSET);
        Reg |= XEMACPS_DMACR_TCPCKSUM_MASK;
        XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, XEMACPS_DMACR_OFFSET, Reg);
    }

    /*!< Enable transmitter */
    if ((Options & XEMACPS_TRANSMITTER_ENABLE_OPTION) != 0x00000000U) 
    {
        Reg = XEmacPs_ReadReg(sprt_emacps->Config.BaseAddress, XEMACPS_NWCTRL_OFFSET);
        Reg |= XEMACPS_NWCTRL_TXEN_MASK;
        XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, XEMACPS_NWCTRL_OFFSET, Reg);
    }

    /*!< Enable receiver */
    if ((Options & XEMACPS_RECEIVER_ENABLE_OPTION) != 0x00000000U) 
    {
        Reg = XEmacPs_ReadReg(sprt_emacps->Config.BaseAddress, XEMACPS_NWCTRL_OFFSET);
        Reg |= XEMACPS_NWCTRL_RXEN_MASK;
        XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, XEMACPS_NWCTRL_OFFSET, Reg);
    }

    /*!< 
     * The remaining options not handled here are managed elsewhere in the
     * driver. No register modifications are needed at this time. Reflecting
     * the option in sprt_emacps->Options is good enough for now.
     */

    /*!< Set options word to its new value */
    sprt_emacps->Options |= Options;

    return ER_NORMAL;
}

kint32_t XEmacPs_ClearOptions(XEmacPs *sprt_emacps, kuint32_t Options)
{
    kuint32_t Reg;		                                /*!< Generic */
    kuint32_t RegNetCfg;		                            /*!< Reflects original contents of NET_CONFIG */
    kuint32_t RegNewNetCfg;	                            /*!< Reflects new contents of NET_CONFIG */

    if ((!sprt_emacps) ||
        (!sprt_emacps->IsReady))
        return -ER_NREADY;

    /*!< Be sure device has been stopped */
    if (sprt_emacps->IsStarted)
        return -ER_LOCKED;

    /*!< 
     * Many of these options will change the NET_CONFIG registers.
     * To reduce the amount of IO to the device, group these options here
     * and change them all at once.
     */

    /*!< Grab current register contents */
    RegNetCfg = XEmacPs_ReadReg(sprt_emacps->Config.BaseAddress,
                      XEMACPS_NWCFG_OFFSET);
    RegNewNetCfg = RegNetCfg;

    /*!< 
     * There is only RX configuration!?
     * It is configured in two different length, upto 1536 and 10240 bytes
     */
    if ((Options & XEMACPS_FRAME1536_OPTION) != 0x00000000U)
        RegNewNetCfg &= (kuint32_t)(~XEMACPS_NWCFG_1536RXEN_MASK);

    /*!< Turn off VLAN packet only */
    if ((Options & XEMACPS_VLAN_OPTION) != 0x00000000U)
        RegNewNetCfg &= (kuint32_t)(~XEMACPS_NWCFG_NVLANDISC_MASK);

    /*!< Turn off FCS stripping on receive packets */
    if ((Options & XEMACPS_FCS_STRIP_OPTION) != 0x00000000U)
        RegNewNetCfg &= (kuint32_t)(~XEMACPS_NWCFG_FCSREM_MASK);

    /*!< Turn off length/type field checking on receive packets */
    if ((Options & XEMACPS_LENTYPE_ERR_OPTION) != 0x00000000U)
        RegNewNetCfg &= (kuint32_t)(~XEMACPS_NWCFG_LENERRDSCRD_MASK);

    /*!< Turn off flow control */
    if ((Options & XEMACPS_FLOW_CONTROL_OPTION) != 0x00000000U)
        RegNewNetCfg &= (kuint32_t)(~XEMACPS_NWCFG_PAUSEEN_MASK);

    /*!< Turn off promiscuous frame filtering (all frames are received) */
    if ((Options & XEMACPS_PROMISC_OPTION) != 0x00000000U)
        RegNewNetCfg &= (kuint32_t)(~XEMACPS_NWCFG_COPYALLEN_MASK);

    /*!< Disallow broadcast address filtering => broadcast reception */
    if ((Options & XEMACPS_BROADCAST_OPTION) != 0x00000000U)
        RegNewNetCfg |= XEMACPS_NWCFG_BCASTDI_MASK;

    /*!< Disallow multicast address filtering */
    if ((Options & XEMACPS_MULTICAST_OPTION) != 0x00000000U)
        RegNewNetCfg &= (kuint32_t)(~XEMACPS_NWCFG_MCASTHASHEN_MASK);

    /*!< Disable RX checksum offload */
    if ((Options & XEMACPS_RX_CHKSUM_ENABLE_OPTION) != 0x00000000U)
        RegNewNetCfg &= (kuint32_t)(~XEMACPS_NWCFG_RXCHKSUMEN_MASK);

    /*!< Disable jumbo frames */
    if (((Options & XEMACPS_JUMBO_ENABLE_OPTION) != 0x00000000U) &&
        (sprt_emacps->Version > 2))
    {
        RegNewNetCfg &= (kuint32_t)(~XEMACPS_NWCFG_JUMBO_MASK);
        Reg = XEmacPs_ReadReg(sprt_emacps->Config.BaseAddress, XEMACPS_DMACR_OFFSET);

        Reg &= ~XEMACPS_DMACR_RXBUF_MASK;
        Reg |= (((((kuint32_t)XEMACPS_RX_BUF_SIZE / (kuint32_t)XEMACPS_RX_BUF_UNIT) +
            (((((kuint32_t)XEMACPS_RX_BUF_SIZE %
            (kuint32_t)XEMACPS_RX_BUF_UNIT))!=(kuint32_t)0) ? 1U : 0U)) <<
            (kuint32_t)(XEMACPS_DMACR_RXBUF_SHIFT)) &
            (kuint32_t)(XEMACPS_DMACR_RXBUF_MASK));

        XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, XEMACPS_DMACR_OFFSET, Reg);

        sprt_emacps->MaxMtuSize = XEMACPS_MTU;
        sprt_emacps->MaxFrameSize = XEMACPS_MTU + XEMACPS_HDR_SIZE + XEMACPS_TRL_SIZE;
        sprt_emacps->MaxVlanFrameSize = sprt_emacps->MaxFrameSize + XEMACPS_HDR_VLAN_SIZE;
        sprt_emacps->RxBufMask = XEMACPS_RXBUF_LEN_MASK;
    }

    if (((Options & XEMACPS_SGMII_ENABLE_OPTION) != 0x00000000U) &&
        (sprt_emacps->Version > 2))
        RegNewNetCfg &= (kuint32_t)(~(XEMACPS_NWCFG_SGMIIEN_MASK | XEMACPS_NWCFG_PCSSEL_MASK));

    /*!< Officially change the NET_CONFIG registers if it needs to be modified. */
    if (RegNetCfg != RegNewNetCfg)
        XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress,
                   XEMACPS_NWCFG_OFFSET, RegNewNetCfg);

    /*!< Disable TX checksum offload */
    if ((Options & XEMACPS_TX_CHKSUM_ENABLE_OPTION) != 0x00000000U) 
    {
        Reg = XEmacPs_ReadReg(sprt_emacps->Config.BaseAddress, XEMACPS_DMACR_OFFSET);
        Reg &= (kuint32_t)(~XEMACPS_DMACR_TCPCKSUM_MASK);
        XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, XEMACPS_DMACR_OFFSET, Reg);
    }

    /*!< Disable transmitter */
    if ((Options & XEMACPS_TRANSMITTER_ENABLE_OPTION) != 0x00000000U) 
    {
        Reg = XEmacPs_ReadReg(sprt_emacps->Config.BaseAddress, XEMACPS_NWCTRL_OFFSET);
        Reg &= (kuint32_t)(~XEMACPS_NWCTRL_TXEN_MASK);
        XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, XEMACPS_NWCTRL_OFFSET, Reg);
    }

    /*!< Disable receiver */
    if ((Options & XEMACPS_RECEIVER_ENABLE_OPTION) != 0x00000000U) 
    {
        Reg = XEmacPs_ReadReg(sprt_emacps->Config.BaseAddress, XEMACPS_NWCTRL_OFFSET);
        Reg &= (kuint32_t)(~XEMACPS_NWCTRL_RXEN_MASK);
        XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, XEMACPS_NWCTRL_OFFSET, Reg);
    }

    /*!< 
     * The remaining options not handled here are managed elsewhere in the
     * driver. No register modifications are needed at this time. Reflecting
     * option in sprt_emacps->Options is good enough for now.
     */

    /*!< Set options word to its new value */
    sprt_emacps->Options &= ~Options;

    return ER_NORMAL;
}

void XEmacPs_Reset(XEmacPs *sprt_emacps)
{
    kuint32_t Reg;
    kuint8_t i;
    kint8_t EmacPs_zero_MAC[6] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };

    if ((!sprt_emacps) ||
        (!sprt_emacps->IsReady))
        return;

    /*!< Stop the device and reset hardware */
    XEmacPs_Stop(sprt_emacps);

    sprt_emacps->Options = XEMACPS_DEFAULT_OPTIONS;
    sprt_emacps->Version = XEmacPs_ReadReg(sprt_emacps->Config.BaseAddress, 0xFC);
    sprt_emacps->Version = (sprt_emacps->Version >> 16) & 0xFFF;
    sprt_emacps->MaxMtuSize = XEMACPS_MTU;
    sprt_emacps->MaxFrameSize = XEMACPS_MTU + XEMACPS_HDR_SIZE + XEMACPS_TRL_SIZE;
    sprt_emacps->MaxVlanFrameSize = sprt_emacps->MaxFrameSize + XEMACPS_HDR_VLAN_SIZE;
    sprt_emacps->RxBufMask = XEMACPS_RXBUF_LEN_MASK;

    /*!< Setup hardware with default values */
    XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress,
            XEMACPS_NWCTRL_OFFSET,
            (XEMACPS_NWCTRL_STATCLR_MASK |
            XEMACPS_NWCTRL_MDEN_MASK) &
            (kuint32_t)(~XEMACPS_NWCTRL_LOOPEN_MASK));

    Reg = XEmacPs_ReadReg(sprt_emacps->Config.BaseAddress, XEMACPS_NWCFG_OFFSET);
    Reg &= XEMACPS_NWCFG_MDCCLKDIV_MASK;
    Reg |= ((kuint32_t)XEMACPS_NWCFG_100_MASK | (kuint32_t)XEMACPS_NWCFG_FDEN_MASK | (kuint32_t)XEMACPS_NWCFG_UCASTHASHEN_MASK);

    XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, XEMACPS_NWCFG_OFFSET, Reg);
    if (sprt_emacps->Version > 2) 
    {
        XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, XEMACPS_NWCFG_OFFSET,
            (XEmacPs_ReadReg(sprt_emacps->Config.BaseAddress, XEMACPS_NWCFG_OFFSET) |
                XEMACPS_NWCFG_DWIDTH_64_MASK));
    }

    XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress,
            XEMACPS_DMACR_OFFSET,
            (((((kuint32_t)XEMACPS_RX_BUF_SIZE / (kuint32_t)XEMACPS_RX_BUF_UNIT) +
                (((((kuint32_t)XEMACPS_RX_BUF_SIZE %
                (kuint32_t)XEMACPS_RX_BUF_UNIT))!=(kuint32_t)0) ? 1U : 0U)) <<
                (kuint32_t)(XEMACPS_DMACR_RXBUF_SHIFT)) &
                (kuint32_t)(XEMACPS_DMACR_RXBUF_MASK)) |
                (kuint32_t)XEMACPS_DMACR_RXSIZE_MASK |
                (kuint32_t)XEMACPS_DMACR_TXSIZE_MASK);

    if (sprt_emacps->Version > 2) 
    {
        XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, XEMACPS_DMACR_OFFSET,
            (XEmacPs_ReadReg(sprt_emacps->Config.BaseAddress, XEMACPS_DMACR_OFFSET) |
            (kuint32_t)XEMACPS_DMACR_INCR16_AHB_BURST));
    }

    XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress,
               XEMACPS_TXSR_OFFSET, 0x0U);

    XEmacPs_SetQueuePtr(sprt_emacps, 0, 0x00U, (kuint16_t)XEMACPS_SEND);
    if (sprt_emacps->Version > 2)
        XEmacPs_SetQueuePtr(sprt_emacps, 0, 0x01U, (kuint16_t)XEMACPS_SEND);
    XEmacPs_SetQueuePtr(sprt_emacps, 0, 0x00U, (kuint16_t)XEMACPS_RECV);

    XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress,
               XEMACPS_RXSR_OFFSET, 0x0U);

    XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, XEMACPS_IDR_OFFSET,
               XEMACPS_IXR_ALL_MASK);

    Reg = XEmacPs_ReadReg(sprt_emacps->Config.BaseAddress, XEMACPS_ISR_OFFSET);
    XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, XEMACPS_ISR_OFFSET, Reg);

    XEmacPs_ClearHash(sprt_emacps);

    for (i = 1U; i < 5U; i++) 
    {
        (void)XEmacPs_SetMacAddress(sprt_emacps, EmacPs_zero_MAC, i);
        (void)XEmacPs_SetTypeIdCheck(sprt_emacps, 0x00000000U, i);
    }

    /*!< clear all counters */
    for (i = 0U; i < (kuint8_t)((XEMACPS_LAST_OFFSET - XEMACPS_OCTTXL_OFFSET) / 4U); i++)
        (void)XEmacPs_ReadReg(sprt_emacps->Config.BaseAddress,
                                   XEMACPS_OCTTXL_OFFSET + (kuint32_t)(((kuint32_t)i) * ((kuint32_t)4)));

    /*!< Disable the receiver */
    Reg = XEmacPs_ReadReg(sprt_emacps->Config.BaseAddress, XEMACPS_NWCTRL_OFFSET);
    Reg &= (kuint32_t)(~XEMACPS_NWCTRL_RXEN_MASK);
    XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, XEMACPS_NWCTRL_OFFSET, Reg);

    /*!< 
     * Sync default options with hardware but leave receiver and
     * transmitter disabled. They get enabled with XEmacPs_Start() if
     * XEMACPS_TRANSMITTER_ENABLE_OPTION and
     * XEMACPS_RECEIVER_ENABLE_OPTION are set.
     */
    (void)XEmacPs_SetOptions(sprt_emacps, sprt_emacps->Options &
                ~((kuint32_t)XEMACPS_TRANSMITTER_ENABLE_OPTION |
                  (kuint32_t)XEMACPS_RECEIVER_ENABLE_OPTION));

    (void)XEmacPs_ClearOptions(sprt_emacps, ~sprt_emacps->Options);
}

void XEmacPs_SetMdioDivisor(XEmacPs *sprt_emacps, XEmacPs_MdcDiv Divisor)
{
    kuint32_t Reg;

    if ((!sprt_emacps) ||
        (!sprt_emacps->IsReady) ||
        (Divisor <= (XEmacPs_MdcDiv)0x7))	/*!< only last three bits are valid */
        return;

    Reg = XEmacPs_ReadReg(sprt_emacps->Config.BaseAddress, XEMACPS_NWCFG_OFFSET);

    /*!< clear these three bits, could be done with mask */
    Reg &= (kuint32_t)(~XEMACPS_NWCFG_MDCCLKDIV_MASK);
    Reg |= ((kuint32_t)Divisor << XEMACPS_NWCFG_MDC_SHIFT_MASK);

    XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, XEMACPS_NWCFG_OFFSET, Reg);
}

XEmacPs_Config *XEmacPs_LookupConfig(kuint16_t DeviceId)
{
	XEmacPs_Config *CfgPtr = NULL;
	kuint32_t i;

	for (i = 0U; i < (kuint32_t)XPAR_XEMACPS_NUM_INSTANCES; i++) {
		if (XEmacPs_ConfigTable[i].DeviceId == DeviceId) {
			CfgPtr = &XEmacPs_ConfigTable[i];
			break;
		}
	}

	return (XEmacPs_Config *)(CfgPtr);
}

kint32_t XEmacPs_CfgInitialize(XEmacPs *sprt_emacps, XEmacPs_Config * CfgPtr, kuint32_t EffectiveAddress)
{
    /*!< Verify arguments */
    if ((!sprt_emacps) ||
        (!CfgPtr))
        return -ER_NULLPTR;

    /*!< Set device base address and ID */
    sprt_emacps->Config.DeviceId = CfgPtr->DeviceId;
    sprt_emacps->Config.BaseAddress = EffectiveAddress;
    sprt_emacps->Config.IsCacheCoherent = CfgPtr->IsCacheCoherent;

    /*!< Set callbacks to an initial stub routine */
    sprt_emacps->SendHandler = (XEmacPs_Handler)mrt_nullptr;
    sprt_emacps->RecvHandler = (XEmacPs_Handler)mrt_nullptr;
    sprt_emacps->ErrorHandler = (XEmacPs_ErrHandler)mrt_nullptr;

    /*!< Reset the hardware and set default options */
    sprt_emacps->IsReady = true;
    XEmacPs_Reset(sprt_emacps);

    return ER_NORMAL;
}

/*!< -------------------------------------------------------------------------------- */
/*!< -------------------------------------------------------------------------------- */
kint32_t XEmacPs_PhyRead(XEmacPs *sprt_emacps, kuint32_t PhyAddress, kuint32_t RegisterNum, kuint16_t *PhyDataPtr)
{
    kuint32_t Mgtcr;
    volatile kuint32_t Ipisr;
    kuint32_t Status;

    if (!sprt_emacps)
        return -ER_NREADY;

    /*!< Make sure no other PHY operation is currently in progress */
    Status = XEmacPs_ReadReg(sprt_emacps->Config.BaseAddress, XEMACPS_NWSR_OFFSET);
    if (!(Status & XEMACPS_NWSR_MDIOIDLE_MASK))
        return -ER_BUSY;

    /*!< Construct Mgtcr mask for the operation */
    Mgtcr = XEMACPS_PHYMNTNC_OP_MASK | XEMACPS_PHYMNTNC_OP_R_MASK |
            (PhyAddress << XEMACPS_PHYMNTNC_PHAD_SHFT_MSK) |
            (RegisterNum << XEMACPS_PHYMNTNC_PREG_SHFT_MSK);

    /*!< Write Mgtcr and wait for completion */
    XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, XEMACPS_PHYMNTNC_OFFSET, Mgtcr);

    do {
        Ipisr = XEmacPs_ReadReg(sprt_emacps->Config.BaseAddress, XEMACPS_NWSR_OFFSET);

    } while ((Ipisr & XEMACPS_NWSR_MDIOIDLE_MASK) == 0x00000000U);

    /*!< Read data */
    *PhyDataPtr = (kuint16_t)XEmacPs_ReadReg(sprt_emacps->Config.BaseAddress, XEMACPS_PHYMNTNC_OFFSET);

    return ER_NORMAL;
}

kint32_t XEmacPs_PhyWrite(XEmacPs *sprt_emacps, kuint32_t PhyAddress, kuint32_t RegisterNum, kuint16_t PhyData)
{
    kuint32_t Mgtcr;
    volatile kuint32_t Ipisr;
    kuint32_t Status;

    if (!sprt_emacps)
        return -ER_NREADY;

    /*!< Make sure no other PHY operation is currently in progress */
    Status = XEmacPs_ReadReg(sprt_emacps->Config.BaseAddress, XEMACPS_NWSR_OFFSET);
    if (!(Status & XEMACPS_NWSR_MDIOIDLE_MASK))
        return -ER_BUSY;

    /*!< Construct Mgtcr mask for the operation */
    Mgtcr = XEMACPS_PHYMNTNC_OP_MASK | XEMACPS_PHYMNTNC_OP_W_MASK |
            (PhyAddress << XEMACPS_PHYMNTNC_PHAD_SHFT_MSK) |
            (RegisterNum << XEMACPS_PHYMNTNC_PREG_SHFT_MSK) | (kuint32_t)PhyData;

    /*!< Write Mgtcr and wait for completion */
    XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, XEMACPS_PHYMNTNC_OFFSET, Mgtcr);

    do {
        Ipisr = XEmacPs_ReadReg(sprt_emacps->Config.BaseAddress, XEMACPS_NWSR_OFFSET);

    } while ((Ipisr & XEMACPS_NWSR_MDIOIDLE_MASK) == 0x00000000U);

    return ER_NORMAL;
}

void XEmacPs_PhyDetect(XEmacPs *sprt_xemac)
{
    kuint16_t phy_reg;
    kuint32_t phy_addr;
    kuint32_t *phymapemac;

    if (XEMACPS_IS_ETH_0(sprt_xemac))
        phymapemac = &phymapemac0[0];
    else
        phymapemac = &phymapemac1[0];

    for (phy_addr = 31; phy_addr > 0; phy_addr--) 
    {
        XEmacPs_PhyRead(sprt_xemac, phy_addr, PHY_DETECT_REG, &phy_reg);

        if ((phy_reg != 0xFFFF) &&
            ((phy_reg & PHY_DETECT_MASK) == PHY_DETECT_MASK)) 
        {
            /*!< Found a valid PHY address */
            print_debug("XEmacPs %s: PHY detected at address %d.\r\n", __FUNCTION__, phy_addr);

            phymapemac[phy_addr] = true;
            XEmacPs_PhyRead(sprt_xemac, phy_addr, PHY_IDENTIFIER_1_REG, &phy_reg);

            if ((phy_reg != PHY_MARVELL_IDENTIFIER) &&
                (phy_reg != PHY_TI_IDENTIFIER) &&
                (phy_reg != PHY_REALTEK_IDENTIFIER))
                print_debug("WARNING: Not a Marvell or TI or Realtek Ethernet PHY. Please verify the initialization sequence\r\n");
        }
    }
}

static kuint32_t XEmacPs_PhyLinkDetect(XEmacPs *xemacp, kuint32_t phy_addr)
{
    kuint16_t status;

    /*!< Read Phy Status register twice to get the confirmation of the current link status. */
    XEmacPs_PhyRead(xemacp, phy_addr, IEEE_STATUS_REG_OFFSET, &status);
    XEmacPs_PhyRead(xemacp, phy_addr, IEEE_STATUS_REG_OFFSET, &status);

    if (status & IEEE_STAT_LINK_STATUS)
        return 1;
    
    return 0;
}

static kuint32_t XEmacPs_PhyAutoNegStatus(XEmacPs *xemacp, kuint32_t phy_addr)
{
    kuint16_t status;

    /*!< Read Phy Status register twice to get the confirmation of the current link status. */
    XEmacPs_PhyRead(xemacp, phy_addr, IEEE_STATUS_REG_OFFSET, &status);
    XEmacPs_PhyRead(xemacp, phy_addr, IEEE_STATUS_REG_OFFSET, &status);

    if (status & IEEE_STAT_AUTONEGOTIATE_COMPLETE)
        return 1;
    
    return 0;
}

void XEmacPs_SetOperatingSpeed(XEmacPs *sprt_emacps, kuint16_t Speed)
{
    kuint32_t Reg;

    if ((!sprt_emacps) ||
        (!sprt_emacps->IsReady) ||
        ((Speed != (kuint16_t)10) || (Speed != (kuint16_t)100) || (Speed != (kuint16_t)1000)))
        return;

    Reg = XEmacPs_ReadReg(sprt_emacps->Config.BaseAddress, XEMACPS_NWCFG_OFFSET);
    Reg &= (kuint32_t)(~(XEMACPS_NWCFG_1000_MASK | XEMACPS_NWCFG_100_MASK));

    switch (Speed) 
    {
        case 10:
            break;

        case 100:
            Reg |= XEMACPS_NWCFG_100_MASK;
            break;

        case 1000:
            Reg |= XEMACPS_NWCFG_1000_MASK;
            break;
    }

    /*!< Set register and return */
    XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, XEMACPS_NWCFG_OFFSET, Reg);
}

static kint32_t XEmacPs_GetTiPhySpeed(XEmacPs *xemacpsp, kuint32_t phy_addr)
{
    kuint16_t control;
    kuint16_t status;
    kuint16_t status_speed;
    kuint32_t timeout_counter = 0;
    kuint32_t phyregtemp;
    int i;
    kint32_t RetStatus;

    print_debug("Start PHY autonegotiation \r\n");

    XEmacPs_PhyRead(xemacpsp, phy_addr, 0x1F, (kuint16_t *)&phyregtemp);
    phyregtemp |= 0x4000;
    XEmacPs_PhyWrite(xemacpsp, phy_addr, 0x1F, phyregtemp);

    RetStatus = XEmacPs_PhyRead(xemacpsp, phy_addr, 0x1F, (kuint16_t *)&phyregtemp);
    if (RetStatus) 
    {
        print_debug("Error during sw reset \n\r");
        return RetStatus;
    }

    XEmacPs_PhyRead(xemacpsp, phy_addr, 0, (kuint16_t *)&phyregtemp);
    phyregtemp |= 0x8000;
    XEmacPs_PhyWrite(xemacpsp, phy_addr, 0, phyregtemp);

    /*!< Delay */
    for (i = 0; i < 1000000000; i++);

    RetStatus = XEmacPs_PhyRead(xemacpsp, phy_addr, 0, (kuint16_t *)&phyregtemp);
    if (RetStatus)
    {
        print_debug("Error during reset \n\r");
        return RetStatus;
    }

    /*!< FIFO depth */
    XEmacPs_PhyWrite(xemacpsp, phy_addr, PHY_TI_CR, PHY_TI_CRVAL);
    RetStatus = XEmacPs_PhyRead(xemacpsp, phy_addr, PHY_TI_CR, (kuint16_t *)&phyregtemp);
    if (RetStatus)
    {
        print_debug("Error writing to 0x10 \n\r");
        return RetStatus;
    }

    /*!< TX/RX tuning */
    /*!< Write to PHY_RGMIIDCTL */
    XEmacPs_PhyWrite(xemacpsp, phy_addr, PHY_REGCR, PHY_REGCR_ADDR);
    XEmacPs_PhyWrite(xemacpsp, phy_addr, PHY_ADDAR, PHY_RGMIIDCTL);
    XEmacPs_PhyWrite(xemacpsp, phy_addr, PHY_REGCR, PHY_REGCR_DATA);
    RetStatus = XEmacPs_PhyWrite(xemacpsp, phy_addr, PHY_ADDAR, 0xA8);
    if (RetStatus)
    {
        print_debug("Error in tuning");
        return RetStatus;
    }

    /*!< Read PHY_RGMIIDCTL */
    XEmacPs_PhyWrite(xemacpsp, phy_addr, PHY_REGCR, PHY_REGCR_ADDR);
    XEmacPs_PhyWrite(xemacpsp, phy_addr, PHY_ADDAR, PHY_RGMIIDCTL);
    XEmacPs_PhyWrite(xemacpsp, phy_addr, PHY_REGCR, PHY_REGCR_DATA);
    RetStatus = XEmacPs_PhyRead(xemacpsp, phy_addr, PHY_ADDAR, (kuint16_t *)&phyregtemp);
    if (RetStatus)
    {
        print_debug("Error in tuning");
        return RetStatus;
    }

    /*!< Write PHY_RGMIICTL */
    XEmacPs_PhyWrite(xemacpsp, phy_addr, PHY_REGCR, PHY_REGCR_ADDR);
    XEmacPs_PhyWrite(xemacpsp, phy_addr, PHY_ADDAR, PHY_RGMIICTL);
    XEmacPs_PhyWrite(xemacpsp, phy_addr, PHY_REGCR, PHY_REGCR_DATA);
    RetStatus = XEmacPs_PhyWrite(xemacpsp, phy_addr, PHY_ADDAR, 0xD3);
    if (RetStatus)
    {
        print_debug("Error in tuning");
        return RetStatus;
    }

    /*!< Read PHY_RGMIICTL */
    XEmacPs_PhyWrite(xemacpsp, phy_addr, PHY_REGCR, PHY_REGCR_ADDR);
    XEmacPs_PhyWrite(xemacpsp, phy_addr, PHY_ADDAR, PHY_RGMIICTL);
    XEmacPs_PhyWrite(xemacpsp, phy_addr, PHY_REGCR, PHY_REGCR_DATA);
    RetStatus = XEmacPs_PhyRead(xemacpsp, phy_addr, PHY_ADDAR, (kuint16_t *)&phyregtemp);
    if (RetStatus)
    {
        print_debug("Error in tuning");
        return RetStatus;
    }

    /*!< SW workaround for unstable link when RX_CTRL is not STRAP MODE 3 or 4 */
    XEmacPs_PhyWrite(xemacpsp, phy_addr, PHY_REGCR, PHY_REGCR_ADDR);
    XEmacPs_PhyWrite(xemacpsp, phy_addr, PHY_ADDAR, PHY_TI_CFG4);
    XEmacPs_PhyWrite(xemacpsp, phy_addr, PHY_REGCR, PHY_REGCR_DATA);

    RetStatus = XEmacPs_PhyRead(xemacpsp, phy_addr, PHY_ADDAR, (kuint16_t *)&phyregtemp);
    phyregtemp &= ~(PHY_TI_CFG4RESVDBIT7);
    XEmacPs_PhyWrite(xemacpsp, phy_addr, PHY_REGCR, PHY_REGCR_ADDR);
    XEmacPs_PhyWrite(xemacpsp, phy_addr, PHY_ADDAR, PHY_TI_CFG4);
    XEmacPs_PhyWrite(xemacpsp, phy_addr, PHY_REGCR, PHY_REGCR_DATA);
    RetStatus = XEmacPs_PhyWrite(xemacpsp, phy_addr, PHY_ADDAR, phyregtemp);

    XEmacPs_PhyRead(xemacpsp, phy_addr, IEEE_AUTONEGO_ADVERTISE_REG, &control);
    control |= IEEE_ASYMMETRIC_PAUSE_MASK;
    control |= IEEE_PAUSE_MASK;
    control |= ADVERTISE_100;
    control |= ADVERTISE_10;
    XEmacPs_PhyWrite(xemacpsp, phy_addr, IEEE_AUTONEGO_ADVERTISE_REG, control);

    XEmacPs_PhyRead(xemacpsp, phy_addr, IEEE_1000_ADVERTISE_REG_OFFSET, &control);
    control |= ADVERTISE_1000;
    XEmacPs_PhyWrite(xemacpsp, phy_addr, IEEE_1000_ADVERTISE_REG_OFFSET, control);

    XEmacPs_PhyRead(xemacpsp, phy_addr, IEEE_CONTROL_REG_OFFSET, &control);
    control |= IEEE_CTRL_AUTONEGOTIATE_ENABLE;
    control |= IEEE_STAT_AUTONEGOTIATE_RESTART;
    XEmacPs_PhyWrite(xemacpsp, phy_addr, IEEE_CONTROL_REG_OFFSET, control);

    XEmacPs_PhyRead(xemacpsp, phy_addr, IEEE_CONTROL_REG_OFFSET, &control);
    XEmacPs_PhyRead(xemacpsp, phy_addr, IEEE_STATUS_REG_OFFSET, &status);

    print_debug("Waiting for PHY to complete autonegotiation.\r\n");

    while (!(status & IEEE_STAT_AUTONEGOTIATE_COMPLETE)) 
    {
        delay_s(1);
        timeout_counter++;

        if (timeout_counter == 30) 
        {
            print_debug("Auto negotiation error \r\n");
            return -ER_TIMEOUT;
        }

        XEmacPs_PhyRead(xemacpsp, phy_addr, IEEE_STATUS_REG_OFFSET, &status);
    }

    print_debug("autonegotiation complete \r\n");

    XEmacPs_PhyRead(xemacpsp, phy_addr, PHY_STS, &status_speed);
    if ((status_speed & 0xC000) == 0x8000)
        return 1000;
    if ((status_speed & 0xC000) == 0x4000)
        return 100;

    return 10;
}

static kint32_t XEmacPs_GetRealtekPhySpeed(XEmacPs *xemacpsp, kuint32_t phy_addr)
{
    kuint16_t control;
    kuint16_t status;
    kuint16_t status_speed;
    kuint32_t timeout_counter = 0;
    kuint32_t temp_speed;

    print_debug("Start PHY autonegotiation \r\n");

    XEmacPs_PhyRead(xemacpsp, phy_addr, IEEE_AUTONEGO_ADVERTISE_REG, &control);
    control |= IEEE_ASYMMETRIC_PAUSE_MASK;
    control |= IEEE_PAUSE_MASK;
    control |= ADVERTISE_100;
    control |= ADVERTISE_10;
    XEmacPs_PhyWrite(xemacpsp, phy_addr, IEEE_AUTONEGO_ADVERTISE_REG, control);

    XEmacPs_PhyRead(xemacpsp, phy_addr, IEEE_1000_ADVERTISE_REG_OFFSET, &control);
    control |= ADVERTISE_1000;
    XEmacPs_PhyWrite(xemacpsp, phy_addr, IEEE_1000_ADVERTISE_REG_OFFSET, control);

    XEmacPs_PhyRead(xemacpsp, phy_addr, IEEE_CONTROL_REG_OFFSET, &control);
    control |= IEEE_CTRL_AUTONEGOTIATE_ENABLE;
    control |= IEEE_STAT_AUTONEGOTIATE_RESTART;
    XEmacPs_PhyWrite(xemacpsp, phy_addr, IEEE_CONTROL_REG_OFFSET, control);

    XEmacPs_PhyRead(xemacpsp, phy_addr, IEEE_CONTROL_REG_OFFSET, &control);
    control |= IEEE_CTRL_RESET_MASK;
    XEmacPs_PhyWrite(xemacpsp, phy_addr, IEEE_CONTROL_REG_OFFSET, control);

    while (1) 
    {
        XEmacPs_PhyRead(xemacpsp, phy_addr, IEEE_CONTROL_REG_OFFSET, &control);
        if (!(control & IEEE_CTRL_RESET_MASK))        
            break;
    }

    XEmacPs_PhyRead(xemacpsp, phy_addr, IEEE_STATUS_REG_OFFSET, &status);

    print_debug("Waiting for PHY to complete autonegotiation.\r\n");

    while ( !(status & IEEE_STAT_AUTONEGOTIATE_COMPLETE) ) 
    {
        delay_s(1);
        timeout_counter++;

        if (timeout_counter == 30) 
        {
            print_debug("Auto negotiation error \r\n");
            return -ER_TIMEOUT;
        }

        XEmacPs_PhyRead(xemacpsp, phy_addr, IEEE_STATUS_REG_OFFSET, &status);
    }
    print_debug("autonegotiation complete \r\n");

    XEmacPs_PhyRead(xemacpsp, phy_addr,IEEE_SPECIFIC_STATUS_REG, &status_speed);
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

static kint32_t XEmacPs_GetMarvellPhySpeed(XEmacPs *xemacpsp, kuint32_t phy_addr)
{
    kuint16_t temp;
    kuint16_t control;
    kuint16_t status;
    kuint16_t status_speed;
    kuint32_t timeout_counter = 0;
    kuint32_t temp_speed;

    print_debug("Start PHY autonegotiation \r\n");

    XEmacPs_PhyWrite(xemacpsp,phy_addr, IEEE_PAGE_ADDRESS_REGISTER, 2);
    XEmacPs_PhyRead(xemacpsp, phy_addr, IEEE_CONTROL_REG_MAC, &control);
    control |= IEEE_RGMII_TXRX_CLOCK_DELAYED_MASK;
    XEmacPs_PhyWrite(xemacpsp, phy_addr, IEEE_CONTROL_REG_MAC, control);

    XEmacPs_PhyWrite(xemacpsp, phy_addr, IEEE_PAGE_ADDRESS_REGISTER, 0);

    XEmacPs_PhyRead(xemacpsp, phy_addr, IEEE_AUTONEGO_ADVERTISE_REG, &control);
    control |= IEEE_ASYMMETRIC_PAUSE_MASK;
    control |= IEEE_PAUSE_MASK;
    control |= ADVERTISE_100;
    control |= ADVERTISE_10;
    XEmacPs_PhyWrite(xemacpsp, phy_addr, IEEE_AUTONEGO_ADVERTISE_REG, control);

    XEmacPs_PhyRead(xemacpsp, phy_addr, IEEE_1000_ADVERTISE_REG_OFFSET, &control);
    control |= ADVERTISE_1000;
    XEmacPs_PhyWrite(xemacpsp, phy_addr, IEEE_1000_ADVERTISE_REG_OFFSET, control);

    XEmacPs_PhyWrite(xemacpsp, phy_addr, IEEE_PAGE_ADDRESS_REGISTER, 0);
    XEmacPs_PhyRead(xemacpsp, phy_addr, IEEE_COPPER_SPECIFIC_CONTROL_REG, &control);
    control |= (7 << 12);	/*!< max number of gigabit attempts */
    control |= (1 << 11);	/*!< enable downshift */
    XEmacPs_PhyWrite(xemacpsp, phy_addr, IEEE_COPPER_SPECIFIC_CONTROL_REG, control);
    XEmacPs_PhyRead(xemacpsp, phy_addr, IEEE_CONTROL_REG_OFFSET, &control);
    control |= IEEE_CTRL_AUTONEGOTIATE_ENABLE;
    control |= IEEE_STAT_AUTONEGOTIATE_RESTART;
    XEmacPs_PhyWrite(xemacpsp, phy_addr, IEEE_CONTROL_REG_OFFSET, control);

    XEmacPs_PhyRead(xemacpsp, phy_addr, IEEE_CONTROL_REG_OFFSET, &control);
    control |= IEEE_CTRL_RESET_MASK;
    XEmacPs_PhyWrite(xemacpsp, phy_addr, IEEE_CONTROL_REG_OFFSET, control);

    while (1) 
    {
        XEmacPs_PhyRead(xemacpsp, phy_addr, IEEE_CONTROL_REG_OFFSET, &control);
        if (!(control & IEEE_CTRL_RESET_MASK))
            break;
    }

    XEmacPs_PhyRead(xemacpsp, phy_addr, IEEE_STATUS_REG_OFFSET, &status);

    print_debug("Waiting for PHY to complete autonegotiation.\r\n");

    while (!(status & IEEE_STAT_AUTONEGOTIATE_COMPLETE)) 
    {
        delay_s(1);
        XEmacPs_PhyRead(xemacpsp, 
                phy_addr, IEEE_COPPER_SPECIFIC_STATUS_REG_2, &temp);
        timeout_counter++;

        if (timeout_counter == 30) 
        {
            print_debug("Auto negotiation error \r\n");
            return -ER_TIMEOUT;
        }

        XEmacPs_PhyRead(xemacpsp, phy_addr, IEEE_STATUS_REG_OFFSET, &status);
    }
    print_debug("autonegotiation complete \r\n");

    XEmacPs_PhyRead(xemacpsp, phy_addr,IEEE_SPECIFIC_STATUS_REG, &status_speed);
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

static kint32_t XEmacPs_GetIEEEPhySpeed(XEmacPs *xemacpsp, kuint32_t phy_addr)
{
    kuint16_t phy_identity;
    kint32_t RetStatus;

    XEmacPs_PhyRead(xemacpsp, phy_addr, PHY_IDENTIFIER_1_REG, &phy_identity);

    if (phy_identity == PHY_TI_IDENTIFIER)
        RetStatus = XEmacPs_GetTiPhySpeed(xemacpsp, phy_addr);
    else if (phy_identity == PHY_REALTEK_IDENTIFIER)
        RetStatus = XEmacPs_GetRealtekPhySpeed(xemacpsp, phy_addr);
    else
        RetStatus = XEmacPs_GetMarvellPhySpeed(xemacpsp, phy_addr);

    return RetStatus;
}

static void XEmacPs_SetUpSLCRDivisors(kuint32_t mac_baseaddr, kint32_t speed)
{
    volatile kuint32_t slcrBaseAddress;
    kuint32_t SlcrDiv0 = 0;
    kuint32_t SlcrDiv1 = 0;
    kuint32_t SlcrTxClkCntrl;
    kuint32_t gigeversion;
    volatile kuint32_t CrlApbBaseAddr;
    kuint32_t CrlApbDiv0 = 0;
    kuint32_t CrlApbDiv1 = 0;
    kuint32_t CrlApbGemCtrl;

    gigeversion = ((mrt_readl(mac_baseaddr + 0xFC)) >> 16) & 0xFFF;
    if (gigeversion == 2) 
    {
        *(volatile kuint32_t *)(SLCR_UNLOCK_ADDR) = SLCR_UNLOCK_KEY_VALUE;

        if (mac_baseaddr == ZYNQ_EMACPS_0_BASEADDR)
            slcrBaseAddress = SLCR_GEM0_CLK_CTRL_ADDR;
        else
            slcrBaseAddress = SLCR_GEM1_CLK_CTRL_ADDR;

        if ((*(volatile kuint32_t *)slcrBaseAddress) & SLCR_GEM_SRCSEL_EMIO)
                return;

        if (speed == 1000) 
        {
            if (mac_baseaddr == XPAR_XEMACPS_0_BASEADDR) 
            {
#ifdef XPAR_PS7_ETHERNET_0_ENET_SLCR_1000MBPS_DIV0
                SlcrDiv0 = XPAR_PS7_ETHERNET_0_ENET_SLCR_1000MBPS_DIV0;
                SlcrDiv1 = XPAR_PS7_ETHERNET_0_ENET_SLCR_1000MBPS_DIV1;
#endif
            } 
            else 
            {
#ifdef XPAR_PS7_ETHERNET_1_ENET_SLCR_1000MBPS_DIV0
                SlcrDiv0 = XPAR_PS7_ETHERNET_1_ENET_SLCR_1000MBPS_DIV0;
                SlcrDiv1 = XPAR_PS7_ETHERNET_1_ENET_SLCR_1000MBPS_DIV1;
#endif
            }
        } 
        else if (speed == 100) 
        {
            if (mac_baseaddr == XPAR_XEMACPS_0_BASEADDR) 
            {
#ifdef XPAR_PS7_ETHERNET_0_ENET_SLCR_100MBPS_DIV0
                SlcrDiv0 = XPAR_PS7_ETHERNET_0_ENET_SLCR_100MBPS_DIV0;
                SlcrDiv1 = XPAR_PS7_ETHERNET_0_ENET_SLCR_100MBPS_DIV1;
#endif
            } 
            else 
            {
#ifdef XPAR_PS7_ETHERNET_1_ENET_SLCR_100MBPS_DIV0
                SlcrDiv0 = XPAR_PS7_ETHERNET_1_ENET_SLCR_100MBPS_DIV0;
                SlcrDiv1 = XPAR_PS7_ETHERNET_1_ENET_SLCR_100MBPS_DIV1;
#endif
            }
        } 
        else 
        {
            if (mac_baseaddr == XPAR_XEMACPS_0_BASEADDR) 
            {
#ifdef XPAR_PS7_ETHERNET_0_ENET_SLCR_10MBPS_DIV0
                SlcrDiv0 = XPAR_PS7_ETHERNET_0_ENET_SLCR_10MBPS_DIV0;
                SlcrDiv1 = XPAR_PS7_ETHERNET_0_ENET_SLCR_10MBPS_DIV1;
#endif
            } 
            else 
            {
#ifdef XPAR_PS7_ETHERNET_1_ENET_SLCR_10MBPS_DIV0
                SlcrDiv0 = XPAR_PS7_ETHERNET_1_ENET_SLCR_10MBPS_DIV0;
                SlcrDiv1 = XPAR_PS7_ETHERNET_1_ENET_SLCR_10MBPS_DIV1;
#endif
            }
        }

        if (SlcrDiv0 != 0 && SlcrDiv1 != 0) 
        {
            SlcrTxClkCntrl = *(volatile kuint32_t *)(kuint32_t)(slcrBaseAddress);
            SlcrTxClkCntrl &= EMACPS_SLCR_DIV_MASK;
            SlcrTxClkCntrl |= (SlcrDiv1 << 20);
            SlcrTxClkCntrl |= (SlcrDiv0 << 8);
            *(volatile kuint32_t *)(kuint32_t)(slcrBaseAddress) = SlcrTxClkCntrl;
            *(volatile kuint32_t *)(SLCR_LOCK_ADDR) = SLCR_LOCK_KEY_VALUE;
        } 
        else
            print_debug("Clock Divisors incorrect - Please check\r\n");
    } 
    else if (gigeversion > 2) 
    {
        /*!< Setup divisors in CRL_APB for Zynq Ultrascale+ MPSoC */
        if (mac_baseaddr == ZYNQMP_EMACPS_0_BASEADDR)
            CrlApbBaseAddr = CRL_APB_GEM0_REF_CTRL;
        else if (mac_baseaddr == ZYNQMP_EMACPS_1_BASEADDR)
            CrlApbBaseAddr = CRL_APB_GEM1_REF_CTRL;
        else if (mac_baseaddr == ZYNQMP_EMACPS_2_BASEADDR)
            CrlApbBaseAddr = CRL_APB_GEM2_REF_CTRL;
        else if (mac_baseaddr == ZYNQMP_EMACPS_3_BASEADDR)
            CrlApbBaseAddr = CRL_APB_GEM3_REF_CTRL;

        if (speed == 1000) 
        {
            if (mac_baseaddr == ZYNQMP_EMACPS_0_BASEADDR) 
            {
#ifdef XPAR_PSU_ETHERNET_0_ENET_SLCR_1000MBPS_DIV0
                CrlApbDiv0 = XPAR_PSU_ETHERNET_0_ENET_SLCR_1000MBPS_DIV0;
                CrlApbDiv1 = XPAR_PSU_ETHERNET_0_ENET_SLCR_1000MBPS_DIV1;
#endif
            } 
            else if (mac_baseaddr == ZYNQMP_EMACPS_1_BASEADDR) 
            {
#ifdef XPAR_PSU_ETHERNET_1_ENET_SLCR_1000MBPS_DIV0
                CrlApbDiv0 = XPAR_PSU_ETHERNET_1_ENET_SLCR_1000MBPS_DIV0;
                CrlApbDiv1 = XPAR_PSU_ETHERNET_1_ENET_SLCR_1000MBPS_DIV1;
#endif
            } 
            else if (mac_baseaddr == ZYNQMP_EMACPS_2_BASEADDR) 
            {
#ifdef XPAR_PSU_ETHERNET_2_ENET_SLCR_1000MBPS_DIV0
                CrlApbDiv0 = XPAR_PSU_ETHERNET_2_ENET_SLCR_1000MBPS_DIV0;
                CrlApbDiv1 = XPAR_PSU_ETHERNET_2_ENET_SLCR_1000MBPS_DIV1;
#endif
            } 
            else if (mac_baseaddr == ZYNQMP_EMACPS_3_BASEADDR) 
            {
#ifdef XPAR_PSU_ETHERNET_3_ENET_SLCR_1000MBPS_DIV0
                CrlApbDiv0 = XPAR_PSU_ETHERNET_3_ENET_SLCR_1000MBPS_DIV0;
                CrlApbDiv1 = XPAR_PSU_ETHERNET_3_ENET_SLCR_1000MBPS_DIV1;
#endif
            }
        } 
        else if (speed == 100) 
        {
            if (mac_baseaddr == ZYNQMP_EMACPS_0_BASEADDR) 
            {
#ifdef XPAR_PSU_ETHERNET_0_ENET_SLCR_100MBPS_DIV0
                CrlApbDiv0 = XPAR_PSU_ETHERNET_0_ENET_SLCR_100MBPS_DIV0;
                CrlApbDiv1 = XPAR_PSU_ETHERNET_0_ENET_SLCR_100MBPS_DIV1;
#endif
            } 
            else if (mac_baseaddr == ZYNQMP_EMACPS_1_BASEADDR) 
            {
#ifdef XPAR_PSU_ETHERNET_1_ENET_SLCR_100MBPS_DIV0
                CrlApbDiv0 = XPAR_PSU_ETHERNET_1_ENET_SLCR_100MBPS_DIV0;
                CrlApbDiv1 = XPAR_PSU_ETHERNET_1_ENET_SLCR_100MBPS_DIV1;
#endif
            } 
            else if (mac_baseaddr == ZYNQMP_EMACPS_2_BASEADDR) 
            {
#ifdef XPAR_PSU_ETHERNET_2_ENET_SLCR_100MBPS_DIV0
                CrlApbDiv0 = XPAR_PSU_ETHERNET_2_ENET_SLCR_100MBPS_DIV0;
                CrlApbDiv1 = XPAR_PSU_ETHERNET_2_ENET_SLCR_100MBPS_DIV1;
#endif
            } 
            else if (mac_baseaddr == ZYNQMP_EMACPS_3_BASEADDR) 
            {
#ifdef XPAR_PSU_ETHERNET_3_ENET_SLCR_100MBPS_DIV0
                CrlApbDiv0 = XPAR_PSU_ETHERNET_3_ENET_SLCR_100MBPS_DIV0;
                CrlApbDiv1 = XPAR_PSU_ETHERNET_3_ENET_SLCR_100MBPS_DIV1;
#endif
            }
        } 
        else 
        {
            if (mac_baseaddr == ZYNQMP_EMACPS_0_BASEADDR) 
            {
#ifdef XPAR_PSU_ETHERNET_0_ENET_SLCR_10MBPS_DIV0
                CrlApbDiv0 = XPAR_PSU_ETHERNET_0_ENET_SLCR_10MBPS_DIV0;
                CrlApbDiv1 = XPAR_PSU_ETHERNET_0_ENET_SLCR_10MBPS_DIV1;
#endif
            } 
            else if (mac_baseaddr == ZYNQMP_EMACPS_1_BASEADDR) 
            {
#ifdef XPAR_PSU_ETHERNET_1_ENET_SLCR_10MBPS_DIV0
                CrlApbDiv0 = XPAR_PSU_ETHERNET_1_ENET_SLCR_10MBPS_DIV0;
                CrlApbDiv1 = XPAR_PSU_ETHERNET_1_ENET_SLCR_10MBPS_DIV1;
#endif
            } 
            else if (mac_baseaddr == ZYNQMP_EMACPS_2_BASEADDR) 
            {
#ifdef XPAR_PSU_ETHERNET_2_ENET_SLCR_10MBPS_DIV0
                CrlApbDiv0 = XPAR_PSU_ETHERNET_2_ENET_SLCR_10MBPS_DIV0;
                CrlApbDiv1 = XPAR_PSU_ETHERNET_2_ENET_SLCR_10MBPS_DIV1;
#endif
            } 
            else if (mac_baseaddr == ZYNQMP_EMACPS_3_BASEADDR) 
            {
#ifdef XPAR_PSU_ETHERNET_3_ENET_SLCR_10MBPS_DIV0
                CrlApbDiv0 = XPAR_PSU_ETHERNET_3_ENET_SLCR_10MBPS_DIV0;
                CrlApbDiv1 = XPAR_PSU_ETHERNET_3_ENET_SLCR_10MBPS_DIV1;
#endif
            }
        }

        if (CrlApbDiv0 != 0 && CrlApbDiv1 != 0) 
        {
        #if (defined(EL1_NONSECURE) && (EL1_NONSECURE))
            XSmc_OutVar RegRead;
            RegRead = Xil_Smc(MMIO_READ_SMC_FID, (kuint64_t)(CrlApbBaseAddr),
                                0, 0, 0, 0, 0, 0);
            CrlApbGemCtrl = RegRead.Arg0 >> 32;
        #else
            CrlApbGemCtrl = *(volatile kuint32_t *)(kuint32_t)(CrlApbBaseAddr);
        #endif

            CrlApbGemCtrl &= ~CRL_APB_GEM_DIV0_MASK;
            CrlApbGemCtrl |= CrlApbDiv0 << CRL_APB_GEM_DIV0_SHIFT;
            CrlApbGemCtrl &= ~CRL_APB_GEM_DIV1_MASK;
            CrlApbGemCtrl |= CrlApbDiv1 << CRL_APB_GEM_DIV1_SHIFT;

        #if (defined(EL1_NONSECURE) && (EL1_NONSECURE))
            Xil_Smc(MMIO_WRITE_SMC_FID, (kuint64_t)(CrlApbBaseAddr) | ((kuint64_t)(0xFFFFFFFF) << 32),
                    (kuint64_t)CrlApbGemCtrl, 0, 0, 0, 0, 0);
            do {
                RegRead = Xil_Smc(MMIO_READ_SMC_FID, (kuint64_t)(CrlApbBaseAddr), 0, 0, 0, 0, 0, 0);
            } while((RegRead.Arg0 >> 32) != CrlApbGemCtrl);
        #else
            *(volatile kuint32_t *)(kuint32_t)(CrlApbBaseAddr) = CrlApbGemCtrl;
        #endif
        } 
        else
            print_debug("Clock Divisors incorrect - Please check\r\n");
    }
}

kint32_t XEmacPs_PhySetup(XEmacPs *xemacpsp, kuint32_t phy_addr)
{
    kint32_t link_speed;
    kuint32_t conv_present = 0;
    kuint32_t convspeeddupsetting = 0;
    kuint32_t convphyaddr = 0;

#ifdef XPAR_GMII2RGMIICON_0N_ETH0_ADDR
    convphyaddr = XPAR_GMII2RGMIICON_0N_ETH0_ADDR;
    conv_present = 1;
#endif
#ifdef XPAR_GMII2RGMIICON_0N_ETH1_ADDR
    convphyaddr = XPAR_GMII2RGMIICON_0N_ETH1_ADDR;
    conv_present = 1;
#endif

#ifdef CONFIG_LINKSPEED_AUTODETECT
    link_speed = XEmacPs_GetIEEEPhySpeed(xemacpsp, phy_addr);
    if (link_speed == 1000) 
    {
        XEmacPs_SetUpSLCRDivisors(xemacpsp->Config.BaseAddress, 1000);
        convspeeddupsetting = XEMACPS_GMII2RGMII_SPEED1000_FD;
    } 
    else if (link_speed == 100) 
    {
        XEmacPs_SetUpSLCRDivisors(xemacpsp->Config.BaseAddress, 100);
        convspeeddupsetting = XEMACPS_GMII2RGMII_SPEED100_FD;
    }
    else if (link_speed == 10)
    {
        XEmacPs_SetUpSLCRDivisors(xemacpsp->Config.BaseAddress, 10);
        convspeeddupsetting = XEMACPS_GMII2RGMII_SPEED10_FD;
    } 
    else 
    {
        print_debug("Phy setup error \r\n");
        return -ER_UNVALID;
    }

#elif defined(CONFIG_LINKSPEED1000)
    XEmacPs_SetUpSLCRDivisors(xemacpsp->Config.BaseAddress,1000);
    link_speed = 1000;
    configure_IEEE_phy_speed(xemacpsp, phy_addr, link_speed);
    convspeeddupsetting = XEMACPS_GMII2RGMII_SPEED1000_FD;
    delay_s(1);

#elif defined(CONFIG_LINKSPEED100)
    XEmacPs_SetUpSLCRDivisors(xemacpsp->Config.BaseAddress,100);
    link_speed = 100;
    configure_IEEE_phy_speed(xemacpsp, phy_addr, link_speed);
    convspeeddupsetting = XEMACPS_GMII2RGMII_SPEED100_FD;
    delay_s(1);

#elif defined(CONFIG_LINKSPEED10)
    XEmacPs_SetUpSLCRDivisors(xemacpsp->Config.BaseAddress,10);
    link_speed = 10;
    configure_IEEE_phy_speed(xemacpsp, phy_addr, link_speed);
    convspeeddupsetting = XEMACPS_GMII2RGMII_SPEED10_FD;
    delay_s(1);
#endif

    if (conv_present) 
    {
        XEmacPs_PhyWrite(xemacpsp, convphyaddr,
                XEMACPS_GMII2RGMII_REG_NUM, convspeeddupsetting);
    }

    print_debug("link speed for phy address %d: %d\r\n", phy_addr, link_speed);
    return link_speed;
}

void XEmacPs_Init(XEmacPs *xemacpsp, void *hwaddr)
{
    kint32_t status;
    kuint32_t i;
    kuint32_t phyfoundforemac0 = false;
    kuint32_t phyfoundforemac1 = false;

#if (defined(LWIP_IGMP) && (LWIP_IGMP))
    XEmacPs_SetOptions(xemacpsp, XEMACPS_MULTICAST_OPTION);
#endif

    /*!< set mac address */
    status = XEmacPs_SetMacAddress(xemacpsp, hwaddr, 1);
    if (status)
        print_debug("In %s: Emac Mac Address set failed...\r\n",__func__);

    XEmacPs_SetMdioDivisor(xemacpsp, MDC_DIV_224);

/*!<  
 *  Please refer to file header comments for the file xemacpsif_physpeed.c
 *  to know more about the PHY programming sequence.
 *  For PCS PMA core, XEmacPs_PhySetup is called with the predefined PHY address
 *  exposed through xaparemeters.h
 *  For RGMII case, assuming multiple PHYs can be present on the MDIO bus,
 *  XEmacPs_PhyDetect is called to get the addresses of the PHY present on
 *  a particular MDIO bus (emac0 or emac1). This address map is populated
 *  in phymapemac0 or phymapemac1.
 *  XEmacPs_PhySetup is then called for each PHY present on the MDIO bus.
 */
#ifdef PCM_PMA_CORE_PRESENT
#ifdef  XPAR_GIGE_PCS_PMA_1000BASEX_CORE_PRESENT
    link_speed = XEmacPs_PhySetup(xemacpsp, XPAR_PCSPMA_1000BASEX_PHYADDR);
#elif XPAR_GIGE_PCS_PMA_SGMII_CORE_PRESENT
    link_speed = XEmacPs_PhySetup(xemacpsp, XPAR_PCSPMA_SGMII_PHYADDR);
#endif

#else
    XEmacPs_PhyDetect(xemacpsp);

    for (i = 31; i > 0; i--) 
    {
        if (XEMACPS_IS_ETH_0(xemacpsp)) 
        {
            if (phymapemac0[i] == true) 
            {
                link_speed = XEmacPs_PhySetup(xemacpsp, i);
                phyfoundforemac0 = true;
                phyaddrforemac = i;
            }
        } 
        else 
        {
            if (phymapemac1[i] == true) 
            {
                link_speed = XEmacPs_PhySetup(xemacpsp, i);
                phyfoundforemac1 = true;
                phyaddrforemac = i;
            }
        }
    }

    /*!< If no PHY was detected, use broadcast PHY address of 0 */
    if (XEMACPS_IS_ETH_0(xemacpsp)) 
    {
        if (phyfoundforemac0 == false)
            link_speed = XEmacPs_PhySetup(xemacpsp, 0);
    } 
    else 
    {
        if (phyfoundforemac1 == false)
            link_speed = XEmacPs_PhySetup(xemacpsp, 0);
    }
#endif

    if (link_speed < 0) 
    {
        eth_link_status = ETH_LINK_DOWN;
        print_debug("Assert due to phy setup failure \n\r",__func__);
    } 
    else
        eth_link_status = ETH_LINK_UP;

    XEmacPs_SetOperatingSpeed(xemacpsp, link_speed);

    /*!< Setting the operating speed of the MAC needs a delay. */

}

/*!< ----------------------------------------------------------------- */
#define XEMACPS_RING_SEEKAHEAD(RingPtr, BdPtr, NumBd)   \
do {    \
    kuint32_t Addr = (kuint32_t)(void *)(BdPtr);    \
    \
    Addr += ((RingPtr)->Separation * (NumBd));  \
    if ((Addr > (RingPtr)->HighBdAddr) ||   \
        ((kuint32_t)(void *)(BdPtr) > Addr))  \
        Addr -= (RingPtr)->Length;  \
    \
    (BdPtr) = (XEmacPs_Bd*)(void *)Addr;    \
} while (0)

#define XEMACPS_RING_SEEKBACK(RingPtr, BdPtr, NumBd)     \
do {    \
    kuint32_t Addr = (kuint32_t)(void *)(BdPtr);    \
    \
    Addr -= ((RingPtr)->Separation * (NumBd));  \
    if ((Addr < (RingPtr)->BaseBdAddr) ||   \
        ((kuint32_t)(void*)(BdPtr) < Addr))   \
        Addr += (RingPtr)->Length;  \
    \
    (BdPtr) = (XEmacPs_Bd*)(void*)Addr; \
} while (0)

#define XEMACPS_BD_TO_INDEX(ringptr, bdptr) \
    (((kuint32_t)bdptr - (kuint32_t)(ringptr)->BaseBdAddr) / (ringptr)->Separation)

#define BD_ALIGNMENT                    (XEMACPS_DMABD_MINIMUM_ALIGNMENT*2)

static u8_t bd_space[0x100000] __align(0x100000);

kint32_t XEmacPs_BdRingCreate(XEmacPs_BdRing * RingPtr, kuint32_t PhysAddr,
                                kuint32_t VirtAddr, kuint32_t Alignment, kuint32_t BdCount)
{
    kuint32_t i;
    kuint32_t BdVirtAddr;
    kuint32_t BdPhyAddr;
    kuint32_t VirtAddrLoc = VirtAddr;

    /*!< 
     * In case there is a failure prior to creating list, make sure the
     * following attributes are 0 to prevent calls to other functions
     * from doing anything.
     */
    RingPtr->AllCnt = 0U;
    RingPtr->FreeCnt = 0U;
    RingPtr->HwCnt = 0U;
    RingPtr->PreCnt = 0U;
    RingPtr->PostCnt = 0U;

    /*!< Make sure Alignment parameter meets minimum requirements */
    if (Alignment < (kuint32_t)XEMACPS_DMABD_MINIMUM_ALIGNMENT)
        return -ER_UNVALID;

    /*!< Make sure Alignment is a power of 2 */
    if (((Alignment - 0x00000001U) & Alignment)!=0x00000000U)
        return -ER_UNVALID;

    /*!< Make sure PhysAddr and VirtAddr are on same Alignment */
    if (((PhysAddr % Alignment)!=(kuint32_t)0) || ((VirtAddrLoc % Alignment)!=(kuint32_t)0))
        return -ER_UNVALID;

    /*!< Is BdCount reasonable? */
    if (BdCount == 0x00000000U)
        return -ER_UNVALID;

    /*!< Figure out how many bytes will be between the start of adjacent BDs */
    RingPtr->Separation = ((kuint32_t)sizeof(XEmacPs_Bd));

    /*!< 
     * Must make sure the ring doesn't span address 0x00000000. If it does,
     * then the next/prev BD traversal macros will fail.
     */
    if (VirtAddrLoc > ((VirtAddrLoc + (RingPtr->Separation * BdCount)) - (kuint32_t)1))
        return -ER_FAULT;

    /*!< 
     * Initial ring setup:
     *  - Clear the entire space
     *  - Setup each BD's BDA field with the physical address of the next BD
     */
    memset((void *)VirtAddrLoc, 0, (RingPtr->Separation * BdCount));

    BdVirtAddr = VirtAddrLoc;
    BdPhyAddr = PhysAddr + RingPtr->Separation;
    for (i = 1U; i < BdCount; i++) 
    {
        BdVirtAddr += RingPtr->Separation;
        BdPhyAddr += RingPtr->Separation;
    }

    /*!< Setup and initialize pointers and counters */
    RingPtr->IsRunning = false;
    RingPtr->BaseBdAddr = VirtAddrLoc;
    RingPtr->PhysBaseAddr = PhysAddr;
    RingPtr->HighBdAddr = BdVirtAddr;
    RingPtr->Length = ((RingPtr->HighBdAddr - RingPtr->BaseBdAddr) + RingPtr->Separation);
    RingPtr->AllCnt = (kuint32_t)BdCount;
    RingPtr->FreeCnt = (kuint32_t)BdCount;
    RingPtr->FreeHead = (XEmacPs_Bd *)(void *)VirtAddrLoc;
    RingPtr->PreHead = (XEmacPs_Bd *)VirtAddrLoc;
    RingPtr->HwHead = (XEmacPs_Bd *)VirtAddrLoc;
    RingPtr->HwTail = (XEmacPs_Bd *)VirtAddrLoc;
    RingPtr->PostHead = (XEmacPs_Bd *)VirtAddrLoc;
    RingPtr->BdaRestart = (XEmacPs_Bd *)(void *)PhysAddr;

    return ER_NORMAL;
}

static void XEmacPs_BdSetRxWrap(kuint32_t BdPtr)
{
    kuint32_t DataValueRx;
    kuint32_t *TempPtr;

    BdPtr += (kuint32_t)(XEMACPS_BD_ADDR_OFFSET);
    TempPtr = (kuint32_t *)BdPtr;

    if (isValid(TempPtr))
    {
        DataValueRx = *TempPtr;
        DataValueRx |= XEMACPS_RXBUF_WRAP_MASK;
        *TempPtr = DataValueRx;
    }
}

static void XEmacPs_BdSetTxWrap(kuint32_t BdPtr)
{
    kuint32_t DataValueTx;
    kuint32_t *TempPtr;

    BdPtr += (kuint32_t)(XEMACPS_BD_STAT_OFFSET);
    TempPtr = (kuint32_t *)BdPtr;

    if (isValid(TempPtr))
    {
        DataValueTx = *TempPtr;
        DataValueTx |= XEMACPS_TXBUF_WRAP_MASK;
        *TempPtr = DataValueTx;
    }
}

kint32_t XEmacPs_BdRingClone(XEmacPs_BdRing *RingPtr, XEmacPs_Bd *SrcBdPtr, kuint8_t Direction)
{
    kuint32_t i;
    kuint32_t CurBd;

    /*!< Can't do this function if there isn't a ring */
    if (RingPtr->AllCnt == 0x00000000U)
        return -ER_EMPTY;

    /*!< Can't do this function with the channel running */
    if (RingPtr->IsRunning)
        return -ER_LOCKED;

    /*!< Can't do this function with some of the BDs in use */
    if (RingPtr->FreeCnt != RingPtr->AllCnt)
        return -ER_ERROR;

    if ((Direction != (kuint8_t)XEMACPS_SEND) && (Direction != (kuint8_t)XEMACPS_RECV))
        return -ER_UNVALID;

    /*!< 
     * Starting from the top of the ring, save BD.Next, overwrite the entire
     * BD with the template, then restore BD.Next
     */
    CurBd = RingPtr->BaseBdAddr;
    for (i = 0U; i < RingPtr->AllCnt; i++) 
    {
        memcpy((void *)CurBd, SrcBdPtr, sizeof(XEmacPs_Bd));
        CurBd += RingPtr->Separation;
    }

    CurBd -= RingPtr->Separation;

    if (Direction == XEMACPS_RECV)
        XEmacPs_BdSetRxWrap(CurBd);
    else
        XEmacPs_BdSetTxWrap(CurBd);

    return ER_NORMAL;
}

kint32_t XEmacPs_BdRingAlloc(XEmacPs_BdRing *RingPtr, kuint32_t NumBd, XEmacPs_Bd **BdSetPtr)
{
    /*!< Enough free BDs available for the request? */
    if (RingPtr->FreeCnt < NumBd)
        return -ER_FAULT;

    /*!< Set the return argument and move FreeHead forward */
    *BdSetPtr = RingPtr->FreeHead;
    XEMACPS_RING_SEEKAHEAD(RingPtr, RingPtr->FreeHead, NumBd);

    RingPtr->FreeCnt -= NumBd;
    RingPtr->PreCnt += NumBd;

    return ER_NORMAL;
}

kint32_t XEmacPs_BdRingUnAlloc(XEmacPs_BdRing * RingPtr, kuint32_t NumBd, XEmacPs_Bd * BdSetPtr)
{
    if ((!RingPtr) ||
        (!BdSetPtr))
        return -ER_UNVALID;

    /*!< Enough BDs in the free state for the request? */
    if (RingPtr->PreCnt < NumBd)
        return -ER_FAULT;

    /*!< Set the return argument and move FreeHead backward */
    XEMACPS_RING_SEEKBACK(RingPtr, (RingPtr->FreeHead), NumBd);
    RingPtr->FreeCnt += NumBd;
    RingPtr->PreCnt -= NumBd;

    return ER_NORMAL;
}

kint32_t XEmacPs_BdRingFree(XEmacPs_BdRing * RingPtr, kuint32_t NumBd, XEmacPs_Bd * BdSetPtr)
{
    /*!< if no bds to process, simply return. */
    if (0U == NumBd)
        return -ER_UNVALID;

    /*!< Make sure we are in sync with XEmacPs_BdRingFromHw() */
    if ((RingPtr->PostCnt < NumBd) || (RingPtr->PostHead != BdSetPtr))
        return -ER_ERROR;
    
    /*!< Update pointers and counters */
    RingPtr->FreeCnt += NumBd;
    RingPtr->PostCnt -= NumBd;
    XEMACPS_RING_SEEKAHEAD(RingPtr, RingPtr->PostHead, NumBd);

    return ER_NORMAL;
}

kuint32_t XEmacPs_BdRingFromHwTx(XEmacPs_BdRing * RingPtr, kuint32_t BdLimit, XEmacPs_Bd ** BdSetPtr)
{
    XEmacPs_Bd *CurBdPtr;
    kuint32_t BdStr = 0U;
    kuint32_t BdCount;
    kuint32_t BdPartialCount;
    kuint32_t Sop = 0U;
    kuint32_t BdLimitLoc = BdLimit;

    CurBdPtr = RingPtr->HwHead;
    BdCount = 0U;
    BdPartialCount = 0U;

    /*!< If no BDs in work group, then there's nothing to search */
    if (RingPtr->HwCnt == 0x00000000U) 
        goto fail;

    if (BdLimitLoc > RingPtr->HwCnt)
        BdLimitLoc = RingPtr->HwCnt;

    /*!< 
     * Starting at HwHead, keep moving forward in the list until:
     *  - A BD is encountered with its new/used bit set which means
     *    hardware has not completed processing of that BD.
     *  - RingPtr->HwTail is reached and RingPtr->HwCnt is reached.
     *  - The number of requested BDs has been processed
     */
    while (BdCount < BdLimitLoc) 
    {
        /*!< Read the status */
        if (CurBdPtr != mrt_nullptr)
            BdStr = XEmacPs_BdRead(CurBdPtr, XEMACPS_BD_STAT_OFFSET);

        if ((Sop == 0x00000000U) && ((BdStr & XEMACPS_TXBUF_USED_MASK)!=0x00000000U))
            Sop = 1U;

        if (Sop == 0x00000001U) 
        {
            BdCount++;
            BdPartialCount++;
        }

        /*!< 
         * hardware has processed this BD so check the "last" bit.
         * If it is clear, then there are more BDs for the current
         * packet. Keep a count of these partial packet BDs.
         */
        if ((Sop == 0x00000001U) && ((BdStr & XEMACPS_TXBUF_LAST_MASK)!=0x00000000U)) 
        {
            Sop = 0U;
            BdPartialCount = 0U;
        }

        /*!< Move on to next BD in work group */
        CurBdPtr = XEmacPs_BdRingNext(RingPtr, CurBdPtr);
    }

    /*!< Subtract off any partial packet BDs found */
    BdCount -= BdPartialCount;

    /*!< 
     * If BdCount is non-zero then BDs were found to return. Set return
     * parameters, update pointers and counters, return success
     */
    if (BdCount > 0x00000000U) 
    {
        *BdSetPtr = RingPtr->HwHead;
        RingPtr->HwCnt -= BdCount;
        RingPtr->PostCnt += BdCount;
        XEMACPS_RING_SEEKAHEAD(RingPtr, RingPtr->HwHead, BdCount);

        return BdCount;
    }

fail:
    *BdSetPtr = mrt_nullptr;
    return 0;
}

kuint32_t XEmacPs_BdRingFromHwRx(XEmacPs_BdRing * RingPtr, kuint32_t BdLimit, XEmacPs_Bd ** BdSetPtr)
{
    XEmacPs_Bd *CurBdPtr;
    kuint32_t BdStr = 0U;
    kuint32_t BdCount;
    kuint32_t BdPartialCount;

    CurBdPtr = RingPtr->HwHead;
    BdCount = 0U;
    BdPartialCount = 0U;

    /*!< If no BDs in work group, then there's nothing to search */
    if (RingPtr->HwCnt == 0x00000000U)
        goto fail;

    /*!< 
     * Starting at HwHead, keep moving forward in the list until:
     *  - A BD is encountered with its new/used bit set which means
     *    hardware has completed processing of that BD.
     *  - RingPtr->HwTail is reached and RingPtr->HwCnt is reached.
     *  - The number of requested BDs has been processed
     */
    while (BdCount < BdLimit) 
    {
        /*!< Read the status */
        if (CurBdPtr != mrt_nullptr)
            BdStr = XEmacPs_BdRead(CurBdPtr, XEMACPS_BD_STAT_OFFSET);

        if (!XEmacPs_BdIsRxNew(CurBdPtr))
            break;

        BdCount++;

        /*!< hardware has processed this BD so check the "last" bit. If
         * it is clear, then there are more BDs for the current packet.
         * Keep a count of these partial packet BDs.
         */
        if ((BdStr & XEMACPS_RXBUF_EOF_MASK)!=0x00000000U)
            BdPartialCount = 0U;
        else
            BdPartialCount++;

        /*!< Move on to next BD in work group */
        CurBdPtr = XEmacPs_BdRingNext(RingPtr, CurBdPtr);
    }

    /*!< Subtract off any partial packet BDs found */
    BdCount -= BdPartialCount;

    /*!< 
     * If BdCount is non-zero then BDs were found to return. Set return
     * parameters, update pointers and counters, return success
     */
    if (BdCount > 0x00000000U) 
    {
        *BdSetPtr = RingPtr->HwHead;
        RingPtr->HwCnt -= BdCount;
        RingPtr->PostCnt += BdCount;
        XEMACPS_RING_SEEKAHEAD(RingPtr, RingPtr->HwHead, BdCount);

        return BdCount;
    }

fail:
    *BdSetPtr = mrt_nullptr;
    return 0;
}

kint32_t XEmacPs_BdRingToHw(XEmacPs_BdRing * RingPtr, kuint32_t NumBd, XEmacPs_Bd * BdSetPtr)
{
    XEmacPs_Bd *CurBdPtr;
    kuint32_t i;

    /*!< if no bds to process, simply return. */
    if (0U == NumBd)
        return -ER_UNVALID;
 
    /*!< Make sure we are in sync with XEmacPs_BdRingAlloc() */
    if ((RingPtr->PreCnt < NumBd) || (RingPtr->PreHead != BdSetPtr))
        return -ER_ERROR;

    CurBdPtr = BdSetPtr;
    for (i = 0U; i < NumBd; i++)
        CurBdPtr = (XEmacPs_Bd *)((void *)XEmacPs_BdRingNext(RingPtr, CurBdPtr));

    /*!< Adjust ring pointers & counters */
    XEMACPS_RING_SEEKAHEAD(RingPtr, RingPtr->PreHead, NumBd);
    RingPtr->PreCnt -= NumBd;
    RingPtr->HwTail = CurBdPtr;
    RingPtr->HwCnt += NumBd;

    return ER_NORMAL;
}

void XEmacPsIf_DmaTxDescsClean(void *args)
{
    struct xemac_s *xemac = (struct xemac_s *)args;
    XEmacPs_Bd bdtemplate;
    XEmacPs_BdRing *txringptr;
    xemacpsif_s *xemacpsif = (xemacpsif_s *)(xemac->state);

    txringptr = &XEmacPs_GetTxRing(&xemacpsif->sgrt_emacps);

    XEmacPs_BdClear(&bdtemplate);
    XEmacPs_BdSetStatus(&bdtemplate, XEMACPS_TXBUF_USED_MASK);

    /*!< Create the TxBD ring */
    XEmacPs_BdRingCreate(txringptr, (kuint32_t)xemacpsif->tx_bdspace,
            (kuint32_t) xemacpsif->tx_bdspace, BD_ALIGNMENT, XNET_CONFIG_N_TX_DESC);
    XEmacPs_BdRingClone(txringptr, &bdtemplate, XEMACPS_SEND);
}

kint32_t XEmacPs_DmaInit(void *args)
{
    struct xemac_s *xemac;
    XEmacPs_Bd bdtemplate;
    XEmacPs_BdRing *rxringptr, *txringptr;
    XEmacPs_Bd *rxbd;
    struct pbuf *p;
    kint32_t status;
    kint32_t i;
    kuint32_t bdindex;
    volatile kuint32_t tempaddress;
    kuint32_t index;
    kuint32_t gigeversion;
    XEmacPs_Bd *bdtxterminate = NULL;
    XEmacPs_Bd *bdrxterminate = NULL;
    kuint32_t *temp;

    xemac = (struct xemac_s *)args;
    xemacpsif_s *xemacpsif = (xemacpsif_s *)(xemac->state);
//  struct xtopology_t *xtopologyp = &xtopology[xemac->topology_index];

    index = 0;
    gigeversion = ((XEmacPs_ReadReg(xemacpsif->sgrt_emacps.Config.BaseAddress, 0xFC)) >> 16) & 0xFFF;

    /*
     * The BDs need to be allocated in uncached memory. Hence the 1 MB
     * address range allocated for Bd_Space is made uncached
     * by setting appropriate attributes in the translation table.
     * The Bd_Space is aligned to 1MB and has a size of 1 MB. This ensures
     * a reserved uncached area used only for BDs.
     */
    if (bd_space_attr_set == 0) 
    {
        // MMU
//		Xil_SetTlbAttributes((kint32_t)bd_space, DEVICE_MEMORY); // addr, attr
        bd_space_attr_set = 1;
    }

    rxringptr = &XEmacPs_GetRxRing(&xemacpsif->sgrt_emacps);
    txringptr = &XEmacPs_GetTxRing(&xemacpsif->sgrt_emacps);
    print_debug("rxringptr: %x\r\n", rxringptr);
    print_debug("txringptr: %x\r\n", txringptr);

    /*!< Allocate 64k for Rx and Tx bds each to take care of extreme cases */
    tempaddress = (kuint32_t)&(bd_space[bd_space_index]);
    xemacpsif->rx_bdspace = (void *)tempaddress;
    bd_space_index += 0x10000;
    tempaddress = (kuint32_t)&(bd_space[bd_space_index]);
    xemacpsif->tx_bdspace = (void *)tempaddress;
    bd_space_index += 0x10000;

    if (gigeversion > 2) 
    {
        tempaddress = (kuint32_t)&(bd_space[bd_space_index]);
        bdrxterminate = (XEmacPs_Bd *)tempaddress;
        bd_space_index += 0x10000;
        tempaddress = (kuint32_t)&(bd_space[bd_space_index]);
        bdtxterminate = (XEmacPs_Bd *)tempaddress;
        bd_space_index += 0x10000;
    }

    print_debug("rx_bdspace: %p \r\n", xemacpsif->rx_bdspace);
    print_debug("tx_bdspace: %p \r\n", xemacpsif->tx_bdspace);

    if (!xemacpsif->rx_bdspace || !xemacpsif->tx_bdspace) 
    {
        print_debug("%s: %d: Error: Unable to allocate memory for TX/RX buffer descriptors",
                __FUNCTION__, __LINE__);
        return -ER_NOMEM;
    }

    /*!<
     * Setup RxBD space.
     *
     * Setup a BD template for the Rx channel. This template will be copied to
     * every RxBD. We will not have to explicitly set these again.
     */
    XEmacPs_BdClear(&bdtemplate);

    /*!< Create the RxBD ring */
    status = XEmacPs_BdRingCreate(rxringptr, (kuint32_t) xemacpsif->rx_bdspace,
                (kuint32_t)xemacpsif->rx_bdspace, BD_ALIGNMENT, XNET_CONFIG_N_RX_DESC);
    if (status) {
        print_debug("Error setting up RxBD space\r\n");
        return -ER_ERROR;
    }

    status = XEmacPs_BdRingClone(rxringptr, &bdtemplate, XEMACPS_RECV);
    if (status) {
        print_debug("Error initializing RxBD space\r\n");
        return -ER_ERROR;
    }

    XEmacPs_BdClear(&bdtemplate);
    XEmacPs_BdSetStatus(&bdtemplate, XEMACPS_TXBUF_USED_MASK);

    /*!< Create the TxBD ring */
    status = XEmacPs_BdRingCreate(txringptr, (kuint32_t) xemacpsif->tx_bdspace,
                (kuint32_t)xemacpsif->tx_bdspace, BD_ALIGNMENT, XNET_CONFIG_N_TX_DESC);
    if (status)
        return -ER_FAILD;

    /*!< We reuse the bd template, as the same one will work for both rx and tx. */
    status = XEmacPs_BdRingClone(txringptr, &bdtemplate, XEMACPS_SEND);
    if (status)
        return -ER_FAILD;

    /*!< Allocate RX descriptors, 1 RxBD at a time. */
    for (i = 0; i < XNET_CONFIG_N_RX_DESC; i++) 
    {
        p = pbuf_alloc(PBUF_RAW, XEMACPS_MAX_FRAME_SIZE, PBUF_POOL);
        if (!p) 
        {
            print_debug("unable to alloc pbuf in XEmacPs_DmaInit\r\n");
            return -ER_NOMEM;
        }

        status = XEmacPs_BdRingAlloc(rxringptr, 1, &rxbd);
        if (status) 
        {
            print_debug("XEmacPs_DmaInit: Error allocating RxBD\r\n");
            pbuf_free(p);

            return -ER_NOMEM;
        }

        /*!< Enqueue to HW */
        status = XEmacPs_BdRingToHw(rxringptr, 1, rxbd);
        if (status) 
        {
            print_debug("Error: committing RxBD to HW\r\n");
            pbuf_free(p);
            XEmacPs_BdRingUnAlloc(rxringptr, 1, rxbd);

            return -ER_FAILD;
        }

        bdindex = XEMACPS_BD_TO_INDEX(rxringptr, rxbd);
        temp = (kuint32_t *)rxbd;
        *temp = 0;

        if (bdindex == (XNET_CONFIG_N_RX_DESC - 1))
            *temp = 0x00000002;

        temp++;
        *temp = 0;
        mrt_dsb();

        if (xemacpsif->sgrt_emacps.Config.IsCacheCoherent == 0)
            Xil_DCacheInvalidateRange((kuint32_t)p->payload, (kuint32_t)XEMACPS_MAX_FRAME_SIZE);

        XEmacPs_BdSetAddressRx(rxbd, (kuint32_t)p->payload);
        rx_pbufs_storage[index + bdindex] = (kuint32_t)p;
    }

    XEmacPs_SetQueuePtr(&(xemacpsif->sgrt_emacps), xemacpsif->sgrt_emacps.RxBdRing.BaseBdAddr, 0, XEMACPS_RECV);
    if (gigeversion > 2)
        XEmacPs_SetQueuePtr(&(xemacpsif->sgrt_emacps), xemacpsif->sgrt_emacps.TxBdRing.BaseBdAddr, 1, XEMACPS_SEND);
    else
        XEmacPs_SetQueuePtr(&(xemacpsif->sgrt_emacps), xemacpsif->sgrt_emacps.TxBdRing.BaseBdAddr, 0, XEMACPS_SEND);

    if (gigeversion > 2)
    {
        /*!<
         * This version of GEM supports priority queuing and the current
         * dirver is using tx priority queue 1 and normal rx queue for
         * packet transmit and receive. The below code ensure that the
         * other queue pointers are parked to known state for avoiding
         * the controller to malfunction by fetching the descriptors
         * from these queues.
         */
        XEmacPs_BdClear(bdrxterminate);
        XEmacPs_BdSetAddressRx(bdrxterminate, (XEMACPS_RXBUF_NEW_MASK | XEMACPS_RXBUF_WRAP_MASK));
        XEmacPs_WriteReg(xemacpsif->sgrt_emacps.Config.BaseAddress, XEMACPS_RXQ1BASE_OFFSET, (kuint32_t)bdrxterminate);

        XEmacPs_BdClear(bdtxterminate);
        XEmacPs_BdSetStatus(bdtxterminate, (XEMACPS_TXBUF_USED_MASK | XEMACPS_TXBUF_WRAP_MASK));
        XEmacPs_WriteReg(xemacpsif->sgrt_emacps.Config.BaseAddress, XEMACPS_TXQBASE_OFFSET, (kuint32_t)bdtxterminate);
    }

    /*
     * Connect the device driver handler that will be called when an
     * interrupt for the device occurs, the handler defined above performs
     * the specific interrupt processing for the device.
     */
//	XScuGic_RegisterHandler(XPAR_SCUGIC_0_CPU_BASEADDR, xtopologyp->scugic_emac_intr,
//				(Xil_ExceptionHandler)XEmacPs_IntrHandler, (void *)&xemacpsif->sgrt_emacps);
    /*
     * Enable the interrupt for emacps.
     */
//	XScuGic_EnableIntr(XPAR_SCUGIC_0_DIST_BASEADDR, (kuint32_t) xtopologyp->scugic_emac_intr);
//	emac_intr_num = (kuint32_t) xtopologyp->scugic_emac_intr;

    return ER_NORMAL;
}

/*!< -------------------------------------------------------------------- */
void XEmacPsIf_SentBds(xemacpsif_s *xemacpsif, XEmacPs_BdRing *txring)
{
    XEmacPs_Bd *txbdset;
    XEmacPs_Bd *curbdpntr;
    kint32_t n_bds;
    kint32_t status;
    kint32_t n_pbufs_freed = 0;
    kuint32_t bdindex;
    struct pbuf *p;
    kuint32_t *temp;
    kuint32_t index = 0;

    if (xemacpsif->sgrt_emacps.Config.BaseAddress == XPAR_XEMACPS_0_BASEADDR)
        index = 0;

    while (1) 
    {
        /*!< obtain processed BD's */
        n_bds = XEmacPs_BdRingFromHwTx(txring,
                                XNET_CONFIG_N_TX_DESC, &txbdset);
        if (n_bds == 0)
            return;

        /*!< free the processed BD's */
        n_pbufs_freed = n_bds;
        curbdpntr = txbdset;

        while (n_pbufs_freed > 0) 
        {
            bdindex = XEMACPS_BD_TO_INDEX(txring, curbdpntr);
            temp = (kuint32_t *)curbdpntr;
            *temp = 0;
            temp++;

            if (bdindex == (XNET_CONFIG_N_TX_DESC - 1))
                *temp = 0xC0000000;
            else
                *temp = 0x80000000;

            mrt_dsb();
            p = (struct pbuf *)tx_pbufs_storage[index + bdindex];
            if (p != NULL)
                pbuf_free(p);
            
            tx_pbufs_storage[index + bdindex] = 0;
            curbdpntr = XEmacPs_BdRingNext(txring, curbdpntr);
            n_pbufs_freed--;
            mrt_dsb();
        }

        status = XEmacPs_BdRingFree(txring, n_bds, txbdset);
        if (status)
            print_debug("Failure while freeing in Tx Done ISR\r\n");
    }
}

kint32_t XEmacPsIf_SgSend(xemacpsif_s *xemacpsif, struct pbuf *p)
{
    struct pbuf *q;
    kint32_t n_pbufs;
    XEmacPs_Bd *txbdset, *txbd, *last_txbd = mrt_nullptr;
    XEmacPs_Bd *temp_txbd;
    kint32_t status;
    XEmacPs_BdRing *txring;
    kuint32_t bdindex;
    kuint32_t lev;
    kuint32_t index;
    kuint32_t max_fr_size;
    kuint32_t reg;

    lev = __get_cpsr();
    __set_cpsr(lev | 0x000000C0);

    txring = &(XEmacPs_GetTxRing(&xemacpsif->sgrt_emacps));

    if (xemacpsif->sgrt_emacps.Config.BaseAddress == XPAR_XEMACPS_0_BASEADDR)
        index = 0;

    /*!< first count the number of pbufs */
    for (q = p, n_pbufs = 0; q != NULL; q = q->next)
        n_pbufs++;

    /*!< obtain as many BD's */
    status = XEmacPs_BdRingAlloc(txring, n_pbufs, &txbdset);
    if (status) 
    {
        print_debug("sgsend: Error allocating TxBD\r\n");
        goto END;
    }

    for (q = p, txbd = txbdset; q != NULL; q = q->next) 
    {
        bdindex = XEMACPS_BD_TO_INDEX(txring, txbd);
        if (tx_pbufs_storage[index + bdindex] != 0) 
        {
            print_debug("PBUFS not available\r\n");
            status = -ER_EXISTED;
            goto END;
        }

        /*!< Send the data from the pbuf to the interface, one pbuf at a
           time. The size of the data in each pbuf is kept in the ->len
           variable. */
        if (xemacpsif->sgrt_emacps.Config.IsCacheCoherent == 0)
            Xil_DCacheFlushRange((kuint32_t)q->payload, (kuint32_t)q->len);

        XEmacPs_BdSetAddressTx(txbd, (kuint32_t)q->payload);

        max_fr_size = XEMACPS_MAX_FRAME_SIZE - 18;
        if (q->len > max_fr_size)
            XEmacPs_BdSetLength(txbd, max_fr_size & 0x3FFF);
        else
            XEmacPs_BdSetLength(txbd, q->len & 0x3FFF);

        tx_pbufs_storage[index + bdindex] = (kuint32_t)q;

        pbuf_ref(q);
        last_txbd = txbd;
        XEmacPs_BdClearLast(txbd);
        txbd = XEmacPs_BdRingNext(txring, txbd);
    }

    XEmacPs_BdSetLast(last_txbd);

    /*!< 
     * For fragmented packets, remember the 1st BD allocated for the 1st
     * packet fragment. The used bit for this BD should be cleared at the end
     * after clearing out used bits for other fragments. For packets without
     * just remember the allocated BD. 
     */
    temp_txbd = txbdset;
    txbd = txbdset;
    txbd = XEmacPs_BdRingNext(txring, txbd);

    q = p->next;
    for (; q != NULL; q = q->next) 
    {
        XEmacPs_BdClearTxUsed(txbd);
        mrt_dsb();
        txbd = XEmacPs_BdRingNext(txring, txbd);
    }

    XEmacPs_BdClearTxUsed(temp_txbd);
    mrt_dsb();

    status = XEmacPs_BdRingToHw(txring, n_pbufs, txbdset);
    if (status) 
    {
        print_debug("sgsend: Error submitting TxBD\r\n");
        goto END;
    }

    /*!< Start transmit */
    reg  = XEmacPs_ReadReg(xemacpsif->sgrt_emacps.Config.BaseAddress, XEMACPS_NWCTRL_OFFSET);
    reg |= XEMACPS_NWCTRL_STARTTX_MASK;
    XEmacPs_WriteReg(xemacpsif->sgrt_emacps.Config.BaseAddress, XEMACPS_NWCTRL_OFFSET, reg);

END:
    __set_cpsr(lev);
    return ER_NORMAL;
}

void XEmacPsIf_ResetRx_WithNoRxData(xemacpsif_s *xemacpsif)
{
    kuint32_t regctrl;
    kuint32_t tempcntr;
    kuint32_t gigeversion;

    gigeversion = ((XEmacPs_ReadReg(xemacpsif->sgrt_emacps.Config.BaseAddress, 0xFC)) >> 16) & 0xFFF;
    if (gigeversion != 2) 
        return;
    
    tempcntr = XEmacPs_ReadReg(xemacpsif->sgrt_emacps.Config.BaseAddress, XEMACPS_RXCNT_OFFSET);
    if ((!tempcntr) && (!(xemacpsif->last_rx_frms_cntr))) 
    {
        regctrl = XEmacPs_ReadReg(xemacpsif->sgrt_emacps.Config.BaseAddress, XEMACPS_NWCTRL_OFFSET);
        regctrl &= (~XEMACPS_NWCTRL_RXEN_MASK);
        XEmacPs_WriteReg(xemacpsif->sgrt_emacps.Config.BaseAddress, XEMACPS_NWCTRL_OFFSET, regctrl);

        regctrl = XEmacPs_ReadReg(xemacpsif->sgrt_emacps.Config.BaseAddress, XEMACPS_NWCTRL_OFFSET);
        regctrl |= (XEMACPS_NWCTRL_RXEN_MASK);
        XEmacPs_WriteReg(xemacpsif->sgrt_emacps.Config.BaseAddress, XEMACPS_NWCTRL_OFFSET, regctrl);
    }

    xemacpsif->last_rx_frms_cntr = tempcntr;
}

void XEmacPsIf_SetupRxBds(xemacpsif_s *xemacpsif, XEmacPs_BdRing *rxring)
{
    XEmacPs_Bd *rxbd;
    kint32_t status;
    struct pbuf *p;
    kuint32_t freebds;
    kuint32_t bdindex;
    kuint32_t *temp;
    kuint32_t index = 0;

    if (XEMACPS_IS_ETH_0(&xemacpsif->sgrt_emacps))
        index = 0;

    freebds = XEmacPs_BdRingGetFreeCnt(rxring);
    while (freebds > 0) 
    {
        freebds--;

        p = pbuf_alloc(PBUF_RAW, XEMACPS_MAX_FRAME_SIZE, PBUF_POOL);
        if (!p) 
        {
            print_debug("unable to alloc pbuf in recv_handler\r\n");
            return;
        }

        status = XEmacPs_BdRingAlloc(rxring, 1, &rxbd);
        if (status) 
        {
            print_debug("XEmacPsIf_SetupRxBds: Error allocating RxBD\r\n");
            pbuf_free(p);
            return;
        }

        status = XEmacPs_BdRingToHw(rxring, 1, rxbd);
        if (status) 
        {
            print_debug("Error committing RxBD to hardware: ");
            if (status == (-ER_ERROR))
                print_debug("XST_DMA_SG_LIST_ERROR: this function was called out of sequence with XEmacPs_BdRingAlloc()\r\n");
            else
                print_debug("set of BDs was rejected because the first BD did not have its start-of-packet bit set, or the last BD did not have its end-of-packet bit set, or any one of the BD set has 0 as length value\r\n");

            pbuf_free(p);
            XEmacPs_BdRingUnAlloc(rxring, 1, rxbd);
            return;
        }

        if (!xemacpsif->sgrt_emacps.Config.IsCacheCoherent)
            Xil_DCacheInvalidateRange((kuint32_t)p->payload, (kuint32_t)XEMACPS_MAX_FRAME_SIZE);

        bdindex = XEMACPS_BD_TO_INDEX(rxring, rxbd);
        temp = (kuint32_t *)rxbd;
        if (bdindex == (XNET_CONFIG_N_RX_DESC - 1))
            *temp = 0x00000002;
        else
            *temp = 0;

        temp++;
        *temp = 0;
        mrt_dsb();

        XEmacPs_BdSetAddressRx(rxbd, (kuint32_t)p->payload);
        rx_pbufs_storage[index + bdindex] = (kuint32_t)p;
    }
}

void XEmacPsIf_TxRxBuffer_Free(xemacpsif_s *xemacpsif)
{
    kint32_t index;
    kint32_t index1 = 0;
    struct pbuf *p;

    if (XEMACPS_IS_ETH_0(&xemacpsif->sgrt_emacps))
        index1 = 0;

    for (index = index1; index < (index1 + XNET_CONFIG_N_TX_DESC); index++) 
    {
        if (tx_pbufs_storage[index] != 0) 
        {
            p = (struct pbuf *)tx_pbufs_storage[index];
            pbuf_free(p);
            tx_pbufs_storage[index] = 0;
        }
    }

    for (index = index1; index < (index1 + XNET_CONFIG_N_TX_DESC); index++) 
    {
        p = (struct pbuf *)rx_pbufs_storage[index];
        pbuf_free(p);
    }
}

void XEmacPsIf_TxBuffer_Free(xemacpsif_s *xemacpsif)
{
    kint32_t index;
    kint32_t index1;
    struct pbuf *p;

    if (XEMACPS_IS_ETH_0(&xemacpsif->sgrt_emacps))
        index1 = 0;

    for (index = index1; index < (index1 + XNET_CONFIG_N_TX_DESC); index++) 
    {
        if (tx_pbufs_storage[index] != 0) 
        {
            p = (struct pbuf *)tx_pbufs_storage[index];
            pbuf_free(p);
            tx_pbufs_storage[index] = 0;
        }
    }
}

void XEmacPsIf_Init_OnError(xemacpsif_s *xemacps)
{
    XEmacPs *xemacpsp;
    struct netif *netif;
    kint32_t status;

    xemacpsp = &xemacps->sgrt_emacps;
    netif = sprt_zynq7_netif;

    /*!< set mac address */
    status = XEmacPs_SetMacAddress(xemacpsp, (void*)(netif->hwaddr), 1);
    if (status)
        print_debug("In %s:Emac Mac Address set failed...\r\n",__func__);

    XEmacPs_SetOperatingSpeed(xemacpsp, link_speed);

    /*!< Setting the operating speed of the MAC needs a delay. */
    delay_s(2);
}

void XEmacPsIf_HandleError(void *args)
{
    xemacpsif_s *xemacpsif;
    struct xemac_s *xemac = (struct xemac_s *)args;
    kint32_t status;
    kuint32_t dmacrreg;

    SYS_ARCH_DECL_PROTECT(lev);
    SYS_ARCH_PROTECT(lev);

    xemacpsif = (xemacpsif_s *)(xemac->state);
    XEmacPsIf_TxRxBuffer_Free(xemacpsif);

    status = XEmacPs_CfgInitialize(&xemacpsif->sgrt_emacps, sprt_zynq7_machconfig, sprt_zynq7_machconfig->BaseAddress);
    if (status)
        print_debug("In %s:EmacPs Configuration Failed....\r\n", __func__);
    
    /*!< initialize the mac */
    XEmacPsIf_Init_OnError(xemacpsif);

    dmacrreg = XEmacPs_ReadReg(xemacpsif->sgrt_emacps.Config.BaseAddress, XEMACPS_DMACR_OFFSET);
    dmacrreg = dmacrreg | (0x01000000);
    XEmacPs_WriteReg(xemacpsif->sgrt_emacps.Config.BaseAddress, XEMACPS_DMACR_OFFSET, dmacrreg);

    XEmacPsIf_SetupIsr(xemac);
    XEmacPs_DmaInit(xemac);
    XEmacPsIf_Start(xemacpsif);

    SYS_ARCH_UNPROTECT(lev);
}

void XEmacPsIf_HandleTxErrors(void *args)
{
    xemacpsif_s *xemacpsif;
    struct xemac_s *xemac = (struct xemac_s *)args;
    kuint32_t netctrlreg;

    SYS_ARCH_DECL_PROTECT(lev);
    SYS_ARCH_PROTECT(lev);

    xemacpsif = (xemacpsif_s *)(xemac->state);

    netctrlreg = XEmacPs_ReadReg(xemacpsif->sgrt_emacps.Config.BaseAddress, XEMACPS_NWCTRL_OFFSET);
    netctrlreg = netctrlreg & (~XEMACPS_NWCTRL_TXEN_MASK);
    XEmacPs_WriteReg(xemacpsif->sgrt_emacps.Config.BaseAddress, XEMACPS_NWCTRL_OFFSET, netctrlreg);

    XEmacPsIf_TxBuffer_Free(xemacpsif);
    XEmacPsIf_DmaTxDescsClean(xemac);

    netctrlreg = XEmacPs_ReadReg(xemacpsif->sgrt_emacps.Config.BaseAddress, XEMACPS_NWCTRL_OFFSET);
    netctrlreg = netctrlreg | (XEMACPS_NWCTRL_TXEN_MASK);
    XEmacPs_WriteReg(xemacpsif->sgrt_emacps.Config.BaseAddress, XEMACPS_NWCTRL_OFFSET, netctrlreg);

    SYS_ARCH_UNPROTECT(lev);
}

void XEmacPsIf_SendHandler(void *arg)
{
    struct xemac_s *xemac;
    xemacpsif_s   *xemacpsif;
    XEmacPs_BdRing *txringptr;
    kuint32_t regval;

    xemac = (struct xemac_s *)(arg);
    xemacpsif = (xemacpsif_s *)(xemac->state);
    txringptr = &(XEmacPs_GetTxRing(&xemacpsif->sgrt_emacps));

    regval = XEmacPs_ReadReg(xemacpsif->sgrt_emacps.Config.BaseAddress, XEMACPS_TXSR_OFFSET);
    XEmacPs_WriteReg(xemacpsif->sgrt_emacps.Config.BaseAddress,XEMACPS_TXSR_OFFSET, regval);

    /*!< If Transmit done interrupt is asserted, process completed BD's */
    XEmacPsIf_SentBds(xemacpsif, txringptr);
}

void XEmacPsIf_RecvHandler(void *arg)
{
    struct pbuf *p;
    XEmacPs_Bd *rxbdset, *curbdptr;
    struct xemac_s *xemac;
    xemacpsif_s *xemacpsif;
    XEmacPs_BdRing *rxring;
    volatile kint32_t bd_processed;
    kint32_t rx_bytes, k;
    kuint32_t bdindex;
    kuint32_t regval;
    kuint32_t index = 0;
    kuint32_t gigeversion;

    xemac = (struct xemac_s *)(arg);
    xemacpsif = (xemacpsif_s *)(xemac->state);
    rxring = &XEmacPs_GetRxRing(&xemacpsif->sgrt_emacps);

    if (XEMACPS_IS_ETH_0(&xemacpsif->sgrt_emacps))
        index = 0;

    /*
     * If Reception done interrupt is asserted, call RX call back function
     * to handle the processed BDs and then raise the according flag.
     */
    regval = XEmacPs_ReadReg(xemacpsif->sgrt_emacps.Config.BaseAddress, XEMACPS_RXSR_OFFSET);
    XEmacPs_WriteReg(xemacpsif->sgrt_emacps.Config.BaseAddress, XEMACPS_RXSR_OFFSET, regval);

    gigeversion = ((XEmacPs_ReadReg(xemacpsif->sgrt_emacps.Config.BaseAddress, 0xFC)) >> 16) & 0xFFF;
    if (gigeversion <= 2)
        XEmacPsIf_ResetRx_WithNoRxData(xemacpsif);

    while (1) 
    {
        bd_processed = XEmacPs_BdRingFromHwRx(rxring, XNET_CONFIG_N_RX_DESC, &rxbdset);
        if (bd_processed <= 0)
            break;

        for (k = 0, curbdptr=rxbdset; k < bd_processed; k++) 
        {
            bdindex = XEMACPS_BD_TO_INDEX(rxring, curbdptr);
            p = (struct pbuf *)rx_pbufs_storage[index + bdindex];

            /*!< Adjust the buffer size to the actual number of bytes received. */
            rx_bytes = XEmacPs_BdGetLength(curbdptr);
            pbuf_realloc(p, rx_bytes);

            /*!<
             *  store it in the receive queue,
             * where it'll be processed by a different handler
             */
            if (pq_enqueue(xemacpsif->sprt_rxq, (void*)p) < 0)
                pbuf_free(p);

            curbdptr = XEmacPs_BdRingNext( rxring, curbdptr);
        }

        /*!< free up the BD's */
        XEmacPs_BdRingFree(rxring, bd_processed, rxbdset);
        XEmacPsIf_SetupRxBds(xemacpsif, rxring);
    }
}

void XEmacPsIf_ErrorHandler(void *arg, kuint8_t Direction, kuint32_t ErrorWord)
{
    struct xemac_s *xemac;
    xemacpsif_s   *xemacpsif;
    XEmacPs_BdRing *rxring;
    XEmacPs_BdRing *txring;

    xemac = (struct xemac_s *)(arg);
    xemacpsif = (xemacpsif_s *)(xemac->state);
    rxring = &XEmacPs_GetRxRing(&xemacpsif->sgrt_emacps);
    txring = &XEmacPs_GetTxRing(&xemacpsif->sgrt_emacps);

    if (ErrorWord == 0) 
        return;

    switch (Direction) 
    {
        case XEMACPS_RECV:
            if (ErrorWord & XEMACPS_RXSR_HRESPNOK_MASK) 
            {
                print_debug("Receive DMA error\r\n");
                XEmacPsIf_HandleError(xemac);
            }
            if (ErrorWord & XEMACPS_RXSR_RXOVR_MASK) 
            {
                print_debug("Receive over run\r\n");
                XEmacPsIf_RecvHandler(arg);
                XEmacPsIf_SetupRxBds(xemacpsif, rxring);
            }
            if (ErrorWord & XEMACPS_RXSR_BUFFNA_MASK) 
            {
                print_debug("Receive buffer not available\r\n");
                XEmacPsIf_RecvHandler(arg);
                XEmacPsIf_SetupRxBds(xemacpsif, rxring);
            }
            break;

        case XEMACPS_SEND:
            if (ErrorWord & XEMACPS_TXSR_HRESPNOK_MASK) 
            {
                print_debug("Transmit DMA error\r\n");
                XEmacPsIf_HandleError(xemac);
            }
            if (ErrorWord & XEMACPS_TXSR_URUN_MASK) 
            {
                print_debug("Transmit under run\r\n");
                XEmacPsIf_HandleTxErrors(xemac);
            }
            if (ErrorWord & XEMACPS_TXSR_BUFEXH_MASK) 
            {
                print_debug("Transmit buffer exhausted\r\n");
                XEmacPsIf_HandleTxErrors(xemac);
            }
            if (ErrorWord & XEMACPS_TXSR_RXOVR_MASK) 
            {
                print_debug("Transmit retry excessed limits\r\n");
                XEmacPsIf_HandleTxErrors(xemac);
            }
            if (ErrorWord & XEMACPS_TXSR_FRAMERX_MASK) 
            {
                print_debug("Transmit collision\r\n");
                XEmacPsIf_SentBds(xemacpsif, txring);
            }
            break;
    }
}

void XEmacPsIf_SetupIsr(void *args)
{
    struct xemac_s *xemac = (struct xemac_s *)args;
    XEmacPs *emacps = &((xemacpsif_s *)xemac->state)->sgrt_emacps;

    emacps->SendHandler = (XEmacPs_Handler)(void *)XEmacPsIf_SendHandler;
    emacps->RecvHandler = (XEmacPs_Handler)(void *)XEmacPsIf_RecvHandler;
    emacps->ErrorHandler = (XEmacPs_ErrHandler)(void *)XEmacPsIf_ErrorHandler;

    emacps->SendRef = emacps->RecvRef = emacps->ErrorRef = xemac;
}

void XEmacPs_IntrHandler(void *XEmacPsPtr)
{
    kuint32_t RegISR;
    kuint32_t RegSR;
    kuint32_t RegCtrl;
    kuint32_t RegQ1ISR = 0U;
    XEmacPs *sprt_emacps = (XEmacPs *)XEmacPsPtr;

    if ((!sprt_emacps) ||
        (!sprt_emacps->IsReady))
        return;

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

    /*!< Clear the interrupt status register */
    XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, XEMACPS_ISR_OFFSET, RegISR);

    /*!< Receive complete interrupt */
    if ((RegISR & XEMACPS_IXR_FRAMERX_MASK) != 0x00000000U) 
    {
        /*!< 
         * Clear RX status register RX complete indication but preserve
         * error bits if there is any */
        XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, XEMACPS_RXSR_OFFSET,
                   ((kuint32_t)XEMACPS_RXSR_FRAMERX_MASK | (kuint32_t)XEMACPS_RXSR_BUFFNA_MASK));
        sprt_emacps->RecvHandler(sprt_emacps->RecvRef);
    }

    /*!< Transmit Q1 complete interrupt */
    if ((sprt_emacps->Version > 2) &&
        ((RegQ1ISR & XEMACPS_INTQ1SR_TXCOMPL_MASK) != 0x00000000U)) 
    {
        /*!< Clear TX status register TX complete indication but preserve
         * error bits if there is any */
        XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, XEMACPS_INTQ1_STS_OFFSET, XEMACPS_INTQ1SR_TXCOMPL_MASK);
        XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, XEMACPS_TXSR_OFFSET,
                   ((kuint32_t)XEMACPS_TXSR_TXCOMPL_MASK | (kuint32_t)XEMACPS_TXSR_USEDREAD_MASK));
        sprt_emacps->SendHandler(sprt_emacps->SendRef);
    }

    /*!< Transmit complete interrupt */
    if ((RegISR & XEMACPS_IXR_TXCOMPL_MASK) != 0x00000000U) 
    {
        /*!< Clear TX status register TX complete indication but preserve
         * error bits if there is any */
        XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, XEMACPS_TXSR_OFFSET,
                   ((kuint32_t)XEMACPS_TXSR_TXCOMPL_MASK | (kuint32_t)XEMACPS_TXSR_USEDREAD_MASK));
        sprt_emacps->SendHandler(sprt_emacps->SendRef);
    }

    /*!< Receive error conditions interrupt */
    if ((RegISR & XEMACPS_IXR_RX_ERR_MASK) != 0x00000000U) 
    {
        /*!< Clear RX status register */
        RegSR = XEmacPs_ReadReg(sprt_emacps->Config.BaseAddress, XEMACPS_RXSR_OFFSET);
        XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, XEMACPS_RXSR_OFFSET, RegSR);

        /*!< 
         * Fix for CR # 692702. Write to bit 18 of net_ctrl
         * register to flush a packet out of Rx SRAM upon
         * an error for receive buffer not available. 
         */
        if ((RegISR & XEMACPS_IXR_RXUSED_MASK) != 0x00000000U) 
        {
            RegCtrl =
            XEmacPs_ReadReg(sprt_emacps->Config.BaseAddress, XEMACPS_NWCTRL_OFFSET);
            RegCtrl |= (kuint32_t)XEMACPS_NWCTRL_FLUSH_DPRAM_MASK;
            XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, XEMACPS_NWCTRL_OFFSET, RegCtrl);
        }

        if (RegSR != 0)
            sprt_emacps->ErrorHandler(sprt_emacps->ErrorRef, XEMACPS_RECV, RegSR);
    }

    /*!< 
     * When XEMACPS_IXR_TXCOMPL_MASK is flaged, XEMACPS_IXR_TXUSED_MASK
     * will be asserted the same time.
     * Have to distinguish this bit to handle the real error condition.
     */
    /*!< Transmit Q1 error conditions interrupt */
    if ((sprt_emacps->Version > 2) &&
        ((RegQ1ISR & XEMACPS_INTQ1SR_TXERR_MASK) != 0x00000000U) &&
        ((RegQ1ISR & XEMACPS_INTQ1SR_TXCOMPL_MASK) != 0x00000000U)) 
    {
        /*!< Clear Interrupt Q1 status register */
        XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, XEMACPS_INTQ1_STS_OFFSET, RegQ1ISR);
        sprt_emacps->ErrorHandler(sprt_emacps->ErrorRef, XEMACPS_SEND, RegQ1ISR);
    }

    /*!< Transmit error conditions interrupt */
    if (((RegISR & XEMACPS_IXR_TX_ERR_MASK) != 0x00000000U) &&
        (!(RegISR & XEMACPS_IXR_TXCOMPL_MASK) != 0x00000000U)) 
    {
        /*!< Clear TX status register */
        RegSR = XEmacPs_ReadReg(sprt_emacps->Config.BaseAddress, XEMACPS_TXSR_OFFSET);
        XEmacPs_WriteReg(sprt_emacps->Config.BaseAddress, XEMACPS_TXSR_OFFSET, RegSR);
        sprt_emacps->ErrorHandler(sprt_emacps->ErrorRef, XEMACPS_SEND, RegSR);
    }
}

void XEmacPsIf_Start(xemacpsif_s *xemacps)
{
    /*!< start the temac */
    XEmacPs_Start(&xemacps->sgrt_emacps);
}

kint32_t XEthernetPs_Initiailize(struct XEthernetPs *sprt_xethps, struct netif *netif)
{
    struct xemac_s *xemac;
    xemacpsif_s *xemacpsif;
    XEmacPs_Config *mac_config;
    kint32_t status;

    xemacpsif = mem_malloc(sizeof(*xemacpsif));
    if (xemacpsif == mrt_nullptr) 
        return -ER_NOMEM;

    xemac = mem_malloc(sizeof(*xemac));
    if (xemac == mrt_nullptr) 
        return -ER_NOMEM;

    sprt_xethps->xemac = xemac;
    sprt_xethps->xemacpsif = xemacpsif;

    xemac->state = (void *)xemacpsif;
    xemac->topology_index = 0;

    xemacpsif->sprt_txq = mrt_nullptr;
    xemacpsif->sprt_rxq = pq_queue_create(NR_PQ_DROP, 4096);
    if (!xemacpsif->sprt_rxq)
        return -ER_NOMEM;

    /*!< obtain config of this emac */
    mac_config = (XEmacPs_Config *)XEmacPs_LookupConfig(XPAR_PS7_ETHERNET_0_DEVICE_ID);
    status = XEmacPs_CfgInitialize(&xemacpsif->sgrt_emacps, mac_config, mac_config->BaseAddress);
    if (status)
        return status;

    sprt_zynq7_machconfig = mac_config;
    sprt_zynq7_netif = netif;
    
    /*!< initialize the mac */
    XEmacPs_Init(&xemacpsif->sgrt_emacps, (void *)netif->hwaddr);

    /*!< setisr */
    XEmacPsIf_SetupIsr(xemac);
    XEmacPs_DmaInit(xemac);
    XEmacPsIf_Start(xemacpsif);

    return ER_NORMAL;
}

void XEthernetPs_LinkDetect(struct netif *netif)
{
    kuint32_t link_speed, phy_link_status;
    struct xemac_s *xemac = (struct xemac_s *)(netif->state);

    xemacpsif_s *xemacs = (xemacpsif_s *)(xemac->state);
    XEmacPs *xemacp = &xemacs->sgrt_emacps;

    if ((!xemacp->IsReady) ||
        (eth_link_status == ETH_LINK_UNDEFINED))
        return;

    phy_link_status = XEmacPs_PhyLinkDetect(xemacp, phyaddrforemac);

    if ((eth_link_status == ETH_LINK_UP) && (!phy_link_status))
        eth_link_status = ETH_LINK_DOWN;

    switch (eth_link_status) 
    {
        case ETH_LINK_UNDEFINED:
        case ETH_LINK_UP:
            return;

        case ETH_LINK_DOWN:
            netif_set_link_down(netif);
            eth_link_status = ETH_LINK_NEGOTIATING;

            print_debug("Ethernet Link down\r\n");
            break;

        case ETH_LINK_NEGOTIATING:
            if (phy_link_status &&
                XEmacPs_PhyAutoNegStatus(xemacp, phyaddrforemac)) 
            {
                /*!< Initiate Phy setup to get link speed */
                link_speed = XEmacPs_PhySetup(xemacp, phyaddrforemac);
                XEmacPs_SetOperatingSpeed(xemacp, link_speed);

                netif_set_link_up(netif);
                eth_link_status = ETH_LINK_UP;

                print_debug("Ethernet Link up\r\n");
            }
            break;
    }
}

static struct pbuf *low_level_input(struct netif *netif)
{
    struct xemac_s *xemac = (struct xemac_s *)(netif->state);
    xemacpsif_s *xemacpsif = (xemacpsif_s *)(xemac->state);
    struct pbuf *p;

    /*!< see if there is data to process */
    if (pq_queue_get_size(xemacpsif->sprt_rxq) == 0)
        return NULL;

    /*!< return one packet from receive q */
    p = (struct pbuf *)pq_dequeue(xemacpsif->sprt_rxq);
    return p;
}

kint32_t is_tx_space_available(xemacpsif_s *emac)
{
    XEmacPs_BdRing *txring;
    kint32_t freecnt = 0;

    txring = &(XEmacPs_GetTxRing(&emac->sgrt_emacps));

    /*!< tx space is available as long as there are valid BD's */
    freecnt = XEmacPs_BdRingGetFreeCnt(txring);
    return freecnt;
}

static err_t _unbuffered_low_level_output(xemacpsif_s *xemacpsif, struct pbuf *p)
{
    kint32_t status = 0;

#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE);	/*!< drop the padding word */
#endif
    status = XEmacPsIf_SgSend(xemacpsif, p);
    if (status) 
    {}

#if ETH_PAD_SIZE
    pbuf_header(p, ETH_PAD_SIZE);	/*!< reclaim the padding word */
#endif

    return ERR_OK;

}

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    SYS_ARCH_DECL_PROTECT(lev);
    err_t err;
    kint32_t freecnt;
    XEmacPs_BdRing *txring;

    struct xemac_s *xemac = (struct xemac_s *)(netif->state);
    xemacpsif_s *xemacpsif = (xemacpsif_s *)(xemac->state);

    SYS_ARCH_PROTECT(lev);

    /*!< check if space is available to send */
    freecnt = is_tx_space_available(xemacpsif);
    if (freecnt <= 5) 
    {
        txring = &(XEmacPs_GetTxRing(&xemacpsif->sgrt_emacps));
        XEmacPsIf_SentBds(xemacpsif, txring);
    }

    if (is_tx_space_available(xemacpsif)) 
    {
        _unbuffered_low_level_output(xemacpsif, p);
        err = ERR_OK;
    } 
    else 
    {
        print_debug("pack dropped, no space\r\n");
        err = ERR_MEM;
    }

    SYS_ARCH_UNPROTECT(lev);
    return err;
}

/*!< end of file */
