/*
 * ZYNQ7 Board HDMI Initial
 *
 * File Name:   zynq7_hdmi.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.10.31
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include "zynq7_common.h"

#include <platform/video/fwk_font.h>
#include <platform/video/fwk_disp.h>
#include <platform/video/fwk_rgbmap.h>

/*!< The defines */

/*!< The globals */
kuint8_t VMODE_BUFFER[1920 * 1080 * 4 * DISPLAY_NUM_FRAMES] __align(8);

static const VideoMode VMODE_1920x1080 = {
	.label = "1920x1080@60Hz",
	.width = 1920,
	.height = 1080,
	.hps = 2008,
	.hpe = 2052,
	.hmax = 2199,
	.hpol = 1,
	.vps = 1084,
	.vpe = 1089,
	.vmax = 1124,
	.vpol = 1,
	.freq = 148.5
};

static DisplayCtrl sgrt_xil_display_ctrl;
static XAxiVdma sgrt_xil_axivdma;
static struct fwk_disp_info sgrt_xil_hdmi_disp[DISPLAY_NUM_FRAMES];

/*!< API function */
/*!
 * @brief   initial and start hdmi video
 * @param   none
 * @retval  none
 * @note    none
 */
void zynq7_hdmi_init(void)
{
	XAxiVdma_Config *sprt_axicfg;
    XVtc_Config *sprt_vcfg;
    kuint8_t *pFrames[DISPLAY_NUM_FRAMES];
    VideoMode *sprt_vmode;
    kint32_t index, retval;

	for (index = 0; index < DISPLAY_NUM_FRAMES; index++)
		pFrames[index] = VMODE_BUFFER + index * (1920 * 1080 * 4);

    sprt_axicfg = XAxiVdma_LookupConfig(XPAR_AXIVDMA_0_DEVICE_ID);
    if (!sprt_axicfg)
        goto kill;

    retval = XAxiVdma_CfgInitialize(&sgrt_xil_axivdma, sprt_axicfg, sprt_axicfg->BaseAddress);
    if (retval)
        goto kill;

    sprt_vmode = (VideoMode *)(&VMODE_1920x1080);
    retval = DisplayInitialize(&sgrt_xil_display_ctrl, &sgrt_xil_axivdma, XPAR_VTC_0_DEVICE_ID, 
                                    XPAR_AXI_DYNCLK_0_BASEADDR, pFrames, 1920 * 4, sprt_vmode);
    if (retval)
        goto kill;

    sprt_vcfg = XVtc_LookupConfig(XPAR_V_TC_0_DEVICE_ID);
    if (!sprt_vcfg)
        goto kill;

    retval = XVtc_CfgInitialize(&(sgrt_xil_display_ctrl.vtc), sprt_vcfg, sprt_vcfg->BaseAddress);
    if (retval)
        goto kill;

    retval = DisplayStart(&sgrt_xil_display_ctrl);
    if (retval)
        goto kill;

    fwk_display_ctrl_init(&sgrt_xil_hdmi_disp[0], &VMODE_BUFFER[0], 1920 * 1080 * 4, 1920, 1080, FWK_RGB_PIXEL32);
    sgrt_xil_hdmi_disp[0].sprt_ops->clear(&sgrt_xil_hdmi_disp[0], RGB_WHITE);

kill:
    return;
}

/* end of file*/
