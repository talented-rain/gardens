/*
 * IMX6ULL Pin Function Defines
 *
 * File Name:   imx6ull_pinfunc.h
 * Author:      Yang Yujun (Copy from "Linux Kernel" --> "arch/arm/boot/dts")
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.09.30
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __IMX6ULL_PINFUNC_H
#define __IMX6ULL_PINFUNC_H

#include "imx6ul_pinfunc.h"

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
#define IMX6UL_MUX_ENET2_RX_DATA0_EPDC_SDDO08                       0x00E4, 0x0370, 0x0000, 0x9, 0x0
#define IMX6UL_MUX_ENET2_RX_DATA1_EPDC_SDDO09                       0x00E8, 0x0374, 0x0000, 0x9, 0x0
#define IMX6UL_MUX_ENET2_RX_EN_EPDC_SDDO10                          0x00EC, 0x0378, 0x0000, 0x9, 0x0
#define IMX6UL_MUX_ENET2_TX_DATA0_EPDC_SDDO11                       0x00F0, 0x037C, 0x0000, 0x9, 0x0
#define IMX6UL_MUX_ENET2_TX_DATA1_EPDC_SDDO12                       0x00F4, 0x0380, 0x0000, 0x9, 0x0
#define IMX6UL_MUX_ENET2_TX_EN_EPDC_SDDO13                          0x00F8, 0x0384, 0x0000, 0x9, 0x0
#define IMX6UL_MUX_ENET2_TX_CLK_EPDC_SDDO14                         0x00FC, 0x0388, 0x0000, 0x9, 0x0
#define IMX6UL_MUX_ENET2_RX_ER_EPDC_SDDO15                          0x0100, 0x038C, 0x0000, 0x9, 0x0
#define IMX6UL_MUX_LCD_CLK_EPDC_SDCLK                               0x0104, 0x0390, 0x0000, 0x9, 0x0
#define IMX6UL_MUX_LCD_ENABLE_EPDC_SDLE                             0x0108, 0x0394, 0x0000, 0x9, 0x0
#define IMX6UL_MUX_LCD_HSYNC_EPDC_SDOE                              0x010C, 0x0398, 0x0000, 0x9, 0x0
#define IMX6UL_MUX_LCD_VSYNC_EPDC_SDCE0                             0x0110, 0x039C, 0x0000, 0x9, 0x0
#define IMX6UL_MUX_LCD_RESET_EPDC_GDOE                              0x0114, 0x03A0, 0x0000, 0x9, 0x0
#define IMX6UL_MUX_LCD_DATA00_EPDC_SDDO00                           0x0118, 0x03A4, 0x0000, 0x9, 0x0
#define IMX6UL_MUX_LCD_DATA01_EPDC_SDDO01                           0x011C, 0x03A8, 0x0000, 0x9, 0x0
#define IMX6UL_MUX_LCD_DATA02_EPDC_SDDO02                           0x0120, 0x03AC, 0x0000, 0x9, 0x0
#define IMX6UL_MUX_LCD_DATA03_EPDC_SDDO03                           0x0124, 0x03B0, 0x0000, 0x9, 0x0
#define IMX6UL_MUX_LCD_DATA04_EPDC_SDDO04                           0x0128, 0x03B4, 0x0000, 0x9, 0x0
#define IMX6UL_MUX_LCD_DATA05_EPDC_SDDO05                           0x012C, 0x03B8, 0x0000, 0x9, 0x0
#define IMX6UL_MUX_LCD_DATA06_EPDC_SDDO06                           0x0130, 0x03BC, 0x0000, 0x9, 0x0
#define IMX6UL_MUX_LCD_DATA07_EPDC_SDDO07                           0x0134, 0x03C0, 0x0000, 0x9, 0x0
#define IMX6UL_MUX_LCD_DATA14_EPDC_SDSHR                            0x0150, 0x03DC, 0x0000, 0x9, 0x0
#define IMX6UL_MUX_LCD_DATA15_EPDC_GDRL                             0x0154, 0x03E0, 0x0000, 0x9, 0x0
#define IMX6UL_MUX_LCD_DATA16_EPDC_GDCLK                            0x0158, 0x03E4, 0x0000, 0x9, 0x0
#define IMX6UL_MUX_LCD_DATA17_EPDC_GDSP                             0x015C, 0x03E8, 0x0000, 0x9, 0x0
#define IMX6UL_MUX_LCD_DATA21_EPDC_SDCE1                            0x016C, 0x03F8, 0x0000, 0x9, 0x0

#define IMX6UL_MUX_CSI_MCLK_ESAI_TX3_RX2                            0x01D4, 0x0460, 0x0000, 0x9, 0x0
#define IMX6UL_MUX_CSI_PIXCLK_ESAI_TX2_RX3                          0x01D8, 0x0464, 0x0000, 0x9, 0x0
#define IMX6UL_MUX_CSI_VSYNC_ESAI_TX4_RX1                           0x01DC, 0x0468, 0x0000, 0x9, 0x0
#define IMX6UL_MUX_CSI_HSYNC_ESAI_TX1                               0x01E0, 0x046C, 0x0000, 0x9, 0x0
#define IMX6UL_MUX_CSI_DATA00_ESAI_TX_HF_CLK                        0x01E4, 0x0470, 0x0000, 0x9, 0x0
#define IMX6UL_MUX_CSI_DATA01_ESAI_RX_HF_CLK                        0x01E8, 0x0474, 0x0000, 0x9, 0x0
#define IMX6UL_MUX_CSI_DATA02_ESAI_RX_FS                            0x01EC, 0x0478, 0x0000, 0x9, 0x0
#define IMX6UL_MUX_CSI_DATA03_ESAI_RX_CLK                           0x01F0, 0x047C, 0x0000, 0x9, 0x0
#define IMX6UL_MUX_CSI_DATA04_ESAI_TX_FS                            0x01F4, 0x0480, 0x0000, 0x9, 0x0
#define IMX6UL_MUX_CSI_DATA05_ESAI_TX_CLK                           0x01F8, 0x0484, 0x0000, 0x9, 0x0
#define IMX6UL_MUX_CSI_DATA06_ESAI_TX5_RX0                          0x01FC, 0x0488, 0x0000, 0x9, 0x0
#define IMX6UL_MUX_CSI_DATA07_ESAI_T0                               0x0200, 0x048C, 0x0000, 0x9, 0x0

#endif /* __IMX6ULL_PINFUNC_H */
