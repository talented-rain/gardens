/******************************************************************************
*
* Copyright (C) 2010 - 2018 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/
/*****************************************************************************/
/**
*
* @file xemacps_bdring.h
* @addtogroup emacps_v3_8
* @{
*
* The Xiline EmacPs Buffer Descriptor ring driver. This is part of EmacPs
* DMA functionalities.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00a wsy  01/10/10 First release
* 2.1   srt  07/15/14 Add support for Zynq Ultrascale Mp architecture.
* 3.0   kvn  02/13/15 Modified code for MISRA-C:2012 compliance.
* 3.6   rb   09/08/17 HwCnt variable (in XEmacPs_BdRing structure) is
*		      changed to volatile.
*
* </pre>
*
******************************************************************************/

#ifndef XEMACPS_BDRING_H	/* prevent curcular inclusions */
#define XEMACPS_BDRING_H	/* by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif

#include "../zynq7_periph.h"
#include "xemacps_bd.h"

/**************************** Type Definitions *******************************/

/** This is an internal structure used to maintain the DMA list */
typedef struct {
	kuint32_t PhysBaseAddr;/**< Physical address of 1st BD in list */
	kuint32_t BaseBdAddr;	 /**< Virtual address of 1st BD in list */
	kuint32_t HighBdAddr;	 /**< Virtual address of last BD in the list */
	kuint32_t Length;	 /**< Total size of ring in bytes */
	kbool_t IsRunning;	 /**< Flag to indicate DMA is started */
	kuint32_t Separation;	 /**< Number of bytes between the starting address
                                  of adjacent BDs */
	XEmacPs_Bd *FreeHead;
			     /**< First BD in the free group */
	XEmacPs_Bd *PreHead;/**< First BD in the pre-work group */
	XEmacPs_Bd *HwHead; /**< First BD in the work group */
	XEmacPs_Bd *HwTail; /**< Last BD in the work group */
	XEmacPs_Bd *PostHead;
			     /**< First BD in the post-work group */
	XEmacPs_Bd *BdaRestart;
			     /**< BDA to load when channel is started */

	volatile kuint32_t HwCnt;    /**< Number of BDs in work group */
	kuint32_t PreCnt;     /**< Number of BDs in pre-work group */
	kuint32_t FreeCnt;    /**< Number of allocatable BDs in the free group */
	kuint32_t PostCnt;    /**< Number of BDs in post-work group */
	kuint32_t AllCnt;     /**< Total Number of BDs for channel */
} XEmacPs_BdRing;


/***************** Macros (Inline Functions) Definitions *********************/

/*****************************************************************************/
/**
* Use this macro at initialization time to determine how many BDs will fit
* in a BD list within the given memory constraints.
*
* The results of this macro can be provided to XEmacPs_BdRingCreate().
*
* @param Alignment specifies what byte alignment the BDs must fall on and
*        must be a power of 2 to get an accurate calculation (32, 64, 128,...)
* @param Bytes is the number of bytes to be used to store BDs.
*
* @return Number of BDs that can fit in the given memory area
*
* @note
* C-style signature:
*    kuint32_t XEmacPs_BdRingCntCalc(kuint32_t Alignment, kuint32_t Bytes)
*
******************************************************************************/
#define XEmacPs_BdRingCntCalc(Alignment, Bytes)                    \
    (kuint32_t)((Bytes) / (sizeof(XEmacPs_Bd)))

/*****************************************************************************/
/**
* Use this macro at initialization time to determine how many bytes of memory
* is required to contain a given number of BDs at a given alignment.
*
* @param Alignment specifies what byte alignment the BDs must fall on. This
*        parameter must be a power of 2 to get an accurate calculation (32, 64,
*        128,...)
* @param NumBd is the number of BDs to calculate memory size requirements for
*
* @return The number of bytes of memory required to create a BD list with the
*         given memory constraints.
*
* @note
* C-style signature:
*    kuint32_t XEmacPs_BdRingMemCalc(kuint32_t Alignment, kuint32_t NumBd)
*
******************************************************************************/
#define XEmacPs_BdRingMemCalc(Alignment, NumBd)                    \
    (kuint32_t)(sizeof(XEmacPs_Bd) * (NumBd))

/****************************************************************************/
/**
* Return the total number of BDs allocated by this channel with
* XEmacPs_BdRingCreate().
*
* @param  RingPtr is the DMA channel to operate on.
*
* @return The total number of BDs allocated for this channel.
*
* @note
* C-style signature:
*    kuint32_t XEmacPs_BdRingGetCnt(XEmacPs_BdRing* RingPtr)
*
*****************************************************************************/
#define XEmacPs_BdRingGetCnt(RingPtr) ((RingPtr)->AllCnt)

/****************************************************************************/
/**
* Return the number of BDs allocatable with XEmacPs_BdRingAlloc() for pre-
* processing.
*
* @param  RingPtr is the DMA channel to operate on.
*
* @return The number of BDs currently allocatable.
*
* @note
* C-style signature:
*    kuint32_t XEmacPs_BdRingGetFreeCnt(XEmacPs_BdRing* RingPtr)
*
*****************************************************************************/
#define XEmacPs_BdRingGetFreeCnt(RingPtr)   ((RingPtr)->FreeCnt)

/****************************************************************************/
/**
* Return the next BD from BdPtr in a list.
*
* @param  RingPtr is the DMA channel to operate on.
* @param  BdPtr is the BD to operate on.
*
* @return The next BD in the list relative to the BdPtr parameter.
*
* @note
* C-style signature:
*    XEmacPs_Bd *XEmacPs_BdRingNext(XEmacPs_BdRing* RingPtr,
*                                      XEmacPs_Bd *BdPtr)
*
*****************************************************************************/
#define XEmacPs_BdRingNext(RingPtr, BdPtr)                           \
    (((kuint32_t)((void *)(BdPtr)) >= (RingPtr)->HighBdAddr) ?                     \
    (XEmacPs_Bd*)((void*)(RingPtr)->BaseBdAddr) :                              \
    (XEmacPs_Bd*)((kuint32_t)((void *)(BdPtr)) + (RingPtr)->Separation))

/****************************************************************************/
/**
* Return the previous BD from BdPtr in the list.
*
* @param  RingPtr is the DMA channel to operate on.
* @param  BdPtr is the BD to operate on
*
* @return The previous BD in the list relative to the BdPtr parameter.
*
* @note
* C-style signature:
*    XEmacPs_Bd *XEmacPs_BdRingPrev(XEmacPs_BdRing* RingPtr,
*                                      XEmacPs_Bd *BdPtr)
*
*****************************************************************************/
#define XEmacPs_BdRingPrev(RingPtr, BdPtr)                           \
    (((kuint32_t)(BdPtr) <= (RingPtr)->BaseBdAddr) ?                     \
    (XEmacPs_Bd*)(RingPtr)->HighBdAddr :                              \
    (XEmacPs_Bd*)((kuint32_t)(BdPtr) - (RingPtr)->Separation))

/************************** Function Prototypes ******************************/
// extern kint32_t XEmacPs_BdRingCreate(XEmacPs_BdRing * RingPtr, kuint32_t PhysAddr,
//                                 kuint32_t VirtAddr, kuint32_t Alignment, kuint32_t BdCount);
// extern kint32_t XEmacPs_BdRingClone(XEmacPs_BdRing *RingPtr, XEmacPs_Bd *SrcBdPtr, kuint8_t Direction);
// extern kint32_t XEmacPs_BdRingAlloc(XEmacPs_BdRing *RingPtr, kuint32_t NumBd, XEmacPs_Bd **BdSetPtr);
// extern kint32_t XEmacPs_BdRingUnAlloc(XEmacPs_BdRing * RingPtr, kuint32_t NumBd, XEmacPs_Bd * BdSetPtr);
// extern kint32_t XEmacPs_BdRingFree(XEmacPs_BdRing * RingPtr, kuint32_t NumBd, XEmacPs_Bd * BdSetPtr);
// extern kuint32_t XEmacPs_BdRingFromHwTx(XEmacPs_BdRing * RingPtr, kuint32_t BdLimit, XEmacPs_Bd ** BdSetPtr);
// extern kuint32_t XEmacPs_BdRingFromHwRx(XEmacPs_BdRing * RingPtr, kuint32_t BdLimit, XEmacPs_Bd ** BdSetPtr);
// extern kint32_t XEmacPs_BdRingToHw(XEmacPs_BdRing * RingPtr, kuint32_t NumBd, XEmacPs_Bd * BdSetPtr);
// 
// extern void XEmacPsIf_DmaTxDescsClean(void *args);
// extern kint32_t XEmacPs_DmaInit(void *args);

extern kint32_t XEmacPs_BdRingCreate(XEmacPs_BdRing *sprt_bdring, kuint32_t PhysAddr,
                            kuint32_t VirtAddr, kuint32_t Alignment, kuint32_t BdCount);
extern void XEmacPs_BdRingInitial(XEmacPs_BdRing *sprt_bdring, kuint32_t PhysAddr,
                            kuint32_t VirtAddr, kuint32_t Alignment, kuint32_t BdCount);
extern kint32_t XEmacPs_BdRingClone(XEmacPs_BdRing *sprt_bdring, XEmacPs_Bd *sprt_bd, kuint8_t Direction);
extern kint32_t XEmacPs_BdRingAlloc(XEmacPs_BdRing *sprt_bdring, kuint32_t NumBd, XEmacPs_Bd **sprt_bd);
extern kint32_t XEmacPs_BdRingUnAlloc(XEmacPs_BdRing *sprt_bdring, kuint32_t NumBd, XEmacPs_Bd *sprt_bd);
extern kint32_t XEmacPs_BdRingFree(XEmacPs_BdRing *sprt_bdring, kuint32_t NumBd, XEmacPs_Bd *sprt_bd);
extern kuint32_t XEmacPs_BdRingFromHwTx(XEmacPs_BdRing *sprt_bdring, kuint32_t BdLimit, XEmacPs_Bd **sprt_bd);
extern kuint32_t XEmacPs_BdRingFromHwRx(XEmacPs_BdRing *sprt_bdring, kuint32_t BdLimit, XEmacPs_Bd **sprt_bd);
extern kint32_t XEmacPs_BdRingToHw(XEmacPs_BdRing *sprt_bdring, kuint32_t NumBd, XEmacPs_Bd *sprt_bd);

#ifdef __cplusplus
	}
#endif

#endif /* end of protection macros */
/** @} */
