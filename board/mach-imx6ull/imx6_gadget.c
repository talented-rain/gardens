/*
 * IMX6ULL Board USB Virtual Device
 *
 * File Name:   imx6_gadget.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.12.25
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <common/generic.h>
#include <common/time.h>
#include <platform/irq/fwk_irq_types.h>
#include <platform/usb/fwk_ch9.h>
#include <platform/usb/fwk_hid.h>
#include <platform/usb/fwk_urb.h>
#include <platform/usb/host/fwk_usb_host.h>
#include "imx6_common.h"

/*!< The defines */
#define IMX_GADGET_OTG_PORT_ENTRY                           IMX6UL_USBOTG_PROPERTY_ENTRY(1)
#define IMX_GADGET_PHY_PORT_ENTRY                           IMX6UL_USBPHY_PROPERTY_ENTRY(1)
#define IMX_GADGET_NC_PORT_ENTRY                            IMX6UL_USBNC_PROPERTY_ENTRY(1)

/*!< CCM */
#define IMX_GADGET_CLK_CG_REG								CG0
#define IMX_GADGET_CLK_SELECT								IMX6UL_CCM_CCGR_CLOCK_ENTRY(6)

/*!< USB PHY condfiguration */
#define IMX_GADGET_PHY_D_CAL                                (0x0cU)
#define IMX_GADGET_PHY_TXCAL45DP                            (0x06U)
#define IMX_GADGET_PHY_TXCAL45DN                            (0x06U)

/*!< How many endpoints are supported in the stack */
#define IMX_GADGET_CONFIG_ENDPOINTS                         (4U)

typedef struct imx_gadget
{
    srt_imx_usbotg_t *sprt_otg;
    srt_imx_usbphy_t *sprt_phy;
    srt_imx_usbnc_t *sprt_nc;

    kuint32_t ep_count;

} srt_imx_gadget_t;

static kuint8_t g_iImx_gadget_queue_head[IMX_GADGET_CONFIG_ENDPOINTS << 1] __align(2048);

/*!< The functions */
irq_return_t imx6_gadget_isr(void *ptrDev);

static void imx6_gadget_ehci_token_handler(void *ptrDev);
static void imx6_gadget_ehci_token_handler(void *ptrDev);
static void imx6_gadget_ehci_detect_handler(void *ptrDev);
static void imx6_gadget_ehci_reset_handler(void *ptrDev);
static void imx6_gadget_ehci_sleep_handler(void *ptrDev);

/*!< API function */
/*!
 * @brief   get usb_phy entry
 * @param   none
 * @retval  phy entry pointer
 * @note    get phy address
 */
static srt_imx_usbphy_t *imx6_gadget_get_phy_entry(void)
{
    return IMX_GADGET_PHY_PORT_ENTRY;
}

/*!
 * @brief   get usb_nc entry
 * @param   none
 * @retval  nc entry pointer
 * @note    get nc address
 */
static srt_imx_usbnc_t *imx6_gadget_get_nc_entry(void)
{
    return IMX_GADGET_NC_PORT_ENTRY;
}

/*!
 * @brief   get usb_otg entry
 * @param   none
 * @retval  otg entry pointer
 * @note    get otg address
 */
static srt_imx_usbotg_t *imx6_gadget_get_otg_entry(void)
{
    return IMX_GADGET_OTG_PORT_ENTRY;
}

/*!
 * @brief   initial usb clock
 * @param   none
 * @retval  none
 * @note    none
 */
static kint32_t imx6_gadget_clk_initial(srt_imx_usbotg_t *sprt_otg, srt_imx_usbphy_t *sprt_phy)
{
    if ((!sprt_otg) || (!sprt_phy))
        return -ER_NULLPTR;

    /*!< PWD register provides overall control of the PHY power state */
    mrt_resetl(&sprt_phy->PWD);

    /*!< 
     * SFTRST: Writing a 1 to this bit will soft-reset the USBPHYx_PWD, USBPHYx_TX, USBPHYx_RX, and
     *      USBPHYx_CTRL registers. Set to 0 to release the PHY from reset;
     * 
     * CLKGATE: Gate UTMI Clocks. Clear to 0 to run clocks. Set to 1 to gate clocks;
     *      Set this to save power while the USB is not actively being used. Configuration state is kept while the clock is gated
     */
    mrt_clrbitl(mrt_bit(31U) | mrt_bit(30U), &sprt_phy->CTRL);

    /*!< 
     * ENAUTOCLR_PHY_PWD: Enables the feature to auto-clear the PWD register bits in USBPHYx_PWD if there is wakeup
     *      event while USB is suspended. This should be enabled if needs to support auto wakeup without S/W's interaction.
     * 
     * ENAUTOCLR_CLKGATE: Enables the feature to auto-clear the CLKGATE bit if there is wakeup event while USB is
     *      suspended. This should be enabled if needs to support auto wakeup without S/W's interaction
     */
    mrt_setbitl(mrt_bit(20U) | mrt_bit(19U), &sprt_phy->CTRL);

	/*!< enable usb clock */
    mrt_imx_ccm_clk_enable(IMX_GADGET_CLK_CG_REG, IMX_GADGET_CLK_SELECT);

    /*!< 
     * RST: Controller Reset (RESET) - Read/Write
     * Software uses this bit to reset the controller. This bit is set to zero by the Host/Device Controller when the reset process is complete
     */
    mrt_setbitl(mrt_bit(1U), &sprt_otg->USBCMD);
    while (!mrt_isBitResetl(mrt_bit(1U), &sprt_otg->USBCMD));

    return ER_NORMAL;
}

/*!
 * @brief   initial usb ehci phy
 * @param   none
 * @retval  none
 * @note    none
 */
static kint32_t imx6_gadget_ehci_phy_initial(srt_imx_usbphy_t *sprt_phy)
{
    if (!sprt_phy)
        return -ER_NULLPTR;

    /*!< Enables UTMI+ Level2. This should be enabled if needs to support LS device */
    mrt_setbitl(mrt_bit(14U), &sprt_phy->CTRL);
    /*!< Enables UTMI+ Level3. This should be enabled if needs to support external FS Hub with LS device connected */
    mrt_setbitl(mrt_bit(15U), &sprt_phy->CTRL);

    /*!< PWD register provides overall control of the PHY power state */
    mrt_clrbitl(0U, &sprt_phy->PWD);
    mrt_clrbitl(0xfU | 0xF00U | 0xF0000U, &sprt_phy->TX);

    /*!< Resistor Trimming Code. 0x0: 0.16%, 0xf, 25%; */
    mrt_setbitl(IMX_GADGET_PHY_D_CAL, &sprt_phy->TX);
    /*!< Decode to select a 45-Ohm resistance to the USB_DP output pin */
    mrt_setbitl(IMX_GADGET_PHY_TXCAL45DP, &sprt_phy->TX);
    /*!< Decode to select a 45-Ohm resistance to the USB_DN output pin */
    mrt_setbitl(IMX_GADGET_PHY_TXCAL45DN, &sprt_phy->TX);

    return ER_NORMAL;
}

/*!
 * @brief   initial usb ehci otg
 * @param   none
 * @retval  none
 * @note    none
 */
static kint32_t imx6_gadget_ehci_otg_initial(srt_imx_usbotg_t *sprt_otg)
{
    if (!sprt_otg)
        return -ER_NULLPTR;

    /*!< 
     * CM: bit[1:0], Controller Mode - R/WO
     * For OTG controller core, reset value is '00b'.
     * 
     * 00 Idle [Default for combination host/device]
     * 01 Reserved
     * 10 Device Controller [Default for device only controller]
     * 11 Host Controller [Default for host only controller]
     */
    mrt_clrbitl(mrt_bit(0U) | mrt_bit(1U), &sprt_otg->USBMODE);
    mrt_setbitl(mrt_bit(1U), &sprt_otg->USBMODE);

    /*!<
     * SLOW: bit3, Setup Lockout Mode
     * In device mode, this bit controls behavior of the setup lock mechanism
     * 
     * 0 Setup Lockouts On (default);
     * 1 Setup Lockouts Off (DCD requires use of Setup Data Buffer Tripwire in USBCMD Register
     */
    mrt_clrbitl(mrt_bit(3U), &sprt_otg->USBMODE);

    /*!<
     * ES: bit2, Endian Select - Read/Write
     * 0 Little Endian [Default]
     * 1 Big Endian
     */
    mrt_clrbitl(mrt_bit(2U), &sprt_otg->USBMODE);

    /*!<
     * ITC: bit[23:16], Interrupt Threshold Control -Read/Write
     *  0x00 Immediate (no threshold)
     *  0x01 1 micro-frame
     *  0x02 2 micro-frames
     *  0x04 4 micro-frames
     *  ...
     *  0x40 64 micro-frames
     */
    mrt_clrbitl(0xff0000U, &sprt_otg->USBCMD);

    /*!<
     * USBADR: bit[31:25], Device Address;
     * USBADRA: bit24, Device Address Advance. Default = 0
     */
    mrt_resetl(&sprt_otg->DEVICEADDR);

    /*!< 
     * EPBASE: bit[31:11], Endpoint List Pointer(Low)
     *      These bits correspond to memory address signals [31:11], respectively. This
     *      field will reference a list of up to 32 Queue Head (QH) (that is, one queue head per endpoint & direction)
     *      (The field bit[10:0] is reserved, that is, QH must be aligned by 2048 bytes)
     */
    mrt_writel(g_iImx_gadget_queue_head, &sprt_otg->ENDPTLISTADDR);

    return ER_NORMAL;
}

/*!
 * @brief   initial usb for device
 * @param   none
 * @retval  none
 * @note    none
 */
void imx6_usb_gadget_initial(void)
{
    srt_imx_gadget_t *sprt_gadget;

    srt_imx_usbotg_t *sprt_otg;
    srt_imx_usbphy_t *sprt_phy;
    srt_imx_usbnc_t *sprt_nc;

    kint32_t retval;

    sprt_otg = imx6_gadget_get_otg_entry();
    sprt_phy = imx6_gadget_get_phy_entry();
    sprt_nc  = imx6_gadget_get_nc_entry();

    /*!< Initial USB Clock */
    retval = imx6_gadget_clk_initial(sprt_otg, sprt_phy);
    if (retval < 0)
        return;

    /*!< Initial USB PHY */
    retval = imx6_gadget_ehci_phy_initial(sprt_phy);
    if (retval < 0)
        return;

    /*!< Initial USB OTG */
    retval = imx6_gadget_ehci_otg_initial(sprt_otg);
    if (retval < 0)
        return;

    sprt_gadget = (srt_imx_gadget_t *)kzalloc(sizeof(srt_imx_gadget_t), GFP_KERNEL);
    if (!isValid(sprt_gadget))
        return;

    sprt_gadget->sprt_otg = sprt_otg;
    sprt_gadget->sprt_phy = sprt_phy;
    sprt_gadget->sprt_nc  = sprt_nc;

    /*!<
     * DEN: bit[4:0], Device Endpoint Number
     * This field indicates the number of endpoints built into the device controller. If this controller is not device
     * capable, then this field will be zero. Valid values are 0 - 15
     */
    sprt_gadget->ep_count = mrt_getbit_u32(0x1fU, 0U, &sprt_otg->DCCPARAMS);

    /*!<
     * USBINTR: Interrupt Enable Register
     *
     * UE: bit0, USB Interrupt Enable
     * UEE: bit1, USB Error Interrupt Enable
     * PCE: bit2, Port Change Detect Interrupt Enable
     * URE: bit6, USB Reset Interrupt Enable
     * SLE: bit8, Sleep Interrupt Enable
     */
    mrt_writel(NR_ImxUsbOtgIntr_UsbIntBit | NR_ImxUsbOtgIntr_UsbErrIntBit | 
            NR_ImxUsbOtgIntr_PortChangeDetectIntBit | NR_ImxUsbOtgIntr_UsbResetIntBit | 
            NR_ImxUsbOtgIntr_SleepIntBit, &sprt_otg->USBINTR);
    
    fwk_enable_irq(NR_IMX_USB_OTG1_IRQn);

    /*!< 
     * Start USB
     * RS: bit0, Run/Stop (RS) - Read/Write. Default 0b. 1=Run. 0=Stop
     * 
     * for device operation mode:
     *  Writing a one to this bit will cause the controller to enable a pull-up on D+ and initiate an attach event;
     *  Writing a 0 to this will cause a detach event
     */
    mrt_setbitl(mrt_bit(0U), &sprt_otg->USBCMD);
}

/*!< Interrupt handler */
#define IMX6_GADGET_IS_INT_OCCUR(bit, reg)                  mrt_isBitSetl(bit, &(reg)->USBSTS)
#define IMX6_GADGET_CLEAR_INT_FLAG(bit, reg)                mrt_setbitl(bit, &(reg)->USBSTS)   

/*!
 * @brief   usb gadget general interrupt handler
 * @param   type: INTR register's interrupt type
 * @param   handler: interrupt handler for different type
 * @param   ptrDev: private parameter
 * @retval  none
 * @note    none
 */
static void __imx6_gadget_handler(nrt_imx_usb_intr_t type, void (*handler)(void *), void *ptrDev)
{
    srt_imx_gadget_t *sprt_gadget = (srt_imx_gadget_t *)ptrDev;

    if (!IMX6_GADGET_IS_INT_OCCUR(type, sprt_gadget->sprt_otg))
        return;        

    if (handler)
        handler(sprt_gadget);

    IMX6_GADGET_CLEAR_INT_FLAG(type, sprt_gadget->sprt_otg);
}

/*!
 * @brief   usb gadget interrupt handler
 * @param   ptrDev: private parameter
 * @retval  error code
 * @note    none
 */
irq_return_t imx6_gadget_isr(void *ptrDev)
{
    if (!ptrDev)
        return -ER_NULLPTR;

    /*!< USB Interrupt Status */
    __imx6_gadget_handler(NR_ImxUsbOtgIntr_UsbIntBit, imx6_gadget_ehci_token_handler, ptrDev);

    /*!< USB Error Interrupt Status */
    __imx6_gadget_handler(NR_ImxUsbOtgIntr_UsbErrIntBit, mrt_nullptr, ptrDev);

    /*!< Port Change Detect Interrupt Status */
    __imx6_gadget_handler(NR_ImxUsbOtgIntr_PortChangeDetectIntBit, imx6_gadget_ehci_detect_handler, ptrDev);

    /*!< USB Reset Interrupt Status */
    __imx6_gadget_handler(NR_ImxUsbOtgIntr_UsbResetIntBit, imx6_gadget_ehci_reset_handler, ptrDev); 

    /*!< Sleep Interrupt Status */
    __imx6_gadget_handler(NR_ImxUsbOtgIntr_SleepIntBit, imx6_gadget_ehci_sleep_handler, ptrDev);

    return ER_NORMAL;
}

/*!
 * @brief   usb gadget token interrupt handler
 * @param   ptrDev: private parameter
 * @retval  none
 * @note    none
 */
static void imx6_gadget_ehci_token_handler(void *ptrDev)
{

}

/*!
 * @brief   usb gadget port change detect interrupt handler
 * @param   ptrDev: private parameter
 * @retval  none
 * @note    none
 */
static void imx6_gadget_ehci_detect_handler(void *ptrDev)
{
    
}

/*!
 * @brief   usb gadget reset interrupt handler
 * @param   ptrDev: private parameter
 * @retval  none
 * @note    none
 */
static void imx6_gadget_ehci_reset_handler(void *ptrDev)
{
    
}

/*!
 * @brief   usb gadget sleep interrupt handler
 * @param   ptrDev: private parameter
 * @retval  none
 * @note    none
 */
static void imx6_gadget_ehci_sleep_handler(void *ptrDev)
{
    
}
