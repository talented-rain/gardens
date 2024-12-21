/*
 * General SD Card Protocol
 *
 * File Name:   fwk_sdcard.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.10.09
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <common/time.h>
#include <platform/fwk_mempool.h>
#include <platform/mmc/fwk_sdcard.h>

/*!< The defines */
enum __ERT_SDCARD_RW_CTRL
{
    NR_SdCard_RWCtrlGet = 0U,
    NR_SdCard_RWCtrlExcute,
};

/*!< command callback */
typedef kint32_t (*frt_fwk_sdcard_control_t)(struct fwk_sdcard_cmd *, struct fwk_sdcard_if *, kuint32_t);

/*!< The function */
/*!< command transfer controller: as callback for "frt_fwk_sdcard_control_t" */
static kint32_t fwk_sdcard_command_go_idle(struct fwk_sdcard_cmd *sprt_cmd, struct fwk_sdcard_if *sprt_if, kuint32_t flags);
static kint32_t fwk_sdcard_command_send_if_cond(struct fwk_sdcard_cmd *sprt_cmd, struct fwk_sdcard_if *sprt_if, kuint32_t flags);
static kint32_t fwk_sdcard_command_app_before(struct fwk_sdcard_cmd *sprt_cmd, struct fwk_sdcard_if *sprt_if, kuint32_t flags);
static kint32_t fwk_sdcard_command_operation_cond(struct fwk_sdcard_cmd *sprt_cmd, struct fwk_sdcard_if *sprt_if, kuint32_t flags);
static kint32_t fwk_sdcard_command_get_cid(struct fwk_sdcard_cmd *sprt_cmd, struct fwk_sdcard_if *sprt_if, kuint32_t flags);
static kint32_t fwk_sdcard_command_get_csd(struct fwk_sdcard_cmd *sprt_cmd, struct fwk_sdcard_if *sprt_if, kuint32_t flags);
static kint32_t fwk_sdcard_command_get_rca(struct fwk_sdcard_cmd *sprt_cmd, struct fwk_sdcard_if *sprt_if, kuint32_t flags);
static kint32_t fwk_sdcard_command_get_scr(struct fwk_sdcard_cmd *sprt_cmd, struct fwk_sdcard_if *sprt_if, kuint32_t flags);
static kint32_t fwk_sdcard_command_select_card(struct fwk_sdcard_cmd *sprt_cmd, struct fwk_sdcard_if *sprt_if, kuint32_t flags);
static kint32_t fwk_sdcard_command_get_status(struct fwk_sdcard_cmd *sprt_cmd, struct fwk_sdcard_if *sprt_if, kuint32_t flags);
static kint32_t fwk_sdcard_command_stop_transmission(struct fwk_sdcard_cmd *sprt_cmd, struct fwk_sdcard_if *sprt_if, kuint32_t flags);
static kint32_t fwk_sdcard_command_switch_voltage(struct fwk_sdcard_cmd *sprt_cmd, struct fwk_sdcard_if *sprt_if, kuint32_t flags);
static kint32_t fwk_sdcard_command_set_buswidth(struct fwk_sdcard_cmd *sprt_cmd, struct fwk_sdcard_if *sprt_if, kuint32_t flags);
static kint32_t fwk_sdcard_command_set_blocksize(struct fwk_sdcard_cmd *sprt_cmd, struct fwk_sdcard_if *sprt_if, kuint32_t flags);
static kint32_t fwk_sdcard_command_pullup(struct fwk_sdcard_cmd *sprt_cmd, struct fwk_sdcard_if *sprt_if, kuint32_t flags);

static kint32_t fwk_sdcard_command_ctrl_function(struct fwk_sdcard_cmd *sprt_cmd, struct fwk_sdcard_if *sprt_if, 
                                            kuint32_t mode, kuint32_t group, kuint32_t funcIndex);
static kint32_t fwk_sdcard_command_select_function(struct fwk_sdcard *sprt_card, kuint32_t group, kuint32_t funcIndex);

static kint32_t fwk_sdcard_check_interface(struct fwk_sdcard_if *sprt_if);
static kint32_t fwk_sdcard_send_command(struct fwk_sdcard *sprt_card, frt_fwk_sdcard_control_t cmd_controller, kuint32_t args);
static kint32_t fwk_sdcard_send_app_command(struct fwk_sdcard *sprt_card, frt_fwk_sdcard_control_t cmd_controller, kuint32_t args);
static kubyte_t fwk_sdcard_calc_crc7(kubyte_t *chptr, kuint32_t lenth);
static void fwk_sdcard_add_prefix_suffix(struct fwk_sdcard_cmd *sprt_cmd);

static kint32_t fwk_sdcard_send_rw_command(struct fwk_sdcard_cmd *sprt_cmd, struct fwk_sdcard_if *sprt_if, 
                                    kuint32_t index, kuint32_t flags, kuint32_t errbit, kbool_t ctrl);
static kint32_t fwk_sdcard_read_blocks(struct fwk_sdcard *sprt_card, void *ptrBuffer, 
                                    kuint32_t iBlockStart, kuint32_t iBlockCount, kusize_t iBlockSize);
static kint32_t fwk_sdcard_write_blocks(struct fwk_sdcard *sprt_card, void *ptrBuffer, 
                                    kuint32_t iBlockStart, kuint32_t iBlockCount, kusize_t iBlockSize);
static kint32_t fwk_sdcard_erase_blocks(struct fwk_sdcard *sprt_card, kuint32_t iBlockStart, kuint32_t iBlockCount, kusize_t iBlockSize);

/*!< API function */
/*!
 * @brief   fwk_sdcard_check_interface
 * @param   none
 * @retval  none
 * @note    Check if interface is null
 */
static kint32_t fwk_sdcard_check_interface(struct fwk_sdcard_if *sprt_if)
{
    kbool_t blRetval = true;

#define mrt_fwk_sdcard_check_interface(x)   \
    do {   \
        blRetval &= (!!(x));   \
    } while (0)

    mrt_fwk_sdcard_check_interface(sprt_if);
    mrt_fwk_sdcard_check_interface(sprt_if->sprt_host);
    mrt_fwk_sdcard_check_interface(sprt_if->sprt_host->iHostIfBase);
    mrt_fwk_sdcard_check_interface(sprt_if->sprt_host->iHostCDBase);

    mrt_fwk_sdcard_check_interface(sprt_if->is_insert);
    mrt_fwk_sdcard_check_interface(sprt_if->setBusWidth);
    mrt_fwk_sdcard_check_interface(sprt_if->setClkFreq);
    mrt_fwk_sdcard_check_interface(sprt_if->cardActive);
    mrt_fwk_sdcard_check_interface(sprt_if->recvData);
    mrt_fwk_sdcard_check_interface(sprt_if->sendData);
    mrt_fwk_sdcard_check_interface(sprt_if->sendCommand);
    mrt_fwk_sdcard_check_interface(sprt_if->recvResp);

#undef mrt_fwk_sdcard_check_interface

    return blRetval ? ER_NORMAL : (-ER_ERROR);
}

/*!
 * @brief   fwk_sdcard_command_go_idle
 * @param   flags: none
 * @retval  none
 * @note    CMD0 Controller
 */
static kint32_t fwk_sdcard_command_go_idle(struct fwk_sdcard_cmd *sprt_cmd, struct fwk_sdcard_if *sprt_if, kuint32_t flags)
{
    kuint32_t iRespType;

    if (!sprt_cmd)
        return -ER_NULLPTR;

    if (!sprt_if)
        return fwk_sdcard_initial_command(sprt_cmd, NR_SdCard_Cmd_GoIdle, FWK_SDCARD_CMD_NONE_ARGS, NR_SdCard_Response_0);

    /*!< read all response register for initial */
    iRespType = sprt_cmd->respType;
    sprt_cmd->respType = NR_SdCard_Response_2;
    sprt_if->recvResp(sprt_cmd);
    sprt_cmd->respType = iRespType;

    /*!< send command packet */
    return sprt_if->sendCommand(sprt_cmd);
}

/*!
 * @brief   fwk_sdcard_command_send_if_cond
 * @param   flags: Voltage supply
 * @retval  none
 * @note    CMD8 Controller
 */
static kint32_t fwk_sdcard_command_send_if_cond(struct fwk_sdcard_cmd *sprt_cmd, struct fwk_sdcard_if *sprt_if, kuint32_t flags)
{
    kuint32_t respValue;
    kint32_t iRetval;

    if (!sprt_cmd)
        return -ER_NULLPTR;

    if (!sprt_if)
        return fwk_sdcard_initial_command(sprt_cmd, NR_SdCard_Cmd_SendIfCond, FWK_SDCARD_CMD8_ARGS(flags), NR_SdCard_Response_7);

    mrt_run_code_retry(10U,

        /*!< send command packet */
        iRetval = sprt_if->sendCommand(sprt_cmd);
        if (!(iRetval < 0))
        {
            sprt_if->recvResp(sprt_cmd);
            respValue = mrt_bit_mask(*sprt_cmd->resp, FWK_SDCARD_CMD_CHECK_MASK, 0);

            if (!mrt_isBitSetl(FWK_SDCARD_CMD_CHECK_PATTERN, &respValue))
                iRetval = -ER_CHECKERR;

            break;
        }
    )

    return iRetval;
}

/*!
 * @brief   fwk_sdcard_command_app_before
 * @param   flags: relative address
 * @retval  none
 * @note    CMD55 Controller
 */
static kint32_t fwk_sdcard_command_app_before(struct fwk_sdcard_cmd *sprt_cmd, struct fwk_sdcard_if *sprt_if, kuint32_t flags)
{
    kint32_t iRetval;

    if (!sprt_cmd)
        return -ER_NULLPTR;

    if (!sprt_if)
        return fwk_sdcard_initial_command(sprt_cmd, NR_SdCard_Cmd_AppCmd, mrt_bit_nr(flags, 16U), NR_SdCard_Response_1);

    /*!< send command package */
    iRetval = sprt_if->sendCommand(sprt_cmd);
    if (iRetval)
        return iRetval;
    
    sprt_if->recvResp(sprt_cmd);

    /*!< if response have error bit ? */
    if (!mrt_isBitResetl(NR_SdCard_R1_AllErrBit, &sprt_cmd->resp[0]))
        return -ER_SCMD_FAILD;

    /*!< if not support application command ? */
    if (!mrt_isBitSetl(NR_SdCard_R1_AppCmdEnabled_Bit, &sprt_cmd->resp[0]))
        return -ER_NSUPPORT;

    return iRetval;
}

/*!
 * @brief   fwk_sdcard_command_operation_cond
 * @param   flags: Voltage supply
 * @retval  none
 * @note    CMD41 Controller
 */
static kint32_t fwk_sdcard_command_operation_cond(struct fwk_sdcard_cmd *sprt_cmd, struct fwk_sdcard_if *sprt_if, kuint32_t flags)
{
    struct fwk_sdcard *sprt_card;
    kint32_t iRetval;

    if (!sprt_cmd)
        return -ER_NULLPTR;

    if (!sprt_if)
        return fwk_sdcard_initial_command(sprt_cmd, NR_SdCard_ACmd_GetOpCond, flags, NR_SdCard_Response_3);

    /*!< get card structure */
    sprt_card = mrt_container_of(sprt_if, struct fwk_sdcard, sgrt_if);

	/*!< send command package */
	iRetval = sprt_if->sendCommand(sprt_cmd);
	if (iRetval)
		return iRetval;

	sprt_if->recvResp(sprt_cmd);

	if (mrt_isBitSetl(NR_SdCard_OcrNotBusy_Bit, &sprt_cmd->resp[0]))
	{
		sprt_card->ocr = sprt_cmd->resp[0];
		return ER_NORMAL;
	}

    return -ER_RETRY;
}

/*
 * @brief   fwk_sdcard_command_get_cid
 * @param   flags: none
 * @retval  none
 * @note    CMD10 Controller
 */
static kint32_t fwk_sdcard_command_get_cid(struct fwk_sdcard_cmd *sprt_cmd, struct fwk_sdcard_if *sprt_if, kuint32_t flags)
{
    struct fwk_sdcard *sprt_card;
    kint32_t iRetval;

    if (!sprt_cmd)
        return -ER_NULLPTR;

    if (!sprt_if)
        return fwk_sdcard_initial_command(sprt_cmd, NR_SdCard_Cmd_AllSendCID, FWK_SDCARD_CMD_NONE_ARGS, NR_SdCard_Response_2);

    /*!< get card structure */
    sprt_card = mrt_container_of(sprt_if, struct fwk_sdcard, sgrt_if);

    /*!< send command package */
    iRetval = sprt_if->sendCommand(sprt_cmd);
    if (iRetval)
        return iRetval;
    
    sprt_if->recvResp(sprt_cmd);

    /*!< update cid */
    sprt_card->sgrt_cid.manufacturerID = (kuint8_t)(sprt_cmd->resp[3] >> 24U);
    sprt_card->sgrt_cid.applicationID = (kuint16_t)(sprt_cmd->resp[3] >> 8U);
    sprt_card->sgrt_cid.productName[0] = (kuint8_t)(sprt_cmd->resp[3]);
    sprt_card->sgrt_cid.productName[1] = (kuint8_t)(sprt_cmd->resp[2] >> 24U);
    sprt_card->sgrt_cid.productName[2] = (kuint8_t)(sprt_cmd->resp[2] >> 16U);
    sprt_card->sgrt_cid.productName[3] = (kuint8_t)(sprt_cmd->resp[2] >> 8U);
    sprt_card->sgrt_cid.productName[4] = (kuint8_t)(sprt_cmd->resp[2]);
    sprt_card->sgrt_cid.productVersion = (kuint8_t)(sprt_cmd->resp[1] >> 24U);
    sprt_card->sgrt_cid.productSerialNumber  = (kuint32_t)((sprt_cmd->resp[1] << 8U)  & 0xffffff00U);
    sprt_card->sgrt_cid.productSerialNumber |= (kuint32_t)((sprt_cmd->resp[0] >> 24U) & 0x000000ffU);
    sprt_card->sgrt_cid.manufacturerData = (kuint16_t)(sprt_cmd->resp[0] >> 8U);
    
    return iRetval;
}

/*
 * @brief   fwk_sdcard_command_get_csd
 * @param   flags: none
 * @retval  none
 * @note    CMD9 Controller
 */
static kint32_t fwk_sdcard_command_get_csd(struct fwk_sdcard_cmd *sprt_cmd, struct fwk_sdcard_if *sprt_if, kuint32_t flags)
{
    struct fwk_sdcard *sprt_card;
    kint32_t iRetval;

    if (!sprt_cmd)
        return -ER_NULLPTR;

    if (!sprt_if)
        return fwk_sdcard_initial_command(sprt_cmd, NR_SdCard_Cmd_GetCSD, FWK_SDCARD_CMD_RCA_ARGS(flags), NR_SdCard_Response_2);

    /*!< get card structure */
    sprt_card = mrt_container_of(sprt_if, struct fwk_sdcard, sgrt_if);

    /*!< send command package */
    iRetval = sprt_if->sendCommand(sprt_cmd);
    if (iRetval)
        return iRetval;
    
    sprt_if->recvResp(sprt_cmd);

    /*!< update csd */
    sprt_card->sgrt_csd.csdStructure = (kuint8_t)(sprt_cmd->resp[3] >> 30U);
    sprt_card->sgrt_csd.dataReadAccessTime1 = (kuint8_t)(sprt_cmd->resp[3] >> 16U);
    sprt_card->sgrt_csd.dataReadAccessTime2 = (kuint8_t)(sprt_cmd->resp[3] >> 8U);
    sprt_card->sgrt_csd.transferSpeed = (kuint8_t)(sprt_cmd->resp[3]);
    sprt_card->sgrt_csd.cardCommandClass = (kuint16_t)(sprt_cmd->resp[2] >> 20U);
    sprt_card->sgrt_csd.readBlockLength = (kuint8_t)((sprt_cmd->resp[2] >> 16U) & 0x0fU);

    /*!< It is different for Card 1.0(SDSC) and 2.0(SDHC/SDXC) */
    switch (sprt_card->sgrt_csd.csdStructure)
    {
        /*!< Version 1.0 */
        case 0U:
            sprt_card->sgrt_csd.deviceSize  = (kuint32_t)((sprt_cmd->resp[2] << 2U)  & 0x000000fcU);
            sprt_card->sgrt_csd.deviceSize |= (kuint32_t)((sprt_cmd->resp[1] >> 30U) & 0x00000003U);
            sprt_card->sgrt_csd.readCurrentVddMin    = (kuint8_t)((sprt_cmd->resp[1] >> 27U) & 0x07U);
            sprt_card->sgrt_csd.readCurrentVddMax    = (kuint8_t)((sprt_cmd->resp[1] >> 24U) & 0x07U);
            sprt_card->sgrt_csd.writeCurrentVddMin   = (kuint8_t)((sprt_cmd->resp[1] >> 21U) & 0x07U);
            sprt_card->sgrt_csd.writeCurrentVddMax   = (kuint8_t)((sprt_cmd->resp[1] >> 18U) & 0x07U);
            sprt_card->sgrt_csd.deviceSizeMultiplier = (kuint8_t)((sprt_cmd->resp[1] >> 15U) & 0x07U);

            /*!<
                * memory capacity = block_count * block_len
                * block_count = (deviceSize + 1) * 2 ^ (deviceSizeMultiplier + 2)
                * block_len = 2 ^ (readBlockLength)
                * 
                * ===> capacity = (((deviceSize + 1) << (deviceSizeMultiplier + 2)) << readBlockLength) (bytes)
                */
            sprt_card->blockCount = mrt_bit_nr(sprt_card->sgrt_csd.deviceSize + 1, (sprt_card->sgrt_csd.deviceSizeMultiplier + 2));
            sprt_card->blockSize = mrt_bit(sprt_card->sgrt_csd.readBlockLength);

            if (FWK_SDCARD_DEFAULT_BLOCKSIZE != sprt_card->blockSize)
            {
                /*!< do nothing */
            }

            sprt_card->capacity = sprt_card->blockCount << sprt_card->sgrt_csd.readBlockLength;
            break;

        /*!< Version 2.0 */
        case 1U:
            sprt_card->sgrt_csd.deviceSize  = (kuint32_t)((sprt_cmd->resp[2] << 16U) & 0x00003f00U);
            sprt_card->sgrt_csd.deviceSize |= (kuint32_t)((sprt_cmd->resp[1] >> 16U) & 0x000000ffU);

            /*!<
                * memory capacity = (deviceSize + 1) * 1024 * 512bytes
                *
                * The Minimum user area size of SDHC is 4211712 sectors (2GB + 8.5MB)
                * The Minimum value of deviceSize for SDHC in CSD version 2.0 is 0x1010
                * The Maximum user area size of SDHC Card is (32GB + 80MB)
                * The Maximum value of deviceSize for SDHC in CSD version 2.0 is 0xff5f
                */
            sprt_card->blockCount = (sprt_card->sgrt_csd.deviceSize + 1) << 10U;
            sprt_card->blockSize = FWK_SDCARD_DEFAULT_BLOCKSIZE;
            sprt_card->capacity = sprt_card->blockCount << 9;
            break;

        default: break;
    }

    sprt_card->sgrt_csd.eraseSingleBlockEnable = (kuint8_t)((sprt_cmd->resp[1] >> 8U) & 0x01U);
    sprt_card->sgrt_csd.eraseSectorSize = (kuint8_t)((sprt_cmd->resp[1] >> 7U) & 0x7fU);
    sprt_card->sgrt_csd.writeProtectGroupSize = (kuint8_t)(sprt_cmd->resp[1] & 0x7fU);
    sprt_card->sgrt_csd.writeSpeedFactor = (kuint8_t)((sprt_cmd->resp[0] >> 26U) & 0x07U);
    sprt_card->sgrt_csd.writeBlockLength = (kuint8_t)((sprt_cmd->resp[0] >> 22U) & 0x0fU);
    sprt_card->sgrt_csd.fileFormat = (kuint8_t)((sprt_cmd->resp[0] >> 10U) & 0x03U);

#define mrt_fwk_sdcard_csd_setflagbit(resp, resp_bit, flagbit)  \
    do {    \
        if (mrt_isBitSetl(mrt_bit(resp_bit), resp)) \
            mrt_setbitl(flagbit, &sprt_card->sgrt_csd.flagBit);  \
    } while (0)

    /*!< fill the flagBit field */
    mrt_resetl(&sprt_card->sgrt_csd.flagBit);
    mrt_fwk_sdcard_csd_setflagbit(&sprt_cmd->resp[0], 12U, NR_SdCard_CsdTempWriteProtect_Bit);
    mrt_fwk_sdcard_csd_setflagbit(&sprt_cmd->resp[0], 13U, NR_SdCard_CsdPermWriteProtect_Bit);
    mrt_fwk_sdcard_csd_setflagbit(&sprt_cmd->resp[0], 14U, NR_SdCard_CsdCopyFlag_Bit);
    mrt_fwk_sdcard_csd_setflagbit(&sprt_cmd->resp[0], 15U, NR_SdCard_CsdFileFormatGroup_Bit);
    mrt_fwk_sdcard_csd_setflagbit(&sprt_cmd->resp[0], 21U, NR_SdCard_CsdPartialBlockWrite_Bit);
    mrt_fwk_sdcard_csd_setflagbit(&sprt_cmd->resp[0], 31U, NR_SdCard_CsdWriteProtectEnable_Bit);
    mrt_fwk_sdcard_csd_setflagbit(&sprt_cmd->resp[1], 14U, NR_SdCard_CsdEraseBlockEnale_Bit);
    mrt_fwk_sdcard_csd_setflagbit(&sprt_cmd->resp[2], 12U, NR_SdCard_CsdDsrImplemented_Bit);
    mrt_fwk_sdcard_csd_setflagbit(&sprt_cmd->resp[2], 13U, NR_SdCard_CsdReadBlockMisalign_Bit);
    mrt_fwk_sdcard_csd_setflagbit(&sprt_cmd->resp[2], 14U, NR_SdCard_CsdWriteBlockMisalign_Bit);
    mrt_fwk_sdcard_csd_setflagbit(&sprt_cmd->resp[2], 15U, NR_SdCard_CsdPartialBlockRead_Bit);

#undef mrt_fwk_sdcard_csd_setflagbit

    return iRetval;
}

/*
 * @brief   fwk_sdcard_command_get_rca
 * @param   flags: none
 * @retval  none
 * @note    CMD3 Controller
 */
static kint32_t fwk_sdcard_command_get_rca(struct fwk_sdcard_cmd *sprt_cmd, struct fwk_sdcard_if *sprt_if, kuint32_t flags)
{
    struct fwk_sdcard *sprt_card;
    kint32_t iRetval;

    if (!sprt_cmd)
        return -ER_NULLPTR;

    if (!sprt_if)
        return fwk_sdcard_initial_command(sprt_cmd, NR_SdCard_Cmd_IssueRCA, FWK_SDCARD_CMD_NONE_ARGS, NR_SdCard_Response_6);

    /*!< get card structure */
    sprt_card = mrt_container_of(sprt_if, struct fwk_sdcard, sgrt_if);

    /*!< send command package */
    iRetval = sprt_if->sendCommand(sprt_cmd);
    if (iRetval)
        return iRetval;
    
    sprt_if->recvResp(sprt_cmd);

    /*!< update rca: get relative address */
    /*!< bit[15:0] is card status; bit[31:16] is RCA Register */
    sprt_card->relativeAddr = sprt_cmd->resp[0] >> 16U;

    return iRetval;
}

/*
 * @brief   fwk_sdcard_command_get_scr
 * @param   flags: none
 * @retval  none
 * @note    CMD51 Controller
 */
static kint32_t fwk_sdcard_command_get_scr(struct fwk_sdcard_cmd *sprt_cmd, struct fwk_sdcard_if *sprt_if, kuint32_t flags)
{
    struct fwk_sdcard *sprt_card;
    struct fwk_sdcard_data sgrt_data;
    kuint32_t *ptrDataBuffer, iDataBufferSize;
    kint32_t iRetval;

    if (!sprt_cmd)
        return -ER_NULLPTR;

    if (!sprt_if)
        return fwk_sdcard_initial_command(sprt_cmd, NR_SdCard_ACmd_GetSCR, FWK_SDCARD_CMD_NONE_ARGS, NR_SdCard_Response_1);
    
    sgrt_data.blockSize = 8U;
    sgrt_data.blockCount = 1U;

    /*!< tell host that data will be transferred after sending commond */
    mrt_resetl(&sgrt_data.flags);
    mrt_setbitl(NR_SdCard_CmdFlagsReadEnable, &sgrt_data.flags);

    /*!< sizeof(iScrDataBuffer) must greater than blockCount * align4(blockSize) */
    iDataBufferSize = mrt_num_align4(sgrt_data.blockSize) * sgrt_data.blockCount + 8U;

    /*!< 8U is reserved */
    ptrDataBuffer = (kuint32_t *)kzalloc(iDataBufferSize, GFP_KERNEL);
    if (!isValid(ptrDataBuffer))
        return -ER_NOMEM;

    sgrt_data.ptrHost = sprt_cmd->ptrHost;
    sgrt_data.txBuffer = mrt_nullptr;
    sgrt_data.rxBuffer = ptrDataBuffer;

    sprt_cmd->ptrData = &sgrt_data;
    if (sprt_if->setup_dma)
        sprt_if->setup_dma(&sgrt_data);

    /*!< send command package */
    iRetval = sprt_if->sendCommand(sprt_cmd);
    if (iRetval)
        goto END;

    sprt_if->recvResp(sprt_cmd);
    iRetval = mrt_isBitResetl(NR_SdCard_R1_AllErrBit, &sprt_cmd->resp[0]) ? ER_NORMAL : -ER_ERROR;

    /*!< receive data from data line */
    if (iRetval)
        goto END;
    
    /*!< start to receive data */
    iRetval = sprt_if->recvData(&sgrt_data);
    if (iRetval)
    {
        iRetval = ((-ER_BUSY) == iRetval) || ((-ER_TIMEOUT) == iRetval) ? -ER_RETRY : iRetval;
        goto END;
    }
    
    /*!< data line: card to host is MSB first, Big Endian */
    /*!< if host is little endian, it needs to convert */
    sgrt_data.rxBuffer[0] = mrt_be32_to_cpu(sgrt_data.rxBuffer[0]);
    sgrt_data.rxBuffer[1] = mrt_be32_to_cpu(sgrt_data.rxBuffer[1]);

    /*!< get card structure */
    sprt_card = mrt_container_of(sprt_if, struct fwk_sdcard, sgrt_if);

    /*!< fill scr */
    sprt_card->sgrt_scr.scrStructure = (kuint8_t)((sgrt_data.rxBuffer[0] >> 28U) & 0x0fU);
    sprt_card->sgrt_scr.sdSpecification = (kuint8_t)((sgrt_data.rxBuffer[0] >> 24U) & 0x0fU);
    sprt_card->sgrt_scr.sdSecurity = (kuint8_t)((sgrt_data.rxBuffer[0] >> 20U) & 0x07U);
    sprt_card->sgrt_scr.sdBusWidths = (kuint8_t)((sgrt_data.rxBuffer[0] >> 16U) & 0x0fU);
    sprt_card->sgrt_scr.extendedSecurity = (kuint8_t)((sgrt_data.rxBuffer[0] >> 11U) & 0x0fU);
    sprt_card->sgrt_scr.commandSupport = (kuint8_t)(sgrt_data.rxBuffer[0] & 0x03U);
    sprt_card->sgrt_scr.reservedForManufacturer = sgrt_data.rxBuffer[1];
    
#define mrt_fwk_sdcard_scr_setflagbit(recv, recv_bit, flagbit)  \
    do {    \
        if (mrt_isBitSetl(mrt_bit(recv_bit), recv)) \
            mrt_setbitl(flagbit, &sprt_card->sgrt_scr.flagBit);  \
    } while (0)

    /*!< fill the flagBit field */
    mrt_resetl(&sprt_card->sgrt_scr.flagBit);
    mrt_fwk_sdcard_scr_setflagbit(&sgrt_data.rxBuffer[0], 15U, NR_SdCard_ScrSdSpecification3);
    mrt_fwk_sdcard_scr_setflagbit(&sgrt_data.rxBuffer[0], 23U, NR_SdCard_ScrDataStatusAfterErase);

#undef mrt_fwk_sdcard_scr_setflagbit

    switch (sprt_card->sgrt_scr.sdSpecification)
    {
        case 0U:
            sprt_card->version = NR_SdCard_Version1_0;
            break;

        case 1U:
            sprt_card->version = NR_SdCard_Version1_1;
            break;

        case 2U:
            sprt_card->version = (mrt_isBitSetl(NR_SdCard_ScrSdSpecification3, &sprt_card->sgrt_scr.flagBit) ? 
                                                                NR_SdCard_Version3_0 : NR_SdCard_Version2_0);
            break;

        default: break;
    }

END:
    kfree(ptrDataBuffer);
    return iRetval;
}

/*
 * @brief   fwk_sdcard_command_select_card
 * @param   flags: relative address
 * @retval  none
 * @note    CMD7 Controller
 */
static kint32_t fwk_sdcard_command_select_card(struct fwk_sdcard_cmd *sprt_cmd, struct fwk_sdcard_if *sprt_if, kuint32_t flags)
{
    kint32_t iRetval;

    if (!sprt_cmd)
        return -ER_NULLPTR;

    if (!sprt_if)
        return fwk_sdcard_initial_command(sprt_cmd, NR_SdCard_Cmd_SelectCard, FWK_SDCARD_CMD_RCA_ARGS(flags), 
                                                        flags ? NR_SdCard_Response_1 : NR_SdCard_Response_0);

    /*!< send command package */
    iRetval = sprt_if->sendCommand(sprt_cmd);
    if (iRetval)
        return iRetval;
    
    sprt_if->recvResp(sprt_cmd);

    /*!< check R1 Status */
    if (NR_SdCard_Response_1 == sprt_cmd->respType)
        iRetval = mrt_isBitResetl(NR_SdCard_R1_AllErrBit, &sprt_cmd->resp[0]) ? ER_NORMAL : -ER_ERROR;

    return iRetval;
}

/*
 * @brief   fwk_sdcard_command_get_status
 * @param   flags: relative address
 * @retval  none
 * @note    CMD13 Controller
 */
static kint32_t fwk_sdcard_command_get_status(struct fwk_sdcard_cmd *sprt_cmd, struct fwk_sdcard_if *sprt_if, kuint32_t flags)
{
    kbool_t blRetval;
    kint32_t iRetval;

    if (!sprt_cmd)
        return -ER_NULLPTR;

    if (!sprt_if)
        return fwk_sdcard_initial_command(sprt_cmd, NR_SdCard_Cmd_GetStatus, FWK_SDCARD_CMD_RCA_ARGS(flags), NR_SdCard_Response_1);

    do
    {
        /*!< send command package */
        iRetval = sprt_if->sendCommand(sprt_cmd);
        if (iRetval)
            continue;

        sprt_if->recvResp(sprt_cmd);

        /*!< check R1 Status */
        blRetval  = mrt_isBitResetl(NR_SdCard_R1_AllErrBit, &sprt_cmd->resp[0]);
        blRetval &= mrt_isBitSetl(NR_SdCard_R1_ReadyForData_Bit, &sprt_cmd->resp[0]);
        blRetval &= !mrt_isBitSetl(NR_SdCard_R1_StateProgram_Bit, &sprt_cmd->resp[0]);

        iRetval = blRetval ? ER_NORMAL : (-ER_ERROR);

    } while (iRetval);

    return iRetval;
}

/*
 * @brief   fwk_sdcard_command_stop_transmission
 * @param   flags: relative address
 * @retval  none
 * @note    CMD12 Controller
 */
static kint32_t fwk_sdcard_command_stop_transmission(struct fwk_sdcard_cmd *sprt_cmd, struct fwk_sdcard_if *sprt_if, kuint32_t flags)
{
    kint32_t iRetval;

    if (!sprt_cmd)
        return -ER_NULLPTR;

    if (!sprt_if)
        return fwk_sdcard_initial_command(sprt_cmd, NR_SdCard_Cmd_StopTrans, FWK_SDCARD_CMD_NONE_ARGS, NR_SdCard_Response_1b);

    do
    {
        /*!< send command package */
        iRetval = sprt_if->sendCommand(sprt_cmd);
        if (iRetval)
            continue;
        
        sprt_if->recvResp(sprt_cmd);
        /*!< check R1 Status */


    } while (iRetval);

    return iRetval;
}

/*
 * @brief   fwk_sdcard_command_switch_voltage
 * @param   flags: none
 * @retval  none
 * @note    CMD11 Controller
 */
static kint32_t fwk_sdcard_command_switch_voltage(struct fwk_sdcard_cmd *sprt_cmd, struct fwk_sdcard_if *sprt_if, kuint32_t flags)
{
    kint32_t iRetval;

    if (!sprt_cmd)
        return -ER_NULLPTR;

    if (!sprt_if)
        return fwk_sdcard_initial_command(sprt_cmd, NR_SdCard_Cmd_SwitchVoltage, FWK_SDCARD_CMD_NONE_ARGS, NR_SdCard_Response_1);

    mrt_assert(sprt_if->switchVoltage);

    /*!< send command package */
    iRetval = sprt_if->sendCommand(sprt_cmd);
    if (iRetval)
        return iRetval;

    sprt_if->recvResp(sprt_cmd);

    /*!< check R1 Status */
    if (mrt_isBitResetl(NR_SdCard_R1_AllErrBit, &sprt_cmd->resp[0]))
        iRetval = sprt_if->switchVoltage(sprt_cmd->ptrHost, NR_SdCard_toVoltage1_8V);
    else
        iRetval = -ER_ERROR;

    return iRetval;
}

/*
 * @brief   fwk_sdcard_command_set_buswidth
 * @param   flags: bus width
 * @retval  none
 * @note    ACMD6 Controller
 */
static kint32_t fwk_sdcard_command_set_buswidth(struct fwk_sdcard_cmd *sprt_cmd, struct fwk_sdcard_if *sprt_if, kuint32_t flags)
{
    kint32_t iRetval;

    if (!sprt_cmd)
        return -ER_NULLPTR;

    if (!sprt_if)
        return fwk_sdcard_initial_command(sprt_cmd, NR_SdCard_ACmd_SetBusWidth, flags, NR_SdCard_Response_1);

    /*!< send command package */
    iRetval = sprt_if->sendCommand(sprt_cmd);
    if (iRetval)
        return iRetval;
    
    sprt_if->recvResp(sprt_cmd);

    /*!< check R1 Status */
    if (!mrt_isBitResetl(NR_SdCard_R1_AllErrBit, &sprt_cmd->resp[0]))
        return -ER_ERROR;

    return iRetval;
}

/*
 * @brief   fwk_sdcard_command_set_blocksize
 * @param   flags: block size
 * @retval  none
 * @note    CMD16 Controller
 */
static kint32_t fwk_sdcard_command_set_blocksize(struct fwk_sdcard_cmd *sprt_cmd, struct fwk_sdcard_if *sprt_if, kuint32_t flags)
{
    kint32_t iRetval;

    if (!sprt_cmd)
        return -ER_NULLPTR;

    if (!sprt_if)
        return fwk_sdcard_initial_command(sprt_cmd, NR_SdCard_Cmd_SetBlockLenth, flags, NR_SdCard_Response_1);

    /*!< send command package */
    iRetval = sprt_if->sendCommand(sprt_cmd);
    if (iRetval)
        return iRetval;
    
    sprt_if->recvResp(sprt_cmd);

    /*!< check R1 Status */
    if (!mrt_isBitResetl(NR_SdCard_R1_AllErrBit, &sprt_cmd->resp[0]))
        return -ER_ERROR;

    return iRetval;
}

/*
 * @brief   fwk_sdcard_command_ctrl_function
 * @param   flags: none
 * @retval  none
 * @note    CMD6 Controller
 */
static kint32_t fwk_sdcard_command_ctrl_function(struct fwk_sdcard_cmd *sprt_cmd, struct fwk_sdcard_if *sprt_if, 
                                                        kuint32_t mode, kuint32_t group, kuint32_t funcIndex)
{
    struct fwk_sdcard *sprt_card;
    struct fwk_sdcard_cmd6_data *sprt_func;
    struct fwk_sdcard_data sgrt_data;
    kuint32_t *ptrDataBuffer, iDataBufferSize;
    kuint8_t bufferCnt;
    kint32_t iRetval;

    if (!sprt_cmd)
        return -ER_NULLPTR;

    if (!sprt_if)
        return fwk_sdcard_initial_command(sprt_cmd, NR_SdCard_Cmd_SelectFunc, 
                                        FWK_SDCARD_CMD6_ARGS(mode, group, funcIndex), NR_SdCard_Response_1);

    /*!< get card and register structure */
    sprt_card = mrt_container_of(sprt_if, struct fwk_sdcard, sgrt_if);

    /*!< if not support function switch */
    if (sprt_card->version < NR_SdCard_Version1_0)
        return -ER_NSUPPORT;

    if (mrt_isBitResetl(NR_SdCard_CmdClassSwitch, &sprt_card->sgrt_csd.cardCommandClass))
        return -ER_NSUPPORT;

    sgrt_data.blockCount = 1U;
    sgrt_data.blockSize = 64U;

    /*!< set block size */
    iRetval = fwk_sdcard_send_command(sprt_card, fwk_sdcard_command_set_blocksize, sgrt_data.blockSize);
    if (iRetval)
    	return iRetval;

    /*!< tell host that data will be transferred after sending commond */
    mrt_resetl(&sgrt_data.flags);
    mrt_setbitl(NR_SdCard_CmdFlagsReadEnable, &sgrt_data.flags);

    sprt_func = (struct fwk_sdcard_cmd6_data *)sprt_cmd->ptrData;
    sprt_cmd->ptrData = &sgrt_data;

    iDataBufferSize = mrt_num_align4(sgrt_data.blockSize) * sgrt_data.blockCount + 8U;

    /*!< 8U is reserved */
    ptrDataBuffer = (kuint32_t *)kzalloc(iDataBufferSize, GFP_KERNEL);
    if (!isValid(ptrDataBuffer))
        return -ER_NOMEM;

    sgrt_data.rxBuffer  = ptrDataBuffer;
    sgrt_data.txBuffer  = mrt_nullptr;
    sgrt_data.ptrHost   = sprt_cmd->ptrHost;

    if (sprt_if->setup_dma)
        sprt_if->setup_dma(&sgrt_data);

    /*!< send command package */
    iRetval = sprt_if->sendCommand(sprt_cmd);
    if (iRetval)
        goto END;

    sprt_if->recvResp(sprt_cmd);

    /*!< check R1 Status */
    iRetval = mrt_isBitResetl(NR_SdCard_R1_AllErrBit, &sprt_cmd->resp[0]) ? ER_NORMAL : -ER_ERROR;
    if (iRetval)
        goto END;
    
    /*!< start to receive data */
    iRetval = sprt_if->recvData(&sgrt_data);
    if (iRetval)
        goto END;

    /*!< endian convert */
    /*!<
    * sgrt_data.rxBuffer[0]: bit[511:480]
    * sgrt_data.rxBuffer[1]: bit[479:448]
    * sgrt_data.rxBuffer[2]: bit[447:416]
    * sgrt_data.rxBuffer[3]: bit[415:384]
    * sgrt_data.rxBuffer[4]: bit[383:352]
    */
    for (bufferCnt = 0; bufferCnt < 5; bufferCnt++)
        sgrt_data.rxBuffer[bufferCnt] = mrt_be32_to_cpu(sgrt_data.rxBuffer[bufferCnt]);

    sprt_func->funcSupport[5U] = (kuint16_t)sgrt_data.rxBuffer[0U];
    sprt_func->funcSupport[4U] = (kuint16_t)(sgrt_data.rxBuffer[1U] >> 16U);
    sprt_func->funcSupport[3U] = (kuint16_t)sgrt_data.rxBuffer[1U];
    sprt_func->funcSupport[2U] = (kuint16_t)(sgrt_data.rxBuffer[2U] >> 16U);
    sprt_func->funcSupport[1U] = (kuint16_t)sgrt_data.rxBuffer[2U];
    sprt_func->funcSupport[0U] = (kuint16_t)(sgrt_data.rxBuffer[3U] >> 16U);

    sprt_func->funcMode[5U] = (kuint8_t)((sgrt_data.rxBuffer[3U] >> 12U) & 0xfU);
    sprt_func->funcMode[4U] = (kuint8_t)((sgrt_data.rxBuffer[3U] >> 8U) & 0xfU);
    sprt_func->funcMode[3U] = (kuint8_t)((sgrt_data.rxBuffer[3U] >> 4U) & 0xfU);
    sprt_func->funcMode[2U] = (kuint8_t)((sgrt_data.rxBuffer[3U]) & 0xfU);
    sprt_func->funcMode[1U] = (kuint8_t)((sgrt_data.rxBuffer[4U] >> 28U) & 0xfU);
    sprt_func->funcMode[0U] = (kuint8_t)((sgrt_data.rxBuffer[4U] >> 24U) & 0xfU);

END:
    kfree(ptrDataBuffer);
    return iRetval;
}

/*
 * @brief   fwk_sdcard_command_select_function
 * @param   flags: none
 * @retval  none
 * @note    CMD6 Controller
 */
static kint32_t fwk_sdcard_command_select_function(struct fwk_sdcard *sprt_card, kuint32_t group, kuint32_t funcIndex)
{
    mrt_assert(sprt_card);

    struct fwk_sdcard_cmd sgrt_check_cmd;
    struct fwk_sdcard_cmd sgrt_switch_cmd;
    struct fwk_sdcard_cmd6_data sgrt_func;
    kint32_t iRetval;

    /*!< initial sgrt_check_cmd */
    iRetval = fwk_sdcard_command_ctrl_function(&sgrt_check_cmd, mrt_nullptr, NR_SdCard_Cmd6ModeCheck, group, funcIndex);
    if (iRetval)
        return iRetval;

    if (sprt_card->sgrt_if.addHeadTail)
        sprt_card->sgrt_if.addHeadTail(&sgrt_check_cmd);

    sgrt_check_cmd.ptrHost = sprt_card->sgrt_if.sprt_host;

    /*!< initial sgrt_switch_cmd */
    iRetval = fwk_sdcard_command_ctrl_function(&sgrt_switch_cmd, mrt_nullptr, NR_SdCard_Cmd6ModeSwitch, group, funcIndex);
    if (iRetval)
        return iRetval;

    if (sprt_card->sgrt_if.addHeadTail)
        sprt_card->sgrt_if.addHeadTail(&sgrt_switch_cmd);

    sgrt_switch_cmd.ptrHost = sprt_card->sgrt_if.sprt_host;

    /*!< -------------------------------------------------------------------------- */
    memset((void *)&sgrt_func, 0, sizeof(sgrt_func));
    sgrt_check_cmd.ptrData = (void *)&sgrt_func;
    iRetval = fwk_sdcard_command_ctrl_function(&sgrt_check_cmd, &sprt_card->sgrt_if, NR_SdCard_Cmd6ModeCheck, group, funcIndex);
    if (iRetval)
        return iRetval;

    /*!< check if support function [i] */
    iRetval = mrt_isBitSetw(mrt_bit(funcIndex), &sgrt_func.funcSupport[group]);
    if (!iRetval)
        return -ER_NSUPPORT;

    /*!< check if function [i] can be switched */
    iRetval = (funcIndex == sgrt_func.funcMode[group]);
    if (!iRetval)
        return -ER_NSUPPORT;

    /*!< -------------------------------------------------------------------------- */
    memset((void *)&sgrt_func, 0, sizeof(sgrt_func));
    sgrt_switch_cmd.ptrData = (void *)&sgrt_func;
    iRetval = fwk_sdcard_command_ctrl_function(&sgrt_switch_cmd, &sprt_card->sgrt_if, NR_SdCard_Cmd6ModeSwitch, group, funcIndex);
    if (iRetval)
        return iRetval;

    /*!< check if function [i] can be switched */
    iRetval = (funcIndex == sgrt_func.funcMode[group]);
    if (!iRetval)
        return -ER_NSUPPORT;

    return ER_NORMAL;
}

/*
 * @brief   fwk_sdcard_command_pullup
 * @param   flags: none
 * @retval  none
 * @note    ACMD42 Controller
 */
static kint32_t fwk_sdcard_command_pullup(struct fwk_sdcard_cmd *sprt_cmd, struct fwk_sdcard_if *sprt_if, kuint32_t flags)
{
    kint32_t iRetval;

    if (!sprt_cmd)
        return -ER_NULLPTR;

    if (!sprt_if)
        return fwk_sdcard_initial_command(sprt_cmd, NR_SdCard_ACmd_PullUp, FWK_SDCARD_CMD_NONE_ARGS, NR_SdCard_Response_1);

    /*!< send command package */
    iRetval = sprt_if->sendCommand(sprt_cmd);
    if (iRetval)
        return iRetval;
    
    sprt_if->recvResp(sprt_cmd);

    /*!< check R1 Status */
    if (!mrt_isBitResetl(NR_SdCard_R1_AllErrBit, &sprt_cmd->resp[0]))
        return -ER_ERROR;

    return iRetval;
}

/*!< -------------------------------------------------------------------------- */
/*!
 * @brief   fwk_sdcard_send_command
 * @param   none
 * @retval  none
 * @note    The size of command is 5 bytes: = 1byte(start + transfer + index) + 4bytes(args) + 1byte(crc + stop)
 */
static kint32_t fwk_sdcard_send_command(struct fwk_sdcard *sprt_card, 
                                    frt_fwk_sdcard_control_t cmd_controller, kuint32_t args)
{
    mrt_assert(sprt_card);
    mrt_assert(cmd_controller);

    struct fwk_sdcard_cmd sgrt_cmds;
    kint32_t iRetval;

    /*!< initial sgrt_cmds */
    iRetval = cmd_controller(&sgrt_cmds, mrt_nullptr, args);
    if (iRetval)
        return iRetval;

    if (sprt_card->sgrt_if.addHeadTail)
        sprt_card->sgrt_if.addHeadTail(&sgrt_cmds);

    sgrt_cmds.ptrHost = sprt_card->sgrt_if.sprt_host;

    return cmd_controller(&sgrt_cmds, &sprt_card->sgrt_if, 0U);
}

/*!
 * @brief   fwk_sdcard_send_app_command
 * @param   none
 * @retval  none
 * @note    The size of command is 5 bytes: = 1byte(start + transfer + index) + 4bytes(args) + 1byte(crc + stop)
 */
static kint32_t fwk_sdcard_send_app_command(struct fwk_sdcard *sprt_card, 
                                        frt_fwk_sdcard_control_t cmd_controller, kuint32_t args)
{
    kuint32_t iRetry = 100U;
    kint32_t iRetval;

    do
    {    
        /*!< send cmd55 first */
        iRetval = fwk_sdcard_send_command(sprt_card, fwk_sdcard_command_app_before, sprt_card->relativeAddr);
        if (!iRetval)
        {
            /*!< send App Command */
            iRetval = fwk_sdcard_send_command(sprt_card, cmd_controller, args);
        }

        delay_us(100);

    } while ((-ER_RETRY == iRetval) && (--iRetry));

    if ((-ER_RETRY == iRetval) || (!iRetry))
        iRetval = -ER_FAILD;

    return iRetval;
}

/*!
 * @brief   fwk_sdcard_calc_crc7
 * @param   none
 * @retval  none
 * @note    Caculate CRC Automatically
 */
static kubyte_t fwk_sdcard_calc_crc7(kubyte_t *chptr, kuint32_t lenth)
{
    kuint32_t bitCnt, byteCnt;
    kubyte_t crc, data;

    for (byteCnt = 0, crc = 0; byteCnt < lenth; byteCnt++)
    {
        data = *(chptr + byteCnt);

        for (bitCnt = 0; bitCnt < sizeof(data); bitCnt++)
        {
            crc <<= 1;

            if ((data & 0x80) ^ (crc & 0x80))
                crc ^= 0x09;

            data <<= 1;
        }
    }

    return (crc << 1);
}

/*!
 * @brief   fwk_sdcard_add_prefix_suffix
 * @param   none
 * @retval  none
 * @note    add start bit and stop bit
 */
static void fwk_sdcard_add_prefix_suffix(struct fwk_sdcard_cmd *sprt_cmd)
{
    kuint32_t calcLenth;

    if (!sprt_cmd)
        return;

    /*!< add start bit and transfer bit */
    mrt_setbitl(mrt_bit(7U) | mrt_bit(6U), &sprt_cmd->index);

    /*!< caculate CRC */
    calcLenth = (kuint32_t)mrt_member_offset(struct fwk_sdcard_cmd, crc);
    sprt_cmd->crc = fwk_sdcard_calc_crc7((kubyte_t *)sprt_cmd, calcLenth);

    /*!< bit[7:1] is CRC; bit0 is stop bit, which value is 1 */
    mrt_setbitl(mrt_bit(0U), &sprt_cmd->crc);
}

/*!
 * @brief   fwk_sdcard_send_rw_command
 * @param   none
 * @retval  none
 * @note    card command transfer for read-write
 */
kint32_t fwk_sdcard_send_rw_command(struct fwk_sdcard_cmd *sprt_cmd, struct fwk_sdcard_if *sprt_if, 
                                    kuint32_t index, kuint32_t flags, kuint32_t errbit, kbool_t ctrl)
{
    kint32_t iRetval;

    if (!sprt_cmd)
        return -ER_NULLPTR;

    if (!ctrl)
    {   
        iRetval = fwk_sdcard_initial_command(sprt_cmd, index, flags, 
                                (NR_SdCard_Cmd_Erase == index) ? NR_SdCard_Response_1b : NR_SdCard_Response_1);

        if (sprt_if->addHeadTail)
            sprt_if->addHeadTail(sprt_cmd);

        sprt_cmd->ptrHost = sprt_if->sprt_host;

        return iRetval;
    }

    /*!< send command package */
    iRetval = sprt_if->sendCommand(sprt_cmd);
    if (iRetval)
        return iRetval;
    
    sprt_if->recvResp(sprt_cmd);

    /*!< check R1 Status */
    return mrt_isBitResetl(errbit, &sprt_cmd->resp[0]) ? ER_NORMAL : -ER_ERROR;
}

/*!
 * @brief   fwk_sdcard_read_blocks
 * @param   none
 * @retval  none
 * @note    read blocks
 */
kint32_t fwk_sdcard_read_blocks(struct fwk_sdcard *sprt_card, void *ptrBuffer, 
                                kuint32_t iBlockStart, kuint32_t iBlockCount, kusize_t iBlockSize)
{
    struct fwk_sdcard_if *sprt_if;
    struct fwk_sdcard_cmd sgrt_cmd;
    struct fwk_sdcard_data sgrt_data;
    kuint32_t index, argument;
    kuint32_t iRetry = 1U;
    kint32_t iRetval;

    sprt_if = &sprt_card->sgrt_if;
    
    /*!< get and wait for "ready for data" response */
    iRetval = fwk_sdcard_send_command(sprt_card, fwk_sdcard_command_get_status, sprt_card->relativeAddr);
    if (iRetval)
        return -ER_NREADY;

    /*!< fill cmd structure */
    index = (1U == iBlockCount) ? NR_SdCard_Cmd_ReadSingleBlock : NR_SdCard_Cmd_ReadMultiBlock;
    argument = (sprt_card->sgrt_csd.csdStructure) ? iBlockStart : (iBlockStart * iBlockSize);

    iRetval = fwk_sdcard_send_rw_command(&sgrt_cmd, sprt_if, index, argument, 0U, false);
    if (iRetval)
        return -ER_FAULT;

    /*!< fill data structure */
    sgrt_data.ptrHost = sprt_if->sprt_host;
    sgrt_data.blockCount = iBlockCount;
    sgrt_data.blockSize = iBlockSize;
    sgrt_data.rxBuffer = (kuint32_t *)ptrBuffer;
    sgrt_data.txBuffer = mrt_nullptr;
    mrt_resetl(&sgrt_data.flags);
    mrt_setbitl(NR_SdCard_CmdFlagsReadEnable, &sgrt_data.flags);
    mrt_setbitl(NR_SdCard_CmdFlagsWithBlock, &sgrt_data.flags);

    sgrt_cmd.ptrData = &sgrt_data;

    /*!< setup DMA before reading */
    if (sprt_if->setup_dma)
        sprt_if->setup_dma(&sgrt_data);

    do
    {
        iRetval = fwk_sdcard_send_rw_command(&sgrt_cmd, sprt_if, index, argument, NR_SdCard_R1_AllErrBit, true);
        if (!iRetval)
            iRetval = sprt_if->recvData(&sgrt_data);

    } while (iRetval && (--iRetry));

    if (iRetval)
        return -ER_RDATA_FAILD;

    /*!< for reading multi blocks, it must send CMD12 to stop transmission */
    if ((sgrt_data.blockCount > 1U) && (mrt_isBitResetl(NR_SdCard_CmdFlagsAuto12Enable, &sgrt_data.flags)))
    {
        iRetval = fwk_sdcard_send_command(sprt_card, fwk_sdcard_command_stop_transmission, 0U);
        if (iRetval)
            return -ER_STOP_FAILD;
    }

    return ER_NORMAL;
}

/*!
 * @brief   fwk_sdcard_write_blocks
 * @param   none
 * @retval  none
 * @note    write blocks
 */
kint32_t fwk_sdcard_write_blocks(struct fwk_sdcard *sprt_card, void *ptrBuffer, 
                                kuint32_t iBlockStart, kuint32_t iBlockCount, kusize_t iBlockSize)
{
    struct fwk_sdcard_if *sprt_if;
    struct fwk_sdcard_cmd sgrt_cmd;
    struct fwk_sdcard_data sgrt_data;
    kuint32_t index, argument;
    kuint32_t iRetry = 1U;
    kint32_t iRetval;

    sprt_if = &sprt_card->sgrt_if;
    
    /*!< get and wait for "ready for data" response */
    iRetval = fwk_sdcard_send_command(sprt_card, fwk_sdcard_command_get_status, sprt_card->relativeAddr);
    if (iRetval)
        return -ER_NREADY;

    /*!< fill cmd structure */
    index = (1U == iBlockCount) ? NR_SdCard_Cmd_WriteSingleBlock : NR_SdCard_Cmd_WriteMultiBlock;
    argument = (sprt_card->sgrt_csd.csdStructure) ? iBlockStart : (iBlockStart * iBlockSize);

    iRetval = fwk_sdcard_send_rw_command(&sgrt_cmd, sprt_if, index, argument, 0U, false);
    if (iRetval)
        return -ER_FAULT;

    /*!< fill data structure */
    sgrt_data.ptrHost = sprt_if->sprt_host;
    sgrt_data.blockCount = iBlockCount;
    sgrt_data.blockSize = iBlockSize;
    sgrt_data.rxBuffer = mrt_nullptr;
    sgrt_data.txBuffer = (const kuint32_t *)ptrBuffer;
    mrt_resetl(&sgrt_data.flags);
    mrt_clrbitl(NR_SdCard_CmdFlagsReadEnable, &sgrt_data.flags);
    mrt_setbitl(NR_SdCard_CmdFlagsWithBlock, &sgrt_data.flags);

    sgrt_cmd.ptrData = &sgrt_data;

    /*!< setup DMA before reading */
    if (sprt_if->setup_dma)
        sprt_if->setup_dma(&sgrt_data);

    do
    {
        iRetval = fwk_sdcard_send_rw_command(&sgrt_cmd, sprt_if, index, argument, NR_SdCard_R1_AllErrBit, true);
        if (!iRetval)
            iRetval = sprt_if->sendData(&sgrt_data);

    } while (iRetval && (--iRetry));

    if (iRetval)
        return -ER_SDATA_FAILD;

    /*!< for reading multi blocks, it must send CMD12 to stop transmission */
    if ((sgrt_data.blockCount > 1U) && (mrt_isBitResetl(NR_SdCard_CmdFlagsAuto12Enable, &sgrt_data.flags)))
    {
        iRetval = fwk_sdcard_send_command(sprt_card, fwk_sdcard_command_stop_transmission, 0U);
        if (iRetval)
            return -ER_STOP_FAILD;
    }

    return ER_NORMAL;
}

/*!
 * @brief   fwk_sdcard_erase_blocks
 * @param   none
 * @retval  none
 * @note    erase blocks
 */
kint32_t fwk_sdcard_erase_blocks(struct fwk_sdcard *sprt_card, kuint32_t iBlockStart, kuint32_t iBlockCount, kusize_t iBlockSize)
{
    struct fwk_sdcard_if *sprt_if;
    struct fwk_sdcard_cmd sgrt_blkStart;
    struct fwk_sdcard_cmd sgrt_blkEnd;
    struct fwk_sdcard_cmd sgrt_erase;
    kuint32_t index, argument;
    kuint32_t iRetry = 1U;
    kint32_t iRetval;

    sprt_if = &sprt_card->sgrt_if;
    
    /*!< get and wait for "ready for data" response */
    iRetval = fwk_sdcard_send_command(sprt_card, fwk_sdcard_command_get_status, sprt_card->relativeAddr);
    if (iRetval)
        return -ER_NREADY;

    /*!< --------------------------------------------------------------------- */
    /*!< fill cmd structure */
    index = NR_SdCard_Cmd_SetEraseBlkStart;
    argument = (sprt_card->sgrt_csd.csdStructure) ? iBlockStart : (iBlockStart * iBlockSize);
    iRetval = fwk_sdcard_send_rw_command(&sgrt_blkStart, sprt_if, index, argument, 0U, false);
    if (iRetval)
        return -ER_FAULT;

    do
    {
        iRetval = fwk_sdcard_send_rw_command(&sgrt_blkStart, sprt_if, index, argument, NR_SdCard_R1_AllErrBit, true);

    } while (iRetval && (--iRetry));

    if (iRetval)
        return -ER_SCMD_FAILD;

    /*!< -------------------------------------------------------------------- */
    index = NR_SdCard_Cmd_SetEraseBlkEnd;
    argument = (sprt_card->sgrt_csd.csdStructure) ? (iBlockStart + iBlockCount - 1U): ((iBlockStart + iBlockCount - 1U) * iBlockSize);
    iRetval = fwk_sdcard_send_rw_command(&sgrt_blkEnd, sprt_if, index, argument, 0U, false);
    if (iRetval)
        return -ER_FAULT;

    iRetval = fwk_sdcard_send_rw_command(&sgrt_blkEnd, sprt_if, index, argument, NR_SdCard_R1_AllErrBit, true);
    if (iRetval)
        return -ER_SCMD_FAILD;

    /*!< -------------------------------------------------------------------- */
    index = NR_SdCard_Cmd_Erase;
    argument = FWK_SDCARD_CMD_NONE_ARGS;
    iRetval = fwk_sdcard_send_rw_command(&sgrt_erase, sprt_if, index, argument, 0U, false);
    if (iRetval)
        return -ER_FAULT;

    iRetval = fwk_sdcard_send_rw_command(&sgrt_erase, sprt_if, index, argument, NR_SdCard_R1_AllErrBit, true);
    if (iRetval)
        return -ER_SCMD_FAILD;

    return ER_NORMAL;
}

/*!< -------------------------------------------------------------------------- */
/*!
 * @brief   host_sdmmc_card_initial
 * @param   none
 * @retval  none
 * @note    initial host of SD Card
 */
__weak void *host_sdmmc_card_initial(struct fwk_sdcard *sprt_card)
{
    return mrt_nullptr;
}

/*!
 * @brief   check if card is inserted
 * @param   sprt_card
 * @retval  1: insert; 0: none
 * @note    card detection
 */
kbool_t fwk_sdcard_is_insert(struct fwk_sdcard *sprt_card)
{
    struct fwk_sdcard_if *sprt_if;
    struct fwk_sdcard_host *sprt_host;

    if (!isValid(sprt_card))
        return false;

    sprt_if = &sprt_card->sgrt_if;
    sprt_host = sprt_if->sprt_host;

    /*!< if card is not inserted, wait a loop */
    return sprt_if->is_insert(sprt_host);
}

/*!
 * @brief   fwk_sdcard_allocate_device
 * @param   none
 * @retval  sprt_card
 * @note    allocate card attribute
 */
void *fwk_sdcard_allocate_device(void *sprt_sd)
{
    struct fwk_sdcard *sprt_card;
    struct fwk_sdcard_host *sprt_host;
    
    if (!isValid(sprt_sd))
    {
        sprt_card = (struct fwk_sdcard *)kmalloc(sizeof(struct fwk_sdcard), GFP_KERNEL);
        if (!isValid(sprt_card))
            goto fail1;
    }
    else
        sprt_card = (struct fwk_sdcard *)sprt_sd;

    memset(sprt_card, 0, sizeof(struct fwk_sdcard));

    sprt_host = host_sdmmc_card_initial(sprt_card);
    if (!isValid(sprt_host))
        goto fail2;

    sprt_card->sgrt_if.sprt_host = sprt_host;
    sprt_card->isDynamic = mrt_isNull(sprt_sd);

    /*!< Check valid */
    if (fwk_sdcard_check_interface(&sprt_card->sgrt_if))
        goto fail3;
    
    mrt_setbitl(NR_SdCard_Idle_State, &sprt_card->mode);

    return sprt_card;

fail3:
    kfree(sprt_host);

fail2:
    if (!isValid(sprt_sd))
        kfree(sprt_card);

fail1:
    return mrt_nullptr;
}

/*!
 * @brief   fwk_sdcard_free_device
 * @param   sprt_card
 * @retval  none
 * @note    free card attribute
 */
void fwk_sdcard_free_device(struct fwk_sdcard *sprt_card)
{
    struct fwk_sdcard_host *sprt_host;

    if (!isValid(sprt_card))
        return;

    sprt_host = sprt_card->sgrt_if.sprt_host;
    
    if (isValid(sprt_host) && sprt_host->isSelfDync)
    {
        memset(sprt_host, 0, sizeof(struct fwk_sdcard_host));
        kfree(sprt_host);
    }

    memset(sprt_card, 0, sizeof(struct fwk_sdcard));

    if (sprt_card->isDynamic)
        kfree(sprt_card);
}

/*!
 * @brief   fwk_sdcard_initial_device
 * @param   none
 * @retval  none
 * @note    detect and initial sdcard
 */
kint32_t fwk_sdcard_initial_device(struct fwk_sdcard *sprt_card)
{
    kbool_t blRetval;

    /*!< Check valid */
    if (fwk_sdcard_check_interface(&sprt_card->sgrt_if))
        return -ER_FAULT;

    blRetval = fwk_sdcard_detect(sprt_card);
    if (!blRetval)
    {
        print_debug("Can not detect SD Card! Please check if card is inserted ...\n");
        return -ER_NREADY;
    }

    mrt_setbitl(NR_SdCard_Identify_State, &sprt_card->mode);    

    blRetval = fwk_sdcard_to_normal(sprt_card);
    if (!blRetval)
    {
        print_debug("Can not configure SD Card! Please check if card is avaliable ...\n");
        return -ER_FAILD;
    }

    mrt_setbitl(NR_SdCard_Transfer_State, &sprt_card->mode);

    return ER_NORMAL;
}

/*!
 * @brief   fwk_sdcard_inactive_device
 * @param   sprt_card
 * @retval  none
 * @note    inactive SD Card
 */
void fwk_sdcard_inactive_device(struct fwk_sdcard *sprt_card)
{
    kint32_t iRetval;

    if (!isValid(sprt_card) || !isValid(sprt_card->sgrt_if.sprt_host))
        return;

    if (mrt_isBitResetl(NR_SdCard_Transfer_State, &sprt_card->mode))
        return;

    /*!< data lines change to 1 */
    iRetval = fwk_sdcard_send_app_command(sprt_card, fwk_sdcard_command_set_buswidth, FWK_SDCARD_ACMD6_BUS_WIDTH_1);
    if (iRetval)
    {
        /*!< do nothing */
    }

    /*!< cancel card */
    iRetval = fwk_sdcard_send_command(sprt_card, fwk_sdcard_command_select_card, 0U);
    if (iRetval)
    {
        /*!< do nothing */
    }

    /*!< go idle state */
    iRetval = fwk_sdcard_send_command(sprt_card, fwk_sdcard_command_go_idle, 0U);
    if (iRetval)
    {
        /*!< do nothing */
    }

    fwk_sdcard_free_device(sprt_card);
}

/*!
 * @brief   fwk_sdcard_detect
 * @param   none
 * @retval  none
 * @note    detect and initial sdcard
 */
kbool_t fwk_sdcard_detect(struct fwk_sdcard *sprt_card)
{
    mrt_assert(sprt_card);

    struct fwk_sdcard_if *sprt_if;
    struct fwk_sdcard_host *sprt_host;
    kuint32_t iOpCondCmdArgs = 0U;
    kint32_t iRetval;
    kbool_t blRetval;

    sprt_if = &sprt_card->sgrt_if;
    sprt_host = sprt_if->sprt_host;

    if (!sprt_if->addHeadTail)
        sprt_if->addHeadTail = fwk_sdcard_add_prefix_suffix;

    /*!< if card is not inserted, wait a loop */
    while (!sprt_if->is_insert(sprt_host));

    /*!< Delay some time to make card stable after inserting */
    delay_ms(500U);
    sprt_card->relativeAddr = 0U;
    sprt_card->voltage = NR_SdCard_toVoltage3_3V;

    /*!< set DATA bus width = 1bit */
    sprt_if->setBusWidth(sprt_host, NR_SdCard_BusWidth_1Bit);

    /*!< set card frequency to 400KHz */
    sprt_if->setClkFreq(sprt_host, NR_SdCard_ClkFreq_400KHz);

    /*!< sent 80 clock to sdcard, and wait done */
    blRetval = sprt_if->cardActive(sprt_host, 100U);
    if (!blRetval)
        goto fail;

    /*!< send CMD0 */
    /*!< reset sdcard, and enter idle status */
    iRetval = fwk_sdcard_send_command(sprt_card, fwk_sdcard_command_go_idle, 0U);
    if (iRetval)
        goto fail;

    /*!< send CMD8 */
    /*!< if return well, SDHC or SDXC is supported; if not, it is a SDSC card */
    iRetval = fwk_sdcard_send_command(sprt_card, fwk_sdcard_command_send_if_cond, NR_SdCard_Cmd8Vhs3_3V);
    if (!iRetval)
        mrt_setbitl(NR_SdCard_OcrCapacity_Bit, &iOpCondCmdArgs);
    else
    {
        /*!< it is a SDSC. Reset sdcard to free CMD8 */
        iRetval = fwk_sdcard_send_command(sprt_card, fwk_sdcard_command_go_idle, 0U);
        if (iRetval)
            goto fail;
    }

    /*!< Voltage supply */
    mrt_setbitl(NR_SdCard_OcrVdd32_33_Bit, &iOpCondCmdArgs);
    mrt_setbitl(NR_SdCard_OcrVdd33_34_Bit, &iOpCondCmdArgs);
//  mrt_setbitl(NR_SdCard_OcrToV18Request_Bit, &iOpCondCmdArgs);

    /*!< send ACMD41 */
    iRetval = fwk_sdcard_send_app_command(sprt_card, fwk_sdcard_command_operation_cond, iOpCondCmdArgs);
    if (iRetval)
        goto fail;

    blRetval  = mrt_isBitSetl(NR_SdCard_SupportVoltage1_8V, &sprt_if->sprt_host->flagBit);
    blRetval &= mrt_isBitSetl(NR_SdCard_OcrToV18Request_Bit, &sprt_card->ocr);

    /*!< switch voltage 1.8V before reading cid/csd/... */
    if (blRetval)
    {
        iRetval = fwk_sdcard_send_command(sprt_card, fwk_sdcard_command_switch_voltage, 0U);
        if (iRetval)
            goto fail;

        sprt_card->voltage = NR_SdCard_toVoltage1_8V;
    }

    /*!< get cid */
    iRetval = fwk_sdcard_send_command(sprt_card, fwk_sdcard_command_get_cid, 0U);
    if (iRetval)
        goto fail;

    /*!< get rca */
    iRetval = fwk_sdcard_send_command(sprt_card, fwk_sdcard_command_get_rca, 0U);
    if (iRetval)
        goto fail;

    if (!sprt_card->relativeAddr)
        goto fail;

    return true;

fail:
    return false;
}

/*!
 * @brief   fwk_sdcard_to_normal
 * @param   none
 * @retval  none
 * @note    configure card to work in normal mode
 */
kbool_t fwk_sdcard_to_normal(struct fwk_sdcard *sprt_card)
{
    mrt_assert(sprt_card);

    struct fwk_sdcard_if *sprt_if;
    struct fwk_sdcard_host *sprt_host;
    kint32_t iRetval;

    sprt_if = &sprt_card->sgrt_if;
    sprt_host = sprt_if->sprt_host;

    if (!sprt_card->relativeAddr)
        goto fail1;

    /*!< get csd */
    iRetval = fwk_sdcard_send_command(sprt_card, fwk_sdcard_command_get_csd, sprt_card->relativeAddr);
    if (iRetval)
        goto fail1;

    /*!< set card frequency to 25MHz */
    sprt_if->setClkFreq(sprt_host, NR_SdCard_ClkFreq_25MHz);

    /*!< select card */
    iRetval = fwk_sdcard_send_command(sprt_card, fwk_sdcard_command_select_card, sprt_card->relativeAddr);
    if (iRetval)
        goto fail1;

    /*!< Pull-up disconnected during data transfer */
    iRetval = fwk_sdcard_send_app_command(sprt_card, fwk_sdcard_command_pullup, 0U);
    if (iRetval)
        goto fail2;
    
    /*!< get scr: ACMD51 */
    iRetval = fwk_sdcard_send_app_command(sprt_card, fwk_sdcard_command_get_scr, 0U);
    if (iRetval)
        goto fail2;

    /*!< check if card and host support 4bits width */
    if (!mrt_isBitSetb(0x4U, &sprt_card->sgrt_scr.sdBusWidths) || mrt_isBitResetl(NR_SdCard_Support4BitWidth, &sprt_host->flagBit))
        goto fail2;

    /*!<
     * for imx6ull:
     *  只更改卡或主机的位宽, CMD6可回复DATA, 不阻塞, 且结果正确;
     *  只更改卡的位宽, CMD17读命令不阻塞(有数据可读), 但读取结果错误;
     *  只更改主机位宽, CMD17读命令阻塞(无数据可读)
     * 
     *  分析: 大概率是主机的问题
     */
    /*!< set card DATA bus width = 4bit */
	iRetval = fwk_sdcard_send_app_command(sprt_card, fwk_sdcard_command_set_buswidth, FWK_SDCARD_ACMD6_BUS_WIDTH_4);
	if (iRetval)
		goto fail2;
    
    /*!< set host DATA bus width = 4bit */
	sprt_if->setBusWidth(sprt_host, NR_SdCard_BusWidth_4Bit);

    /*!< set card with high speed */
    iRetval = fwk_sdcard_command_select_function(sprt_card, NR_SdCard_Cmd6GroupAccess, NR_SdCard_Cmd6SpeedHigh);
    if (iRetval)
        goto fail2;

    /*!< set card drive strenth */
    iRetval = fwk_sdcard_command_select_function(sprt_card, NR_SdCard_Cmd6GroupDrvStrenth, NR_SdCard_Cmd6DriverDefault);
    if (iRetval)
        goto fail2;

    /*!< set card power current */
    iRetval = fwk_sdcard_command_select_function(sprt_card, NR_SdCard_Cmd6GroupPowerLimit, NR_SdCard_Cmd6PowerDefault);
    if (iRetval)
        goto fail2;

    /*!< set card block lenth */
    iRetval = fwk_sdcard_send_command(sprt_card, fwk_sdcard_command_set_blocksize, FWK_SDCARD_DEFAULT_BLOCKSIZE);
    if (iRetval)
        goto fail2;

    return true;

fail2:
    /*!< cancel card */
    fwk_sdcard_send_command(sprt_card, fwk_sdcard_command_select_card, 0U);

fail1:
    return false;
}

/*!
 * @brief   fwk_sdcard_rw_blocks
 * @param   none
 * @retval  none
 * @note    read or write blocks
 */
kbool_t fwk_sdcard_rw_blocks(struct fwk_sdcard *sprt_card, void *ptrBuffer, 
                                kuint32_t iBlockStart, kuint32_t iBlockCount, nrt_fwk_sdcard_rw_t egrt_rw)
{
    mrt_assert(sprt_card);

    struct fwk_sdcard_host *sprt_host;
    kuint8_t *ptrNextBuffer;
    kuint32_t iBlockTrans, iBlockSize;
    kint32_t iRetval = -ER_ERROR;

    if (!isValid(ptrBuffer))
        return false;

    sprt_host = sprt_card->sgrt_if.sprt_host;
//  if ((!sprt_host->maxBlockCount) || ((iBlockStart + iBlockCount) > sprt_host->maxBlockCount))
//      return false;

    ptrNextBuffer = (kuint8_t *)ptrBuffer;
    iBlockTrans = 0U;
    iBlockSize = FWK_SDCARD_DEFAULT_BLOCKSIZE;

    while (iBlockCount)
    {
        switch (egrt_rw)
        {
            case NR_SdCard_ReadToHost:
                iBlockTrans = mrt_ret_min2(iBlockCount, sprt_host->maxBlockCount);
                iRetval = fwk_sdcard_read_blocks(sprt_card, ptrNextBuffer, iBlockStart, iBlockTrans, iBlockSize);
                break;

            case NR_SdCard_WriteToCard:
                iBlockTrans = mrt_ret_min2(iBlockCount, sprt_host->maxBlockCount);
                iRetval = fwk_sdcard_write_blocks(sprt_card, ptrNextBuffer, iBlockStart, iBlockTrans, iBlockSize);
                break;

            default:
                iRetval = -ER_ERROR;
                break;
        }

        if (iRetval)
            break;

        iBlockStart += iBlockTrans;
        iBlockCount -= iBlockTrans;
        ptrNextBuffer += iBlockTrans * iBlockSize;
    }

    return (iRetval < 0) ? false : true;
}

/*!
 * @brief   fwk_sdcard_format_blocks
 * @param   none
 * @retval  none
 * @note    format(erase) blocks
 */
kbool_t fwk_sdcard_format_blocks(struct fwk_sdcard *sprt_card, kuint32_t iBlockStart, kuint32_t iBlockCount)
{
    mrt_assert(sprt_card);

    kuint32_t iBlockTrans, iBlockSize;
    kint32_t iRetval = -ER_ERROR;

    if ((!sprt_card->sgrt_csd.eraseSectorSize) || ((iBlockStart + iBlockCount) > (sprt_card->sgrt_csd.eraseSectorSize + 1U)))
        return false;

    iBlockTrans = 0U;
    iBlockSize = FWK_SDCARD_DEFAULT_BLOCKSIZE;

    while (iBlockCount)
    {
        iBlockTrans = mrt_ret_min2(iBlockCount, sprt_card->sgrt_csd.eraseSectorSize + 1U);
        iRetval = fwk_sdcard_erase_blocks(sprt_card, iBlockStart, iBlockTrans, iBlockSize);

        if (iRetval)
            break;

        iBlockStart += iBlockTrans;
        iBlockCount -= iBlockTrans;
    }

    return (iRetval < 0) ? false : true;
}

/* end of file */
