/*
 * Copyright (C) 2010 - 2018 Xilinx, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 */

#ifndef __NETIF_XEMACPSIF_H__
#define __NETIF_XEMACPSIF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <common/generic.h>
#include <common/queue.h>

#include "lwip/netif.h"
#include "netif/etharp.h"
#include "lwip/sys.h"

#include "../xparameters.h"
#include "../xparameters_ps.h"	/* defines XPAR values */
#include "../zynq7_periph.h"

#include "xemacps.h"			/* defines XEmacPs API */
#include "xemacps_bdring.h"

#define ZYNQ_EMACPS_0_BASEADDR 0xE000B000
#define ZYNQ_EMACPS_1_BASEADDR 0xE000C000

#define ZYNQMP_EMACPS_0_BASEADDR 0xFF0B0000
#define ZYNQMP_EMACPS_1_BASEADDR 0xFF0C0000
#define ZYNQMP_EMACPS_2_BASEADDR 0xFF0D0000
#define ZYNQMP_EMACPS_3_BASEADDR 0xFF0E0000

#define CRL_APB_GEM0_REF_CTRL	0xFF5E0050
#define CRL_APB_GEM1_REF_CTRL	0xFF5E0054
#define CRL_APB_GEM2_REF_CTRL	0xFF5E0058
#define CRL_APB_GEM3_REF_CTRL	0xFF5E005C

#define CRL_APB_GEM_DIV0_MASK	0x00003F00
#define CRL_APB_GEM_DIV0_SHIFT	8
#define CRL_APB_GEM_DIV1_MASK	0x003F0000
#define CRL_APB_GEM_DIV1_SHIFT	16

#define MAX_FRAME_SIZE_JUMBO (XEMACPS_MTU_JUMBO + XEMACPS_HDR_SIZE + XEMACPS_TRL_SIZE)

struct xemac_s {
    kint32_t  topology_index;
    void *state;
};

/* structure within each netif, encapsulating all information required for
 * using a particular temac instance
 */
typedef struct {
	XEmacPs sgrt_emacps;

	/* queue to store overflow packets */
	srt_pq_t *sprt_rxq;
	srt_pq_t *sprt_txq;

	/* pointers to memory holding buffer descriptors (used only with SDMA) */
	void *rx_bdspace;
	void *tx_bdspace;

	unsigned int last_rx_frms_cntr;

} xemacpsif_s;

// extern void XEmacPsIf_SentBds(xemacpsif_s *xemacpsif, XEmacPs_BdRing *txring);
// extern kint32_t XEmacPsIf_SgSend(xemacpsif_s *xemacpsif, struct pbuf *p);
// extern void XEmacPsIf_ResetRx_WithNoRxData(xemacpsif_s *xemacpsif);
// extern void XEmacPsIf_SetupRxBds(xemacpsif_s *xemacpsif, XEmacPs_BdRing *rxring);
// extern void XEmacPsIf_TxRxBuffer_Free(xemacpsif_s *xemacpsif);
// extern void XEmacPsIf_TxBuffer_Free(xemacpsif_s *xemacpsif);
// extern void XEmacPsIf_Init_OnError(xemacpsif_s *xemacps);
// extern void XEmacPsIf_HandleError(void *args);
// extern void XEmacPsIf_HandleTxErrors(void *args);
// extern void XEmacPsIf_SendHandler(void *arg);
// extern void XEmacPsIf_RecvHandler(void *arg);
// extern void XEmacPsIf_ErrorHandler(void *arg, kuint8_t Direction, kuint32_t ErrorWord);
// extern void XEmacPsIf_SetupIsr(void *args);
// extern void XEmacPs_IntrHandler(void *XEmacPsPtr);
// 
// extern void XEmacPsIf_Start(xemacpsif_s *xemacps);

#ifdef __cplusplus
}
#endif

#endif /* __NETIF_XAXIEMACIF_H__ */
