/*
 * IMX6ULL CPU part of Peripheral Configure
 *
 * File Name:   imx6ull_periph.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.10.18
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __IMX6UL_PERIPH_H
#define __IMX6UL_PERIPH_H

/*!< The includes */
#include <common/basic_types.h>
#include <common/error_types.h>
#include <common/generic.h>
#include <common/io_stream.h>

/*!< The defines */
/*!< ------------------------------------------------------------------------- */
/*!< Gpio - Register Layout Typedef */
typedef struct hal_imx_gpio
{
    kuint32_t DR;                                                   /*!< GPIO data register, offset: 0x0 */
    kuint32_t GDIR;                                                 /*!< GPIO direction register, offset: 0x4 */
    kuint32_t PSR;                                                  /*!< GPIO pad status register, offset: 0x8 */
    kuint32_t ICR1;                                                 /*!< GPIO interrupt configuration register1, offset: 0xC */
    kuint32_t ICR2;                                                 /*!< GPIO interrupt configuration register2, offset: 0x10 */
    kuint32_t IMR;                                                  /*!< GPIO interrupt mask register, offset: 0x14 */
    kuint32_t ISR;                                                  /*!< GPIO interrupt status register, offset: 0x18 */
    kuint32_t EDGE_SEL;                                             /*!< GPIO edge select register, offset: 0x1C */

} srt_hal_imx_gpio_t;

/*!< Peripheral GPIO base address */
#define IMX6UL_GPIO1_ADDR_BASE                                      (0x209C000u)
#define IMX6UL_GPIO2_ADDR_BASE                                      (0x20A0000u)
#define IMX6UL_GPIO3_ADDR_BASE                                      (0x20A4000u)
#define IMX6UL_GPIO4_ADDR_BASE                                      (0x20A8000u)
#define IMX6UL_GPIO5_ADDR_BASE                                      (0x20AC000u)
#define IMX6UL_GPIO_PROPERTY_ENTRY(x)                               (srt_hal_imx_gpio_t *)IMX6UL_GPIO##x##_ADDR_BASE
#define IMX6UL_GPIO_PIN_OFFSET_BIT(x)                               mrt_bit(x)

/*!< ICR1/ICR2 Register */
#define HAL_IMX_ICR_LOW_LEVEL						                (0x00)
#define HAL_IMX_ICR_HIGH_LEVEL						                (0x01)
#define HAL_IMX_ICR_RISING_EDGE						                (0x02)
#define HAL_IMX_ICR_FALLING_EDGE					                (0x03)
#define HAL_IMX_ICR_MASK							                (0x03)

/*!< ------------------------------------------------------------------------- */
/*!< Timer - Register Layout Typedef */
typedef struct hal_imx_gptimer
{
    kuint32_t CR;                                                   /*!< GPT Control Register, offset: 0x0 */
    kuint32_t PR;                                                   /*!< GPT Prescaler Register, offset: 0x4 */
    kuint32_t SR;                                                   /*!< GPT Status Register, offset: 0x8 */
    kuint32_t IR;                                                   /*!< GPT Interrupt Register, offset: 0xC */
    kuint32_t OCR[3];                                               /*!< GPT Output Compare Register 1..GPT Output Compare Register 3, array offset: 0x10, array step: 0x4 */
    kuint32_t ICR[2];                                               /*!< GPT Input Capture Register 1..GPT Input Capture Register 2, array offset: 0x1C, array step: 0x4 */
    kuint32_t CNT;                                                  /*!< GPT Counter Register, offset: 0x24 */
    
} srt_hal_imx_gptimer_t;

/*!< Peripheral GPT base address */
#define IMX6UL_GPT1_ADDR_BASE                                       (0x2098000u)
#define IMX6UL_GPT2_ADDR_BASE                                       (0x20E8000u)
#define IMX6UL_GPT_PROPERTY_ENTRY(x)                                (srt_hal_imx_gptimer_t *)IMX6UL_GPT##x##_ADDR_BASE

/*!< ------------------------------------------------------------------------- */
/*!< WDOG - Register Layout Typedef */
typedef struct 
{
    kuint16_t WCR;                                                  /*!< Watchdog Control Register, offset: 0x0 */
    kuint16_t WSR;                                                  /*!< Watchdog Service Register, offset: 0x2 */
    kuint16_t WRSR;                                                 /*!< Watchdog Reset Status Register, offset: 0x4 */
    kuint16_t WICR;                                                 /*!< Watchdog Interrupt Control Register, offset: 0x6 */
    kuint16_t WMCR;                                                 /*!< Watchdog Miscellaneous Control Register, offset: 0x8 */
} srt_imx_wdog_t;

/*!< Peripheral WDOG base address */
#define IMX6UL_WDOG1_ADDR_BASE                                      (0x20BC000u)
#define IMX6UL_WDOG2_ADDR_BASE                                      (0x20C0000u)
#define IMX6UL_WDOG3_ADDR_BASE                                      (0x21E4000u)
#define IMX6UL_WDOG_PROPERTY_ENTRY(x)                               (srt_imx_wdog_t *)IMX6UL_WDOG##x##_ADDR_BASE

/*!< ------------------------------------------------------------------------- */
/*!< Uart - Register Layout Typedef */
typedef struct 
{
    kuint32_t URXD;                                                 /*!< UART Receiver Register, offset: 0x0 */
    kuint8_t  RESERVED_0[60];               
    kuint32_t UTXD;                                                 /*!< UART Transmitter Register, offset: 0x40 */
    kuint8_t  RESERVED_1[60];               
    kuint32_t UCR1;                                                 /*!< UART Control Register 1, offset: 0x80 */
    kuint32_t UCR2;                                                 /*!< UART Control Register 2, offset: 0x84 */
    kuint32_t UCR3;                                                 /*!< UART Control Register 3, offset: 0x88 */
    kuint32_t UCR4;                                                 /*!< UART Control Register 4, offset: 0x8C */
    kuint32_t UFCR;                                                 /*!< UART FIFO Control Register, offset: 0x90 */
    kuint32_t USR1;                                                 /*!< UART Status Register 1, offset: 0x94 */
    kuint32_t USR2;                                                 /*!< UART Status Register 2, offset: 0x98 */
    kuint32_t UESC;                                                 /*!< UART Escape Character Register, offset: 0x9C */
    kuint32_t UTIM;                                                 /*!< UART Escape Timer Register, offset: 0xA0 */
    kuint32_t UBIR;                                                 /*!< UART BRM Incremental Register, offset: 0xA4 */
    kuint32_t UBMR;                                                 /*!< UART BRM Modulator Register, offset: 0xA8 */
    kuint32_t UBRC;                                                 /*!< UART Baud Rate Count Register, offset: 0xAC */
    kuint32_t ONEMS;                                                /*!< UART One Millisecond Register, offset: 0xB0 */
    kuint32_t UTS;                                                  /*!< UART Test Register, offset: 0xB4 */
    kuint32_t UMCR;                                                 /*!< UART RS-485 Mode Control Register, offset: 0xB8 */
} srt_imx_uart_t;

/*!< Peripheral UART base address */
#define IMX6UL_UART1_ADDR_BASE                                      (0x2020000u)
#define IMX6UL_UART2_ADDR_BASE                                      (0x21E8000u)
#define IMX6UL_UART3_ADDR_BASE                                      (0x21EC000u)
#define IMX6UL_UART4_ADDR_BASE                                      (0x21F0000u)
#define IMX6UL_UART5_ADDR_BASE                                      (0x21F4000u)
#define IMX6UL_UART6_ADDR_BASE                                      (0x21FC000u)
#define IMX6UL_UART7_ADDR_BASE                                      (0x2018000u)
#define IMX6UL_UART8_ADDR_BASE                                      (0x2288000u)
#define IMX6UL_UART_PROPERTY_ENTRY(x)                               (srt_imx_uart_t *)IMX6UL_UART##x##_ADDR_BASE

/*!< ------------------------------------------------------------------------- */
/*!< I2C - Register Layout Typedef */
typedef struct hal_imx_i2c
{
    kuint16_t IADR;                                                 /*!< I2C Address Register, offset: 0x0 */
    kuint8_t  RESERVED_0[2];
    kuint16_t IFDR;                                                 /*!< I2C Frequency Divider Register, offset: 0x4 */
    kuint8_t  RESERVED_1[2];
    kuint16_t I2CR;                                                 /*!< I2C Control Register, offset: 0x8 */
    kuint8_t  RESERVED_2[2];
    kuint16_t I2SR;                                                 /*!< I2C Status Register, offset: 0xC */
    kuint8_t  RESERVED_3[2];
    kuint16_t I2DR;                                                 /*!< I2C Data I/O Register, offset: 0x10 */
} srt_hal_imx_i2c_t;

/*!< Peripheral I2C base address */
#define IMX6UL_I2C1_ADDR_BASE                                       (0x21A0000u)
#define IMX6UL_I2C2_ADDR_BASE                                       (0x21A4000u)
#define IMX6UL_I2C3_ADDR_BASE                                       (0x21A8000u)
#define IMX6UL_I2C4_ADDR_BASE                                       (0x21F8000u)
#define IMX6UL_I2C_PROPERTY_ENTRY(x)                                (srt_hal_imx_i2c_t *)IMX6UL_I2C##x##_ADDR_BASE

/*!< ------------------------------------------------------------------------- */
/*!< ECSPI - Register Layout Typedef */
typedef struct 
{
    kuint32_t RXDATA;                                               /*!< Receive Data Register, offset: 0x0 */
    kuint32_t TXDATA;                                               /*!< Transmit Data Register, offset: 0x4 */
    kuint32_t CONREG;                                               /*!< Control Register, offset: 0x8 */
    kuint32_t CONFIGREG;                                            /*!< Config Register, offset: 0xC */
    kuint32_t INTREG;                                               /*!< Interrupt Control Register, offset: 0x10 */
    kuint32_t DMAREG;                                               /*!< DMA Control Register, offset: 0x14 */
    kuint32_t STATREG;                                              /*!< Status Register, offset: 0x18 */
    kuint32_t PERIODREG;                                            /*!< Sample Period Control Register, offset: 0x1C */
    kuint32_t TESTREG;                                              /*!< Test Control Register, offset: 0x20 */
    kuint8_t  RESERVED_0[28];
    kuint32_t MSGDATA;                                              /*!< Message Data Register, offset: 0x40 */
} srt_imx_ecspi_t;

/*!< Peripheral ECSPI base address */
#define IMX6UL_ECSPI1_ADDR_BASE                                     (0x2008000u)
#define IMX6UL_ECSPI2_ADDR_BASE                                     (0x200C000u)
#define IMX6UL_ECSPI3_ADDR_BASE                                     (0x2010000u)
#define IMX6UL_ECSPI4_ADDR_BASE                                     (0x2014000u)
#define IMX6UL_ECSPI_PROPERTY_ENTRY(x)                              (srt_imx_ecspi_t *)IMX6UL_ECSPI##x##_ADDR_BASE

/*!< ------------------------------------------------------------------------- */
/*!< ENET - Register Layout Typedef */
typedef struct 
{
    kuint8_t  RESERVED_0[4];
    kuint32_t EIR;                                                  /*!< Interrupt Event Register, offset: 0x4 */
    kuint32_t EIMR;                                                 /*!< Interrupt Mask Register, offset: 0x8 */
    kuint8_t  RESERVED_1[4];
    kuint32_t RDAR;                                                 /*!< Receive Descriptor Active Register, offset: 0x10 */
    kuint32_t TDAR;                                                 /*!< Transmit Descriptor Active Register, offset: 0x14 */
    kuint8_t  RESERVED_2[12];
    kuint32_t ECR;                                                  /*!< Ethernet Control Register, offset: 0x24 */
    kuint8_t  RESERVED_3[24];
    kuint32_t MMFR;                                                 /*!< MII Management Frame Register, offset: 0x40 */
    kuint32_t MSCR;                                                 /*!< MII Speed Control Register, offset: 0x44 */
    kuint8_t  RESERVED_4[28];
    kuint32_t MIBC;                                                 /*!< MIB Control Register, offset: 0x64 */
    kuint8_t  RESERVED_5[28];
    kuint32_t RCR;                                                  /*!< Receive Control Register, offset: 0x84 */
    kuint8_t  RESERVED_6[60];
    kuint32_t TCR;                                                  /*!< Transmit Control Register, offset: 0xC4 */
    kuint8_t  RESERVED_7[28];
    kuint32_t PALR;                                                 /*!< Physical Address Lower Register, offset: 0xE4 */
    kuint32_t PAUR;                                                 /*!< Physical Address Upper Register, offset: 0xE8 */
    kuint32_t OPD;                                                  /*!< Opcode/Pause Duration Register, offset: 0xEC */
    kuint32_t TXIC;                                                 /*!< Transmit Interrupt Coalescing Register, offset: 0xF0 */
    kuint8_t  RESERVED_8[12];
    kuint32_t RXIC;                                                 /*!< Receive Interrupt Coalescing Register, offset: 0x100 */
    kuint8_t  RESERVED_9[20];
    kuint32_t IAUR;                                                 /*!< Descriptor Individual Upper Address Register, offset: 0x118 */
    kuint32_t IALR;                                                 /*!< Descriptor Individual Lower Address Register, offset: 0x11C */
    kuint32_t GAUR;                                                 /*!< Descriptor Group Upper Address Register, offset: 0x120 */
    kuint32_t GALR;                                                 /*!< Descriptor Group Lower Address Register, offset: 0x124 */
    kuint8_t  RESERVED_10[28];
    kuint32_t TFWR;                                                 /*!< Transmit FIFO Watermark Register, offset: 0x144 */
    kuint8_t  RESERVED_11[56];
    kuint32_t RDSR;                                                 /*!< Receive Descriptor Ring Start Register, offset: 0x180 */
    kuint32_t TDSR;                                                 /*!< Transmit Buffer Descriptor Ring Start Register, offset: 0x184 */
    kuint32_t MRBR;                                                 /*!< Maximum Receive Buffer Size Register, offset: 0x188 */
    kuint8_t  RESERVED_12[4];
    kuint32_t RSFL;                                                 /*!< Receive FIFO Section Full Threshold, offset: 0x190 */
    kuint32_t RSEM;                                                 /*!< Receive FIFO Section Empty Threshold, offset: 0x194 */
    kuint32_t RAEM;                                                 /*!< Receive FIFO Almost Empty Threshold, offset: 0x198 */
    kuint32_t RAFL;                                                 /*!< Receive FIFO Almost Full Threshold, offset: 0x19C */
    kuint32_t TSEM;                                                 /*!< Transmit FIFO Section Empty Threshold, offset: 0x1A0 */
    kuint32_t TAEM;                                                 /*!< Transmit FIFO Almost Empty Threshold, offset: 0x1A4 */
    kuint32_t TAFL;                                                 /*!< Transmit FIFO Almost Full Threshold, offset: 0x1A8 */
    kuint32_t TIPG;                                                 /*!< Transmit Inter-Packet Gap, offset: 0x1AC */
    kuint32_t FTRL;                                                 /*!< Frame Truncation Length, offset: 0x1B0 */
    kuint8_t  RESERVED_13[12];
    kuint32_t TACC;                                                 /*!< Transmit Accelerator Function Configuration, offset: 0x1C0 */
    kuint32_t RACC;                                                 /*!< Receive Accelerator Function Configuration, offset: 0x1C4 */
    kuint8_t  RESERVED_14[56];
    kuint32_t RMON_T_DROP;                                          /*!< Reserved Statistic Register, offset: 0x200 */
    kuint32_t RMON_T_PACKETS;                                       /*!< Tx Packet Count Statistic Register, offset: 0x204 */
    kuint32_t RMON_T_BC_PKT;                                        /*!< Tx Broadcast Packets Statistic Register, offset: 0x208 */
    kuint32_t RMON_T_MC_PKT;                                        /*!< Tx Multicast Packets Statistic Register, offset: 0x20C */
    kuint32_t RMON_T_CRC_ALIGN;                                     /*!< Tx Packets with CRC/Align Error Statistic Register, offset: 0x210 */
    kuint32_t RMON_T_UNDERSIZE;                                     /*!< Tx Packets Less Than Bytes and Good CRC Statistic Register, offset: 0x214 */
    kuint32_t RMON_T_OVERSIZE;                                      /*!< Tx Packets GT MAX_FL bytes and Good CRC Statistic Register, offset: 0x218 */
    kuint32_t RMON_T_FRAG;                                          /*!< Tx Packets Less Than 64 Bytes and Bad CRC Statistic Register, offset: 0x21C */
    kuint32_t RMON_T_JAB;                                           /*!< Tx Packets Greater Than MAX_FL bytes and Bad CRC Statistic Register, offset: 0x220 */
    kuint32_t RMON_T_COL;                                           /*!< Tx Collision Count Statistic Register, offset: 0x224 */
    kuint32_t RMON_T_P64;                                           /*!< Tx 64-Byte Packets Statistic Register, offset: 0x228 */
    kuint32_t RMON_T_P65TO127;                                      /*!< Tx 65- to 127-byte Packets Statistic Register, offset: 0x22C */
    kuint32_t RMON_T_P128TO255;                                     /*!< Tx 128- to 255-byte Packets Statistic Register, offset: 0x230 */
    kuint32_t RMON_T_P256TO511;                                     /*!< Tx 256- to 511-byte Packets Statistic Register, offset: 0x234 */
    kuint32_t RMON_T_P512TO1023;                                    /*!< Tx 512- to 1023-byte Packets Statistic Register, offset: 0x238 */
    kuint32_t RMON_T_P1024TO2047;                                   /*!< Tx 1024- to 2047-byte Packets Statistic Register, offset: 0x23C */
    kuint32_t RMON_T_P_GTE2048;                                     /*!< Tx Packets Greater Than 2048 Bytes Statistic Register, offset: 0x240 */
    kuint32_t RMON_T_OCTETS;                                        /*!< Tx Octets Statistic Register, offset: 0x244 */
    kuint32_t IEEE_T_DROP;                                          /*!< Reserved Statistic Register, offset: 0x248 */
    kuint32_t IEEE_T_FRAME_OK;                                      /*!< Frames Transmitted OK Statistic Register, offset: 0x24C */
    kuint32_t IEEE_T_1COL;                                          /*!< Frames Transmitted with Single Collision Statistic Register, offset: 0x250 */
    kuint32_t IEEE_T_MCOL;                                          /*!< Frames Transmitted with Multiple Collisions Statistic Register, offset: 0x254 */
    kuint32_t IEEE_T_DEF;                                           /*!< Frames Transmitted after Deferral Delay Statistic Register, offset: 0x258 */
    kuint32_t IEEE_T_LCOL;                                          /*!< Frames Transmitted with Late Collision Statistic Register, offset: 0x25C */
    kuint32_t IEEE_T_EXCOL;                                         /*!< Frames Transmitted with Excessive Collisions Statistic Register, offset: 0x260 */
    kuint32_t IEEE_T_MACERR;                                        /*!< Frames Transmitted with Tx FIFO Underrun Statistic Register, offset: 0x264 */
    kuint32_t IEEE_T_CSERR;                                         /*!< Frames Transmitted with Carrier Sense Error Statistic Register, offset: 0x268 */
    kuint32_t IEEE_T_SQE;                                           /*!< Reserved Statistic Register, offset: 0x26C */
    kuint32_t IEEE_T_FDXFC;                                         /*!< Flow Control Pause Frames Transmitted Statistic Register, offset: 0x270 */
    kuint32_t IEEE_T_OCTETS_OK;                                     /*!< Octet Count for Frames Transmitted w/o Error Statistic Register, offset: 0x274 */
    kuint8_t  RESERVED_15[12];
    kuint32_t RMON_R_PACKETS;                                       /*!< Rx Packet Count Statistic Register, offset: 0x284 */
    kuint32_t RMON_R_BC_PKT;                                        /*!< Rx Broadcast Packets Statistic Register, offset: 0x288 */
    kuint32_t RMON_R_MC_PKT;                                        /*!< Rx Multicast Packets Statistic Register, offset: 0x28C */
    kuint32_t RMON_R_CRC_ALIGN;                                     /*!< Rx Packets with CRC/Align Error Statistic Register, offset: 0x290 */
    kuint32_t RMON_R_UNDERSIZE;                                     /*!< Rx Packets with Less Than 64 Bytes and Good CRC Statistic Register, offset: 0x294 */
    kuint32_t RMON_R_OVERSIZE;                                      /*!< Rx Packets Greater Than MAX_FL and Good CRC Statistic Register, offset: 0x298 */
    kuint32_t RMON_R_FRAG;                                          /*!< Rx Packets Less Than 64 Bytes and Bad CRC Statistic Register, offset: 0x29C */
    kuint32_t RMON_R_JAB;                                           /*!< Rx Packets Greater Than MAX_FL Bytes and Bad CRC Statistic Register, offset: 0x2A0 */
    kuint32_t RMON_R_RESVD_0;                                       /*!< Reserved Statistic Register, offset: 0x2A4 */
    kuint32_t RMON_R_P64;                                           /*!< Rx 64-Byte Packets Statistic Register, offset: 0x2A8 */
    kuint32_t RMON_R_P65TO127;                                      /*!< Rx 65- to 127-Byte Packets Statistic Register, offset: 0x2AC */
    kuint32_t RMON_R_P128TO255;                                     /*!< Rx 128- to 255-Byte Packets Statistic Register, offset: 0x2B0 */
    kuint32_t RMON_R_P256TO511;                                     /*!< Rx 256- to 511-Byte Packets Statistic Register, offset: 0x2B4 */
    kuint32_t RMON_R_P512TO1023;                                    /*!< Rx 512- to 1023-Byte Packets Statistic Register, offset: 0x2B8 */
    kuint32_t RMON_R_P1024TO2047;                                   /*!< Rx 1024- to 2047-Byte Packets Statistic Register, offset: 0x2BC */
    kuint32_t RMON_R_P_GTE2048;                                     /*!< Rx Packets Greater than 2048 Bytes Statistic Register, offset: 0x2C0 */
    kuint32_t RMON_R_OCTETS;                                        /*!< Rx Octets Statistic Register, offset: 0x2C4 */
    kuint32_t IEEE_R_DROP;                                          /*!< Frames not Counted Correctly Statistic Register, offset: 0x2C8 */
    kuint32_t IEEE_R_FRAME_OK;                                      /*!< Frames Received OK Statistic Register, offset: 0x2CC */
    kuint32_t IEEE_R_CRC;                                           /*!< Frames Received with CRC Error Statistic Register, offset: 0x2D0 */
    kuint32_t IEEE_R_ALIGN;                                         /*!< Frames Received with Alignment Error Statistic Register, offset: 0x2D4 */
    kuint32_t IEEE_R_MACERR;                                        /*!< Receive FIFO Overflow Count Statistic Register, offset: 0x2D8 */
    kuint32_t IEEE_R_FDXFC;                                         /*!< Flow Control Pause Frames Received Statistic Register, offset: 0x2DC */
    kuint32_t IEEE_R_OCTETS_OK;                                     /*!< Octet Count for Frames Received without Error Statistic Register, offset: 0x2E0 */
    kuint8_t  RESERVED_16[284];
    kuint32_t ATCR;                                                 /*!< Adjustable Timer Control Register, offset: 0x400 */
    kuint32_t ATVR;                                                 /*!< Timer Value Register, offset: 0x404 */
    kuint32_t ATOFF;                                                /*!< Timer Offset Register, offset: 0x408 */
    kuint32_t ATPER;                                                /*!< Timer Period Register, offset: 0x40C */
    kuint32_t ATCOR;                                                /*!< Timer Correction Register, offset: 0x410 */
    kuint32_t ATINC;                                                /*!< Time-Stamping Clock Period Register, offset: 0x414 */
    kuint32_t ATSTMP;                                               /*!< Timestamp of Last Transmitted Frame, offset: 0x418 */
    kuint8_t  RESERVED_17[488];
    kuint32_t TGSR;                                                 /*!< Timer Global Status Register, offset: 0x604 */

    struct 
    {                                                               /*!< offset: 0x608, array step: 0x8 */
        kuint32_t TCSR;                                             /*!< Timer Control Status Register, array offset: 0x608, array step: 0x8 */
        kuint32_t TCCR;                                             /*!< Timer Compare Capture Register, array offset: 0x60C, array step: 0x8 */
    } CHANNEL[4];

} srt_imx_enet_t;

/*!< Peripheral ENET base address */
#define IMX6UL_ENET1_ADDR_BASE                                      (0x2188000u)
#define IMX6UL_ENET2_ADDR_BASE                                      (0x20B4000u)
#define IMX6UL_ENET_PROPERTY_ENTRY(x)                               (srt_imx_enet_t *)IMX6UL_ENET##x##_ADDR_BASE

/*!< ------------------------------------------------------------------------- */
/*!< USB - Register Layout Typedef */
typedef struct 
{
    kuint32_t ID;                                                   /*!< Identification register, offset: 0x0 */
    kuint32_t HWGENERAL;                                            /*!< Hardware General, offset: 0x4 */
    kuint32_t HWHOST;                                               /*!< Host Hardware Parameters, offset: 0x8 */
    kuint32_t HWDEVICE;                                             /*!< Device Hardware Parameters, offset: 0xC */
    kuint32_t HWTXBUF;                                              /*!< TX Buffer Hardware Parameters, offset: 0x10 */
    kuint32_t HWRXBUF;                                              /*!< RX Buffer Hardware Parameters, offset: 0x14 */
    kuint8_t  RESERVED_0[104];
    kuint32_t GPTIMER0LD;                                           /*!< General Purpose Timer #0 Load, offset: 0x80 */
    kuint32_t GPTIMER0CTRL;                                         /*!< General Purpose Timer #0 Controller, offset: 0x84 */
    kuint32_t GPTIMER1LD;                                           /*!< General Purpose Timer #1 Load, offset: 0x88 */
    kuint32_t GPTIMER1CTRL;                                         /*!< General Purpose Timer #1 Controller, offset: 0x8C */
    kuint32_t SBUSCFG;                                              /*!< System Bus Config, offset: 0x90 */
    kuint8_t  RESERVED_1[108];
    kuint8_t  CAPLENGTH;                                            /*!< Capability Registers Length, offset: 0x100 */
    kuint8_t  RESERVED_2[1];
    kuint16_t HCIVERSION;                                           /*!< Host Controller Interface Version, offset: 0x102 */
    kuint32_t HCSPARAMS;                                            /*!< Host Controller Structural Parameters, offset: 0x104 */
    kuint32_t HCCPARAMS;                                            /*!< Host Controller Capability Parameters, offset: 0x108 */
    kuint8_t  RESERVED_3[20];
    kuint16_t DCIVERSION;                                           /*!< Device Controller Interface Version, offset: 0x120 */
    kuint8_t  RESERVED_4[2];
    kuint32_t DCCPARAMS;                                            /*!< Device Controller Capability Parameters, offset: 0x124 */
    kuint8_t  RESERVED_5[24];
    kuint32_t USBCMD;                                               /*!< USB Command Register, offset: 0x140 */
    kuint32_t USBSTS;                                               /*!< USB Status Register, offset: 0x144 */
    kuint32_t USBINTR;                                              /*!< Interrupt Enable Register, offset: 0x148 */
    kuint32_t FRINDEX;                                              /*!< USB Frame Index, offset: 0x14C */
    kuint8_t  RESERVED_6[4];
  
    union 
    {                                                               /*!< offset: 0x154 */
        kuint32_t DEVICEADDR;                                       /*!< Device Address, offset: 0x154 */
        kuint32_t PERIODICLISTBASE;                                 /*!< Frame List Base Address, offset: 0x154 */
    };

    union 
    {                                                               /*!< offset: 0x158 */
        kuint32_t ASYNCLISTADDR;                                    /*!< Next Asynch. Address, offset: 0x158 */
        kuint32_t ENDPTLISTADDR;                                    /*!< Endpoint List Address, offset: 0x158 */
    };

    kuint8_t  RESERVED_7[4];
    kuint32_t BURSTSIZE;                                            /*!< Programmable Burst Size, offset: 0x160 */
    kuint32_t TXFILLTUNING;                                         /*!< TX FIFO Fill Tuning, offset: 0x164 */
    kuint8_t  RESERVED_8[16];
    kuint32_t ENDPTNAK;                                             /*!< Endpoint NAK, offset: 0x178 */
    kuint32_t ENDPTNAKEN;                                           /*!< Endpoint NAK Enable, offset: 0x17C */
    kuint32_t CONFIGFLAG;                                           /*!< Configure Flag Register, offset: 0x180 */
    kuint32_t PORTSC1;                                              /*!< Port Status & Control, offset: 0x184 */
    kuint8_t  RESERVED_9[28];
    kuint32_t OTGSC;                                                /*!< On-The-Go Status & control, offset: 0x1A4 */
    kuint32_t USBMODE;                                              /*!< USB Device Mode, offset: 0x1A8 */
    kuint32_t ENDPTSETUPSTAT;                                       /*!< Endpoint Setup Status, offset: 0x1AC */
    kuint32_t ENDPTPRIME;                                           /*!< Endpoint Prime, offset: 0x1B0 */
    kuint32_t ENDPTFLUSH;                                           /*!< Endpoint Flush, offset: 0x1B4 */
    kuint32_t ENDPTSTAT;                                            /*!< Endpoint Status, offset: 0x1B8 */
    kuint32_t ENDPTCOMPLETE;                                        /*!< Endpoint Complete, offset: 0x1BC */
    kuint32_t ENDPTCTRL0;                                           /*!< Endpoint Control0, offset: 0x1C0 */
    kuint32_t ENDPTCTRL[7];                                         /*!< Endpoint Control 1..Endpoint Control 7, array offset: 0x1C4, array step: 0x4 */
} srt_imx_usbotg_t;

/*!< Peripheral USBOTG base address */
#define IMX6UL_USBOTG1_ADDR_BASE                                    (0x2184000u)
#define IMX6UL_USBOTG2_ADDR_BASE                                    (0x2184200u)
#define IMX6UL_USBOTG_PROPERTY_ENTRY(x)                             (srt_imx_usbotg_t *)IMX6UL_USBOTG##x##_ADDR_BASE

typedef enum __ERT_IMX_USBOTG_INTR_BIT
{
    NR_ImxUsbOtgIntr_UsbIntBit = mrt_bit(0U),
    NR_ImxUsbOtgIntr_UsbErrIntBit = mrt_bit(1U),
    NR_ImxUsbOtgIntr_PortChangeDetectIntBit = mrt_bit(2U),
    NR_ImxUsbOtgIntr_FrameRolloverIntBit = mrt_bit(3U),
    NR_ImxUsbOtgIntr_SystemErrIntBit = mrt_bit(4U),
    NR_ImxUsbOtgIntr_AsyncAdvanceIntBit = mrt_bit(5U),
    NR_ImxUsbOtgIntr_UsbResetIntBit = mrt_bit(6U),
    NR_ImxUsbOtgIntr_SofRecvIntBit = mrt_bit(7U),
    NR_ImxUsbOtgIntr_SleepIntBit = mrt_bit(8U),
    NR_ImxUsbOtgIntr_UlpiIntBit = mrt_bit(10U),
    NR_ImxUsbOtgIntr_NakIntBit = mrt_bit(16U),
    NR_ImxUsbOtgIntr_UsbHostAsyncIntBit = mrt_bit(18U),
    NR_ImxUsbOtgIntr_UsbHostPeriodicIntBit = mrt_bit(19U),
    NR_ImxUsbOtgIntr_GpTimer0IntBit = mrt_bit(24U),
    NR_ImxUsbOtgIntr_GpTimer1IntBit = mrt_bit(25U),

} ert_imx_usb_intr_t;

/*!< ------------------------------------------------------------------------- */
/*!< USBNC - Register Layout Typedef */
typedef struct 
{
    kuint32_t USB_OTGn_CTRL;                                        /*!< USB OTGn Control Register, offset: 0x0 */
    kuint8_t  RESERVED_0[20];
    kuint32_t USB_OTGn_PHY_CTRL_0;                                  /*!< OTGn UTMI PHY Control 0 Register, offset: 0x18 */
} srt_imx_usbnc_t;

/*!< Peripheral USBNC base address */
#define IMX6UL_USBNC1_ADDR_BASE                                     (0x2184800u)
#define IMX6UL_USBNC2_ADDR_BASE                                     (0x2184804u)
#define IMX6UL_USBNC_PROPERTY_ENTRY(x)                              (srt_imx_usbnc_t *)IMX6UL_USBNC##x##_ADDR_BASE

/*!< ------------------------------------------------------------------------- */
/*!< USBPHY - Register Layout Typedef */
typedef struct 
{
    kuint32_t PWD;                                                  /*!< USB PHY Power-Down Register, offset: 0x0 */
    kuint32_t PWD_SET;                                              /*!< USB PHY Power-Down Register, offset: 0x4 */
    kuint32_t PWD_CLR;                                              /*!< USB PHY Power-Down Register, offset: 0x8 */
    kuint32_t PWD_TOG;                                              /*!< USB PHY Power-Down Register, offset: 0xC */
    kuint32_t TX;                                                   /*!< USB PHY Transmitter Control Register, offset: 0x10 */
    kuint32_t TX_SET;                                               /*!< USB PHY Transmitter Control Register, offset: 0x14 */
    kuint32_t TX_CLR;                                               /*!< USB PHY Transmitter Control Register, offset: 0x18 */
    kuint32_t TX_TOG;                                               /*!< USB PHY Transmitter Control Register, offset: 0x1C */
    kuint32_t RX;                                                   /*!< USB PHY Receiver Control Register, offset: 0x20 */
    kuint32_t RX_SET;                                               /*!< USB PHY Receiver Control Register, offset: 0x24 */
    kuint32_t RX_CLR;                                               /*!< USB PHY Receiver Control Register, offset: 0x28 */
    kuint32_t RX_TOG;                                               /*!< USB PHY Receiver Control Register, offset: 0x2C */
    kuint32_t CTRL;                                                 /*!< USB PHY General Control Register, offset: 0x30 */
    kuint32_t CTRL_SET;                                             /*!< USB PHY General Control Register, offset: 0x34 */
    kuint32_t CTRL_CLR;                                             /*!< USB PHY General Control Register, offset: 0x38 */
    kuint32_t CTRL_TOG;                                             /*!< USB PHY General Control Register, offset: 0x3C */
    kuint32_t STATUS;                                               /*!< USB PHY Status Register, offset: 0x40 */
    kuint8_t  RESERVED_0[12];
    kuint32_t DEBUGr;                                               /*!< USB PHY Debug Register, offset: 0x50 */
    kuint32_t DEBUG_SET;                                            /*!< USB PHY Debug Register, offset: 0x54 */
    kuint32_t DEBUG_CLR;                                            /*!< USB PHY Debug Register, offset: 0x58 */
    kuint32_t DEBUG_TOG;                                            /*!< USB PHY Debug Register, offset: 0x5C */
    kuint32_t DEBUG0_STATUS;                                        /*!< UTMI Debug Status Register 0, offset: 0x60 */
    kuint8_t  RESERVED_1[12];
    kuint32_t DEBUG1;                                               /*!< UTMI Debug Status Register 1, offset: 0x70 */
    kuint32_t DEBUG1_SET;                                           /*!< UTMI Debug Status Register 1, offset: 0x74 */
    kuint32_t DEBUG1_CLR;                                           /*!< UTMI Debug Status Register 1, offset: 0x78 */
    kuint32_t DEBUG1_TOG;                                           /*!< UTMI Debug Status Register 1, offset: 0x7C */
    kuint32_t VERSION;                                              /*!< UTMI RTL Version, offset: 0x80 */
} srt_imx_usbphy_t;

/*!< Peripheral USBPHY base address */
#define IMX6UL_USBPHY1_ADDR_BASE                                    (0x20C9000u)
#define IMX6UL_USBPHY2_ADDR_BASE                                    (0x20CA000u)
#define IMX6UL_USBPHY_PROPERTY_ENTRY(x)                             (srt_imx_usbphy_t *)IMX6UL_USBPHY##x##_ADDR_BASE

/*!< ------------------------------------------------------------------------- */
/*!< LCDIF - Register Layout Typedef */
typedef struct 
{
    kuint32_t CTRL;                                                 /*!< eLCDIF General Control Register, offset: 0x0 */
    kuint32_t CTRL_SET;                                             /*!< eLCDIF General Control Register, offset: 0x4 */
    kuint32_t CTRL_CLR;                                             /*!< eLCDIF General Control Register, offset: 0x8 */
    kuint32_t CTRL_TOG;                                             /*!< eLCDIF General Control Register, offset: 0xC */
    kuint32_t CTRL1;                                                /*!< eLCDIF General Control1 Register, offset: 0x10 */
    kuint32_t CTRL1_SET;                                            /*!< eLCDIF General Control1 Register, offset: 0x14 */
    kuint32_t CTRL1_CLR;                                            /*!< eLCDIF General Control1 Register, offset: 0x18 */
    kuint32_t CTRL1_TOG;                                            /*!< eLCDIF General Control1 Register, offset: 0x1C */
    kuint32_t CTRL2;                                                /*!< eLCDIF General Control2 Register, offset: 0x20 */
    kuint32_t CTRL2_SET;                                            /*!< eLCDIF General Control2 Register, offset: 0x24 */
    kuint32_t CTRL2_CLR;                                            /*!< eLCDIF General Control2 Register, offset: 0x28 */
    kuint32_t CTRL2_TOG;                                            /*!< eLCDIF General Control2 Register, offset: 0x2C */
    kuint32_t TRANSFER_COUNT;                                       /*!< eLCDIF Horizontal and Vertical Valid Data Count Register, offset: 0x30 */
    kuint8_t  RESERVED_0[12];
    kuint32_t CUR_BUF;                                              /*!< LCD Interface Current Buffer Address Register, offset: 0x40 */
    kuint8_t  RESERVED_1[12];
    kuint32_t NEXT_BUF;                                             /*!< LCD Interface Next Buffer Address Register, offset: 0x50 */
    kuint8_t  RESERVED_2[12];
    kuint32_t TIMING;                                               /*!< LCD Interface Timing Register, offset: 0x60 */
    kuint8_t  RESERVED_3[12];
    kuint32_t VDCTRL0;                                              /*!< eLCDIF VSYNC Mode and Dotclk Mode Control Register0, offset: 0x70 */
    kuint32_t VDCTRL0_SET;                                          /*!< eLCDIF VSYNC Mode and Dotclk Mode Control Register0, offset: 0x74 */
    kuint32_t VDCTRL0_CLR;                                          /*!< eLCDIF VSYNC Mode and Dotclk Mode Control Register0, offset: 0x78 */
    kuint32_t VDCTRL0_TOG;                                          /*!< eLCDIF VSYNC Mode and Dotclk Mode Control Register0, offset: 0x7C */
    kuint32_t VDCTRL1;                                              /*!< eLCDIF VSYNC Mode and Dotclk Mode Control Register1, offset: 0x80 */
    kuint8_t  RESERVED_4[12];
    kuint32_t VDCTRL2;                                              /*!< LCDIF VSYNC Mode and Dotclk Mode Control Register2, offset: 0x90 */
    kuint8_t  RESERVED_5[12];
    kuint32_t VDCTRL3;                                              /*!< eLCDIF VSYNC Mode and Dotclk Mode Control Register3, offset: 0xA0 */
    kuint8_t  RESERVED_6[12];
    kuint32_t VDCTRL4;                                              /*!< eLCDIF VSYNC Mode and Dotclk Mode Control Register4, offset: 0xB0 */
    kuint8_t  RESERVED_7[12];
    kuint32_t DVICTRL0;                                             /*!< Digital Video Interface Control0 Register, offset: 0xC0 */
    kuint8_t  RESERVED_8[12];
    kuint32_t DVICTRL1;                                             /*!< Digital Video Interface Control1 Register, offset: 0xD0 */
    kuint8_t  RESERVED_9[12];
    kuint32_t DVICTRL2;                                             /*!< Digital Video Interface Control2 Register, offset: 0xE0 */
    kuint8_t  RESERVED_10[12];
    kuint32_t DVICTRL3;                                             /*!< Digital Video Interface Control3 Register, offset: 0xF0 */
    kuint8_t  RESERVED_11[12];
    kuint32_t DVICTRL4;                                             /*!< Digital Video Interface Control4 Register, offset: 0x100 */
    kuint8_t  RESERVED_12[12];
    kuint32_t CSC_COEFF0;                                           /*!< RGB to YCbCr 4:2:2 CSC Coefficient0 Register, offset: 0x110 */
    kuint8_t  RESERVED_13[12];
    kuint32_t CSC_COEFF1;                                           /*!< RGB to YCbCr 4:2:2 CSC Coefficient1 Register, offset: 0x120 */
    kuint8_t  RESERVED_14[12];
    kuint32_t CSC_COEFF2;                                           /*!< RGB to YCbCr 4:2:2 CSC Coefficent2 Register, offset: 0x130 */
    kuint8_t  RESERVED_15[12];
    kuint32_t CSC_COEFF3;                                           /*!< RGB to YCbCr 4:2:2 CSC Coefficient3 Register, offset: 0x140 */
    kuint8_t  RESERVED_16[12];
    kuint32_t CSC_COEFF4;                                           /*!< RGB to YCbCr 4:2:2 CSC Coefficient4 Register, offset: 0x150 */
    kuint8_t  RESERVED_17[12];
    kuint32_t CSC_OFFSET;                                           /*!< RGB to YCbCr 4:2:2 CSC Offset Register, offset: 0x160 */
    kuint8_t  RESERVED_18[12];
    kuint32_t CSC_LIMIT;                                            /*!< RGB to YCbCr 4:2:2 CSC Limit Register, offset: 0x170 */
    kuint8_t  RESERVED_19[12];
    kuint32_t DATA;                                                 /*!< LCD Interface Data Register, offset: 0x180 */
    kuint8_t  RESERVED_20[12];
    kuint32_t BM_ERROR_STAT;                                        /*!< Bus Master Error Status Register, offset: 0x190 */
    kuint8_t  RESERVED_21[12];
    kuint32_t CRC_STAT;                                             /*!< CRC Status Register, offset: 0x1A0 */
    kuint8_t  RESERVED_22[12];
    kuint32_t STAT;                                                 /*!< LCD Interface Status Register, offset: 0x1B0 */
    kuint8_t  RESERVED_23[76];
    kuint32_t THRES;                                                /*!< eLCDIF Threshold Register, offset: 0x200 */
    kuint8_t  RESERVED_24[12];
    kuint32_t AS_CTRL;                                              /*!< eLCDIF AS Buffer Control Register, offset: 0x210 */
    kuint8_t  RESERVED_25[12];
    kuint32_t AS_BUF;                                               /*!< Alpha Surface Buffer Pointer, offset: 0x220 */
    kuint8_t  RESERVED_26[12];
    kuint32_t AS_NEXT_BUF;                                          /*!< offset: 0x230 */
    kuint8_t  RESERVED_27[12];
    kuint32_t AS_CLRKEYLOW;                                         /*!< eLCDIF Overlay Color Key Low, offset: 0x240 */
    kuint8_t  RESERVED_28[12];
    kuint32_t AS_CLRKEYHIGH;                                        /*!< eLCDIF Overlay Color Key High, offset: 0x250 */
    kuint8_t  RESERVED_29[12];
    kuint32_t SYNC_DELAY;                                           /*!< LCD working insync mode with CSI for VSYNC delay, offset: 0x260 */
} srt_imx_lcdif_t;

/*!< Peripheral LCDIF base address */
#define IMX6UL_LCDIF_ADDR_BASE                                      (0x21C8000u)
#define IMX6UL_LCDIF_PROPERTY_ENTRY()                               (srt_imx_lcdif_t *)IMX6UL_LCDIF_ADDR_BASE

/*!< ------------------------------------------------------------------------- */
/*!< USDHC - Register Layout Typedef */
typedef struct 
{
    kuint32_t DS_ADDR;                                              /*!< DMA System Address, offset: 0x0 */
    kuint32_t BLK_ATT;                                              /*!< Block Attributes, offset: 0x4 */
    kuint32_t CMD_ARG;                                              /*!< Command Argument, offset: 0x8 */
    kuint32_t CMD_XFR_TYP;                                          /*!< Command Transfer Type, offset: 0xC */
    kuint32_t CMD_RSP0;                                             /*!< Command Response0, offset: 0x10 */
    kuint32_t CMD_RSP1;                                             /*!< Command Response1, offset: 0x14 */
    kuint32_t CMD_RSP2;                                             /*!< Command Response2, offset: 0x18 */
    kuint32_t CMD_RSP3;                                             /*!< Command Response3, offset: 0x1C */
    kuint32_t DATA_BUFF_ACC_PORT;                                   /*!< Data Buffer Access Port, offset: 0x20 */
    kuint32_t PRES_STATE;                                           /*!< Present State, offset: 0x24 */
    kuint32_t PROT_CTRL;                                            /*!< Protocol Control, offset: 0x28 */
    kuint32_t SYS_CTRL;                                             /*!< System Control, offset: 0x2C */
    kuint32_t INT_STATUS;                                           /*!< Interrupt Status, offset: 0x30 */
    kuint32_t INT_STATUS_EN;                                        /*!< Interrupt Status Enable, offset: 0x34 */
    kuint32_t INT_SIGNAL_EN;                                        /*!< Interrupt Signal Enable, offset: 0x38 */
    kuint32_t AUTOCMD12_ERR_STATUS;                                 /*!< Auto CMD12 Error Status, offset: 0x3C */
    kuint32_t HOST_CTRL_CAP;                                        /*!< Host Controller Capabilities, offset: 0x40 */
    kuint32_t WTMK_LVL;                                             /*!< Watermark Level, offset: 0x44 */
    kuint32_t MIX_CTRL;                                             /*!< Mixer Control, offset: 0x48 */
    kuint8_t  RESERVED_0[4];
    kuint32_t FORCE_EVENT;                                          /*!< Force Event, offset: 0x50 */
    kuint32_t ADMA_ERR_STATUS;                                      /*!< ADMA Error Status Register, offset: 0x54 */
    kuint32_t ADMA_SYS_ADDR;                                        /*!< ADMA System Address, offset: 0x58 */
    kuint8_t  RESERVED_1[4];
    kuint32_t DLL_CTRL;                                             /*!< DLL (Delay Line) Control, offset: 0x60 */
    kuint32_t DLL_STATUS;                                           /*!< DLL Status, offset: 0x64 */
    kuint32_t CLK_TUNE_CTRL_STATUS;                                 /*!< CLK Tuning Control and Status, offset: 0x68 */
    kuint8_t  RESERVED_2[84];
    kuint32_t VEND_SPEC;                                            /*!< Vendor Specific Register, offset: 0xC0 */
    kuint32_t MMC_BOOT;                                             /*!< MMC Boot Register, offset: 0xC4 */
    kuint32_t VEND_SPEC2;                                           /*!< Vendor Specific 2 Register, offset: 0xC8 */
    kuint32_t TUNING_CTRL;                                          /*!< Tuning Control Register, offset: 0xCC */
} srt_imx_usdhc_t;

/*!< Peripheral USDHC base address */
#define IMX6UL_USDHC1_ADDR_BASE                                     (0x2190000u)
#define IMX6UL_USDHC2_ADDR_BASE                                     (0x2194000u)
#define IMX6UL_USDHC_PROPERTY_ENTRY(x)                              (srt_imx_usdhc_t *)IMX6UL_USDHC##x##_ADDR_BASE

/*!< USDHC Interrupt Status/Signal Bit */
/*!< for Registers: INT_SIGNAL_EN, INT_STATUS_EN, INT_STATUS */
enum __ERT_IMX_USDHC_INTERRUPT_BIT
{
    NR_ImxUsdhc_IntCmdComplete_Bit = mrt_bit(0U),                   /*!< Command Complete Status Enable */
    NR_ImxUsdhc_IntDataComplete_Bit = mrt_bit(1U),                  /*!< Transfer Complete Status Enable */
    NR_ImxUsdhc_IntBlockGapEvent_Bit = mrt_bit(2U),                 /*!< Block Gap Event Status Enable */
    NR_ImxUsdhc_IntDmaInterrupt_Bit = mrt_bit(3U),                  /*!< DMA Interrupt Status Enable */
    NR_ImxUsdhc_IntBufferWriteReady_Bit = mrt_bit(4U),              /*!< Buffer Write Ready Status Enable */
    NR_ImxUsdhc_IntBufferReadReady_Bit = mrt_bit(5U),               /*!< Buffer Read Ready Status Enable */
    NR_ImxUsdhc_IntCardInsertion_Bit = mrt_bit(6U),                 /*!< Card Insertion Status Enable */
    NR_ImxUsdhc_IntCardRemoval_Bit = mrt_bit(7U),                   /*!< Card Removal Status Enable */
    NR_ImxUsdhc_IntCardInterrupt_Bit = mrt_bit(8U),                 /*!< Card Interrupt Status Enable */
    NR_ImxUsdhc_IntReTuningEvent_Bit = mrt_bit(12U),                /*!< Re-Tuning Event Status Enable */
    NR_ImxUsdhc_IntTuningPass_Bit = mrt_bit(14U),                   /*!< Tuning Pass Status Enable */
    NR_ImxUsdhc_IntCmdTimeOutErr_Bit = mrt_bit(16U),                /*!< Command Timeout Error Status Enable */
    NR_ImxUsdhc_IntCmdCrcErr_Bit = mrt_bit(17U),                    /*!< Command CRC Error Status Enable */
    NR_ImxUsdhc_IntCmdEndBitErr_Bit = mrt_bit(18U),                 /*!< Command End Bit Error Status Enable */
    NR_ImxUsdhc_IntCmdIndexErr_Bit = mrt_bit(19U),                  /*!< Command Index Error Status Enable */
    NR_ImxUsdhc_IntDataTimeOutErr_Bit = mrt_bit(20U),               /*!< Data Timeout Error Status Enable */
    NR_ImxUsdhc_IntDataCrcErr_Bit = mrt_bit(21U),                   /*!< Data CRC Error Status Enable */
    NR_ImxUsdhc_IntDataEndBitErr_Bit = mrt_bit(22U),                /*!< Data End Bit Error Status Enable */
    NR_ImxUsdhc_IntACmd12Err_Bit = mrt_bit(24U),                    /*!< Auto CMD12 Error Status Enable */
    NR_ImxUsdhc_IntTuningErr_Bit = mrt_bit(26U),                    /*!< Tuning Error Status Enable */
    NR_ImxUsdhc_IntDmaErr_Bit = mrt_bit(28U),                       /*!< DMA Error Status Enable */

    NR_ImxUsdhc_IntCmdErr_Bit = (NR_ImxUsdhc_IntCmdIndexErr_Bit | NR_ImxUsdhc_IntCmdEndBitErr_Bit |
                                NR_ImxUsdhc_IntCmdCrcErr_Bit | NR_ImxUsdhc_IntCmdTimeOutErr_Bit),
    NR_ImxUsdhc_IntDataErr_Bit = (NR_ImxUsdhc_IntDataTimeOutErr_Bit | NR_ImxUsdhc_IntDataCrcErr_Bit |
                                NR_ImxUsdhc_IntDataEndBitErr_Bit | NR_ImxUsdhc_IntACmd12Err_Bit),
    NR_ImxUsdhc_IntAllErr_Bit = (NR_ImxUsdhc_IntCmdErr_Bit | NR_ImxUsdhc_IntDataErr_Bit | NR_ImxUsdhc_IntDmaErr_Bit),
    NR_ImxUsdhc_IntAllCmd_Bit = (NR_ImxUsdhc_IntCmdErr_Bit | NR_ImxUsdhc_IntCmdComplete_Bit),
    NR_ImxUsdhc_IntAllData_Bit = (NR_ImxUsdhc_IntDataErr_Bit | NR_ImxUsdhc_IntDataComplete_Bit),
    NR_ImxUsdhc_IntAllTuning_Bit = (NR_ImxUsdhc_IntReTuningEvent_Bit | NR_ImxUsdhc_IntTuningPass_Bit |
                                NR_ImxUsdhc_IntTuningErr_Bit),
    NR_ImxUsdhc_IntCardDetect_Bit = (NR_ImxUsdhc_IntCardInsertion_Bit | NR_ImxUsdhc_IntCardRemoval_Bit),

    NR_ImxUsdhc_IntAllInterrupt_Bit = (NR_ImxUsdhc_IntBlockGapEvent_Bit | NR_ImxUsdhc_IntCardInterrupt_Bit |
                                NR_ImxUsdhc_IntAllErr_Bit | NR_ImxUsdhc_IntAllCmd_Bit | NR_ImxUsdhc_IntAllData_Bit | 
                                NR_ImxUsdhc_IntAllTuning_Bit),

/*!< write 1 to clear */
#define mrt_imx_clear_interrupt_flags(flagBit, usdhc)                mrt_writel(flagBit, usdhc->INT_STATUS)
};

/*!< for Register: SYS_CTRL */
enum __ERT_IMX_USDHC_SYS_CTRL_BIT
{
/*!< 
 * DVS: bit[7:4]. Divisor
 * This register is used to select the frequency of the SDCLK pin. This register field selects the divide value
 * of the first divisor stage.
 * Before changing clock divisor value(SDCLKFS or DVS), Host Driver should make sure the SDSTB bit is
 * high.
 * dividers: 1(0h) ~ 16(fh)
 */
#define IMX_USDHC_SYS_CTRL_DVS_OFFSET                               (4U)
#define IMX_USDHC_SYS_CTRL_DVS_MASK                                 (0xf0U) 
#define IMX_USDHC_SYS_CTRL_DVS_BIT(x)                               mrt_bit_mask((x), IMX_USDHC_SYS_CTRL_DVS_MASK, IMX_USDHC_SYS_CTRL_DVS_OFFSET)
#define IMX_USDHC_SYS_CTRL_DVS_U32(x)                               (((x) >= 1) ? IMX_USDHC_SYS_CTRL_DVS_BIT((x) - 1) : IMX_USDHC_SYS_CTRL_DVS_BIT(1U))

/*!< 
 * SDCLKFS: bit[15:8]. SDCLK Frequency Select
 * This register is used to select the frequency of the SDCLK pin.This register field selects the divide value of
 * the second divisor stage.
 * In Single Data Rate mode(DDR_EN bit of MIX_CTRL is '0'): it can divided by 1(00h), 2(01h), 4(10h), ..., 128(40h), 256(80h)
 * While in Dual Data Rate mode(DDR_EN bit of MIX_CTRL is '1'): it can divided by 2(00h), 4(01h), 8(10h), ..., 256(40h), 512(80h)
 */
#define IMX_USDHC_SYS_CTRL_SDCLKFS_OFFSET                           (8U)
#define IMX_USDHC_SYS_CTRL_SDCLKFS_MASK                             (0xff00U) 
#define IMX_USDHC_SYS_CTRL_SDCLKFS_BIT(x)                           mrt_bit_mask((x), IMX_USDHC_SYS_CTRL_SDCLKFS_MASK, IMX_USDHC_SYS_CTRL_SDCLKFS_OFFSET)
#define IMX_USDHC_SYS_CTRL_SDCLKFS_U32(ddr_en, x)                   ((ddr_en) ? IMX_USDHC_SYS_CTRL_SDCLKFS_BIT((x) >> 2U) : IMX_USDHC_SYS_CTRL_SDCLKFS_BIT((x) >> 1U))

/*!< DTOCV: bit[19:16]. Data Timeout Counter Value */
#define IMX_USDHC_SYS_CTRL_DTOCV_OFFSET                             (16U)
#define IMX_USDHC_SYS_CTRL_DTOCV_MASK                               (0xf0000U) 
#define IMX_USDHC_SYS_CTRL_DTOCV_BIT(x)                             mrt_bit_mask((x), IMX_USDHC_SYS_CTRL_DTOCV_MASK, IMX_USDHC_SYS_CTRL_DTOCV_OFFSET)
#define IMX_USDHC_SYS_CTRL_DTOCV_U32(x)                             IMX_USDHC_SYS_CTRL_DTOCV_BIT(x)

    NR_ImxUsdhc_SysCtrl_SoftResetAll = mrt_bit(24U),                /*!< RSTA: Software Reset For ALL. 1: Reset; 0: No Reset */   
    NR_ImxUsdhc_SysCtrl_SoftResetCmdLine = mrt_bit(25U),            /*!< RSTC: Software Reset For CMD Line. 1: Reset; 0: No Reset */
    NR_ImxUsdhc_SysCtrl_SoftResetDataLine = mrt_bit(26U),           /*!< RSTD: Software Reset For DATA Line. 1: Reset; 0: No Reset */

    /*!< 
     * INITA: Initialization Active.
     * When this bit is set, 80 SD-Clocks are sent to the card. After the 80 clocks are sent, this bit is self cleared.
     * This bit is very useful during the card power-up period when 74 SD-Clocks are needed and the clock auto
     * gating feature is enabled. Writing 1 to this bit when this bit is already 1 has no effect. Writing 0 to this bit at
     * any time has no effect. When either of the CIHB and CDIHB bits in the Present State Register are set,
     * writing 1 to this bit is ignored (i.e. when command line or data lines are active, write to this bit is not
     * allowed). On the otherhand, when this bit is set, i.e., during intialization active period, it is allowed to issue
     * command, and the command bit stream will appear on the CMD pad after all 80 clock cycles are done. So
     * when this command ends, the driver can make sure the 80 clock cycles are sent out. This is very useful
     * when the driver needs send 80 cycles to the card and does not want to wait till this bit is self cleared
     */
    NR_ImxUsdhc_SysCtrl_InitialActive = mrt_bit(27U),

    /*!< 
     * RSTT: Reset Tuning. 
     * When set this bit to 1, it will reset tuning circuit. After tuning circuits are reset, bit value is 0 
     */
    NR_ImxUsdhc_SysCtrl_ResetTuning = mrt_bit(28U),

    NR_ImxUsdhc_SysCtrl_ResetMask = ((NR_ImxUsdhc_SysCtrl_SoftResetAll | NR_ImxUsdhc_SysCtrl_SoftResetCmdLine |
                                      NR_ImxUsdhc_SysCtrl_SoftResetDataLine | NR_ImxUsdhc_SysCtrl_ResetTuning) & 0x17000000U),
};

/*
 * for Register: MIX_CTRL
 * This register is used to DMA and data transfer
 */
enum __ERT_IMX_USDHC_MIX_CTRL_BIT
{
    /*!<
     * DMAEN: DMA Enable
     * This bit enables DMA functionality. If this bit is set to 1, a DMA operation shall begin when the Host Driver
     * sets the DPSEL bit of this register. Whether the Simple DMA or the Advanced DMA is active depends on
     * the DMA Select field of the Protocol Control register
     */
    NR_ImxUsdhc_MixCtrl_DmaEnable = mrt_bit(0U),

    /*!< 
     * BCEN: Block Count Enable
     * This bit is used to enable the Block Count register, which is only relevant for multiple block transfers.
     * When this bit is 0, the internal counter for block is disabled, which is useful in executing an infinite transfer
     */
    NR_ImxUsdhc_MixCtrl_BlockCountEnable = mrt_bit(1U),

    NR_ImxUsdhc_MixCtrl_AutoCmd12Enable = mrt_bit(2U),              /*!< Auto CMD12 Enable */
    NR_ImxUsdhc_MixCtrl_DualDataRateMode = mrt_bit(3U),             /*!< DDR_EN: Dual Data Rate mode selection */

    /*!< 
     * DTDSEL: Data Transfer Direction Select
     *  1, Read (Card to Host)
     *  0, Write (Host to Card)
     */
    NR_ImxUsdhc_MixCtrl_DataTransferDirection = mrt_bit(4U),

    /*!<
     * MSBSEL: Multi/Single Block Select
     * This bit enables multiple block DATA line data transfers. For any other commands, this bit can be set to 0.
     * If this bit is 0, it is not necessary to set the Block Count register (CMD_XFR_TYP)
     *  1, Multiple Blocks
     *  0, Single Block
     */
    NR_ImxUsdhc_MixCtrl_MultiSingleBlockSelect = mrt_bit(5U),

    /*!<
     * AC23EN
     * When this bit is set to 1, the Host Controller issues a CMD23 automatically before issuing a command
     * specified in the Command Register
     */
    NR_ImxUsdhc_MixCtrl_AutoCmd23Enable = mrt_bit(7U),

    /*!<
     * EXE_TUNE: Execute Tuning: (Only used for SD3.0, SDR104 mode)
     * When STD_TUNING_EN is 0, this bit is set to 1 to indicate the Host Driver is starting tuning procedure.
     * Tuning procedure is aborted by writing 0.
     *  1, Execute Tuning
     *  0, Not Tuned or Tuning Completed
     */
    NR_ImxUsdhc_MixCtrl_ExecuteTuning = mrt_bit(22U),

    /*!<
     * SMP_CLK_SEL: When STD_TUNING_EN is 0, this bit is used to select Tuned clock or Fixed clock to sample data / cmd
     * (Only used for SD3.0, SDR104 mode)
     *  1, Tuned clock is used to sample data/cmd
     *  0, Fixed clock is used to sample data/cmd
     */
    NR_ImxUsdhc_MixCtrl_SampleClkSelect = mrt_bit(23U),

    /*!<
     * AUTO_TUNE_EN: Auto Tuning Enable (Only used for SD3.0, SDR104 mode)
     *  1, Enable auto tuning
     *  0, Disable auto tuning
     */
    NR_ImxUsdhc_MixCtrl_AutoTuning = mrt_bit(24U),

    /*!<
     * FBCLK_SEL: Feedback Clock Source Selection (Only used for SD3.0, SDR104 mode)
     *  1, Feedback clock comes from the ipp_card_clk_out
     *  0, Feedback clock comes from the loopback CLK
     */
    NR_ImxUsdhc_MixCtrl_FeedbackClkSelect = mrt_bit(25U),
};

/*!< for Register: PROT_CTRL */
enum __ERT_IMX_USDHC_PROT_CTRL_BIT
{
    NR_ImxUsdhc_ProtCtrl_LedControl = mrt_bit(0U),                  /*!< LCTL. 1: LED on; 0: LED off */

/*!<
 * DTW: Data Transfer Width
 * This bit selects the data width of the SD bus for a data transfer. The Host Driver shall set it to match the
 * data width of the card. Possible Data transfer Width is 1-bit, 4-bits or 8-bits.
 */
#define IMX_USDHC_PROT_CTRL_DTW_OFFSET                              (1U)
#define IMX_USDHC_PROT_CTRL_DTW_MASK                                (0x6U)
#define IMX_USDHC_PROT_CTRL_DTW_BIT(x)                              mrt_bit_mask((x), IMX_USDHC_PROT_CTRL_DTW_MASK, IMX_USDHC_PROT_CTRL_DTW_OFFSET)

    NR_ImxUsdhc_ProtCtrl_DataWidth1Bits = IMX_USDHC_PROT_CTRL_DTW_BIT(0U),
    NR_ImxUsdhc_ProtCtrl_DataWidth4Bits = IMX_USDHC_PROT_CTRL_DTW_BIT(1U),
    NR_ImxUsdhc_ProtCtrl_DataWidth8Bits = IMX_USDHC_PROT_CTRL_DTW_BIT(2U),

    NR_ImxUsdhc_ProtCtrl_Data3AsCardDetect = mrt_bit(3U),           /*!< D3CD. 1: DATA3 as Card Detection Pin; 0 DATA3 does not monitor Card Insertion */

/*!< EMODE: Endian Mode */
#define IMX_USDHC_PROT_CTRL_EMODE_OFFSET                            (4U)
#define IMX_USDHC_PROT_CTRL_EMODE_MASK                              (0x30U)
#define IMX_USDHC_PROT_CTRL_EMODE_BIT(x)                            mrt_bit_mask((x), IMX_USDHC_PROT_CTRL_EMODE_MASK, IMX_USDHC_PROT_CTRL_EMODE_OFFSET)

    NR_ImxUsdhc_ProtCtrl_BigEndian = IMX_USDHC_PROT_CTRL_EMODE_BIT(0U),
    NR_ImxUsdhc_ProtCtrl_HalfWordEndian = IMX_USDHC_PROT_CTRL_EMODE_BIT(1U),
    NR_ImxUsdhc_ProtCtrl_LittleEndian = IMX_USDHC_PROT_CTRL_EMODE_BIT(2U),

    /*!<
     * CDTL: Card Detect Test Level
     * This is bit is enabled while the Card Detection Signal Selection is set to 1 and it indicates card insertion.
     *  1, Card Detect Test Level is 1, card inserted
     *  0, Card Detect Test Level is 0, no card inserted
     */
    NR_ImxUsdhc_ProtCtrl_CardDetectTestLevel = mrt_bit(6U),

    /*!<
     * CDSS: Card Detect Signal Selection
     * This bit selects the source for the card detection.
     *  1, Card Detection Test Level is selected (for test purpose).
     *  0, Card Detection Level is selected (for normal purpose).
     */
    NR_ImxUsdhc_ProtCtrl_CardDetectLevelSelect = mrt_bit(7U),

/*!< DMA Select */
#define IMX_USDHC_PROT_CTRL_DMASEL_OFFSET                           (8U)
#define IMX_USDHC_PROT_CTRL_DMASEL_MASK                             (0x300U)
#define IMX_USDHC_PROT_CTRL_DMASEL_BIT(x)                           mrt_bit_mask((x), IMX_USDHC_PROT_CTRL_DMASEL_MASK, IMX_USDHC_PROT_CTRL_DMASEL_OFFSET)

    NR_ImxUsdhc_ProtCtrl_SDmaSelect = IMX_USDHC_PROT_CTRL_DMASEL_BIT(0U),
    NR_ImxUsdhc_ProtCtrl_ADma1Select = IMX_USDHC_PROT_CTRL_DMASEL_BIT(1U),
    NR_ImxUsdhc_ProtCtrl_ADma2Select = IMX_USDHC_PROT_CTRL_DMASEL_BIT(2U),

    NR_ImxUsdhc_ProtCtrl_StopBlockkGapRequest = mrt_bit(16U),       /*!< SABGREQ. Stop At Block Gap Request. */
    NR_ImxUsdhc_ProtCtrl_ContinueRequest = mrt_bit(17U),            /*!< CREQ. Continue Request */
    NR_ImxUsdhc_ProtCtrl_ReadWaitControl = mrt_bit(18U),            /*!< RWCTL. Read Wait Control */
    NR_ImxUsdhc_ProtCtrl_InterruptBlockGap = mrt_bit(19U),          /*!< IABG. Interrupt At Block Gap */

    /*!<
     * RD_DONE_NO_8CLK: Read done no 8 clock
     * According to the SD/MMC spec, for read data transaction, 8 clocks are needed after the end bit of the
     * last data block. So, by default(RD_DONE_NO_8CLK=0), 8 clocks will be active after the end bit of the
     * last read data transaction.
     * However, this 8 clocks should not be active if user wants to use stop at block gap(include the auto stop
     * at block gap in boot mode) feature for read and the RWCTL bit(bit18) is not enabled. In this case,
     * software should set RD_DONE_NO_8CLK to avoid this 8 clocks. Otherwise, the device may send extra
     * data to uSDHC while uSDHC ignores these data.
     * In a summary, this bit should be set only if the use case needs to use stop at block gap feature while the
     * device can't support the read wait feature
     */
    NR_ImxUsdhc_ProtCtrl_ReadDoneNo8Clock = mrt_bit(20U),

    NR_ImxUsdhc_ProtCtrl_WakeEventOnCardInt = mrt_bit(24U),         /*!< WECINT. Wakeup Event Enable On Card Interrupt */
    NR_ImxUsdhc_ProtCtrl_WakeEventOnCardInsert = mrt_bit(25U),      /*!< WECINS. Wakeup Event Enable On SD Card Insertion */
    NR_ImxUsdhc_ProtCtrl_WakeEventOnCardRemoval = mrt_bit(26U),     /*!< WECRM. Wakeup Event Enable On SD Card Removal */
};

/*!< for Register: PRES_STATE */
enum __ERT_IMX_USDHC_PRES_STATE_BIT
{
    NR_ImxUsdhc_PresState_CmdInhibitCmdLine = mrt_bit(0U),          /*!< CIHB. 1: Cannot issue command; 0: Can issue command using only CMD line */
    NR_ImxUsdhc_PresState_CmdInhibitDataLine = mrt_bit(1U),         /*!< CDIHB. 1: Cannot issue command which uses the DATA line; 0: Can issue command which uses the DATA line */
    NR_ImxUsdhc_PresState_DataLineActive = mrt_bit(2U),             /*!< DLA. Data Line Active */
    NR_ImxUsdhc_PresState_SDClockStable = mrt_bit(3U),              /*!< SDSTB. SD Clock Stable. */
    NR_ImxUsdhc_PresState_WriteTransferActive = mrt_bit(8U),        /*!< WTA. Write Transfer Active */

    NR_ImxUsdhc_PresState_ReadTransferActive = mrt_bit(9U),         /*!< RTA. Read Transfer Active */
    NR_ImxUsdhc_PresState_BufferWriteEnable = mrt_bit(10U),         /*!< BWEN. Buffer Write Enable */
    NR_ImxUsdhc_PresState_BufferReadEnable = mrt_bit(11U),          /*!< BREN. Buffer Read Enable */
    NR_ImxUsdhc_PresState_CardInserted = mrt_bit(16U),              /*!< CINST. Card Inserted */
    NR_ImxUsdhc_PresState_CommandLineLevel = mrt_bit(23U),          /*!< CLSL. CMD Line Signal Level */

    NR_ImxUsdhc_PresState_Data0LineLevel = mrt_bit(24U),            /*!< DLSL. Data 0 line signal level */
    NR_ImxUsdhc_PresState_Data1LineLevel = mrt_bit(25U),            /*!< DLSL. Data 1 line signal level */
    NR_ImxUsdhc_PresState_Data2LineLevel = mrt_bit(26U),            /*!< DLSL. Data 2 line signal level */
    NR_ImxUsdhc_PresState_Data3LineLevel = mrt_bit(27U),            /*!< DLSL. Data 3 line signal level */
    NR_ImxUsdhc_PresState_Data4LineLevel = mrt_bit(28U),            /*!< DLSL. Data 4 line signal level */
    NR_ImxUsdhc_PresState_Data5LineLevel = mrt_bit(29U),            /*!< DLSL. Data 5 line signal level */
    NR_ImxUsdhc_PresState_Data6LineLevel = mrt_bit(30U),            /*!< DLSL. Data 6 line signal level */
    NR_ImxUsdhc_PresState_Data7LineLevel = mrt_bit(31U),            /*!< DLSL. Data 7 line signal level */
};

/*!< for Register: CMD_XFR_TYP */
enum __ERT_IMX_USDHC_CMD_XFR_TYP_BIT
{
/*!< RSPTYP: Response Type Select */
#define IMX_USDHC_CMD_XFR_TYP_RSPTYP_OFFSET                         (16U)
#define IMX_USDHC_CMD_XFR_TYP_RSPTYP_MASK                           (0x30000U)
#define IMX_USDHC_CMD_XFR_TYP_RSPTYP_BIT(x)                         mrt_bit_mask((x), IMX_USDHC_CMD_XFR_TYP_RSPTYP_MASK, IMX_USDHC_CMD_XFR_TYP_RSPTYP_OFFSET)

    NR_ImxUsdhc_CmdXfrTyp_NoResponse = IMX_USDHC_CMD_XFR_TYP_RSPTYP_BIT(0U),
    NR_ImxUsdhc_CmdXfrTyp_Response136 = IMX_USDHC_CMD_XFR_TYP_RSPTYP_BIT(1U),
    NR_ImxUsdhc_CmdXfrTyp_Response48 = IMX_USDHC_CMD_XFR_TYP_RSPTYP_BIT(2U),
    NR_ImxUsdhc_CmdXfrTyp_ResponseBusy48 = IMX_USDHC_CMD_XFR_TYP_RSPTYP_BIT(3U),

    NR_ImxUsdhc_CmdXfrTyp_CmdCrcCheckEnable = mrt_bit(19U),
    NR_ImxUsdhc_CmdXfrTyp_CmdIndexCheckEnable = mrt_bit(20U),
    NR_ImxUsdhc_CmdXfrTyp_DataPresentSelect = mrt_bit(21U),

#define IMX_USDHC_CMD_XFR_TYP_CMDTYP_OFFSET                         (22U)
#define IMX_USDHC_CMD_XFR_TYP_CMDTYP_MASK                           (0xc00000U)
#define IMX_USDHC_CMD_XFR_TYP_CMDTYP_BIT(x)                         mrt_bit_mask((x), IMX_USDHC_CMD_XFR_TYP_CMDTYP_MASK, IMX_USDHC_CMD_XFR_TYP_CMDTYP_OFFSET)

    /*!<
     * CMDTYP: bit[23:22], Command Type
     *  11, Abort CMD12, CMD52 for writing I/O Abort in CCCR
     *  10, Resume CMD52 for writing Function Select in CCCR
     *  01, Suspend CMD52 for writing Bus Suspend in CCCR
     *  00, Normal Other commands
     */
    NR_ImxUsdhc_CmdXfrTyp_SuspendCommand = IMX_USDHC_CMD_XFR_TYP_CMDTYP_BIT(1U),
    NR_ImxUsdhc_CmdXfrTyp_ResumeCommand = IMX_USDHC_CMD_XFR_TYP_CMDTYP_BIT(2U),
    NR_ImxUsdhc_CmdXfrTyp_AbortCommand = IMX_USDHC_CMD_XFR_TYP_CMDTYP_BIT(3U),

#define IMX_USDHC_CMD_XFR_TYP_CMDINX_OFFSET                         (24U)
#define IMX_USDHC_CMD_XFR_TYP_CMDINX_MASK                           (0x3f000000U)
#define IMX_USDHC_CMD_XFR_TYP_CMDINX_BIT(x)                         mrt_bit_mask((x), IMX_USDHC_CMD_XFR_TYP_CMDINX_MASK, IMX_USDHC_CMD_XFR_TYP_CMDINX_OFFSET)
#define IMX_USDHC_CMD_XFR_TYP_CMDINX_U32(x)                         IMX_USDHC_CMD_XFR_TYP_CMDINX_BIT(x)
};

/*!< for Register: HOST_CTRL_CAP */
enum __ERT_IMX_USDHC_HOST_CTRL_CAP_BIT
{
    NR_ImxUsdhc_HostCtrlCap_Sdr50Support = mrt_bit(0U),             /*!< This bit indicates support of SDR50 mode */
    NR_ImxUsdhc_HostCtrlCap_Sdr104Support = mrt_bit(1U),            /*!< This bit indicates support of SDR104 mode */
    NR_ImxUsdhc_HostCtrlCap_Ddr50Support = mrt_bit(2U),             /*!< This bit indicates support of DDR50 mode */

#define IMX_USDHC_HOST_CTRL_CAP_TIMCNT_RETUNING_OFFSET              (8U)
#define IMX_USDHC_HOST_CTRL_CAP_TIMCNT_RETUNING_MASK                (0xf00U)
#define IMX_USDHC_HOST_CTRL_CAP_TIMCNT_RETUNING_BIT(x)              mrt_bit_mask((x), IMX_USDHC_HOST_CTRL_CAP_TIMCNT_RETUNING_MASK, IMX_USDHC_HOST_CTRL_CAP_TIMCNT_RETUNING_OFFSET)

    NR_ImxUsdhc_HostCtrlCap_UseTuningSdr50 = mrt_bit(13U),          /*!< This bit is set to 1. Host controller requires tuning to operate SDR50 */

#define IMX_USDHC_HOST_CTRL_CAP_RETUNING_MODE_OFFSET                (14U)
#define IMX_USDHC_HOST_CTRL_CAP_RETUNING_MODE_MASK                  (0xc000U)
#define IMX_USDHC_HOST_CTRL_CAP_RETUNING_MODE_BIT(x)                mrt_bit_mask((x), IMX_USDHC_HOST_CTRL_CAP_RETUNING_MODE_MASK, IMX_USDHC_HOST_CTRL_CAP_RETUNING_MODE_OFFSET)

    NR_ImxUsdhc_HostCtrlCap_RetuningMode1 = IMX_USDHC_HOST_CTRL_CAP_RETUNING_MODE_BIT(0U),
    NR_ImxUsdhc_HostCtrlCap_RetuningMode2 = IMX_USDHC_HOST_CTRL_CAP_RETUNING_MODE_BIT(1U),
    NR_ImxUsdhc_HostCtrlCap_RetuningMode3 = IMX_USDHC_HOST_CTRL_CAP_RETUNING_MODE_BIT(3U),

#define IMX_USDHC_HOST_CTRL_CAP_MBL_OFFSET                          (16U)
#define IMX_USDHC_HOST_CTRL_CAP_MBL_MASK                            (0x70000U)
#define IMX_USDHC_HOST_CTRL_CAP_MBL_BIT(x)                          mrt_bit_mask((x), IMX_USDHC_HOST_CTRL_CAP_MBL_MASK, IMX_USDHC_HOST_CTRL_CAP_MBL_OFFSET)

    NR_ImxUsdhc_HostCtrlCap_MaxBlockLenth512Byte = IMX_USDHC_HOST_CTRL_CAP_MBL_BIT(0U),
    NR_ImxUsdhc_HostCtrlCap_MaxBlockLenth1024Byte = IMX_USDHC_HOST_CTRL_CAP_MBL_BIT(1U),
    NR_ImxUsdhc_HostCtrlCap_MaxBlockLenth2048Byte = IMX_USDHC_HOST_CTRL_CAP_MBL_BIT(2U),
    NR_ImxUsdhc_HostCtrlCap_MaxBlockLenth4096Byte = IMX_USDHC_HOST_CTRL_CAP_MBL_BIT(3U),

    NR_ImxUsdhc_HostCtrlCap_AdmaSupport = mrt_bit(20U),             /*!< This bit indicates whether the uSDHC supports the ADMA feature */
    NR_ImxUsdhc_HostCtrlCap_HighSpeedSupport = mrt_bit(21U),        /*!< High Speed Supported (Host System maybe supply a SD Clock frequency from 25 MHz to 50 MHz) */
    NR_ImxUsdhc_HostCtrlCap_DmaSupport = mrt_bit(22U),              /*!< DMA Support */
    NR_ImxUsdhc_HostCtrlCap_SuspendResumeSupport = mrt_bit(23U),    /*!< Suspend / Resume Support */
    NR_ImxUsdhc_HostCtrlCap_Voltage33VSupport = mrt_bit(24U),       /*!< Voltage Support 3.3V */
    NR_ImxUsdhc_HostCtrlCap_Voltage30VSupport = mrt_bit(25U),       /*!< Voltage Support 3.0V */
    NR_ImxUsdhc_HostCtrlCap_Voltage18VSupport = mrt_bit(26U),       /*!< Voltage Support 1.8V */
};

/*!< for Register: WTMK_LVL */
#define IMX_USDHC_WTMK_LVL_RD_WML_OFFSET                            (0U)
#define IMX_USDHC_WTMK_LVL_RD_WML_MASK                              (0xffU)
#define IMX_USDHC_WTMK_LVL_RD_WML_BIT(x)                            mrt_bit_mask((x), IMX_USDHC_WTMK_LVL_RD_WML_MASK, IMX_USDHC_WTMK_LVL_RD_WML_OFFSET)
#define IMX_USDHC_WTMK_LVL_RD_WML_U32(x)                            IMX_USDHC_WTMK_LVL_RD_WML_BIT(x)

#define IMX_USDHC_WTMK_LVL_RD_BRST_LEN_OFFSET                       (8U)
#define IMX_USDHC_WTMK_LVL_RD_BRST_LEN_MASK                         (0x1f00U)
#define IMX_USDHC_WTMK_LVL_RD_BRST_LEN_BIT(x)                       mrt_bit_mask((x), IMX_USDHC_WTMK_LVL_RD_BRST_LEN_MASK, IMX_USDHC_WTMK_LVL_RD_BRST_LEN_OFFSET)
#define IMX_USDHC_WTMK_LVL_RD_BRST_LEN_U32(x)                       IMX_USDHC_WTMK_LVL_RD_BRST_LEN_BIT(x)

#define IMX_USDHC_WTMK_LVL_WR_WML_OFFSET                            (16U)
#define IMX_USDHC_WTMK_LVL_WR_WML_MASK                              (0xff0000U)
#define IMX_USDHC_WTMK_LVL_WR_WML_BIT(x)                            mrt_bit_mask((x), IMX_USDHC_WTMK_LVL_WR_WML_MASK, IMX_USDHC_WTMK_LVL_WR_WML_OFFSET)
#define IMX_USDHC_WTMK_LVL_WR_WML_U32(x)                            IMX_USDHC_WTMK_LVL_WR_WML_BIT(x)

#define IMX_USDHC_WTMK_LVL_WR_BRST_LEN_OFFSET                       (24U)
#define IMX_USDHC_WTMK_LVL_WR_BRST_LEN_MASK                         (0x1f000000U)
#define IMX_USDHC_WTMK_LVL_WR_BRST_LEN_BIT(x)                       mrt_bit_mask((x), IMX_USDHC_WTMK_LVL_WR_BRST_LEN_MASK, IMX_USDHC_WTMK_LVL_WR_BRST_LEN_OFFSET)
#define IMX_USDHC_WTMK_LVL_WR_BRST_LEN_U32(x)                       IMX_USDHC_WTMK_LVL_WR_BRST_LEN_BIT(x)

/*!< for Register: BLK_ATT */
/*!<
 * BLKCNT: bit[31:16], Blocks Count For Current Transfer
 *          0xffff: 65535 blocks
 *          0x0002: 2 blocks
 *          0x0001: 1 blocks
 *          0x0000: stop count
 */
#define IMX_USDHC_BLK_ATT_BLKCNT_OFFSET                             (16U)
#define IMX_USDHC_BLK_ATT_BLKCNT_MASK                               (0xffff0000U)
#define IMX_USDHC_BLK_ATT_BLKCNT_BIT(x)                             mrt_bit_mask((x), IMX_USDHC_BLK_ATT_BLKCNT_MASK, IMX_USDHC_BLK_ATT_BLKCNT_OFFSET)
#define IMX_USDHC_BLK_ATT_BLKCNT_U32(x)                             IMX_USDHC_BLK_ATT_BLKCNT_BIT(x)

/*!<
 * BLKSIZE: bit[12:0], Transfer Block Size
 *          0x1000(4096 Bytes), 0x800(2048 Bytes), 0x200(512 Bytes), 0x1ff(511 Bytes), 0x004(4 Bytes)
 *          and 0x003, 0x002, 0x001, 0x000(no data transfer)
 */
#define IMX_USDHC_BLK_ATT_BLKSIZE_OFFSET                            (0U)
#define IMX_USDHC_BLK_ATT_BLKSIZE_MASK                              (0x00000fffU)
#define IMX_USDHC_BLK_ATT_BLKSIZE_BIT(x)                            mrt_bit_mask((x), IMX_USDHC_BLK_ATT_BLKSIZE_MASK, IMX_USDHC_BLK_ATT_BLKSIZE_OFFSET)
#define IMX_USDHC_BLK_ATT_BLKSIZE_U32(x)                            IMX_USDHC_BLK_ATT_BLKSIZE_BIT(x)

/*!< for Register: VEND_SPEC */
enum  __ERT_IMX_USDHC_VEND_SPEC_BIT
{
    NR_ImxUsdhc_VendSpec_ExtDmaRequestEnable = mrt_bit(0U),         /*!< External DMA Request Enable */
    NR_ImxUsdhc_VendSpec_VoltageSelect = mrt_bit(1U),               /*!< Voltage Selection */
    NR_ImxUsdhc_VendSpec_ConflictCheckEnable = mrt_bit(2U),         /*!< Conflict check enable. */
    NR_ImxUsdhc_VendSpec_CheckBusyAfterAC12WR = mrt_bit(3U),        /*!< Check busy enable after auto CMD12 for write data packet */
    NR_ImxUsdhc_VendSpec_Data3DetectPolarity = mrt_bit(4U),         /*!< Polarity of DATA3 pin when it is used as card detection. Only for debug */
    NR_ImxUsdhc_VendSpec_CD_B_Polarity = mrt_bit(5U),               /*!< Polarity of the CD_B pin. Only for debug */
    NR_ImxUsdhc_VendSpec_WriteProtectPolarity = mrt_bit(6U),        /*!< Polarity of the WP pin. Only for debug */
    NR_ImxUsdhc_VendSpec_ActiveClkInAbortSend = mrt_bit(7U),        /*!< Force CLK output active when sending Abort command. Only for debug */
    NR_ImxUsdhc_VendSpec_ActiveClk = mrt_bit(8U),                   /*!< Force CLK output active */
    NR_ImxUsdhc_VendSpec_IpgClkSoftEnable = mrt_bit(11U),           /*!< IPG_CLK Software Enable */
    NR_ImxUsdhc_VendSpec_AhbClkSoftEnable = mrt_bit(12U),           /*!< AHB Clock Software Enable */
    NR_ImxUsdhc_VendSpec_IpgPerClkSoftEnable = mrt_bit(13U),        /*!< IPG_PERCLK Software Enable */
    NR_ImxUsdhc_VendSpec_CardClkSoftEnable = mrt_bit(14U),          /*!< Card Clock Software Enable */
    NR_ImxUsdhc_VendSpec_CrcCheckDisable = mrt_bit(15U),            /*!< CRC Check Disable */

    NR_ImxUsdhc_VendSpec_CmdByteAccess = mrt_bit(31U),              /*!< Byte access */
};

/*!< ------------------------------------------------------------------------- */
/*!< QuadSPI - Register Layout Typedef */
typedef struct 
{
    kuint32_t MCR;                                                  /*!< Module Configuration Register, offset: 0x0 */
    kuint8_t  RESERVED_0[4];
    kuint32_t IPCR;                                                 /*!< IP Configuration Register, offset: 0x8 */
    kuint32_t FLSHCR;                                               /*!< Flash Configuration Register, offset: 0xC */
    kuint32_t BUF0CR;                                               /*!< Buffer0 Configuration Register, offset: 0x10 */
    kuint32_t BUF1CR;                                               /*!< Buffer1 Configuration Register, offset: 0x14 */
    kuint32_t BUF2CR;                                               /*!< Buffer2 Configuration Register, offset: 0x18 */
    kuint32_t BUF3CR;                                               /*!< Buffer3 Configuration Register, offset: 0x1C */
    kuint32_t BFGENCR;                                              /*!< Buffer Generic Configuration Register, offset: 0x20 */
    kuint8_t  RESERVED_1[12];
    kuint32_t BUF0IND;                                              /*!< Buffer0 Top Index Register, offset: 0x30 */
    kuint32_t BUF1IND;                                              /*!< Buffer1 Top Index Register, offset: 0x34 */
    kuint32_t BUF2IND;                                              /*!< Buffer2 Top Index Register, offset: 0x38 */
    kuint8_t  RESERVED_2[196];
    kuint32_t SFAR;                                                 /*!< Serial Flash Address Register, offset: 0x100 */
    kuint8_t  RESERVED_3[4];
    kuint32_t SMPR;                                                 /*!< Sampling Register, offset: 0x108 */
    kuint32_t RBSR;                                                 /*!< RX Buffer Status Register, offset: 0x10C */
    kuint32_t RBCT;                                                 /*!< RX Buffer Control Register, offset: 0x110 */
    kuint8_t  RESERVED_4[60];
    kuint32_t TBSR;                                                 /*!< TX Buffer Status Register, offset: 0x150 */
    kuint32_t TBDR;                                                 /*!< TX Buffer Data Register, offset: 0x154 */
    kuint8_t  RESERVED_5[4];
    kuint32_t SR;                                                   /*!< Status Register, offset: 0x15C */
    kuint32_t FR;                                                   /*!< Flag Register, offset: 0x160 */
    kuint32_t RSER;                                                 /*!< Interrupt and DMA Request Select and Enable Register, offset: 0x164 */
    kuint32_t SPNDST;                                               /*!< Sequence Suspend Status Register, offset: 0x168 */
    kuint32_t SPTRCLR;                                              /*!< Sequence Pointer Clear Register, offset: 0x16C */
    kuint8_t  RESERVED_6[16];
    kuint32_t SFA1AD;                                               /*!< Serial Flash A1 Top Address, offset: 0x180 */
    kuint32_t SFA2AD;                                               /*!< Serial Flash A2 Top Address, offset: 0x184 */
    kuint32_t SFB1AD;                                               /*!< Serial Flash B1Top Address, offset: 0x188 */
    kuint32_t SFB2AD;                                               /*!< Serial Flash B2Top Address, offset: 0x18C */
    kuint8_t  RESERVED_7[112];
    kuint32_t RBDR[32];                                             /*!< RX Buffer Data Register, array offset: 0x200, array step: 0x4 */
    kuint8_t  RESERVED_8[128];
    kuint32_t LUTKEY;                                               /*!< LUT Key Register, offset: 0x300 */
    kuint32_t LCKCR;                                                /*!< LUT Lock Configuration Register, offset: 0x304 */
    kuint8_t  RESERVED_9[8];
    kuint32_t LUT[64];                                              /*!< Look-up Table register, array offset: 0x310, array step: 0x4 */
} srt_imx_qspi_t;

/*!< Peripheral QuardSPI base address */
#define IMX6UL_QUARDSPI_ADDR_BASE                                   (0x21E0000u)
#define IMX6UL_QSPI_PROPERTY_ENTRY(x)                               (srt_imx_qspi_t *)IMX6UL_QUARDSPI##x##_ADDR_BASE


#endif /* __IMX6UL_PINS_H */
