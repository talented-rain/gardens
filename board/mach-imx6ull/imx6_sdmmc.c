/*
 * IMX6ULL Board USDHC Initial
 *
 * File Name:   imx6_sdmmc.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.10.02
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <common/time.h>
#include <board/board.h>
#include <platform/fwk_mempool.h>
#include <platform/mmc/fwk_sdcard.h>

#include "imx6_common.h"

/*!< The defines */
/*!< Mux */
#define IMX_SDMMC_MUX_WP                                IMX6UL_MUX_GPIO1_IO02_USDHC1_WP         /*!< write protection */
#define IMX_SDMMC_MUX_CD                                IMX6UL_MUX_UART1_RTS_B_GPIO1_IO19       /*!< card detect */
#define IMX_SDMMC_MUX_CMD                               IMX6UL_MUX_SD1_CMD_USDHC1_CMD           /*!< command */
#define IMX_SDMMC_MUX_CLK                               IMX6UL_MUX_SD1_CLK_USDHC1_CLK           /*!< clock */
#define IMX_SDMMC_MUX_DATA0                             IMX6UL_MUX_SD1_DATA0_USDHC1_DATA0       /*!< data line 0 */
#define IMX_SDMMC_MUX_DATA1                             IMX6UL_MUX_SD1_DATA1_USDHC1_DATA1       /*!< data line 1 */
#define IMX_SDMMC_MUX_DATA2                             IMX6UL_MUX_SD1_DATA2_USDHC1_DATA2       /*!< data line 2 */
#define IMX_SDMMC_MUX_DATA3                             IMX6UL_MUX_SD1_DATA3_USDHC1_DATA3       /*!< data line 3 */

/*!< CCM */
#define IMX_SDMMC_CLK_CG_REG							CG1
#define IMX_SDMMC_CLK_SELECT							IMX6UL_CCM_CCGR_CLOCK_ENTRY(6)
#define IMX_SDMMC_CD_CLK_CG_REG							CG13
#define IMX_SDMMC_CD_CLK_SELECT							IMX6UL_CCM_CCGR_CLOCK_ENTRY(1)
#define IMX_SDMMC_CLK_PORT_ENTRY()                      IMX6UL_CCM_PROPERTY_ENTRY()             /*!< register base address */

/*!< Pin */
#define IMX_SDMMC_CD_PORT_ENTRY()                       IMX6UL_GPIO_PROPERTY_ENTRY(1)
#define IMX_SDMMC_CD_PIN_BIT                            IMX6UL_GPIO_PIN_OFFSET_BIT(19)
#define IMX_SDMMC_IF_PORT_ENTRY()                       IMX6UL_USDHC_PROPERTY_ENTRY(1)          /*!< register base address */

/*!< The functions */
static kbool_t imx6ull_sdmmc_is_card_insert(struct fwk_sdcard_host *sprt_host);
static void imx6ull_sdmmc_set_bus_width(struct fwk_sdcard_host *sprt_host, kuint32_t option);
static void imx6ull_sdmmc_set_clk_freq(struct fwk_sdcard_host *sprt_host, kuint32_t option);
static kbool_t imx6ull_sdmmc_initial_active(struct fwk_sdcard_host *sprt_host, kuint32_t timeout);
static kint32_t imx6ull_sdmmc_switch_voltage(struct fwk_sdcard_host *sprt_host, kuint32_t voltage);
static kint32_t imx6ull_sdmmc_send_command(struct fwk_sdcard_cmd *sprt_cmds);
static void imx6ull_sdmmc_recv_response(struct fwk_sdcard_cmd *sprt_cmds);
static kint32_t imx6ull_sdmmc_transfer_data(struct fwk_sdcard_data *sprt_data);
static void imx6ull_sdmmc_reset_transfer(struct fwk_sdcard_host *sprt_host);

/*!< private function */
static void imx6ull_sdmmc_reset(srt_imx_usdhc_t *sprt_usdhc, kuint32_t optBit, kuint32_t timeout);
static void imx6ull_sdmmc_data_configure(srt_imx_usdhc_t *sprt_usdhc, struct fwk_sdcard_data *sprt_data, void *ptrData);
static kint32_t imx6ull_sdmmc_write_data(srt_imx_usdhc_t *sprt_usdhc, struct fwk_sdcard_data *sprt_data);
static kint32_t imx6ull_sdmmc_read_data(srt_imx_usdhc_t *sprt_usdhc, struct fwk_sdcard_data *sprt_data);

/*!< API function */
/*!
 * @brief   imx6ull_sdmmc_clk_initial
 * @param   none
 * @retval  none
 * @note    initial USDHC clk for SD Card
 */
static void imx6ull_sdmmc_clk_initial(void)
{
    srt_hal_imx_ccm_t *sprt_clk;

    sprt_clk = IMX_SDMMC_CLK_PORT_ENTRY();

    /*!<
     * The clock of USDHC is from two PFDs(Phase Fractional Dividers): PFD0(352MHz), PFD2(396MHz), which will output to USDHC after dividing 2.
     * The register CSCMR1 is used to choose PFD0 or PFD2; Usually, PFD2 will be first.
     * The register CSCDR1 is used to configure dividing level, generally 2
     * 
     * clock = PFD2 / 2 = 192MHz
     */
    mrt_clrbitl(mrt_bit(16), &sprt_clk->CSCMR1);
    mrt_clrbitl(mrt_bit(11) | mrt_bit(12) | mrt_bit(13), &sprt_clk->CSCDR1);
    mrt_setbitl(mrt_bit(11), &sprt_clk->CSCDR1);

    /*!< enable clock */
    mrt_imx_ccm_clk_enable(IMX_SDMMC_CLK_CG_REG, IMX_SDMMC_CLK_SELECT);

    /*!< enable cd pin clock */
    mrt_imx_ccm_clk_enable(IMX_SDMMC_CD_CLK_CG_REG, IMX_SDMMC_CD_CLK_SELECT);
}

/*!
 * @brief   imx6ull_sdmmc_pin_initial
 * @param   none
 * @retval  none
 * @note    initial USDHC Pin for SD Card
 */
static void imx6ull_sdmmc_pin_initial(void)
{
    srt_hal_imx_pin_t sgrt_uSDHC;
    urt_imx_io_ctl_pad_t ugrt_ioPad;

    /*!< Card detect pin, input direction, 47k pull up, enable HYS */
    mrt_reset_urt_data(&ugrt_ioPad);
    mrt_write_urt_bits(&ugrt_ioPad, HYS, IMX6UL_IO_CTL_PAD_HYS_ENABLE);
    mrt_write_urt_bits(&ugrt_ioPad, PUS, IMX6UL_IO_CTL_PAD_PUS_47K_UP);
    mrt_write_urt_bits(&ugrt_ioPad, PUE, IMX6UL_IO_CTL_PAD_PUE_PULL);
    mrt_write_urt_bits(&ugrt_ioPad, PKE, IMX6UL_IO_CTL_PAD_PKE_DISABLE);
    mrt_write_urt_bits(&ugrt_ioPad, DSE, IMX6UL_IO_CTL_PAD_DSE_RDIV(4));
    mrt_write_urt_bits(&ugrt_ioPad, SRE, IMX6UL_IO_CTL_PAD_FAST_RATE);
    mrt_write_urt_bits(&ugrt_ioPad, SPEED, IMX6UL_IO_CTL_PAD_SPEED_200MHZ);
    hal_imx_pin_attribute_init(&sgrt_uSDHC, IMX6UL_PIN_ADDR_BASE, 
                        IMX_SDMMC_MUX_CD, mrt_trans_urt_data(&ugrt_ioPad), IMX6UL_PIN_MUX_FUNC_DISABLE);
    hal_imx_pin_mux_configure(&sgrt_uSDHC);
    hal_imx_pin_pad_configure(&sgrt_uSDHC);

    /*!< Clock pin */
    mrt_write_urt_bits(&ugrt_ioPad, HYS, IMX6UL_IO_CTL_PAD_HYS_DISABLE);
    mrt_write_urt_bits(&ugrt_ioPad, PUS, IMX6UL_IO_CTL_PAD_PUS_22K_UP);
    mrt_write_urt_bits(&ugrt_ioPad, PUE, IMX6UL_IO_CTL_PAD_PUE_PULL);
    mrt_write_urt_bits(&ugrt_ioPad, PKE, IMX6UL_IO_CTL_PAD_PKE_DISABLE);
    mrt_write_urt_bits(&ugrt_ioPad, DSE, IMX6UL_IO_CTL_PAD_DSE_RDIV(4));
    mrt_write_urt_bits(&ugrt_ioPad, SRE, IMX6UL_IO_CTL_PAD_FAST_RATE);
    mrt_write_urt_bits(&ugrt_ioPad, SPEED, IMX6UL_IO_CTL_PAD_SPEED_100MHZ);
    hal_imx_pin_attribute_init(&sgrt_uSDHC, IMX6UL_PIN_ADDR_BASE, 
                        IMX_SDMMC_MUX_CLK, mrt_trans_urt_data(&ugrt_ioPad), IMX6UL_PIN_MUX_FUNC_DISABLE);
    hal_imx_pin_mux_configure(&sgrt_uSDHC);
    hal_imx_pin_pad_configure(&sgrt_uSDHC);

    /*!< Common Settings */
    mrt_reset_urt_data(&ugrt_ioPad);
    mrt_write_urt_bits(&ugrt_ioPad, HYS, IMX6UL_IO_CTL_PAD_HYS_DISABLE);
    mrt_write_urt_bits(&ugrt_ioPad, PUS, IMX6UL_IO_CTL_PAD_PUS_22K_UP);
    mrt_write_urt_bits(&ugrt_ioPad, PUE, IMX6UL_IO_CTL_PAD_PUE_PULL);
    mrt_write_urt_bits(&ugrt_ioPad, PKE, IMX6UL_IO_CTL_PAD_PKE_DISABLE);
    mrt_write_urt_bits(&ugrt_ioPad, DSE, IMX6UL_IO_CTL_PAD_DSE_RDIV(4));
    mrt_write_urt_bits(&ugrt_ioPad, SRE, IMX6UL_IO_CTL_PAD_FAST_RATE);

    /*!< Command pin */
    mrt_write_urt_bits(&ugrt_ioPad, SPEED, IMX6UL_IO_CTL_PAD_SPEED_100MHZ);
    hal_imx_pin_attribute_init(&sgrt_uSDHC, IMX6UL_PIN_ADDR_BASE, 
                        IMX_SDMMC_MUX_CMD, mrt_trans_urt_data(&ugrt_ioPad), IMX6UL_PIN_MUX_FUNC_DISABLE);
    hal_imx_pin_mux_configure(&sgrt_uSDHC);
    hal_imx_pin_pad_configure(&sgrt_uSDHC);

    /*!< Data pin 0 ~ 3 */
    mrt_write_urt_bits(&ugrt_ioPad, SPEED, IMX6UL_IO_CTL_PAD_SPEED_100MHZ);

    /*!< Data0 pin */
    hal_imx_pin_attribute_init(&sgrt_uSDHC, IMX6UL_PIN_ADDR_BASE, 
                        IMX_SDMMC_MUX_DATA0, mrt_trans_urt_data(&ugrt_ioPad), IMX6UL_PIN_MUX_FUNC_DISABLE);
    hal_imx_pin_mux_configure(&sgrt_uSDHC);
    hal_imx_pin_pad_configure(&sgrt_uSDHC);

    /*!< Data1 pin */
    hal_imx_pin_attribute_init(&sgrt_uSDHC, IMX6UL_PIN_ADDR_BASE, 
                        IMX_SDMMC_MUX_DATA1, mrt_trans_urt_data(&ugrt_ioPad), IMX6UL_PIN_MUX_FUNC_DISABLE);
    hal_imx_pin_mux_configure(&sgrt_uSDHC);
    hal_imx_pin_pad_configure(&sgrt_uSDHC);

    /*!< Data2 pin */
    hal_imx_pin_attribute_init(&sgrt_uSDHC, IMX6UL_PIN_ADDR_BASE, 
                        IMX_SDMMC_MUX_DATA2, mrt_trans_urt_data(&ugrt_ioPad), IMX6UL_PIN_MUX_FUNC_DISABLE);
    hal_imx_pin_mux_configure(&sgrt_uSDHC);
    hal_imx_pin_pad_configure(&sgrt_uSDHC);

    /*!< Data3 pin */
    hal_imx_pin_attribute_init(&sgrt_uSDHC, IMX6UL_PIN_ADDR_BASE, 
                        IMX_SDMMC_MUX_DATA3, mrt_trans_urt_data(&ugrt_ioPad), IMX6UL_PIN_MUX_FUNC_DISABLE);
    hal_imx_pin_mux_configure(&sgrt_uSDHC);
    hal_imx_pin_pad_configure(&sgrt_uSDHC);
}

/*!
 * @brief   imx6ull_sdmmc_host_initial
 * @param   none
 * @retval  none
 * @note    initial USDHC func for SD Card
 */
static void imx6ull_sdmmc_host_initial(void)
{
    srt_hal_imx_gpio_t  *sprt_cd;
    srt_imx_usdhc_t *sprt_usdhc;
    kuint32_t iSysCtrlReg, iWtmkLvlReg, iProtCtrlReg, iIntStatusReg;

    sprt_cd    = IMX_SDMMC_CD_PORT_ENTRY();
    sprt_usdhc = IMX_SDMMC_IF_PORT_ENTRY();

    /*!< set to input direction */
    mrt_clrbitl(IMX_SDMMC_CD_PIN_BIT, &sprt_cd->GDIR);

    /*!< ------------------------------------------------------------------------------- */
    /*!< Reset all */
    mrt_setbitl(NR_ImxUsdhc_SysCtrl_SoftResetAll, &sprt_usdhc->SYS_CTRL);
    while (mrt_isBitSetl(NR_ImxUsdhc_SysCtrl_SoftResetAll, &sprt_usdhc->SYS_CTRL));

    /*!< Reset Tuning */
    mrt_setbitl(NR_ImxUsdhc_SysCtrl_ResetTuning, &sprt_usdhc->SYS_CTRL);
    while (mrt_isBitSetl(NR_ImxUsdhc_SysCtrl_ResetTuning, &sprt_usdhc->SYS_CTRL));

    /*!< Read Registers */
    iSysCtrlReg = mrt_readl(&sprt_usdhc->SYS_CTRL);

    /*!< Data Timeout Counter Value */
    mrt_clrbitl(IMX_USDHC_SYS_CTRL_DTOCV_MASK, &iSysCtrlReg);
    mrt_setbitl(IMX_USDHC_SYS_CTRL_DTOCV_U32(0xfU), &iSysCtrlReg);

    /*!< Update Registers */
    mrt_writel(iSysCtrlReg,  &sprt_usdhc->SYS_CTRL);

    /*!< ------------------------------------------------------------------------------- */
    /*!< Read Registers */
    iWtmkLvlReg = mrt_readl(&sprt_usdhc->WTMK_LVL);

    /*!<
     * Read Watermark Level
     * The number of words used as the watermark level (FIFO threshold) in a DMA read operation. Also the
     * number of words as a sequence of read bursts in back-to-back mode. The maximum legal value for the
     * read water mark level is 128
     */
    mrt_clrbitl(IMX_USDHC_WTMK_LVL_RD_WML_MASK, &iWtmkLvlReg);
    mrt_setbitl(IMX_USDHC_WTMK_LVL_RD_WML_U32(0x80U), &iWtmkLvlReg);

    /*!<
     * Read Burst Length
     * The number of words the uSDHC reads in a single burst. The read burst length must be less than or equal
     * to the read watermark level, and all bursts within a watermark level transfer will be in back-to-back mode.
     * On reset, this field will be 8. Writing 0 to this field will result in '01000' (i.e. it is not able to clear this field)
     */
    mrt_clrbitl(IMX_USDHC_WTMK_LVL_RD_BRST_LEN_MASK, &iWtmkLvlReg);
    mrt_setbitl(IMX_USDHC_WTMK_LVL_RD_BRST_LEN_U32(0x08U), &iWtmkLvlReg);

    /*!<
     * Write Watermark Level
     * The number of words used as the watermark level (FIFO threshold) in a DMA write operation. Also the
     * number of words as a sequence of write bursts in back-to-back mode. The maximum legal value for the
     * write watermark level is 128
     */
    mrt_clrbitl(IMX_USDHC_WTMK_LVL_WR_WML_MASK, &iWtmkLvlReg);
    mrt_setbitl(IMX_USDHC_WTMK_LVL_WR_WML_U32(0x80U), &iWtmkLvlReg);

    /*!<
     * Write Burst Length
     * The number of words the uSDHC writes in a single burst. The write burst length must be less than or
     * equal to the write watermark level, and all bursts within a watermark level transfer will be in back-to-back
     * mode. On reset, this field will be 8. Writing 0 to this field will result in '01000' (i.e. it is not able to clear this
     * field).
     */
    mrt_clrbitl(IMX_USDHC_WTMK_LVL_WR_BRST_LEN_MASK, &iWtmkLvlReg);
    mrt_setbitl(IMX_USDHC_WTMK_LVL_WR_BRST_LEN_U32(0x08U), &iWtmkLvlReg);

    /*!< Update Registers */
    mrt_writel(iWtmkLvlReg,  &sprt_usdhc->WTMK_LVL);
    
    /*!< ------------------------------------------------------------------------------- */
    /*!< Read Registers */
    iProtCtrlReg = mrt_readl(&sprt_usdhc->PROT_CTRL);

    /*!< Endian Mode */
    mrt_clrbitl(IMX_USDHC_PROT_CTRL_EMODE_MASK, &iProtCtrlReg);
    mrt_setbitl(NR_ImxUsdhc_ProtCtrl_LittleEndian, &iProtCtrlReg);

    /*!< select which DMA (SDMA or ADMA) will be enabled */
    mrt_clrbitl(IMX_USDHC_PROT_CTRL_DMASEL_MASK, &iProtCtrlReg);
    mrt_setbitl(NR_ImxUsdhc_ProtCtrl_SDmaSelect, &iProtCtrlReg);

    /*!< Update Registers */
    mrt_writel(iProtCtrlReg, &sprt_usdhc->PROT_CTRL);

    /*!< ------------------------------------------------------------------------------- */
    /*!<
     * External DMA Request Enable
     *  0: In any scenario, uSDHC does not send out external DMA request.
     *  1: When internal DMA is not active, the external DMA request will be sent out.
     */
    mrt_clrbitl(NR_ImxUsdhc_VendSpec_ExtDmaRequestEnable, &sprt_usdhc->VEND_SPEC);

    /*!< ------------------------------------------------------------------------------- */
    /*!< DMA Enable */
    mrt_clrbitl(NR_ImxUsdhc_MixCtrl_DmaEnable, &sprt_usdhc->MIX_CTRL);

    /*!< ------------------------------------------------------------------------------- */
    /*!< Read Registers */
    iIntStatusReg = mrt_readl(&sprt_usdhc->INT_STATUS_EN);

    /*!< Disable interrupt signal, but enable interrupt status  */
    mrt_resetl(&iIntStatusReg);

    /*!< Select Interrupt Status Bit to Enable */
    /*!< Command Status Bit: Timeout Error, CRC Error, End Bit Error, Index Error */
    mrt_setbitl(NR_ImxUsdhc_IntCmdTimeOutErr_Bit | NR_ImxUsdhc_IntCmdCrcErr_Bit | 
               NR_ImxUsdhc_IntCmdEndBitErr_Bit | NR_ImxUsdhc_IntCmdIndexErr_Bit, &iIntStatusReg);
    /*!< Card Insertion & Removal Status Bit */
    mrt_setbitl(NR_ImxUsdhc_IntCardInsertion_Bit | NR_ImxUsdhc_IntCardRemoval_Bit, &iIntStatusReg);
    /*!< Status Bit: Cmd Transfer Complete, Data Transfer Complete, DMA Interrupt, Buffer Write Ready, Buffer Read Ready */
    mrt_setbitl(NR_ImxUsdhc_IntCmdComplete_Bit | NR_ImxUsdhc_IntDataComplete_Bit | 
               NR_ImxUsdhc_IntDmaInterrupt_Bit | NR_ImxUsdhc_IntBufferWriteReady_Bit | 
               NR_ImxUsdhc_IntBufferReadReady_Bit, &iIntStatusReg);
    /*!< Data Status Bit: Data Timeout Error, Data CRC Error, Data End Bit Error, Auto CMD12 Error */
    mrt_setbitl(NR_ImxUsdhc_IntDataTimeOutErr_Bit | NR_ImxUsdhc_IntDataCrcErr_Bit |
               NR_ImxUsdhc_IntDataEndBitErr_Bit | NR_ImxUsdhc_IntACmd12Err_Bit, &iIntStatusReg);
    /*!< SDR104 Tuning Status Bit: Re-Tuning Event, Tuning Pass, Tuning Error */
    mrt_setbitl(NR_ImxUsdhc_IntReTuningEvent_Bit | NR_ImxUsdhc_IntTuningPass_Bit |
               NR_ImxUsdhc_IntTuningErr_Bit, &iIntStatusReg);

    /*!< Update Registers */
    mrt_writel(iIntStatusReg, &sprt_usdhc->INT_STATUS_EN);
    mrt_resetl(&sprt_usdhc->INT_SIGNAL_EN);
}

/*!
 * @brief   imx6ull_sdmmc_init
 * @param   none
 * @retval  none
 * @note    initial USDHC for SD Card
 */
void imx6ull_sdmmc_init(void)
{
    imx6ull_sdmmc_clk_initial();
    imx6ull_sdmmc_pin_initial();
    imx6ull_sdmmc_host_initial();
}

/*!< ------------------------------------------------------------------------- */
/*!
 * @brief   imx6ull_sdmmc_reset_transfer
 * @param   none
 * @retval  none
 * @note    reset command/data line
 */
static void imx6ull_sdmmc_reset(srt_imx_usdhc_t *sprt_usdhc, kuint32_t optBit, kuint32_t timeout)
{
    mrt_setbitl(optBit & NR_ImxUsdhc_SysCtrl_ResetMask, &sprt_usdhc->SYS_CTRL);

    mrt_run_code_retry(timeout,

        if (mrt_isBitResetl(optBit & NR_ImxUsdhc_SysCtrl_ResetMask, &sprt_usdhc->SYS_CTRL))
            break;
    )

}

/*!
 * @brief   imx6ull_sdmmc_reset_transfer
 * @param   none
 * @retval  none
 * @note    reset command/data line
 */
static void imx6ull_sdmmc_reset_transfer(struct fwk_sdcard_host *sprt_host)
{
    srt_imx_usdhc_t *sprt_usdhc = (srt_imx_usdhc_t *)sprt_host->iHostIfBase;

    /*!<
     * CIHB: bit0, Command Inhibit (CMD). 0: idle; 1 : busy
     * CDIHB: bit1, Command Inhibit (DATA). 0: idle; 1 : busy
     */
    if (mrt_isBitSetl(NR_ImxUsdhc_PresState_CmdInhibitCmdLine, &sprt_usdhc->PRES_STATE))
        imx6ull_sdmmc_reset(sprt_usdhc, NR_ImxUsdhc_SysCtrl_SoftResetCmdLine, 100U);

    if (mrt_isBitSetl(NR_ImxUsdhc_PresState_CmdInhibitDataLine, &sprt_usdhc->PRES_STATE))
        imx6ull_sdmmc_reset(sprt_usdhc, NR_ImxUsdhc_SysCtrl_SoftResetDataLine, 100U); 
}

/*!
 * @brief   imx6ull_sdmmc_is_card_insert
 * @param   none
 * @retval  none
 * @note    detect if card insert
 */
static kbool_t imx6ull_sdmmc_is_card_insert(struct fwk_sdcard_host *sprt_host)
{
    srt_hal_imx_gpio_t *sprt_cd = (srt_hal_imx_gpio_t *)sprt_host->iHostCDBase;

    /*!< Waitting for Card Inserting */
    return mrt_isBitResetl(IMX_SDMMC_CD_PIN_BIT, &sprt_cd->DR);
}

/*!
 * @brief   imx6ull_sdmmc_set_bus_width
 * @param   none
 * @retval  none
 * @note    configure card bus width
 */
static void imx6ull_sdmmc_set_bus_width(struct fwk_sdcard_host *sprt_host, kuint32_t option)
{
    srt_imx_usdhc_t *sprt_usdhc = (srt_imx_usdhc_t *)sprt_host->iHostIfBase;

    if (option > NR_SdCard_BusWidth_8Bit)
        option = NR_SdCard_BusWidth_4Bit;

    /*!< reset command line and data line */
    imx6ull_sdmmc_reset_transfer(sprt_host);

    /*!< clear all interrupt flags */
    mrt_imx_clear_interrupt_flags(NR_ImxUsdhc_IntAllInterrupt_Bit, &sprt_usdhc);
    while (!mrt_isBitResetl(NR_ImxUsdhc_IntAllInterrupt_Bit, &sprt_usdhc->INT_STATUS));

    /*!< set DATA bus width */
    mrt_clrbitl(IMX_USDHC_PROT_CTRL_DTW_MASK, &sprt_usdhc->PROT_CTRL);
    mrt_setbitl(IMX_USDHC_PROT_CTRL_DTW_BIT(option), &sprt_usdhc->PROT_CTRL);
}

/*!
 * @brief   imx6ull_sdmmc_set_clk_freq
 * @param   none
 * @retval  none
 * @note    configure sdcard clock frequency
 */
static void imx6ull_sdmmc_set_clk_freq(struct fwk_sdcard_host *sprt_host, kuint32_t option)
{
    srt_imx_usdhc_t *sprt_usdhc = (srt_imx_usdhc_t *)sprt_host->iHostIfBase;
    kuint32_t bitFreq, preScaler = 256U, divisor = 16U;
    kuint32_t freqTimes2, sys_ctl;

    if (option > NR_SdCard_ClkFreq_50MHz)
        option = NR_SdCard_ClkFreq_25MHz;

    /*!< 
     * Dual Data Rate mode selection
     * check if freqTimes2 is 1:
     *  1: Dual Data Rate mode. Max dividers is 512
     *  0: Single Data Rate mode. Max dividers is 256
     */
    freqTimes2 = mrt_isBitSetl(NR_ImxUsdhc_MixCtrl_DualDataRateMode, &sprt_usdhc->MIX_CTRL);

    /*!<
     * uSDHC Frequency is 192MHz
     * if sdclk = 400KHz, divide = 192MHz / 400KHz = 480 = 32 * 15
     * if sdclk = 25MHz,  divide = 192MHz / 24MHz  = 8   = 8  * 1
     * if sdclk = 50MHz,  divide = 192MHz / 48MHz  = 4   = 4  * 1
     */
    switch (option)
    {
        case NR_SdCard_ClkFreq_400KHz:
            preScaler = IMX_USDHC_SYS_CTRL_SDCLKFS_U32(freqTimes2, 32U);
            divisor   = IMX_USDHC_SYS_CTRL_DVS_U32(15U);
            break;

        case NR_SdCard_ClkFreq_25MHz:
            preScaler = IMX_USDHC_SYS_CTRL_SDCLKFS_U32(freqTimes2, 8U);
            divisor   = IMX_USDHC_SYS_CTRL_DVS_U32(1U);
            break;

        case NR_SdCard_ClkFreq_50MHz:
            preScaler = IMX_USDHC_SYS_CTRL_SDCLKFS_U32(freqTimes2, 4U);
            divisor   = IMX_USDHC_SYS_CTRL_DVS_U32(1U);
            break;
    }

    bitFreq = preScaler + divisor;
    sys_ctl = mrt_readl(&sprt_usdhc->SYS_CTRL);

    /*!< Update Divisor and SDCLK */
    mrt_clrbitl(IMX_USDHC_SYS_CTRL_DVS_MASK | IMX_USDHC_SYS_CTRL_SDCLKFS_MASK, &sys_ctl);
    mrt_setbitl(bitFreq, &sys_ctl);
    mrt_writel(sys_ctl, &sprt_usdhc->SYS_CTRL);

    /*!< make sure that SDSTB is true: SD clock is stable */
    while (!mrt_isBitSetl(NR_ImxUsdhc_PresState_SDClockStable, &sprt_usdhc->PRES_STATE));
}

/*!
 * @brief   imx6ull_sdmmc_initial_active
 * @param   none
 * @retval  none
 * @note    initial active card; it will make card enter idle status
 */
static kbool_t imx6ull_sdmmc_initial_active(struct fwk_sdcard_host *sprt_host, kuint32_t timeout)
{
    srt_imx_usdhc_t *sprt_usdhc = (srt_imx_usdhc_t *)sprt_host->iHostIfBase;

    /*!<
     * When this bit is set, 80 SD-Clocks are sent to the card. After the 80 clocks are sent, this bit is self cleared.
     * This bit is very useful during the card power-up period when 74 SD-Clocks are needed and the clock auto
     * gating feature is enabled. Writing 1 to this bit when this bit is already 1 has no effect. Writing 0 to this bit at
     * any time has no effect. When either of the CIHB and CDIHB bits in the Present State Register are set,
     * writing 1 to this bit is ignored (i.e. when command line or data lines are active, write to this bit is not
     * allowed). On the otherhand, when this bit is set, i.e., during intialization active period, it is allowed to issue
     * command, and the command bit stream will appear on the CMD pad after all 80 clock cycles are done. So
     * when this command ends, the driver can make sure the 80 clock cycles are sent out. This is very useful
     * when the driver needs send 80 cycles to the card and does not want to wait till this bit is self cleared.
     */
    mrt_setbitl(NR_ImxUsdhc_SysCtrl_InitialActive, &sprt_usdhc->SYS_CTRL);

    /*!< wait for 74 clk at less */
    while (mrt_isBitSetl(NR_ImxUsdhc_SysCtrl_InitialActive, &sprt_usdhc->SYS_CTRL))
    {
        if (!(timeout--))
        {
            break;
        }
    }

    return mrt_to_kbool(timeout);
}

/*!
 * @brief   imx6ull_sdmmc_switch_voltage
 * @param   none
 * @retval  none
 * @note    switch voltage by uSDHC
 */
static kint32_t imx6ull_sdmmc_switch_voltage(struct fwk_sdcard_host *sprt_host, kuint32_t voltage)
{
    srt_imx_usdhc_t *sprt_usdhc = (srt_imx_usdhc_t *)sprt_host->iHostIfBase;

    kbool_t blRetval;

    /* check data line and cmd line status */
    /*!< all status should be "0" before switching */
    blRetval = mrt_isBitResetl(NR_ImxUsdhc_PresState_Data0LineLevel | NR_ImxUsdhc_PresState_Data1LineLevel |
                             NR_ImxUsdhc_PresState_Data2LineLevel | NR_ImxUsdhc_PresState_Data3LineLevel, 
                             &sprt_usdhc->PRES_STATE);
    if (!blRetval)
        return -ER_NREADY;

    /*!< switch to "voltage" */
    if (NR_SdCard_toVoltage1_8V == voltage)
        mrt_setbitl(NR_ImxUsdhc_VendSpec_VoltageSelect, &sprt_usdhc->VEND_SPEC);
    else
        mrt_clrbitl(NR_ImxUsdhc_VendSpec_VoltageSelect, &sprt_usdhc->VEND_SPEC);

    delay_ms(100U);

    /*!< enable force clock on */
    mrt_setbitl(NR_ImxUsdhc_VendSpec_ActiveClk, &sprt_usdhc->VEND_SPEC);
    delay_ms(10U);

    /*!< disable force clock on */
    mrt_clrbitl(NR_ImxUsdhc_VendSpec_ActiveClk, &sprt_usdhc->VEND_SPEC);

    /* check data line and cmd line status */
    /*!< at least one of the status should be "1" after switching */
    blRetval = mrt_isBitResetl(NR_ImxUsdhc_PresState_Data0LineLevel | NR_ImxUsdhc_PresState_Data1LineLevel |
                             NR_ImxUsdhc_PresState_Data2LineLevel | NR_ImxUsdhc_PresState_Data3LineLevel, 
                             &sprt_usdhc->PRES_STATE);
    if (blRetval)
        return -ER_FAILD;

    return ER_NORMAL;
}

/*!
 * @brief   imx6ull_sdmmc_send_command
 * @param   none
 * @retval  none
 * @note    send command by uSDHC
 */
static kint32_t imx6ull_sdmmc_send_command(struct fwk_sdcard_cmd *sprt_cmds)
{
    srt_imx_usdhc_t *sprt_usdhc;
    struct fwk_sdcard_host *sprt_host;
    kuint32_t iCmdXfrTypReg;
    kuint8_t index;
    kuint32_t argument;
    kint32_t iRetval;

    sprt_host = (struct fwk_sdcard_host *)sprt_cmds->ptrHost;
    if (!isValid(sprt_host))
        return -ER_NULLPTR;

    sprt_usdhc = (srt_imx_usdhc_t *)sprt_host->iHostIfBase;

    index = sprt_cmds->index & 0x3fU;
    argument = mrt_be32_to_cpu(sprt_cmds->args);

    /*!< Wait until command/data bus out of busy status */
    while (mrt_isBitSetl(NR_ImxUsdhc_PresState_CmdInhibitCmdLine, &sprt_usdhc->PRES_STATE))
    {
        /*!< do nothing */
    }

    /*!< check re-tuning request */
    if (mrt_isBitSetl(NR_ImxUsdhc_IntReTuningEvent_Bit, &sprt_usdhc->INT_STATUS))
    {
        mrt_imx_clear_interrupt_flags(NR_ImxUsdhc_IntReTuningEvent_Bit, &sprt_usdhc);
        return -ER_NREADY;
    }

    /*!< Read Registers */
    iCmdXfrTypReg = mrt_readl(&sprt_usdhc->CMD_XFR_TYP);

    /*!< configure data registers before command configure */
    imx6ull_sdmmc_data_configure(sprt_usdhc, (struct fwk_sdcard_data *)sprt_cmds->ptrData, &iCmdXfrTypReg);

    /*!< set Command index */
    mrt_clrbitl(IMX_USDHC_CMD_XFR_TYP_CMDINX_MASK, &iCmdXfrTypReg);
    mrt_setbitl(IMX_USDHC_CMD_XFR_TYP_CMDINX_U32(index), &iCmdXfrTypReg);

    /*!< Normal Other commands */
    mrt_clrbitl(IMX_USDHC_CMD_XFR_TYP_CMDTYP_MASK, &iCmdXfrTypReg);
    /*!< Disable [Command Index Check] and [Command CRC Check] temporarily */
    mrt_clrbitl(NR_ImxUsdhc_CmdXfrTyp_CmdIndexCheckEnable | NR_ImxUsdhc_CmdXfrTyp_CmdCrcCheckEnable | 
               IMX_USDHC_CMD_XFR_TYP_RSPTYP_MASK, &iCmdXfrTypReg);

    /*!< set/reset bits according to response type */
    switch (sprt_cmds->respType)
    {
        case NR_SdCard_Response_0:
            break;

        case NR_SdCard_Response_1:
        case NR_SdCard_Response_5:
        case NR_SdCard_Response_6:
        case NR_SdCard_Response_7:
            mrt_setbitl(NR_ImxUsdhc_CmdXfrTyp_CmdIndexCheckEnable | NR_ImxUsdhc_CmdXfrTyp_CmdCrcCheckEnable, &iCmdXfrTypReg);
            mrt_setbitl(NR_ImxUsdhc_CmdXfrTyp_Response48, &iCmdXfrTypReg);
            break;

        case NR_SdCard_Response_2:
            mrt_setbitl(NR_ImxUsdhc_CmdXfrTyp_CmdCrcCheckEnable, &iCmdXfrTypReg);
            mrt_setbitl(NR_ImxUsdhc_CmdXfrTyp_Response136, &iCmdXfrTypReg);
            break;

        case NR_SdCard_Response_3:
        case NR_SdCard_Response_4:
            mrt_setbitl(NR_ImxUsdhc_CmdXfrTyp_Response48, &iCmdXfrTypReg);
            break;

        case NR_SdCard_Response_1b:
        case NR_SdCard_Response_5b:
            mrt_setbitl(NR_ImxUsdhc_CmdXfrTyp_CmdIndexCheckEnable | NR_ImxUsdhc_CmdXfrTyp_CmdCrcCheckEnable, &iCmdXfrTypReg);
            mrt_setbitl(NR_ImxUsdhc_CmdXfrTyp_ResponseBusy48, &iCmdXfrTypReg);
            break;

        default:
            mrt_assert(IT_FALSE);
            break;
    }

    /*!< CMD_ARG::CMDARG: bit[31:0], Command Argument */
    mrt_writel(argument, &sprt_usdhc->CMD_ARG);
    mrt_writel(iCmdXfrTypReg, &sprt_usdhc->CMD_XFR_TYP);

    /*!< ------------------------------------------------------------------------------- */
    /*!< Wait command done */
    while (mrt_isBitResetl(NR_ImxUsdhc_IntCmdErr_Bit | NR_ImxUsdhc_IntCmdComplete_Bit, &sprt_usdhc->INT_STATUS))
    {
        /*!< do nothing */
    }

    iRetval = (mrt_isBitResetl(NR_ImxUsdhc_IntCmdErr_Bit | NR_ImxUsdhc_IntTuningErr_Bit, &sprt_usdhc->INT_STATUS) ? 
                                                                ER_NORMAL : (-ER_SCMD_FAILD));

    /*!< Clear Interrupt Status: write 1 to clear */
    mrt_imx_clear_interrupt_flags(NR_ImxUsdhc_IntCmdErr_Bit | NR_ImxUsdhc_IntTuningErr_Bit | 
                                                    NR_ImxUsdhc_IntCmdComplete_Bit, &sprt_usdhc);
    while (mrt_isBitSetl(NR_ImxUsdhc_IntCmdComplete_Bit, &sprt_usdhc->INT_STATUS))
    {
        /*!< do nothing */
    }                                                    

    /*!< reset */
    if ((-ER_SCMD_FAILD) == iRetval)
        imx6ull_sdmmc_reset_transfer(sprt_host);

    return iRetval;
}

/*!
 * @brief   imx6ull_sdmmc_recv_response
 * @param   none
 * @retval  none
 * @note    receive response by uSDHC
 */
static void imx6ull_sdmmc_recv_response(struct fwk_sdcard_cmd *sprt_cmds)
{
    srt_imx_usdhc_t *sprt_usdhc;
    struct fwk_sdcard_host *sprt_host;
    kuint8_t rsp_cnt = 3U;

    sprt_host = (struct fwk_sdcard_host *)sprt_cmds->ptrHost;
    if (!isValid(sprt_host))
        return;

    sprt_usdhc = (srt_imx_usdhc_t *)sprt_host->iHostIfBase;  

    if (NR_SdCard_Response_0 == sprt_cmds->respType)
        return;

    /*!< for CID/CSD: CMD_RSP0->bit[31:0] is CID/CSD bit[39:8] */
    *(sprt_cmds->resp) = mrt_readl(&sprt_usdhc->CMD_RSP0);

    /*!< R2 args = CID/CSD (bit[127:8]) */
    if (NR_SdCard_Response_2 == sprt_cmds->respType)
    {
        /*!< CMD_RSP3: bit[23:0] is CID/CSD bit[127:104]; CMD_RSP3 bit[31:24] is reserved */
        *(sprt_cmds->resp + 3U) = mrt_readl(&sprt_usdhc->CMD_RSP3);

        /*!< CMD_RSP2: bit[31:0] is CID/CSD bit[103:72] */
        *(sprt_cmds->resp + 2U) = mrt_readl(&sprt_usdhc->CMD_RSP2);

        /*!< CMD_RSP1: bit[31:0] is CID/CSD bit[71:40] */
        *(sprt_cmds->resp + 1U) = mrt_readl(&sprt_usdhc->CMD_RSP1);

        /*!< make resp similar to CID/CSD(bit[127:8]) */
        do
        {
            sprt_cmds->resp[rsp_cnt] = mrt_bit_mask(sprt_cmds->resp[rsp_cnt], 0xffffff00U, 8U);
            mrt_setbitl(sprt_cmds->resp[rsp_cnt - 1] >> 24U, &sprt_cmds->resp[rsp_cnt]);
            
        } while (--rsp_cnt);

        sprt_cmds->resp[0] = mrt_bit_mask(sprt_cmds->resp[0], 0xffffff00U, 8U);
    }

}

/*!
 * @brief   imx6ull_sdmmc_transfer_data
 * @param   none
 * @retval  none
 * @note    send data by uSDHC
 */
static kint32_t imx6ull_sdmmc_transfer_data(struct fwk_sdcard_data *sprt_data)
{
    srt_imx_usdhc_t *sprt_usdhc;
    struct fwk_sdcard_host *sprt_host;
    kint32_t iRetval;

    if (!sprt_data)
        return -ER_NULLPTR;

    sprt_host = (struct fwk_sdcard_host *)sprt_data->ptrHost;
    if (!isValid(sprt_host))
        return -ER_NULLPTR;

    sprt_usdhc = (srt_imx_usdhc_t *)sprt_host->iHostIfBase;

    /*!< ------------------------------------------------------------ */
    /*!< write or read */
    iRetval = (sprt_data->txBuffer) ? imx6ull_sdmmc_write_data(sprt_usdhc, sprt_data) : imx6ull_sdmmc_read_data(sprt_usdhc, sprt_data);
    switch (iRetval)
    {
        case -ER_BUSY:
            imx6ull_sdmmc_reset(sprt_usdhc, NR_ImxUsdhc_SysCtrl_ResetTuning, 100U);
            break;

        case -ER_SDATA_FAILD:
        case -ER_RDATA_FAILD:
            imx6ull_sdmmc_reset_transfer(sprt_host);
            break;

        default:
            break;
    }

    mrt_imx_clear_interrupt_flags(NR_ImxUsdhc_IntDataComplete_Bit | NR_ImxUsdhc_IntDataErr_Bit, &sprt_usdhc);
    while (!mrt_isBitResetl(NR_ImxUsdhc_IntDataComplete_Bit, &sprt_usdhc->INT_STATUS));

    return iRetval;
}

/*!
 * @brief   imx6ull_sdmmc_write_data
 * @param   none
 * @retval  none
 * @note    write data to USDHC
 */
static kint32_t imx6ull_sdmmc_write_data(srt_imx_usdhc_t *sprt_usdhc, struct fwk_sdcard_data *sprt_data)
{
    struct fwk_sdcard_host *sprt_host;
    kuint32_t iWaterMarkLimit;
    kuint32_t *ptrTxBuffer;
    kuint32_t iDataWords, iTransWords, iTransCnt;
    kuint32_t iRetry = 4096U;
    kint32_t iRetval;
    kbool_t blRetval;

    if ((!sprt_data) || (!sprt_usdhc))
        return -ER_NULLPTR;

    sprt_host = (struct fwk_sdcard_host *)sprt_data->ptrHost;
    if (!isValid(sprt_host))
        return -ER_NULLPTR;

    if (!mrt_isBitResetl(NR_ImxUsdhc_MixCtrl_DataTransferDirection, &sprt_usdhc->MIX_CTRL))
        return -ER_NSUPPORT;

    /*!< 4 bytes align. blocksize is per block size (unit: byte) */
    iDataWords = mrt_num_align4(sprt_data->blockSize) >> 2;
    iDataWords *= sprt_data->blockCount;
    ptrTxBuffer = (kuint32_t *)sprt_data->txBuffer;

    /*!< get watermark */
    iWaterMarkLimit = mrt_getbitl(IMX_USDHC_WTMK_LVL_WR_WML_MASK, &sprt_usdhc->WTMK_LVL) >> IMX_USDHC_WTMK_LVL_WR_WML_OFFSET;

    /*!< the maximum of data_size that transmited must less than iWaterMarkLimit */
    while (iDataWords)
    {
        iTransWords = mrt_ret_min2(iDataWords, iWaterMarkLimit);

        /*!< wait for buffer ready */
        do
        {
            blRetval = mrt_isBitResetl(NR_ImxUsdhc_IntBufferWriteReady_Bit | NR_ImxUsdhc_IntDataErr_Bit | 
                                     NR_ImxUsdhc_IntTuningErr_Bit, &sprt_usdhc->INT_STATUS);

        } while (blRetval); // && (--iRetry));

        if (blRetval || (!iRetry))
            return -ER_TIMEOUT;

        if (mrt_isBitSetl(NR_ImxUsdhc_IntTuningErr_Bit, &sprt_usdhc->INT_STATUS))
        {
            /*!< write 1 to clear */
            mrt_imx_clear_interrupt_flags(NR_ImxUsdhc_IntTuningErr_Bit, &sprt_usdhc);
            return -ER_BUSY;
        }

        /*!< iRetval = (no error) ? true : false */
        iRetval = mrt_isBitResetl(NR_ImxUsdhc_IntDataErr_Bit, &sprt_usdhc->INT_STATUS);
        mrt_imx_clear_interrupt_flags(NR_ImxUsdhc_IntDataErr_Bit, &sprt_usdhc);

        if (iRetval)
        {
            iDataWords -= iTransWords;

            for (iTransCnt = 0; iTransCnt < iTransWords; iTransCnt++)
            {
                /*!< 32 bits (4 bytes) will be written to Register */
                mrt_writel(*(ptrTxBuffer++), &sprt_usdhc->DATA_BUFF_ACC_PORT);
            }

            /*!< watermark is full, clear write-ready bit, waiting for the next transmission */
            mrt_imx_clear_interrupt_flags(NR_ImxUsdhc_IntBufferWriteReady_Bit, &sprt_usdhc);
        }
    }

    /*!< ------------------------------------------------------------------- */
    /*!< waitting for writting data complete, or error occur */
    while (mrt_isBitResetl(NR_ImxUsdhc_IntDataComplete_Bit | NR_ImxUsdhc_IntDataErr_Bit, &sprt_usdhc->INT_STATUS))
    {
        /*!< do nothing */
    }

    if (mrt_isBitSetl(NR_ImxUsdhc_IntDataErr_Bit, &sprt_usdhc->INT_STATUS))
        iRetval = -ER_SDATA_FAILD;

    return iRetval;
}

/*!
 * @brief   imx6ull_sdmmc_read_data
 * @param   none
 * @retval  none
 * @note    read data from USDHC
 */
static kint32_t imx6ull_sdmmc_read_data(srt_imx_usdhc_t *sprt_usdhc, struct fwk_sdcard_data *sprt_data)
{
    struct fwk_sdcard_host *sprt_host;
    kuint32_t iWaterMarkLimit;
    kuint32_t *ptrRxBuffer;
    kuint32_t iDataWords, iTransWords, iTransCnt;
    kuint32_t iRetry = 4096U;
    kint32_t iRetval = ER_NORMAL;
    kbool_t blRetval;

    if ((!sprt_data) || (!sprt_usdhc))
        return -ER_NULLPTR;

    sprt_host = (struct fwk_sdcard_host *)sprt_data->ptrHost;
    if (!isValid(sprt_host))
        return -ER_NULLPTR;

    if (!mrt_isBitSetl(NR_ImxUsdhc_MixCtrl_DataTransferDirection, &sprt_usdhc->MIX_CTRL))
        return -ER_NSUPPORT;

    /*!< 4 bytes align. blocksize is per block size (unit: byte) */
    iDataWords = mrt_num_align4(sprt_data->blockSize) >> 2;
    iDataWords *= sprt_data->blockCount;
    ptrRxBuffer = (kuint32_t *)sprt_data->rxBuffer;

    /*!< get watermark */
    iWaterMarkLimit = mrt_getbitl(IMX_USDHC_WTMK_LVL_RD_WML_MASK, &sprt_usdhc->WTMK_LVL) >> IMX_USDHC_WTMK_LVL_RD_WML_OFFSET;

    /*!< the maximum of data_size that transmited must less than iWaterMarkLimit */
    while (iDataWords)
    {
        iTransWords = mrt_ret_min2(iDataWords, iWaterMarkLimit);

        /*!< wait for buffer ready */
        do
        {
            blRetval = mrt_isBitResetl(NR_ImxUsdhc_IntBufferReadReady_Bit | NR_ImxUsdhc_IntDataErr_Bit | 
                                     NR_ImxUsdhc_IntTuningErr_Bit, &sprt_usdhc->INT_STATUS);

        } while (blRetval); // && (--iRetry));

        if (blRetval || (!iRetry))
            return -ER_TIMEOUT;

        if (mrt_isBitSetl(NR_ImxUsdhc_IntTuningErr_Bit, &sprt_usdhc->INT_STATUS))
        {
            mrt_imx_clear_interrupt_flags(NR_ImxUsdhc_IntTuningErr_Bit, &sprt_usdhc);
            return -ER_BUSY;
        }

        /*!< iRetval = (no error) ? true : false */
        iRetval = mrt_isBitResetl(NR_ImxUsdhc_IntDataErr_Bit, &sprt_usdhc->INT_STATUS);
        mrt_imx_clear_interrupt_flags(NR_ImxUsdhc_IntDataErr_Bit, &sprt_usdhc);

        if (iRetval)
        {
            iDataWords -= iTransWords;

            for (iTransCnt = 0; iTransCnt < iTransWords; iTransCnt++)
            {
                /*!< 32 bits (4 bytes) will be read from Register */
                *(ptrRxBuffer++) = mrt_readl(&sprt_usdhc->DATA_BUFF_ACC_PORT);
            }

            /*!< watermark is full, clear read-ready bit, waiting for the next transmission */
            mrt_imx_clear_interrupt_flags(NR_ImxUsdhc_IntBufferReadReady_Bit, &sprt_usdhc);
            iRetval = ER_NORMAL;
        }
        else
            iRetval = -ER_RDATA_FAILD;
    }

    return iRetval;
}

/*!
 * @brief   imx6ull_sdmmc_data_configure
 * @param   none
 * @retval  none
 * @note    configure registers about data transfer
 */
static void imx6ull_sdmmc_data_configure(srt_imx_usdhc_t *sprt_usdhc, struct fwk_sdcard_data *sprt_data, void *ptrData)
{
    kuint32_t iCmdXfrTyp;
    kuint32_t iMixCtrlReg;
    kuint32_t iBlockAttr;

    /*!<
     * ACMD23_ARGU2_EN: bit23
     * 1:   Argument2 register enable for ACMD23 sharing with SDMA system address register. Default is
     *      enable
     * 0:   Disable
     * 
     * default 1
     */
    mrt_clrbitl(mrt_bit(23U), &sprt_usdhc->VEND_SPEC2);

    iCmdXfrTyp = *(kuint32_t *)ptrData;

    /*!< Commands only using CMD line */
    mrt_clrbitl(NR_ImxUsdhc_CmdXfrTyp_DataPresentSelect, &iCmdXfrTyp);

    /*!< read register */
    iMixCtrlReg = mrt_readl(&sprt_usdhc->MIX_CTRL);

    /*!< Disable DMA */
    mrt_clrbitl(NR_ImxUsdhc_MixCtrl_DmaEnable, &iMixCtrlReg);
    /*!< Block Count Disable, Select Single Block, Auto CMD12 Disable, Data Transfer Direction is Write */
    mrt_clrbitl(NR_ImxUsdhc_MixCtrl_BlockCountEnable | NR_ImxUsdhc_MixCtrl_AutoCmd12Enable | NR_ImxUsdhc_MixCtrl_AutoCmd23Enable |
               NR_ImxUsdhc_MixCtrl_DataTransferDirection | NR_ImxUsdhc_MixCtrl_MultiSingleBlockSelect, &iMixCtrlReg);

    /*!< check if data request command */
    if (!sprt_data)
    {
        /*!< Update Register */
        *(kuint32_t *)ptrData = iMixCtrlReg;

        return;
    }

    /*!< Wait until command/data bus out of busy status */
    while (mrt_isBitSetl(NR_ImxUsdhc_PresState_CmdInhibitDataLine, &sprt_usdhc->PRES_STATE))
    {
        /*!< do nothing */
    }

    if (sprt_data->blockCount > 1U)
    {
        mrt_setbitl(NR_ImxUsdhc_MixCtrl_BlockCountEnable | NR_ImxUsdhc_MixCtrl_MultiSingleBlockSelect, &iMixCtrlReg);

        if (mrt_isBitSetw(NR_SdCard_CmdFlagsAuto23Enable, &sprt_data->flags))
        {
            mrt_setbitl(NR_ImxUsdhc_MixCtrl_AutoCmd23Enable, &iMixCtrlReg);
            mrt_setbitl(mrt_bit(23U), &sprt_usdhc->VEND_SPEC2);

            /*!<
            * DMA System Address
            * When ACMD23_ARGU2_EN is set to 0, SDMA uses this register as system address and supports only
            * 32-bit addressing mode. Auto CMD23 cannot be used with SDMA. When ACMD23_ARGU2_EN is set to
            * 1, SDMA uses ADMA System Address register (05Fh – 058h) instead of this register to support both 32-bit
            * and 64-bit addressing. This register is used only for Argument2 and SDMA may use Auto CMD23
            */
            mrt_writel(sprt_data->blockCount, &sprt_usdhc->DS_ADDR);
        }

        if (mrt_isBitSetw(NR_SdCard_CmdFlagsAuto12Enable, &sprt_data->flags))
            mrt_setbitl(NR_ImxUsdhc_MixCtrl_AutoCmd12Enable, &iMixCtrlReg);
    }

    if (mrt_isBitSetw(NR_SdCard_CmdFlagsReadEnable, &sprt_data->flags))
        mrt_setbitl(NR_ImxUsdhc_MixCtrl_DataTransferDirection, &iMixCtrlReg);

    /*!< BLK_ATT (Block Attribute) */
    iBlockAttr = mrt_readl(&sprt_usdhc->BLK_ATT);
    mrt_clrbitl(IMX_USDHC_BLK_ATT_BLKCNT_MASK | IMX_USDHC_BLK_ATT_BLKSIZE_MASK, &iBlockAttr);

    /*!< block count */
    mrt_setbitl(IMX_USDHC_BLK_ATT_BLKCNT_U32(sprt_data->blockCount), &iBlockAttr);
    /*!< block size */
    mrt_setbitl(IMX_USDHC_BLK_ATT_BLKSIZE_U32(sprt_data->blockSize), &iBlockAttr);

    /*!< Commands using CMD line and DATA line */
    mrt_setbitl(NR_ImxUsdhc_CmdXfrTyp_DataPresentSelect, &iCmdXfrTyp);

    /*!< Update Register */
    mrt_writel(iBlockAttr, &sprt_usdhc->BLK_ATT);
    mrt_writel(iMixCtrlReg, &sprt_usdhc->MIX_CTRL);

    *(kuint32_t *)ptrData = iCmdXfrTyp;
}

/*!
 * @brief   host_sdmmc_card_initial
 * @param   none
 * @retval  none
 * @note    initial host of SD Card
 */
void *host_sdmmc_card_initial(struct fwk_sdcard *sprt_card)
{
    srt_imx_usdhc_t *sprt_usdhc;
    struct fwk_sdcard_if *sprt_if;
    struct fwk_sdcard_host *sprt_host;
    kuint32_t iDoEmpty;

    if (!isValid(sprt_card))
        return mrt_nullptr;

    sprt_if = &sprt_card->sgrt_if;

    sprt_host = (struct fwk_sdcard_host *)kzalloc(sizeof(struct fwk_sdcard_host), GFP_KERNEL);
    if (!isValid(sprt_host))
        return mrt_nullptr;

    sprt_host->iHostIfBase = (kuaddr_t)IMX_SDMMC_IF_PORT_ENTRY();
    sprt_host->iHostCDBase = (kuaddr_t)IMX_SDMMC_CD_PORT_ENTRY();
    sprt_host->isSelfDync = true;

    sprt_usdhc = (srt_imx_usdhc_t *)sprt_host->iHostIfBase;

    /*!< get support */
    sprt_host->maxBlockLength = mrt_getbit_u32(IMX_USDHC_HOST_CTRL_CAP_MBL_MASK, IMX_USDHC_HOST_CTRL_CAP_MBL_OFFSET, &sprt_usdhc->HOST_CTRL_CAP);
    sprt_host->maxBlockCount  = 0xffffU;

    mrt_resetl(&sprt_host->flagBit);
    if (mrt_isBitSetl(NR_ImxUsdhc_HostCtrlCap_Voltage18VSupport, &sprt_usdhc->HOST_CTRL_CAP))
        mrt_setbitl(NR_SdCard_SupportVoltage1_8V, &sprt_host->flagBit);

    mrt_setbitl(NR_SdCard_Support4BitWidth, &sprt_host->flagBit);

    /*!< interface initial */
    sprt_if->is_insert      = imx6ull_sdmmc_is_card_insert;
    sprt_if->setBusWidth    = imx6ull_sdmmc_set_bus_width;
    sprt_if->setClkFreq     = imx6ull_sdmmc_set_clk_freq;
    sprt_if->cardActive     = imx6ull_sdmmc_initial_active;
    sprt_if->sendCommand    = imx6ull_sdmmc_send_command;
    sprt_if->recvResp       = imx6ull_sdmmc_recv_response;
    sprt_if->sendData       = imx6ull_sdmmc_transfer_data;
    sprt_if->recvData       = imx6ull_sdmmc_transfer_data;
    sprt_if->switchVoltage  = imx6ull_sdmmc_switch_voltage;
    sprt_if->addHeadTail    = mrt_nullptr;

    /*!< read but not use */
    iDoEmpty = mrt_readl(&sprt_usdhc->DATA_BUFF_ACC_PORT);
    iDoEmpty++;

    mrt_imx_clear_interrupt_flags(NR_ImxUsdhc_IntBufferWriteReady_Bit, &sprt_usdhc);
    mrt_imx_clear_interrupt_flags(NR_ImxUsdhc_IntBufferReadReady_Bit, &sprt_usdhc);

    return sprt_host;
}


/* end of file */
