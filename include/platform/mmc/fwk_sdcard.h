/*
 * General SD Card Protocol
 *
 * File Name:   fwk_sdcard.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.10.09
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_SDCARD_H
#define __FWK_SDCARD_H

/*!< The includes */
#include <common/generic.h>
#include <common/io_stream.h>
#include <common/mem_manage.h>
#include <board/board.h>

/*!< The defines */
#define FWK_SDCARD_DEFAULT_BLOCKSIZE                            (512U)
#define FWK_SDCARD_RW_BLOCKCOUNT(x)                             ((x) >> 9U)

/*!<
 * SD Card type:
 *  SDSC: Standard Capacity
 *  SDHC: High Capacity
 *  SDXC: Extended Capacity
 *
 * SD Card include:
 *  1) Memory Unit
 *  2) Memory Unit Interface
 *  3) Power Detect
 *  4) Card and Interface Control Uint: 
 *          CID[127:0]: Card indentification. Card individual number for indentification
 *          RCA[15:0]:  Relative card address, local system address of a card, dynamically suggested by the card and approved by the host during initialization.
 *          DSR[15:0]:  Driver Stage Register, to configure the card's output drivers
 *          CSD[127:0]: Card Specific Data, infomation about the card operation conditions
 *          SCR[63:0]:  SD Configuration Register, information about the SD Memory Card's Special Features capabilities
 *          OCR[31:0]:  Operation condition register
 *          SSR[511:0]: SD Status; information about the card proprietary features
 *          CSR[31:0]:  Card Status, information about the card status
 *  5) Interface Drive: form left to right are: DAT2, CD/DAT3, CMD, VSS1(GND), VDD(Power), CLK, VSS2(GND), DAT0, DAT1
 *          CLK: Clock line, rising edge valid. while in indentification stage, CLK should be 400KHz; but when entering transmision mode, CLK can be up to 25MHz.
 *          CMD: command and response tranfer by CMD line
 *          D0 ~ D3: Data transfer, taking D0 to low can indicate that SD is busy
 */
/*!< Registers */
/*!< OCR Register */
typedef enum nrt_fwk_sdcard_ocr
{
    NR_SdCard_OcrVddLowVol_Bit = mrt_bit(7U),               /*!< reserved for low power */
    NR_SdCard_OcrVdd27_28_Bit = mrt_bit(15U),               /*!< VDD: 2.7V ~ 2.8V */
    NR_SdCard_OcrVdd28_29_Bit = mrt_bit(16U),               /*!< VDD: 2.8V ~ 2.9V */
    NR_SdCard_OcrVdd29_30_Bit = mrt_bit(17U),               /*!< VDD: 2.9V ~ 3.0V */
    NR_SdCard_OcrVdd30_31_Bit = mrt_bit(18U),               /*!< VDD: 3.0V ~ 3.1V */
    NR_SdCard_OcrVdd31_32_Bit = mrt_bit(19U),               /*!< VDD: 3.1V ~ 3.2V */
    NR_SdCard_OcrVdd32_33_Bit = mrt_bit(20U),               /*!< VDD: 3.2V ~ 3.3V */
    NR_SdCard_OcrVdd33_34_Bit = mrt_bit(21U),               /*!< VDD: 3.3V ~ 3.4V */
    NR_SdCard_OcrVdd34_35_Bit = mrt_bit(22U),               /*!< VDD: 3.4V ~ 3.5V */
    NR_SdCard_OcrVdd35_36_Bit = mrt_bit(23U),               /*!< VDD: 3.5V ~ 3.6V */
    NR_SdCard_OcrToV18Request_Bit = mrt_bit(24U),           /*!< Switch to 1.8V request */

    /*!< bit30 should be read after Power-on finished and bit31 is true */
    NR_SdCard_OcrCapacity_Bit = mrt_bit(30U),               /*!< Capacity of Card. 1: High-Capacity Card(HC or XC); 0: Standard Card(SC) */
    NR_SdCard_OcrNotBusy_Bit = mrt_bit(31U),                /*!< Power-on State of Card. 1: Power on complete; 0: Power on not complete */

} nrt_fwk_sdcard_ocr_t;

/*!< CID Register */
typedef struct fwk_sdcard_cid
{
    kuint8_t manufacturerID;                                /*!< Manufacturer ID [127:120] */
    kuint16_t applicationID;                                /*!< OEM/Application ID [119:104] */
    kuint8_t productName[5];                                /*!< Product name [103:64] */
    kuint8_t productVersion;                                /*!< Product revision [63:56] */
    kuint32_t productSerialNumber;                          /*!< Product serial number [55:24] */
    kuint16_t manufacturerData;                             /*!< Manufacturing date [19:8] */

} srt_fwk_sdcard_cid_t;

/*!
 * @brief   CSD Register
 *          It can read CSD by sending CMD9
 * @note    some flags that only occupy 1 bit
 */
enum __ERT_SDCARD_CSD_SLICE_BIT
{
    NR_SdCard_CsdTempWriteProtect_Bit = mrt_bit(0U),        /*!< Temporary write protection [12:12] */
    NR_SdCard_CsdPermWriteProtect_Bit = mrt_bit(1U),        /*!< Permanent write protection [13:13] */
    NR_SdCard_CsdCopyFlag_Bit = mrt_bit(2U),                /*!< Copy flag [14:14] */
    NR_SdCard_CsdFileFormatGroup_Bit = mrt_bit(3U),         /*!< File format group [15:15] */
    NR_SdCard_CsdPartialBlockWrite_Bit = mrt_bit(4U),       /*!< Partial blocks for write allowed [21:21] */
    NR_SdCard_CsdWriteProtectEnable_Bit = mrt_bit(5U),      /*!< Write protect group enabled [31:31] */
    NR_SdCard_CsdEraseBlockEnale_Bit = mrt_bit(6U),         /*!< Erase single block enabled [46:46] */
    NR_SdCard_CsdDsrImplemented_Bit = mrt_bit(7U),          /*!< DSR implemented [76:76] */
    NR_SdCard_CsdReadBlockMisalign_Bit = mrt_bit(8U),       /*!< Read block misalignment [77:77] */
    NR_SdCard_CsdWriteBlockMisalign_Bit = mrt_bit(9U),      /*!< Write block misalignment [78:78] */
    NR_SdCard_CsdPartialBlockRead_Bit = mrt_bit(10U),       /*!< Partial blocks for read allowed [79:79] */
};

/*!
 * @brief   CSD Register
 *          It can read CSD by sending CMD9
 * @note    all bits, including "__ERT_SDCARD_CSD_SLICE_BIT"
 */
typedef struct fwk_sdcard_csd
{
    /*!< 
     * Including Version of CSD
     *  00: CSD version 1.0 (SDSC)
     *  01: CSD version 2.0 (SDHC and SDXC)
     *  10, 11: reserved
     */
    kuint8_t csdStructure;                                  /*!< CSD structure [127:126] */
    kuint8_t dataReadAccessTime1;                           /*!< Data read access-time-1 [119:112] */
    kuint8_t dataReadAccessTime2;                           /*!< Data read access-time-2 in clock cycles (NSAC*100) [111:104] */

    /*!<
     * max data-transfer-speed
     * for SDSC: transferSpeed = 0b00110010 (50h), 25MHz
     * for SDHC: transferSpeed = 0b01011010 (90h), 50MHz
     */
    kuint8_t transferSpeed;                                 /*!< Maximum data transfer rate [103:96] */

    /*!<
     * Card Class that is supported
     * 0 ~ 11: Class0 ~ Class11
     */
    kuint16_t cardCommandClass;                             /*!< Card command classes [95:84] */

    /*!< readBlockLength is 9 */
    kuint8_t readBlockLength;                               /*!< Maximum read data block length [83:80] */

    /*!< ------------------------------------------------------------------- */
    kuint32_t deviceSize;                                   /*!< for 1.0: [73:62]; for 2.0: [69:48] */

    /*!< Following fields from 'readCurrentVddMin' to 'deviceSizeMultiplier' exist in CSD version 1.0 */
    kuint8_t readCurrentVddMin;                             /*!< Maximum read current at VDD min [61:59] */
    kuint8_t readCurrentVddMax;                             /*!< Maximum read current at VDD max [58:56] */
    kuint8_t writeCurrentVddMin;                            /*!< Maximum write current at VDD min [55:53] */
    kuint8_t writeCurrentVddMax;                            /*!< Maximum write current at VDD max [52:50] */
    kuint8_t deviceSizeMultiplier;                          /*!< Device size multiplier [49:47] */

    /*!< ------------------------------------------------------------------- */
    kuint8_t eraseSingleBlockEnable;                        /*!< Erase single block enable [46]*/
    kuint8_t eraseSectorSize;                               /*!< Erase sector size [45:39] */
    kuint8_t writeProtectGroupSize;                         /*!< Write protect group size [38:32] */
    kuint8_t writeSpeedFactor;                              /*!< Write speed factor [28:26] */
    kuint8_t writeBlockLength;                              /*!< Maximum write data block length [25:22] */
    kuint8_t fileFormat;                                    /*!< File format [11:10] */

    kuint32_t flagBit;                                      /*!< refer to "__ERT_SDCARD_CSD_SLICE_BIT" */

} srt_fwk_sdcard_csd_t;

/*!< cardCommandClass with CSD Register */
enum __ERT_SDCARD_CSD_CMD_CLASS
{
    NR_SdCard_CmdClassBasic = mrt_bit(1U),                  /*!< Card command class 0 */
    NR_SdCard_CmdClassBlockRead = mrt_bit(2U),              /*!< Card command class 2 */
    NR_SdCard_CmdClassBlockWrite = mrt_bit(4U),             /*!< Card command class 4 */
    NR_SdCard_CmdClassErase = mrt_bit(5U),                  /*!< Card command class 5 */
    NR_SdCard_CmdClassWriteProtect = mrt_bit(6U),           /*!< Card command class 6 */
    NR_SdCard_CmdClassLockCard = mrt_bit(7U),               /*!< Card command class 7 */
    NR_SdCard_CmdClassAppSpecific = mrt_bit(8U),            /*!< Card command class 8 */
    NR_SdCard_CmdClassInputOutputMode = mrt_bit(9U),        /*!< Card command class 9 */
    NR_SdCard_CmdClassSwitch = mrt_bit(10U),                /*!< Card command class 10 */
};

/*!< SD card SCR register bit */
enum __ERT_SDCARD_SCR_SLICE_BIT
{
    NR_SdCard_ScrSdSpecification3 = mrt_bit(0U),            /*!< Specification version 3.00 or higher [47:47]*/
    NR_SdCard_ScrDataStatusAfterErase = mrt_bit(1U),        /*!< Data status after erases [55:55] */
};

/*!< SD card SCR register */
typedef struct fwk_sdcard_scr
{
    kuint8_t scrStructure;                                  /*!< SCR Structure [63:60] */
    kuint8_t sdSpecification;                               /*!< SD memory card specification version [59:56] */
    kuint8_t sdSecurity;                                    /*!< Security specification supported [54:52] */
    kuint8_t sdBusWidths;                                   /*!< Data bus widths supported [51:48] */
    kuint8_t extendedSecurity;                              /*!< Extended security support [46:43] */
    kuint8_t commandSupport;                                /*!< Command support bits [33:32] 33-support CMD23, 32-support cmd20*/
    kuint32_t reservedForManufacturer;                      /*!< reserved for manufacturer usage [31:0] */

    kuint32_t flagBit;                                      /*!< refer to "__ERT_SDCARD_SCR_SLICE_BIT" */

} srt_fwk_sdcard_scr_t;

/*!
 * @brief SD Card Operation Mode
 */
enum __ERT_SDCARD_MODE_STATUS
{
    /*!< Card identification mode */
    NR_SdCard_Idle_State = 0U,                              /*!< Idle State */
    NR_SdCard_Ready_State,                                  /*!< Ready State */
    NR_SdCard_Identify_State,                               /*!< Identification State */

    /*!< Data transfer mode */
    NR_SdCard_StandBy_State,                                /*!< Stand-by State */
    NR_SdCard_Transfer_State,                               /*!< Transfer State */
    NR_SdCard_DataTx_State,                                 /*!< Sending-data State */
    NR_SdCard_DataRx_State,                                 /*!< Receive-data State */
    NR_SdCard_Program_State,                                /*!< Programming State */
    NR_SdCard_Disconnect_State,                             /*!< Disconnect State */

    /*!< Inactive mode */
    NR_SdCard_Inactive_State,                               /*!< Inactive State */
};

/*!<
 * Command:
 *  start bit(bit47, fixed to 0) + transfer bit(bit46, fixed to 1) + index(bit[45:40]) + argument(bit[39:8] + CRC(bit[7:1]) + stop bit(bit0, fixed to 1))
 *  SDIO Command includes APP_CMD and GEN_CMD, GEN_CMD55 must be send before ACMD
 *  Command type:
 *      1) Unresponsive broadcast command(bc): send to all cards, but not receive any response;
 *      2) Broadcast command with response(bcr): send to all cards, and receive response form all cards;
 *      3) Addressing command(ac): send to card is seleted, but no data will be transmitted;
 *      4) Addressing Data Transfer Command(adtc): send to card is seleted, and data will be transmitted after command
 * 
 * Response(R1):
 *  start bit(bit47, fixed to 0) + transfer bit(bit46, fixed to 0) + index(bit[45:40]) + status(bit[39:8] + CRC(bit[7:1]) + stop bit(bit0, fixed to 1))
 *  Both commands and responses are transmitted through CMD line, but start bit, transfer bit, CRC7和stop bit are controlled by SDIO/MMC, we can just care argument and index
 *  CMD0 no response; Response includes short response(48位) and long response(136位). R2 is long response, and Others is short response 
 *
 * sequential:
 *  When using 4-wire transmission, each data line must have a start bit, stop bit, and CRC bit; 
 *  Each data line needs to check the CRC bit separately, summarize the inspection results, and then provide feedback to the host through D0 after data transmission is completed.
 *  1) For 8-bit wide packets, send low bytes first and then high bytes, with each byte sending high bits first and then low bits;
 *  2) For wide bit data packets (512bit), after the host issues the ACMD13, the SD card sends the content of the SSR register to the host through the DAT line
 */
enum __ERT_SDCARD_CMD8_VHS
{
    NR_SdCard_Cmd8VhsNone = 0U,                             /*!< card voltage supply: none */
    NR_SdCard_Cmd8Vhs3_3V = 1U,                             /*!< card voltage supply: 3.3V */
    NR_SdCard_Cmd8Vhs3_0V = 2U,                             /*!< card voltage supply: 3.0V */
    NR_SdCard_Cmd8Vhs1_8V = 3U,                             /*!< card voltage supply: 1.8V */
};

enum __ERT_SDCARD_CMD6_MODE
{
    NR_SdCard_Cmd6ModeCheck = 0U,                           /*!< SD switch mode 0: check function */
    NR_SdCard_Cmd6ModeSwitch = 1U,                          /*!< SD switch mode 1: set function */
};

enum __ERT_SDCARD_CMD6_GROUP
{
    NR_SdCard_Cmd6GroupAccess = 0U,                         /*!< access mode group*/
    NR_SdCard_Cmd6GroupCommand,                             /*!< command system group*/
    NR_SdCard_Cmd6GroupDrvStrenth,                          /*!< driver strength group*/
    NR_SdCard_Cmd6GroupPowerLimit,                          /*!< current limit group*/
};

enum __ERT_SDCARD_CMD6_SPEED
{
    NR_SdCard_Cmd6SpeedSDR12 = 0U,                          /*!< SDR12 mode & default */
    NR_SdCard_Cmd6SpeedSDR25,                               /*!< SDR25 & high speed*/
    NR_SdCard_Cmd6SpeedSDR50,                               /*!< SDR50 mode*/
    NR_SdCard_Cmd6SpeedSDR104,                              /*!< SDR104 mode*/
    NR_SdCard_Cmd6SpeedDDR50,                               /*!< DDR50 mode*/

    NR_SdCard_Cmd6SpeedDefault = NR_SdCard_Cmd6SpeedSDR12,
    NR_SdCard_Cmd6SpeedHigh = NR_SdCard_Cmd6SpeedSDR25,
};

enum __ERT_SDCARD_CMD6_DRV_STRENTH
{
    NR_SdCard_Cmd6DriverTypeB = 0U,                         /*!< default driver strength*/
    NR_SdCard_Cmd6DriverTypeA,                              /*!< driver strength TYPE A */
    NR_SdCard_Cmd6DriverTypeC,                              /*!< driver strength TYPE C */
    NR_SdCard_Cmd6DriverTypeD,                              /*!< driver strength TYPE D */

    NR_SdCard_Cmd6DriverDefault = NR_SdCard_Cmd6DriverTypeB,
};

enum __ERT_SDCARD_CMD6_POWER
{
    NR_SdCard_Cmd6PowerCurrent200MA = 0U,                   /*!< default current limit */
    NR_SdCard_Cmd6PowerCurrent400MA,                        /*!< current limit to 400MA */
    NR_SdCard_Cmd6PowerCurrent600MA,                        /*!< current limit to 600MA */
    NR_SdCard_Cmd6PowerCurrent800MA,                        /*!< current limit to 800MA */

    NR_SdCard_Cmd6PowerDefault = NR_SdCard_Cmd6PowerCurrent200MA,
};

/*!< if mode is check, it will return R1 and data[511:0] */
typedef struct fwk_sdcard_cmd6_data
{
    /*!<
     * bit[495:480]: group6
     * bit[479:464]: group5
     * bit[463:448]: group4
     * bit[447:432]: group3
     * bit[431:416]: group2
     * bit[415:400]: if a bit[400 + i] is set, function i is supported in group1 (i = function 15 to 0)
     */
    kuint16_t funcSupport[6U];

    /*!<
     * bit[399:396]: group6
     * bit[395:392]: group5
     * bit[391:388]: group4
     * bit[387:384]: group3
     * bit[383:380]: group2
     * bit[379:376]: 0xf shows function set error with the argument;
     *               mode 0: the function which can be switched in function group 1;
     *               mode 1: the function which is result of the switch command, in function group 1
     */
    kuint8_t funcMode[6U];

    /*!<
     * bit[287:272]: if bit[i] is set, function [i] is busy. This field can be read in mode 0 and mode 1
     */
    kuint8_t funcBusy[6U];

} srt_fwk_sdcard_cmd6_data_t;

/*!< argument: bit[39:8], 32 bits totally */
/*!< 
 * reserved all(full 0), used to:
 *  CMD0, CMD2, CMD3, CMD12, CMD27, CMD38, CMD42, ACMD13, ACMD51
 */
#define FWK_SDCARD_CMD_NONE_ARGS                            (0x00000000U)
/*!< 
 * all use, for:
 *  CMD16, CMD17, CMD18, CMD24, CMD25, CMD32, CMD33
 */
#define FWK_SDCARD_CMD_USE_ARGS(data)                       ((data) & 0xffffffffU)
/*!< 
 * DSR(bit[31:16]), reserved(bit[15:0]); for:
 *  CMD4
 */
#define FWK_SDCARD_CMD_DSR_ARGS(data)                       (((data) << 16) & 0xffff0000U)
/*!< 
 * RCA(bit[31:16]), reserved(bit[15:0]); for:
 *  CMD7, CMD9, CMD10, CMD13, CMD15, CMD55
 */
#define FWK_SDCARD_CMD_RCA_ARGS(data)                       (((data) << 16) & 0xffff0000U)
/*!<
 * CMD8 argument:
 *  reserved(bit[31:12]), Voltage Supplied(VHS, bit[11:8]), Check Pattern(0xAA)
 *  VHS:  if set 0b0001(0x01), the specified host power supply is between 2.7-3.6V
 *  Response: if card support CMD8 (indicate the card's version is 2.0 at less, and support the voltage supply), R7 will be get; Otherwise, no response will feedback
 */
#define FWK_SDCARD_CMD_CHECK_PATTERN                        (0x000000aaU)
#define FWK_SDCARD_CMD_CHECK_MASK                           (0x000000ffU)
#define FWK_SDCARD_CMD8_ARGS(VHS)                           ((((VHS) << 8) | FWK_SDCARD_CMD_CHECK_PATTERN) & 0x0000ffffU)
#define FWK_SDCARD_CMD8_HOST_V27_V36                        FWK_SDCARD_CMD8_ARGS(NR_SdCard_Cmd8Vhs3_3V)
/*!<
 * CMD6 argument:
 *  bit31:      Mode (0: check function; 1: switch function)
 *  bit[30:24]: reserved (0)
 *  bit[23:20]: reserved for function group6 (0x0 or 0xf). But group6 is also reserved
 *  bit[19:16]: reserved for function group5 (0x0 or 0xf). But group5 is slso reserved
 *  bit[15:12]: function group 4 for Power limit. But bit15 is reserved
 *  bit[11:8]:  function group 3 for Drive Strenth. But bit11 is reserved
 *  bit[7:4]:   function group 2 for Power Command System
 *  bit[3:0]:   function group 1 for Power Access Mode
 */
#define FWK_SDCARD_CMD6_SET_MODE(MODE)                      (mrt_bit_nr(MODE, 31U) | 0x00ffffffU)
#define FWK_SDCARD_CMD6_GRP_MASK(group)                     (mrt_bit_nr(0xfU, (group) << 2U))
#define FWK_SDCARD_CMD6_GRP_ARGS(group, data)               (mrt_bit_nr(data, (group) << 2U) | (~FWK_SDCARD_CMD6_GRP_MASK(group)))
#define FWK_SDCARD_CMD6_CHECK_MODE(group, data)             (FWK_SDCARD_CMD6_GRP_ARGS(group, data) & 0x00ffffffU)
#define FWK_SDCARD_CMD6_SWITCH_MODE(group, data)            (FWK_SDCARD_CMD6_GRP_ARGS(group, data) & 0x80ffffffU)
#define FWK_SDCARD_CMD6_ARGS(mode, group, data)             (FWK_SDCARD_CMD6_GRP_ARGS(group, data) & FWK_SDCARD_CMD6_SET_MODE(mode))
/*!<
 * CMD56 argument:
 *  reserved(bit[31:1]), R/W(bit0; 1: read; 0: write)
 */
#define FWK_SDCARD_CMD56_ARGS(RW)                           ((RW) & 0x00000001U)
#define FWK_SDCARD_CMD56_DIR_R(RW)                          FWK_SDCARD_CMD56_ARGS(1U)
#define FWK_SDCARD_CMD56_DIR_W(RW)                          FWK_SDCARD_CMD56_ARGS(0U)
/*!<
 * ACMD6 argument:
 *  reserved(bit[31:2]), BUS_Width(bit[1:0]; 00: 1bit; 10: 4bit)
 */
#define FWK_SDCARD_ACMD6_ARGS(width)                        ((width) & 0x00000003U)
#define FWK_SDCARD_ACMD6_BUS_WIDTH_1                        FWK_SDCARD_ACMD6_ARGS(0U)
#define FWK_SDCARD_ACMD6_BUS_WIDTH_4                        FWK_SDCARD_ACMD6_ARGS(2U)
/*!<
 * ACMD41 argument:
 *  reserved(bit31), HCS(bit30, OCR[30]), reserved(bit[29:24]), VDD Power(bit[23:0], OCR[23:0])
 */
#define FWK_SDCARD_ACMD41_ARGS(HCS, VDD)                    ((((HCS) << 30U) | (VDD)) & 0x40ffffffU)
#define FWK_SDCARD_ACMD41_SC_ARGS(VDD)                      ((VDD) & 0x00ffffffU)
#define FWK_SDCARD_ACMD41_HC_ARGS(VDD)                      ((NR_SdCard_OcrCapacity_Bit | (VDD)) & 0x40ffffffU)

/*!
 * @brief Response
 *
 *  SDIO has 7 response types(R1 ~ R7), but SD does not include R4 and R5.
 *  The lenth of short response is 48bits, long response is 136bits. Only R2 is long response
 *  Except for the R3 type(R3 does not have CRC, bit[7:1] fixed to 0x7f), all other responses are verified using CRC7, R2 uses internal CRC7 of CID or CSD registers
 *  
 *  <The highest bit is the starting bit(fixed to 0); The next bit is the transmission bit(fixed to 0 as response); The last bit is the stop bit(fixed to 1)>
 *  
 *  R1 = start bit + trans bit + CMDx(bit[45:40], index) + card status(bit[39:8]) + CRC7(bit[7:1]) + stop bit
 *  R2 = start bit + trans bit + reserved(bit[133:128], 0x7f) + CID or CSD Register[127:8](bit[127:8]) + Internal CRC7(bit[7:1]) + stop bit
 *  R3 = start bit + trans bit + reserved(bit[45:40], 0x7f) + OCR Register(bit[39:8]) + reserved(bit[7:1], 0x7f) + stop bit
 *  R6 = start bit + trans bit + CMD3(bit[45:40], 0x03) + RCA Register(bit[39:24]) + card status(bit[23:8]) + CRC7(bit[7:1]) + stop bit
 *  R7 = start bit + trans bit + CMD8(bit[45:40], 0x80) + reserved(bit[39:20]) + voltage accepted(bit[19:16]) + check pattern(bit[15:8]) + CRC7(bit[7:1]) + stop bit
 * 
 *  R1b is indentical to R1 with an optional busy signal transmitted on the [data line]. The card may become busy after receiving these
 *  commands based on its state prior to the command reception. The host shall check for busy at the response.
 */
typedef enum nrt_fwk_sdcard_resp
{
    NR_SdCard_Response_0 = 0U,                         /*!< R0 = No Response */
    NR_SdCard_Response_1,
    NR_SdCard_Response_2,
    NR_SdCard_Response_3,
    NR_SdCard_Response_4,
    NR_SdCard_Response_5,
    NR_SdCard_Response_6,
    NR_SdCard_Response_7,

    NR_SdCard_Response_1b,                             /*!< R1 + Busy bit */
    NR_SdCard_Response_5b,                             /*!< R5 + Busy bit */

} nrt_fwk_sdcard_resp_t;

/*!< R1 Response: Card Status Bit (bit[39:8]) */
enum _ERT_FWK_SDCARD_R1_STATUS_BIT
{
    /*!< error in the sequence of authentication process */
    NR_SdCard_R1_AuthenticationSequenceErr_Bit = mrt_bit(3U),

    /*!< Application command enabled status bit */
    NR_SdCard_R1_AppCmdEnabled_Bit = mrt_bit(5U),

    /*!< Switch error status bit */
    NR_SdCard_R1_SwitchErr_Bit = mrt_bit(7U),

    /*!< Ready for data status bit */
    NR_SdCard_R1_ReadyForData_Bit = mrt_bit(8U),

    /*!< bit[12:9] is "__ERT_SDCARD_MODE_STATUS" */
#define FWK_SDCARD_R1_STATUS_STATE_OFFSET                   (9U)
#define FWK_SDCARD_R1_STATUS_STATE_MASK                     (0x1e00U)
#define FWK_SDCARD_R1_STATUS_STATE_BIT(x)                   mrt_bit_mask((x), FWK_SDCARD_R1_STATUS_STATE_MASK, FWK_SDCARD_R1_STATUS_STATE_OFFSET)
#define FWK_SDCARD_R1_STATUS_STATE_U32(x)                   FWK_SDCARD_R1_STATUS_STATE_BIT(x)

    /*!< 0U */
    NR_SdCard_R1_StateIdle_Bit = FWK_SDCARD_R1_STATUS_STATE_BIT(NR_SdCard_Idle_State),
    /*!< mrt_bit(9U) */
    NR_SdCard_R1_StateReady_Bit = FWK_SDCARD_R1_STATUS_STATE_BIT(NR_SdCard_Ready_State),
    /*!< mrt_bit(10U) */
    NR_SdCard_R1_StateIdentify_Bit = FWK_SDCARD_R1_STATUS_STATE_BIT(NR_SdCard_Identify_State),
    /*!< mrt_bit(9U) | mrt_bit(10U) */
    NR_SdCard_R1_StateStandBy_Bit = FWK_SDCARD_R1_STATUS_STATE_BIT(NR_SdCard_StandBy_State),
    /*!< mrt_bit(11U) */
    NR_SdCard_R1_StateTransfer_Bit = FWK_SDCARD_R1_STATUS_STATE_BIT(NR_SdCard_Transfer_State),
    /*!< mrt_bit(9U) | mrt_bit(11U) */
    NR_SdCard_R1_StateDataTx_Bit = FWK_SDCARD_R1_STATUS_STATE_BIT(NR_SdCard_DataTx_State),
    /*!< mrt_bit(10U) | mrt_bit(11U) */
    NR_SdCard_R1_StateDataRx_Bit = FWK_SDCARD_R1_STATUS_STATE_BIT(NR_SdCard_DataRx_State),
    /*!< mrt_bit(9U) | mrt_bit(10U) | mrt_bit(11U) */
    NR_SdCard_R1_StateProgram_Bit = FWK_SDCARD_R1_STATUS_STATE_BIT(NR_SdCard_Program_State),
    /*!< mrt_bit(12U) */
    NR_SdCard_R1_StateDisconnect_Bit = FWK_SDCARD_R1_STATUS_STATE_BIT(NR_SdCard_Disconnect_State),

    /*!< Erase reset status bit */
    NR_SdCard_R1_EraseReset_Bit = mrt_bit(13U),

    /*!< Card ecc disabled status bit */
    NR_SdCard_R1_CardEccDisabled_Bit = mrt_bit(14U),

    /*!< Write protection erase skip status bit */
    NR_SdCard_R1_WriteProtEraseSkip_Bit = mrt_bit(15U),

    /*!< Cid/csd overwrite status bit */
    NR_SdCard_R1_CidCsdOverWrite_Bit = mrt_bit(16U),

    /*!< A general or an unknown error status bit */
    NR_SdCard_R1_Error_Bit = mrt_bit(19U),

    /*!< Internal card controller error status bit */
    NR_SdCard_R1_InternalCardCtrlErr_Bit = mrt_bit(20U),

    /*!< Card ecc error status bit */
    NR_SdCard_R1_CardEccErr_Bit = mrt_bit(21U),

    /*!< Illegal command status bit */
    NR_SdCard_R1_IllegalCmd_Bit = mrt_bit(22U),

    /*!< CRC error status bit */
    NR_SdCard_R1_CrcErr_Bit = mrt_bit(23U),

    /*!< lock/unlock error status bit */
    NR_SdCard_R1_LockUnlockErr_Bit = mrt_bit(24U),

    /*!< Card locked status bit */
    NR_SdCard_R1_CardLocked_Bit = mrt_bit(25U),

    /*!< Write protection violation status bit */
    NR_SdCard_R1_WriteProtViolate_Bit = mrt_bit(26U),

    /*!< Erase parameter error status bit */
    NR_SdCard_R1_EraseParameterErr_Bit = mrt_bit(27U),

    /*!< Erase sequence error status bit */
    NR_SdCard_R1_EraseSequenceErr_Bit = mrt_bit(28U),

    /*!< Block length error status bit */
    NR_SdCard_R1_BlockLenErr_Bit = mrt_bit(29U),

    /*!< Address error status bit */
    NR_SdCard_R1_AddressErr_Bit = mrt_bit(30U),
    
    /*!< Out of range status bit */
    NR_SdCard_R1_OutOfRange_Bit = mrt_bit(31U),

    /*!< Card error status */
    NR_SdCard_R1_AllErrBit = (NR_SdCard_R1_AuthenticationSequenceErr_Bit |
        NR_SdCard_R1_CidCsdOverWrite_Bit | NR_SdCard_R1_Error_Bit | NR_SdCard_R1_InternalCardCtrlErr_Bit |
        NR_SdCard_R1_CardEccErr_Bit | NR_SdCard_R1_IllegalCmd_Bit | NR_SdCard_R1_CrcErr_Bit | 
        NR_SdCard_R1_LockUnlockErr_Bit | NR_SdCard_R1_CardLocked_Bit | NR_SdCard_R1_WriteProtViolate_Bit | 
        NR_SdCard_R1_EraseParameterErr_Bit | NR_SdCard_R1_EraseSequenceErr_Bit | NR_SdCard_R1_BlockLenErr_Bit | 
        NR_SdCard_R1_AddressErr_Bit | NR_SdCard_R1_OutOfRange_Bit),
};

#define FWK_SDCARD_CMD_APP								(0x80000000U)
#define FWK_SDCARD_CMD_MASK								(0x0000003fU)

/*!< Command index(bit[45:40]): General Command */
typedef enum nrt_fwk_sdcard_gcmd
{
    /*!< Basic (Class 0) */
    NR_SdCard_Cmd_GoIdle = 0U,                          /*!< bc,    -       Resets all MMC and SD memory cards to idle state */
    NR_SdCard_Cmd_GetAllOpCond = 1U,                    /*!< bcr,   R3,     Notify all cards to return to the operating conditions through the CMD line(OCR) */
    NR_SdCard_Cmd_AllSendCID = 2U,                      /*!< bcr,   R2,     Notify all cards to return CID values through CMD lines */
    NR_SdCard_Cmd_IssueRCA = 3U,                        /*!< bcr,   R6,     Notify all cards to issue a new RCA */
    NR_SdCard_Cmd_SetDSR = 4U,                          /*!< bc,    -       Programming DSR for all cards */
    NR_SdCard_Cmd_SelectFunc = 6U,                      /*!< adtc,  R1,     Function mode switching (speed, power, etc.) */
    NR_SdCard_Cmd_SelectCard = 7U,                      /*!< ac,    R1,     Select/Deselect RCA Address Card */
    NR_SdCard_Cmd_SendIfCond = 8U,                      /*!< bcr,   R7,     Send SD card interface conditions, including voltage information supported by the host, and ask if the card supports it */
    NR_SdCard_Cmd_GetCSD = 9U,                          /*!< ac,    R2,     Read the CSD register contents of the selected card */
    NR_SdCard_Cmd_GetCID = 10U,                         /*!< ac,    R2,     Read the CID register content of the selected card */
    NR_SdCard_Cmd_SwitchVoltage = 11U,                  /*!< adtc,  R1,     Voltage switching; The prerequisite for use is that the ACMD41 response includes 1.8V support */
    NR_SdCard_Cmd_StopTrans = 12U,                      /*!< ac,    R1b,    Force card to stop transmission */
    NR_SdCard_Cmd_GetStatus = 13U,                      /*!< ac,    R1,     Read the status register content of the selected card */
    NR_SdCard_Cmd_GoInactive = 15U,                     /*!< ac,    -       Put the selected card into an 'inactive' state */

    /*!< Read (Class 2) */
    NR_SdCard_Cmd_SetBlockLenth = 16U,                  /*!< ac,    R1,     For SDSC, set the length of the card command; For SDHC, the block command length is fixed at 512 bytes */
    NR_SdCard_Cmd_ReadSingleBlock = 17U,                /*!< adtc,  R1,     For SDSC, the size of the read block is the value set by CMD16; For SDHC, the size of the read block is 512 bytes */
    NR_SdCard_Cmd_ReadMultiBlock = 18U,                 /*!< adtc,  R1,     Continuously read data blocks from the SD card until interrupted by CMD12 Each block has the same length as CMD17 */

    NR_SdCard_Cmd_WriteUntilStop = 20U,                 /*!< adtc,  R1,     Continuously write data to the card until transmission stops */

    /*!< Write (Class 4) */
    NR_SdCard_Cmd_SetBlockCount = 23U,                  /*!< reserved */
    NR_SdCard_Cmd_WriteSingleBlock = 24U,               /*!< adtc,  R1,     For SDSC, the size of the write block is the value set by CMD16; For SDHC, the size of the write block is 512 bytes */
    NR_SdCard_Cmd_WriteMultiBlock = 25U,                /*!< adtc,  R1,     Continuously write data blocks from the SD card until interrupted by CMD12 Each block has the same length as CMD17 */
    NR_SdCard_Cmd_ProgramCID = 26U,                     /*!< adtc,  R1,     Programming the programmable bits of CID */
    NR_SdCard_Cmd_ProgramCSD = 27U,                     /*!< adtc,  R1,     Programming the programmable bits of CSD */
    NR_SdCard_Cmd_SetWriteProtect = 28U,                /*!< ac,    R1b,    Set write protection */
    NR_SdCard_Cmd_ClrWriteProtect = 29U,                /*!< ac,    R1b,    Cancel write protection */
    NR_SdCard_Cmd_GetWriteProtect = 30U,                /*!< actc,  R1,     Read the current write protected state */

    /*!< Erase (Class 5) */
    NR_SdCard_Cmd_SetEraseBlkStart = 32U,               /*!< ac,    R1,     Set the starting block address for erasure */
    NR_SdCard_Cmd_SetEraseBlkEnd = 33U,                 /*!< ac,    R1,     Set the end block address for erasure */
    NR_SdCard_Cmd_Erase = 38U,                          /*!< ac,    R1b,    Erase pre-selected blocks */

    /*!< Lock (Class 7) */
    NR_SdCard_Cmd_LockUnlock = 42U,                     /*!< adtc,  R1b,    Lock/Unlock SD Card */

    /*!< Class 8 */
    NR_SdCard_Cmd_AppCmd = 55U,                         /*!< ac,    R1,     Specify the next command as a specific application command, not a standard command */
    NR_SdCard_Cmd_GenCmd = 56U,                         /*!< adtc,  R1b,    In general commands or specific application commands, used to transfer a data block. The lowest bit of 1 indicates reading data, while 0 indicates writing data */

} nrt_fwk_sdcard_gcmd_t;

/*!< Command index(bit[45:40]): Special Application Command */
typedef enum nrt_fwk_sdcard_acmd
{
	/*!< ac, R1, Define data bus width (00: 1bit; 10: 4bit) */
    NR_SdCard_ACmd_SetBusWidth = FWK_SDCARD_CMD_APP | 6U,

	/*!< adtc, R1, Send SD status */
    NR_SdCard_ACmd_GetSdStatus = FWK_SDCARD_CMD_APP | 13U,

	/*!< bcr, R3, The host requires the card to send its support information (HCS) and OCR register content */
    NR_SdCard_ACmd_GetOpCond = FWK_SDCARD_CMD_APP | 41U,

	/*!< ac, R1, The host requires to send pullup command to card before using DAT line 3(using 4-bit bus) */
    NR_SdCard_ACmd_PullUp = FWK_SDCARD_CMD_APP | 42U,

	/*!< adtc, R1, Read Configuration Register SCR */
    NR_SdCard_ACmd_GetSCR = FWK_SDCARD_CMD_APP | 51U,

} nrt_fwk_sdcard_acmd_t;

/*!< for nrt_fwk_sdcard_gcmd */
#define FWK_SDCARD_CMDTYPE_GENERIC						(0)
/*!< for nrt_fwk_sdcard_acmd */
#define FWK_SDCARD_CMDTYPE_APPLICATION					(1)

__align(1) typedef struct fwk_sdcard_cmd
{
    kuint8_t index;

    union
    {
        kuint8_t  bytes[4];
        kuint32_t args;
    };
    
    kuint8_t crc;

    kuint32_t cmdType;
    kuint32_t resp[4];
    kuint32_t respType;

    void *ptrHost;
    void *ptrData;

} srt_fwk_sdcard_cmd_t;

typedef struct fwk_sdcard_data
{
    kusize_t blockSize;
    kuint32_t blockCount;

    /*!< total bytes that will be transmited. unit: bytes */
    kuint32_t capacity;

    kuint32_t *rxBuffer;
    const kuint32_t *txBuffer;

    kuint16_t flags;                                        /*!< corresponding to "__ERT_SDCARD_CMD_FLAGS" */

    void *ptrHost;
    void *privData;

} srt_fwk_sdcard_data_t;

/*!< command flags */
enum __ERT_SDCARD_DATA_FLAGS
{
    NR_SdCard_CmdFlagsReadEnable = mrt_bit(1U),             /*!< data direction(1: read; 0: write) */
    NR_SdCard_CmdFlagsAuto12Enable = mrt_bit(2U),           /*!< auto stop, Suitable for multi block operations */
    NR_SdCard_CmdFlagsAuto23Enable = mrt_bit(3U),           /*!< auto write, Suitable for multi block operations */
	NR_SdCard_CmdFlagsWithBlock = mrt_bit(4U),				/*!< r/w block ? if no, it maybe call by cmd6 or SCR */
};

/*!
 * @brief SD Card Workflow: Indentification

 * (Power on) CMD0: Enter idle state (after powering on, all cards, including those currently in an inactive state, enter idle state)
 *                  (By using the CMD0 command, all cards that are currently in a "non invalid" state can be soft reset to enter an idle state)
 * (idle) CMD8: Cards with V2.0 protocol must first send CMD8 and then ACMD41;
 *              If the working voltage is not supported, the card will not respond and return to idle state
 * (idle) ACMD41: If the card returns to a busy state or the host does not support voltage, return to an idle state;
 *                If the card voltage is inconsistent, the card will not respond and enter an inactive state;
 *                The response is normal, entering the ready-state
 *                (For CMD8 responsive cards, set the HCS bit of ACMD41 to 1 to test the card's capacity type)
 *                (If the CCS bit of the card response is 1, it is a high-capacity SD card; Otherwise, it is a standard card)
 * (ready) CMD2 ---> Enter the recognition state Card returns CID value, select the card
 * (identify) CMD3 ---> Requesting a new RCA value, the host and card enter standby mode (data transmission mode) Card recognition status ended
 * 
 * (Any state of data transmission mode) CMD15 ---> Enter inactive state
 */
/*!
 * @brief SD Card Workflow: Data Transmission
 * 
 * (standby) CMD4(program)/CMD9(read CSD)/CMD10(read CID)/CMD3(request new RCA): All in standby mode
 * (standby) CMD7: According to RCA selection, the card enters the transmission state
 * (transmit) CMD7: According to RCA cancellation, the card returns to standby mode
 * (transmit) CMD16(set blocksize)/CMD32(set erase block start)/CMD33(set erase block end)/ACMD6(set bus width): All in transmit mode
 * (transmit) Send commands related to requesting read, for SD cards, that is, send data. Including read cmd(CMD6/17/18/56), write cmd(ACMD13/ACMD51)
 * (send data) CMD12 or sending finished: Returns to transmission status; CMD7: Return to standby mode
 * (transmit) Send commands related to requesting write, for SD cards, that is, receive data. Including write cmd(CMD24/25/56), program(CMD27), lock/unlock(CMD42)
 * (receive data) CMD12 or receiving finished: Enter program status
 * (transmit) CMD28: Pre erase the block, and the card enters the programming state from the transmission state
 * (program) Complete operation: return to transmission status; CMD7: Entering disconnected state
 * (disconected) Complete operation: Return to standby mode
 */
/*!< ---------------------------------------------------------------------- */
/*!< SD Card Host */
enum __ERT_SDCARD_HOST_SUPPORT_BIT
{
    NR_SdCard_SupportHighCapacity = mrt_bit(0U),
    NR_SdCard_Support4BitWidth = mrt_bit(1U),
    NR_SdCard_SupportCardSdhc = mrt_bit(2U),
    NR_SdCard_SupportCardSdxc = mrt_bit(3U),
    NR_SdCard_SupportVoltage1_8V = mrt_bit(4U),
    NR_SdCard_SupportSetBlockCount = mrt_bit(5U),
    NR_SdCard_SupportSpeedClassCtrl = mrt_bit(6U),
};

typedef struct fwk_sdcard_host
{
    kuaddr_t iHostIfBase;
    kuaddr_t iHostCDBase;

    kuint32_t sdSupportVersion;
    kuint32_t mmcSupportVersion;

    kuint32_t maxBlockCount;
    kuint32_t maxBlockLength;
    
    kuint32_t flagBit;                                      /*!< corresponding to "__ERT_SDCARD_SUPPORT_BIT" */
    kbool_t isSelfDync;
    void *privData;

} srt_fwk_sdcard_host_t;

/*!< SD Card Interface */
typedef struct fwk_sdcard_if
{
    struct fwk_sdcard_host *sprt_host;

    kint32_t (*sendCommand) (struct fwk_sdcard_cmd *sprt_cmds);
    void (*recvResp) (struct fwk_sdcard_cmd *sprt_cmds);
    kint32_t (*sendData) (struct fwk_sdcard_data *sprt_data);
    kint32_t (*recvData) (struct fwk_sdcard_data *sprt_data);
    kint32_t (*setup_dma)(struct fwk_sdcard_data *sprt_data);

    kbool_t (*is_insert) (struct fwk_sdcard_host *sprt_host);
    void (*setBusWidth) (struct fwk_sdcard_host *sprt_host, kuint32_t option);
    void (*setClkFreq) (struct fwk_sdcard_host *sprt_host, kuint32_t option);
    kbool_t (*cardActive) (struct fwk_sdcard_host *sprt_host, kuint32_t timeout);
    kint32_t (*switchVoltage) (struct fwk_sdcard_host *sprt_host, kuint32_t voltage);

    void (*addHeadTail) (struct fwk_sdcard_cmd *sprt_cmd);

} srt_fwk_sdcard_if_t;

/*!< SD Card Struct */
typedef struct fwk_sdcard
{
    kuint16_t relativeAddr;
    kuint32_t version;
    kuint32_t busclk;
    kuint32_t mode;
    kuint32_t blockCount;
    kuint32_t blockSize;
    kuint32_t capacity;
    kuint32_t voltage;

    /*!< Registers */
    kuint32_t ocr;
    struct fwk_sdcard_cid sgrt_cid;
    struct fwk_sdcard_csd sgrt_csd;
    struct fwk_sdcard_scr sgrt_scr;

    /*!< Interface */
    struct fwk_sdcard_if sgrt_if;

    kbool_t isDynamic;

} srt_fwk_sdcard_t;

/*!< Bus Width */
enum __ERT_SDCARD_BUS_WIDTH
{
    NR_SdCard_BusWidth_1Bit = 0U,
    NR_SdCard_BusWidth_4Bit,
    NR_SdCard_BusWidth_8Bit,
};

/*!< Clk */
enum __ERT_SDCARD_CLK_FREQ
{
    NR_SdCard_ClkFreq_400KHz = 0U,
    NR_SdCard_ClkFreq_25MHz,
    NR_SdCard_ClkFreq_50MHz,
};

/*!< version */
enum __ERT_SDCARD_VERSION
{
    NR_SdCard_Version1_0 = 1U,
    NR_SdCard_Version1_1 = 2U,
    NR_SdCard_Version2_0 = 3U,
    NR_SdCard_Version3_0 = 4U,
};

/*!< read or write direction */
typedef enum nrt_fwk_sdcard_rw
{
    NR_SdCard_ReadToHost = 0U,
    NR_SdCard_WriteToCard,
    NR_SdCard_EraseSector,

} nrt_fwk_sdcard_rw_t;

/*!< switch voltage */
enum __ERT_SDCARD_SW_VOLTAGE
{
    NR_SdCard_toVoltage1_8V = 0U,
    NR_SdCard_toVoltage3_3V = 1U,
};

/*!< The functions */
TARGET_EXT void *host_sdmmc_card_initial(struct fwk_sdcard *sprt_card);

TARGET_EXT void *fwk_sdcard_allocate_device(void *sprt_sd);
TARGET_EXT void fwk_sdcard_free_device(struct fwk_sdcard *sprt_card);
TARGET_EXT kint32_t fwk_sdcard_initial_device(struct fwk_sdcard *sprt_card);
TARGET_EXT void fwk_sdcard_inactive_device(struct fwk_sdcard *sprt_card);

TARGET_EXT kbool_t fwk_sdcard_is_insert(struct fwk_sdcard *sprt_card);
TARGET_EXT kbool_t fwk_sdcard_detect(struct fwk_sdcard *sprt_card);
TARGET_EXT kbool_t fwk_sdcard_to_normal(struct fwk_sdcard *sprt_card);
TARGET_EXT kbool_t fwk_sdcard_rw_blocks(struct fwk_sdcard *sprt_card, void *ptrBuffer, 
                                kuint32_t iBlockStart, kuint32_t iBlockCount, nrt_fwk_sdcard_rw_t egrt_rw);
TARGET_EXT kbool_t fwk_sdcard_format_blocks(struct fwk_sdcard *sprt_card, kuint32_t iBlockStart, kuint32_t iBlockCount);

/*!< API function */
/*!
 * @brief   fwk_sdcard_initial_command
 * @param   none
 * @retval  none
 * @note    Command Structure fill up
 */
static inline kint32_t fwk_sdcard_initial_command(struct fwk_sdcard_cmd *sprt_cmd, kuint32_t index, kuint32_t args, kuint32_t response)
{
    memory_reset(sprt_cmd, sizeof(struct fwk_sdcard_cmd));

    sprt_cmd->index = index & FWK_SDCARD_CMD_MASK;
    sprt_cmd->args = mrt_be32_to_cpu(args);
    sprt_cmd->respType = response;
    sprt_cmd->cmdType = (index & FWK_SDCARD_CMD_APP) ? FWK_SDCARD_CMDTYPE_APPLICATION : FWK_SDCARD_CMDTYPE_GENERIC;

    return ER_NORMAL;
}

#endif /* __FWK_SDCARD_H */
