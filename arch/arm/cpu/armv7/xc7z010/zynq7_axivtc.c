/*
 * ZYNQ7 Peripheral APIs For AXIVDMA and VTC
 *
 * File Name:   zynq7_axivtc.c
 * Author:      Yang Yujun (Copy from "Xilinx SDK")
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.10.30
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <configs/configs.h>
#include <asm/armv7/gcc_config.h>
#include <asm/zynq7/zynq7_periph.h>
#include <common/time.h>
#include <common/api_string.h>

/*!< The defines */
/* The polling upon starting the hardware
 *
 * We have the assumption that reset is fast upon hardware start
 */
#define INITIALIZATION_POLLING                  100000

/* Translate virtual address to physical address */
#define XAXIVDMA_VIRT_TO_PHYS(VirtAddr)         (VirtAddr)

/* Debug Configuration Parameter Constants (C_ENABLE_DEBUG_INFO_*) */
#define XAXIVDMA_ENABLE_DBG_THRESHOLD_REG	    0x01
#define XAXIVDMA_ENABLE_DBG_FRMSTORE_REG	    0x02
#define XAXIVDMA_ENABLE_DBG_FRM_CNTR	        0x04
#define XAXIVDMA_ENABLE_DBG_DLY_CNTR	        0x08
#define XAXIVDMA_ENABLE_DBG_ALL_FEATURES	    0x0F

/** GenLock Mode Constants */
#define XAXIVDMA_GENLOCK_MASTER		            0
#define XAXIVDMA_GENLOCK_SLAVE		            1
#define XAXIVDMA_DYN_GENLOCK_MASTER	            2
#define XAXIVDMA_DYN_GENLOCK_SLAVE	            3

/** VDMA data transfer direction */
#define XAXIVDMA_WRITE                          1           /**< DMA transfer into memory */
#define XAXIVDMA_READ                           2           /**< DMA transfer from memory */

/**
* This typedef contains Timing (typically in Display Timing) format
* configuration information for the VTC core.
*/
typedef struct 
{
    /* Horizontal Timing */
    kuint16_t HActiveVideo;	                                /**< Horizontal Active Video Size */
    kuint16_t HFrontPorch;	                                /**< Horizontal Front Porch Size */
    kuint16_t HSyncWidth;		                            /**< Horizontal Sync Width */
    kuint16_t HBackPorch;		                            /**< Horizontal Back Porch Size */
    kuint16_t HSyncPolarity;	                            /**< Horizontal Sync Polarity */

    /* Vertical Timing */
    kuint16_t VActiveVideo;	                                /**< Vertical Active Video Size */
    kuint16_t V0FrontPorch;	                                /**< Vertical Front Porch Size */
    kuint16_t V0SyncWidth;	                                /**< Vertical Sync Width */
    kuint16_t V0BackPorch;	                                /**< Horizontal Back Porch Size */

    kuint16_t V1FrontPorch;	                                /**< Vertical Front Porch Size */
    kuint16_t V1SyncWidth;	                                /**< Vertical Sync Width */
    kuint16_t V1BackPorch;	                                /**< Vertical Back Porch Size */

    kuint16_t VSyncPolarity;	                            /**< Vertical Sync Polarity */

    kuint8_t Interlaced;		                            /**< Interlaced / Progressive video */

} XVtc_Timing;

/**
 * This typedef contains Polarity configuration information for a VTC core.
 */
typedef struct 
{
    kuint8_t ActiveChromaPol;	                            /**< Active Chroma Output Polarity */
    kuint8_t ActiveVideoPol;	                            /**< Active Video Output Polarity */
    kuint8_t FieldIdPol;		                            /**< Field ID Output Polarity */
    kuint8_t VBlankPol;		                                /**< Vertical Blank Output Polarity */
    kuint8_t VSyncPol;		                                /**< Vertical Sync Output Polarity */
    kuint8_t HBlankPol;		                                /**< Horizontal Blank Output Polarity */
    kuint8_t HSyncPol;		                                /**< Horizontal Sync Output Polarity */

} XVtc_Polarity;

/**
 * This typedef contains the VTC signal configuration used by the
 * Generator/Detector modules in a VTC device.
 */
typedef struct 
{
    kuint16_t OriginMode;		                            /**< Origin Mode */
    kuint16_t HTotal;		                                /**< Horizontal total clock cycles per Line */
    kuint16_t HFrontPorchStart;	                            /**< Horizontal Front Porch Start Cycle Count */
    kuint16_t HSyncStart;		                            /**< Horizontal Sync Start Cycle Count */
    kuint16_t HBackPorchStart;	                            /**< Horizontal Back Porch Start Cycle Count */
    kuint16_t HActiveStart;	                                /**< Horizontal Active Video Start Cycle Count */

    kuint16_t V0Total;		                                /**< Total lines per Frame (Field 0) */
    kuint16_t V0FrontPorchStart;	                        /**< Vertical Front Porch Start Line Count * (Field 0) */
    kuint16_t V0SyncStart;	                                /**< Vertical Sync Start Line Count (Field 0) */
    kuint16_t V0BackPorchStart;	                            /**< Vertical Back Porch Start Line Count *  (Field 0) */
    kuint16_t V0ActiveStart;	                            /**< Vertical Active Video Start Line Count *  (Field 0) */
    kuint16_t V0ChromaStart;	                            /**< Active Chroma Start Line Count (Field 0) */

    kuint16_t V1Total;		                                /**< Total lines per Frame (Field 1) */
    kuint16_t V1FrontPorchStart;	                        /**< Vertical Front Porch Start Line Count *  (Field 1) */
    kuint16_t V1SyncStart;	                                /**< Vertical Sync Start Line Count (Field 1) */
    kuint16_t V1BackPorchStart;	                            /**< Vertical Back Porch Start Line Count  (Field 1) */
    kuint16_t V1ActiveStart;	                            /**< Vertical Active Video Start Line Count (Field 1) */
    kuint16_t V1ChromaStart;	                            /**< Active Chroma Start Line Count (Field 1) */
    kuint8_t Interlaced;		                            /**< Interlaced / Progressive video */

} XVtc_Signal;

/**
 * This typedef contains Detector/Generator VBlank/VSync Horizontal Offset
 * configuration information for a VTC device.
 */
typedef struct 
{
    kuint16_t V0BlankHoriStart;	                            /**< Vertical Blank Hori Offset Start (field 0) */
    kuint16_t V0BlankHoriEnd;	                            /**< Vertical Blank Hori Offset End (field 0) */
    kuint16_t V0SyncHoriStart;	                            /**< Vertical Sync  Hori Offset Start (field 0) */
    kuint16_t V0SyncHoriEnd;	                            /**< Vertical Sync  Hori Offset End (field 0) */
    kuint16_t V1BlankHoriStart;	                            /**< Vertical Blank Hori Offset Start (field 1) */
    kuint16_t V1BlankHoriEnd;	                            /**< Vertical Blank Hori Offset End (field 1) */
    kuint16_t V1SyncHoriStart;	                            /**< Vertical Sync  Hori Offset Start (field 1) */
    kuint16_t V1SyncHoriEnd;	                            /**< Vertical Sync  Hori Offset End (field 1) */

} XVtc_HoriOffsets;

/**
 * This typedef contains Source Selection configuration information for a
 * VTC core.
 */
typedef struct 
{
    kuint8_t FieldIdPolSrc;	                                /**< Field ID Output Polarity Source */
    kuint8_t ActiveChromaPolSrc;	                        /**< Active Chroma Output Polarity Source */
    kuint8_t ActiveVideoPolSrc;	                            /**< Active Video Output Polarity Source */
    kuint8_t HSyncPolSrc;		                            /**< Horizontal Sync Output Polarity Source */
    kuint8_t VSyncPolSrc;		                            /**< Vertical Sync Output Polarity Source */
    kuint8_t HBlankPolSrc;	                                /**< Horizontal Blank Output Polarity Source */
    kuint8_t VBlankPolSrc;	                                /**< Vertical Blank Output Polarity Source */

    kuint8_t VChromaSrc;		                            /**< Start of Active Chroma Register Source Select */
    kuint8_t VActiveSrc;		                            /**< Vertical Active Video Start Register Source Select */
    kuint8_t VBackPorchSrc;	                                /**< Vertical Back Porch Start Register Source Select */
    kuint8_t VSyncSrc;		                                /**< Vertical Sync Start Register Source Select */
    kuint8_t VFrontPorchSrc;	                            /**< Vertical Front Porch Start Register Source Select */
    kuint8_t VTotalSrc;		                                /**< Vertical Total Register Source Select */
    kuint8_t HActiveSrc;		                            /**< Horizontal Active Video Start Register Source Select */
    kuint8_t HBackPorchSrc;	                                /**< Horizontal Back Porch Start Register Source Select */
    kuint8_t HSyncSrc;		                                /**< Horizontal Sync Start Register Source Select */
    kuint8_t HFrontPorchSrc;	                            /**< Horizontal Front Porch Start Register Source Select */
    kuint8_t HTotalSrc;		                                /**< Horizontal Total Register Source Select */
    kuint8_t InterlacedMode;	                            /**< Interelaced mode */

} XVtc_SourceSelect;

/*!< The globals */
static XAxiVdma_Config XAxiVdma_ConfigTable[XPAR_XAXIVDMA_NUM_INSTANCES] =
{
    {
        XPAR_AXI_VDMA_0_DEVICE_ID,
        XPAR_AXI_VDMA_0_BASEADDR,

        XPAR_AXI_VDMA_0_NUM_FSTORES,

        XPAR_AXI_VDMA_0_INCLUDE_MM2S,
        XPAR_AXI_VDMA_0_INCLUDE_MM2S_DRE,
        XPAR_AXI_VDMA_0_M_AXI_MM2S_DATA_WIDTH,
        XPAR_AXI_VDMA_0_INCLUDE_S2MM,
        XPAR_AXI_VDMA_0_INCLUDE_S2MM_DRE,
        XPAR_AXI_VDMA_0_M_AXI_S2MM_DATA_WIDTH,
        XPAR_AXI_VDMA_0_INCLUDE_SG,
        XPAR_AXI_VDMA_0_ENABLE_VIDPRMTR_READS,

        XPAR_AXI_VDMA_0_USE_FSYNC,
        XPAR_AXI_VDMA_0_FLUSH_ON_FSYNC,
        XPAR_AXI_VDMA_0_MM2S_LINEBUFFER_DEPTH,
        XPAR_AXI_VDMA_0_S2MM_LINEBUFFER_DEPTH,
        XPAR_AXI_VDMA_0_MM2S_GENLOCK_MODE,
        XPAR_AXI_VDMA_0_S2MM_GENLOCK_MODE,
        XPAR_AXI_VDMA_0_INCLUDE_INTERNAL_GENLOCK,
        XPAR_AXI_VDMA_0_S2MM_SOF_ENABLE,
        XPAR_AXI_VDMA_0_M_AXIS_MM2S_TDATA_WIDTH,
        XPAR_AXI_VDMA_0_S_AXIS_S2MM_TDATA_WIDTH,
        XPAR_AXI_VDMA_0_ENABLE_DEBUG_INFO_1,
        XPAR_AXI_VDMA_0_ENABLE_DEBUG_INFO_5,
        XPAR_AXI_VDMA_0_ENABLE_DEBUG_INFO_6,
        XPAR_AXI_VDMA_0_ENABLE_DEBUG_INFO_7,
        XPAR_AXI_VDMA_0_ENABLE_DEBUG_INFO_9,
        XPAR_AXI_VDMA_0_ENABLE_DEBUG_INFO_13,
        XPAR_AXI_VDMA_0_ENABLE_DEBUG_INFO_14,
        XPAR_AXI_VDMA_0_ENABLE_DEBUG_INFO_15,
        XPAR_AXI_VDMA_0_ENABLE_DEBUG_ALL,
        XPAR_AXI_VDMA_0_ADDR_WIDTH,
        XPAR_AXI_VDMA_0_ENABLE_VERT_FLIP
    }
};

static XVtc_Config XVtc_ConfigTable[XPAR_XVTC_NUM_INSTANCES] =
{
    {
        XPAR_V_TC_0_DEVICE_ID,
        XPAR_V_TC_0_BASEADDR
    }
};

static const kuint64_t lock_lookup[64] = 
{
    0b0011000110111110100011111010010000000001,
    0b0011000110111110100011111010010000000001,
    0b0100001000111110100011111010010000000001,
    0b0101101011111110100011111010010000000001,
    0b0111001110111110100011111010010000000001,
    0b1000110001111110100011111010010000000001,
    0b1001110011111110100011111010010000000001,
    0b1011010110111110100011111010010000000001,
    0b1100111001111110100011111010010000000001,
    0b1110011100111110100011111010010000000001,
    0b1111111111111000010011111010010000000001,
    0b1111111111110011100111111010010000000001,
    0b1111111111101110111011111010010000000001,
    0b1111111111101011110011111010010000000001,
    0b1111111111101000101011111010010000000001,
    0b1111111111100111000111111010010000000001,
    0b1111111111100011111111111010010000000001,
    0b1111111111100010011011111010010000000001,
    0b1111111111100000110111111010010000000001,
    0b1111111111011111010011111010010000000001,
    0b1111111111011101101111111010010000000001,
    0b1111111111011100001011111010010000000001,
    0b1111111111011010100111111010010000000001,
    0b1111111111011001000011111010010000000001,
    0b1111111111011001000011111010010000000001,
    0b1111111111010111011111111010010000000001,
    0b1111111111010101111011111010010000000001,
    0b1111111111010101111011111010010000000001,
    0b1111111111010100010111111010010000000001,
    0b1111111111010100010111111010010000000001,
    0b1111111111010010110011111010010000000001,
    0b1111111111010010110011111010010000000001,
    0b1111111111010010110011111010010000000001,
    0b1111111111010001001111111010010000000001,
    0b1111111111010001001111111010010000000001,
    0b1111111111010001001111111010010000000001,
    0b1111111111001111101011111010010000000001,
    0b1111111111001111101011111010010000000001,
    0b1111111111001111101011111010010000000001,
    0b1111111111001111101011111010010000000001,
    0b1111111111001111101011111010010000000001,
    0b1111111111001111101011111010010000000001,
    0b1111111111001111101011111010010000000001,
    0b1111111111001111101011111010010000000001,
    0b1111111111001111101011111010010000000001,
    0b1111111111001111101011111010010000000001,
    0b1111111111001111101011111010010000000001,
    0b1111111111001111101011111010010000000001,
    0b1111111111001111101011111010010000000001,
    0b1111111111001111101011111010010000000001,
    0b1111111111001111101011111010010000000001,
    0b1111111111001111101011111010010000000001,
    0b1111111111001111101011111010010000000001,
    0b1111111111001111101011111010010000000001,
    0b1111111111001111101011111010010000000001,
    0b1111111111001111101011111010010000000001,
    0b1111111111001111101011111010010000000001,
    0b1111111111001111101011111010010000000001,
    0b1111111111001111101011111010010000000001,
    0b1111111111001111101011111010010000000001,
    0b1111111111001111101011111010010000000001,
    0b1111111111001111101011111010010000000001,
    0b1111111111001111101011111010010000000001,
    0b1111111111001111101011111010010000000001
};

static const kuint32_t filter_lookup_low[64] = 
{
    0b0001011111,
    0b0001010111,
    0b0001111011,
    0b0001011011,
    0b0001101011,
    0b0001110011,
    0b0001110011,
    0b0001110011,
    0b0001110011,
    0b0001001011,
    0b0001001011,
    0b0001001011,
    0b0010110011,
    0b0001010011,
    0b0001010011,
    0b0001010011,
    0b0001010011,
    0b0001010011,
    0b0001010011,
    0b0001010011,
    0b0001010011,
    0b0001010011,
    0b0001010011,
    0b0001100011,
    0b0001100011,
    0b0001100011,
    0b0001100011,
    0b0001100011,
    0b0001100011,
    0b0001100011,
    0b0001100011,
    0b0001100011,
    0b0001100011,
    0b0001100011,
    0b0001100011,
    0b0001100011,
    0b0001100011,
    0b0010010011,
    0b0010010011,
    0b0010010011,
    0b0010010011,
    0b0010010011,
    0b0010010011,
    0b0010010011,
    0b0010010011,
    0b0010010011,
    0b0010010011,
    0b0010100011,
    0b0010100011,
    0b0010100011,
    0b0010100011,
    0b0010100011,
    0b0010100011,
    0b0010100011,
    0b0010100011,
    0b0010100011,
    0b0010100011,
    0b0010100011,
    0b0010100011,
    0b0010100011,
    0b0010100011,
    0b0010100011,
    0b0010100011,
    0b0010100011
};

/*!< The functions */
/*!
 * TODO:This function currently requires that the reference clock is 100MHz.
 * 		This should be changed so that the ref. clock can be specified, or read directly
 * 		out of hardware. This has been done in the linux driver, it just needs to be
 * 		ported here.
 */
kfloat_t ClkFindParams(float freq, ClkMode *sprt_bestPick)
{
    kfloat_t bestError = 2000.0;
    kfloat_t curError;
    kfloat_t curClkMult;
    kfloat_t curFreq;
    kuint32_t curDiv, curFb, curClkDiv;
    kuint32_t minFb = 0;
    kuint32_t maxFb = 0;

    /*!
     * This is necessary because the MMCM actual is generating 5x the desired pixel clock, and that
     * clock is then run through a BUFR that divides it by 5 to generate the pixel clock. Note this
     * means the pixel clock is on the Regional clock network, not the global clock network. In the
     * future if options like these are parameterized in the axi_dynclk core, then this function will
     * need to change.
     */
    freq = freq * 5.0;
    sprt_bestPick->freq = 0.0;

    /*! TODO: replace with a smarter algorithm that doesn't doesn't check every possible combination */
    for (curDiv = 1; curDiv <= 10; curDiv++)
    {
        /*< This accounts for the 100MHz input and the 600MHz minimum VCO */
        minFb = curDiv * 6; 
        /*!< This accounts for the 100MHz input and the 1200MHz maximum VCO */
        maxFb = curDiv * 12; 
        if (maxFb > 64)
            maxFb = 64;

        /*!< This multiplier is used to find the best clkDiv value for each FB value */
        curClkMult = (100.0 / (kfloat_t) curDiv) / freq; 

        curFb = minFb;
        while (curFb <= maxFb)
        {
            curClkDiv = (kuint32_t) ((curClkMult * (kfloat_t)curFb) + 0.5);
            curFreq = ((100.0 / (kfloat_t) curDiv) / (kfloat_t) curClkDiv) * (kfloat_t) curFb;
            curError = fabs(curFreq - freq);
            if (curError < bestError)
            {
                bestError = curError;
                sprt_bestPick->clkdiv = curClkDiv;
                sprt_bestPick->fbmult = curFb;
                sprt_bestPick->maindiv = curDiv;
                sprt_bestPick->freq = curFreq;
            }

            curFb++;
        }
    }

    /*!
     * We want the ClkMode struct and errors to be based on the desired frequency. 
     * Need to check this doesn't introduce rounding errors.
     */
    sprt_bestPick->freq = sprt_bestPick->freq / 5.0;
    bestError = bestError / 5.0;

    return bestError;
}

kuint32_t ClkDivider(kuint32_t divide)
{
    kuint32_t output = 0;
    kuint32_t highTime = 0;
    kuint32_t lowTime = 0;

    if ((divide < 1) || (divide > 128))
        return ERR_CLKDIVIDER;

    if (divide == 1)
        return 0x1041;

    highTime = divide / 2;

    /*!< if divide is odd */
    if (divide & 0b1)
    {
        lowTime = highTime + 1;
        output = 1 << CLK_BIT_WEDGE;
    }
    else
    {
        lowTime = highTime;
    }

    output |= 0x03F & lowTime;
    output |= 0xFC0 & (highTime << 6);

    return output;
}

kuint32_t ClkCountCalc(kuint32_t divide)
{
    kuint32_t output = 0;
    kuint32_t divCalc = 0;

    divCalc = ClkDivider(divide);
    if (divCalc == ERR_CLKDIVIDER)
        output = ERR_CLKCOUNTCALC;
    else
        output = (0xFFF & divCalc) | ((divCalc << 10) & 0x00C00000);

    return output;
}

void ClkWriteReg(ClkConfig *sprt_regVal, kuint32_t dynClkAddr)
{
    mrt_writel(sprt_regVal->clk0L, dynClkAddr + OFST_DYNCLK_CLK_L);
    mrt_writel(sprt_regVal->clkFBL, dynClkAddr + OFST_DYNCLK_FB_L);
    mrt_writel(sprt_regVal->clkFBH_clk0H, dynClkAddr + OFST_DYNCLK_FB_H_CLK_H);
    mrt_writel(sprt_regVal->divclk, dynClkAddr + OFST_DYNCLK_DIV);
    mrt_writel(sprt_regVal->lockL, dynClkAddr + OFST_DYNCLK_LOCK_L);
    mrt_writel(sprt_regVal->fltr_lockH, dynClkAddr + OFST_DYNCLK_FLTR_LOCK_H);
}

kuint32_t ClkFindReg(ClkConfig *sprt_regVal, ClkMode *sprt_params)
{
    if ((sprt_params->fbmult < 2) || sprt_params->fbmult > 64 )
        return 0;

    sprt_regVal->clk0L = ClkCountCalc(sprt_params->clkdiv);
    if (sprt_regVal->clk0L == ERR_CLKCOUNTCALC)
        return 0;

    sprt_regVal->clkFBL = ClkCountCalc(sprt_params->fbmult);
    if (sprt_regVal->clkFBL == ERR_CLKCOUNTCALC)
        return 0;

    sprt_regVal->clkFBH_clk0H = 0;

    sprt_regVal->divclk = ClkDivider(sprt_params->maindiv);
    if (sprt_regVal->divclk == ERR_CLKDIVIDER)
        return 0;

    sprt_regVal->lockL = (kuint32_t)(lock_lookup[sprt_params->fbmult - 1] & 0xFFFFFFFF);

    sprt_regVal->fltr_lockH = (kuint32_t)((lock_lookup[sprt_params->fbmult - 1] >> 32) & 0x000000FF);
    sprt_regVal->fltr_lockH |= ((filter_lookup_low[sprt_params->fbmult - 1] << 16) & 0x03FF0000);

    return 1;
}

void ClkStart(kuint32_t dynClkAddr)
{
    mrt_writel((1 << BIT_DYNCLK_START), dynClkAddr + OFST_DYNCLK_CTRL);
    while (!(mrt_readl(dynClkAddr + OFST_DYNCLK_STATUS) & (1 << BIT_DYNCLK_RUNNING)));
}

void ClkStop(kuint32_t dynClkAddr)
{
    mrt_writel(0, dynClkAddr + OFST_DYNCLK_CTRL);
    while ((mrt_readl(dynClkAddr + OFST_DYNCLK_STATUS) & (1 << BIT_DYNCLK_RUNNING)));
}

/*!
 * Read one word from BD
 *
 * @param sprt_bd is the BD to work on
 * @param Offset is the byte offset to read from
 *
 * @return
 *  The word value
 *
 */
static kuint32_t XAxiVdma_BdRead(XAxiVdma_Bd *sprt_bd, kint32_t Offset)
{
    return mrt_readl((kuint32_t)sprt_bd + Offset);
}

/*!
 * Set one word in BD
 *
 * @param sprt_bd is the BD to work on
 * @param Offset is the byte offset to write to
 * @param Value is the value to write to the BD
 *
 * @return
 *  None
 */
static void XAxiVdma_BdWrite(XAxiVdma_Bd *sprt_bd, kint32_t Offset, kuint32_t Value)
{
    mrt_writel(Value, (kuint32_t)sprt_bd + Offset);
}

/*!
 * Set the next ptr from BD
 *
 * @param sprt_bd is the BD to work on
 * @param NextPtr is the next ptr to set in BD
 *
 * @return
 *  None
 */
static void XAxiVdma_BdSetNextPtr(XAxiVdma_Bd *sprt_bd, kuint32_t NextPtr)
{
    XAxiVdma_BdWrite(sprt_bd, XAXIVDMA_BD_NDESC_OFFSET, NextPtr);
}

/*!
 * Set the vertical size for a BD
 *
 * @param sprt_bd is the BD to work on
 * @param Vsize is the vertical size to set in BD
 *
 * @return
 *  - ER_NORMAL if successful
 *  - ER_UNVALID if argument Vsize is invalid
 */
static kint32_t XAxiVdma_BdSetVsize(XAxiVdma_Bd *sprt_bd, kint32_t Vsize)
{
    if ((Vsize <= 0) || (Vsize > XAXIVDMA_VSIZE_MASK))
        return -ER_UNVALID;

    XAxiVdma_BdWrite(sprt_bd, XAXIVDMA_BD_VSIZE_OFFSET, Vsize);
    return ER_NORMAL;
}

/*!
 * Set the horizontal size for a BD
 *
 * @param sprt_bd is the BD to work on
 * @param Hsize is the horizontal size to set in BD
 *
 * @return
 *  - ER_NORMAL if successful
 *  - ER_UNVALID if argument Hsize is invalid
 */
static kint32_t XAxiVdma_BdSetHsize(XAxiVdma_Bd *sprt_bd, kint32_t Hsize)
{
    if ((Hsize <= 0) || (Hsize > XAXIVDMA_HSIZE_MASK))
        return -ER_UNVALID;

    XAxiVdma_BdWrite(sprt_bd, XAXIVDMA_BD_HSIZE_OFFSET, Hsize);
    return ER_NORMAL;
}

/*!
 * Set the stride size for a BD
 *
 * @param BdPtr is the BD to work on
 * @param Stride is the stride size to set in BD
 *
 * @return
 *  - ER_NORMAL if successful
 *  - ER_UNVALID if argument Stride is invalid
 */
static kint32_t XAxiVdma_BdSetStride(XAxiVdma_Bd *sprt_bd, kint32_t Stride)
{
    kuint32_t Bits;

    if ((Stride <= 0) || (Stride > XAXIVDMA_STRIDE_MASK))
        return -ER_UNVALID;

    Bits = XAxiVdma_BdRead(sprt_bd, XAXIVDMA_BD_STRIDE_OFFSET) & (~XAXIVDMA_STRIDE_MASK);
    XAxiVdma_BdWrite(sprt_bd, XAXIVDMA_BD_STRIDE_OFFSET, Bits | Stride);

    return ER_NORMAL;
}

/*!
 * Set the frame delay for a BD
 *
 * @param sprt_bd is the BD to work on
 * @param FrmDly is the frame delay value to set in BD
 *
 * @return
 *  - ER_NORMAL if successful
 *  - ER_UNVALID if argument FrmDly is invalid
 */
static kint32_t XAxiVdma_BdSetFrmDly(XAxiVdma_Bd *sprt_bd, kint32_t FrmDly)
{
    kuint32_t Bits;

    if ((FrmDly < 0) || (FrmDly > XAXIVDMA_FRMDLY_MAX))
        return -ER_UNVALID;

    Bits = XAxiVdma_BdRead(sprt_bd, XAXIVDMA_BD_STRIDE_OFFSET) & (~XAXIVDMA_FRMDLY_MASK);
    XAxiVdma_BdWrite(sprt_bd, XAXIVDMA_BD_STRIDE_OFFSET, Bits | (FrmDly << XAXIVDMA_FRMDLY_SHIFT));

    return ER_NORMAL;
}

/*!
 * Set the start address from BD
 *
 * The address is physical address.
 *
 * @param sprt_bd is the BD to work on
 * @param Addr is the address to set in BD
 *
 * @return
 *  None
 */
static void XAxiVdma_BdSetAddr(XAxiVdma_Bd *sprt_bd, kuint32_t Addr)
{
    XAxiVdma_BdWrite(sprt_bd, XAXIVDMA_BD_START_ADDR_OFFSET, Addr);
}

/*!
 * @brief   get vdma's major
 * @param   sprt_vdma
 * @retval  XAxiVdma
 * @note    none
 */
static kuint32_t XAxiVdma_Major(XAxiVdma *sprt_vdma) 
{
    kuint32_t Reg;

    Reg = XAxiVdma_ReadReg(sprt_vdma->BaseAddr, XAXIVDMA_VERSION_OFFSET);
    return ((Reg & XAXIVDMA_VERSION_MAJOR_MASK) >> XAXIVDMA_VERSION_MAJOR_SHIFT);
}

/*!
 * Get a channel
 *
 * @param sprt_vdma is the DMA engine to work on
 * @param Direction is the direction for the channel to get
 *
 * @return
 * The pointer to the channel. Upon error, return NULL.
 *
 * @note
 * Since this function is internally used, we assume Direction is valid
 */
XAxiVdma_Channel *XAxiVdma_GetChannel(XAxiVdma *sprt_vdma, kuint32_t Direction)
{
    if (Direction == XAXIVDMA_READ)
        return &(sprt_vdma->ReadChannel);

    else if (Direction == XAXIVDMA_WRITE)
        return &(sprt_vdma->WriteChannel);

    else 
        return mrt_nullptr;
}

/*!
 * Initialize a channel of a DMA engine
 *
 * This function initializes the BD ring for this channel
 *
 * @param sprt_chan is the pointer to the DMA channel to work on
 *
 * @return
 *   None
 */
void XAxiVdma_ChannelInit(XAxiVdma_Channel *sprt_chan)
{
    kint32_t i;
    kint32_t NumFrames;
    XAxiVdma_Bd *FirstBdPtr = &(sprt_chan->BDs[0]);
    XAxiVdma_Bd *LastBdPtr;

    /*!< Initialize the BD variables, so proper memory management can be done */
    NumFrames = sprt_chan->NumFrames;

    sprt_chan->IsValid = 0;
    sprt_chan->HeadBdPhysAddr = 0;
    sprt_chan->HeadBdAddr = 0;
    sprt_chan->TailBdPhysAddr = 0;
    sprt_chan->TailBdAddr = 0;

    LastBdPtr = &(sprt_chan->BDs[NumFrames - 1]);

    /*!< Setup the BD ring */
    memset((void *)FirstBdPtr, 0, NumFrames * sizeof(XAxiVdma_Bd));

    for (i = 0; i < NumFrames; i++) 
    {
        XAxiVdma_Bd *BdPtr;
        XAxiVdma_Bd *NextBdPtr;

        BdPtr = &(sprt_chan->BDs[i]);

        /*!< The last BD connects to the first BD */
        if (i == (NumFrames - 1))
            NextBdPtr = FirstBdPtr;
        else
            NextBdPtr = &(sprt_chan->BDs[i + 1]);

        XAxiVdma_BdSetNextPtr(BdPtr, XAXIVDMA_VIRT_TO_PHYS((kuint32_t)NextBdPtr));
    }

    sprt_chan->AllCnt = NumFrames;

    /*!< Setup the BD addresses so that access the head/tail BDs fast */
    sprt_chan->HeadBdAddr = (kuint32_t)FirstBdPtr;
    sprt_chan->HeadBdPhysAddr = XAXIVDMA_VIRT_TO_PHYS((kuint32_t)FirstBdPtr);

    sprt_chan->TailBdAddr = (kuint32_t)LastBdPtr;
    sprt_chan->TailBdPhysAddr = XAXIVDMA_VIRT_TO_PHYS((kuint32_t)LastBdPtr);

    sprt_chan->IsValid = true;
}

/*!
 * This function resets one DMA channel
 *
 * The registers will be default values after the reset
 *
 * @param sprt_chan is the pointer to the DMA channel to work on
 *
 * @return
 *  None
 */
void XAxiVdma_ChannelReset(XAxiVdma_Channel *sprt_chan)
{
    XAxiVdma_WriteReg(sprt_chan->ChanBase, XAXIVDMA_CR_OFFSET, XAXIVDMA_CR_RESET_MASK);
}

/*!
 * This function checks whether reset operation is done
 *
 * @param sprt_chan is the pointer to the DMA channel to work on
 *
 * @return
 * - 0 if reset is done
 * - 1 if reset is still going
 */
kuint32_t XAxiVdma_ChannelResetNotDone(XAxiVdma_Channel *sprt_chan)
{
    return (XAxiVdma_ReadReg(sprt_chan->ChanBase, XAXIVDMA_CR_OFFSET) & XAXIVDMA_CR_RESET_MASK);
}

/*!
 * Check whether a DMA channel is busy
 *
 * @param sprt_chan is the pointer to the channel to work on
 *
 * @return
 * - non zero if the channel is busy
 * - 0 is the channel is idle
 */
kbool_t XAxiVdma_ChannelIsBusy(XAxiVdma_Channel *sprt_chan)
{
    kuint32_t Bits;

    /*!< If the channel is idle, then it is not busy */
    Bits = XAxiVdma_ReadReg(sprt_chan->ChanBase, XAXIVDMA_SR_OFFSET) & XAXIVDMA_SR_IDLE_MASK;
    if (Bits)
        return false;

    /*!< If the channel is halted, then it is not busy */
    Bits = XAxiVdma_ReadReg(sprt_chan->ChanBase, XAXIVDMA_SR_OFFSET) & XAXIVDMA_SR_HALTED_MASK;
    if (Bits)
        return false;

    /*!< Otherwise, it is busy */
    return true;
}

/*!
 * Check whether a DMA channel is running
 *
 * @param Channel is the pointer to the channel to work on
 *
 * @return
 * - non zero if the channel is running
 * - 0 is the channel is idle
 */
kbool_t XAxiVdma_ChannelIsRunning(XAxiVdma_Channel *sprt_chan)
{
    kuint32_t Bits;

    /*!< If halted bit set, channel is not running */
    Bits = XAxiVdma_ReadReg(sprt_chan->ChanBase, XAXIVDMA_SR_OFFSET) & XAXIVDMA_SR_HALTED_MASK;
    if (Bits)
        return false;

    /*!< If Run/Stop bit low, then channel is not running */
    Bits = XAxiVdma_ReadReg(sprt_chan->ChanBase, XAXIVDMA_CR_OFFSET) & XAXIVDMA_CR_RUNSTOP_MASK;
    if (!Bits)
        return false;

    return true;
}

/*!
 * Set the channel to disable access higher Frame Buffer Addresses (SG=0)
 *
 * @param sprt_chan is the pointer to the channel to work on
 */
void XAxiVdma_ChannelHiFrmAddrDisable(XAxiVdma_Channel *sprt_chan)
{
    XAxiVdma_WriteReg(sprt_chan->ChanBase,
                    XAXIVDMA_HI_FRMBUF_OFFSET, (XAXIVDMA_REGINDEX_MASK >> 1));
}

/*!
 * Set the channel to enable access to higher Frame Buffer Addresses (SG=0)
 *
 * @param sprt_chan is the pointer to the channel to work on
 */
void XAxiVdma_ChannelHiFrmAddrEnable(XAxiVdma_Channel *sprt_chan)
{
    XAxiVdma_WriteReg(sprt_chan->ChanBase,
                    XAXIVDMA_HI_FRMBUF_OFFSET, XAXIVDMA_REGINDEX_MASK);
}

/*!
 * Configure buffer addresses for one DMA channel
 *
 * The buffer addresses are physical addresses.
 * Access to 32 Frame Buffer Addresses in direct mode is done through
 * XAxiVdma_ChannelHiFrmAddrEnable/Disable Functions.
 * 0 - Access Bank0 Registers (0x5C - 0x98)
 * 1 - Access Bank1 Registers (0x5C - 0x98)
 *
 * @param sprt_chan is the pointer to the channel to work on
 * @param BufferAddrSet is the set of addresses for the transfers
 * @param NumFrames is the number of frames to set the address
 *
 * @return
 * - ER_NORMAL if successful
 * - ER_FAILD if channel has not being initialized
 * - ER_UNVALID if buffer address not valid, for example, unaligned
 * address with no DRE built in the hardware
 */
kint32_t XAxiVdma_ChannelSetBufferAddr(XAxiVdma_Channel *sprt_chan,
                                    kuint32_t *BufferAddrSet, kint32_t NumFrames)
{
    kint32_t i;
    kuint32_t WordLenBits;
    kint32_t HiFrmAddr = 0;
    kint32_t FrmBound;
    kint32_t Loop16 = 0;

    if (sprt_chan->AddrWidth > 32)
        FrmBound = (XAXIVDMA_MAX_FRAMESTORE_64) / 2 - 1;
    else
        FrmBound = (XAXIVDMA_MAX_FRAMESTORE) / 2 - 1;

    if (!sprt_chan->IsValid)
        return -ER_FAILD;

    WordLenBits = (kuint32_t)(sprt_chan->WordLength - 1);

    /*!< If hardware has no DRE, then buffer addresses must be word-aligned */
    for (i = 0; i < NumFrames; i++) 
    {
        if (!sprt_chan->HasDRE) 
        {
            if (BufferAddrSet[i] & WordLenBits)
                return -ER_UNVALID;
        }
    }

    for (i = 0; i < NumFrames; i++, Loop16++) 
    {
        XAxiVdma_Bd *BdPtr = (XAxiVdma_Bd *)(sprt_chan->HeadBdAddr + i * sizeof(XAxiVdma_Bd));

        if (sprt_chan->HasSG)
            XAxiVdma_BdSetAddr(BdPtr, BufferAddrSet[i]);
        else 
        {
            if ((i > FrmBound) && !HiFrmAddr) 
            {
                XAxiVdma_ChannelHiFrmAddrEnable(sprt_chan);
                HiFrmAddr = 1;
                Loop16 = 0;
            }

            if (sprt_chan->AddrWidth > 32) 
            {
                /*!< For a 40-bit address XAXIVDMA_MAX_FRAMESTORE value should be set to 16 */
                XAxiVdma_WriteReg(sprt_chan->StartAddrBase, 
                            XAXIVDMA_START_ADDR_OFFSET + Loop16 * XAXIVDMA_START_ADDR_LEN + i * 4,
                            (kuint32_t)(BufferAddrSet[i]));

                XAxiVdma_WriteReg(sprt_chan->StartAddrBase,
                            XAXIVDMA_START_ADDR_MSB_OFFSET + Loop16 * XAXIVDMA_START_ADDR_LEN + i * 4,
                            ((kuint64_t)(BufferAddrSet[i]) >> 32));
            } 
            else 
            {
                XAxiVdma_WriteReg(sprt_chan->StartAddrBase,
                            XAXIVDMA_START_ADDR_OFFSET + Loop16 * XAXIVDMA_START_ADDR_LEN,
                            BufferAddrSet[i]);
            }

            if ((NumFrames > FrmBound) && (i == (NumFrames - 1)))
                XAxiVdma_ChannelHiFrmAddrDisable(sprt_chan);
        }
    }

    return ER_NORMAL;
}

/*!
 * Check DMA channel errors
 *
 * @param sprt_chan is the pointer to the channel to work on
 *
 * @return
 *  	Error bits of the channel, 0 means no errors
 */
kuint32_t XAxiVdma_ChannelErrors(XAxiVdma_Channel *sprt_chan)
{
    return (XAxiVdma_ReadReg(sprt_chan->ChanBase, XAXIVDMA_SR_OFFSET) & XAXIVDMA_SR_ERR_ALL_MASK);
}

/*!
 * Clear DMA channel errors
 *
 * @param   sprt_chan is the pointer to the channel to work on
 * @param   ErrorMask is the mask of error bits to clear.
 *
 * @return  None
 */
void XAxiVdma_ClearChannelErrors(XAxiVdma_Channel *sprt_chan, kuint32_t ErrorMask)
{
    kuint32_t SrBits;

    /*!< Write on Clear bits */
    SrBits = XAxiVdma_ReadReg(sprt_chan->ChanBase, XAXIVDMA_SR_OFFSET) | ErrorMask;
    XAxiVdma_WriteReg(sprt_chan->ChanBase, XAXIVDMA_SR_OFFSET, SrBits);
}

/*!
 * Configure one DMA channel using the configuration structure
 *
 * Setup the control register and BDs, however, BD addresses are not set.
 *
 * @param sprt_chan is the pointer to the channel to work on
 * @param sprt_setup is the pointer to the setup structure
 *
 * @return
 * - ER_NORMAL if successful
 * - ER_FAULT if channel has not being initialized
 * - ER_BUSY if the DMA channel is not idle
 * - ER_UNVALID if fields in ChannelCfgPtr is not valid
 */
kint32_t XAxiVdma_ChannelConfig(XAxiVdma_Channel *sprt_chan, XAxiVdma_ChannelSetup *sprt_setup)
{
    kuint32_t CrBits;
    kint32_t i;
    kint32_t NumBds;
    kint32_t Status;
    kuint32_t hsize_align;
    kuint32_t stride_align;

    if (!sprt_chan->IsValid)
        return -ER_FAULT;

    if (sprt_chan->HasSG && XAxiVdma_ChannelIsBusy(sprt_chan))
        return -ER_BUSY;

    sprt_chan->Vsize = sprt_setup->VertSizeInput;

    /*!< Check whether Hsize is properly aligned */
    if (sprt_chan->direction == XAXIVDMA_WRITE) 
    {
        if (sprt_setup->HoriSizeInput < sprt_chan->WordLength)
            hsize_align = (kuint32_t)sprt_chan->WordLength;
        else 
        {
            hsize_align = (kuint32_t)(sprt_setup->HoriSizeInput % sprt_chan->WordLength);
            if (hsize_align > 0)
                hsize_align = (sprt_chan->WordLength - hsize_align);
        }
    }
    else 
    {
        if (sprt_setup->HoriSizeInput < sprt_chan->WordLength)
            hsize_align = (kuint32_t)sprt_chan->WordLength;
        else 
        {
            hsize_align = (kuint32_t)(sprt_setup->HoriSizeInput % sprt_chan->StreamWidth);
            if (hsize_align > 0)
                hsize_align = (sprt_chan->StreamWidth - hsize_align);
        }
    }

    /*!< Check whether Stride is properly aligned */
    if (sprt_setup->Stride < sprt_chan->WordLength)
        stride_align = (kuint32_t)sprt_chan->WordLength;
    else 
    {
        stride_align = (kuint32_t)(sprt_setup->Stride % sprt_chan->WordLength);
        if (stride_align > 0)
            stride_align = (sprt_chan->WordLength - stride_align);
    }

    /*!< If hardware has no DRE, then Hsize and Stride must be word-aligned */
    if (!sprt_chan->HasDRE) 
    {
        if (hsize_align != 0) 
        {
            /*!< Adjust hsize to multiples of stream/mm data width */
            sprt_setup->HoriSizeInput += hsize_align;
        }
        if (stride_align != 0) 
        {
            /*!< Adjust stride to multiples of stream/mm data width */
            sprt_setup->Stride += stride_align;
        }
    }

    sprt_chan->Hsize = sprt_setup->HoriSizeInput;

    CrBits = XAxiVdma_ReadReg(sprt_chan->ChanBase, XAXIVDMA_CR_OFFSET);
    CrBits = XAxiVdma_ReadReg(sprt_chan->ChanBase, XAXIVDMA_CR_OFFSET) &
                ~(XAXIVDMA_CR_TAIL_EN_MASK | XAXIVDMA_CR_SYNC_EN_MASK |
                XAXIVDMA_CR_FRMCNT_EN_MASK | XAXIVDMA_CR_RD_PTR_MASK);

    if (sprt_setup->EnableCircularBuf)
        CrBits |= XAXIVDMA_CR_TAIL_EN_MASK;
    else 
    {
        /*!< Park mode */
        kuint32_t FrmBits;
        kuint32_t RegValue;

        if ((!XAxiVdma_ChannelIsRunning(sprt_chan)) && sprt_chan->HasSG)
            return -ER_UNVALID;

        if (sprt_setup->FixedFrameStoreAddr > XAXIVDMA_FRM_MAX)
            return -ER_UNVALID;

        if (sprt_chan->IsRead) 
        {
            FrmBits = sprt_setup->FixedFrameStoreAddr & XAXIVDMA_PARKPTR_READREF_MASK;

            RegValue  = XAxiVdma_ReadReg(sprt_chan->InstanceBase, XAXIVDMA_PARKPTR_OFFSET);
            RegValue &= ~XAXIVDMA_PARKPTR_READREF_MASK;
            RegValue |= FrmBits;

            XAxiVdma_WriteReg(sprt_chan->InstanceBase, XAXIVDMA_PARKPTR_OFFSET, RegValue);
        }
        else 
        {
            FrmBits = sprt_setup->FixedFrameStoreAddr << XAXIVDMA_WRTREF_SHIFT;

            FrmBits  &= XAXIVDMA_PARKPTR_WRTREF_MASK;
            RegValue  = XAxiVdma_ReadReg(sprt_chan->InstanceBase, XAXIVDMA_PARKPTR_OFFSET);
            RegValue &= ~XAXIVDMA_PARKPTR_WRTREF_MASK;
            RegValue |= FrmBits;

            XAxiVdma_WriteReg(sprt_chan->InstanceBase, XAXIVDMA_PARKPTR_OFFSET, RegValue);
        }
    }

    if (sprt_setup->EnableSync) 
    {
        if (sprt_chan->GenLock != XAXIVDMA_GENLOCK_MASTER)
            CrBits |= XAXIVDMA_CR_SYNC_EN_MASK;
    }

    if (sprt_setup->GenLockRepeat) 
    {
        if ((sprt_chan->GenLock == XAXIVDMA_GENLOCK_MASTER) ||
            (sprt_chan->GenLock == XAXIVDMA_DYN_GENLOCK_MASTER))
            CrBits |= XAXIVDMA_CR_GENLCK_RPT_MASK;
    }

    if (sprt_setup->EnableFrameCounter)
        CrBits |= XAXIVDMA_CR_FRMCNT_EN_MASK;

    CrBits |= (sprt_setup->PointNum << XAXIVDMA_CR_RD_PTR_SHIFT) & XAXIVDMA_CR_RD_PTR_MASK;

    /*!< Write the control register value out */
    XAxiVdma_WriteReg(sprt_chan->ChanBase, XAXIVDMA_CR_OFFSET, CrBits);

    if (sprt_chan->HasVFlip && !sprt_chan->IsRead) 
    {
        kuint32_t RegValue;

        RegValue  = XAxiVdma_ReadReg(sprt_chan->InstanceBase, XAXIVDMA_VFLIP_OFFSET);
        RegValue &= ~XAXIVDMA_VFLIP_EN_MASK;
        RegValue |= (sprt_setup->EnableVFlip & XAXIVDMA_VFLIP_EN_MASK);

        XAxiVdma_WriteReg(sprt_chan->InstanceBase, XAXIVDMA_VFLIP_OFFSET, RegValue);
    }

    if (sprt_chan->HasSG) 
    {
        /*!<
         * Setup the information in BDs
         *
         * All information is available except the buffer addrs
         * Buffer addrs are set through XAxiVdma_ChannelSetBufferAddr()
         */
        NumBds = sprt_chan->AllCnt;

        for (i = 0; i < NumBds; i++) 
        {
            XAxiVdma_Bd *sprt_bd = (XAxiVdma_Bd *)(sprt_chan->HeadBdAddr + i * sizeof(XAxiVdma_Bd));

            Status = XAxiVdma_BdSetVsize(sprt_bd, sprt_setup->VertSizeInput);
            if (Status)
                return Status;

            Status = XAxiVdma_BdSetHsize(sprt_bd, sprt_setup->HoriSizeInput);
            if (Status)
                return Status;

            Status = XAxiVdma_BdSetStride(sprt_bd, sprt_setup->Stride);
            if (Status)
                return Status;

            Status = XAxiVdma_BdSetFrmDly(sprt_bd, sprt_setup->FrameDelay);
            if (Status)
                return Status;
        }
    }
    else 
    {   /*!< direct register mode */
        if ((sprt_setup->VertSizeInput > XAXIVDMA_MAX_VSIZE) ||
            (sprt_setup->VertSizeInput <= 0) ||
            (sprt_setup->HoriSizeInput > XAXIVDMA_MAX_HSIZE) ||
            (sprt_setup->HoriSizeInput <= 0) ||
            (sprt_setup->Stride > XAXIVDMA_MAX_STRIDE) ||
            (sprt_setup->Stride <= 0) ||
            (sprt_setup->FrameDelay < 0) ||
            (sprt_setup->FrameDelay > XAXIVDMA_FRMDLY_MAX))
            return -ER_UNVALID;

        XAxiVdma_WriteReg(sprt_chan->StartAddrBase,
                        XAXIVDMA_HSIZE_OFFSET, sprt_setup->HoriSizeInput);

        XAxiVdma_WriteReg(sprt_chan->StartAddrBase, XAXIVDMA_STRD_FRMDLY_OFFSET,
                        (sprt_setup->FrameDelay << XAXIVDMA_FRMDLY_SHIFT) | sprt_setup->Stride);
    }

    return ER_NORMAL;
}

/*!
 * Start one DMA channel
 *
 * @param Channel is the pointer to the channel to work on
 *
 * @return
 * - ER_NORMAL if successful
 * - ER_FAILD if channel is not initialized
 * - ER_FAULT if:
 *   . The DMA channel fails to stop
 *   . The DMA channel fails to start
 * - ER_BUSY is the channel is doing transfers
 */
kint32_t XAxiVdma_ChannelStart(XAxiVdma_Channel *sprt_chan)
{
    kuint32_t CrBits;

    if (!sprt_chan->IsValid)
        return -ER_FAILD;

    if (sprt_chan->HasSG && XAxiVdma_ChannelIsBusy(sprt_chan))
        return -ER_BUSY;

    /*!< If sprt_chan is not running, setup the CDESC register and set the sprt_chan to run */
    if (!XAxiVdma_ChannelIsRunning(sprt_chan)) 
    {
        if (sprt_chan->HasSG) 
        {
            /*!<
             * Set up the current bd register
             *
             * Can only setup current bd register when sprt_chan is halted
             */
            XAxiVdma_WriteReg(sprt_chan->ChanBase, XAXIVDMA_CDESC_OFFSET,
                            sprt_chan->HeadBdPhysAddr);
        }

        /*!< Start DMA hardware */
        CrBits = XAxiVdma_ReadReg(sprt_chan->ChanBase, XAXIVDMA_CR_OFFSET);
        CrBits = XAxiVdma_ReadReg(sprt_chan->ChanBase,
                                XAXIVDMA_CR_OFFSET) | XAXIVDMA_CR_RUNSTOP_MASK;
        XAxiVdma_WriteReg(sprt_chan->ChanBase, XAXIVDMA_CR_OFFSET, CrBits);

    }

    if (XAxiVdma_ChannelIsRunning(sprt_chan)) 
    {
        /*!< Start DMA transfers */
        if (sprt_chan->HasSG) 
        {
            /*!<
             * SG mode:
             * Update the tail pointer so that hardware will start
             * fetching BDs
             */
            XAxiVdma_WriteReg(sprt_chan->ChanBase, XAXIVDMA_TDESC_OFFSET,
                                sprt_chan->TailBdPhysAddr);
        }
        else 
        {
            /*!<
             * Direct register mode:
             * Update vsize to start the sprt_chan
             */
            XAxiVdma_WriteReg(sprt_chan->StartAddrBase,
                                XAXIVDMA_VSIZE_OFFSET, sprt_chan->Vsize);
        }

        return ER_NORMAL;
    }

    return -ER_FAULT;
}

/*!
 * Stop one DMA channel
 *
 * @param   sprt_chan is the pointer to the channel to work on
 *
 * @return  None
 */
void XAxiVdma_ChannelStop(XAxiVdma_Channel *sprt_chan)
{
    kuint32_t CrBits;

    if (!XAxiVdma_ChannelIsRunning(sprt_chan))
        return;

    /*!< Clear the RS bit in CR register */
    CrBits = XAxiVdma_ReadReg(sprt_chan->ChanBase, XAXIVDMA_CR_OFFSET) & (~XAXIVDMA_CR_RUNSTOP_MASK);
    XAxiVdma_WriteReg(sprt_chan->ChanBase, XAXIVDMA_CR_OFFSET, CrBits);
}

/*!
 * Set the channel to run in parking mode
 *
 * @param Channel is the pointer to the channel to work on
 *
 * @return
 *   - ER_NORMAL if everything is fine
 *   - ER_FAULT if hardware is not running
 *
 */
kint32_t XAxiVdma_ChannelStartParking(XAxiVdma_Channel *sprt_chan)
{
    kuint32_t CrBits;

    if (!XAxiVdma_ChannelIsRunning(sprt_chan))
        return -ER_FAULT;

    CrBits = XAxiVdma_ReadReg(sprt_chan->ChanBase, XAXIVDMA_CR_OFFSET) & (~XAXIVDMA_CR_TAIL_EN_MASK);
    XAxiVdma_WriteReg(sprt_chan->ChanBase, XAXIVDMA_CR_OFFSET, CrBits);

    return ER_NORMAL;
}

/*!
 * Check for DMA Channel Errors.
 *
 * @param 	sprt_vdma is the XAxiVdma instance to operate on
 * @param	Direction is the channel to work on, use XAXIVDMA_READ/WRITE
 *
 * @return	- Errors seen on the channel
 *		- ER_UNVALID, when channel pointer is invalid.
 *		- ER_NOTFOUND, when the channel is not valid.
 *
 * @note	None
 */
kint32_t XAxiVdma_GetDmaChannelErrors(XAxiVdma *sprt_vdma, kuint16_t Direction)
{
    XAxiVdma_Channel *sprt_chan;

    sprt_chan = XAxiVdma_GetChannel(sprt_vdma, Direction);
    if (!sprt_chan)
        return -ER_UNVALID;

    if (sprt_chan->IsValid)
        return XAxiVdma_ChannelErrors(sprt_chan);

    return -ER_NOTFOUND;
}

/*!
 * Clear DMA Channel Errors.
 *
 * @param 	sprt_vdma is the XAxiVdma instance to operate on
 * @param	Direction is the channel to work on, use XAXIVDMA_READ/WRITE
 * @param	ErrorMask is the mask of error bits to clear
 *
 * @return	- ER_NORMAL, when error bits are cleared.
 *		- ER_NOTFOUND, when channel pointer is invalid.
 *		- ER_UNVALID, when the channel is not valid.
 *
 * @note	None
 */
kint32_t XAxiVdma_ClearDmaChannelErrors(XAxiVdma *sprt_vdma, kuint16_t Direction, kuint32_t ErrorMask)
{
    XAxiVdma_Channel *sprt_chan;

    sprt_chan = XAxiVdma_GetChannel(sprt_vdma, Direction);
    if (!sprt_chan)
        return -ER_UNVALID;

    if (sprt_chan->IsValid) 
    {
        XAxiVdma_ClearChannelErrors(sprt_chan, ErrorMask);
        return ER_NORMAL;
    }

    return -ER_NOTFOUND;
}

/*!
 * Configure one DMA channel using the configuration structure
 *
 * @param sprt_vdma is the pointer to the DMA engine to work on
 * @param Direction is the DMA channel to work on
 * @param DmaConfigPtr is the pointer to the setup structure
 *
 * @return
 * - ER_NORMAL if successful
 * - ER_BUSY if the DMA channel is not idle, BDs are still being used
 * - ER_UNVALID if buffer address not valid, for example, unaligned
 *   address with no DRE built in the hardware, or Direction invalid
 * - ER_NOTFOUND if the channel is invalid
 */
kint32_t XAxiVdma_DmaConfig(XAxiVdma *sprt_vdma, kuint16_t Direction, XAxiVdma_DmaSetup *sprt_setup)
{
    XAxiVdma_Channel *sprt_chan;

    sprt_chan = XAxiVdma_GetChannel(sprt_vdma, Direction);
    if (!sprt_chan)
        goto fail;

    if (sprt_chan->IsValid)
        return XAxiVdma_ChannelConfig(sprt_chan, (XAxiVdma_ChannelSetup *)sprt_setup);

fail:
    return -ER_NOTFOUND;
}

/*!
 * Configure buffer addresses for one DMA channel
 *
 * @param sprt_vdma is the pointer to the DMA engine to work on
 * @param Direction is the DMA channel to work on
 * @param BufferAddrSet is the set of addresses for the transfers
 *
 * @return
 * - ER_NORMAL if successful
 * - ER_BUSY if the DMA channel is not idle, BDs are still being used
 * - ER_UNVALID if buffer address not valid, for example, unaligned
 *   address with no DRE built in the hardware, or Direction invalid
 * - ER_NOTFOUND if the channel is invalid
 */
kint32_t XAxiVdma_DmaSetBufferAddr(XAxiVdma *sprt_vdma, kuint16_t Direction, kuint32_t *BufferAddrSet)
{
    XAxiVdma_Channel *sprt_chan;

    sprt_chan = XAxiVdma_GetChannel(sprt_vdma, Direction);
    if (!sprt_chan)
        goto fail;

    if (sprt_chan->IsValid)
        return XAxiVdma_ChannelSetBufferAddr(sprt_chan, BufferAddrSet, sprt_chan->NumFrames);

fail:
    return -ER_NOTFOUND;
}

/*!
 * Start one DMA channel
 *
 * @param sprt_vdma is the pointer to the DMA engine to work on
 * @param Direction is the DMA channel to work on
 *
 * @return
 * - ER_NORMAL if channel started successfully
 * - ER_FAILD otherwise
 * - ER_NOTFOUND if the channel is invalid
 * - ER_UNVALID if Direction invalid
 */
kint32_t XAxiVdma_DmaStart(XAxiVdma *sprt_vdma, kuint16_t Direction)
{
    XAxiVdma_Channel *sprt_chan;

    sprt_chan = XAxiVdma_GetChannel(sprt_vdma, Direction);
    if (!sprt_chan)
        goto fail;

    if (sprt_chan->IsValid)
        return XAxiVdma_ChannelStart(sprt_chan);

fail:
    return -ER_NOTFOUND;
}

/*!
 * Stop one DMA channel
 *
 * @param sprt_vdma is the pointer to the DMA engine to work on
 * @param Direction is the DMA channel to work on
 *
 * @return
 *  None
 *
 * @note
 * If channel is invalid, then do nothing on that channel
 */
void XAxiVdma_DmaStop(XAxiVdma *sprt_vdma, kuint16_t Direction)
{
    XAxiVdma_Channel *sprt_chan;

    sprt_chan = XAxiVdma_GetChannel(sprt_vdma, Direction);
    if (!sprt_chan)
        return;

    if (sprt_chan->IsValid)
        XAxiVdma_ChannelStop(sprt_chan);
}

/*!
 * Check whether a DMA channel is busy
 *
 * @param sprt_vdma is the pointer to the DMA engine to work on
 * @param Direction is the channel to work on, use XAXIVDMA_READ/WRITE
 *
 * @return
 * - Non-zero if the channel is busy
 * - Zero if the channel is idle
 */
kint32_t XAxiVdma_IsBusy(XAxiVdma *sprt_vdma, kuint16_t Direction)
{
    XAxiVdma_Channel *sprt_chan;

    sprt_chan = XAxiVdma_GetChannel(sprt_vdma, Direction);
    if (!sprt_chan)
        return 0;

    if (sprt_chan->IsValid)
        return XAxiVdma_ChannelIsBusy(sprt_chan);

    /*!< An invalid channel is never busy */
    return 0;
}

/*!
 * Start parking mode on a certain frame
 *
 * @param sprt_vdma is the pointer to the DMA engine to work on
 * @param FrameIndex is the frame to park on
 * @param Direction is the channel to work on, use XAXIVDMA_READ/WRITE
 *
 * @return
 *  - ER_NORMAL if everything is fine
 *  - ER_UNVALID if
 *    . channel is invalid
 *    . FrameIndex is invalid
 *    . Direction is invalid
 */
kint32_t XAxiVdma_StartParking(XAxiVdma *sprt_vdma, kint32_t FrameIndex, kuint16_t Direction)
{
    XAxiVdma_Channel *sprt_chan;
    kuint32_t FrmBits;
    kuint32_t RegValue;
    kint32_t Status;

    if (FrameIndex > XAXIVDMA_FRM_MAX)
        return -ER_UNVALID;

    if (Direction == XAXIVDMA_READ) 
    {
        FrmBits = FrameIndex & XAXIVDMA_PARKPTR_READREF_MASK;

        RegValue = XAxiVdma_ReadReg(sprt_vdma->BaseAddr, XAXIVDMA_PARKPTR_OFFSET);
        RegValue &= ~XAXIVDMA_PARKPTR_READREF_MASK;
        RegValue |= FrmBits;

        XAxiVdma_WriteReg(sprt_vdma->BaseAddr, XAXIVDMA_PARKPTR_OFFSET, RegValue);
    }
    else if (Direction == XAXIVDMA_WRITE) 
    {
        FrmBits = FrameIndex << XAXIVDMA_WRTREF_SHIFT;
        FrmBits &= XAXIVDMA_PARKPTR_WRTREF_MASK;

        RegValue = XAxiVdma_ReadReg(sprt_vdma->BaseAddr,
                      XAXIVDMA_PARKPTR_OFFSET);
        RegValue &= ~XAXIVDMA_PARKPTR_WRTREF_MASK;
        RegValue |= FrmBits;

        XAxiVdma_WriteReg(sprt_vdma->BaseAddr, XAXIVDMA_PARKPTR_OFFSET, RegValue);
    }
    else 
    {
        /*!< Invalid direction, do nothing */
        return -ER_UNVALID;
    }

    sprt_chan = XAxiVdma_GetChannel(sprt_vdma, Direction);
    if (sprt_chan->IsValid) 
    {
        Status = XAxiVdma_ChannelStartParking(sprt_chan);
        if (Status)
            return Status;
    }

    return ER_NORMAL;
}

XAxiVdma_Config *XAxiVdma_LookupConfig(kuint16_t DeviceId)
{
    XAxiVdma_Config *sprt_cfg = mrt_nullptr;
    kuint32_t i;

    for (i = 0U; i < XPAR_XAXIVDMA_NUM_INSTANCES; i++) 
    {
        if (XAxiVdma_ConfigTable[i].DeviceId == DeviceId) 
        {
            sprt_cfg = &XAxiVdma_ConfigTable[i];
            break;
        }
    }

    return sprt_cfg;
}

/*!
 * Initialize the driver with hardware configuration
 *
 * @param sprt_vdma is the pointer to the DMA engine to work on
 * @param sprt_cfg is the pointer to the hardware configuration structure
 * @param EffectiveAddr is the virtual address map for the device
 *
 * @return
 *  - ER_NORMAL if everything goes fine
 *  - ER_FAILD if reset the hardware failed, need system reset to recover
 *
 * @note
 * If channel fails reset,  then it will be set as invalid
 */
kint32_t XAxiVdma_CfgInitialize(XAxiVdma *sprt_vdma, XAxiVdma_Config *sprt_cfg, kuint32_t EffectiveAddr)
{
    XAxiVdma_Channel *sprt_rdchan;
    XAxiVdma_Channel *sprt_wrchan;
    kint32_t Polls;

    /*!< Validate parameters */
    if ((!sprt_vdma) ||
        (!sprt_cfg))
        return -ER_NULLPTR;

    /*!< Initially, no interrupt callback functions */
    sprt_vdma->ReadCallBack.CompletionCallBack = 0x0;
    sprt_vdma->ReadCallBack.ErrCallBack = 0x0;
    sprt_vdma->WriteCallBack.CompletionCallBack = 0x0;
    sprt_vdma->WriteCallBack.ErrCallBack = 0x0;

    sprt_vdma->BaseAddr = EffectiveAddr;
    sprt_vdma->MaxNumFrames = sprt_cfg->MaxFrameStoreNum;
    sprt_vdma->HasMm2S = sprt_cfg->HasMm2S;
    sprt_vdma->HasS2Mm = sprt_cfg->HasS2Mm;
    sprt_vdma->UseFsync = sprt_cfg->UseFsync;
    sprt_vdma->InternalGenLock = sprt_cfg->InternalGenLock;
    sprt_vdma->AddrWidth = sprt_cfg->AddrWidth;

    if (XAxiVdma_Major(sprt_vdma) < 3)
        sprt_vdma->HasSG = 1;
    else
        sprt_vdma->HasSG = sprt_cfg->HasSG;

    /*!< The channels are not valid until being initialized */
    sprt_rdchan = XAxiVdma_GetChannel(sprt_vdma, XAXIVDMA_READ);
    sprt_rdchan->IsValid = 0;

    sprt_wrchan = XAxiVdma_GetChannel(sprt_vdma, XAXIVDMA_WRITE);
    sprt_wrchan->IsValid = 0;

    if (sprt_vdma->HasMm2S) 
    {
        sprt_rdchan->direction = XAXIVDMA_READ;
        sprt_rdchan->ChanBase = sprt_vdma->BaseAddr + XAXIVDMA_TX_OFFSET;
        sprt_rdchan->InstanceBase = sprt_vdma->BaseAddr;
        sprt_rdchan->HasSG = sprt_vdma->HasSG;
        sprt_rdchan->IsRead = 1;
        sprt_rdchan->StartAddrBase = sprt_vdma->BaseAddr + XAXIVDMA_MM2S_ADDR_OFFSET;
        sprt_rdchan->NumFrames = sprt_cfg->MaxFrameStoreNum;

        /*!< Flush on Sync */
        sprt_rdchan->FlushonFsync = sprt_cfg->FlushonFsync;

        /*!< Dynamic Line Buffers Depth */
        sprt_rdchan->LineBufDepth = sprt_cfg->Mm2SBufDepth;
        if (sprt_rdchan->LineBufDepth > 0)
            sprt_rdchan->LineBufThreshold = XAxiVdma_ReadReg(sprt_rdchan->ChanBase, XAXIVDMA_BUFTHRES_OFFSET);

        sprt_rdchan->HasDRE = sprt_cfg->HasMm2SDRE;
        sprt_rdchan->WordLength = sprt_cfg->Mm2SWordLen >> 3;
        sprt_rdchan->StreamWidth = sprt_cfg->Mm2SStreamWidth >> 3;
        sprt_rdchan->AddrWidth = sprt_vdma->AddrWidth;

        /*!< Internal GenLock */
        sprt_rdchan->GenLock = sprt_cfg->Mm2SGenLock;

        /*!< Debug Info Parameter flags */
        if (!sprt_cfg->EnableAllDbgFeatures) 
        {
            if (sprt_cfg->Mm2SThresRegEn)
                sprt_rdchan->DbgFeatureFlags |= XAXIVDMA_ENABLE_DBG_THRESHOLD_REG;

            if (sprt_cfg->Mm2SFrmStoreRegEn)
                sprt_rdchan->DbgFeatureFlags |= XAXIVDMA_ENABLE_DBG_FRMSTORE_REG;

            if (sprt_cfg->Mm2SDlyCntrEn)
                sprt_rdchan->DbgFeatureFlags |= XAXIVDMA_ENABLE_DBG_DLY_CNTR;

            if (sprt_cfg->Mm2SFrmCntrEn)
                sprt_rdchan->DbgFeatureFlags |= XAXIVDMA_ENABLE_DBG_FRM_CNTR;
        } 
        else 
        {
            sprt_rdchan->DbgFeatureFlags = XAXIVDMA_ENABLE_DBG_ALL_FEATURES;
        }

        XAxiVdma_ChannelInit(sprt_rdchan);
        XAxiVdma_ChannelReset(sprt_rdchan);

        /*!< At time of initialization, no transfers are going on, reset is expected to be quick */
        Polls = INITIALIZATION_POLLING;
        while (Polls && XAxiVdma_ChannelResetNotDone(sprt_rdchan))
            Polls -= 1;

        if (!Polls)
            return -ER_FAILD;
    }

    if (sprt_vdma->HasS2Mm) 
    {
        sprt_wrchan->direction = XAXIVDMA_WRITE;
        sprt_wrchan->ChanBase = sprt_vdma->BaseAddr + XAXIVDMA_RX_OFFSET;
        sprt_wrchan->InstanceBase = sprt_vdma->BaseAddr;
        sprt_wrchan->HasSG = sprt_vdma->HasSG;
        sprt_wrchan->IsRead = 0;
        sprt_wrchan->StartAddrBase = sprt_vdma->BaseAddr + XAXIVDMA_S2MM_ADDR_OFFSET;
        sprt_wrchan->NumFrames = sprt_cfg->MaxFrameStoreNum;
        sprt_wrchan->AddrWidth = sprt_vdma->AddrWidth;
        sprt_wrchan->HasVFlip = sprt_cfg->HasVFlip;

        /*!< Flush on Sync */
        sprt_wrchan->FlushonFsync = sprt_cfg->FlushonFsync;

        /*!< Dynamic Line Buffers Depth */
        sprt_wrchan->LineBufDepth = sprt_cfg->S2MmBufDepth;
        if (sprt_wrchan->LineBufDepth > 0)
            sprt_wrchan->LineBufThreshold = XAxiVdma_ReadReg(sprt_wrchan->ChanBase, XAXIVDMA_BUFTHRES_OFFSET);

        sprt_wrchan->HasDRE = sprt_cfg->HasS2MmDRE;
        sprt_wrchan->WordLength = sprt_cfg->S2MmWordLen >> 3;
        sprt_wrchan->StreamWidth = sprt_cfg->S2MmStreamWidth >> 3;

        /*!< Internal GenLock */
        sprt_wrchan->GenLock = sprt_cfg->S2MmGenLock;

        /*!< Frame Sync Source Selection*/
        sprt_wrchan->S2MmSOF = sprt_cfg->S2MmSOF;

        /*!< Debug Info Parameter flags */
        if (!sprt_cfg->EnableAllDbgFeatures) 
        {
            if (sprt_cfg->S2MmThresRegEn)
                sprt_wrchan->DbgFeatureFlags |= XAXIVDMA_ENABLE_DBG_THRESHOLD_REG;

            if (sprt_cfg->S2MmFrmStoreRegEn)
                sprt_wrchan->DbgFeatureFlags |= XAXIVDMA_ENABLE_DBG_FRMSTORE_REG;

            if (sprt_cfg->S2MmDlyCntrEn)
                sprt_wrchan->DbgFeatureFlags |= XAXIVDMA_ENABLE_DBG_DLY_CNTR;

            if (sprt_cfg->S2MmFrmCntrEn)
                sprt_wrchan->DbgFeatureFlags |= XAXIVDMA_ENABLE_DBG_FRM_CNTR;

        } 
        else 
        {
            sprt_wrchan->DbgFeatureFlags = XAXIVDMA_ENABLE_DBG_ALL_FEATURES;
        }

        XAxiVdma_ChannelInit(sprt_wrchan);
        XAxiVdma_ChannelReset(sprt_wrchan);

        /*!< At time of initialization, no transfers are going on, reset is expected to be quick */
        Polls = INITIALIZATION_POLLING;
        while (Polls && XAxiVdma_ChannelResetNotDone(sprt_wrchan)) {
            Polls -= 1;
        }

        if (!Polls)
            return -ER_FAILD;
    }

    sprt_vdma->IsReady = true;

    return ER_NORMAL;
}

void XVtc_RegUpdateEnable(XVtc *sprt_vtc)
{
    kuint32_t reg;

    reg  = XVtc_ReadReg(sprt_vtc->Config.BaseAddress, XVTC_CTL_OFFSET);
    XVtc_WriteReg(sprt_vtc->Config.BaseAddress, XVTC_CTL_OFFSET, reg | XVTC_CTL_RU_MASK);
}

/*!
* This function converts the video timing structure into the VTC signal
* configuration structure, horizontal offsets structure and the
* polarity structure.
*
* @param	sprt_vtc is a pointer to the VTC instance to be worked on.
* @param	sprt_timing is a pointer to a Video Timing structure to be read.
* @param	sprt_signal is a pointer to a VTC signal configuration to be set.
* @param	sprt_horiOff is a pointer to a VTC horizontal offsets structure to be set.
* @param	sprt_polarity is a pointer to a VTC polarity structure to be set.
*
* @return	None.
*
* @note		None.
*/
void XVtc_ConvTiming2Signal(XVtc *sprt_vtc, XVtc_Timing *sprt_timing,
                        XVtc_Signal *sprt_signal, XVtc_HoriOffsets *sprt_horiOff,
                        XVtc_Polarity *sprt_polarity)
{
    /* Verify arguments. */
    if ((!sprt_vtc) ||
        (!sprt_vtc->IsReady) ||
        (!sprt_timing) ||
        (!sprt_signal) ||
        (!sprt_horiOff) ||
        (!sprt_polarity))
        return;

    /*!< Setting up VTC Polarity.  */
    memset((void *)sprt_polarity, 0, sizeof(XVtc_Polarity));

    sprt_polarity->ActiveChromaPol = 1;
    sprt_polarity->ActiveVideoPol = 1;
    sprt_polarity->FieldIdPol = 1;

    /*!< Vblank matches Vsync Polarity */
    sprt_polarity->VBlankPol = sprt_timing->VSyncPolarity;
    sprt_polarity->VSyncPol = sprt_timing->VSyncPolarity;

    /*!< hblank matches hsync Polarity */
    sprt_polarity->HBlankPol = sprt_timing->HSyncPolarity;
    sprt_polarity->HSyncPol = sprt_timing->HSyncPolarity;

    memset((void *)sprt_signal, 0, sizeof(XVtc_Signal));
    memset((void *)sprt_horiOff, 0, sizeof(XVtc_HoriOffsets));

    /*!< Populate the VTC Signal config structure. */
    /*!< Active Video starts at 0 */
    sprt_signal->OriginMode = 1;
    sprt_signal->HActiveStart = 0;
    sprt_signal->HFrontPorchStart = sprt_timing->HActiveVideo;
    sprt_signal->HSyncStart = sprt_signal->HFrontPorchStart + sprt_timing->HFrontPorch;
    sprt_signal->HBackPorchStart = sprt_signal->HSyncStart + sprt_timing->HSyncWidth;
    sprt_signal->HTotal = sprt_signal->HBackPorchStart + sprt_timing->HBackPorch;

    sprt_signal->V0ChromaStart = 0;
    sprt_signal->V0ActiveStart = 0;
    sprt_signal->V0FrontPorchStart = sprt_timing->VActiveVideo;
    sprt_signal->V0SyncStart = sprt_signal->V0FrontPorchStart + sprt_timing->V0FrontPorch - 1;
    sprt_signal->V0BackPorchStart = sprt_signal->V0SyncStart + sprt_timing->V0SyncWidth;
    sprt_signal->V0Total = sprt_signal->V0BackPorchStart + sprt_timing->V0BackPorch + 1;

    sprt_horiOff->V0BlankHoriStart = sprt_signal->HFrontPorchStart;
    sprt_horiOff->V0BlankHoriEnd = sprt_signal->HFrontPorchStart;
    sprt_horiOff->V0SyncHoriStart = sprt_signal->HSyncStart;
    sprt_horiOff->V0SyncHoriEnd = sprt_signal->HSyncStart;

    if (sprt_timing->Interlaced == 1) 
    {
        sprt_signal->V1ChromaStart = 0;
        sprt_signal->V1ActiveStart = 0;
        sprt_signal->V1FrontPorchStart = sprt_timing->VActiveVideo;
        sprt_signal->V1SyncStart = sprt_signal->V1FrontPorchStart + sprt_timing->V1FrontPorch - 1;
        sprt_signal->V1BackPorchStart = sprt_signal->V1SyncStart + sprt_timing->V1SyncWidth;
        sprt_signal->V1Total = sprt_signal->V1BackPorchStart + sprt_timing->V1BackPorch + 1;
        sprt_signal->Interlaced = 1;

        /*!< Align to H blank */
        sprt_horiOff->V1BlankHoriStart = sprt_signal->HFrontPorchStart;
        /*!< Align to H Blank */
        sprt_horiOff->V1BlankHoriEnd = sprt_signal->HFrontPorchStart;

        /*!< Align to half line */
        sprt_horiOff->V1SyncHoriStart = sprt_signal->HSyncStart - (sprt_signal->HTotal / 2);
        sprt_horiOff->V1SyncHoriEnd = sprt_signal->HSyncStart - (sprt_signal->HTotal / 2);
    }
    /*!< Progressive formats */
    else
    {
        /*!< Set Field 1 same as Field 0 */
        sprt_signal->V1ChromaStart = sprt_signal->V0ChromaStart;
        sprt_signal->V1ActiveStart = sprt_signal->V0ActiveStart;
        sprt_signal->V1FrontPorchStart = sprt_signal->V0FrontPorchStart;
        sprt_signal->V1SyncStart = sprt_signal->V0SyncStart;
        sprt_signal->V1BackPorchStart = sprt_signal->V0BackPorchStart;
        sprt_signal->V1Total = sprt_signal->V0Total;
        sprt_signal->Interlaced = 0;

        sprt_horiOff->V1BlankHoriStart = sprt_horiOff->V0BlankHoriStart;
        sprt_horiOff->V1BlankHoriEnd = sprt_horiOff->V0BlankHoriEnd;
        sprt_horiOff->V1SyncHoriStart = sprt_horiOff->V0SyncHoriStart;
        sprt_horiOff->V1SyncHoriEnd = sprt_horiOff->V0SyncHoriEnd;
    }
}

/*!
* This function converts the VTC signal structure, horizontal offsets
* structure and the polarity structure into the Video Timing structure.
*
* @param	sprt_vtc is a pointer to the VTC instance to be
*		    worked on.
* @param	sprt_signal is a pointer to a VTC signal configuration to
*		    be read
* @param	sprt_horiOff is a pointer to a VTC horizontal offsets structure
*		    to be read
* @param	sprt_polarity is a pointer to a VTC polarity structure to be
*		    read.
* @param	sprt_timing is a pointer to a Video Timing structure to be set.
*
* @return	None.
*
* @note		None.
*/
void XVtc_ConvSignal2Timing(XVtc *sprt_vtc, XVtc_Signal *sprt_signal,
                        XVtc_HoriOffsets *sprt_horiOff, XVtc_Polarity *sprt_polarity,
                        XVtc_Timing *sprt_timing)
{
    /*!< Verify arguments. */
    if ((!sprt_vtc) ||
        (!sprt_vtc->IsReady) ||
        (!sprt_timing) ||
        (!sprt_signal) ||
        (sprt_signal->OriginMode != 1) ||
        (!sprt_horiOff) ||
        (!sprt_polarity))
        return;

    memset((void *)sprt_timing, 0, sizeof(XVtc_Timing));

    /*!< Set Polarity */
    sprt_timing->VSyncPolarity = sprt_polarity->VSyncPol;
    sprt_timing->HSyncPolarity = sprt_polarity->HSyncPol;

    /*!< Horizontal Timing */
    sprt_timing->HActiveVideo = sprt_signal->HFrontPorchStart;

    sprt_timing->HFrontPorch = sprt_signal->HSyncStart - sprt_signal->HFrontPorchStart;
    sprt_timing->HSyncWidth = sprt_signal->HBackPorchStart - sprt_signal->HSyncStart;
    sprt_timing->HBackPorch = sprt_signal->HTotal - sprt_signal->HBackPorchStart;

    /*!< Vertical Timing */
    sprt_timing->VActiveVideo = sprt_signal->V0FrontPorchStart;
    sprt_timing->V0FrontPorch = sprt_signal->V0SyncStart - sprt_signal->V0FrontPorchStart + 1;
    sprt_timing->V0SyncWidth = sprt_signal->V0BackPorchStart - sprt_signal->V0SyncStart + 1;
    sprt_timing->V0BackPorch = sprt_signal->V0Total - sprt_signal->V0BackPorchStart;

    sprt_timing->V1FrontPorch = sprt_signal->V1SyncStart - sprt_signal->V1FrontPorchStart + 1;
    sprt_timing->V1SyncWidth = sprt_signal->V1BackPorchStart - sprt_signal->V1SyncStart + 1;
    sprt_timing->V1BackPorch = sprt_signal->V1Total - sprt_signal->V1BackPorchStart;

    /*!< Interlaced */
    sprt_timing->Interlaced = sprt_signal->Interlaced;
}

/*!
* This function sets up the output polarity of the VTC core.
*
* @param	sprt_vtc is a pointer to the VTC instance to be worked on.
* @param	sprt_polarity points to a Polarity configuration structure with the setting to use on the VTC core.
*
* @return	None.
*
* @note		None.
*/
void XVtc_SetPolarity(XVtc *sprt_vtc, XVtc_Polarity *sprt_polarity)
{
    kuint32_t PolRegValue;

    /*!< Verify arguments. */
    if ((!sprt_vtc) ||
        (!sprt_vtc->IsReady) ||
        (!sprt_polarity))
        return;

    /*!< Read Control register value back and clear all polarity bits first */
    PolRegValue  = XVtc_ReadReg(sprt_vtc->Config.BaseAddress, (XVTC_GPOL_OFFSET));
    PolRegValue &= (kuint32_t)(~(XVTC_POL_ALLP_MASK));

    /*!< Change the register value according to the setting in the Polarity configuration structure */
    if (sprt_polarity->ActiveChromaPol)
        PolRegValue |= XVTC_POL_ACP_MASK;

    if (sprt_polarity->ActiveVideoPol)
        PolRegValue |= XVTC_POL_AVP_MASK;

    if (sprt_polarity->FieldIdPol)
        PolRegValue |= XVTC_POL_FIP_MASK;

    if (sprt_polarity->VBlankPol)
        PolRegValue |= XVTC_POL_VBP_MASK;

    if (sprt_polarity->VSyncPol)
        PolRegValue |= XVTC_POL_VSP_MASK;

    if (sprt_polarity->HBlankPol)
        PolRegValue |= XVTC_POL_HBP_MASK;

    if (sprt_polarity->HSyncPol)
        PolRegValue |= XVTC_POL_HSP_MASK;

    XVtc_WriteReg(sprt_vtc->Config.BaseAddress, (XVTC_GPOL_OFFSET), PolRegValue);
}

/*!
 * This function sets the VBlank/VSync Horizontal Offsets for the Generator
 * in a VTC device.
 *
 * @param  sprt_vtc is a pointer to the VTC device instance to be worked on.
 * @param  sprt_horiOff points to a VBlank/VSync Horizontal Offset configuration
 *	   with the setting to use on the VTC device.
 * @return NONE.
 */
void XVtc_SetGeneratorHoriOffset(XVtc *sprt_vtc, XVtc_HoriOffsets *sprt_horiOff)
{
    kuint32_t RegValue;

    /*!< Assert bad arguments and conditions */
    if ((!sprt_vtc) ||
        (!sprt_vtc->IsReady) ||
        (!sprt_horiOff))
        return;

    /*!< Calculate and update Generator VBlank Hori. Offset 0 register value */
    RegValue  = (sprt_horiOff->V0BlankHoriStart) & XVTC_XVXHOX_HSTART_MASK;
    RegValue |= (sprt_horiOff->V0BlankHoriEnd << XVTC_XVXHOX_HEND_SHIFT) & XVTC_XVXHOX_HEND_MASK;
    XVtc_WriteReg(sprt_vtc->Config.BaseAddress, XVTC_GVBHOFF_OFFSET, RegValue);

    /*!< Calculate and update Generator VSync Hori. Offset 0 register value */
    RegValue  = (sprt_horiOff->V0SyncHoriStart) & XVTC_XVXHOX_HSTART_MASK;
    RegValue |= (sprt_horiOff->V0SyncHoriEnd << XVTC_XVXHOX_HEND_SHIFT) & XVTC_XVXHOX_HEND_MASK;
    XVtc_WriteReg(sprt_vtc->Config.BaseAddress, XVTC_GVSHOFF_OFFSET, RegValue);

    /*!< Calculate and update Generator VBlank Hori. Offset 1 register value */
    RegValue = (sprt_horiOff->V1BlankHoriStart) & XVTC_XVXHOX_HSTART_MASK;
    RegValue |= (sprt_horiOff->V1BlankHoriEnd << XVTC_XVXHOX_HEND_SHIFT) & XVTC_XVXHOX_HEND_MASK;
    XVtc_WriteReg(sprt_vtc->Config.BaseAddress, XVTC_GVBHOFF_F1_OFFSET, RegValue);

    /*!< Calculate and update Generator VSync Hori. Offset 1 register value */
    RegValue = (sprt_horiOff->V1SyncHoriStart) & XVTC_XVXHOX_HSTART_MASK;
    RegValue |= (sprt_horiOff->V1SyncHoriEnd << XVTC_XVXHOX_HEND_SHIFT) & XVTC_XVXHOX_HEND_MASK;

    XVtc_WriteReg(sprt_vtc->Config.BaseAddress, XVTC_GVSHOFF_F1_OFFSET, RegValue);
}

/*!
 * This function sets up VTC signal to be used by the Generator module
 * in the VTC core.
 *
 * @param	sprt_vtc is a pointer to the VTC instance to be
 *		    worked on.
 * @param	sprt_signal is a pointer to the VTC signal configuration
 *		    to be used by the Generator module in the VTC core.
 *
 * @return	None.
 *
 * @note	None.
 */
void XVtc_SetGenerator(XVtc *sprt_vtc, XVtc_Signal *sprt_signal)
{
    kuint32_t RegValue;
    kuint32_t r_htotal, r_vtotal, r_hactive, r_vactive;
    XVtc_HoriOffsets sgrt_horiOff;

    /*!< Verify arguments. */
    if ((!sprt_vtc) ||
        (!sprt_vtc->IsReady) ||
        (!sprt_signal))
        return;

    if (sprt_signal->OriginMode == 0)
    {
        r_htotal = sprt_signal->HTotal+1;
        r_vtotal = sprt_signal->V0Total+1;

        r_hactive = r_htotal - sprt_signal->HActiveStart;
        r_vactive = r_vtotal - sprt_signal->V0ActiveStart;

        RegValue = (r_htotal) & XVTC_SB_START_MASK;
        XVtc_WriteReg(sprt_vtc->Config.BaseAddress, XVTC_GHSIZE_OFFSET, RegValue);

        RegValue = (r_vtotal) & XVTC_VSIZE_F0_MASK;
        RegValue |= ((sprt_signal->V1Total+1) << XVTC_VSIZE_F1_SHIFT) & XVTC_VSIZE_F1_MASK;
        XVtc_WriteReg(sprt_vtc->Config.BaseAddress, XVTC_GVSIZE_OFFSET, RegValue);

        RegValue = (r_hactive) & XVTC_ASIZE_HORI_MASK;
        RegValue |= ((r_vactive) << XVTC_ASIZE_VERT_SHIFT ) & XVTC_ASIZE_VERT_MASK;
        XVtc_WriteReg(sprt_vtc->Config.BaseAddress, XVTC_GASIZE_OFFSET, RegValue);

        /*!<
         * For some resolutions, the FIELD1 vactive size is different
         * from FIELD0, e.g. XVIDC_VM_720x486_60_I (SDI NTSC),
         * As there is no vactive FIELD1 entry in the video common
         * library, program it separately. For resolutions where
         * vactive values are different, it should be taken care in
         * corrosponding driver. Otherwise program same values in
         * FIELD0 and FIELD1 registers 
         */
        RegValue = ((r_vactive) << XVTC_ASIZE_VERT_SHIFT) & XVTC_ASIZE_VERT_MASK;
        XVtc_WriteReg(sprt_vtc->Config.BaseAddress, XVTC_GASIZE_F1_OFFSET, RegValue);

        /*!< Update the Generator Horizontal 1 Register */
        RegValue = (sprt_signal->HSyncStart + r_hactive) & XVTC_SB_START_MASK;
        RegValue |= ((sprt_signal->HBackPorchStart + r_hactive) << XVTC_SB_END_SHIFT) & XVTC_SB_END_MASK;
        XVtc_WriteReg(sprt_vtc->Config.BaseAddress, XVTC_GHSYNC_OFFSET, RegValue);

        /*!< Update the Generator Vertical 1 Register (field 0) */
        RegValue = (sprt_signal->V0SyncStart + r_vactive -1) & XVTC_SB_START_MASK;
        RegValue |= ((sprt_signal->V0BackPorchStart + r_vactive -1) << XVTC_SB_END_SHIFT) & XVTC_SB_END_MASK;
        XVtc_WriteReg(sprt_vtc->Config.BaseAddress, XVTC_GVSYNC_OFFSET, RegValue);

        /*!< Update the Generator Vertical Sync Register (field 1) */
        RegValue = (sprt_signal->V1SyncStart + r_vactive -1) & XVTC_SB_START_MASK;
        RegValue |= ((sprt_signal->V1BackPorchStart + r_vactive -1) << XVTC_SB_END_SHIFT) & XVTC_SB_END_MASK;
        XVtc_WriteReg(sprt_vtc->Config.BaseAddress, XVTC_GVSYNC_F1_OFFSET, RegValue);

        /*!< Chroma Start */
        RegValue = XVtc_ReadReg(sprt_vtc->Config.BaseAddress, XVTC_GFENC_OFFSET);
        RegValue &= ~XVTC_ENC_CPARITY_MASK;
        RegValue = ((((sprt_signal->V0ChromaStart - sprt_signal->V0ActiveStart) << XVTC_ENC_CPARITY_SHIFT) &
                    XVTC_ENC_CPARITY_MASK) | RegValue);
        RegValue &= ~XVTC_ENC_PROG_MASK;
        RegValue |= (sprt_signal->Interlaced << XVTC_ENC_PROG_SHIFT) & XVTC_ENC_PROG_MASK;
        XVtc_WriteReg(sprt_vtc->Config.BaseAddress, XVTC_GFENC_OFFSET, RegValue);

        /*!<
         * Setup default Horizontal Offsets - can override later with
         * XVtc_SetGeneratorHoriOffset()
         */
        sgrt_horiOff.V0BlankHoriStart = r_hactive;
        sgrt_horiOff.V0BlankHoriEnd = r_hactive;
        sgrt_horiOff.V0SyncHoriStart = sprt_signal->HSyncStart + r_hactive;
        sgrt_horiOff.V0SyncHoriEnd = sprt_signal->HSyncStart + r_hactive;

        sgrt_horiOff.V1BlankHoriStart = r_hactive;
        sgrt_horiOff.V1BlankHoriEnd = r_hactive;
        sgrt_horiOff.V1SyncHoriStart = sprt_signal->HSyncStart + r_hactive;
        sgrt_horiOff.V1SyncHoriEnd = sprt_signal->HSyncStart + r_hactive;
    }
    else
    {
        /*!< Total in mode=1 is the line width */
        r_htotal = sprt_signal->HTotal;
        /*!< Total in mode=1 is the frame height */
        r_vtotal = sprt_signal->V0Total;
        r_hactive = sprt_signal->HFrontPorchStart;
        r_vactive = sprt_signal->V0FrontPorchStart;

        RegValue = (r_htotal) & XVTC_SB_START_MASK;
        XVtc_WriteReg(sprt_vtc->Config.BaseAddress, XVTC_GHSIZE_OFFSET, RegValue);

        RegValue = (r_vtotal) & XVTC_VSIZE_F0_MASK;
        RegValue |= ((sprt_signal->V1Total) << XVTC_VSIZE_F1_SHIFT) & XVTC_VSIZE_F1_MASK;
        XVtc_WriteReg(sprt_vtc->Config.BaseAddress, XVTC_GVSIZE_OFFSET, RegValue);

        RegValue = (r_hactive) & XVTC_ASIZE_HORI_MASK;
        RegValue |= ((r_vactive) << XVTC_ASIZE_VERT_SHIFT) & XVTC_ASIZE_VERT_MASK;
        XVtc_WriteReg(sprt_vtc->Config.BaseAddress, XVTC_GASIZE_OFFSET, RegValue);

        /*!<
         * For some resolutions, the FIELD1 vactive size is different
         * from FIELD0, e.g. XVIDC_VM_720x486_60_I (SDI NTSC),
         * As there is no vactive FIELD1 entry in the video common
         * library, program it separately. For resolutions where
         * vactive values are different, it should be taken care in
         * corrosponding driver. Otherwise program same values in
         * FIELD0 and FIELD1 registers 
         */
        RegValue = ((r_vactive) << XVTC_ASIZE_VERT_SHIFT) & XVTC_ASIZE_VERT_MASK;
        XVtc_WriteReg(sprt_vtc->Config.BaseAddress, XVTC_GASIZE_F1_OFFSET, RegValue);

        /*!< Update the Generator Horizontal 1 Register */
        RegValue = (sprt_signal->HSyncStart) & XVTC_SB_START_MASK;
        RegValue |= ((sprt_signal->HBackPorchStart) << XVTC_SB_END_SHIFT) & XVTC_SB_END_MASK;
        XVtc_WriteReg(sprt_vtc->Config.BaseAddress, XVTC_GHSYNC_OFFSET, RegValue);

        /*!< Update the Generator Vertical Sync Register (field 0) */
        RegValue = (sprt_signal->V0SyncStart) & XVTC_SB_START_MASK;
        RegValue |= ((sprt_signal->V0BackPorchStart) << XVTC_SB_END_SHIFT) & XVTC_SB_END_MASK;
        XVtc_WriteReg(sprt_vtc->Config.BaseAddress, XVTC_GVSYNC_OFFSET, RegValue);

        /*!< Update the Generator Vertical Sync Register (field 1) */
        RegValue = (sprt_signal->V1SyncStart) & XVTC_SB_START_MASK;
        RegValue |= ((sprt_signal->V1BackPorchStart) << XVTC_SB_END_SHIFT) & XVTC_SB_END_MASK;
        XVtc_WriteReg(sprt_vtc->Config.BaseAddress, XVTC_GVSYNC_F1_OFFSET, RegValue);

        /*!< Chroma Start */
        RegValue = XVtc_ReadReg(sprt_vtc->Config.BaseAddress, XVTC_GFENC_OFFSET);
        RegValue &= ~XVTC_ENC_CPARITY_MASK;
        RegValue = ((((sprt_signal->V0ChromaStart - sprt_signal->V0ActiveStart) << XVTC_ENC_CPARITY_SHIFT)
                    & XVTC_ENC_CPARITY_MASK) | RegValue);
        RegValue &= ~XVTC_ENC_PROG_MASK;
        RegValue |= (sprt_signal->Interlaced << XVTC_ENC_PROG_SHIFT) & XVTC_ENC_PROG_MASK;
        XVtc_WriteReg(sprt_vtc->Config.BaseAddress, XVTC_GFENC_OFFSET, RegValue);

        /*!<
         * Setup default Horizontal Offsets - can override later with
         * XVtc_SetGeneratorHoriOffset()
         */
        sgrt_horiOff.V0BlankHoriStart = r_hactive;
        sgrt_horiOff.V0BlankHoriEnd = r_hactive;
        sgrt_horiOff.V0SyncHoriStart = sprt_signal->HSyncStart;
        sgrt_horiOff.V0SyncHoriEnd = sprt_signal->HSyncStart;
        sgrt_horiOff.V1BlankHoriStart = r_hactive;
        sgrt_horiOff.V1BlankHoriEnd = r_hactive;
        sgrt_horiOff.V1SyncHoriStart = sprt_signal->HSyncStart;
        sgrt_horiOff.V1SyncHoriEnd = sprt_signal->HSyncStart;
    }

    XVtc_SetGeneratorHoriOffset(sprt_vtc, &sgrt_horiOff);
}

/*!
 * This function sets up the generator (Polarity, H/V values and horizontal
 * offsets) by reading the configuration from a video timing structure.
 *
 * @param	sprt_vtc is a pointer to the VTC instance to be
 *		    worked on.
 * @param	sprt_timing is a pointer to a Video Timing Structure to be read.
 *
 * @return	None.
 *
 * @note	None.
 */
void XVtc_SetGeneratorTiming(XVtc *sprt_vtc, XVtc_Timing * sprt_timing)
{
    XVtc_Polarity sgrt_polarity;
    XVtc_Signal sgrt_signal;
    XVtc_HoriOffsets sgrt_horiOff;

    /*!< Verify arguments. */
    if ((!sprt_vtc) ||
        (!sprt_vtc->IsReady) ||
        (!sprt_timing))
        return;

    XVtc_ConvTiming2Signal(sprt_vtc, sprt_timing, &sgrt_signal, &sgrt_horiOff, &sgrt_polarity);
    XVtc_SetPolarity(sprt_vtc, &sgrt_polarity);
    XVtc_SetGenerator(sprt_vtc, &sgrt_signal);
    XVtc_SetGeneratorHoriOffset(sprt_vtc, &sgrt_horiOff);
}

/*!
 * This function enables the VTC Generator core.
 *
 * @param	sprt_vtc is a pointer to the VTC instance to be
 *		    worked on.
 *
 * @return	None.
 *
 * @note	None.
 */
void XVtc_EnableGenerator(XVtc *sprt_vtc)
{
    kuint32_t CtrlRegValue;

    /*!< Verify arguments. */
    if ((!sprt_vtc) ||
        (!sprt_vtc->IsReady))
        return;

    /*!< Read Control register value back */
    CtrlRegValue = XVtc_ReadReg(sprt_vtc->Config.BaseAddress, (XVTC_CTL_OFFSET));

    /*!< Change the value according to the enabling type and write it back */
    CtrlRegValue |= XVTC_CTL_GE_MASK;

    XVtc_WriteReg(sprt_vtc->Config.BaseAddress, (XVTC_CTL_OFFSET), CtrlRegValue);
}

/*!
 * This function disables the VTC Generator core.
 *
 * @param	InstancePtr is a pointer to the VTC instance to be
 *		    worked on.
 *
 * @return	None.
 *
 * @note	None.
 */
void XVtc_DisableGenerator(XVtc *sprt_vtc)
{
    kuint32_t CtrlRegValue;

    /*!< Verify arguments. */
    if ((!sprt_vtc) ||
        (!sprt_vtc->IsReady))
        return;

    /*!< Read Control register value back */
    CtrlRegValue = XVtc_ReadReg(sprt_vtc->Config.BaseAddress, (XVTC_CTL_OFFSET));

    /*!< Change the value according to the disabling type and write it back */
    CtrlRegValue &= (kuint32_t)(~(XVTC_CTL_GE_MASK));

    XVtc_WriteReg(sprt_vtc->Config.BaseAddress, (XVTC_CTL_OFFSET), CtrlRegValue);
}

/*!
 * This function sets up the source selecting of the VTC core.
 *
 * @param	sprt_vtc is a pointer to the VTC instance to be
 *		    worked on
 * @param 	sprt_srcSel points to a Source Selecting configuration structure
 *		    with the setting to use on the VTC device.
 *
 * @return	None.
 *
 * @note	None.
 */
void XVtc_SetSource(XVtc *sprt_vtc, XVtc_SourceSelect *sprt_srcSel)
{
    kuint32_t CtrlRegValue;

    /*!< Verify arguments. */
    if ((!sprt_vtc) ||
        (!sprt_vtc->IsReady) ||
        (!sprt_srcSel))
        return;

    /*!< Read Control register value back and clear all source selection bits first */
    CtrlRegValue = XVtc_ReadReg(sprt_vtc->Config.BaseAddress, (XVTC_CTL_OFFSET));
    CtrlRegValue &= ~XVTC_CTL_ALLSS_MASK;

    /*!< Change the register value according to the setting in the source selection configuration structure */
    if (sprt_srcSel->FieldIdPolSrc)
        CtrlRegValue |= XVTC_CTL_FIPSS_MASK;

    if (sprt_srcSel->ActiveChromaPolSrc)
        CtrlRegValue |= XVTC_CTL_ACPSS_MASK;

    if (sprt_srcSel->ActiveVideoPolSrc)
        CtrlRegValue |= XVTC_CTL_AVPSS_MASK;

    if (sprt_srcSel->HSyncPolSrc)
        CtrlRegValue |= XVTC_CTL_HSPSS_MASK;

    if (sprt_srcSel->VSyncPolSrc)
        CtrlRegValue |= XVTC_CTL_VSPSS_MASK;

    if (sprt_srcSel->HBlankPolSrc)
        CtrlRegValue |= XVTC_CTL_HBPSS_MASK;

    if (sprt_srcSel->VBlankPolSrc)
        CtrlRegValue |= XVTC_CTL_VBPSS_MASK;

    if (sprt_srcSel->VChromaSrc)
        CtrlRegValue |= XVTC_CTL_VCSS_MASK;

    if (sprt_srcSel->VActiveSrc)
        CtrlRegValue |= XVTC_CTL_VASS_MASK;

    if (sprt_srcSel->VBackPorchSrc)
        CtrlRegValue |= XVTC_CTL_VBSS_MASK;

    if (sprt_srcSel->VSyncSrc)
        CtrlRegValue |= XVTC_CTL_VSSS_MASK;

    if (sprt_srcSel->VFrontPorchSrc)
        CtrlRegValue |= XVTC_CTL_VFSS_MASK;

    if (sprt_srcSel->VTotalSrc)
        CtrlRegValue |= XVTC_CTL_VTSS_MASK;

    if (sprt_srcSel->HBackPorchSrc)
        CtrlRegValue |= XVTC_CTL_HBSS_MASK;

    if (sprt_srcSel->HSyncSrc)
        CtrlRegValue |= XVTC_CTL_HSSS_MASK;

    if (sprt_srcSel->HFrontPorchSrc)
        CtrlRegValue |= XVTC_CTL_HFSS_MASK;

    if (sprt_srcSel->HTotalSrc)
        CtrlRegValue |= XVTC_CTL_HTSS_MASK;

    if (sprt_srcSel->InterlacedMode)
        CtrlRegValue |= XVTC_CTL_INTERLACE_MASK;

    XVtc_WriteReg(sprt_vtc->Config.BaseAddress, (XVTC_CTL_OFFSET), CtrlRegValue);
}

/*!
 * This function reads version register of the VTC core and compares with zero
 * as part of self test.
 *
 * @param	sprt_vtc is a pointer to the XVtc instance.
 *
 * @return
 *		    - ER_NORMAL if the Version register read test was successful.
 *		    - ER_FAULT if the Version register read test failed.
 *
 * @note	None.
 */
kint32_t XVtc_SelfTest(XVtc *sprt_vtc)
{
    kuint32_t Version;

    /*!< Verify argument. */
    if (!sprt_vtc)
        return -ER_NULLPTR;

    /*!< Read VTC core version register. */
    Version = XVtc_ReadReg((sprt_vtc)->Config.BaseAddress, (XVTC_VER_OFFSET));

    /*!< Compare version with zero */
    if (!Version)
        return -ER_FAULT;

    return ER_NORMAL;
}

XVtc_Config *XVtc_LookupConfig(kuint16_t DeviceId)
{
    XVtc_Config *sprt_vcfg = mrt_nullptr;
    kint32_t i;

    /*!< Checking for device id for which instance it is matching */
    for (i = 0; i < XPAR_XVTC_NUM_INSTANCES; i++) 
    {
        /*!< Assigning address of config table if both device ids are matched */
        if (XVtc_ConfigTable[i].DeviceId == DeviceId) 
        {
            sprt_vcfg = &XVtc_ConfigTable[i];
            break;
        }
    }

    return sprt_vcfg;
}

/*!
 * This function initializes the VTC core. This function must be called
 * prior to using the VTC core. Initialization of the VTC includes setting up
 * the instance data, and ensuring the hardware is in a quiescent state.
 *
 * @param	sprt_vtc is a pointer to the VTC core instance to be
 *		    worked on.
 * @param	CfgPtr points to the configuration structure associated with
 *		    the VTC core.
 * @param	EffectiveAddr is the base address of the device. If address
 *		    translation is being used, then this parameter must reflect the
 *		    virtual base address. Otherwise, the physical address should be
 *		    used.
 *
 * @return
 *		    - ER_NORMAL if XVtc_CfgInitialize was successful.
 *
 * @note	None.
 */
kint32_t XVtc_CfgInitialize(XVtc *sprt_vtc, XVtc_Config *sprt_vcfg, kuint32_t EffectiveAddr)
{
    /*!< Verify arguments */
    if ((!sprt_vtc) ||
        (!sprt_vcfg) ||
        (!EffectiveAddr))
        return -ER_NULLPTR;

    /*!< Setup the instance */
    memset((void *)sprt_vtc, 0, sizeof(XVtc));

    memcpy((void *)&(sprt_vtc->Config), (const void *)sprt_vcfg, sizeof(XVtc_Config));
    sprt_vtc->Config.BaseAddress = EffectiveAddr;

    /*!< Set all handlers to stub values, let user configure this data later */
    sprt_vtc->FrameSyncCallBack = mrt_nullptr;
    sprt_vtc->LockCallBack = mrt_nullptr;
    sprt_vtc->DetectorCallBack = mrt_nullptr;
    sprt_vtc->GeneratorCallBack = mrt_nullptr;
    sprt_vtc->ErrCallBack = mrt_nullptr;

    /*!< Set the flag to indicate the driver is ready */
    sprt_vtc->IsReady = true;

    return ER_NORMAL;
}

/*!	
 * DisplayInitialize(DisplayCtrl *sprt_disp, XAxiVdma *vdma, kuint16_t vtcId, 
 *                  kuint32_t dynClkAddr, u8 *framePtr[DISPLAY_NUM_FRAMES], kuint32_t stride)
 *
 *	Parameters:
 *		sprt_disp - Pointer to the struct that will be initialized
 *		vdma - Pointer to initialized VDMA struct
 *		vtcId - Device ID of the VTC core as found in xparameters.h
 *		dynClkAddr - BASE ADDRESS of the axi_dynclk core
 *		framePtr - array of pointers to the frame buffers. The frame buffers must be instantiated above this driver, and there must be 3
 *		stride - line stride of the frame buffers. This is the number of bytes between the start of one line and the start of another.
 *
 *	Return Value: kint32_t
 *		XST_SUCCESS if successful, XST_FAILURE otherwise
 *
 *	Errors:
 *
 *	Description:
 *		Initializes the driver struct for use.
 */
kint32_t DisplayInitialize(DisplayCtrl *sprt_disp, XAxiVdma *sprt_vdma, kuint16_t vtcId, 
                    kuint32_t dynClkAddr, kuint8_t *framePtr[DISPLAY_NUM_FRAMES], 
                    kuint32_t stride, VideoMode *sprt_vmode)
{
//  kint32_t Status;
    kint32_t i;
//  XVtc_Config *sprt_vcfg;
    ClkConfig sgrt_clkcfg;
    ClkMode sgrt_clkmode;

    /*!< Initialize all the fields in the DisplayCtrl struct */
    sprt_disp->curFrame = 0;
    sprt_disp->dynClkAddr = dynClkAddr;
    for (i = 0; i < DISPLAY_NUM_FRAMES; i++)
        sprt_disp->framePtr[i] = framePtr[i];

    sprt_disp->state = NR_XIL_DISPLAY_STOPPED;
    sprt_disp->stride = stride;

    /*!< Supported resolution */
    memcpy(&sprt_disp->vMode, sprt_vmode, sizeof(*sprt_vmode));

    ClkFindParams(sprt_disp->vMode.freq, &sgrt_clkmode);

    /*!<
     * Store the obtained frequency to pxlFreq. It is possible that the PLL was not able to
     * exactly generate the desired pixel clock, so this may differ from vMode.freq.
     */
    sprt_disp->pxlFreq = sgrt_clkmode.freq;

    /*!< Write to the PLL dynamic configuration registers to configure it with the calculated parameters. */
    if (!ClkFindReg(&sgrt_clkcfg, &sgrt_clkmode))
        return -ER_FAILD;

    ClkWriteReg(&sgrt_clkcfg, sprt_disp->dynClkAddr);

    /*!< Enable the dynamically generated clock */
    ClkStart(sprt_disp->dynClkAddr);

    /*!<
     * Initialize the VTC driver so that it's ready to use look up
     * configuration in the config table, then initialize it.
     */
//  sprt_vcfg = XVtc_LookupConfig(vtcId);
//  /*!< Checking Config variable */
//  if (mrt_nullptr == sprt_vcfg)
//      return -ER_FAILD;

//  Status = XVtc_CfgInitialize(&(sprt_disp->vtc), sprt_vcfg, sprt_vcfg->BaseAddress);
//  /*!< Checking status */
//  if (Status)
//      return Status;

    sprt_disp->vdma = sprt_vdma;

    /*!< Initialize the VDMA Read configuration struct */
    sprt_disp->vdmaConfig.FrameDelay = 0;
    sprt_disp->vdmaConfig.EnableCircularBuf = 1;
    sprt_disp->vdmaConfig.EnableSync = 0;
    sprt_disp->vdmaConfig.PointNum = 0;
    sprt_disp->vdmaConfig.EnableFrameCounter = 0;

    return ER_NORMAL;
}

/*!
 *	DisplayChangeFrameBuffer(DisplayCtrl *sprt_disp, kuint32_t FrameAddr, kusize_t FrameSize)
 *
 *	Parameters:
 *		sprt_disp - Pointer to the initialized DisplayCtrl struct
 *      FrameAddr - New FrameBuffer Address
 *      
 *	Return Value: kint32_t
 *		ER_NORMAL if successful, ER_UNVALID otherwise
 *
 *	Errors:
 *
 *	Description:
 *		Set FrameBuffer Address
 */
kint32_t DisplayChangeFrameBuffer(DisplayCtrl *sprt_disp, kuint32_t FrameAddr, kusize_t FrameSize)
{
    kint32_t index, Status;

    /*!< If already stopped, do nothing */
    if (sprt_disp->state == NR_XIL_DISPLAY_STOPPED)
        return -ER_NREADY;

    /*!<  Stop the VDMA core */
//  XAxiVdma_DmaStop(sprt_disp->vdma, XAXIVDMA_READ);
//  while (XAxiVdma_IsBusy(sprt_disp->vdma, XAXIVDMA_READ));

    index = sprt_disp->curFrame;
    sprt_disp->framePtr[index] = (kuint8_t *)FrameAddr;
    sprt_disp->vdmaConfig.FrameStoreStartAddr[index] = FrameAddr;
    Xil_DCacheFlushRange(FrameAddr, FrameSize);

    Status = XAxiVdma_DmaSetBufferAddr(sprt_disp->vdma, XAXIVDMA_READ, 
                                    sprt_disp->vdmaConfig.FrameStoreStartAddr);
    if (Status)
        return Status;

    Status = XAxiVdma_DmaStart(sprt_disp->vdma, XAXIVDMA_READ);
    if (Status)
        return Status;

    return ER_NORMAL;
}

/*!
 *	DisplayStart(DisplayCtrl *sprt_disp)
 *
 *	Parameters:
 *		sprt_disp - Pointer to the initialized DisplayCtrl struct
 *
 *	Return Value: kint32_t
 *		ER_NORMAL if successful, ER_UNVALID otherwise
 *
 *	Errors:
 *
 *	Description:
 *		Starts the display.
 */
kint32_t DisplayStart(DisplayCtrl *sprt_disp)
{
    kint32_t Status;
    ClkConfig sgrt_clkcfg;
    ClkMode sgrt_clkmode;
    kint32_t i;
    XVtc_Timing sgrt_timing;
    XVtc_SourceSelect sgrt_srcSel;

    /*!< If already started, do nothing */
    if (sprt_disp->state == NR_XIL_DISPLAY_RUNNING)
        return ER_NORMAL;

    /*!< Calculate the PLL divider parameters based on the required pixel clock frequency */
    ClkFindParams(sprt_disp->vMode.freq, &sgrt_clkmode);

    /*!<
     * Store the obtained frequency to pxlFreq. It is possible that the PLL was not able to
     * exactly generate the desired pixel clock, so this may differ from vMode.freq.
     */
    sprt_disp->pxlFreq = sgrt_clkmode.freq;

    /*!<
     * Write to the PLL dynamic configuration registers to configure it with the calculated
     * parameters.
     */
    if (!ClkFindReg(&sgrt_clkcfg, &sgrt_clkmode))
        return -ER_NOTFOUND;

    ClkWriteReg(&sgrt_clkcfg, sprt_disp->dynClkAddr);

    /*!< Enable the dynamically generated clock */
    ClkStop(sprt_disp->dynClkAddr);
    ClkStart(sprt_disp->dynClkAddr);

    /*!< Configure the vtc core with the display mode timing parameters */
    sgrt_timing.HActiveVideo = sprt_disp->vMode.width;						        /*!< Horizontal Active Video Size */
    sgrt_timing.HFrontPorch = sprt_disp->vMode.hps - sprt_disp->vMode.width;	    /*!< Horizontal Front Porch Size */
    sgrt_timing.HSyncWidth = sprt_disp->vMode.hpe - sprt_disp->vMode.hps;		    /*!< Horizontal Sync Width */
    sgrt_timing.HBackPorch = sprt_disp->vMode.hmax - sprt_disp->vMode.hpe + 1;      /*!< Horizontal Back Porch Size */
    sgrt_timing.HSyncPolarity = sprt_disp->vMode.hpol;	                            /*!< Horizontal Sync Polarity */
    sgrt_timing.VActiveVideo = sprt_disp->vMode.height;	                            /*!< Vertical Active Video Size */
    sgrt_timing.V0FrontPorch = sprt_disp->vMode.vps - sprt_disp->vMode.height;      /*!< Vertical Front Porch Size */
    sgrt_timing.V0SyncWidth = sprt_disp->vMode.vpe - sprt_disp->vMode.vps;	        /*!< Vertical Sync Width */
    sgrt_timing.V0BackPorch = sprt_disp->vMode.vmax - sprt_disp->vMode.vpe + 1;;	/*!< Horizontal Back Porch Size */
    sgrt_timing.V1FrontPorch = sprt_disp->vMode.vps - sprt_disp->vMode.height;	    /*!< Vertical Front Porch Size */
    sgrt_timing.V1SyncWidth = sprt_disp->vMode.vpe - sprt_disp->vMode.vps;	        /*!< Vertical Sync Width */
    sgrt_timing.V1BackPorch = sprt_disp->vMode.vmax - sprt_disp->vMode.vpe + 1;;	/*!< Horizontal Back Porch Size */
    sgrt_timing.VSyncPolarity = sprt_disp->vMode.vpol;	                            /*!< Vertical Sync Polarity */
    sgrt_timing.Interlaced = 0;		                                                /*!< Interlaced / Progressive video */

    /*!< Setup the VTC Source Select config structure. */
    /*!< 1=Generator registers are source */
    /*!< 0=Detector registers are source */
    memset((void *)&sgrt_srcSel, 0, sizeof(sgrt_srcSel));

    sgrt_srcSel.VBlankPolSrc = 1;
    sgrt_srcSel.VSyncPolSrc = 1;
    sgrt_srcSel.HBlankPolSrc = 1;
    sgrt_srcSel.HSyncPolSrc = 1;
    sgrt_srcSel.ActiveVideoPolSrc = 1;
    sgrt_srcSel.ActiveChromaPolSrc= 1;
    sgrt_srcSel.VChromaSrc = 1;
    sgrt_srcSel.VActiveSrc = 1;
    sgrt_srcSel.VBackPorchSrc = 1;
    sgrt_srcSel.VSyncSrc = 1;
    sgrt_srcSel.VFrontPorchSrc = 1;
    sgrt_srcSel.VTotalSrc = 1;
    sgrt_srcSel.HActiveSrc = 1;
    sgrt_srcSel.HBackPorchSrc = 1;
    sgrt_srcSel.HSyncSrc = 1;
    sgrt_srcSel.HFrontPorchSrc = 1;
    sgrt_srcSel.HTotalSrc = 1;

    XVtc_SelfTest(&(sprt_disp->vtc));

    XVtc_RegUpdateEnable(&(sprt_disp->vtc));
    XVtc_SetGeneratorTiming(&(sprt_disp->vtc), &sgrt_timing);
    XVtc_SetSource(&(sprt_disp->vtc), &sgrt_srcSel);

    /*!< Enable VTC core, releasing backpressure on VDMA */
    XVtc_EnableGenerator(&sprt_disp->vtc);

    /*!< Configure the VDMA to access a frame with the same dimensions as the current mode */
    sprt_disp->vdmaConfig.VertSizeInput = sprt_disp->vMode.height;
    sprt_disp->vdmaConfig.HoriSizeInput = (sprt_disp->vMode.width) * 4;
    sprt_disp->vdmaConfig.FixedFrameStoreAddr = sprt_disp->curFrame;
    /*!< Also reset the stride and address values, in case the user manually changed them */
    sprt_disp->vdmaConfig.Stride = sprt_disp->stride;

    for (i = 0; i < DISPLAY_NUM_FRAMES; i++)
        sprt_disp->vdmaConfig.FrameStoreStartAddr[i] = (kuint32_t)sprt_disp->framePtr[i];

    /*!<
     * Perform the VDMA driver calls required to start a transfer. Note that no data is actually
     * transferred until the disp_ctrl core signals the VDMA core by pulsing fsync.
     */
    Status = XAxiVdma_DmaConfig(sprt_disp->vdma, XAXIVDMA_READ, &(sprt_disp->vdmaConfig));
    if (Status)
        return Status;

    Status = XAxiVdma_DmaSetBufferAddr(sprt_disp->vdma, XAXIVDMA_READ, sprt_disp->vdmaConfig.FrameStoreStartAddr);
    if (Status)
        return Status;

    Status = XAxiVdma_DmaStart(sprt_disp->vdma, XAXIVDMA_READ);
    if (Status)
        return Status;

    Status = XAxiVdma_StartParking(sprt_disp->vdma, sprt_disp->curFrame, XAXIVDMA_READ);
    if (Status)
        return Status;

    sprt_disp->state = NR_XIL_DISPLAY_RUNNING;

    return ER_NORMAL;
}

/*!
 *	DisplayStop(DisplayCtrl *dispPtr)
 *
 *	Parameters:
 *		dispPtr - Pointer to the initialized DisplayCtrl struct
 *
 *	Return Value: int
 *		XST_SUCCESS if successful.
 *		XST_DMA_ERROR if an error was detected on the DMA channel. The
 *			Display is still successfully stopped, and the error is
 *			cleared so that subsequent DisplayStart calls will be
 *			successful. This typically indicates insufficient bandwidth
 *			on the AXI Memory-Map Interconnect (VDMA<->DDR)
 *
 *	Description:
 *		Halts output to the display
 */
kint32_t DisplayStop(DisplayCtrl *sprt_dispctrl)
{
    /*!< If already stopped, do nothing */
    if (sprt_dispctrl->state == NR_XIL_DISPLAY_STOPPED)
        return ER_NORMAL;

    /*!<
     * Disable the disp_ctrl core, and wait for the current frame to finish 
     * (the core cannot stop mid-frame)
     */
    XVtc_DisableGenerator(&sprt_dispctrl->vtc);

    /*!<  Stop the VDMA core */
    XAxiVdma_DmaStop(sprt_dispctrl->vdma, XAXIVDMA_READ);
    while (XAxiVdma_IsBusy(sprt_dispctrl->vdma, XAXIVDMA_READ));

    /*!< Update Struct state */
    sprt_dispctrl->state = NR_XIL_DISPLAY_STOPPED;

    /*! TODO: consider stopping the clock here, perhaps after a check to see if the VTC is finished */

    if (XAxiVdma_GetDmaChannelErrors(sprt_dispctrl->vdma, XAXIVDMA_READ))
    {
        XAxiVdma_ClearDmaChannelErrors(sprt_dispctrl->vdma, XAXIVDMA_READ, 0xFFFFFFFF);
        return -ER_FAULT;
    }

    return ER_NORMAL;
}

/* end of file */
