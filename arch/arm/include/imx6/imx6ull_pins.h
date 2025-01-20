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

#ifndef __IMX6UL_PINS_H
#define __IMX6UL_PINS_H

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

/*!< The defines */
/*!< mux Pin Property */
typedef struct
{
    kuaddr_t mux_base;
    kuaddr_t pad_base;
    kuaddr_t input_base;
    kuaddr_t opt_base;

    kuint32_t mux_data;
    kuint32_t pad_data;
    kuint32_t input_data;

} srt_hal_imx_pin_t;

/*!< IO Pin Base Address */
#define IMX6UL_PIN_ADDR_BASE                        (0x20E0000u)

/*!
 * SW_PAD
 * For Gpio/Uart/LCD/Ether/CSI/USDHC/Nand/Jtag:
 *  bit[31 : 17]: reserved
 *  bit[16]:      HYS. Hysteresis. It is a delay filter for input mode usually. 0: disable; 1: enable  
 *  bit[15 : 14]: PUS. pull up/down.
 *                00, 100k pull down;
 *                01, 47k pull up;
 *                10, 100k pull up
 *                11, 22k pull up
 *  bit[13]:      PUE. pull/keep select. 0: keeper; 1: pull
 *  bit[12]:      PKE. pull/keep enable. 0: disable; 1: enable
 *  bit[11]:      ODE. Open-drain ouput enable. 0: disable; 1: enable
 *  bit[10 : 8]:  reserved
 *  bit[7 : 6]:   SPEED. speed configure.
 *                00: low(50MHz);
 *                01: medium(100MHz);
 *                10: medium(100MHz);
 *                11: max(200MHz)
 *  bit[5 : 3]:   DSE. drive strenth.
 *                000: output driver diable;
 *                001: R0 (260 Ohm @3.3V, 150 Ohm@1.8V, 240 Ohm for DDR)
 *                010 ~ 111: R0 / x, x = 0x010, 0x011, ..., 0x111
 *  bit[2 : 1]:   reserved
 *  bit[0]:       SRE. slew rate. 0: slow slew rate; 1: fast slew rate
 */
/*!< struct, littile endian, the low byte is at low address */
typedef struct imx_io_ctl_pad
{
    kuint32_t SRE : 1;
    kuint32_t RESVD1 : 2;
    kuint32_t DSE : 3;
    kuint32_t SPEED : 2;
    kuint32_t RESVD2 : 2;
    kuint32_t ODE : 1;
    kuint32_t PKE : 1;
    kuint32_t PUE : 1;
    kuint32_t PUS : 2;
    kuint32_t HYS : 1;
    kuint32_t RESVD3 : 16;

} srt_imx_io_ctl_pad_t;

/*!< union for io ctl pad */
UNION_SRT_FORMAT_DECLARE(urt_imx_io_ctl_pad, srt_imx_io_ctl_pad_t)

/*!< bit shift macro definition */
#define IMX6UL_IO_CTL_PAD_HYS_ENABLE                (1U)
#define IMX6UL_IO_CTL_PAD_HYS_DISABLE               (0U)
#define IMX6UL_IO_CTL_PAD_HYS_OFFSET                (16U)
#define IMX6UL_IO_CTL_PAD_HYS_MASK                  (1U)
#define IMX6UL_IO_CTL_PAD_HYS_BIT(x)                mrt_bit_mask_nr(x, IMX6UL_IO_CTL_PAD_HYS_MASK, IMX6UL_IO_CTL_PAD_HYS_OFFSET)

#define IMX6UL_IO_CTL_PAD_PUS_100K_DOWN             (0U)
#define IMX6UL_IO_CTL_PAD_PUS_47K_UP                (1U)
#define IMX6UL_IO_CTL_PAD_PUS_100K_UP               (2U)
#define IMX6UL_IO_CTL_PAD_PUS_22K_UP                (3U)
#define IMX6UL_IO_CTL_PAD_PUS_OFFSET                (14U)
#define IMX6UL_IO_CTL_PAD_PUS_MASK                  (3U)
#define IMX6UL_IO_CTL_PAD_PUS_BIT(x)                mrt_bit_mask_nr(x, IMX6UL_IO_CTL_PAD_PUS_MASK, IMX6UL_IO_CTL_PAD_PUS_OFFSET)

#define IMX6UL_IO_CTL_PAD_PUE_PULL                  (1U)
#define IMX6UL_IO_CTL_PAD_PUE_KEEPER                (0U)
#define IMX6UL_IO_CTL_PAD_PUE_OFFSET                (13U)
#define IMX6UL_IO_CTL_PAD_PUE_MASK                  (1U)
#define IMX6UL_IO_CTL_PAD_PUE_BIT(x)                mrt_bit_mask_nr(x, IMX6UL_IO_CTL_PAD_PUE_MASK, IMX6UL_IO_CTL_PAD_PUE_OFFSET)

#define IMX6UL_IO_CTL_PAD_PKE_ENABLE                (1U)
#define IMX6UL_IO_CTL_PAD_PKE_DISABLE               (0U)
#define IMX6UL_IO_CTL_PAD_PKE_OFFSET                (12U)
#define IMX6UL_IO_CTL_PAD_PKE_MASK                  (1U)
#define IMX6UL_IO_CTL_PAD_PKE_BIT(x)                mrt_bit_mask_nr(x, IMX6UL_IO_CTL_PAD_PKE_MASK, IMX6UL_IO_CTL_PAD_PKE_OFFSET)

#define IMX6UL_IO_CTL_PAD_ODE_ENABLE                (1U)
#define IMX6UL_IO_CTL_PAD_ODE_DISABLE               (0U)
#define IMX6UL_IO_CTL_PAD_ODE_OFFSET                (11U)
#define IMX6UL_IO_CTL_PAD_ODE_MASK                  (1U)
#define IMX6UL_IO_CTL_PAD_ODE_BIT(x)                mrt_bit_mask_nr(x, IMX6UL_IO_CTL_PAD_ODE_MASK, IMX6UL_IO_CTL_PAD_ODE_OFFSET)

#define IMX6UL_IO_CTL_PAD_SPEED_50MHZ               (0U)
#define IMX6UL_IO_CTL_PAD_SPEED_100MHZ              (1U)
#define IMX6UL_IO_CTL_PAD_SPEED_200MHZ              (3U)
#define IMX6UL_IO_CTL_PAD_SPEED_OFFSET              (6U)
#define IMX6UL_IO_CTL_PAD_SPEED_MASK                (3U)
#define IMX6UL_IO_CTL_PAD_SPEED_BIT(x)              mrt_bit_mask_nr(x, IMX6UL_IO_CTL_PAD_SPEED_MASK, IMX6UL_IO_CTL_PAD_SPEED_OFFSET)

#define IMX6UL_IO_CTL_PAD_DSE_DISABLE               (0U)
#define IMX6UL_IO_CTL_PAD_DSE_RDIV(x)               (x)
#define IMX6UL_IO_CTL_PAD_DSE_OFFSET                (3U)
#define IMX6UL_IO_CTL_PAD_DSE_MASK                  (7U)
#define IMX6UL_IO_CTL_PAD_DSE_BIT(x)                mrt_bit_mask_nr(x, IMX6UL_IO_CTL_PAD_DSE_MASK, IMX6UL_IO_CTL_PAD_DSE_OFFSET)

#define IMX6UL_IO_CTL_PAD_FAST_RATE                 (1U)
#define IMX6UL_IO_CTL_PAD_SLOW_RATE                 (0U)
#define IMX6UL_IO_CTL_PAD_SRE_OFFSET                (0U)
#define IMX6UL_IO_CTL_PAD_SRE_MASK                  (1U)
#define IMX6UL_IO_CTL_PAD_SRE_BIT(x)                mrt_bit_mask_nr(x, IMX6UL_IO_CTL_PAD_SRE_MASK, IMX6UL_IO_CTL_PAD_SRE_OFFSET)

/*!< Mux functionality */
#define IMX6UL_PIN_MUX_FUNC_ENABLE                  (1U)
#define IMX6UL_PIN_MUX_FUNC_DISABLE                 (0U)

/*!
 * @brief   hal_imx_pin_attribute_init
 * @param   none
 * @retval  none
 * @note    initial pin configuration
 */
static inline void hal_imx_pin_attribute_init(srt_hal_imx_pin_t *sprt_cfg, kuaddr_t base,
                                                kuint32_t mux_offset, 
                                                kuint32_t pad_offset,
                                                kuint32_t input_offset,
                                                kuint32_t mux_data,
                                                kuint32_t input_data,
                                                kuint32_t pad_data,
                                                kuint8_t  functionality)
{
    /*!< clear all */
    /* kmemzero(sprt_cfg, sizeof(srt_hal_imx_pin_t)); */

    /*!<
     * SW_MUX
     *  bit[31 : 5]: reserved
     *  bit[4]:      SION, Force the selected mux mode Input path no matter of MUX_MODE functionality.
     *               1, ENABLE, Force ...
     *               0, DISABLE, Determined by functionly. Suggest 0.
     *  bit[3 : 0]:  Function; ALT5 (0101 <0x5>) is generally used to configure GPIO. Refer to "imx6ul_pinfunc.h"
     */
    sprt_cfg->mux_base   = base + mux_offset;
    sprt_cfg->mux_data   = mrt_mask(mux_data, 0xfU) | mrt_bit_nr(functionality, 4U);

    /*!<
     * SW_PAD
     * sometimes it will be set "0x10b0" ===>
     *                Hyst = 0, 100k pull down, keeper = 1, OD = 0, 100MHz, R0/6, slow slew rate
     */
    sprt_cfg->pad_base   = base + pad_offset;
    sprt_cfg->pad_data   = pad_data;

    /*!<
     * INPUT
     */
    sprt_cfg->input_base = input_offset ? (base + input_offset) : 0;
    sprt_cfg->input_data = input_data;
}

/*!
 * @brief   hal_imx_pin_auto_init
 * @param   none
 * @retval  none
 * @note    initial pin configuration
 */
static inline void hal_imx_pin_auto_init(srt_hal_imx_pin_t *sprt_cfg, kuaddr_t base, 
                                        kuint32_t *ptr_value, kuint8_t functionality, kusize_t size)
{
    if (size != IMX6UL_MUX_CONF_SIZE)
        return;
    
    hal_imx_pin_attribute_init(sprt_cfg, base, ptr_value[0], ptr_value[1], 
                            ptr_value[2], ptr_value[3], ptr_value[4], ptr_value[5], functionality);
}

/*!
 * @brief   hal_imx_pin_mux_configure
 * @param   none
 * @retval  none
 * @note    configure pin multiplex
 */
static inline void hal_imx_pin_mux_configure(srt_hal_imx_pin_t *sprt_cfg)
{
    if (sprt_cfg->mux_base)
        mrt_writel(sprt_cfg->mux_data, sprt_cfg->mux_base);
}

/*!
 * @brief   hal_imx_pin_pad_configure
 * @param   none
 * @retval  none
 * @note    configure pin property
 */
static inline void hal_imx_pin_pad_configure(srt_hal_imx_pin_t *sprt_cfg)
{
    if (sprt_cfg->pad_base)
        mrt_writel(sprt_cfg->pad_data, sprt_cfg->pad_base);
}

/*!
 * @brief   configure all
 * @param   none
 * @retval  none
 * @note    configure pin property
 */
static inline void hal_imx_pin_configure(srt_hal_imx_pin_t *sprt_cfg)
{
    mrt_writel(sprt_cfg->mux_data, sprt_cfg->mux_base);
    mrt_writel(sprt_cfg->pad_data, sprt_cfg->pad_base);

    if (sprt_cfg->input_base)
        mrt_writel(sprt_cfg->input_data, sprt_cfg->input_base);
}

/*!
 * @brief   hal_imx_set_pin_mux
 * @param   none
 * @retval  none
 * @note    configure pin multiplex
 */
static inline void hal_imx_set_pin_mux(srt_hal_imx_pin_t *sprt_cfg, kuint32_t data)
{
    sprt_cfg->mux_data = data;
    hal_imx_pin_mux_configure(sprt_cfg);
}

/*!
 * @brief   hal_imx_set_pin_pad
 * @param   none
 * @retval  none
 * @note    configure pin property
 */
static inline void hal_imx_set_pin_pad(srt_hal_imx_pin_t *sprt_cfg, kuint32_t data)
{
    sprt_cfg->pad_data = data;
    hal_imx_pin_pad_configure(sprt_cfg);
}

#ifdef __cplusplus
    }
#endif

#endif /* __IMX6UL_PINS_H */
