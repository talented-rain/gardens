/*
 * Clock Driver
 *
 * File Name:   imx-clks.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.03.26
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/of/fwk_of.h>
#include <platform/of/fwk_of_device.h>
#include <platform/fwk_platdrv.h>
#include <platform/clk/fwk_clk.h>
#include <platform/clk/fwk_clk_provider.h>
#include <platform/clk/fwk_clk_gate.h>

#include <asm/imx6/imx6ull_clocks.h>

/*!< The defines */
typedef struct fwk_clks srt_fwk_clks_t;

struct imx_clks_data
{
    srt_hal_imx_ccm_t *sprt_ccm;
    srt_hal_imx_ccm_pll_t *sprt_pll;
    struct fwk_device_node *sprt_clks;
    struct fwk_device_node *sprt_anatop;
};

typedef struct imx_clk_gate_fix 
{
    struct fwk_clk_one_cell sgrt_cell;
	const struct fwk_clk_ops *sprt_ops;
	void *reg;

} srt_imx_clk_gate_fix_t;

/*!< The globals */
static struct fwk_clk sgrt_imx_clks_data[IMX6UL_CLK_END];
static struct fwk_clk_one_cell sgrt_imx_clk_one_cell_data;

/*!< device id for device-tree */
static struct fwk_of_device_id sgrt_imx_antop_driver_ids[] =
{
	{ .compatible = "fsl,imx6ul-anatop", },
	{},
};

static struct fwk_of_device_id sgrt_imx_ccm_driver_ids[] =
{
	{ .compatible = "fsl,imx6ul-ccm", },
	{},
};

/*!< API function */
/*!
 * @brief   fill sprt_data
 * @param   sprt_data
 * @retval  none
 * @note    none
 */
static void imx_clk_init_data(srt_imx_clk_gate_fix_t *sprt_data, const struct fwk_clk_ops *sprt_ops, void *reg)
{
    sprt_data->sgrt_cell.sprt_clks = &sgrt_imx_clks_data[0];
    sprt_data->sgrt_cell.clks_size = ARRAY_SIZE(sgrt_imx_clks_data);

	/*!< it is not necessary to update every time */
	if (sprt_ops)
		sprt_data->sprt_ops = sprt_ops;
	
    if (reg)
        sprt_data->reg = reg;
}

/*!
 * @brief   register clk gate
 * @param   sprt_clk, sprt_data
 * @retval  the fwk_clk allocated
 * @note    none
 */
static struct fwk_clk *imx_clk_gate(struct fwk_clk *sprt_clk, const kchar_t *name, 
                            kuint8_t shift, const kchar_t *parent, srt_imx_clk_gate_fix_t *sprt_data)
{
    return fwk_clk_gate_register(sprt_clk, 
                                 sprt_data->sprt_ops,
                                 name,
                                 parent,
                                 sprt_data->reg,
                                 shift);
}

/*!
 * @brief   register clk gate
 * @param   number, sprt_data
 * @retval  errno
 * @note    none
 */
static kint32_t imx_clk_init_gate(kuint32_t number, const kchar_t *name, 
                            kuint8_t shift, const kchar_t *parent, srt_imx_clk_gate_fix_t *sprt_data)
{
    struct fwk_clk *sprt_clk;

    if (number > sprt_data->sgrt_cell.clks_size)
        return -ER_FAULT;

    sprt_clk = &sprt_data->sgrt_cell.sprt_clks[number];
    sprt_clk = imx_clk_gate(sprt_clk, name, shift, parent, sprt_data);

    return isValid(sprt_clk) ? 0 : (-ER_NOMEM);
}

/*!
 * @brief   initial video clock
 * @param   sprt_data
 * @retval  none
 * @note    none
 */
static void imx_clks_video_init(struct imx_clks_data *sprt_data)
{
	srt_hal_imx_ccm_t *sprt_ccm;
	srt_hal_imx_ccm_pll_t *sprt_pll;

    sprt_ccm = sprt_data->sprt_ccm;
    sprt_pll = sprt_data->sprt_pll;

    /* PLL5 (PLL_VIDEO), output clock is (OSC24MHz / (PLL_VIDEO[20:19] + (PLL_VIDEO_NUM / PLL_VIDEO_DENOM)) / MISC2[31:30]) */

    /*!<
     * PLL_VIDEO_NUM: Numerator of Video PLL Fractional Loop Divider Register
     *  bit[31:30]: set to zero
     *  bit[29:0]:  30 bit numerator of fractional loop divider(Signed number), absolute value should be less than denominator
     */
    mrt_writel(0, &sprt_pll->PLL_VIDEO_NUM);

    /*!<
     * PLL_VIDEO_DENOM: Denominator of Video PLL Fractional Loop Divider Register
     *  bit[31:30]: set to zero
     *  bit[29:0]:  30 bit Denominator of fractional loop divider.
     */
    mrt_writel(0, &sprt_pll->PLL_VIDEO_DENOM);

	/*!<
     * MISC2: Miscellaneous Register 2
     * bit[31:30]: Post-divider for video. 
     *      The output clock of the video PLL should be gated prior to changing this divider to prevent glitches. 
     *      This divider is feed by PLL_VIDEOn[POST_DIV_SELECT] to achieve division ratios of /1, /2, /4, /8, and /16.
     *      value: 00(1), 01(2), 10(4), 11(8)
     * 
     * here set to 11, divied by 8
	 */
    mrt_clrbitl(mrt_bit_nr(0x3, 30), &sprt_pll->MISC2);
    mrt_setbitl(mrt_bit(31) | mrt_bit(30), &sprt_pll->MISC2);

	/*!<
     * PLL_VIDEO (PLL5): Analog Video PLL control Register
     * bit31:       1: pll is currently locked; 0: pll is not currently locked
     * bit[20:19]:  These bits implement a divider after the PLL, but before the enable and bypass mux. value: 00(4), 01(2), 10(1)
     * bit[13]:     Enalbe PLL output
     * bit[6:0]:    pll loop divider, value: 27 ~ 54
     * 
     * here set bit19 (divied by 2) and bit 13, 
	 */
    mrt_writel(mrt_bit(19) | mrt_bit(13), &sprt_pll->PLL_VIDEO);

    /*!< pll loop divider */
    mrt_clrbitl(0x7f, &sprt_pll->PLL_VIDEO);
    mrt_setbitl(31, &sprt_pll->PLL_VIDEO);

    /*!< check if locked */
    while (mrt_isBitResetl(mrt_bit(31), &sprt_pll->PLL_VIDEO));

	/*!<
     * CSCDR2: CCM Serial Clock Divider Register 2
     * bit[17:15]: Selector for lcdif1 root clock pre-multiplexer
     *      000 derive clock from PLL2
     *      001 derive clock from PLL3 PFD3
     *      010 derive clock from PLL5
     *      011 derive clock from PLL2 PFD0
     *      100 derive clock from PLL2 PFD1
     *      101 derive clock from PLL3 PFD1
     *      110-111 Reserved
     * bit[14:12]: Pre-divider for lcdif1 clock. (NOTE: Divider should be updated when output clock is gated )
     *      000 ~ 111 divide by 1 ~ 8
     * bit[11:9]: Selector for LCDIF1 root clock multiplexer
     *      000 derive clock from divided pre-muxed LCDIF1 clock
     *      001 derive clock from ipp_di0_clk
     *      010 derive clock from ipp_di1_clk
     *      011 derive clock from ldb_di0_clk
     *      100 derive clock from ldb_di1_clk
     * 
     * select PLL5
     */
    mrt_clrbitl(mrt_bit(17) | mrt_bit(16) | mrt_bit(15), &sprt_ccm->CSCDR2);
    mrt_setbitl(mrt_bit(16), &sprt_ccm->CSCDR2);

    /*!<
     * set ratio = PLL_VIDEO[6:0] / CSCDR2[14:12] / CBCMR[25:23] ===> pixclk = OSC24MHz * ratio
     * 
     * if LCD pixclk = 9.2MHz, ===> make it to 9.3MHz = (OSC24MHZ * (31 + 0) / 8) / (5 * 2)
     * set:
     *      PLL_VIDEO[6:0] = 31, CSCDR2[14:12] = 5, CBCMR[25:23] = 2, and MISC2[31:30] = 8
     */

	/*!< Pre-divider for lcdif1 clock */
    mrt_clrbitl(mrt_bit_nr(0x7, 12), &sprt_ccm->CSCDR2);
    mrt_setbitl(mrt_bit_nr(5 - 1, 12), &sprt_ccm->CSCDR2);

	/*!<
     * CBCMR: CCM Bus Clock Multiplexer Register
     * bit[25:23], Post-divider for LCDIF1 clock. value: 1 ~ 8
     */
    mrt_clrbitl(mrt_bit_nr(0x7, 23), &sprt_ccm->CBCMR);
    mrt_setbitl(mrt_bit_nr(2 - 1, 23), &sprt_ccm->CBCMR);

    /*!< select pre-muxed LCDIF1 clock */
    mrt_clrbitl(mrt_bit(11) | mrt_bit(10) | mrt_bit(9), &sprt_ccm->CSCDR2);

    /*!< enbale LCD clock */
//  mrt_setbitl(IMX6UL_CCM_CCGR_BIT(14), &sprt_ccm->CCGR2);

    /*!< enable LCD PixClock */
//  mrt_setbitl(IMX6UL_CCM_CCGR_BIT(5), &sprt_ccm->CCGR3);
}

/*!
 * @brief   enable clk
 * @param   sprt_hw
 * @retval  errono
 * @note    none
 */
static kint32_t imx_clks_gate_enable(struct fwk_clk_hw *sprt_hw)
{
    struct fwk_clk_gate *sprt_gate;

    sprt_gate = mrt_container_of(sprt_hw, struct fwk_clk_gate, sgrt_hw);
    mrt_setbitl(IMX6UL_CCM_CCGR_BIT(sprt_gate->bit_idx), sprt_gate->reg);

    return ER_NORMAL;
}

/*!
 * @brief   disable clk
 * @param   sprt_hw
 * @retval  none
 * @note    none
 */
static void	imx_clks_gate_disable(struct fwk_clk_hw *sprt_hw)
{
    struct fwk_clk_gate *sprt_gate;

    sprt_gate = mrt_container_of(sprt_hw, struct fwk_clk_gate, sgrt_hw);
    mrt_clrbitl(IMX6UL_CCM_CCGR_BIT(sprt_gate->bit_idx), sprt_gate->reg);
}

/*!
 * @brief   get clk status
 * @param   sprt_hw
 * @retval  errono
 * @note    none
 */
static kint32_t imx_clks_gate_is_enabled(struct fwk_clk_hw *sprt_hw)
{
    struct fwk_clk_gate *sprt_gate;
    kuint32_t value;

    sprt_gate = mrt_container_of(sprt_hw, struct fwk_clk_gate, sgrt_hw);
    value = mrt_getbitl(IMX6UL_CCM_CCGR_BIT(sprt_gate->bit_idx), sprt_gate->reg);

    return !!value;
}

static const struct fwk_clk_ops sgrt_imx_clks_gate_oprts =
{
    .enable = imx_clks_gate_enable,
    .disable = imx_clks_gate_disable,
    .is_enabled = imx_clks_gate_is_enabled,
};

/*!
 * @brief   create clk global data from device_tree
 * @param   sprt_data
 * @retval  errono
 * @note    none
 */
static kint32_t imx_clks_driver_of_init(struct imx_clks_data *sprt_data)
{
    struct fwk_clk *sprt_gclk;
	srt_hal_imx_ccm_t *sprt_ccm;
    struct fwk_device_node *sprt_npccm;
    srt_imx_clk_gate_fix_t sgrt_init;
    kuint32_t ret = 0;

    sprt_ccm = sprt_data->sprt_ccm;
    sprt_npccm = sprt_data->sprt_clks;

    imx_clk_init_data(&sgrt_init, mrt_nullptr, mrt_nullptr);
    sprt_gclk = sgrt_init.sgrt_cell.sprt_clks;
    if (!sprt_gclk)
        return -ER_NODEV;

    memset(sprt_gclk, 0, sgrt_init.sgrt_cell.clks_size * sizeof(*sprt_gclk));
    memcpy(&sgrt_imx_clk_one_cell_data, &sgrt_init.sgrt_cell, sizeof(sgrt_imx_clk_one_cell_data));
    
    /*!< gate */
    imx_clk_init_data(&sgrt_init, &sgrt_imx_clks_gate_oprts, mrt_nullptr);

    /*!< 1. CCGR0 */
    imx_clk_init_data(&sgrt_init, mrt_nullptr, &sprt_ccm->CCGR0);

    ret |= imx_clk_init_gate(IMX6UL_CLK_AIPSTZ1, "aips_tz1", NR_IMX_CCGR0_AIPS_TZ1, "ahb", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_AIPSTZ2, "aips_tz2", NR_IMX_CCGR0_AIPS_TZ2, "ahb", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_APBHDMA, "apbh_dma", NR_IMX_CCGR0_APBHDMA, "bch_podf", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_ASRC_IPG, "asrc_ipg", NR_IMX_CCGR0_ASRC_IPG, "ahb", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_ASRC_MEM, "asrc_mem", NR_IMX_CCGR0_ASRC_MEM, "ahb", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_DCP_CLK, "dcp",	NR_IMX_CCGR0_DCP, "ahb", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_ENET, "enet", NR_IMX_CCGR0_ENET, "ipg", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_ENET_AHB, "enet_ahb", NR_IMX_CCGR0_ENET, "ahb", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_CAN1_IPG, "can1_ipg", NR_IMX_CCGR0_CAN1_IPG, "ipg", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_CAN1_SERIAL, "can1_serial",	NR_IMX_CCGR0_CAN1_SERIAL, "can_podf", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_CAN2_IPG, "can2_ipg", NR_IMX_CCGR0_CAN2_IPG, "ipg", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_CAN2_SERIAL, "can2_serial",	NR_IMX_CCGR0_CAN2_SERIAL, "can_podf", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_GPT2_BUS, "gpt_bus", NR_IMX_CCGR0_GPT2_IPG, "perclk", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_GPT2_SERIAL, "gpt_serial", NR_IMX_CCGR0_GPT2_SERIAL, "perclk", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_UART2_IPG, "uart2_ipg",	NR_IMX_CCGR0_UART2, "ipg", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_UART2_SERIAL, "uart2_serial", NR_IMX_CCGR0_UART2, "uart_podf", &sgrt_init);

    /*!< 2. CCGR1 */
    imx_clk_init_data(&sgrt_init, mrt_nullptr, &sprt_ccm->CCGR1);

    ret |= imx_clk_init_gate(IMX6UL_CLK_ECSPI1, "ecspi1", NR_IMX_CCGR1_ECSPI1, "ecspi_podf", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_ECSPI2, "ecspi2", NR_IMX_CCGR1_ECSPI2, "ecspi_podf", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_ECSPI3, "ecspi3", NR_IMX_CCGR1_ESCPI3, "ecspi_podf", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_ECSPI4, "ecspi4", NR_IMX_CCGR1_ESCPI4, "ecspi_podf", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_ADC2, "adc2", NR_IMX_CCGR1_ADC2, "ipg", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_UART3_IPG, "uart3_ipg", NR_IMX_CCGR1_UART3, "ipg", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_UART3_SERIAL, "uart3_serial", NR_IMX_CCGR1_UART3, "uart_podf", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_EPIT1, "epit1", NR_IMX_CCGR1_EPIT1, "perclk", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_EPIT2, "epit2", NR_IMX_CCGR1_EPIT2, "perclk", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_ADC1, "adc1", NR_IMX_CCGR1_ADC1, "ipg", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_GPT1_BUS, "gpt1_bus", NR_IMX_CCGR1_GPT_BUS, "perclk", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_GPT1_SERIAL, "gpt1_serial", NR_IMX_CCGR1_GPT_SERIAL, "perclk", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_UART4_IPG, "uart4_ipg", NR_IMX_CCGR1_UART4, "ipg", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_UART4_SERIAL, "uart4_serail", NR_IMX_CCGR1_UART4, "uart_podf", &sgrt_init);

    /*!< 3. CCGR2 */
    imx_clk_init_data(&sgrt_init, mrt_nullptr, &sprt_ccm->CCGR2);

    ret |= imx_clk_init_gate(IMX6UL_CLK_CSI, "csi", NR_IMX_CCGR2_CSI, "csi_podf", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_I2C1, "i2c1", NR_IMX_CCGR2_I2C1_SERIAL, "perclk", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_I2C2, "i2c2", NR_IMX_CCGR2_I2C2_SERIAL, "perclk", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_I2C3, "i2c3", NR_IMX_CCGR2_I2C3_SERIAL, "perclk", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_OCOTP, "ocotp", NR_IMX_CCGR2_OCOTP_CTRL, "ipg", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_IOMUXC, "iomuxc", NR_IMX_CCGR2_IOMUXC_IPT, "lcdif_podf", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_LCDIF_APB, "lcdif_apb", NR_IMX_CCGR2_LCD, "axi", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_PXP, "pxp", NR_IMX_CCGR2_PXP, "axi", &sgrt_init);

    /*!< 4. CCGR3 */
    imx_clk_init_data(&sgrt_init, mrt_nullptr, &sprt_ccm->CCGR3);

    ret |= imx_clk_init_gate(IMX6UL_CLK_UART5_IPG, "uart5_ipg", NR_IMX_CCGR3_UART5, "ipg", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_UART5_SERIAL, "uart5_serial", NR_IMX_CCGR3_UART5, "uart_podf", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_EPDC_ACLK, "epdc_aclk", NR_IMX_CCGR3_EPDC, "axi", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_EPDC_PIX, "epdc_pix", NR_IMX_CCGR3_EPDC, "epdc_podf", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_UART6_IPG, "uart6_ipg", NR_IMX_CCGR3_UART6, "ipg", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_UART6_SERIAL, "uart6_serial", NR_IMX_CCGR3_UART6, "uart_podf", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_LCDIF_PIX, "lcdif_pix", NR_IMX_CCGR3_LCDIF1_PIX, "lcdif_podf", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_QSPI, "qspi1", NR_IMX_CCGR3_QSPI, "qspi1_podf", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_WDOG1, "wdog1", NR_IMX_CCGR3_WDOG1, "ipg", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_MMDC_P0_FAST, "mmdc_p0_fast", NR_IMX_CCGR3_MMDC_IPG_P0, "mmdc_podf", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_MMDC_P0_IPG, "mmdc_p0_ipg", NR_IMX_CCGR3_MMDC_IPG_P1, "ipg", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_AXI, "axi", NR_IMX_CCGR3_RAW_GPMI, "axi_podf", &sgrt_init);

    /*!< 5. CCGR4 */
    imx_clk_init_data(&sgrt_init, mrt_nullptr, &sprt_ccm->CCGR4);

    ret |= imx_clk_init_gate(IMX6UL_CLK_PER_BCH, "per_bch", NR_IMX_CCGR4_PL301_BCH, "bch_podf", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_PWM1, "pwm1", NR_IMX_CCGR4_PWM1, "perclk", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_PWM2, "pwm2", NR_IMX_CCGR4_PWM2, "perclk", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_PWM3, "pwm3", NR_IMX_CCGR4_PWM3, "perclk", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_PWM4, "pwm4", NR_IMX_CCGR4_PWM4, "perclk", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_GPMI_BCH_APB, "gpmi_bch_apb", NR_IMX_CCGR4_RAWNAND_BCH_APB, "bch_podf", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_GPMI_BCH, "gpmi_bch", NR_IMX_CCGR4_RAWNAND_GPMI_BCH, "gpmi_podf", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_GPMI_IO, "gpmi_io", NR_IMX_CCGR4_RWNAND_GPMI_IO, "enfc_podf", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_GPMI_APB, "gpmi_apb", NR_IMX_CCGR4_RWNAND_GPMI_APB, "bch_podf", &sgrt_init);

    /*!< CCGR5 */
    imx_clk_init_data(&sgrt_init, mrt_nullptr, &sprt_ccm->CCGR5);

    ret |= imx_clk_init_gate(IMX6UL_CLK_ROM, "rom", NR_IMX_CCGR5_ROM, "ahb", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_SDMA, "sdma", NR_IMX_CCGR5_SDMA, "ahb", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_WDOG2, "wdog2", NR_IMX_CCGR5_WDOG2, "ipg", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_SPBA, "spba", NR_IMX_CCGR5_SPDA, "ipg", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_SPDIF, "spdif", NR_IMX_CCGR5_SPDIF_AUDIO, "spdif_podf", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_SPDIF_GCLK, "spdif_gclk", NR_IMX_CCGR5_SPDIF_AUDIO, "ipg", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_SAI3, "sai3", NR_IMX_CCGR5_SAI3, "sai3_podf", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_SAI3_IPG, "sai3_ipg", NR_IMX_CCGR5_SAI3, "ipg", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_UART1_IPG, "uart1_ipg", NR_IMX_CCGR5_UART1, "ipg", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_UART1_SERIAL, "uart1_serial", NR_IMX_CCGR5_UART1, "uart_podf", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_UART7_IPG, "uart7_ipg", NR_IMX_CCGR5_UART7, "ipg", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_UART7_SERIAL, "uart7_serial", NR_IMX_CCGR5_UART7, "uart_podf", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_SAI1, "sai1", NR_IMX_CCGR5_SAI1, "sai1_podf", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_SAI1_IPG, "sai1_ipg", NR_IMX_CCGR5_SAI1, "ipg", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_SAI2, "sai2", NR_IMX_CCGR5_SAI2, "sai2_podf", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_SAI2_IPG, "sai2_ipg", NR_IMX_CCGR5_SAI2, "ipg", &sgrt_init);

    /*!< CCGR6 */
    imx_clk_init_data(&sgrt_init, mrt_nullptr, &sprt_ccm->CCGR6);

    ret |= imx_clk_init_gate(IMX6UL_CLK_USBOH3, "usboh3", NR_IMX_CCGR6_USBOH3, "ipg", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_USDHC1, "usdhc1", NR_IMX_CCGR6_USDHC1, "usdhc1_podf", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_USDHC2, "usdhc2", NR_IMX_CCGR6_USDHC2, "usdhc2_podf", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_EIM, "eim", NR_IMX_CCGR6_EIM_SLOW, "eim_slow_podf", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_PWM8, "pwm8", NR_IMX_CCGR6_PWM8, "perclk", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_UART8_IPG, "uart8_ipg", NR_IMX_CCGR6_UART8, "ipg", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_UART8_SERIAL, "uart8_serial", NR_IMX_CCGR6_UART8, "uart_podf", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_AIPSTZ3, "aips_tz3", NR_IMX_CCGR6_AIPS_TZ3, "ahb", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_WDOG3, "wdog3", NR_IMX_CCGR6_WDOG3, "ipg", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_I2C4, "i2c4", NR_IMX_CCGR6_I2C4_SERIAL, "perclk", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_PWM5, "pwm5", NR_IMX_CCGR6_PWM5, "perclk", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_PWM6, "pwm6", NR_IMX_CCGR6_PWM6, "perclk", &sgrt_init);
    ret |= imx_clk_init_gate(IMX6UL_CLK_PWM7, "pwm7", NR_IMX_CCGR6_PWM7, "perclk", &sgrt_init);

    if (ret)
        return -ER_NOMEM;

    /*!< register clock cell */
    return fwk_clk_add_provider(sprt_npccm, fwk_of_clk_src_onecell_get, &sgrt_imx_clk_one_cell_data);
}

/*!
 * @brief   init driver
 * @param   none
 * @retval  errno
 * @note    none
 */
kint32_t __fwk_init imx_clks_driver_init(void)
{
	srt_hal_imx_ccm_t *sprt_ccm;
	srt_hal_imx_ccm_pll_t *sprt_pll;
	struct fwk_device_node *sprt_clks, *sprt_anatop;
    struct imx_clks_data sgrt_data;
    void *reg;

	sprt_anatop = fwk_of_find_matching_node_and_match(mrt_nullptr, sgrt_imx_antop_driver_ids, mrt_nullptr);
    sprt_clks = fwk_of_find_matching_node_and_match(mrt_nullptr, sgrt_imx_ccm_driver_ids, mrt_nullptr);
	if (!isValid(sprt_anatop) || !isValid(sprt_clks))
		return -ER_NOTFOUND;

    reg = fwk_of_iomap(sprt_anatop, 0);
    sprt_pll = (srt_hal_imx_ccm_pll_t *)fwk_io_remap(reg);

	reg = fwk_of_iomap(sprt_clks, 0);
    sprt_ccm = (srt_hal_imx_ccm_t *)fwk_io_remap(reg);

    if (!isValid(sprt_pll) || !isValid(sprt_ccm))
        return -ER_UNVALID;

    sgrt_data.sprt_ccm = sprt_ccm;
    sgrt_data.sprt_pll = sprt_pll;
    sgrt_data.sprt_clks = sprt_clks;
    sgrt_data.sprt_anatop = sprt_anatop;

    imx_clks_driver_of_init(&sgrt_data);

/*!< ----------------------------------------------------------------------------------
 * Part of the clock has been initialized in boot, and the rest is supplemented here 
 * --------------------------------------------------------------------------------- */
    /*!< 1. gpio */
    /*!< gpio1 */
    mrt_setbitl(IMX6UL_CCM_CCGR_BIT(13), &sprt_ccm->CCGR1);
    /*!< gpio2 */
    mrt_setbitl(IMX6UL_CCM_CCGR_BIT(15), &sprt_ccm->CCGR0);
    /*!< gpio3 */
    mrt_setbitl(IMX6UL_CCM_CCGR_BIT(13), &sprt_ccm->CCGR2);
    /*!< gpio4 */
    mrt_setbitl(IMX6UL_CCM_CCGR_BIT(6), &sprt_ccm->CCGR3);
    /*!< gpio5 */
    mrt_setbitl(IMX6UL_CCM_CCGR_BIT(15), &sprt_ccm->CCGR1);

    /*!< 2. lcdif */
    imx_clks_video_init(&sgrt_data);

    return ER_NORMAL;
}

/*!
 * @brief   exit driver
 * @param   none
 * @retval  none
 * @note    none
 */
void __fwk_exit imx_clks_driver_exit(void)
{

}

IMPORT_LATE_INIT(imx_clks_driver_init);
IMPORT_LATE_EXIT(imx_clks_driver_exit);

/*!< end of file */
