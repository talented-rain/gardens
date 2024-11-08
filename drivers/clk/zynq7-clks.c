/*
 * Clock Driver
 *
 * File Name:   zynq7-clks.c
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

/*!< The defines */
typedef struct zynq7_clks_data
{
    struct fwk_device_node *sprt_clkc;
    struct fwk_device_node *sprt_sclr;

    void *sclr_base;
    void *clkc_base;

} srt_zynq7_clks_data_t;

typedef struct zynq7_clk_grp
{
    kuint32_t number;
    const kchar_t *name;

} srt_zynq7_clk_grp_t;

typedef struct zynq7_sclr
{
    kuint32_t ARMPLL_CTRL;          /*!< 0x00 */
    kuint32_t DDRPLL_CTRL;          /*!< 0x04 */
    kuint32_t IOPLL_CTRL;           /*!< 0x08 */
    kuint32_t PLL_STATUS;           /*!< 0x0c */
    kuint32_t RSVD1[4];             /*!< 0x10 ~ 0x1f*/
    kuint32_t ARM_CLK_CTRL;         /*!< 0x20 */
    kuint32_t DDR_CLK_CTRL;         /*!< 0x24 */
    kuint32_t DCI_CLK_CTRL;         /*!< 0x28 */
    kuint32_t APER_CLK_CTRL;        /*!< 0x2c */
    kuint32_t RSVD2[4];             /*!< 0x30 ~ 0x3f */
    kuint32_t GEM0_CLK_CTRL;        /*!< 0x40 */
    kuint32_t GEM1_CLK_CTRL;        /*!< 0x44 */
    kuint32_t SMC_CLK_CTRL;         /*!< 0x48 */
    kuint32_t LQSPI_CLK_CTRL;       /*!< 0x4c */
    kuint32_t SDIO_CLK_CTRL;        /*!< 0x50 */
    kuint32_t UART_CLK_CTRL;        /*!< 0x54 */
    kuint32_t SPI_CLK_CTRL;         /*!< 0x58 */
    kuint32_t CAN_CLK_CTRL;         /*!< 0x5c */
    kuint32_t CAN_MIOCLK_CTRL;      /*!< 0x60 */
    kuint32_t DBG_CLK_CTRL;         /*!< 0x64 */
    kuint32_t PCAP_CLK_CTRL;        /*!< 0x68 */
    kuint32_t RSVD3;                /*!< 0x6c */
    kuint32_t FPGA0_CLK_CTRL;       /*!< 0x70 */
    kuint32_t RSVD4[20];            /*!< 0x74 ~ 0xc3 */
    kuint32_t SLCR_621_TRUE;        /*!< 0xc4 */
    kuint32_t RSVD5[79];            /*!< 0xc8 ~ 0x203 */
    kuint32_t SWDT_CLK_SEL;         /*!< 0x204 */

} srt_zynq7_sclr_t;

/*!< The globals */
static struct zynq7_clks_data sgrt_zynq7_clks_data;

/*!< device id for device-tree */
static struct fwk_of_device_id sgrt_zynq7_clks_driver_ids[] =
{
	{ .compatible = "xlnx,ps7-clkc", },
	{},
};

#define ZYNQ7_CLKS(num, sym)        [num] = { .number = num, .name = sym }
static const struct zynq7_clk_grp sgrt_zynq7_clks_gate_name[] =
{
    ZYNQ7_CLKS(0,  "armpll"),
    ZYNQ7_CLKS(1,  "ddrpll"),
    ZYNQ7_CLKS(2,  "iopll"),
    ZYNQ7_CLKS(3,  "cpu_6or4x"),
    ZYNQ7_CLKS(4,  "cpu_3or2x"),
    ZYNQ7_CLKS(5,  "cpu_2x"),
    ZYNQ7_CLKS(6,  "cpu_1x"),
    ZYNQ7_CLKS(7,  "ddr2x"),
    ZYNQ7_CLKS(8,  "ddr3x"),
    ZYNQ7_CLKS(9,  "dci"),
    ZYNQ7_CLKS(10, "lqspi"),
    ZYNQ7_CLKS(11, "smc"),
    ZYNQ7_CLKS(12, "pcap"),
    ZYNQ7_CLKS(13, "gem0"),
    ZYNQ7_CLKS(14, "gem1"),
    ZYNQ7_CLKS(15, "fclk0"),
    ZYNQ7_CLKS(16, "fclk1"),
    ZYNQ7_CLKS(17, "fclk2"),
    ZYNQ7_CLKS(18, "fclk3"),
    ZYNQ7_CLKS(19, "can0"),
    ZYNQ7_CLKS(20, "can1"),
    ZYNQ7_CLKS(21, "sdio0"),
    ZYNQ7_CLKS(22, "sdio1"),
    ZYNQ7_CLKS(23, "spi1"),
    ZYNQ7_CLKS(24, "uart0"),
    ZYNQ7_CLKS(25, "uart1"),
    ZYNQ7_CLKS(26, "spi0"),
    ZYNQ7_CLKS(27, "dma"),
    ZYNQ7_CLKS(28, "usb0_aper"),
    ZYNQ7_CLKS(29, "usb1_aper"),
    ZYNQ7_CLKS(30, "gem0_aper"),
    ZYNQ7_CLKS(31, "gem1_aper"),
    ZYNQ7_CLKS(32, "sdio0_aper"),
    ZYNQ7_CLKS(33, "sdio1_aper"),
    ZYNQ7_CLKS(34, "spi0_aper"),
    ZYNQ7_CLKS(35, "spi1_aper"),
    ZYNQ7_CLKS(36, "can0_aper"),
    ZYNQ7_CLKS(37, "can1_aper"),
    ZYNQ7_CLKS(38, "i2c0_aper"),
    ZYNQ7_CLKS(39, "i2c1_aper"),
    ZYNQ7_CLKS(40, "uart0_aper"),
    ZYNQ7_CLKS(41, "uart1_aper"),
    ZYNQ7_CLKS(42, "gpio_aper"),
    ZYNQ7_CLKS(43, "lqspi_aper"),
    ZYNQ7_CLKS(44, "smc_aper"),
    ZYNQ7_CLKS(45, "swdt"),
    ZYNQ7_CLKS(46, "dbg_trc"),
    ZYNQ7_CLKS(47, "dbg_apb"),
};
#undef ZYNQ7_CLKS

/*!< API function */
/*!
 * @brief   enable clk
 * @param   sprt_hw
 * @retval  errono
 * @note    none
 */
static kint32_t zynq7_clks_gate_enable(struct fwk_clk_hw *sprt_hw)
{
    struct fwk_clk_gate *sprt_gate;

    sprt_gate = mrt_container_of(sprt_hw, struct fwk_clk_gate, sgrt_hw);

    return ER_NORMAL;
}

/*!
 * @brief   disable clk
 * @param   sprt_hw
 * @retval  none
 * @note    none
 */
static void	zynq7_clks_gate_disable(struct fwk_clk_hw *sprt_hw)
{
    struct fwk_clk_gate *sprt_gate;

    sprt_gate = mrt_container_of(sprt_hw, struct fwk_clk_gate, sgrt_hw);

}

/*!
 * @brief   get clk status
 * @param   sprt_hw
 * @retval  errono
 * @note    none
 */
static kint32_t zynq7_clks_gate_is_enabled(struct fwk_clk_hw *sprt_hw)
{
    struct fwk_clk_gate *sprt_gate;
    kuint32_t value = 0;

    sprt_gate = mrt_container_of(sprt_hw, struct fwk_clk_gate, sgrt_hw);


    return !!value;
}

static const struct fwk_clk_ops sgrt_zynq7_clks_gate_oprts =
{
    .enable = zynq7_clks_gate_enable,
    .disable = zynq7_clks_gate_disable,
    .is_enabled = zynq7_clks_gate_is_enabled,
};

/*!
 * @brief   create clk global data from device_tree
 * @param   sprt_data
 * @retval  errono
 * @note    none
 */
static kint32_t zynq7_clks_driver_of_init(struct zynq7_clks_data *sprt_data)
{
    struct zynq7_sclr *sprt_zsclr;

    sprt_zsclr = (struct zynq7_sclr *)sprt_data->sclr_base;

    return 0;
}

/*!
 * @brief   init driver
 * @param   none
 * @retval  errno
 * @note    none
 */
kint32_t __fwk_init zynq7_clks_driver_init(void)
{
    struct zynq7_clks_data *sprt_data;
    struct fwk_device_node *sprt_clkc, *sprt_sclr;
    struct zynq7_clk_grp *sprt_grps;
    kuint32_t grps_size;
    kuaddr_t clkc_reg, sclr_reg;
    kuint32_t index;

    sprt_data = &sgrt_zynq7_clks_data;
    sprt_grps = (struct zynq7_clk_grp *)sgrt_zynq7_clks_gate_name;
    grps_size = ARRAY_SIZE(sgrt_zynq7_clks_gate_name);

    /*!< check */
    for (index = 0; index < grps_size; index++)
    {
        if (index != sprt_grps[index].number)
        {
            print_err("array sgrt_zynq7_clks_gate_name[%d] is unvalid!\n", index);
            return -ER_UNVALID;
        }
    }

    sprt_clkc = fwk_of_find_matching_node_and_match(mrt_nullptr, sgrt_zynq7_clks_driver_ids, mrt_nullptr);
    if (!isValid(sprt_clkc))
        return -ER_NOTFOUND;

    sprt_sclr = fwk_of_get_parent(sprt_clkc);
    sclr_reg  = (kuaddr_t)fwk_of_iomap(sprt_sclr, 0);
    clkc_reg  = sclr_reg + (kuaddr_t)fwk_of_iomap(sprt_clkc, 0);

    sprt_data->sprt_clkc = sprt_clkc;
    sprt_data->sprt_sclr = sprt_sclr;
    sprt_data->clkc_base = fwk_io_remap((void *)clkc_reg);
    sprt_data->sclr_base = fwk_io_remap((void *)sclr_reg);

    return zynq7_clks_driver_of_init(sprt_data);
}

/*!
 * @brief   exit driver
 * @param   none
 * @retval  none
 * @note    none
 */
void __fwk_exit zynq7_clks_driver_exit(void)
{

}

IMPORT_LATE_INIT(zynq7_clks_driver_init);
IMPORT_LATE_EXIT(zynq7_clks_driver_exit);

/*!< end of file */
