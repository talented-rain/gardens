/*
 * ZYNQ7 Board SD MMC Initial
 *
 * File Name:   zynq7_sdmmc.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.06.23
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include "zynq7_common.h"
#include <common/time.h>
#include <platform/fwk_mempool.h>
#include <platform/mmc/fwk_sdcard.h>

/*!< The globals */
static XSdPs sgrt_xsd_ps_data;

/*!< API function */
/*!
 * @brief   initial and start sd
 * @param   none
 * @retval  none
 * @note    none
 */
void zynq7_sdmmc_init(void)
{
    XSdPs *sprt_sd = &sgrt_xsd_ps_data;
    XSdPs_Config *sprt_cfg;

    sprt_cfg = XSdPs_LookupConfig(XPAR_PS7_SD_0_DEVICE_ID);
    if (!isValid(sprt_cfg))
        return;

    XSdPs_CfgInitialize(sprt_sd, sprt_cfg, sprt_cfg->BaseAddress);
}

/*!< ---------------------------------------------------- */
/*!
 * @brief   XSdPs_ReadADMA_Data
 * @param   none
 * @retval  none
 * @note    read data by ADMA
 */
static kint32_t XSdPs_ReadADMA_Data(XSdPs *sprt_sd, struct fwk_sdcard_data *sprt_data)
{
    kuint8_t *Buff;
    kint32_t BlkCnt, BlkSize;
    kuint32_t StatusReg;
    kuaddr_t BaseAddress;

    if (!sprt_sd || !sprt_data)
        return -ER_NOMEM;

    if (!XSdPs_IsCardDetected(sprt_sd))
        return -ER_FAILD;

    BaseAddress = sprt_sd->sgrt_cfg.BaseAddress;
    Buff = (kuint8_t *)sprt_data->rxBuffer;
    BlkCnt = sprt_data->blockCount;
    BlkSize = sprt_data->blockSize & XSDPS_BLK_SIZE_MASK;

    /* Check for transfer complete */
    do {
        StatusReg = XSdPs_ReadReg16(BaseAddress, XSDPS_NORM_INTR_STS_OFFSET);
        if ((StatusReg & XSDPS_INTR_ERR_MASK) != 0U) 
        {
            /* Write to clear error bits */
            XSdPs_WriteReg16(BaseAddress, XSDPS_ERR_INTR_STS_OFFSET, XSDPS_ERROR_INTR_ALL_MASK);
            return -ER_FAILD;
        }
    
    } while ((StatusReg & XSDPS_INTR_TC_MASK) == 0U);

    /* Write to clear bit */
    XSdPs_WriteReg16(BaseAddress, XSDPS_NORM_INTR_STS_OFFSET, XSDPS_INTR_TC_MASK);

    if (!sprt_sd->sgrt_cfg.IsCacheCoherent)
        Xil_DCacheInvalidateRange((kuaddr_t)Buff, BlkCnt * BlkSize);

    return ER_NORMAL;
}

/*!
 * @brief   XSdPs_WriteADMA_Data
 * @param   none
 * @retval  none
 * @note    write data by ADMA
 */
static kint32_t XSdPs_WriteADMA_Data(XSdPs *sprt_sd, struct fwk_sdcard_data *sprt_data)
{
    kuint32_t StatusReg;
    kuaddr_t BaseAddress;

    if (!sprt_sd || !sprt_data)
        return -ER_NOMEM;

    if (!XSdPs_IsCardDetected(sprt_sd))
        return -ER_FAILD;

    BaseAddress = sprt_sd->sgrt_cfg.BaseAddress;

    /*
     * Check for transfer complete
     * Polling for response for now
     */
    do {
        StatusReg = XSdPs_ReadReg16(BaseAddress, XSDPS_NORM_INTR_STS_OFFSET);
        if ((StatusReg & XSDPS_INTR_ERR_MASK) != 0U) 
        {
            /* Write to clear error bits */
            XSdPs_WriteReg16(BaseAddress, XSDPS_ERR_INTR_STS_OFFSET, XSDPS_ERROR_INTR_ALL_MASK);

            return -ER_FAILD;
        }
    
    } while ((StatusReg & XSDPS_INTR_TC_MASK) == 0U);

    /* Write to clear bit */
    XSdPs_WriteReg16(BaseAddress, XSDPS_NORM_INTR_STS_OFFSET, XSDPS_INTR_TC_MASK);

    return ER_NORMAL;
}

static void zynq7_sdmmc_reset(struct fwk_sdcard_host *sprt_host)
{
    XSdPs *sprt_sd = (XSdPs *)sprt_host->iHostIfBase;
    kuint8_t ReadReg;
    kuaddr_t BaseAddress;

    BaseAddress = sprt_sd->sgrt_cfg.BaseAddress;

    XSdPs_WriteReg16(BaseAddress, XSDPS_NORM_INTR_STS_OFFSET, XSDPS_NORM_INTR_ALL_MASK);
    XSdPs_WriteReg16(BaseAddress, XSDPS_ERR_INTR_STS_OFFSET, XSDPS_ERROR_INTR_ALL_MASK);

    /* "Software reset for all" is initiated */
    XSdPs_WriteReg8(BaseAddress, XSDPS_SW_RST_OFFSET, XSDPS_SWRST_CMD_LINE_MASK);

    /* Proceed with initialization only after reset is complete */
    do 
    {
        ReadReg = XSdPs_ReadReg8(BaseAddress, XSDPS_SW_RST_OFFSET);

    } while ((ReadReg & XSDPS_SWRST_CMD_LINE_MASK) != 0U);
}

/*!
 * @brief   zynq7_sdmmc_is_card_insert
 * @param   none
 * @retval  none
 * @note    detect if card insert
 */
static kbool_t zynq7_sdmmc_is_card_insert(struct fwk_sdcard_host *sprt_host)
{
    XSdPs *sprt_sd = (XSdPs *)sprt_host->iHostCDBase;

    return XSdPs_IsCardDetected(sprt_sd);
}

/*!
 * @brief   zynq7_sdmmc_set_bus_width
 * @param   none
 * @retval  none
 * @note    configure card bus width
 */
static void zynq7_sdmmc_set_bus_width(struct fwk_sdcard_host *sprt_host, kuint32_t option)
{
    XSdPs *sprt_sd = (XSdPs *)sprt_host->iHostIfBase;

    switch (option)
    {
        case NR_SdCard_BusWidth_8Bit:
        	sprt_sd->BusWidth = XSDPS_8_BIT_WIDTH;
            break;
        case NR_SdCard_BusWidth_1Bit:
        	sprt_sd->BusWidth = XSDPS_1_BIT_WIDTH;
            break;
        default:
        	sprt_sd->BusWidth = XSDPS_4_BIT_WIDTH;
            break;
    }

    XSdPs_Change_BusWidth(sprt_sd, sprt_sd->BusWidth);
}

/*!
 * @brief   zynq7_sdmmc_set_clk_freq
 * @param   none
 * @retval  none
 * @note    configure sdcard clock frequency
 */
static void zynq7_sdmmc_set_clk_freq(struct fwk_sdcard_host *sprt_host, kuint32_t option)
{
    XSdPs *sprt_sd = (XSdPs *)sprt_host->iHostIfBase;

    if (option > NR_SdCard_ClkFreq_50MHz)
        option = NR_SdCard_ClkFreq_25MHz;

    if (sprt_sd->HC_Version == XSDPS_HC_SPEC_V3)
        sprt_sd->BusSpeed = SD_CLK_19_MHZ;
    else
    {
        switch (option)
        {
            case NR_SdCard_ClkFreq_400KHz:
                sprt_sd->BusSpeed = XSDPS_CLK_400_KHZ;
                break;

            default:
                sprt_sd->BusSpeed = SD_CLK_25_MHZ;
                break;
        }
    }

    XSdPs_Change_ClkFreq(sprt_sd, sprt_sd->BusSpeed);
}

/*!
 * @brief   zynq7_sdmmc_initial_active
 * @param   none
 * @retval  none
 * @note    initial active card; it will make card enter idle status
 */
static kbool_t zynq7_sdmmc_initial_active(struct fwk_sdcard_host *sprt_host, kuint32_t timeout)
{
    /*!< for zynq ps7, 74 CLK delay after card is powered up, before the first command */
    /*!< not to do anything here */
    delay_ms(timeout);

    return true;
}

/*!
 * @brief   zynq7_sdmmc_send_command
 * @param   none
 * @retval  none
 * @note    send command by SD
 */
static kint32_t zynq7_sdmmc_send_command(struct fwk_sdcard_cmd *sprt_cmds)
{
    struct fwk_sdcard_host *sprt_host;
    XSdPs *sprt_sd;

    kuint32_t PresentStateReg;
    kuint32_t CommandReg;
    kuint32_t StatusReg;
    kint32_t Status;
    kuint32_t Index, Argument;
    kuaddr_t BaseAddress;

    sprt_host = (struct fwk_sdcard_host *)sprt_cmds->ptrHost;
    if (!isValid(sprt_host))
        return -ER_NOMEM;

    sprt_sd = (XSdPs *)sprt_host->iHostIfBase;  
    if (!sprt_sd || !sprt_sd->IsReady)
        return -ER_NOMEM;

    BaseAddress = sprt_sd->sgrt_cfg.BaseAddress;
    Index = sprt_cmds->index & FWK_SDCARD_CMD_MASK;
    Argument = mrt_be32_to_cpu(sprt_cmds->args);

    /*
     * Check the command inhibit to make sure no other
     * command transfer is in progress
     */
    PresentStateReg = XSdPs_ReadReg(BaseAddress, XSDPS_PRES_STATE_OFFSET);
    while (PresentStateReg & XSDPS_PSR_INHIBIT_CMD_MASK);

    XSdPs_WriteReg8(BaseAddress, XSDPS_TIMEOUT_CTRL_OFFSET, 0xEU);

    XSdPs_WriteReg16(BaseAddress, XSDPS_NORM_INTR_STS_OFFSET, XSDPS_NORM_INTR_ALL_MASK);
    XSdPs_WriteReg16(BaseAddress, XSDPS_ERR_INTR_STS_OFFSET, XSDPS_ERROR_INTR_ALL_MASK);

    CommandReg = Index << 8;
    Index |= ((sprt_cmds->cmdType == FWK_SDCARD_CMDTYPE_APPLICATION) ? FWK_SDCARD_CMD_APP : 0U);

    /*!< set/reset bits according to response type */
    switch (sprt_cmds->respType)
    {
        case NR_SdCard_Response_0:
            break;

        case NR_SdCard_Response_1:
        case NR_SdCard_Response_7:
            CommandReg |= XSDPS_RESP_R1;

            if ((Index == NR_SdCard_Cmd_SelectFunc) || 
                (Index == NR_SdCard_Cmd_ReadSingleBlock) ||
                (Index == NR_SdCard_Cmd_ReadMultiBlock) ||
                (Index == NR_SdCard_Cmd_SetBlockCount) ||
                (Index == NR_SdCard_Cmd_WriteSingleBlock) ||
                (Index == NR_SdCard_Cmd_WriteMultiBlock) ||
                (Index == NR_SdCard_ACmd_GetSCR))
                CommandReg |= XSDPS_DAT_PRESENT_SEL_MASK;
            
            break;

        case NR_SdCard_Response_1b:
            CommandReg |= XSDPS_RESP_R1B;
            break;
        
        case NR_SdCard_Response_6:
            CommandReg |= XSDPS_RESP_R6;
            break;

        case NR_SdCard_Response_2:
            CommandReg |= XSDPS_RESP_R2;
            break;

        case NR_SdCard_Response_3:
            CommandReg |= XSDPS_RESP_R3;
            break;

        default:
            mrt_assert(false);
            break;
    }

    /*
     * Mask to avoid writing to reserved bits 31-30
     * This is necessary because 0x80000000 is used  by this software to
     * distinguish between ACMD and CMD of same number
     */
    CommandReg &= 0x3FFFU;

    /*
     * Check for data inhibit in case of command using DAT lines.
     * For Tuning Commands DAT lines check can be ignored.
     */
    PresentStateReg = XSdPs_ReadReg(BaseAddress, XSDPS_PRES_STATE_OFFSET);
    if ((PresentStateReg & XSDPS_PSR_INHIBIT_DAT_MASK) &&
        (CommandReg & XSDPS_DAT_PRESENT_SEL_MASK))
        return -ER_FAILD;

    /* Write argument register */
    XSdPs_WriteReg(BaseAddress, XSDPS_ARGMT_OFFSET, Argument);
    XSdPs_WriteReg(BaseAddress, XSDPS_XFER_MODE_OFFSET,
                            (CommandReg << 16) | sprt_sd->TransferMode);

    /* Polling for response for now */
    do {
        StatusReg = XSdPs_ReadReg16(BaseAddress, XSDPS_NORM_INTR_STS_OFFSET);
        if ((StatusReg & XSDPS_INTR_ERR_MASK) != 0U) 
        {
            Status = XSdPs_ReadReg16(BaseAddress, XSDPS_ERR_INTR_STS_OFFSET);
            if ((Status & ~XSDPS_INTR_ERR_CT_MASK) == 0)
                return -ER_SCMD_FAILD;
            
             /* Write to clear error bits */
            XSdPs_WriteReg16(BaseAddress, XSDPS_ERR_INTR_STS_OFFSET, XSDPS_ERROR_INTR_ALL_MASK);
            return -ER_FAILD;
        }

    } while ((StatusReg & XSDPS_INTR_CC_MASK) == 0U);

    /* Write to clear bit */
    XSdPs_WriteReg16(BaseAddress, XSDPS_NORM_INTR_STS_OFFSET, XSDPS_INTR_CC_MASK);

    return ER_NORMAL;
}

/*!
 * @brief   zynq7_sdmmc_recv_response
 * @param   none
 * @retval  none
 * @note    receive response by uSDHC
 */
static void zynq7_sdmmc_recv_response(struct fwk_sdcard_cmd *sprt_cmds)
{
    struct fwk_sdcard_host *sprt_host;
    XSdPs *sprt_sd;
    kuint8_t rsp_cnt = 3U;
    kuaddr_t BaseAddress;

    sprt_host = (struct fwk_sdcard_host *)sprt_cmds->ptrHost;
    if (!isValid(sprt_host))
        return;

    sprt_sd = (XSdPs *)sprt_host->iHostIfBase;
    BaseAddress = sprt_sd->sgrt_cfg.BaseAddress;

    if (NR_SdCard_Response_0 == sprt_cmds->respType)
        return;

    /*!< for CID/CSD: CMD_RSP0->bit[31:0] is CID/CSD bit[39:8] */
    *(sprt_cmds->resp) = XSdPs_ReadReg(BaseAddress, XSDPS_RESP0_OFFSET);

    /*!< R2 args = CID/CSD (bit[127:8]) */
    if (NR_SdCard_Response_2 == sprt_cmds->respType)
    {
        /*!< CMD_RSP3: bit[23:0] is CID/CSD bit[127:104]; CMD_RSP3 bit[31:24] is reserved */
        *(sprt_cmds->resp + 3U) = XSdPs_ReadReg(BaseAddress, XSDPS_RESP3_OFFSET);

        /*!< CMD_RSP2: bit[31:0] is CID/CSD bit[103:72] */
        *(sprt_cmds->resp + 2U) = XSdPs_ReadReg(BaseAddress, XSDPS_RESP2_OFFSET);

        /*!< CMD_RSP1: bit[31:0] is CID/CSD bit[71:40] */
        *(sprt_cmds->resp + 1U) = XSdPs_ReadReg(BaseAddress, XSDPS_RESP1_OFFSET);

        /*!< make resp similar to CID/CSD(bit[127:8]) */
        do
        {
            sprt_cmds->resp[rsp_cnt] = mrt_bit_mask(sprt_cmds->resp[rsp_cnt], 0xffffff00U, 8U);
            mrt_setbitl(sprt_cmds->resp[rsp_cnt - 1] >> 24U, &sprt_cmds->resp[rsp_cnt]);
            
        } while (--rsp_cnt);

        sprt_cmds->resp[0] = mrt_bit_mask(sprt_cmds->resp[0], 0xffffff00U, 8U);
    }
}

/*!
 * @brief   zynq7_sdmmc_setup_dma
 * @param   none
 * @retval  none
 * @note    setup dma before transfer
 */
static kint32_t zynq7_sdmmc_setup_dma(struct fwk_sdcard_data *sprt_data)
{
    struct fwk_sdcard_host *sprt_host;
    XSdPs *sprt_sd;
    kuint8_t *Buff;
    kuint32_t BlkCnt, BlkSize;

    if (!sprt_data)
        return -ER_NULLPTR;

    sprt_host = (struct fwk_sdcard_host *)sprt_data->ptrHost;
    if (!isValid(sprt_host))
        return -ER_NULLPTR;

    sprt_sd = (XSdPs *)sprt_host->iHostIfBase;
    if (!sprt_sd || !sprt_sd->IsReady)
        return -ER_NREADY;

    Buff = (kuint8_t *)(sprt_data->txBuffer ? sprt_data->txBuffer : sprt_data->rxBuffer);
    if (!Buff)
        return -ER_NOMEM;

    BlkCnt = sprt_data->blockCount;
    BlkSize = sprt_data->blockSize & XSDPS_BLK_SIZE_MASK;

    if (!BlkCnt || !BlkSize)
        return -ER_FAULT;

    /* Write block count register */
    XSdPs_SetBlkSize(sprt_sd, BlkCnt, BlkSize);
    XSdPs_SetupADMA2DescTbl(sprt_sd, BlkCnt, Buff);

    /*!< TransferMode will be used in "send_command" function */
    sprt_sd->TransferMode = XSDPS_TM_DMA_EN_MASK;

    if (mrt_isBitSetl(NR_SdCard_CmdFlagsWithBlock, &sprt_data->flags))
    	sprt_sd->TransferMode |= XSDPS_TM_BLK_CNT_EN_MASK;

    /*!< direction: recv */
    if (!sprt_data->txBuffer)
        sprt_sd->TransferMode |= XSDPS_TM_DAT_DIR_SEL_MASK;

    if (BlkCnt > 1U)
    {
        sprt_sd->TransferMode |= XSDPS_TM_MUL_SIN_BLK_SEL_MASK;

        if (mrt_isBitSetl(NR_SdCard_CmdFlagsAuto12Enable, &sprt_data->flags))
        	sprt_sd->TransferMode |= XSDPS_TM_AUTO_CMD12_EN_MASK;
    }

    if (!sprt_sd->sgrt_cfg.IsCacheCoherent) 
        Xil_DCacheInvalidateRange((kuaddr_t)Buff, BlkSize);

    return ER_NORMAL;
}

/*!
 * @brief   zynq7_sdmmc_transfer_data
 * @param   none
 * @retval  none
 * @note    send data by uSDHC
 */
static kint32_t zynq7_sdmmc_transfer_data(struct fwk_sdcard_data *sprt_data)
{
    struct fwk_sdcard_host *sprt_host;
    XSdPs *sprt_sd;
    kint32_t iRetval;

    if (!sprt_data)
        return -ER_NULLPTR;

    sprt_host = (struct fwk_sdcard_host *)sprt_data->ptrHost;
    if (!isValid(sprt_host))
        return -ER_NULLPTR;

    sprt_sd = (XSdPs *)sprt_host->iHostIfBase;
    if (!sprt_sd || !sprt_sd->IsReady)
        return -ER_NREADY;

    iRetval = sprt_data->txBuffer ? XSdPs_WriteADMA_Data(sprt_sd, sprt_data) : XSdPs_ReadADMA_Data(sprt_sd, sprt_data);
    if (iRetval)
        zynq7_sdmmc_reset(sprt_host);

    return iRetval;
}

/*!
 * @brief   zynq7_sdmmc_switch_voltage
 * @param   none
 * @retval  none
 * @note    switch voltage by uSDHC
 */
static kint32_t zynq7_sdmmc_switch_voltage(struct fwk_sdcard_host *sprt_host, kuint32_t voltage)
{
    XSdPs *sprt_sd = (XSdPs *)sprt_host->iHostIfBase;
    kuint16_t CtrlReg;
    kuint32_t ReadReg, ClockReg;
    kuaddr_t BaseAddress;

    BaseAddress = sprt_sd->sgrt_cfg.BaseAddress;

    /* Wait for CMD and DATA line to go low */
    do {
        ReadReg = XSdPs_ReadReg(BaseAddress, XSDPS_PRES_STATE_OFFSET);

    } while ((ReadReg & (XSDPS_PSR_CMD_SG_LVL_MASK | XSDPS_PSR_DAT30_SG_LVL_MASK)) != 0U);
    
    /* Stop the clock */
    CtrlReg = XSdPs_ReadReg16(BaseAddress, XSDPS_CLK_CTRL_OFFSET);
    CtrlReg &= ~(XSDPS_CC_SD_CLK_EN_MASK | XSDPS_CC_INT_CLK_EN_MASK);
    XSdPs_WriteReg16(BaseAddress, XSDPS_CLK_CTRL_OFFSET, CtrlReg);

    /* Enabling 1.8V in controller */
    CtrlReg = XSdPs_ReadReg16(BaseAddress, XSDPS_HOST_CTRL2_OFFSET);
    CtrlReg |= XSDPS_HC2_1V8_EN_MASK;
    XSdPs_WriteReg16(BaseAddress, XSDPS_HOST_CTRL2_OFFSET, CtrlReg);

    /* Wait minimum 5mSec */
    delay_us(5000U);

    /* Check for 1.8V signal enable bit is cleared by Host */
    CtrlReg = XSdPs_ReadReg16(BaseAddress, XSDPS_HOST_CTRL2_OFFSET);
    if ((CtrlReg & XSDPS_HC2_1V8_EN_MASK) == 0U)
        return -ER_NSUPPORT;

    /* Wait for internal clock to stabilize */
    ClockReg = XSdPs_ReadReg16(BaseAddress, XSDPS_CLK_CTRL_OFFSET);
    XSdPs_WriteReg16(BaseAddress, XSDPS_CLK_CTRL_OFFSET, ClockReg | XSDPS_CC_INT_CLK_EN_MASK);

    do {
        ClockReg = XSdPs_ReadReg16(BaseAddress, XSDPS_CLK_CTRL_OFFSET);   
    } while((ClockReg & XSDPS_CC_INT_CLK_STABLE_MASK) == 0U);

    /* Enable SD clock */
    ClockReg = XSdPs_ReadReg16(BaseAddress, XSDPS_CLK_CTRL_OFFSET);
    XSdPs_WriteReg16(BaseAddress, XSDPS_CLK_CTRL_OFFSET, ClockReg | XSDPS_CC_SD_CLK_EN_MASK);

    /* Wait for 1mSec */
    delay_us(1000U);

    /* Wait for CMD and DATA line to go high */
    do {
        ReadReg = XSdPs_ReadReg(BaseAddress, XSDPS_PRES_STATE_OFFSET);
    } while ((ReadReg & (XSDPS_PSR_CMD_SG_LVL_MASK | XSDPS_PSR_DAT30_SG_LVL_MASK))
            != (XSDPS_PSR_CMD_SG_LVL_MASK | XSDPS_PSR_DAT30_SG_LVL_MASK));

    return ER_NORMAL;
}

/*!
 * @brief   host_sdmmc_card_initial
 * @param   none
 * @retval  none
 * @note    initial host of SD Card
 */
void *host_sdmmc_card_initial(struct fwk_sdcard *sprt_card)
{
    XSdPs *sprt_sd;
    struct fwk_sdcard_if *sprt_if;
    struct fwk_sdcard_host *sprt_host;

    if (!isValid(sprt_card))
        return mrt_nullptr;

    sprt_sd = &sgrt_xsd_ps_data;
    sprt_if = &sprt_card->sgrt_if;

    sprt_host = (struct fwk_sdcard_host *)kzalloc(sizeof(struct fwk_sdcard_host), GFP_KERNEL);
    if (!isValid(sprt_host))
        return mrt_nullptr;

    sprt_host->iHostIfBase = (kuaddr_t)sprt_sd;
    sprt_host->iHostCDBase = (kuaddr_t)sprt_sd;
    sprt_host->isSelfDync = true;

    /*!< Default settings */
    sprt_sd->BusWidth = XSDPS_1_BIT_WIDTH;
    sprt_sd->CardType = XSDPS_CARD_SD;
    sprt_sd->Switch1v8 = 0U;
    sprt_sd->BusSpeed = XSDPS_CLK_400_KHZ;

    /*!< get support */
    sprt_host->maxBlockLength = mrt_getbitw(XSDPS_BLK_SIZE_MASK, sprt_sd->sgrt_cfg.BaseAddress + XSDPS_BLK_SIZE_OFFSET);
    sprt_host->maxBlockCount  = 0xffffU;

    mrt_resetl(&sprt_host->flagBit);
    if (!mrt_isBitResetl(XSDPS_CAP_VOLT_1V8_MASK, &sprt_sd->Host_Caps))
        mrt_setbitl(NR_SdCard_SupportVoltage1_8V, &sprt_host->flagBit);

    mrt_setbitl(NR_SdCard_Support4BitWidth, &sprt_host->flagBit);

    /*!< interface initial */
    sprt_if->is_insert      = zynq7_sdmmc_is_card_insert;
    sprt_if->setBusWidth    = zynq7_sdmmc_set_bus_width;
    sprt_if->setClkFreq     = zynq7_sdmmc_set_clk_freq;
    sprt_if->cardActive     = zynq7_sdmmc_initial_active;
    sprt_if->sendCommand    = zynq7_sdmmc_send_command;
    sprt_if->recvResp       = zynq7_sdmmc_recv_response;
    sprt_if->sendData       = zynq7_sdmmc_transfer_data;
    sprt_if->recvData       = zynq7_sdmmc_transfer_data;
    sprt_if->switchVoltage  = zynq7_sdmmc_switch_voltage;
    sprt_if->addHeadTail    = mrt_nullptr;
    sprt_if->setup_dma      = zynq7_sdmmc_setup_dma;

    if ((sprt_sd->HC_Version == XSDPS_HC_SPEC_V3) &&
        ((sprt_sd->Host_Caps & XSDPS_CAPS_SLOT_TYPE_MASK) == XSDPS_CAPS_EMB_SLOT))
        sprt_sd->CardType = XSDPS_CHIP_EMMC;

    zynq7_sdmmc_reset(sprt_host);

    return sprt_host;
}

/* end of file*/
