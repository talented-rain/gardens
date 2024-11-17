/*
 * IOMUX Device Driver
 *
 * File Name:   imx-snvs.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.04.20
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/fwk_cdev.h>
#include <platform/fwk_chrdev.h>
#include <platform/fwk_inode.h>
#include <platform/fwk_fs.h>
#include <platform/of/fwk_of.h>
#include <platform/of/fwk_of_device.h>
#include <platform/fwk_platdrv.h>
#include <platform/fwk_uaccess.h>
#include <platform/fwk_pinctrl.h>

#include <imx6/imx6ull_pins.h>

/*!< The defines */
#define IMX6UL_MUX_PIN_PER_LENTH                            (IMX6UL_MUX_CONF_SIZE)
#define IMX6UL_MUX_PIN_CONFIG_SION                          (0x40000000)

/*!< defines by driver */
typedef struct imx_iomuxc_pads
{
    const struct fwk_pinctrl_pin_desc *sprt_pins;
    const kusize_t npins;

} srt_imx_iomuxc_pads_t;

typedef struct imx_iomuxc_pin
{
    kuint32_t pin;

    /*!< see device_node of dts: such as "fsl,pins" */
    srt_hal_imx_pin_t sgrt_pin;
    
} srt_imx_iomuxc_pin_t;

/*!< see device_node of dts: pinctrl_xxx */
typedef struct imx_iomuxc_pin_grp
{
    kchar_t *name;

    struct imx_iomuxc_pin *sprt_pin;
    kuint32_t npins;

} srt_imx_iomuxc_pin_grp_t;

/*!< see device_node of dts: such as "imx6ul-evk" */
typedef struct imx_iomuxc_pin_func
{
    kchar_t *name;   

    struct imx_iomuxc_pin_grp *sprt_pingrps;
    kuint32_t ngrps;

} srt_imx_iomuxc_pin_func_t;

typedef struct imx_iomuxc_pin_res
{
    kuaddr_t mux_base;
    kuaddr_t conf_base;

} srt_imx_iomuxc_pin_res_t;

/*!< -------------------------------------------------------------------
 *!<                  private driver data interface
 *!< ---------------------------------------------------------------- */
typedef struct imx_iomuxc_data
{
    void *base;
    const srt_imx_iomuxc_pads_t *sprt_pad;
    struct fwk_pinctrl_dev *sprt_pctldev;

    srt_imx_iomuxc_pin_func_t *sprt_pinfuncs;
    kuint32_t nfuncs;

    srt_imx_iomuxc_pin_res_t *sprt_pins;
    kuint32_t npins;

} srt_imx_iomuxc_data_t;

/*!< The globals */
#define IOMUXC_NAME(x)     "IMX6ULL_PAD_"#x
static const struct fwk_pinctrl_pin_desc sgrt_imx_iomuxc_pin_descs[] =
{
	FWK_PINCTRL_PIN(0, IOMUXC_NAME(RESERVE0)),
	FWK_PINCTRL_PIN(1, IOMUXC_NAME(RESERVE1)),
	FWK_PINCTRL_PIN(2, IOMUXC_NAME(RESERVE2)),
	FWK_PINCTRL_PIN(3, IOMUXC_NAME(RESERVE3)),
	FWK_PINCTRL_PIN(4, IOMUXC_NAME(RESERVE4)),
	FWK_PINCTRL_PIN(5, IOMUXC_NAME(RESERVE5)),
	FWK_PINCTRL_PIN(6, IOMUXC_NAME(RESERVE6)),
	FWK_PINCTRL_PIN(7, IOMUXC_NAME(RESERVE7)),
	FWK_PINCTRL_PIN(8, IOMUXC_NAME(RESERVE8)),
	FWK_PINCTRL_PIN(9, IOMUXC_NAME(RESERVE9)),
	FWK_PINCTRL_PIN(10, IOMUXC_NAME(RESERVE10)),
	FWK_PINCTRL_PIN(11, IOMUXC_NAME(SNVS_TAMPER4)),
	FWK_PINCTRL_PIN(12, IOMUXC_NAME(RESERVE12)),
	FWK_PINCTRL_PIN(13, IOMUXC_NAME(RESERVE13)),
	FWK_PINCTRL_PIN(14, IOMUXC_NAME(RESERVE14)),
	FWK_PINCTRL_PIN(15, IOMUXC_NAME(RESERVE15)),
	FWK_PINCTRL_PIN(16, IOMUXC_NAME(RESERVE16)),
	FWK_PINCTRL_PIN(17, IOMUXC_NAME(JTAG_MOD)),
	FWK_PINCTRL_PIN(18, IOMUXC_NAME(JTAG_TMS)),
	FWK_PINCTRL_PIN(19, IOMUXC_NAME(JTAG_TDO)),
	FWK_PINCTRL_PIN(20, IOMUXC_NAME(JTAG_TDI)),
	FWK_PINCTRL_PIN(21, IOMUXC_NAME(JTAG_TCK)),
	FWK_PINCTRL_PIN(22, IOMUXC_NAME(JTAG_TRST_B)),
	FWK_PINCTRL_PIN(23, IOMUXC_NAME(GPIO1_IO00)),
	FWK_PINCTRL_PIN(24, IOMUXC_NAME(GPIO1_IO01)),
	FWK_PINCTRL_PIN(25, IOMUXC_NAME(GPIO1_IO02)),
	FWK_PINCTRL_PIN(26, IOMUXC_NAME(GPIO1_IO03)),
	FWK_PINCTRL_PIN(27, IOMUXC_NAME(GPIO1_IO04)),
	FWK_PINCTRL_PIN(28, IOMUXC_NAME(GPIO1_IO05)),
	FWK_PINCTRL_PIN(29, IOMUXC_NAME(GPIO1_IO06)),
	FWK_PINCTRL_PIN(30, IOMUXC_NAME(GPIO1_IO07)),
	FWK_PINCTRL_PIN(31, IOMUXC_NAME(GPIO1_IO08)),
	FWK_PINCTRL_PIN(32, IOMUXC_NAME(GPIO1_IO09)),
	FWK_PINCTRL_PIN(33, IOMUXC_NAME(UART1_TX_DATA)),
	FWK_PINCTRL_PIN(34, IOMUXC_NAME(UART1_RX_DATA)),
	FWK_PINCTRL_PIN(35, IOMUXC_NAME(UART1_CTS_B)),
	FWK_PINCTRL_PIN(36, IOMUXC_NAME(UART1_RTS_B)),
	FWK_PINCTRL_PIN(37, IOMUXC_NAME(UART2_TX_DATA)),
	FWK_PINCTRL_PIN(38, IOMUXC_NAME(UART2_RX_DATA)),
	FWK_PINCTRL_PIN(39, IOMUXC_NAME(UART2_CTS_B)),
	FWK_PINCTRL_PIN(40, IOMUXC_NAME(UART2_RTS_B)),
	FWK_PINCTRL_PIN(41, IOMUXC_NAME(UART3_TX_DATA)),
	FWK_PINCTRL_PIN(42, IOMUXC_NAME(UART3_RX_DATA)),
	FWK_PINCTRL_PIN(43, IOMUXC_NAME(UART3_CTS_B)),
	FWK_PINCTRL_PIN(44, IOMUXC_NAME(UART3_RTS_B)),
	FWK_PINCTRL_PIN(45, IOMUXC_NAME(UART4_TX_DATA)),
	FWK_PINCTRL_PIN(46, IOMUXC_NAME(UART4_RX_DATA)),
	FWK_PINCTRL_PIN(47, IOMUXC_NAME(UART5_TX_DATA)),
	FWK_PINCTRL_PIN(48, IOMUXC_NAME(UART5_RX_DATA)),
	FWK_PINCTRL_PIN(49, IOMUXC_NAME(ENET1_RX_DATA0)),
	FWK_PINCTRL_PIN(50, IOMUXC_NAME(ENET1_RX_DATA1)),
	FWK_PINCTRL_PIN(51, IOMUXC_NAME(ENET1_RX_EN)),
	FWK_PINCTRL_PIN(52, IOMUXC_NAME(ENET1_TX_DATA0)),
	FWK_PINCTRL_PIN(53, IOMUXC_NAME(ENET1_TX_DATA1)),
	FWK_PINCTRL_PIN(54, IOMUXC_NAME(ENET1_TX_EN)),
	FWK_PINCTRL_PIN(55, IOMUXC_NAME(ENET1_TX_CLK)),
	FWK_PINCTRL_PIN(56, IOMUXC_NAME(ENET1_RX_ER)),
	FWK_PINCTRL_PIN(57, IOMUXC_NAME(ENET2_RX_DATA0)),
	FWK_PINCTRL_PIN(58, IOMUXC_NAME(ENET2_RX_DATA1)),
	FWK_PINCTRL_PIN(59, IOMUXC_NAME(ENET2_RX_EN)),
	FWK_PINCTRL_PIN(60, IOMUXC_NAME(ENET2_TX_DATA0)),
	FWK_PINCTRL_PIN(61, IOMUXC_NAME(ENET2_TX_DATA1)),
	FWK_PINCTRL_PIN(62, IOMUXC_NAME(ENET2_TX_EN)),
	FWK_PINCTRL_PIN(63, IOMUXC_NAME(ENET2_TX_CLK)),
	FWK_PINCTRL_PIN(64, IOMUXC_NAME(ENET2_RX_ER)),
	FWK_PINCTRL_PIN(65, IOMUXC_NAME(LCD_CLK)),
	FWK_PINCTRL_PIN(66, IOMUXC_NAME(LCD_ENABLE)),
	FWK_PINCTRL_PIN(67, IOMUXC_NAME(LCD_HSYNC)),
	FWK_PINCTRL_PIN(68, IOMUXC_NAME(LCD_VSYNC)),
	FWK_PINCTRL_PIN(69, IOMUXC_NAME(LCD_RESET)),
	FWK_PINCTRL_PIN(70, IOMUXC_NAME(LCD_DATA00)),
	FWK_PINCTRL_PIN(71, IOMUXC_NAME(LCD_DATA01)),
	FWK_PINCTRL_PIN(72, IOMUXC_NAME(LCD_DATA02)),
	FWK_PINCTRL_PIN(73, IOMUXC_NAME(LCD_DATA03)),
	FWK_PINCTRL_PIN(74, IOMUXC_NAME(LCD_DATA04)),
	FWK_PINCTRL_PIN(75, IOMUXC_NAME(LCD_DATA05)),
	FWK_PINCTRL_PIN(76, IOMUXC_NAME(LCD_DATA06)),
	FWK_PINCTRL_PIN(77, IOMUXC_NAME(LCD_DATA07)),
	FWK_PINCTRL_PIN(78, IOMUXC_NAME(LCD_DATA08)),
	FWK_PINCTRL_PIN(79, IOMUXC_NAME(LCD_DATA09)),
	FWK_PINCTRL_PIN(80, IOMUXC_NAME(LCD_DATA10)),
	FWK_PINCTRL_PIN(81, IOMUXC_NAME(LCD_DATA11)),
	FWK_PINCTRL_PIN(82, IOMUXC_NAME(LCD_DATA12)),
	FWK_PINCTRL_PIN(83, IOMUXC_NAME(LCD_DATA13)),
	FWK_PINCTRL_PIN(84, IOMUXC_NAME(LCD_DATA14)),
	FWK_PINCTRL_PIN(85, IOMUXC_NAME(LCD_DATA15)),
	FWK_PINCTRL_PIN(86, IOMUXC_NAME(LCD_DATA16)),
	FWK_PINCTRL_PIN(87, IOMUXC_NAME(LCD_DATA17)),
	FWK_PINCTRL_PIN(88, IOMUXC_NAME(LCD_DATA18)),
	FWK_PINCTRL_PIN(89, IOMUXC_NAME(LCD_DATA19)),
	FWK_PINCTRL_PIN(90, IOMUXC_NAME(LCD_DATA20)),
	FWK_PINCTRL_PIN(91, IOMUXC_NAME(LCD_DATA21)),
	FWK_PINCTRL_PIN(92, IOMUXC_NAME(LCD_DATA22)),
	FWK_PINCTRL_PIN(93, IOMUXC_NAME(LCD_DATA23)),
	FWK_PINCTRL_PIN(94, IOMUXC_NAME(NAND_RE_B)),
	FWK_PINCTRL_PIN(95, IOMUXC_NAME(NAND_WE_B)),
	FWK_PINCTRL_PIN(96, IOMUXC_NAME(NAND_DATA00)),
	FWK_PINCTRL_PIN(97, IOMUXC_NAME(NAND_DATA01)),
	FWK_PINCTRL_PIN(98, IOMUXC_NAME(NAND_DATA02)),
	FWK_PINCTRL_PIN(99, IOMUXC_NAME(NAND_DATA03)),
	FWK_PINCTRL_PIN(100, IOMUXC_NAME(NAND_DATA04)),
	FWK_PINCTRL_PIN(101, IOMUXC_NAME(NAND_DATA05)),
	FWK_PINCTRL_PIN(102, IOMUXC_NAME(NAND_DATA06)),
	FWK_PINCTRL_PIN(103, IOMUXC_NAME(NAND_DATA07)),
	FWK_PINCTRL_PIN(104, IOMUXC_NAME(NAND_ALE)),
	FWK_PINCTRL_PIN(105, IOMUXC_NAME(NAND_WP_B)),
	FWK_PINCTRL_PIN(106, IOMUXC_NAME(NAND_READY_B)),
	FWK_PINCTRL_PIN(107, IOMUXC_NAME(NAND_CE0_B)),
	FWK_PINCTRL_PIN(108, IOMUXC_NAME(NAND_CE1_B)),
	FWK_PINCTRL_PIN(109, IOMUXC_NAME(NAND_CLE)),
	FWK_PINCTRL_PIN(110, IOMUXC_NAME(NAND_DQS)),
	FWK_PINCTRL_PIN(111, IOMUXC_NAME(SD1_CMD)),
	FWK_PINCTRL_PIN(112, IOMUXC_NAME(SD1_CLK)),
	FWK_PINCTRL_PIN(113, IOMUXC_NAME(SD1_DATA0)),
	FWK_PINCTRL_PIN(114, IOMUXC_NAME(SD1_DATA1)),
	FWK_PINCTRL_PIN(115, IOMUXC_NAME(SD1_DATA2)),
	FWK_PINCTRL_PIN(116, IOMUXC_NAME(SD1_DATA3)),
	FWK_PINCTRL_PIN(117, IOMUXC_NAME(CSI_MCLK)),
	FWK_PINCTRL_PIN(118, IOMUXC_NAME(CSI_PIXCLK)),
	FWK_PINCTRL_PIN(119, IOMUXC_NAME(CSI_VSYNC)),
	FWK_PINCTRL_PIN(120, IOMUXC_NAME(CSI_HSYNC)),
	FWK_PINCTRL_PIN(121, IOMUXC_NAME(CSI_DATA00)),
	FWK_PINCTRL_PIN(122, IOMUXC_NAME(CSI_DATA01)),
	FWK_PINCTRL_PIN(123, IOMUXC_NAME(CSI_DATA02)),
	FWK_PINCTRL_PIN(124, IOMUXC_NAME(CSI_DATA03)),
	FWK_PINCTRL_PIN(125, IOMUXC_NAME(CSI_DATA04)),
	FWK_PINCTRL_PIN(126, IOMUXC_NAME(CSI_DATA05)),
	FWK_PINCTRL_PIN(127, IOMUXC_NAME(CSI_DATA06)),
	FWK_PINCTRL_PIN(128, IOMUXC_NAME(CSI_DATA07)),
};

static const struct fwk_pinctrl_pin_desc sgrt_imx_iomuxc_snvs_descs[] =
{
	FWK_PINCTRL_PIN(0, IOMUXC_NAME(BOOT_MODE0)),
	FWK_PINCTRL_PIN(1, IOMUXC_NAME(BOOT_MODE1)),
	FWK_PINCTRL_PIN(2, IOMUXC_NAME(SNVS_TAMPER0)),
	FWK_PINCTRL_PIN(3, IOMUXC_NAME(SNVS_TAMPER1)),
	FWK_PINCTRL_PIN(4, IOMUXC_NAME(SNVS_TAMPER2)),
	FWK_PINCTRL_PIN(5, IOMUXC_NAME(SNVS_TAMPER3)),
	FWK_PINCTRL_PIN(6, IOMUXC_NAME(SNVS_TAMPER4)),
	FWK_PINCTRL_PIN(7, IOMUXC_NAME(SNVS_TAMPER5)),
	FWK_PINCTRL_PIN(8, IOMUXC_NAME(SNVS_TAMPER6)),
	FWK_PINCTRL_PIN(9, IOMUXC_NAME(SNVS_TAMPER7)),
	FWK_PINCTRL_PIN(10, IOMUXC_NAME(SNVS_TAMPER8)),
	FWK_PINCTRL_PIN(11, IOMUXC_NAME(SNVS_TAMPER9)),

    /*!< (.numer + 1) < IMX_IOMUXC_PAD_GROUP_MAX */
};
#undef IOMUXC_NAME

static const srt_imx_iomuxc_pads_t sgrt_imx_iomuxc_pads =
{
    .sprt_pins = sgrt_imx_iomuxc_pin_descs,
    .npins = ARRAY_SIZE(sgrt_imx_iomuxc_pin_descs),
};

static const srt_imx_iomuxc_pads_t sgrt_imx_iomuxc_snvs_pads =
{
    .sprt_pins = sgrt_imx_iomuxc_snvs_descs,
    .npins = ARRAY_SIZE(sgrt_imx_iomuxc_snvs_descs),
};

/*!< device id for device-tree */
static const struct fwk_of_device_id sgrt_imx_iomuxc_driver_ids[] =
{
    { .compatible = "fsl,imx6ul-iomuxc", .data = &sgrt_imx_iomuxc_pads },
	{ .compatible = "fsl,imx6ull-iomuxc-snvs", .data = &sgrt_imx_iomuxc_snvs_pads },
	{},
};

/*!< API function */
/*!
 * @brief   get group handler by group's name (= sprt_node->name)
 * @param   sprt_data: private data
 * @param   name: name (it may be sprt_node->name)
 * @retval  none
 * @note    none
 */
static void *imx_iomuxc_get_group_by_name(srt_imx_iomuxc_data_t *sprt_data, const kchar_t *name)
{
    srt_imx_iomuxc_pin_func_t *sprt_pinfunc;
    srt_imx_iomuxc_pin_grp_t *sprt_pingrp;
    kuint32_t i, j;

    /*!< traval all functions and groups, until it is found */
    for (i = 0; i < sprt_data->nfuncs; i++)
    {
        sprt_pinfunc = &sprt_data->sprt_pinfuncs[i];

        for (j = 0; j < sprt_pinfunc->ngrps; j++)
        {
            sprt_pingrp = &sprt_pinfunc->sprt_pingrps[j];

            if (!strcmp(sprt_pingrp->name, name))
                return sprt_pingrp;
        }
    }

    return mrt_nullptr;
}

/*!
 * @brief   allocate and initialize map
 * @param   sprt_pctldev: pinctrl device
 * @param   sprt_node: group node of dts
 * @param   sprt_map/num_maps: save to them
 * @retval  errno
 * @note    none
 */
static kint32_t imx_iomuxc_dt_node_to_map(struct fwk_pinctrl_dev *sprt_pctldev, struct fwk_device_node *sprt_node, 
												            struct fwk_pinctrl_map **sprt_map, kuint32_t *num_maps)
{
    srt_imx_iomuxc_data_t *sprt_data;
    srt_imx_iomuxc_pin_grp_t *sprt_pingrp;
    srt_imx_iomuxc_pin_t *sprt_pin;
    struct fwk_pinctrl_map *sprt_newmap;
    const struct fwk_pinctrl_pin_desc *sprt_pdesc;
    kuint32_t i;

    if (!sprt_pctldev || !sprt_node)
        return -ER_NODEV;

    sprt_data = (srt_imx_iomuxc_data_t *)fwk_pinctrl_get_drvdata(sprt_pctldev);
    sprt_pingrp = (srt_imx_iomuxc_pin_grp_t *)imx_iomuxc_get_group_by_name(sprt_data, sprt_node->name);
    if (!sprt_pingrp)
        return -ER_NOTFOUND;

    sprt_pdesc = sprt_pctldev->sprt_desc->sprt_pins;

    sprt_newmap = (struct fwk_pinctrl_map *)kzalloc((sprt_pingrp->npins + 1) * sizeof(*sprt_newmap), GFP_KERNEL);
    if (!isValid(sprt_newmap))
        return -ER_NOMEM;

    sprt_newmap[0].type = NR_FWK_PINCTRL_PIN_MUX;
    sprt_newmap[0].ugrt_data.sgrt_mux.function = sprt_node->parent->name;
    sprt_newmap[0].ugrt_data.sgrt_mux.group = sprt_node->name;

    *sprt_map = sprt_newmap;
    *num_maps = sprt_pingrp->npins + 1;

    sprt_newmap++;
    for (i = 0; i < sprt_pingrp->npins; i++)
    {
        sprt_pin = &sprt_pingrp->sprt_pin[i];

        sprt_newmap[i].type = NR_FWK_PINCTRL_PIN_CONF;
        sprt_newmap[i].ugrt_data.sgrt_configs.group_or_pin = sprt_pdesc[sprt_pin->pin].name;
        sprt_newmap[i].ugrt_data.sgrt_configs.configs = &sprt_pin->sgrt_pin.pad_data;
        sprt_newmap[i].ugrt_data.sgrt_configs.num_configs = 1;
    }

    return ER_NORMAL;
}

/*!
 * @brief   release map
 * @param   sprt_pctldev: pinctrl_dev
 * @param   sprt_map: map address
 * @param   num_maps: numbers
 * @retval  none
 * @note    none
 */
static void imx_iomuxc_dt_free_map(struct fwk_pinctrl_dev *sprt_pctldev, struct fwk_pinctrl_map *sprt_map, kuint32_t num_maps)
{
    if (!sprt_map)
        return;

    kfree(sprt_map);
}

/*!
 * @brief   get function information
 * @param   sprt_pctldev: pinctrl_dev
 * @param   selector: function index
 * @param   groups, num_groups: save to them
 * @retval  numbers
 * @note    none
 */
static kint32_t imx_iomuxc_get_function_groups(struct fwk_pinctrl_dev *sprt_pctldev,
				                        kuint32_t selector, kuaddr_t **groups, kuint32_t * const num_groups)
{
    srt_imx_iomuxc_data_t *sprt_data;

    sprt_data = (srt_imx_iomuxc_data_t *)fwk_pinctrl_get_drvdata(sprt_pctldev);
    *groups = (kuaddr_t *)sprt_data->sprt_pinfuncs[selector].sprt_pingrps;
    *num_groups = sprt_data->sprt_pinfuncs[selector].ngrps;

    return ER_NORMAL;
}

/*!
 * @brief   get the number of groups of functions[func_selector]
 * @param   sprt_pctldev: pinctrl_dev
 * @param   func_selector: function index
 * @retval  numbers
 * @note    none
 */
static kint32_t imx_iomuxc_get_groups_count(struct fwk_pinctrl_dev *sprt_pctldev, kuint32_t func_selector)
{
    srt_imx_iomuxc_data_t *sprt_data;

    sprt_data = (srt_imx_iomuxc_data_t *)fwk_pinctrl_get_drvdata(sprt_pctldev);
    return sprt_data->sprt_pinfuncs[func_selector].ngrps;
}

/*!
 * @brief   get group name (struct fwk_pinctrl_pin_desc::name)
 * @param   sprt_pctldev: pinctrl_dev
 * @param   func_selector: function index
 * @param   group_selector: group index
 * @retval  name
 * @note    none
 */
static const kchar_t *imx_iomuxc_get_group_name(struct fwk_pinctrl_dev *sprt_pctldev, 
                                                    kuint32_t func_selector, kuint32_t group_selector)
{
    srt_imx_iomuxc_data_t *sprt_data;

    sprt_data = (srt_imx_iomuxc_data_t *)fwk_pinctrl_get_drvdata(sprt_pctldev);
    return sprt_data->sprt_pinfuncs[func_selector].sprt_pingrps[group_selector].name;
}

/*!
 * @brief   get pin name (struct fwk_pinctrl_pin_desc::name)
 * @param   sprt_pctldev: pinctrl_dev
 * @param   number: struct fwk_pinctrl_pin_desc::number
 * @retval  name
 * @note    none
 */
static const kchar_t *imx_iomuxc_get_pin_desc(struct fwk_pinctrl_dev *sprt_pctldev, kuint32_t number)
{
    const struct fwk_pinctrl_pin_desc *sprt_pdesc;

    sprt_pdesc = sprt_pctldev->sprt_desc->sprt_pins;
    return (sprt_pdesc[number].number != number) ? mrt_nullptr : sprt_pdesc[number].name;
}

static const struct fwk_pinctrl_ops sgrt_imx_iomuxc_pctl_oprts =
{
    .get_function_groups    = imx_iomuxc_get_function_groups,
    .get_groups_count       = imx_iomuxc_get_groups_count,
    .get_group_name         = imx_iomuxc_get_group_name,
    .get_pin_desc           = imx_iomuxc_get_pin_desc,

    .dt_node_to_map         = imx_iomuxc_dt_node_to_map,
    .dt_free_map            = imx_iomuxc_dt_free_map,
};

/*!
 * @brief   request one pin
 * @param   sprt_pctldev, selector
 * @retval  errno
 * @note    none
 */
static kint32_t imx_iomuxc_request(struct fwk_pinctrl_dev *sprt_pctldev, kuint32_t offset)
{
    return ER_NORMAL;
}

/*!
 * @brief   free one pin
 * @param   sprt_pctldev, offset
 * @retval  errno
 * @note    none
 */
static kint32_t imx_iomuxc_free(struct fwk_pinctrl_dev *sprt_pctldev, kuint32_t offset)
{
    return ER_NORMAL;
}

/*!
 * @brief   get the number of functions
 * @param   sprt_pctldev
 * @retval  errno
 * @note    none
 */
static kint32_t imx_iomuxc_get_functions_count(struct fwk_pinctrl_dev *sprt_pctldev)
{
    srt_imx_iomuxc_data_t *sprt_data;

    sprt_data = (srt_imx_iomuxc_data_t *)fwk_pinctrl_get_drvdata(sprt_pctldev);
    return sprt_data->nfuncs;
}

/*!
 * @brief   get function name
 * @param   sprt_pctldev, selector
 * @retval  errno
 * @note    none
 */
static const kchar_t *imx_iomuxc_get_function_name(struct fwk_pinctrl_dev *sprt_pctldev, kuint32_t selector)
{
    srt_imx_iomuxc_data_t *sprt_data;

    sprt_data = (srt_imx_iomuxc_data_t *)fwk_pinctrl_get_drvdata(sprt_pctldev);
    return sprt_data->sprt_pinfuncs[selector].name;
}

/*!
 * @brief   configure pin mux register
 * @param   sprt_pctldev: pin controller device
 * @param   func_selector: function index
 * @param   group_selector: group index
 * @retval  errno
 * @note    none
 */
static kint32_t imx_iomuxc_set_mux(struct fwk_pinctrl_dev *sprt_pctldev, kuint32_t func_selector, kuint32_t group_selector)
{
    srt_imx_iomuxc_data_t *sprt_data;
    srt_imx_iomuxc_pin_func_t *sprt_pinfunc;
    srt_imx_iomuxc_pin_grp_t *sprt_pingrp;
    srt_imx_iomuxc_pin_t *sprt_pin;
    kuaddr_t base;
    kuint32_t i;

    if (!sprt_pctldev)
        return -ER_NODEV;

    sprt_data = (srt_imx_iomuxc_data_t *)fwk_pinctrl_get_drvdata(sprt_pctldev);
    if (!sprt_data || !sprt_data->sprt_pinfuncs || (func_selector >= sprt_data->nfuncs))
        return -ER_NODEV;

    /*!< get function */
    sprt_pinfunc = &sprt_data->sprt_pinfuncs[func_selector];
    if (!sprt_pinfunc->sprt_pingrps || (group_selector >= sprt_pinfunc->ngrps))
        return -ER_NODEV;

    /*!< get group */
    sprt_pingrp = &sprt_pinfunc->sprt_pingrps[group_selector];
    if (!sprt_pingrp->sprt_pin || !sprt_pingrp->npins)
        return -ER_NODEV;

    base = (kuaddr_t)sprt_data->base;

    /*!< configure all pins of this group */
    for (i = 0; i < sprt_pingrp->npins; i++)
    {
        sprt_pin = &sprt_pingrp->sprt_pin[i];
        mrt_writel(sprt_pin->sgrt_pin.mux_data, sprt_pin->sgrt_pin.mux_base + base);

        if (sprt_pin->sgrt_pin.input_base)
            mrt_writel(sprt_pin->sgrt_pin.input_data, sprt_pin->sgrt_pin.input_base + base);
    }

    return ER_NORMAL;
}

/*!
 * @brief   request and enable gpio
 * @param   sprt_pctldev: pin controller device
 * @param   sprt_range: gpio data
 * @param   offset: pin index
 * @retval  errno
 * @note    none
 */
static kint32_t imx_iomuxc_gpio_request_enable(struct fwk_pinctrl_dev *sprt_pctldev, 
										struct fwk_pinctrl_gpio_range *sprt_range, kuint32_t offset)
{
    return ER_NORMAL;
}

/*!
 * @brief   disable and free gpio
 * @param   sprt_pctldev: pin controller device
 * @param   sprt_range: gpio data
 * @param   offset: pin index
 * @retval  errno
 * @note    none
 */
static void imx_iomuxc_gpio_disable_free(struct fwk_pinctrl_dev *sprt_pctldev, 
										struct fwk_pinctrl_gpio_range *sprt_range, kuint32_t offset)
{

}

/*!
 * @brief   set pin direction
 * @param   sprt_pctldev: pin controller device
 * @param   sprt_range: gpio data
 * @param   offset: pin index
 * @param   input: direction (1: input; 0: output)
 * @retval  errno
 * @note    none
 */
static kint32_t imx_iomuxc_gpio_set_direction(struct fwk_pinctrl_dev *sprt_pctldev, 
										struct fwk_pinctrl_gpio_range *sprt_range, kuint32_t offset, kbool_t input)
{
    return ER_NORMAL;
}

static const struct fwk_pinmux_ops sgrt_imx_iomuxc_pinmux_oprts =
{
    .request                = imx_iomuxc_request,
    .free                   = imx_iomuxc_free,
    .get_functions_count    = imx_iomuxc_get_functions_count,
    .get_function_name      = imx_iomuxc_get_function_name,
    .set_mux                = imx_iomuxc_set_mux,
    .gpio_request_enable    = imx_iomuxc_gpio_request_enable,
    .gpio_disable_free      = imx_iomuxc_gpio_disable_free,
    .gpio_set_direction     = imx_iomuxc_gpio_set_direction,
};

/*!
 * @brief   configure pin conf register
 * @param   sprt_pctldev: pin controller device
 * @param   pin: struct fwk_pinctrl_pin_desc::number
 * @param   config: address of conf data
 * @retval  errno
 * @note    none
 */
static kint32_t imx_iomuxc_pin_config_get(struct fwk_pinctrl_dev *sprt_pctldev, kuint32_t pin, kuint32_t *config)
{
    srt_imx_iomuxc_data_t *sprt_data;
    srt_imx_iomuxc_pin_res_t *sprt_pins;
    kuaddr_t base;

    if (!sprt_pctldev)
        return -ER_NODEV;

    sprt_data = (srt_imx_iomuxc_data_t *)fwk_pinctrl_get_drvdata(sprt_pctldev);
    if (!sprt_data)
        return -ER_NODEV;

    base = (kuaddr_t)sprt_data->base;
    sprt_pins = &sprt_data->sprt_pins[pin];
    if (!sprt_pins->conf_base)
        return -ER_NODEV;

    *config = mrt_readl(base + sprt_pins->conf_base);

    return ER_NORMAL;
}

/*!
 * @brief   configure pin conf register
 * @param   sprt_pctldev: pin controller device
 * @param   pin: struct fwk_pinctrl_pin_desc::number
 * @param   configs: address of conf data
 * @param   num_configs: the number of configs, is 1 most likely
 * @retval  errno
 * @note    none
 */
static kint32_t imx_iomuxc_pin_config_set(struct fwk_pinctrl_dev *sprt_pctldev, 
                                        kuint32_t pin, kuint32_t *configs, kuint32_t num_configs)
{
    srt_imx_iomuxc_data_t *sprt_data;
    srt_imx_iomuxc_pin_res_t *sprt_pins;
    kuaddr_t base;
    kuint32_t i, reg_data;

    if (!sprt_pctldev)
        return -ER_NODEV;

    sprt_data = (srt_imx_iomuxc_data_t *)fwk_pinctrl_get_drvdata(sprt_pctldev);
    if (!sprt_data)
        return -ER_NODEV;

    base = (kuaddr_t)sprt_data->base;
    sprt_pins = &sprt_data->sprt_pins[pin];
    if (!sprt_pins->conf_base)
        return -ER_NODEV;

    reg_data = mrt_readl(base + sprt_pins->conf_base);
    reg_data &= ~0xffff;
    
    for (i = 0; i < num_configs; i++)
        reg_data |= configs[i];

    mrt_writel(reg_data, base + sprt_pins->conf_base);
    
    return ER_NORMAL;
}

static const struct fwk_pinconf_ops sgrt_imx_iomuxc_pinconf_oprts =
{
    .pin_config_get = imx_iomuxc_pin_config_get,
    .pin_config_set = imx_iomuxc_pin_config_set,
};

/*!
 * @brief   destroy pinctrl info
 * @param   sprt_pdev
 * @retval  errno
 * @note    none
 */
static void imx_iomuxc_driver_remove_dt(struct fwk_platdev *sprt_pdev)
{
    srt_imx_iomuxc_pin_func_t *sprt_pinfunc;
    srt_imx_iomuxc_pin_grp_t *sprt_pingrp;
    srt_imx_iomuxc_pin_res_t *sprt_pins;
    srt_imx_iomuxc_data_t *sprt_data = fwk_platform_get_drvdata(sprt_pdev);
    kuint32_t i, j;

    if (!sprt_data->sprt_pinfuncs)
        return;

    for (i = 0; i < sprt_data->nfuncs; i++)
    {
        sprt_pinfunc = &sprt_data->sprt_pinfuncs[i];

        if (!sprt_pinfunc->sprt_pingrps)
            continue;

        for (j = 0; j < sprt_pinfunc->ngrps; j++)
        {
            sprt_pingrp = &sprt_pinfunc->sprt_pingrps[j];

            if (sprt_pingrp->sprt_pin)
                kfree(sprt_pingrp->sprt_pin);
        }

        kfree(sprt_pinfunc->sprt_pingrps);
    }

    kfree(sprt_data->sprt_pinfuncs);
    sprt_data->sprt_pinfuncs = mrt_nullptr;
    sprt_data->nfuncs = 0;

    sprt_pins = sprt_data->sprt_pins;
    memset(sprt_pins, 0, sprt_data->npins * sizeof(*sprt_pins));
}

/*!
 * @brief   parse pinctrl device-node
 * @param   sprt_pdev
 * @retval  errno
 * @note    parse current iomuxc, save every pin info to sprt_data
 * 
 *          sprt_pinfuncs:  boards, such as "imx6ul_evk", including multiple sprt_pinfunc;
 *          nfuncs:         the number of sprt_pinfunc
 * 
 *          sprt_pingrps:   groups of one function, including multiple sprt_pingrp
 *          ngrps:          the number of sprt_pingrp
 * 
 *          sprt_pingrp:    indicates a set of pins for the same purpose
 *          sprt_pin/sprt_conf: pin, consists of mux_reg/data, conf_reg/data, input_reg/data
 * 
 *          sprt_func:      function device node, such as "imx6ul_evk"
 *          sprt_grp:       group device node, such as "pinctrl_hog_2: hoggrp-2"
 *          sprt_prop:      property of group node, such as "fsl,pins"
 * 
 *          sprt_pins:      one pin, including the reg offsets of mux and conf
 */
static kint32_t imx_iomuxc_driver_probe_dt(struct fwk_platdev *sprt_pdev)
{
    srt_imx_iomuxc_data_t *sprt_data;
    struct fwk_device_node *sprt_node, *sprt_func, *sprt_grp;
    struct fwk_of_property *sprt_prop;
    srt_imx_iomuxc_pin_t *sprt_pin;
    srt_hal_imx_pin_t *sprt_conf;
    srt_imx_iomuxc_pin_func_t *sprt_pinfunc, *sprt_pinfuncs;
    srt_imx_iomuxc_pin_grp_t *sprt_pingrp, *sprt_pingrps;
    srt_imx_iomuxc_pin_res_t *sprt_pins;
    kuint32_t pin, nfuncs, ngrps, npins;
    kusize_t prop_size = 0;

    /*!< node: "iomux" or "iomuxc_snvs" */
    sprt_node = sprt_pdev->sgrt_dev.sprt_node;
    sprt_data = (srt_imx_iomuxc_data_t *)fwk_platform_get_drvdata(sprt_pdev);
    sprt_pins = sprt_data->sprt_pins;

    /*!< how many boards here (sprt_node: iomuxc/iomuxc_snvs) */
    nfuncs = fwk_of_get_child_count(sprt_node);
    if (!nfuncs)
        return -ER_EMPTY;

    sprt_pinfuncs = (srt_imx_iomuxc_pin_func_t *)kzalloc(nfuncs * sizeof(*sprt_pinfuncs), GFP_KERNEL);
    if (!isValid(sprt_pinfuncs))
        return -ER_NOMEM;

    sprt_data->sprt_pinfuncs = sprt_pinfuncs;
    sprt_data->nfuncs = nfuncs;
    sprt_pinfunc = &sprt_pinfuncs[0];

    /*!< parse each function (such as "imx6ul_evk") */
    foreach_fwk_of_child(sprt_node, sprt_func)
    {
        /*!< name = group's node of dts */
        sprt_pinfunc->name = sprt_func->name;

        ngrps = fwk_of_get_child_count(sprt_func);
        if (!ngrps)
        {
            sprt_pinfunc++;
            continue;
        }

        sprt_pingrps = (srt_imx_iomuxc_pin_grp_t *)kzalloc(ngrps * sizeof(*sprt_pingrps), GFP_KERNEL);
        if (!isValid(sprt_pingrps))
            goto fail;

        sprt_pinfunc->ngrps = ngrps;
        sprt_pinfunc->sprt_pingrps = sprt_pingrps;
        sprt_pingrp = &sprt_pingrps[0];

        /*!< parse each group */
        foreach_fwk_of_child(sprt_func, sprt_grp)
        {
            kuint32_t value[IMX6UL_MUX_PIN_PER_LENTH];
            kuint32_t idx, v_size;

            sprt_pingrp->name = sprt_grp->name;

            /*!<
             * v_size: lenth of value array (IMX6UL_MUX_PIN_PER_LENTH)
             * prop_size: bytes of property; lenth = bytes / 4 = prop_size >> 2
             * npins: check if it is an integer multiple of v_size
             * 
             * 1, if it is empty node, prop_size maybe 0, or sprt_prop is null, --->
             *    this is a normal case, let's to save node name but sprt_pingrp->npins = 0;
             * 2, if pin format is incorrect, skip the node too
             */
            v_size = ARRAY_SIZE(value);
            sprt_prop = fwk_of_find_property(sprt_grp, "fsl,pins", &prop_size);
            prop_size >>= 2;
            npins = prop_size % v_size;
            if (!prop_size || !sprt_prop || npins)
            {
                sprt_pingrp++;
                continue;
            }

            /*!< get the number of pins */
            npins = prop_size / v_size;
            sprt_pin = (srt_imx_iomuxc_pin_t *)kzalloc(npins * sizeof(*sprt_pin), GFP_KERNEL);
            if (!isValid(sprt_pin))
                goto fail;

            sprt_pingrp->sprt_pin = sprt_pin;
            sprt_pingrp->npins = npins;

            for (idx = 0; idx < npins; idx++)
            {
                if (fwk_of_property_read_u32_array_index(sprt_grp, "fsl,pins", value, idx * v_size, v_size))
                    goto fail;
                
                sprt_conf = &sprt_pin[idx].sgrt_pin;

                if (value[5] & IMX6UL_MUX_PIN_CONFIG_SION)
                {
                    value[5] &= ~IMX6UL_MUX_PIN_CONFIG_SION;
                    hal_imx_pin_auto_init(sprt_conf, 0, value, IMX6UL_PIN_MUX_FUNC_ENABLE, v_size);
                }
                else
                    hal_imx_pin_auto_init(sprt_conf, 0, value, IMX6UL_PIN_MUX_FUNC_DISABLE, v_size);

                /*!< pin refers to the index of "struct fwk_pinctrl_pin_desc[]" */
                /*!< or: pin = sprt_conf->pad_base >> 2 */
                pin = sprt_conf->mux_base >> 2;
                if (pin > sprt_data->npins)
                    goto fail;

                sprt_pin[idx].pin = pin;
                sprt_pins[pin].mux_base = sprt_conf->mux_base;
                sprt_pins[pin].conf_base = sprt_conf->pad_base;
            }

            sprt_pingrp++;
        }

        sprt_pinfunc++;
    }

    return ER_NORMAL;

fail:
    imx_iomuxc_driver_remove_dt(sprt_pdev);

    return -ER_ERROR;    
}

/*!
 * @brief   driver probe
 * @param   sprt_pdev
 * @retval  errno
 * @note    none
 */
static kint32_t imx_iomuxc_driver_probe(struct fwk_platdev *sprt_pdev)
{
    srt_imx_iomuxc_data_t *sprt_data;
    struct fwk_device_node *sprt_node;
    struct fwk_of_device_id *sprt_id;
    srt_imx_iomuxc_pads_t *sprt_pad;
    struct fwk_pinctrl_desc *sprt_desc;
    srt_imx_iomuxc_pin_res_t *sprt_pins;
    struct fwk_driver *sprt_drv;
    struct fwk_pinctrl *sprt_pctl;
    struct fwk_pinctrl_state *sprt_state;

    /*!< compatible: "iomuxc", "iomuxc_snvs" */
    sprt_node = sprt_pdev->sgrt_dev.sprt_node;

    /*!< private driver data for imx */
    sprt_data = (srt_imx_iomuxc_data_t *)kzalloc(sizeof(*sprt_data), GFP_KERNEL);
    if (!isValid(sprt_data))
        return -ER_NOMEM;

    /*!< get the reg address of "iomuxc"/"iomuxc_snvs" */
    sprt_data->base = fwk_of_iomap(sprt_node, 0);
    sprt_data->base = fwk_io_remap(sprt_data->base);
    if (!sprt_data->base)
        goto fail1;

    /*!< acknowledge "iomuxc" or "iomuxc_snvs" */
    sprt_drv = sprt_pdev->sgrt_dev.sprt_driver;
    sprt_id = fwk_of_match_node(sprt_drv->sprt_of_match_table, sprt_node);
    sprt_pad = (srt_imx_iomuxc_pads_t *)sprt_id->data;

    /*!< allocate; but initialized in "imx_iomuxc_driver_probe_dt" */
    sprt_pins = (srt_imx_iomuxc_pin_res_t *)kzalloc(sprt_pad->npins * sizeof(*sprt_pins), GFP_KERNEL);
    if (!isValid(sprt_pins))
        goto fail2;

    /*!< every iomux controller will has only one desc */
    sprt_desc = (struct fwk_pinctrl_desc *)kzalloc(sizeof(*sprt_desc), GFP_KERNEL);
    if (!isValid(sprt_data))
        goto fail3;

    /*!< sprt_pins is a private data too */
    sprt_data->sprt_pins    = sprt_pins;
    sprt_data->npins        = sprt_pad->npins;

    /*!< sprt_pad->sprt_pins: all pins arrary */
    sprt_desc->name         = sprt_pdev->name;
    sprt_desc->sprt_pins    = sprt_pad->sprt_pins;
    sprt_desc->npins        = sprt_pad->npins;
    sprt_desc->sprt_pctlops = &sgrt_imx_iomuxc_pctl_oprts;
    sprt_desc->sprt_pmxops  = &sgrt_imx_iomuxc_pinmux_oprts;
    sprt_desc->sprt_confops = &sgrt_imx_iomuxc_pinconf_oprts;
    fwk_platform_set_drvdata(sprt_pdev, sprt_data);

    /*!<
     * parse pinctrl node
     * 1. 
     */
    if (imx_iomuxc_driver_probe_dt(sprt_pdev))
        goto fail4;

    /*!< here is no pinctrl*/
    if (!sprt_data->sprt_pinfuncs)
        goto fail5;

    /*!< 
     * sprt_pctldev->sprt_desc = sprt_desc;
     * sprt_pctldev->sprt_dev = &sprt_pdev->sgrt_dev;
     * sprt_pctldev->driver_data = sprt_data
     * 
     * create pinctrl_dev and pinctrl (parse current device_node)
     */
    sprt_data->sprt_pctldev = fwk_pinctrl_register(sprt_desc, &sprt_pdev->sgrt_dev, sprt_data);
    if (!isValid(sprt_data->sprt_pctldev))
        goto fail5;

    sprt_pctl = sprt_data->sprt_pctldev->sprt_pctl;
    if (!sprt_pctl->sprt_state)
    {
        sprt_state = fwk_pinctrl_lookup_state(sprt_pctl, "default_snvs");
        if (sprt_state)
        {
            fwk_pinctrl_select_state(sprt_pctl, sprt_state);
            sprt_data->sprt_pctldev->sprt_hog_default = sprt_state;
        }
    }

    return ER_NORMAL;

fail5:
    imx_iomuxc_driver_remove_dt(sprt_pdev);
fail4:
    fwk_platform_set_drvdata(sprt_pdev, mrt_nullptr);
fail3:
    kfree(sprt_pins);
fail2:
    fwk_io_unmap(sprt_data->base);
fail1:
    kfree(sprt_data);

    return -ER_ERROR;
}

/*!
 * @brief   driver remove
 * @param   sprt_pdev
 * @retval  errno
 * @note    none
 */
static kint32_t imx_iomuxc_driver_remove(struct fwk_platdev *sprt_pdev)
{
    srt_imx_iomuxc_data_t *sprt_data;
    struct fwk_pinctrl_desc *sprt_desc;

    sprt_data = (srt_imx_iomuxc_data_t *)fwk_platform_get_drvdata(sprt_pdev);
    sprt_desc = sprt_data->sprt_pctldev->sprt_desc;

    fwk_pinctrl_unregister(sprt_data->sprt_pctldev);
    imx_iomuxc_driver_remove_dt(sprt_pdev);
    fwk_platform_set_drvdata(sprt_pdev, mrt_nullptr);
    kfree(sprt_data->sprt_pins);
    kfree(sprt_desc);
    fwk_io_unmap(sprt_data->base);
    kfree(sprt_data);

    return ER_NORMAL;
}

/*!< platform instance */
static struct fwk_platdrv sgrt_imx_iomuxc_platdriver =
{
	.probe	= imx_iomuxc_driver_probe,
	.remove	= imx_iomuxc_driver_remove,
	
	.sgrt_driver =
	{
		.name 	= "freescale, imx6ull, pinctrl",
		.id 	= -1,
		.sprt_of_match_table = sgrt_imx_iomuxc_driver_ids,
	},
};

/*!< --------------------------------------------------------------------- */
/*!
 * @brief   driver init
 * @param   none
 * @retval  errno
 * @note    none
 */
kint32_t __fwk_init imx_iomuxc_driver_init(void)
{
	return fwk_register_platdriver(&sgrt_imx_iomuxc_platdriver);
}

/*!
 * @brief   driver exit
 * @param   none
 * @retval  none
 * @note    none
 */
void __fwk_exit imx_iomuxc_driver_exit(void)
{
	fwk_unregister_platdriver(&sgrt_imx_iomuxc_platdriver);
}

IMPORT_PATTERN_INIT(imx_iomuxc_driver_init);
IMPORT_PATTERN_EXIT(imx_iomuxc_driver_exit);

/*!< end of file */
