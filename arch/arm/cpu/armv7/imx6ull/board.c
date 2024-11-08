/*
 * IMX6ULL Board General Initial
 *
 * File Name:   board.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.09.10
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <common/generic.h>
#include <boot/board_init.h>
#include <asm/armv7/gcc_config.h>
#include <asm/imx6/imx6ull_clocks.h>
#include <asm/imx6/imx6ull_pins.h>

/*!< The defines */
#define IMX_CCM_PORT_ENTRY()								IMX6UL_CCM_PROPERTY_ENTRY()
#define IMX_CCM_PLL_PORT_ENTRY()							IMX6UL_CCM_PLL_PROPERTY_ENTRY()

/*!<
 * PLL1 (PLL_ARM): ARM Core Clock is from ARM_PLL, Maximum frequency can be up to 1.3GHz
 * 		PLL_ARM = pll1_sw_clk
 *			source1: pll1_main_clk (PLL1)
 *			source2: step_clk
 *	 			step_clk ---> source1: osc_clk, 24MHz
 *	 			step_clk ---> source2: secondary_clk, PFD2 (400MHz) or PLL2 (528MHz)
 * PLL2 (PLL_SYS): System_PLL, 22 times the freq; ===> PLL2 = 22 * 24MHz = 528MHz
 * 			PLL2 includes PLL2_PFD0 ~ PLL2_PFD3:
 * 			PLL2_PFD0: 352MHz
 * 			PLL2_PFD1: 594MHz
 * 			PLL2_PFD2: 396MHz
 * 			PLL2_PFD3: 200MHz
 * PLL3 (PLL_USB1): Used to USB1PHY. Including PLL2_PFD0 ~ PLL2_PFD3. ===> PLL3 = 20 * 24MHz = 480MHz
 * 			PLL3_PFD0: 720MHz
 * 			PLL3_PFD1: 540MHz
 * 			PLL3_PFD2: 508.2MHz
 * 			PLL3_PFD3: 454.7MHz
 * PLL7 (PLL_USB2): Used to USB2PHY. PLL7 = 20 * 24MHz = 480MHz
 * PLL6 (PLL_ENET): Used to ENET. PLL6 = (20 + (5/6)) * 24MHz = 500MHz
 * PLL5 (PLL_VIDEO): Used to Video, like LCD. PLL5 = 650MHz ~ 1300MHz, divider = 1/2/4/8/16
 * PLL4 (PLL_AUDIO): Used to Audio. PLL5 = 650MHz ~ 1300MHz, divider = 1/2/4
 */

/*!< API function */
/*!
 * @brief  	imx6ull_main_freq_configure
 * @param  	none
 * @retval 	none
 * @note   	configure main frequency(PLL1)
 */
static void imx6ull_main_freq_configure(void)
{
    srt_hal_imx_ccm_t *sprt_clkRegs;
    srt_hal_imx_ccm_pll_t *sprt_ccmPll;

    sprt_clkRegs = IMX_CCM_PORT_ENTRY();
    sprt_ccmPll  = IMX_CCM_PLL_PORT_ENTRY();

    /*!<
     * CCSR:
     * bit8:		STEP_SEL. 0: derive clock from osc_clk (24M) - source for lp_apm
     * 						  1: derive clock from secondary_clk
     * bit3:		SECONDARY_CLK_SEL. 0: PLL2 PFD2 (400 M); 1: PLL2 (528 M)
     * bit2:		PLL1_SW_CLK_SEL. 0: pll1_main_clk; 1: step_clk
     * bit0:		PLL3_SW_CLK_SEL. 0: pll3_main_clk; 1: pll3 bypass clock
     */
    mrt_clrbitl(mrt_bit(8), &sprt_clkRegs->CCSR);
    mrt_setbitl(mrt_bit(2), &sprt_clkRegs->CCSR);
    while (!mrt_isBitSetl(mrt_bit(2), &sprt_clkRegs->CCSR));

    /*!< 
     * PLL_ARMn:
     * bit[15:14]:  0x0, REF_CLK_24M — Select the 24MHz oscillator as source.
     *				0x1, CLK1 — Select the CLK1_N / CLK1_P as source.
     *				0x2, 0x3, Reserved
     * bit13:		Enable the clock output
     * bit12:		Power down the PLL
     * bit[6:0]:	div_select. PLL divider, Fout = Fin * div_select / 2.0
     */
    mrt_clrbitl(mrt_bit(13), &sprt_ccmPll->PLL_ARM);

    /*!< 
     * CACRR
     * bit[2:0]:	ARM_PODF. divide by (ARM_PODF + 1), ARM_PODF = 000, 001, ..., 111
     */
    mrt_clrbitl(mrt_bit(0) | mrt_bit(1) | mrt_bit(2), &sprt_clkRegs->CACRR);
    mrt_setbitl(mrt_bit(0), &sprt_clkRegs->CACRR);

    /*!<
     * Fin = osc_clk = 24MHz. Because Main_Freq = PLL1 devided (ARM_PODF + 1), if target Main_Freq = 528MHz,
     * ARM_PODF = 1, the PLL1 should be configure 1056MHz. That is to say, Fout = 1056MHz.
     * ===> div_select = (1056 * 2.0) / 24 = 88
     */
    mrt_clrbitl(0x7f, &sprt_ccmPll->PLL_ARM);
    mrt_setbitl(88, &sprt_ccmPll->PLL_ARM);

    /*!< delay nop */
    mrt_delay_nop();

    /*!< Enable PLL */
    mrt_setbitl(mrt_bit(13), &sprt_ccmPll->PLL_ARM);

    /*!< switch pll1_sw_clk to PLL1 */
    mrt_clrbitl(mrt_bit(2), &sprt_clkRegs->CCSR);
    while (!mrt_isBitResetl(mrt_bit(2), &sprt_clkRegs->CCSR));

    /*!< delay nop */
    mrt_delay_nop();
}

/*!
 * @brief  	imx6ull_sys_freq_configure
 * @param  	none
 * @retval 	none
 * @note   	configure sys frequency(PLL2)
 */
static void imx6ull_sys_freq_configure(void)
{
    srt_hal_imx_ccm_pll_t *sprt_ccmPll;
    kuint32_t reg_temp;

    sprt_ccmPll = IMX_CCM_PLL_PORT_ENTRY();

    /*!< 
     * PLL_SYSn:
     * bit[15:14]:  0x0, REF_CLK_24M — Select the 24MHz oscillator as source.
     *				0x1, CLK1 — Select the CLK1_N / CLK1_P as source.
     *				0x2, 0x3, Reserved
     * bit13:		Enable the clock output
     * bit12:		Power down the PLL
     * bit0:		div_select. PLL divider. 0: Fout = Fref * 20; 1: Fout = Fref * 22
     * 
     * PLL_SYS = Fref * 22 = 528MHz
     */
    mrt_setbitl(mrt_bit(0), &sprt_ccmPll->PLL_SYS);
    while (!mrt_isBitSetl(mrt_bit(0), &sprt_ccmPll->PLL_SYS));

    /*!<
     * PFD_528n:
     * The format and usage of PFD3(bit[31:24]), PFD2(bit[23:16]), PFD1(bit[15:8]), PFD0(bit[7:0]) are the same.
     * Take PFD0 for example:
     * 
     * bit7:	 0: ref_pfd0 fractional divider clock is enabled; 1: disable
     * bit6:	 PFD0_STABLE. Indicate if PFD0 is stable.
     * bit[5:0]: PFD0_FRAC. This field controls the fractional divide value. 
     * 			 The resulting frequency shall be 528 * 18 / PFD0_FRAC where PFD0_FRAC is in the range 12 ~ 35
     */
    reg_temp = mrt_readl(&sprt_ccmPll->PFD_528);

    /*!< if set PFD0 = 352MHz, PFD0_FRAC = (528 * 18) / 352 = 27 */
    mrt_clrbitl(0x3f << 0, &reg_temp);
    mrt_setbitl(27   << 0, &reg_temp);

    /*!< if set PFD1 = 594MHz, PFD1_FRAC = (528 * 18) / 594 = 16 */
    mrt_clrbitl(0x3f << 8, &reg_temp);
    mrt_setbitl(16   << 8, &reg_temp);

    /*!< if set PFD2 = 396MHz, PFD2_FRAC = (528 * 18) / 396 = 24 */
    mrt_clrbitl(0x3f << 16, &reg_temp);
    mrt_setbitl(24   << 16, &reg_temp);

    /*!< if set PFD3 = 297MHz, PFD3_FRAC = (528 * 18) / 297 = 32*/
    mrt_clrbitl(0x3f << 24, &reg_temp);
    mrt_setbitl(32   << 24, &reg_temp);

    /*!< update PFD_528 register */
    mrt_writel(reg_temp, &sprt_ccmPll->PFD_528);

    /*!< delay nop */
    mrt_delay_nop();
}

/*!
 * @brief  	imx6ull_periph_freq_configure
 * @param  	none
 * @retval 	none
 * @note   	configure periphral frequency(PLL3 and AHB)
 */
static void imx6ull_periph_freq_configure(void)
{
    srt_hal_imx_ccm_t *sprt_clkRegs;
    srt_hal_imx_ccm_pll_t *sprt_ccmPll;
    kuint32_t reg_temp;

    sprt_clkRegs = IMX_CCM_PORT_ENTRY();
    sprt_ccmPll  = IMX_CCM_PLL_PORT_ENTRY();

    /*!< 
     * PLL_USB1n:
     * bit[15:14]:  0x0, REF_CLK_24M — Select the 24MHz oscillator as source.
     *				0x1, CLK1 — Select the CLK1_N / CLK1_P as source.
     *				0x2, GPANAIO; 0x3, CHRG_DET_B
     * bit13:		Enable the PLL clock output
     * bit12:		Power up the PLL. This bit will be set automatically when USBPHY0 remote wakeup event happens
     * bit6:		0, PLL outputs for USBPHYn off; 1, PLL outputs for USBPHYn on
     * bit0:		div_select. PLL divider. 0: Fout = Fref * 20; 1: Fout = Fref * 22
     * 
     * PLL_USB1n = Fref * 20 = 480MHz
     */
    mrt_clrbitl(mrt_bit(0), &sprt_ccmPll->PLL_USB1);
    while (!mrt_isBitResetl(mrt_bit(0), &sprt_ccmPll->PLL_USB1));

    /*!<
     * PFD_480n:
     *	The same as PFD_520n
     */
    reg_temp = mrt_readl(&sprt_ccmPll->PFD_480);

    /*!< if set PFD0 = 720MHz, PFD0_FRAC = (480 * 18) / 720 = 12 */
    mrt_clrbitl(0x3f << 0, &reg_temp);
    mrt_setbitl(12   << 0, &reg_temp);

    /*!< if set PFD1 = 540MHz, PFD1_FRAC = (480 * 18) / 540 = 16 */
    mrt_clrbitl(0x3f << 8, &reg_temp);
    mrt_setbitl(16   << 8, &reg_temp);

    /*!< if set PFD2 = 508.2MHz, PFD2_FRAC = (480 * 18) / 508.2 = 17 */
    mrt_clrbitl(0x3f << 16, &reg_temp);
    mrt_setbitl(17   << 16, &reg_temp);

    /*!< if set PFD3 = 454.7MHz, PFD3_FRAC = (480 * 18) / 454.7 = 19*/
    mrt_clrbitl(0x3f << 24, &reg_temp);
    mrt_setbitl(19   << 24, &reg_temp);

    /*!< update PFD_480 register */
    mrt_writel(reg_temp, &sprt_ccmPll->PFD_480);

    /*!< delay nop */
    mrt_delay_nop();

    /*!< Disable clock unused */
    mrt_writel(0, &sprt_ccmPll->PLL_USB2);
    mrt_writel(0, &sprt_ccmPll->PLL_AUDIO);
    mrt_writel(0, &sprt_ccmPll->PLL_VIDEO);
    mrt_writel(0, &sprt_ccmPll->PLL_ENET);

    /*!< select pll3_main_clk for pll3_sw_clk */
    mrt_clrbitl(mrt_bit(0), &sprt_clkRegs->CCSR);

    /*!< AHB Clock */
    /*!< 
     * CBCMR
     * bit[19:18]: 	PRE_PERIPH_CLK_SEL
     *				00, derive clock from PLL2
     *				01, derive clock from PLL2 PFD2
     *				10, derive clock from PLL2 PFD0
     *				11, derive clock from divided (/2) PLL2 PFD2
     *
     * here select PLL2_PFD2, freq = 396MHz
     * 
     * CBCDR
     * bit25:		PERIPH_CLK_SEL
     * 				0, PLL2 (pll2_main_clk)
     * 				1, derive clock from periph_clk2_clk clock source
     * 
     * here select 0, PLL2
     * 
     * default divider: 3
     * ===> AHB clk = 396MHz / 3 = 132MHz
     */
    mrt_clrbitl(mrt_bit(18) | mrt_bit(19), &sprt_clkRegs->CBCMR);
    mrt_setbitl(mrt_bit(18), &sprt_clkRegs->CBCMR);
    mrt_clrbitl(mrt_bit(25), &sprt_clkRegs->CBCDR);

    /*!< delay nop */
    mrt_delay_nop();

    /*!< 
     * IPG_Clk
     * ipg_clk = AHB_Clk / (IPG_PODF + 1)
     * 
     * CBCDR
     * bit[9:8]:	IPG_PODF. divide by (IPG_PODF + 1) (IPG_PODF = 0x0, 0x1, 0x2, 0x3)
     * 
     * here set IPG_PODF = 0x01, so divider = 2, ipg_clk = 132MHz / 2 = 66MHz
     */
    mrt_clrbitl(mrt_bit(8) | mrt_bit(9), &sprt_clkRegs->CBCDR);
    mrt_setbitl(mrt_bit(8), &sprt_clkRegs->CBCDR);

    /*!< delay nop */
    mrt_delay_nop();

    /*!<
     * PERCLK_CLK
     * perclk_clk = PERCLK_CLK_SEL / (PERCLK_PODF + 1)
     * 
     * CSCMR1
     * bit6:		PERCLK_CLK_SEL. 0: clock from ipg_clk_root; 1, clock from osc
     * bit[5:0]:	PERCLK_PODF. 000000 ~ 000110, and 111111
     * 
     * here set PERCLK_CLK_SEL = 0, PERCLK_PODF = 0, perclk_clk = ipg_clk
     */
    mrt_clrbitl(mrt_bit(6), &sprt_clkRegs->CSCMR1);
    mrt_clrbitl(0x3f, &sprt_clkRegs->CSCMR1);

    /*!< delay nop */
    mrt_delay_nop();
}

/*!
 * @brief  	imx6ull_clk_initial
 * @param  	none
 * @retval 	none
 * @note   	initial all clock
 */
static void imx6ull_clk_initial(void)
{
    kuaddr_t g_iCCM_CGRx[NR_IMX6UL_CCM_CCGR_NUM] =
    {
        [NR_IMX6UL_CCM_CCGR0] = IMX6UL_CCM_CCGR_CLOCK_ENTRY(0),
        [NR_IMX6UL_CCM_CCGR1] = IMX6UL_CCM_CCGR_CLOCK_ENTRY(1),
        [NR_IMX6UL_CCM_CCGR2] = IMX6UL_CCM_CCGR_CLOCK_ENTRY(2),
        [NR_IMX6UL_CCM_CCGR3] = IMX6UL_CCM_CCGR_CLOCK_ENTRY(3),
        [NR_IMX6UL_CCM_CCGR4] = IMX6UL_CCM_CCGR_CLOCK_ENTRY(4),
        [NR_IMX6UL_CCM_CCGR5] = IMX6UL_CCM_CCGR_CLOCK_ENTRY(5),
        [NR_IMX6UL_CCM_CCGR6] = IMX6UL_CCM_CCGR_CLOCK_ENTRY(6),
    };

    /*!< Main Freq */
    imx6ull_main_freq_configure();

    /*!< PLL1 (PLL System) */
    imx6ull_sys_freq_configure();

    /*!< PLL2 (PLL USB1) */
    imx6ull_periph_freq_configure();

    /*!< Enable/Disable Clock */
    /*!< CCM_CCGR0 */
    mrt_writel(IMX6UL_CCM_CCGR_BIT(0)  | IMX6UL_CCM_CCGR_BIT(1)  | 		/*!< aips_tz1 and aips_tz2 */
               IMX6UL_CCM_CCGR_BIT(11), 								/*!< cpu debug */
               g_iCCM_CGRx[NR_IMX6UL_CCM_CCGR0]);

    /*!< CCM_CCGR1 */
    mrt_writel(IMX6UL_CCM_CCGR_BIT(9) | IMX6UL_CCM_CCGR_BIT(14), 		/*!< sim_s and csu */
               g_iCCM_CGRx[NR_IMX6UL_CCM_CCGR1]);

    /*!< CCM_CCGR2 */
    mrt_writel(IMX6UL_CCM_CCGR_BIT(2)  | IMX6UL_CCM_CCGR_BIT(8) | 		/*!< iomuxc_snvs and ipmux1 */
               IMX6UL_CCM_CCGR_BIT(9) | IMX6UL_CCM_CCGR_BIT(10), 		/*!< ipmux2 and ipmux3 */
               g_iCCM_CGRx[NR_IMX6UL_CCM_CCGR2]);

    /*!< CCM_CCGR3 */
    mrt_writel(IMX6UL_CCM_CCGR_BIT(4)  | IMX6UL_CCM_CCGR_BIT(7) |		/*!< CA7 CCM DAP and iomux_ipt_clk_io */
               IMX6UL_CCM_CCGR_BIT(9) | IMX6UL_CCM_CCGR_BIT(10) |		/*!< a7 clkdiv patch and mmdc_core_aclk_fast_core_p0 */
               IMX6UL_CCM_CCGR_BIT(13) | IMX6UL_CCM_CCGR_BIT(14) |		/*!< mmdc_core_ipg_clk_p1 and ocram */
               IMX6UL_CCM_CCGR_BIT(15), 								/*!< iomuxc_snvs_gpr */
               g_iCCM_CGRx[NR_IMX6UL_CCM_CCGR3]);

    /*!< CCM_CCGR4 */
    mrt_writel(IMX6UL_CCM_CCGR_BIT(1)  | IMX6UL_CCM_CCGR_BIT(2)  |		/*!< iomuxc and iomuxc gpr */
               IMX6UL_CCM_CCGR_BIT(3)  | IMX6UL_CCM_CCGR_BIT(4)  |		/*!< sim_cpu and cxapbsyncbridge slave */
               IMX6UL_CCM_CCGR_BIT(6) | IMX6UL_CCM_CCGR_BIT(7), 		/*!< pl301_mx6qper1_bch and pl301_mx6qper2_mainclk */
               g_iCCM_CGRx[NR_IMX6UL_CCM_CCGR4]);

    /*!< CCM_CCGR5 */
    mrt_writel(IMX6UL_CCM_CCGR_BIT(0)  | IMX6UL_CCM_CCGR_BIT(2)  |		/*!< rom and snvs dryice */
               IMX6UL_CCM_CCGR_BIT(8) | IMX6UL_CCM_CCGR_BIT(10), 		/*!< sim_main and snvs_lp */
               g_iCCM_CGRx[NR_IMX6UL_CCM_CCGR5]);

    /*!< CCM_CCGR6 */
    mrt_writel(IMX6UL_CCM_CCGR_BIT(4)  | IMX6UL_CCM_CCGR_BIT(9) |		/*!< ipmux4 and aips_tz3 */
               IMX6UL_CCM_CCGR_BIT(11) | IMX6UL_CCM_CCGR_BIT(14), 		/*!< anadig and csu */
               g_iCCM_CGRx[NR_IMX6UL_CCM_CCGR6]);
}

/*!
 * @brief  	imx6ull_sram_initial
 * @param  	none
 * @retval 	none
 * @note   	void function
 */
static void imx6ull_sram_initial(void)
{

}

/*!
 * @brief  	s_init
 * @param  	none
 * @retval 	none
 * @note   	The first C program, initial lowlevel enviroment
 */
void s_init(void)
{
    /*!< clock init */
    imx6ull_clk_initial();

    /*!< sram init */
    imx6ull_sram_initial();
}

/* end of file */
