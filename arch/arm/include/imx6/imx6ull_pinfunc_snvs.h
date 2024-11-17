/*
 * IMX6ULL Pin Function Defines
 *
 * File Name:   imx6ull_pinfunc_snvs.h
 * Author:      Yang Yujun (Copy from "Linux Kernel" --> "arch/arm/boot/dts")
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.09.30
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __IMX6ULL_PINFUNC_SNVS_H
#define __IMX6ULL_PINFUNC_SNVS_H

/*
 * The pin function ID is a tuple of
 * <mux_reg conf_reg input_reg mux_mode input_val>
 * for example: 
 *                                                              <mux_reg conf_reg input_reg mux_mode input_val>
 * #define MX6UL_PAD_GPIO1_IO03_GPIO1_IO03                       0x0068  0x02F4   0x0000    0x5        0x0
 *
 * mux_reg  : IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO03, address = 20E_0000h base +  68h offset = 20E_0068h
 * conf_reg : IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO03, address = 20E_0000h base + 2F4h offset = 20E_02F4h
 * input_reg: none, there is no input register for IMX6ULl
 * mux_mode : the value of <mux_reg>, 0x5 (ALT 0101), it means MUX to GPIO1_IO03
 * input_val: the value of <input_reg>, it does not need to be set, because input_reg is empty
 *
 * for pinctrl, the defined value is used to fill <conf_reg>, including pull-up, rate, etc.
 */
#if 0
#define IMX6UL_MUX_BOOT_MODE0_GPIO5_IO10                            0x0000, 0x0044, 0x0000, 0x5, 0x0
#define IMX6UL_MUX_BOOT_MODE1_GPIO5_IO11                            0x0004, 0x0048, 0x0000, 0x5, 0x0
#define IMX6UL_MUX_SNVS_TAMPER0_GPIO5_IO00                          0x0008, 0x004C, 0x0000, 0x5, 0x0
#define IMX6UL_MUX_SNVS_TAMPER1_GPIO5_IO01                          0x000C, 0x0050, 0x0000, 0x5, 0x0
#define IMX6UL_MUX_SNVS_TAMPER2_GPIO5_IO02                          0x0010, 0x0054, 0x0000, 0x5, 0x0
#define IMX6UL_MUX_SNVS_TAMPER3_GPIO5_IO03                          0x0014, 0x0058, 0x0000, 0x5, 0x0
#define IMX6UL_MUX_SNVS_TAMPER4_GPIO5_IO04                          0x0018, 0x005C, 0x0000, 0x5, 0x0
#define IMX6UL_MUX_SNVS_TAMPER5_GPIO5_IO05                          0x001C, 0x0060, 0x0000, 0x5, 0x0
#define IMX6UL_MUX_SNVS_TAMPER6_GPIO5_IO06                          0x0020, 0x0064, 0x0000, 0x5, 0x0
#define IMX6UL_MUX_SNVS_TAMPER7_GPIO5_IO07                          0x0024, 0x0068, 0x0000, 0x5, 0x0
#define IMX6UL_MUX_SNVS_TAMPER8_GPIO5_IO08                          0x0028, 0x006C, 0x0000, 0x5, 0x0
#define IMX6UL_MUX_SNVS_TAMPER9_GPIO5_IO09                          0x002C, 0x0070, 0x0000, 0x5, 0x0
#endif

#endif /* _IMX6ULL_PINFUNC_SNVS_H */
