/*
 * IMX6ULL CPU part of Pin Configure
 *
 * File Name:   imx6ull_pins.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.09.10
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __IMX6UL_CLOCKS_H
#define __IMX6UL_CLOCKS_H

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The includes */
#include <common/basic_types.h>
#include <common/error_types.h>
#include <common/generic.h>
#include <common/io_stream.h>

#include <imx6/imx6ul_pinfunc.h>
#include <imx6/imx6ull_pinfunc.h>
#include <imx6/imx6ull_pinfunc_snvs.h>

#include <configs/mach_configs.h>

/*!< The defines */
#define IMX6UL_CCM_CCGR_ENABLE                      (0x3)
#define IMX6UL_CCM_CCGR_DISABLE                     (0x0)
#define IMX6UL_CCGR_ENABLE_ALL                      (0xffffffff)

#define IMX6UL_CCM_CCGR_MASK                        (0x3U)
#define IMX6UL_CCM_CCGR_BIT(offset)                 mrt_bit_nr(IMX6UL_CCM_CCGR_MASK, (offset) << 1)

enum __ERT_IMX6UL_CCM_CCGRx
{
    NR_IMX6UL_CCM_CCGR0 = 0,
    NR_IMX6UL_CCM_CCGR1,
    NR_IMX6UL_CCM_CCGR2,
    NR_IMX6UL_CCM_CCGR3,
    NR_IMX6UL_CCM_CCGR4,
    NR_IMX6UL_CCM_CCGR5,
    NR_IMX6UL_CCM_CCGR6,

	NR_IMX6UL_CCM_CCGR_NUM
};

enum __ERT_IMX_CLOCK_GATE
{
    /*!< CCGR0 */
    NR_IMX_CCGR0_AIPS_TZ1 = 0,
    NR_IMX_CCGR0_AIPS_TZ2,
    NR_IMX_CCGR0_APBHDMA,
    NR_IMX_CCGR0_ASRC_IPG,
    NR_IMX_CCGR0_ASRC_MEM,
    NR_IMX_CCGR0_DCP,
    NR_IMX_CCGR0_ENET,
    NR_IMX_CCGR0_CAN1_IPG,
    NR_IMX_CCGR0_CAN1_SERIAL,
    NR_IMX_CCGR0_CAN2_IPG,
    NR_IMX_CCGR0_CAN2_SERIAL,
    NR_IMX_CCGR0_CPU_DBG,
    NR_IMX_CCGR0_GPT2_IPG,
    NR_IMX_CCGR0_GPT2_SERIAL,
    NR_IMX_CCGR0_UART2,
    NR_IMX_CCGR0_GPIO2,

    /*!< CCGR1 */
    NR_IMX_CCGR1_ECSPI1 = 0,
    NR_IMX_CCGR1_ECSPI2,
    NR_IMX_CCGR1_ESCPI3,
    NR_IMX_CCGR1_ESCPI4,
    NR_IMX_CCGR1_ADC2,
    NR_IMX_CCGR1_UART3,
    NR_IMX_CCGR1_EPIT1,
    NR_IMX_CCGR1_EPIT2,
    NR_IMX_CCGR1_ADC1,
    NR_IMX_CCGR1_SIM_S,
    NR_IMX_CCGR1_GPT_BUS,
    NR_IMX_CCGR1_GPT_SERIAL,
    NR_IMX_CCGR1_UART4,
    NR_IMX_CCGR1_GPIO1,
    NR_IMX_CCGR1_CSU,
    NR_IMX_CCGR1_GPIO5,

    /*!< CCGR2 */
    NR_IMX_CCGR2_ESAI = 0,
    NR_IMX_CCGR2_CSI,
    NR_IMX_CCGR2_IOMUXC_SNVS,
    NR_IMX_CCGR2_I2C1_SERIAL,
    NR_IMX_CCGR2_I2C2_SERIAL,
    NR_IMX_CCGR2_I2C3_SERIAL,
    NR_IMX_CCGR2_OCOTP_CTRL,
    NR_IMX_CCGR2_IOMUXC_IPT,
    NR_IMX_CCGR2_IPMUX1,
    NR_IMX_CCGR2_IPMUX2,
    NR_IMX_CCGR2_IPMUX3,
    NR_IMX_CCGR2_IPSYNC_IPG,
    NR_IMX_CCGR2_RSVD1,
    NR_IMX_CCGR2_GPIO3,
    NR_IMX_CCGR2_LCD,
    NR_IMX_CCGR2_PXP,

    /*!< CCGR3 */
    NR_IMX_CCGR3_RSVD1 = 0,
    NR_IMX_CCGR3_UART5,
    NR_IMX_CCGR3_EPDC,
    NR_IMX_CCGR3_UART6,
    NR_IMX_CCGR3_CA7_CCM_DAP,
    NR_IMX_CCGR3_LCDIF1_PIX,
    NR_IMX_CCGR3_GPIO4,
    NR_IMX_CCGR3_QSPI,
    NR_IMX_CCGR3_WDOG1,
    NR_IMX_CCGR3_A7_CLKDIV_PATCH,
    NR_IMX_CCGR3_MMDC_FAST,
    NR_IMX_CCGR3_RSVD2,
    NR_IMX_CCGR3_MMDC_IPG_P0,
    NR_IMX_CCGR3_MMDC_IPG_P1,
    NR_IMX_CCGR3_RAW_GPMI,
    NR_IMX_CCGR3_IOMUXC_SNVS_GPR,

    /*!< CCGR4 */
    NR_IMX_CCGR4_RSVD1 = 0,
    NR_IMX_CCGR4_IOMUXC,
    NR_IMX_CCGR4_IOMUXC_GPR,
    NR_IMX_CCGR4_SIM_CPU,
    NR_IMX_CCGR4_CXAP_SLAVE,
    NR_IMX_CCGR4_TSC_DIG,
    NR_IMX_CCGR4_PL301_BCH,
    NR_IMX_CCGR4_PL301_MAIN,
    NR_IMX_CCGR4_PWM1,
    NR_IMX_CCGR4_PWM2,
    NR_IMX_CCGR4_PWM3,
    NR_IMX_CCGR4_PWM4,
    NR_IMX_CCGR4_RAWNAND_BCH_APB,
    NR_IMX_CCGR4_RAWNAND_GPMI_BCH,
    NR_IMX_CCGR4_RWNAND_GPMI_IO,
    NR_IMX_CCGR4_RWNAND_GPMI_APB,

    /*!< CCGR5 */
    NR_IMX_CCGR5_ROM = 0,
    NR_IMX_CCGR5_SCTR,
    NR_IMX_CCGR5_SNVS_DRYICE,
    NR_IMX_CCGR5_SDMA,
    NR_IMX_CCGR5_KPP,
    NR_IMX_CCGR5_WDOG2,
    NR_IMX_CCGR5_SPDA,
    NR_IMX_CCGR5_SPDIF_AUDIO,
    NR_IMX_CCGR5_SIM_MAIN,
    NR_IMX_CCGR5_SNVS_HP,
    NR_IMX_CCGR5_SNVS_LP,
    NR_IMX_CCGR5_SAI3,
    NR_IMX_CCGR5_UART1,
    NR_IMX_CCGR5_UART7,
    NR_IMX_CCGR5_SAI1,
    NR_IMX_CCGR5_SAI2,

    /*!< CCGR6 */
    NR_IMX_CCGR6_USBOH3 = 0,
    NR_IMX_CCGR6_USDHC1,
    NR_IMX_CCGR6_USDHC2,
    NR_IMX_CCGR6_RSVD1,
    NR_IMX_CCGR6_IPMUX4,
    NR_IMX_CCGR6_EIM_SLOW,
    NR_IMX_CCGR6_UART_DEBUG,
    NR_IMX_CCGR6_UART8,
    NR_IMX_CCGR6_PWM8,
    NR_IMX_CCGR6_AIPS_TZ3,
    NR_IMX_CCGR6_WDOG3,
    NR_IMX_CCGR6_ANADIG,
    NR_IMX_CCGR6_I2C4_SERIAL,
    NR_IMX_CCGR6_PWM5,
    NR_IMX_CCGR6_PWM6,
    NR_IMX_CCGR6_PWM7
};

/*!< clock Property */
typedef struct
{
    /*!<
     * bit[1:0]
     * CCGR0: aips_tz1 clocks (aips_tz1_clk_enable)
     * CCGR1: ecspi1 clocks (ecspi1_clk_enable)
     * CCGR2: esai clock (esai_clk_enable)
     * CCGR3: Reserved
     * CCGR4: Reserved
     * CCGR5: rom clock (rom_clk_enable)
     * CCGR6: usboh3 clock (usboh3_clk_enable)
     */
    kuint32_t CG0 : 2;

    /*!<
     * bit[3:2]
     * CCGR0: aips_tz2 clocks (aips_tz2_clk_enable)
     * CCGR1: ecspi2 clocks (ecspi2_clk_enable)
     * CCGR2: csi clock (csi_clk_enable)
     * CCGR3: uart5 clock (uart5_clk_enable)
     * CCGR4: iomuxc clock (iomuxc_clk_enable)
     * CCGR5: sctr clock (sctr_clk_enable)
     * CCGR6: usdhc1 clocks (usdhc1_clk_enable)
     */
    kuint32_t CG1 : 2;

    /*!<
     * bit[5:4]
     * CCGR0: apbhdma hclk clock (apbhdma_hclk_enable)
     * CCGR1: ecspi3 clocks (ecspi3_clk_enable)
     * CCGR2: iomuxc_snvs clock (iomuxc_snvs_clk_enable)
     * CCGR3: epdc clock (epdc_clk_enable)
     * CCGR4: iomuxc gpr clock (iomuxc_gpr_clk_enable)
     * CCGR5: snvs dryice clock (snvs_dryice_clk_enable)
     * CCGR6: usdhc2 clocks (usdhc2_clk_enable)
     */
    kuint32_t CG2 : 2;

    /*!<
     * bit[7:6]
     * CCGR0: asrc clock (asrc_clk_enable)
     * CCGR1: ecspi4 clocks (ecspi4_clk_enable)
     * CCGR2: i2c1_serial clock (i2c1_serial_clk_enable)
     * CCGR3: uart6 clock (uart6_clk_enable)
     * CCGR4: sim_cpu clock(sim_cpu_clk_enable)
     * CCGR5: sdma clock (sdma_clk_enable)
     * CCGR6: Reserved
     */
    kuint32_t CG3 : 2;

    /*!<
     * bit[9:8]
     * CCGR0: Reserved
     * CCGR1: adc2 clock (adc2_clk_enable)
     * CCGR2: i2c2_serial clock (i2c2_serial_clk_enable)
     * CCGR3: CA7 CCM DAP clock (ccm_dap_clk_enable)
     * CCGR4: cxapbsyncbridge slave clock (cxapbsyncbridge_slave_clk_enable)
     * CCGR5: kpp clock (kpp_clk_enable)
     * CCGR6: ipmux4 clock (ipmux4_clk_enable)
     */
    kuint32_t CG4 : 2;

    /*!<
     * bit[11:10]
     * CCGR0: dcp clock (dcp_clk_enable)
     * CCGR1: uart3 clock (uart3_clk_enable)
     * CCGR2: i2c3_serial clock (i2c3_serial_clk_enable)
     * CCGR3: lcdif1 pix clock (lcdif1_pix_clk_enable)
     * CCGR4: tsc_dig clock (tsc_clk_enable)
     * CCGR5: wdog2 clock (wdog2_clk_enable)
     * CCGR6: eim_slow clocks (eim_slow_clk_enable)
     */
    kuint32_t CG5 : 2;

    /*!<
     * bit[13:12]
     * CCGR0: enet clock (enet_clk_enable)
     * CCGR1: epit1 clocks (epit1_clk_enable)
     * CCGR2: OCOTP_CTRL clock (iim_clk_enable)
     * CCGR3: gpio4 clock (gpio4_clk_enable)
     * CCGR4: pl301_mx6qper1_bch clocks (pl301_mx6qper1_bchclk_enable) ( This gates bch_clk_root to sim_m fabric.)
     * CCGR5: spba clock (spba_clk_enable)
     * CCGR6: uart debug req gate (This is not a clock, but a gate for the debug request signal to reach the UART module)
     */
    kuint32_t CG6 : 2;

    /*!<
     * bit[15:14]
     * CCGR0: can1 clock (can1_clk_enable)
     * CCGR1: epit2 clocks (epit2_clk_enable)
     * CCGR2: iomux_ipt_clk_io clock (iomux_ipt_clk_io_enable)
     * CCGR3: qspi clock (qspi_clk_enable)
     * CCGR4: pl301_mx6qper2_mainclk_enable (pl301_mx6qper2_mainclk_enable)
     * CCGR5: spdif / audio clock (spdif_clk_enable, audio_clk_root)
     * CCGR6: uart8 clocks (uart8_clk_enable)
     */
    kuint32_t CG7 : 2;

    /*!<
     * bit[17:16]
     * CCGR0: can1_serial clock (can1_serial_clk_enable)
     * CCGR1: adc1 clock (adc1_clk_enable)
     * CCGR2: ipmux1 clock (ipmux1_clk_enable)
     * CCGR3: wdog1 clock (wdog1_clk_enable)
     * CCGR4: pwm1 clocks (pwm1_clk_enable)
     * CCGR5: sim_main clock (sim_main_clk_enable)
     * CCGR6: pwm8 clocks (pwm8_clk_enable)
     */
    kuint32_t CG8 : 2;

    /*!<
     * bit[19:18]
     * CCGR0: can2 clock (can2_clk_enable)
     * CCGR1: sim_s clock (sim_s_clk_enable)
     * CCGR2: ipmux2 clock (ipmux2_clk_enable)
     * CCGR3: a7 clkdiv patch clock (a7_clkdiv_patch_clk_enable)
     * CCGR4: pwm2 clocks (pwm2_clk_enable)
     * CCGR5: snvs_hp clock (snvs_hp_clk_enable)
     * CCGR6: aips_tz3 clock (aips_tz3_clk_enable)
     */
    kuint32_t CG9 : 2;

    /*!<
     * bit[21:20]
     * CCGR0: can2_serial clock (can2_serial_clk_enable)
     * CCGR1: gpt bus clock (gpt_clk_enable)
     * CCGR2: ipmux3 clock (ipmux3_clk_enable)
     * CCGR3: mmdc_core_aclk_fast_core_p0 clock (mmdc_core_aclk_fast_core_p0_enable)
     * CCGR4: pwm3 clocks (pwm3_clk_enable)
     * CCGR5: snvs_lp clock (snvs_lp_clk_enable)
     * CCGR6: wdog3 clock (wdog3_clk_enable)
     */
    kuint32_t CG10 : 2;

    /*!<
     * bit[23:22]
     * CCGR0: CPU debug clocks (arm_dbg_clk_enable)
     * CCGR1: gpt serial clock (gpt_serial_clk_enable)
     * CCGR2: ipsync_ip2apb_tzasc1_ipg clocks (ipsync_ip2apb_tzasc1_ipg_master_clk_enable)
     * CCGR3: Reserved
     * CCGR4: pwm4 clocks (pwm4_clk_enable)
     * CCGR5: sai3 clock (sai3_clk_enable)
     * CCGR6: anadig clocks (anadig_clk_enable)
     */
    kuint32_t CG11 : 2;

    /*!<
     * bit[25:24]
     * CCGR0: gpt2 bus clocks (gpt2_bus_clk_enable)
     * CCGR1: uart4 clock (uart4_clk_enable)
     * CCGR2: Reserved
     * CCGR3: mmdc_core_ipg_clk_p0 clock (mmdc_core_ipg_clk_p0_enable)
     * CCGR4: rawnand_u_bch_input_apb clock (rawnand_u_bch_input_apb_clk_enable)
     * CCGR5: uart1 clock (uart1_clk_enable)
     * CCGR6: i2c4 serial clock (i2c4_serial_clk_enable)
     */
    kuint32_t CG12 : 2;

    /*!<
     * bit[27:26]
     * CCGR0: gpt2 serial clocks (gpt2_serial_clk_enable)
     * CCGR1: gpio1 clock (gpio1_clk_enable)
     * CCGR2: gpio3 clock (gpio3_clk_enable)
     * CCGR3: mmdc_core_ipg_clk_p1 clock (mmdc_core_ipg_clk_p1_enable)
     * CCGR4: rawnand_u_gpmi_bch_input_bch clock (rawnand_u_gpmi_bch_input_bch_clk_enable)
     * CCGR5: uart7 clock (uart7_clk_enable)
     * CCGR6: pwm5 clocks (pwm5_clk_enable)
     */
    kuint32_t CG13 : 2;

    /*!<
     * bit[29:28]
     * CCGR0: uart2 clock (uart2_clk_enable)
     * CCGR1: csu clock (csu_clk_enable)
     * CCGR2: lcd clocks (lcd_clk_enable)
     * CCGR3: ocram clock (ocram_clk_enable)
     * CCGR4: rawnand_u_gpmi_bch_input_gpmi_io clock (rawnand_u_gpmi_bch_input_gpmi_io_clk_enable)
     * CCGR5: sai1 clock (sai1_clk_enable)
     * CCGR6: pwm6 clocks (pwm6_clk_enable)
     */
    kuint32_t CG14 : 2;

    /*!<
     * bit[31:30]
     * CCGR0: gpio2_clocks (gpio2_clk_enable)
     * CCGR1: gpio5_clocks (gpio5_clk_enable)
     * CCGR2: pxp clocks (pxp_clk_enable)
     * CCGR3: iomuxc_snvs_gpr clock (iomuxc_snvs_gpr_clk_enable)
     * CCGR4: rawnand_u_gpmi_input_apb clock (rawnand_u_gpmi_input_apb_clk_enable)
     * CCGR5: sai2 clock (sai2_clk_enable)
     * CCGR6: pwm7 clocks (pwm7_clk_enable)
     */
    kuint32_t CG15 : 2;

} srt_imx_ccgr_t;

/*!< union for ccm_ccgr */
UNION_SRT_FORMAT_DECLARE(urt_imx_ccgr, srt_imx_ccgr_t)

/*!< ------------------------------------------------------------------------- */
/*!< CCM - Register Layout Typedef */
typedef struct 
{
    kuint32_t CCR;                                                  /*!< CCM Control Register, offset: 0x0 */
    kuint32_t CCDR;                                                 /*!< CCM Control Divider Register, offset: 0x4 */
    kuint32_t CSR;                                                  /*!< CCM Status Register, offset: 0x8 */
    kuint32_t CCSR;                                                 /*!< CCM Clock Switcher Register, offset: 0xC */
    kuint32_t CACRR;                                                /*!< CCM Arm Clock Root Register, offset: 0x10 */
    kuint32_t CBCDR;                                                /*!< CCM Bus Clock Divider Register, offset: 0x14 */
    kuint32_t CBCMR;                                                /*!< CCM Bus Clock Multiplexer Register, offset: 0x18 */
    kuint32_t CSCMR1;                                               /*!< CCM Serial Clock Multiplexer Register 1, offset: 0x1C */
    kuint32_t CSCMR2;                                               /*!< CCM Serial Clock Multiplexer Register 2, offset: 0x20 */
    kuint32_t CSCDR1;                                               /*!< CCM Serial Clock Divider Register 1, offset: 0x24 */
    kuint32_t CS1CDR;                                               /*!< CCM SAI1 Clock Divider Register, offset: 0x28 */
    kuint32_t CS2CDR;                                               /*!< CCM SAI2 Clock Divider Register, offset: 0x2C */
    kuint32_t CDCDR;                                                /*!< CCM D1 Clock Divider Register, offset: 0x30 */
    kuint32_t CHSCCDR;                                              /*!< CCM HSC Clock Divider Register, offset: 0x34 */
    kuint32_t CSCDR2;                                               /*!< CCM Serial Clock Divider Register 2, offset: 0x38 */
    kuint32_t CSCDR3;                                               /*!< CCM Serial Clock Divider Register 3, offset: 0x3C */
    kuint8_t  RESERVED_0[8];
    kuint32_t CDHIPR;                                               /*!< CCM Divider Handshake In-Process Register, offset: 0x48 */
    kuint8_t  RESERVED_1[8];
    kuint32_t CLPCR;                                                /*!< CCM Low Power Control Register, offset: 0x54 */
    kuint32_t CISR;                                                 /*!< CCM Interrupt Status Register, offset: 0x58 */
    kuint32_t CIMR;                                                 /*!< CCM Interrupt Mask Register, offset: 0x5C */
    kuint32_t CCOSR;                                                /*!< CCM Clock Output Source Register, offset: 0x60 */
    kuint32_t CGPR;                                                 /*!< CCM General Purpose Register, offset: 0x64 */

    kuint32_t CCGR0;                                                /*!< CCM Clock Gating Register 0, offset: 0x68 */
    kuint32_t CCGR1;                                                /*!< CCM Clock Gating Register 1, offset: 0x6C */
    kuint32_t CCGR2;                                                /*!< CCM Clock Gating Register 2, offset: 0x70 */
    kuint32_t CCGR3;                                                /*!< CCM Clock Gating Register 3, offset: 0x74 */
    kuint32_t CCGR4;                                                /*!< CCM Clock Gating Register 4, offset: 0x78 */
    kuint32_t CCGR5;                                                /*!< CCM Clock Gating Register 5, offset: 0x7C */
    kuint32_t CCGR6;                                                /*!< CCM Clock Gating Register 6, offset: 0x80 */

    kuint8_t  RESERVED_2[4];
    kuint32_t CMEOR;                                                /*!< CCM Module Enable Overide Register, offset: 0x88 */
} srt_hal_imx_ccm_t;

/*!< CCM_ANALOG - Register Layout Typedef */
typedef struct 
{
    kuint32_t PLL_ARM;                                              /*!< Analog ARM PLL control Register, offset: 0x0 */
    kuint32_t PLL_ARM_SET;                                          /*!< Analog ARM PLL control Register, offset: 0x4 */
    kuint32_t PLL_ARM_CLR;                                          /*!< Analog ARM PLL control Register, offset: 0x8 */
    kuint32_t PLL_ARM_TOG;                                          /*!< Analog ARM PLL control Register, offset: 0xC */
    kuint32_t PLL_USB1;                                             /*!< Analog USB1 480MHz PLL Control Register, offset: 0x10 */
    kuint32_t PLL_USB1_SET;                                         /*!< Analog USB1 480MHz PLL Control Register, offset: 0x14 */
    kuint32_t PLL_USB1_CLR;                                         /*!< Analog USB1 480MHz PLL Control Register, offset: 0x18 */
    kuint32_t PLL_USB1_TOG;                                         /*!< Analog USB1 480MHz PLL Control Register, offset: 0x1C */
    kuint32_t PLL_USB2;                                             /*!< Analog USB2 480MHz PLL Control Register, offset: 0x20 */
    kuint32_t PLL_USB2_SET;                                         /*!< Analog USB2 480MHz PLL Control Register, offset: 0x24 */
    kuint32_t PLL_USB2_CLR;                                         /*!< Analog USB2 480MHz PLL Control Register, offset: 0x28 */
    kuint32_t PLL_USB2_TOG;                                         /*!< Analog USB2 480MHz PLL Control Register, offset: 0x2C */
    kuint32_t PLL_SYS;                                              /*!< Analog System PLL Control Register, offset: 0x30 */
    kuint32_t PLL_SYS_SET;                                          /*!< Analog System PLL Control Register, offset: 0x34 */
    kuint32_t PLL_SYS_CLR;                                          /*!< Analog System PLL Control Register, offset: 0x38 */
    kuint32_t PLL_SYS_TOG;                                          /*!< Analog System PLL Control Register, offset: 0x3C */
    kuint32_t PLL_SYS_SS;                                           /*!< 528MHz System PLL Spread Spectrum Register, offset: 0x40 */
    kuint8_t  RESERVED_0[12];
    kuint32_t PLL_SYS_NUM;                                          /*!< Numerator of 528MHz System PLL Fractional Loop Divider Register, offset: 0x50 */
    kuint8_t  RESERVED_1[12];
    kuint32_t PLL_SYS_DENOM;                                        /*!< Denominator of 528MHz System PLL Fractional Loop Divider Register, offset: 0x60 */
    kuint8_t  RESERVED_2[12];
    kuint32_t PLL_AUDIO;                                            /*!< Analog Audio PLL control Register, offset: 0x70 */
    kuint32_t PLL_AUDIO_SET;                                        /*!< Analog Audio PLL control Register, offset: 0x74 */
    kuint32_t PLL_AUDIO_CLR;                                        /*!< Analog Audio PLL control Register, offset: 0x78 */
    kuint32_t PLL_AUDIO_TOG;                                        /*!< Analog Audio PLL control Register, offset: 0x7C */
    kuint32_t PLL_AUDIO_NUM;                                        /*!< Numerator of Audio PLL Fractional Loop Divider Register, offset: 0x80 */
    kuint8_t  RESERVED_3[12];
    kuint32_t PLL_AUDIO_DENOM;                                      /*!< Denominator of Audio PLL Fractional Loop Divider Register, offset: 0x90 */
    kuint8_t  RESERVED_4[12];
    kuint32_t PLL_VIDEO;                                            /*!< Analog Video PLL control Register, offset: 0xA0 */
    kuint32_t PLL_VIDEO_SET;                                        /*!< Analog Video PLL control Register, offset: 0xA4 */
    kuint32_t PLL_VIDEO_CLR;                                        /*!< Analog Video PLL control Register, offset: 0xA8 */
    kuint32_t PLL_VIDEO_TOG;                                        /*!< Analog Video PLL control Register, offset: 0xAC */
    kuint32_t PLL_VIDEO_NUM;                                        /*!< Numerator of Video PLL Fractional Loop Divider Register, offset: 0xB0 */
    kuint8_t  RESERVED_5[12];
    kuint32_t PLL_VIDEO_DENOM;                                      /*!< Denominator of Video PLL Fractional Loop Divider Register, offset: 0xC0 */
    kuint8_t  RESERVED_6[28];
    kuint32_t PLL_ENET;                                             /*!< Analog ENET PLL Control Register, offset: 0xE0 */
    kuint32_t PLL_ENET_SET;                                         /*!< Analog ENET PLL Control Register, offset: 0xE4 */
    kuint32_t PLL_ENET_CLR;                                         /*!< Analog ENET PLL Control Register, offset: 0xE8 */
    kuint32_t PLL_ENET_TOG;                                         /*!< Analog ENET PLL Control Register, offset: 0xEC */
    kuint32_t PFD_480;                                              /*!< 480MHz Clock (PLL3) Phase Fractional Divider Control Register, offset: 0xF0 */
    kuint32_t PFD_480_SET;                                          /*!< 480MHz Clock (PLL3) Phase Fractional Divider Control Register, offset: 0xF4 */
    kuint32_t PFD_480_CLR;                                          /*!< 480MHz Clock (PLL3) Phase Fractional Divider Control Register, offset: 0xF8 */
    kuint32_t PFD_480_TOG;                                          /*!< 480MHz Clock (PLL3) Phase Fractional Divider Control Register, offset: 0xFC */
    kuint32_t PFD_528;                                              /*!< 528MHz Clock (PLL2) Phase Fractional Divider Control Register, offset: 0x100 */
    kuint32_t PFD_528_SET;                                          /*!< 528MHz Clock (PLL2) Phase Fractional Divider Control Register, offset: 0x104 */
    kuint32_t PFD_528_CLR;                                          /*!< 528MHz Clock (PLL2) Phase Fractional Divider Control Register, offset: 0x108 */
    kuint32_t PFD_528_TOG;                                          /*!< 528MHz Clock (PLL2) Phase Fractional Divider Control Register, offset: 0x10C */
    kuint8_t  RESERVED_7[64];
    kuint32_t MISC0;                                                /*!< Miscellaneous Register 0, offset: 0x150 */
    kuint32_t MISC0_SET;                                            /*!< Miscellaneous Register 0, offset: 0x154 */
    kuint32_t MISC0_CLR;                                            /*!< Miscellaneous Register 0, offset: 0x158 */
    kuint32_t MISC0_TOG;                                            /*!< Miscellaneous Register 0, offset: 0x15C */
    kuint32_t MISC1;                                                /*!< Miscellaneous Register 1, offset: 0x160 */
    kuint32_t MISC1_SET;                                            /*!< Miscellaneous Register 1, offset: 0x164 */
    kuint32_t MISC1_CLR;                                            /*!< Miscellaneous Register 1, offset: 0x168 */
    kuint32_t MISC1_TOG;                                            /*!< Miscellaneous Register 1, offset: 0x16C */
    kuint32_t MISC2;                                                /*!< Miscellaneous Register 2, offset: 0x170 */
    kuint32_t MISC2_SET;                                            /*!< Miscellaneous Register 2, offset: 0x174 */
    kuint32_t MISC2_CLR;                                            /*!< Miscellaneous Register 2, offset: 0x178 */
    kuint32_t MISC2_TOG;                                            /*!< Miscellaneous Register 2, offset: 0x17C */
} srt_hal_imx_ccm_pll_t;

/*!< ------------------------------------------------------------------------- */
/*!< Register Property Base Address */
/*!< Peripheral CCM base address */
#define IMX6UL_CCM_ADDR_BASE                        (0x20C4000u)
#define IMX6UL_CCM_CCGR0_ADDR_OFFSET 			    mrt_member_offset(srt_hal_imx_ccm_t, CCGR0)
#define IMX6UL_CCM_CCGR1_ADDR_OFFSET 			    mrt_member_offset(srt_hal_imx_ccm_t, CCGR1)
#define IMX6UL_CCM_CCGR2_ADDR_OFFSET 			    mrt_member_offset(srt_hal_imx_ccm_t, CCGR2)
#define IMX6UL_CCM_CCGR3_ADDR_OFFSET 			    mrt_member_offset(srt_hal_imx_ccm_t, CCGR3)
#define IMX6UL_CCM_CCGR4_ADDR_OFFSET 			    mrt_member_offset(srt_hal_imx_ccm_t, CCGR4)
#define IMX6UL_CCM_CCGR5_ADDR_OFFSET 			    mrt_member_offset(srt_hal_imx_ccm_t, CCGR5)
#define IMX6UL_CCM_CCGR6_ADDR_OFFSET 			    mrt_member_offset(srt_hal_imx_ccm_t, CCGR6)
#define IMX6UL_CCM_CCGR_CLOCK_ENTRY(x)              (IMX6UL_CCM_ADDR_BASE + IMX6UL_CCM_CCGR##x##_ADDR_OFFSET)

#define mrt_imx_ccm_configure(port, data)   \
do {   \
    *((volatile kuaddr_t *)(IMX6UL_CCM_ADDR_BASE + IMX6UL_CCM_CCGR##port##_ADDR_OFFSET)) = (data);  \
} while(0)

#define IMX6UL_CCM_PROPERTY_ENTRY()                 (srt_hal_imx_ccm_t      *)IMX6UL_CCM_ADDR_BASE

/*!< Peripheral CCM_ANALOG base address */
#define IMX6UL_CCM_ANALOG_BASE                      (0x20C8000u)
#define IMX6UL_CCM_PLL_PROPERTY_ENTRY()             (srt_hal_imx_ccm_pll_t  *)IMX6UL_CCM_ANALOG_BASE

/*!< enable clk */
#define mrt_imx_ccm_clk_enable(reg, ccgr)    \
do {   \
    urt_imx_ccgr_t ugrt_ccgr;   \
	mrt_write_urt_data(&ugrt_ccgr, mrt_readl(ccgr));   \
	mrt_write_urt_bits(&ugrt_ccgr, reg, IMX6UL_CCM_CCGR_ENABLE);  \
	mrt_writel(mrt_trans_urt_data(&ugrt_ccgr), ccgr);  \
} while(0)

/*!< disable clk */
#define mrt_imx_ccm_clk_disable(uprt_ccgr, reg, ccgr)    \
do {   \
    urt_imx_ccgr_t ugrt_ccgr;   \
	mrt_write_urt_data(&ugrt_ccgr, mrt_readl(ccgr));   \
	mrt_write_urt_bits(&ugrt_ccgr, reg, IMX6UL_CCM_CCGR_DISABLE);  \
	mrt_writel(mrt_trans_urt_data(&ugrt_ccgr), ccgr);  \
} while (0)

/*!< ------------------------------------------------------------------------- */
/*!< Usual defines */
#define IMX6UL_SYSCLK_DIVID_FAC1                    ((((srt_hal_imx_ccm_t      *)IMX6UL_CCM_ADDR_BASE)->CACRR & 0x07) + 1)
#define IMX6UL_SYSCLK_DIVID_FAC2                    (((srt_hal_imx_ccm_pll_t   *)IMX6UL_CCM_ANALOG_BASE)->PLL_ARM & 0x7f)

/*!< Define Sys_Clk by Inline function */
static inline kuint64_t hal_imx_sysclk_freq_counter(void)
{
    return (kuint64_t)(((CONFIG_XTAL_FREQ_CLK * IMX6UL_SYSCLK_DIVID_FAC2) / IMX6UL_SYSCLK_DIVID_FAC1) >> 1);
}

/*!< Define Sys_Clk by Macro Definition */
#define mrt_imx_sysclk_freq_counter \
    ((kuint64_t)(((CONFIG_XTAL_FREQ_CLK * IMX6UL_SYSCLK_DIVID_FAC2) / IMX6UL_SYSCLK_DIVID_FAC1) >> 1))

#define IMX_SYSCLK_FREQ_COUNTER                     mrt_imx_sysclk_freq_counter

/* clks for device-tree */
#define IMX6UL_CLK_DUMMY		                    0
#define IMX6UL_CLK_CKIL			                    1
#define IMX6UL_CLK_CKIH			                    2
#define IMX6UL_CLK_OSC			                    3
#define IMX6UL_PLL1_BYPASS_SRC		                4
#define IMX6UL_PLL2_BYPASS_SRC		                5
#define IMX6UL_PLL3_BYPASS_SRC		                6
#define IMX6UL_PLL4_BYPASS_SRC		                7
#define IMX6UL_PLL5_BYPASS_SRC		                8
#define IMX6UL_PLL6_BYPASS_SRC		                9
#define IMX6UL_PLL7_BYPASS_SRC		                10
#define IMX6UL_CLK_PLL1			                    11
#define IMX6UL_CLK_PLL2			                    12
#define IMX6UL_CLK_PLL3			                    13
#define IMX6UL_CLK_PLL4			                    14
#define IMX6UL_CLK_PLL5			                    15
#define IMX6UL_CLK_PLL6			                    16
#define IMX6UL_CLK_PLL7			                    17
#define IMX6UL_PLL1_BYPASS		                    18
#define IMX6UL_PLL2_BYPASS		                    19
#define IMX6UL_PLL3_BYPASS		                    20
#define IMX6UL_PLL4_BYPASS		                    21
#define IMX6UL_PLL5_BYPASS		                    22
#define IMX6UL_PLL6_BYPASS		                    23
#define IMX6UL_PLL7_BYPASS		                    24
#define IMX6UL_CLK_PLL1_SYS		                    25
#define IMX6UL_CLK_PLL2_BUS		                    26
#define IMX6UL_CLK_PLL3_USB_OTG		                27
#define IMX6UL_CLK_PLL4_AUDIO		                28
#define IMX6UL_CLK_PLL5_VIDEO		                29
#define IMX6UL_CLK_PLL6_ENET		                30
#define IMX6UL_CLK_PLL7_USB_HOST	                31
#define IMX6UL_CLK_USBPHY1		                    32
#define IMX6UL_CLK_USBPHY2		                    33
#define IMX6UL_CLK_USBPHY1_GATE		                34
#define IMX6UL_CLK_USBPHY2_GATE		                35
#define IMX6UL_CLK_PLL2_PFD0		                36
#define IMX6UL_CLK_PLL2_PFD1		                37
#define IMX6UL_CLK_PLL2_PFD2		                38
#define IMX6UL_CLK_PLL2_PFD3		                39
#define IMX6UL_CLK_PLL3_PFD0		                40
#define IMX6UL_CLK_PLL3_PFD1		                41
#define IMX6UL_CLK_PLL3_PFD2		                42
#define IMX6UL_CLK_PLL3_PFD3		                43
#define IMX6UL_CLK_ENET_REF		                    44
#define IMX6UL_CLK_ENET2_REF		                45
#define IMX6UL_CLK_ENET2_REF_125M	                46
#define IMX6UL_CLK_ENET_PTP_REF		                47
#define IMX6UL_CLK_ENET_PTP		                    48
#define IMX6UL_CLK_PLL4_POST_DIV	                49
#define IMX6UL_CLK_PLL4_AUDIO_DIV	                50
#define IMX6UL_CLK_PLL5_POST_DIV	                51
#define IMX6UL_CLK_PLL5_VIDEO_DIV	                52
#define IMX6UL_CLK_PLL2_198M		                53
#define IMX6UL_CLK_PLL3_80M		                    54
#define IMX6UL_CLK_PLL3_60M		                    55
#define IMX6UL_CLK_STEP			                    56
#define IMX6UL_CLK_PLL1_SW		                    57
#define IMX6UL_CLK_AXI_ALT_SEL		                58
#define IMX6UL_CLK_AXI_SEL		                    59
#define IMX6UL_CLK_PERIPH_PRE		                60
#define IMX6UL_CLK_PERIPH2_PRE		                61
#define IMX6UL_CLK_PERIPH_CLK2_SEL	                62
#define IMX6UL_CLK_PERIPH2_CLK2_SEL	                63
#define IMX6UL_CLK_USDHC1_SEL		                64
#define IMX6UL_CLK_USDHC2_SEL		                65
#define IMX6UL_CLK_BCH_SEL		                    66
#define IMX6UL_CLK_GPMI_SEL		                    67
#define IMX6UL_CLK_EIM_SLOW_SEL		                68
#define IMX6UL_CLK_SPDIF_SEL		                69
#define IMX6UL_CLK_SAI1_SEL		                    70
#define IMX6UL_CLK_SAI2_SEL		                    71
#define IMX6UL_CLK_SAI3_SEL		                    72
#define IMX6UL_CLK_LCDIF_PRE_SEL	                73
#define IMX6UL_CLK_SIM_PRE_SEL		                74
#define IMX6UL_CLK_LDB_DI0_SEL		                75
#define IMX6UL_CLK_LDB_DI1_SEL		                76
#define IMX6UL_CLK_ENFC_SEL		                    77
#define IMX6UL_CLK_CAN_SEL		                    78
#define IMX6UL_CLK_ECSPI_SEL		                79
#define IMX6UL_CLK_UART_SEL		                    80
#define IMX6UL_CLK_QSPI1_SEL		                81
#define IMX6UL_CLK_PERCLK_SEL		                82
#define IMX6UL_CLK_LCDIF_SEL		                83
#define IMX6UL_CLK_SIM_SEL		                    84
#define IMX6UL_CLK_PERIPH		                    85
#define IMX6UL_CLK_PERIPH2		                    86
#define IMX6UL_CLK_LDB_DI0_DIV_3_5	                87
#define IMX6UL_CLK_LDB_DI0_DIV_7	                88
#define IMX6UL_CLK_LDB_DI1_DIV_3_5	                89
#define IMX6UL_CLK_LDB_DI1_DIV_7	                90
#define IMX6UL_CLK_LDB_DI0_DIV_SEL	                91
#define IMX6UL_CLK_LDB_DI1_DIV_SEL	                92
#define IMX6UL_CLK_ARM			                    93
#define IMX6UL_CLK_PERIPH_CLK2		                94
#define IMX6UL_CLK_PERIPH2_CLK2 	                95
#define IMX6UL_CLK_AHB			                    96
#define IMX6UL_CLK_MMDC_PODF		                97
#define IMX6UL_CLK_AXI_PODF		                    98
#define IMX6UL_CLK_PERCLK		                    99
#define IMX6UL_CLK_IPG			                    100
#define IMX6UL_CLK_USDHC1_PODF		                101
#define IMX6UL_CLK_USDHC2_PODF		                102
#define IMX6UL_CLK_BCH_PODF		                    103
#define IMX6UL_CLK_GPMI_PODF		                104
#define IMX6UL_CLK_EIM_SLOW_PODF	                105
#define IMX6UL_CLK_SPDIF_PRED		                106
#define IMX6UL_CLK_SPDIF_PODF		                107
#define IMX6UL_CLK_SAI1_PRED		                108
#define IMX6UL_CLK_SAI1_PODF		                109
#define IMX6UL_CLK_SAI2_PRED		                110
#define IMX6UL_CLK_SAI2_PODF		                111
#define IMX6UL_CLK_SAI3_PRED		                112
#define IMX6UL_CLK_SAI3_PODF		                113
#define IMX6UL_CLK_LCDIF_PRED		                114
#define IMX6UL_CLK_LCDIF_PODF		                115
#define IMX6UL_CLK_SIM_PODF		                    116
#define IMX6UL_CLK_QSPI1_PDOF		                117
#define IMX6UL_CLK_ENFC_PRED		                118
#define IMX6UL_CLK_ENFC_PODF		                119
#define IMX6UL_CLK_CAN_PODF		                    120
#define IMX6UL_CLK_ECSPI_PODF		                121
#define IMX6UL_CLK_UART_PODF		                122
#define IMX6UL_CLK_ADC1			                    123
#define IMX6UL_CLK_ADC2			                    124

#define IMX6UL_CLK_AIPSTZ1		                    125
#define IMX6UL_CLK_AIPSTZ2		                    126
#define IMX6UL_CLK_AIPSTZ3		                    127
#define IMX6UL_CLK_APBHDMA		                    128
#define IMX6UL_CLK_ASRC_IPG		                    129
#define IMX6UL_CLK_ASRC_MEM		                    130
#define IMX6UL_CLK_GPMI_BCH_APB		                131
#define IMX6UL_CLK_GPMI_BCH		                    132
#define IMX6UL_CLK_GPMI_IO		                    133
#define IMX6UL_CLK_GPMI_APB		                    134
#define IMX6UL_CLK_CAAM_MEM		                    135
#define IMX6UL_CLK_CAAM_ACLK		                136
#define IMX6UL_CLK_CAAM_IPG		                    137
#define IMX6UL_CLK_CSI			                    138
#define IMX6UL_CLK_ECSPI1		                    139
#define IMX6UL_CLK_ECSPI2		                    140
#define IMX6UL_CLK_ECSPI3		                    141
#define IMX6UL_CLK_ECSPI4		                    142
#define IMX6UL_CLK_EIM			                    143
#define IMX6UL_CLK_ENET			                    144
#define IMX6UL_CLK_ENET_AHB		                    145
#define IMX6UL_CLK_EPIT1		                    146
#define IMX6UL_CLK_EPIT2		                    147
#define IMX6UL_CLK_CAN1_IPG		                    148
#define IMX6UL_CLK_CAN1_SERIAL		                149
#define IMX6UL_CLK_CAN2_IPG		                    150
#define IMX6UL_CLK_CAN2_SERIAL		                151
#define IMX6UL_CLK_GPT1_BUS		                    152
#define IMX6UL_CLK_GPT1_SERIAL		                153
#define IMX6UL_CLK_GPT2_BUS		                    154
#define IMX6UL_CLK_GPT2_SERIAL		                155
#define IMX6UL_CLK_I2C1			                    156
#define IMX6UL_CLK_I2C2			                    157
#define IMX6UL_CLK_I2C3			                    158
#define IMX6UL_CLK_I2C4			                    159
#define IMX6UL_CLK_IOMUXC		                    160
#define IMX6UL_CLK_LCDIF_APB		                161
#define IMX6UL_CLK_LCDIF_PIX		                162
#define IMX6UL_CLK_MMDC_P0_FAST		                163
#define IMX6UL_CLK_MMDC_P0_IPG		                164
#define IMX6UL_CLK_OCOTP		                    165
#define IMX6UL_CLK_OCRAM		                    166
#define IMX6UL_CLK_PWM1			                    167
#define IMX6UL_CLK_PWM2			                    168
#define IMX6UL_CLK_PWM3			                    169
#define IMX6UL_CLK_PWM4			                    170
#define IMX6UL_CLK_PWM5			                    171
#define IMX6UL_CLK_PWM6			                    172
#define IMX6UL_CLK_PWM7			                    173
#define IMX6UL_CLK_PWM8			                    174
#define IMX6UL_CLK_PXP			                    175
#define IMX6UL_CLK_QSPI			                    176
#define IMX6UL_CLK_ROM			                    177
#define IMX6UL_CLK_SAI1			                    178
#define IMX6UL_CLK_SAI1_IPG		                    179
#define IMX6UL_CLK_SAI2			                    180
#define IMX6UL_CLK_SAI2_IPG		                    181
#define IMX6UL_CLK_SAI3			                    182
#define IMX6UL_CLK_SAI3_IPG		                    183
#define IMX6UL_CLK_SDMA			                    184
#define IMX6UL_CLK_SIM			                    185
#define IMX6UL_CLK_SIM_S		                    186
#define IMX6UL_CLK_SPBA			                    187
#define IMX6UL_CLK_SPDIF		                    188
#define IMX6UL_CLK_UART1_IPG		                189
#define IMX6UL_CLK_UART1_SERIAL		                190
#define IMX6UL_CLK_UART2_IPG		                191
#define IMX6UL_CLK_UART2_SERIAL		                192
#define IMX6UL_CLK_UART3_IPG		                193
#define IMX6UL_CLK_UART3_SERIAL		                194
#define IMX6UL_CLK_UART4_IPG		                195
#define IMX6UL_CLK_UART4_SERIAL		                196
#define IMX6UL_CLK_UART5_IPG		                197
#define IMX6UL_CLK_UART5_SERIAL		                198
#define IMX6UL_CLK_UART6_IPG		                199
#define IMX6UL_CLK_UART6_SERIAL		                200
#define IMX6UL_CLK_UART7_IPG		                201
#define IMX6UL_CLK_UART7_SERIAL		                202
#define IMX6UL_CLK_UART8_IPG		                203
#define IMX6UL_CLK_UART8_SERIAL		                204
#define IMX6UL_CLK_USBOH3		                    205
#define IMX6UL_CLK_USDHC1		                    206
#define IMX6UL_CLK_USDHC2		                    207
#define IMX6UL_CLK_WDOG1		                    208
#define IMX6UL_CLK_WDOG2		                    209
#define IMX6UL_CLK_WDOG3		                    210
#define IMX6UL_CLK_LDB_DI0		                    211
#define IMX6UL_CLK_AXI			                    212
#define IMX6UL_CLK_SPDIF_GCLK		                213
#define IMX6UL_CLK_GPT_3M		                    214
#define IMX6UL_CLK_SIM2			                    215
#define IMX6UL_CLK_SIM1			                    216
#define IMX6UL_CLK_IPP_DI0		                    217
#define IMX6UL_CLK_IPP_DI1		                    218
#define IMX6UL_CA7_SECONDARY_SEL	                219
#define IMX6UL_CLK_PER_BCH		                    220
#define IMX6UL_CLK_CSI_SEL		                    221
#define IMX6UL_CLK_CSI_PODF		                    222
#define IMX6UL_CLK_PLL3_120M		                223
/* For i.MX6ULL */
#define IMX6UL_CLK_ESAI_SEL		                    224
#define IMX6UL_CLK_ESAI_PRED		                225
#define IMX6UL_CLK_ESAI_PODF		                226
#define IMX6UL_CLK_ESAI_EXTAL		                227
#define IMX6UL_CLK_ESAI_MEM		                    228
#define IMX6UL_CLK_ESAI_IPG		                    229
#define IMX6UL_CLK_DCP_CLK		                    230
#define IMX6UL_CLK_EPDC_PRE_SEL		                231
#define IMX6UL_CLK_EPDC_SEL		                    232
#define IMX6UL_CLK_EPDC_PODF		                233
#define IMX6UL_CLK_EPDC_ACLK		                234
#define IMX6UL_CLK_EPDC_PIX		                    235

#define IMX6UL_CLK_END			                    236

#ifdef __cplusplus
    }
#endif

#endif /* __IMX6UL_CLOCKS_H */
