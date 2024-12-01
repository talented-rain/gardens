/*
 * ZYNQ7 Function Defines
 *
 * File Name:   zynq7_periph.h
 * Author:      Yang Yujun (Copy from "Xilinx SDK: ps7_init.h")
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.06.19
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __ZYNQ7_PERIPH_H
#define __ZYNQ7_PERIPH_H

#ifdef __cplusplus
    TARGET_EXT "C" {
#endif

/*!< The includes */
#include <common/generic.h>
#include <common/io_stream.h>

#include "xparameters.h"
#include "xvtc_hw.h"
#include "xaxivdma_hw.h"

/*!< The defines */
typedef kuaddr_t UINTPTR;
typedef kint32_t LONG;

#define XIL_COMPONENT_IS_READY              0x11111111U     /**< In device drivers, This macro will be
                                                                assigend to "IsReady" member of driver
												                instance to indicate that driver
												                instance is initialized and ready to use. */
#define XIL_COMPONENT_IS_STARTED            0x22222222U     /**< In device drivers, This macro will be assigend to
                                                                "IsStarted" member of driver instance
												                to indicate that driver instance is
												                started and it can be enabled. */

#define xil_printf                          print_debug
#define ULONG64_HI_MASK	                    0xFFFFFFFF00000000U
#define ULONG64_LO_MASK	                    (~ULONG64_HI_MASK)

#define XSdPs_WriteReg8(addr, offset, data) \
                                            mrt_writeb(data, (addr) + (offset))
#define XSdPs_WriteReg16(addr, offset, data)    \
                                            mrt_writew(data, (addr) + (offset))
#define XSdPs_WriteReg(addr, offset, data)  mrt_writel(data, (addr) + (offset))
#define XSdPs_ReadReg8(addr, offset)        mrt_readb((addr) + (offset))
#define XSdPs_ReadReg16(addr, offset)       mrt_readw((addr) + (offset))
#define XSdPs_ReadReg(addr, offset)         mrt_readl((addr) + (offset))

#define OPCODE_EXIT                         0U
#define OPCODE_CLEAR                        1U
#define OPCODE_WRITE                        2U
#define OPCODE_MASKWRITE                    3U
#define OPCODE_MASKPOLL                     4U
#define OPCODE_MASKDELAY                    5U
#define NEW_PS7_ERR_CODE                    1

/*!< Encode number of arguments in last nibble */
#define EMIT_EXIT()                         ((OPCODE_EXIT      << 4) | 0)
#define EMIT_CLEAR(addr)                    ((OPCODE_CLEAR     << 4) | 1) , addr
#define EMIT_WRITE(addr, val)               ((OPCODE_WRITE     << 4) | 2) , addr, val
#define EMIT_MASKWRITE(addr, mask,val)      ((OPCODE_MASKWRITE << 4) | 3) , addr, mask, val
#define EMIT_MASKPOLL(addr, mask)           ((OPCODE_MASKPOLL  << 4) | 2) , addr, mask
#define EMIT_MASKDELAY(addr, mask)          ((OPCODE_MASKDELAY << 4) | 2) , addr, mask

/*!< Returns codes of PS7_Init */
#define PS7_INIT_SUCCESS                    (0)             /*!< 0 is success in good old C */
#define PS7_INIT_CORRUPT                    (1)             /*!< 1 the data is corrupted, and slcr reg are in corrupted state now */
#define PS7_INIT_TIMEOUT                    (2)             /*!< 2 when a poll operation timed out */
#define PS7_POLL_FAILED_DDR_INIT            (3)             /*!< 3 when a poll operation timed out for ddr init */
#define PS7_POLL_FAILED_DMA                 (4)             /*!< 4 when a poll operation timed out for dma done bit */
#define PS7_POLL_FAILED_PLL                 (5)             /*!< 5 when a poll operation timed out for pll sequence init */

/*!< Silicon Versions */
#define PCW_SILICON_VERSION_1               0
#define PCW_SILICON_VERSION_2               1
#define PCW_SILICON_VERSION_3               2

/*!< This flag to be used by FSBL to check whether ps7_post_config() proc exixts */
#define PS7_POST_CONFIG

/*!< L2 Cache */
#define XPS_L2CC_CACHE_SYNC_OFFSET		    0x0730U		    /*!< Cache Sync */
#define XPS_L2CC_DUMMY_CACHE_SYNC_OFFSET	0x0740U		    /*!< Dummy Register for Cache Sync */
#define XPS_L2CC_CACHE_INVLD_PA_OFFSET		0x0770U		    /*!< Cache Invalid by PA */
#define XPS_L2CC_CACHE_INVLD_WAY_OFFSET		0x077CU		    /*!< Cache Invalid by Way */
#define XPS_L2CC_CACHE_CLEAN_PA_OFFSET		0x07B0U		    /*!< Cache Clean by PA */
#define XPS_L2CC_CACHE_CLEAN_INDX_OFFSET	0x07B8U		    /*!< Cache Clean by Index */
#define XPS_L2CC_CACHE_CLEAN_WAY_OFFSET		0x07BCU		    /*!< Cache Clean by Way */
#define XPS_L2CC_CACHE_INV_CLN_PA_OFFSET	0x07F0U		    /*!< Cache Invalidate and Clean by PA */
#define XPS_L2CC_CACHE_INV_CLN_INDX_OFFSET	0x07F8U		    /*!< Cache Invalidate and Clean by Index */
#define XPS_L2CC_CACHE_INV_CLN_WAY_OFFSET	0x07FCU		    /*!< Cache Invalidate and Clean by Way */

#define XPS_L2CC_DEBUG_CTRL_OFFSET		    0x0F40U		    /*!< Debug Control Register */

#define XREG_CP15_CACHE_SIZE_SEL		    "2, %0, c0, c0, 0"
#define XREG_CP15_CLEAN_INVAL_DC_LINE_MVA_POC   \
                                            "0, %0, c7, c14, 1"
#define XREG_CP15_CLEAN_INVAL_DC_LINE_SW	"0, %0, c7, c14, 2"
#define XREG_CP15_INVAL_DC_LINE_MVA_POC		"0, %0, c7, c6, 1"
#define XREG_CP15_INVAL_DC_LINE_SW		    "0, %0, c7, c6, 2"

/*!< Freq of all peripherals */
#define APU_FREQ                            666666687       /*!< 667MHz */
#define DDR_FREQ                            533333374
#define DCI_FREQ                            10158730
#define QSPI_FREQ                           200000000
#define SMC_FREQ                            10000000
#define ENET0_FREQ                          125000000
#define ENET1_FREQ                          10000000
#define USB0_FREQ                           60000000
#define USB1_FREQ                           60000000
#define SDIO_FREQ                           100000000
#define UART_FREQ                           100000000
#define SPI_FREQ                            160000000
#define I2C_FREQ                            111111115
#define WDT_FREQ                            111111115
#define TTC_FREQ                            50000000
#define CAN_FREQ                            100000000
#define PCAP_FREQ                           200000000
#define TPIU_FREQ                           200000000
#define FPGA0_FREQ                          100000000
#define FPGA1_FREQ                          145454544
#define FPGA2_FREQ                          10000000
#define FPGA3_FREQ                          10000000

/*!< For delay calculation using global registers*/
#define SCU_GLOBAL_TIMER_COUNT_L32	        0xF8F00200
#define SCU_GLOBAL_TIMER_COUNT_U32	        0xF8F00204
#define SCU_GLOBAL_TIMER_CONTROL	        0xF8F00208
#define SCU_GLOBAL_TIMER_AUTO_INC	        0xF8F00218

/*!< ---------------------------------------------------------------------
                                GpioPs                                   
 ---------------------------------------------------------------------- */
#define XGPIOPS_BANK0			            0x00U           /*!< MIO/GPIO  Bank 0: pin0  ~ pin31 */
#define XGPIOPS_BANK1			            0x01U           /*!< MIO/GPIO  Bank 1: pin32 ~ pin53 */
#define XGPIOPS_BANK2			            0x02U           /*!< EMIO/GPIO Bank 2: pin54 ~ pin85 */
#define XGPIOPS_BANK3			            0x03U           /*!< EMIO/GPIO Bank 3: pin85 ~ pin117 */
#define XGPIOPS_MAX_BANKS		            0x04U           /*!< Max banks in a Zynq GPIO device */

#define XGPIOPS_BANK_MAX_PINS		        32U             /*!< Max pins in a GPIO bank */
#define XGPIOPS_BANK0_MAX_PIN				31U             /*!< MIO  Bank 0: pin0  ~ pin31 */
#define XGPIOPS_BANK1_MAX_PIN				53U             /*!< MIO  Bank 1: pin31 ~ pin53 */
#define XGPIOPS_BANK2_MAX_PIN				85U             /*!< EMIO Bank 0: pin0  ~ pin31 */
#define XGPIOPS_BANK3_MAX_PIN				117U            /*!< EMIO Bank 1: pin32 ~ pin63 */

/*!< Max pins in the Zynq GPIO device: 118 */
#define XGPIOPS_DEVICE_MAX_PIN_NUM	        118U

/*!< Register offsets for the GPIO. Each register is 32 bits */
#define XGPIOPS_DATA_LSW_OFFSET             0x00000000U     /*!< Mask and Data Register LSW, WO */
#define XGPIOPS_DATA_MSW_OFFSET             0x00000004U     /*!< Mask and Data Register MSW, WO */
#define XGPIOPS_DATA_OFFSET	                0x00000040U     /*!< Data Register, RW */
#define XGPIOPS_DATA_RO_OFFSET	            0x00000060U     /*!< Data Register - Input, RO */
#define XGPIOPS_DIRM_OFFSET	                0x00000204U     /*!< Direction Mode Register, RW */
#define XGPIOPS_OUTEN_OFFSET	            0x00000208U     /*!< Output Enable Register, RW */
#define XGPIOPS_INTMASK_OFFSET	            0x0000020CU     /*!< Interrupt Mask Register, RO */
#define XGPIOPS_INTEN_OFFSET	            0x00000210U     /*!< Interrupt Enable Register, WO */
#define XGPIOPS_INTDIS_OFFSET	            0x00000214U     /*!< Interrupt Disable Register, WO*/
#define XGPIOPS_INTSTS_OFFSET	            0x00000218U     /*!< Interrupt Status Register, RO */
#define XGPIOPS_INTTYPE_OFFSET	            0x0000021CU     /*!< Interrupt Type Register, RW */
#define XGPIOPS_INTPOL_OFFSET	            0x00000220U     /*!< Interrupt Polarity Register, RW */
#define XGPIOPS_INTANY_OFFSET	            0x00000224U     /*!< Interrupt On Any Register, RW */

/*!< Register offsets for each Bank */
#define XGPIOPS_DATA_MASK_OFFSET            0x00000008U     /*!< Data/Mask Registers offset */
#define XGPIOPS_DATA_BANK_OFFSET            0x00000004U     /*!< Data Registers offset */
#define XGPIOPS_REG_MASK_OFFSET             0x00000040U     /*!< Registers offset */

/*!< pin operation */
#define XGPIOPS_PIN(pin)                    (pin)

#define XGPIOPS_BANK1_MAX_PINS              (XGPIOPS_BANK1_MAX_PIN - XGPIOPS_BANK0_MAX_PIN)
#define XGPIOPS_BANK_PIN(bank, pin) \
    (((bank) > XGPIOPS_BANK1) ? ((pin) + ((bank) - 1) * XGPIOPS_BANK_MAX_PINS + XGPIOPS_BANK1_MAX_PINS) : ((pin) + (bank) * XGPIOPS_BANK_MAX_PINS))

#define XGPIOPS_PIN_LEVEL_HIGH              1
#define XGPIOPS_PIN_LEVEL_LOW               0

#define XGPIOPS_PIN_DIR_OUTPUT              1
#define XGPIOPS_PIN_DIR_INPUT               0

/*!< This typedef contains configuration information for a device. */
typedef struct 
{
    kuint16_t DeviceId;		                                /*!< Unique ID of device */
    kuint32_t BaseAddr;		                                /*!< Register base address */

} XGpioPs_Config;

/*!<
 * The XGpioPs driver instance data. The user is required to allocate a
 * variable of this type for the GPIO device in the system. A pointer
 * to a variable of this type is then passed to the driver API functions.
 */
typedef struct 
{
    XGpioPs_Config sgrt_cfg;	                            /*!< Device configuration */
    kbool_t IsReady;			                            /*!< Device is initialized and ready */

    kuint32_t MaxPinNum;			                        /*!< Max pins in the GPIO device */
    kuint8_t MaxBanks;			                            /*!< Max banks in a GPIO device */

} XGpioPs;

/*!< ---------------------------------------------------------------------
                                ScuTimer                                   
 ---------------------------------------------------------------------- */
/*!< @name Register Map
 * Offsets of registers from the start of the device
 * @{
 */

#define XSCUTIMER_LOAD_OFFSET               0x00U           /*!< Timer Load Register */
#define XSCUTIMER_COUNTER_OFFSET            0x04U           /*!< Timer Counter Register */
#define XSCUTIMER_CONTROL_OFFSET            0x08U           /*!< Timer Control Register */
#define XSCUTIMER_ISR_OFFSET                0x0CU           /*!< Timer Interrupt Status Register */

/*!< @name Timer Control register
 * This register bits control the prescaler, Intr enable,
 * auto-reload and timer enable.
 * @{
 */
#define XSCUTIMER_CONTROL_PRESCALER_MASK    0x0000FF00U     /*!< Prescaler */
#define XSCUTIMER_CONTROL_PRESCALER_SHIFT   8U  
#define XSCUTIMER_CONTROL_IRQ_ENABLE_MASK   0x00000004U     /*!< Intr enable */
#define XSCUTIMER_CONTROL_AUTO_RELOAD_MASK  0x00000002U     /*!< Auto-reload */
#define XSCUTIMER_CONTROL_ENABLE_MASK       0x00000001U     /*!< Timer enable */

/*!< @name Interrupt Status register
 * This register indicates the Timer counter register has reached zero.
 * @{
 */

#define XSCUTIMER_ISR_EVENT_FLAG_MASK       0x00000001U     /**< Event flag */

/*!<
 * This typedef contains configuration information for the device.
 */
typedef struct 
{
	kuint16_t DeviceId;	                                    /*!< Unique ID of device */
	kuint32_t BaseAddr;	                                    /*!< Base address of the device */

} XScuTimer_Config;

/*!<
 * The XScuTimer driver instance data. The user is required to allocate a
 * variable of this type for every timer device in the system.
 * A pointer to a variable of this type is then passed to the driver API
 * functions.
 */
typedef struct 
{
	XScuTimer_Config Config;                                /*!< Hardware Configuration */
	kbool_t IsReady;		                                /*!< Device is initialized and ready */
	kbool_t IsStarted;		                                /*!< Device timer is running */

} XScuTimer;

/*!< ---------------------------------------------------------------------
                                UartPs                                   
 ---------------------------------------------------------------------- */
#define XUARTPS_CR_OFFSET		            0x0000U         /*!< Control Register [8:0] */
#define XUARTPS_MR_OFFSET		            0x0004U         /*!< Mode Register [9:0] */
#define XUARTPS_IER_OFFSET		            0x0008U         /*!< Interrupt Enable [12:0] */
#define XUARTPS_IDR_OFFSET		            0x000CU         /*!< Interrupt Disable [12:0] */
#define XUARTPS_IMR_OFFSET		            0x0010U         /*!< Interrupt Mask [12:0] */
#define XUARTPS_ISR_OFFSET		            0x0014U         /*!< Interrupt Status [12:0]*/
#define XUARTPS_BAUDGEN_OFFSET	            0x0018U         /*!< Baud Rate Generator [15:0] */
#define XUARTPS_RXTOUT_OFFSET	            0x001CU         /*!< RX Timeout [7:0] */
#define XUARTPS_RXWM_OFFSET		            0x0020U         /*!< RX FIFO Trigger Level [5:0] */
#define XUARTPS_MODEMCR_OFFSET	            0x0024U         /*!< Modem Control [5:0] */
#define XUARTPS_MODEMSR_OFFSET	            0x0028U         /*!< Modem Status [8:0] */
#define XUARTPS_SR_OFFSET		            0x002CU         /*!< Channel Status [14:0] */
#define XUARTPS_FIFO_OFFSET		            0x0030U         /*!< FIFO [7:0] */
#define XUARTPS_BAUDDIV_OFFSET	            0x0034U         /*!< Baud Rate Divider [7:0] */
#define XUARTPS_FLOWDEL_OFFSET	            0x0038U         /*!< Flow Delay [5:0] */
#define XUARTPS_TXWM_OFFSET		            0x0044U         /*!< TX FIFO Trigger Level [5:0] */
#define XUARTPS_RXBS_OFFSET		            0x0048U         /*!< RX FIFO Byte Status [11:0] */

/*! @name Control Register */
#define XUARTPS_CR_STOPBRK	                0x00000100U     /*!< Stop transmission of break */
#define XUARTPS_CR_STARTBRK	                0x00000080U     /*!< Set break */
#define XUARTPS_CR_TORST	                0x00000040U     /*!< RX timeout counter restart */
#define XUARTPS_CR_TX_DIS	                0x00000020U     /*!< TX disabled. */
#define XUARTPS_CR_TX_EN	                0x00000010U     /*!< TX enabled */
#define XUARTPS_CR_RX_DIS	                0x00000008U     /*!< RX disabled. */
#define XUARTPS_CR_RX_EN	                0x00000004U     /*!< RX enabled */
#define XUARTPS_CR_EN_DIS_MASK	            0x0000003CU     /*!< Enable/disable Mask */
#define XUARTPS_CR_TXRST	                0x00000002U     /*!< TX logic reset */
#define XUARTPS_CR_RXRST	                0x00000001U     /*!< RX logic reset */

/*! @name Mode Register */
#define XUARTPS_MR_CCLK				        0x00000400U     /*!< Input clock selection */
#define XUARTPS_MR_CHMODE_R_LOOP	        0x00000300U     /*!< Remote loopback mode */
#define XUARTPS_MR_CHMODE_L_LOOP	        0x00000200U     /*!< Local loopback mode */
#define XUARTPS_MR_CHMODE_ECHO		        0x00000100U     /*!< Auto echo mode */
#define XUARTPS_MR_CHMODE_NORM		        0x00000000U     /*!< Normal mode */
#define XUARTPS_MR_CHMODE_SHIFT				8U              /*!< Mode shift */
#define XUARTPS_MR_CHMODE_MASK		        0x00000300U     /*!< Mode mask */
#define XUARTPS_MR_STOPMODE_2_BIT	        0x00000080U     /*!< 2 stop bits */
#define XUARTPS_MR_STOPMODE_1_5_BIT	        0x00000040U     /*!< 1.5 stop bits */
#define XUARTPS_MR_STOPMODE_1_BIT	        0x00000000U     /*!< 1 stop bit */
#define XUARTPS_MR_STOPMODE_SHIFT			6U              /*!< Stop bits shift */
#define XUARTPS_MR_STOPMODE_MASK	        0x000000A0U     /*!< Stop bits mask */
#define XUARTPS_MR_PARITY_NONE		        0x00000020U     /*!< No parity mode */
#define XUARTPS_MR_PARITY_MARK		        0x00000018U     /*!< Mark parity mode */
#define XUARTPS_MR_PARITY_SPACE		        0x00000010U     /*!< Space parity mode */
#define XUARTPS_MR_PARITY_ODD		        0x00000008U     /*!< Odd parity mode */
#define XUARTPS_MR_PARITY_EVEN		        0x00000000U     /*!< Even parity mode */
#define XUARTPS_MR_PARITY_SHIFT				3U              /*!< Parity setting shift */
#define XUARTPS_MR_PARITY_MASK		        0x00000038U     /*!< Parity mask */
#define XUARTPS_MR_CHARLEN_6_BIT	        0x00000006U     /*!< 6 bits data */
#define XUARTPS_MR_CHARLEN_7_BIT	        0x00000004U     /*!< 7 bits data */
#define XUARTPS_MR_CHARLEN_8_BIT	        0x00000000U     /*!< 8 bits data */
#define XUARTPS_MR_CHARLEN_SHIFT			1U              /*!< Data Length shift */
#define XUARTPS_MR_CHARLEN_MASK		        0x00000006U     /*!< Data length mask */
#define XUARTPS_MR_CLKSEL			        0x00000001U     /*!< Input clock selection */

/*! @name Interrupt Registers */
#define XUARTPS_IXR_RBRK	                0x00002000U     /*!< Rx FIFO break detect interrupt */
#define XUARTPS_IXR_TOVR	                0x00001000U     /*!< Tx FIFO Overflow interrupt */
#define XUARTPS_IXR_TNFUL	                0x00000800U     /*!< Tx FIFO Nearly Full interrupt */
#define XUARTPS_IXR_TTRIG	                0x00000400U     /*!< Tx Trig interrupt */
#define XUARTPS_IXR_DMS		                0x00000200U     /*!< Modem status change interrupt */
#define XUARTPS_IXR_TOUT	                0x00000100U     /*!< Timeout error interrupt */
#define XUARTPS_IXR_PARITY 	                0x00000080U     /*!< Parity error interrupt */
#define XUARTPS_IXR_FRAMING	                0x00000040U     /*!< Framing error interrupt */
#define XUARTPS_IXR_OVER	                0x00000020U     /*!< Overrun error interrupt */
#define XUARTPS_IXR_TXFULL 	                0x00000010U     /*!< TX FIFO full interrupt. */
#define XUARTPS_IXR_TXEMPTY	                0x00000008U     /*!< TX FIFO empty interrupt. */
#define XUARTPS_IXR_RXFULL 	                0x00000004U     /*!< RX FIFO full interrupt. */
#define XUARTPS_IXR_RXEMPTY	                0x00000002U     /*!< RX FIFO empty interrupt. */
#define XUARTPS_IXR_RXOVR  	                0x00000001U     /*!< RX FIFO trigger interrupt. */
#define XUARTPS_IXR_MASK	                0x00003FFFU     /*!< Valid bit mask */

/*! @name Channel Status Register */
#define XUARTPS_SR_TNFUL	                0x00004000U     /*!< TX FIFO Nearly Full Status */
#define XUARTPS_SR_TTRIG	                0x00002000U     /*!< TX FIFO Trigger Status */
#define XUARTPS_SR_FLOWDEL	                0x00001000U     /*!< RX FIFO fill over flow delay */
#define XUARTPS_SR_TACTIVE	                0x00000800U     /*!< TX active */
#define XUARTPS_SR_RACTIVE	                0x00000400U     /*!< RX active */
#define XUARTPS_SR_TXFULL	                0x00000010U     /*!< TX FIFO full */
#define XUARTPS_SR_TXEMPTY	                0x00000008U     /*!< TX FIFO empty */
#define XUARTPS_SR_RXFULL	                0x00000004U     /*!< RX FIFO full */
#define XUARTPS_SR_RXEMPTY	                0x00000002U     /*!< RX FIFO empty */
#define XUARTPS_SR_RXOVR	                0x00000001U     /*!< RX FIFO fill over trigger */

/*!< 
 * Baud Rate Generator Register (BRGR, value is 1 ~ 65535)
 * Bit Sample Rate = CCLK / BRGR, where the CCLK is selected by the MR_CCLK bit in the MR register.
 */
#define XUARTPS_BAUDGEN_DISABLE		        0x00000000U     /*!< Disable clock */
#define XUARTPS_BAUDGEN_MASK		        0x0000FFFFU     /*!< Valid bits mask */
#define XUARTPS_BAUDGEN_RESET_VAL	        0x0000028BU     /*!< Reset value */

/*!<
 * Baud Divisor Rate register (BDIV)
 * Valid values are 0x04 to 0xFF. Writing a value less than 4 will be ignored.
 * Baud rate = CCLK / ((BAUDDIV + 1) x BRGR), where the CCLK is selected by the MR_CCLK bit in the MR register.
 */
#define XUARTPS_BAUDDIV_MASK                0x000000FFU	    /*!< 8 bit baud divider mask */
#define XUARTPS_BAUDDIV_RESET_VAL           0x0000000FU	    /*!< Reset value */

#define XUARTPS_MAX_RATE	                921600U
#define XUARTPS_MIN_RATE	                110U
#define XUARTPS_DFT_BAUDRATE                115200U         /*!< Default baud rate */

/*!
 * These constants specify the options that may be set or retrieved
 * with the driver, each is a unique bit mask such that multiple options
 * may be specified.  These constants indicate the available options
 * in active state.
 */
#define XUARTPS_OPTION_SET_BREAK	        0x0080U         /*!< Starts break transmission */
#define XUARTPS_OPTION_STOP_BREAK	        0x0040U         /*!< Stops break transmission */
#define XUARTPS_OPTION_RESET_TMOUT	        0x0020U         /*!< Reset the receive timeout */
#define XUARTPS_OPTION_RESET_TX		        0x0010U         /*!< Reset the transmitter */
#define XUARTPS_OPTION_RESET_RX		        0x0008U         /*!< Reset the receiver */
#define XUARTPS_OPTION_ASSERT_RTS	        0x0004U         /*!< Assert the RTS bit */
#define XUARTPS_OPTION_ASSERT_DTR	        0x0002U         /*!< Assert the DTR bit */
#define XUARTPS_OPTION_SET_FCM		        0x0001U         /*!< Turn on flow control mode */

/*!
 * @name Channel Operational Mode
 *
 * The UART can operate in one of four modes: Normal, Local Loopback, Remote Loopback, or automatic echo.
 */
#define XUARTPS_OPER_MODE_NORMAL		    0x00U	        /*!< Normal Mode */
#define XUARTPS_OPER_MODE_AUTO_ECHO		    0x01U	        /*!< Auto Echo Mode */
#define XUARTPS_OPER_MODE_LOCAL_LOOP	    0x02U	        /*!< Local Loopback Mode */
#define XUARTPS_OPER_MODE_REMOTE_LOOP	    0x03U	        /*!< Remote Loopback Mode */

/*!
 * @name Data format values
 *
 * These constants specify the data format that the driver supports.
 * The data format includes the number of data bits, the number of stop bits and parity.
 */
#define XUARTPS_FORMAT_8_BITS		        0U              /*!< 8 data bits */
#define XUARTPS_FORMAT_7_BITS		        2U              /*!< 7 data bits */
#define XUARTPS_FORMAT_6_BITS		        3U              /*!< 6 data bits */

#define XUARTPS_FORMAT_NO_PARITY	        4U              /*!< No parity */
#define XUARTPS_FORMAT_MARK_PARITY	        3U              /*!< Mark parity */
#define XUARTPS_FORMAT_SPACE_PARITY	        2U              /*!< parity */
#define XUARTPS_FORMAT_ODD_PARITY	        1U              /*!< Odd parity */
#define XUARTPS_FORMAT_EVEN_PARITY	        0U              /*!< Even parity */

#define XUARTPS_FORMAT_2_STOP_BIT	        2U              /*!< 2 stop bits */
#define XUARTPS_FORMAT_1_5_STOP_BIT	        1U              /*!< 1.5 stop bits */
#define XUARTPS_FORMAT_1_STOP_BIT	        0U              /*!< 1 stop bit */

/*!< This typedef contains configuration information for the device. */
typedef struct 
{
    kuint16_t DeviceId;	                                    /*!< Unique ID  of device */
    kuint32_t BaseAddress;                                  /*!< Base address of device (IPIF) */
    kuint32_t InputClockHz;                                 /*!< Input clock frequency */
    kint32_t ModemPinsConnected;                            /*!< Specifies whether modem pins are connected to MIO or EMIO */

} XUartPs_Config;

/*!< Keep track of state information about a data buffer in the interrupt mode. */
typedef struct 
{
    kuint8_t *NextBytePtr;
    kuint32_t RequestedBytes;
    kuint32_t RemainingBytes;

} XUartPsBuffer;

/*!< Keep track of data format setting of a device. */
typedef struct 
{
    kuint32_t BaudRate;	                                    /*!< In bps, ie 1200 */
    kuint32_t DataBits;	                                    /*!< Number of data bits */
    kuint32_t Parity;		                                /*!< Parity */
    kuint8_t StopBits;	                                    /*!< Number of stop bits */

} XUartPsFormat;

/*!<
 * The XUartPs driver instance data structure. A pointer to an instance data
 * structure is passed around by functions to refer to a specific driver
 * instance.
 */
typedef struct 
{
    XUartPs_Config sgrt_cfg;	                            /*!< Configuration data structure */
    kuint32_t InputClockHz;	                                /*!< Input clock frequency */
    kuint32_t BaudRate;		                                /*!< Current baud rate */

    XUartPsBuffer sgrt_txbuf;
    XUartPsBuffer sgrt_rxbuf;

    kbool_t IsReady;		                                /*!< Device is initialized and ready */
    kbool_t is_rxbs_error;

} XUartPs;

/*!< ---------------------------------------------------------------------
                                SdPs                                   
 ---------------------------------------------------------------------- */
/*!< @name Register Map
 *
 * Register offsets from the base address of an SD device.
 * @{
 */

#define XSDPS_SDMA_SYS_ADDR_OFFSET	        0x00U	        /*!< SDMA System Address Register */
/*!< SDMA System Address Low Register */
#define XSDPS_SDMA_SYS_ADDR_LO_OFFSET	    XSDPS_SDMA_SYS_ADDR_OFFSET 
#define XSDPS_ARGMT2_LO_OFFSET		        0x00U	        /*!< Argument2 Low Register */
#define XSDPS_SDMA_SYS_ADDR_HI_OFFSET	    0x02U	        /*!< SDMA System Address High Register */
#define XSDPS_ARGMT2_HI_OFFSET		        0x02U	        /*!< Argument2 High Register */

#define XSDPS_BLK_SIZE_OFFSET		        0x04U	        /*!< Block Size Register */
#define XSDPS_BLK_CNT_OFFSET		        0x06U	        /*!< Block Count Register */
#define XSDPS_ARGMT_OFFSET		            0x08U	        /*!< Argument Register */
/*!< Argument1 Register */
#define XSDPS_ARGMT1_LO_OFFSET		        XSDPS_ARGMT_OFFSET
#define XSDPS_ARGMT1_HI_OFFSET		        0x0AU	        /*!< Argument1 Register */

#define XSDPS_XFER_MODE_OFFSET		        0x0CU	        /*!< Transfer Mode Register */
#define XSDPS_CMD_OFFSET		            0x0EU	        /*!< Command Register */
#define XSDPS_RESP0_OFFSET		            0x10U	        /*!< Response0 Register */
#define XSDPS_RESP1_OFFSET		            0x14U	        /*!< Response1 Register */
#define XSDPS_RESP2_OFFSET		            0x18U	        /*!< Response2 Register */
#define XSDPS_RESP3_OFFSET		            0x1CU	        /*!< Response3 Register */
#define XSDPS_BUF_DAT_PORT_OFFSET	        0x20U	        /*!< Buffer Data Port */
#define XSDPS_PRES_STATE_OFFSET		        0x24U	        /*!< Present State */
#define XSDPS_HOST_CTRL1_OFFSET		        0x28U	        /*!< Host Control 1 */
#define XSDPS_POWER_CTRL_OFFSET		        0x29U	        /*!< Power Control */
#define XSDPS_BLK_GAP_CTRL_OFFSET	        0x2AU	        /*!< Block Gap Control */
#define XSDPS_WAKE_UP_CTRL_OFFSET	        0x2BU	        /*!< Wake Up Control */
#define XSDPS_CLK_CTRL_OFFSET		        0x2CU	        /*!< Clock Control */
#define XSDPS_TIMEOUT_CTRL_OFFSET	        0x2EU	        /*!< Timeout Control */
#define XSDPS_SW_RST_OFFSET		            0x2FU	        /*!< Software Reset */
#define XSDPS_NORM_INTR_STS_OFFSET 	        0x30U	        /*!< Normal Interrupt Status Register */
#define XSDPS_ERR_INTR_STS_OFFSET 	        0x32U	        /*!< Error Interrupt Status Register */
#define XSDPS_NORM_INTR_STS_EN_OFFSET	    0x34U	        /*!< Normal Interrupt Status Enable Register */
#define XSDPS_ERR_INTR_STS_EN_OFFSET	    0x36U	        /*!< Error Interrupt Status Enable Register */
#define XSDPS_NORM_INTR_SIG_EN_OFFSET	    0x38U	        /*!< Normal Interrupt Signal Enable Register */
#define XSDPS_ERR_INTR_SIG_EN_OFFSET	    0x3AU	        /*!< Error Interrupt Signal Enable Register */

#define XSDPS_AUTO_CMD12_ERR_STS_OFFSET	    0x3CU	        /*!< Auto CMD12 Error Status Register */
#define XSDPS_HOST_CTRL2_OFFSET		        0x3EU	        /*!< Host Control2 Register */
#define XSDPS_CAPS_OFFSET 		            0x40U	        /*!< Capabilities Register */
#define XSDPS_CAPS_EXT_OFFSET 		        0x44U	        /*!< Capabilities Extended */
#define XSDPS_MAX_CURR_CAPS_OFFSET	        0x48U	        /*!< Maximum Current Capabilities Register */
#define XSDPS_MAX_CURR_CAPS_EXT_OFFSET	    0x4CU	        /*!< Maximum Current Capabilities Ext Register */
#define XSDPS_FE_ERR_INT_STS_OFFSET	        0x52U	        /*!< Force Event for Error Interrupt Status */
#define XSDPS_FE_AUTO_CMD12_EIS_OFFSET	    0x50U	        /*!< Auto CM12 Error Interrupt Status Register */
#define XSDPS_ADMA_ERR_STS_OFFSET	        0x54U	        /*!< ADMA Error Status Register */
#define XSDPS_ADMA_SAR_OFFSET		        0x58U	        /*!< ADMA System Address Register */
#define XSDPS_ADMA_SAR_EXT_OFFSET	        0x5CU	        /*!< ADMA System Address Extended Register */
#define XSDPS_PRE_VAL_1_OFFSET		        0x60U	        /*!< Preset Value Register */
#define XSDPS_PRE_VAL_2_OFFSET		        0x64U	        /*!< Preset Value Register */
#define XSDPS_PRE_VAL_3_OFFSET		        0x68U	        /*!< Preset Value Register */
#define XSDPS_PRE_VAL_4_OFFSET		        0x6CU	        /*!< Preset Value Register */
#define XSDPS_BOOT_TOUT_CTRL_OFFSET	        0x70U	        /*!< Boot timeout control register */

#define XSDPS_SHARED_BUS_CTRL_OFFSET	    0xE0U	        /*!< Shared Bus Control Register */
#define XSDPS_SLOT_INTR_STS_OFFSET	        0xFCU	        /*!< Slot Interrupt Status Register */
#define XSDPS_HOST_CTRL_VER_OFFSET	        0xFEU	        /*!< Host Controller Version Register */

/*!< @} */

/*!< @name Control Register - Host control, Power control,
 * 			Block Gap control and Wakeup control
 *
 * This register contains bits for various configuration options of
 * the SD host controller. Read/Write apart from the reserved bits.
 * @{
 */

#define XSDPS_HC_LED_MASK		            0x00000001U     /**< LED Control */
#define XSDPS_HC_WIDTH_MASK		            0x00000002U     /**< Bus width */
#define XSDPS_HC_BUS_WIDTH_4		        0x00000002U
#define XSDPS_HC_SPEED_MASK		            0x00000004U     /**< High Speed */
#define XSDPS_HC_DMA_MASK		            0x00000018U     /**< DMA Mode Select */
#define XSDPS_HC_DMA_SDMA_MASK		        0x00000000U     /**< SDMA Mode */
#define XSDPS_HC_DMA_ADMA1_MASK		        0x00000008U     /**< ADMA1 Mode */
#define XSDPS_HC_DMA_ADMA2_32_MASK	        0x00000010U     /**< ADMA2 Mode - 32 bit */
#define XSDPS_HC_DMA_ADMA2_64_MASK	        0x00000018U     /**< ADMA2 Mode - 64 bit */
#define XSDPS_HC_EXT_BUS_WIDTH		        0x00000020U     /**< Bus width - 8 bit */
#define XSDPS_HC_CARD_DET_TL_MASK	        0x00000040U     /**< Card Detect Tst Lvl */
#define XSDPS_HC_CARD_DET_SD_MASK	        0x00000080U     /**< Card Detect Sig Det */

#define XSDPS_PC_BUS_PWR_MASK		        0x00000001U     /**< Bus Power Control */
#define XSDPS_PC_BUS_VSEL_MASK		        0x0000000EU     /**< Bus Voltage Select */
#define XSDPS_PC_BUS_VSEL_3V3_MASK	        0x0000000EU     /**< Bus Voltage 3.3V */
#define XSDPS_PC_BUS_VSEL_3V0_MASK	        0x0000000CU     /**< Bus Voltage 3.0V */
#define XSDPS_PC_BUS_VSEL_1V8_MASK	        0x0000000AU     /**< Bus Voltage 1.8V */
#define XSDPS_PC_EMMC_HW_RST_MASK	        0x00000010U     /**< HW reset for eMMC */

#define XSDPS_BGC_STP_REQ_MASK		        0x00000001U     /**< Block Gap Stop Req */
#define XSDPS_BGC_CNT_REQ_MASK		        0x00000002U     /**< Block Gap Cont Req */
#define XSDPS_BGC_RWC_MASK		            0x00000004U     /**< Block Gap Rd Wait */
#define XSDPS_BGC_INTR_MASK		            0x00000008U     /**< Block Gap Intr */
#define XSDPS_BGC_SPI_MODE_MASK		        0x00000010U     /**< Block Gap SPI Mode */
#define XSDPS_BGC_BOOT_EN_MASK		        0x00000020U     /**< Block Gap Boot Enb */
#define XSDPS_BGC_ALT_BOOT_EN_MASK	        0x00000040U     /**< Block Gap Alt BootEn */
#define XSDPS_BGC_BOOT_ACK_MASK		        0x00000080U     /**< Block Gap Boot Ack */

#define XSDPS_WC_WUP_ON_INTR_MASK	        0x00000001U     /**< Wakeup Card Intr */
#define XSDPS_WC_WUP_ON_INSRT_MASK	        0x00000002U     /**< Wakeup Card Insert */
#define XSDPS_WC_WUP_ON_REM_MASK	        0x00000004U     /**< Wakeup Card Removal */

/*!< @} */

/*!< @name Control Register - Clock control, Timeout control & Software reset
 *
 * This register contains bits for configuration options of clock, timeout and
 * software reset.
 * Read/Write except for Inter_Clock_Stable bit (read only) and reserved bits.
 * @{
 */

#define XSDPS_CC_INT_CLK_EN_MASK		    0x00000001U
#define XSDPS_CC_INT_CLK_STABLE_MASK	    0x00000002U
#define XSDPS_CC_SD_CLK_EN_MASK			    0x00000004U
#define XSDPS_CC_SD_CLK_GEN_SEL_MASK		0x00000020U
#define XSDPS_CC_SDCLK_FREQ_SEL_EXT_MASK	0x000000C0U
#define XSDPS_CC_SDCLK_FREQ_SEL_MASK		0x0000FF00U
#define XSDPS_CC_SDCLK_FREQ_D256_MASK		0x00008000U
#define XSDPS_CC_SDCLK_FREQ_D128_MASK		0x00004000U
#define XSDPS_CC_SDCLK_FREQ_D64_MASK		0x00002000U
#define XSDPS_CC_SDCLK_FREQ_D32_MASK		0x00001000U
#define XSDPS_CC_SDCLK_FREQ_D16_MASK		0x00000800U
#define XSDPS_CC_SDCLK_FREQ_D8_MASK		    0x00000400U
#define XSDPS_CC_SDCLK_FREQ_D4_MASK		    0x00000200U
#define XSDPS_CC_SDCLK_FREQ_D2_MASK		    0x00000100U
#define XSDPS_CC_SDCLK_FREQ_BASE_MASK	    0x00000000U
#define XSDPS_CC_MAX_DIV_CNT			    256U
#define XSDPS_CC_EXT_MAX_DIV_CNT		    2046U
#define XSDPS_CC_EXT_DIV_SHIFT			    6U

#define XSDPS_TC_CNTR_VAL_MASK			    0x0000000FU

#define XSDPS_SWRST_ALL_MASK			    0x00000001U
#define XSDPS_SWRST_CMD_LINE_MASK		    0x00000002U
#define XSDPS_SWRST_DAT_LINE_MASK		    0x00000004U

#define XSDPS_CC_MAX_NUM_OF_DIV		        9U
#define XSDPS_CC_DIV_SHIFT		            8U

/*! @name interrupt register */
#define XSDPS_INTR_CC_MASK		            0x00000001U     /**< Command Complete */
#define XSDPS_INTR_TC_MASK		            0x00000002U     /**< Transfer Complete */
#define XSDPS_INTR_BGE_MASK		            0x00000004U     /**< Block Gap Event */
#define XSDPS_INTR_DMA_MASK		            0x00000008U     /**< DMA Interrupt */
#define XSDPS_INTR_BWR_MASK		            0x00000010U     /**< Buffer Write Ready */
#define XSDPS_INTR_BRR_MASK		            0x00000020U     /**< Buffer Read Ready */
#define XSDPS_INTR_CARD_INSRT_MASK	        0x00000040U     /**< Card Insert */
#define XSDPS_INTR_CARD_REM_MASK	        0x00000080U     /**< Card Remove */
#define XSDPS_INTR_CARD_MASK		        0x00000100U     /**< Card Interrupt */
#define XSDPS_INTR_INT_A_MASK		        0x00000200U     /**< INT A Interrupt */
#define XSDPS_INTR_INT_B_MASK		        0x00000400U     /**< INT B Interrupt */
#define XSDPS_INTR_INT_C_MASK		        0x00000800U     /**< INT C Interrupt */
#define XSDPS_INTR_RE_TUNING_MASK	        0x00001000U     /**< Re-Tuning Interrupt */
#define XSDPS_INTR_BOOT_ACK_RECV_MASK	    0x00002000U     /**< Boot Ack Recv Interrupt */
#define XSDPS_INTR_BOOT_TERM_MASK	        0x00004000U     /**< Boot Terminate Interrupt */
#define XSDPS_INTR_ERR_MASK		            0x00008000U     /**< Error Interrupt */
#define XSDPS_NORM_INTR_ALL_MASK	        0x0000FFFFU

#define XSDPS_INTR_ERR_CT_MASK		        0x00000001U     /**< Command Timeout Error */
#define XSDPS_INTR_ERR_CCRC_MASK	        0x00000002U     /**< Command CRC Error */
#define XSDPS_INTR_ERR_CEB_MASK		        0x00000004U     /**< Command End Bit Error */
#define XSDPS_INTR_ERR_CI_MASK		        0x00000008U     /**< Command Index Error */
#define XSDPS_INTR_ERR_DT_MASK		        0x00000010U     /**< Data Timeout Error */
#define XSDPS_INTR_ERR_DCRC_MASK	        0x00000020U     /**< Data CRC Error */
#define XSDPS_INTR_ERR_DEB_MASK		        0x00000040U     /**< Data End Bit Error */
#define XSDPS_INTR_ERR_CUR_LMT_MASK	        0x00000080U     /**< Current Limit Error */
#define XSDPS_INTR_ERR_AUTO_CMD12_MASK	    0x00000100U     /**< Auto CMD12 Error */
#define XSDPS_INTR_ERR_ADMA_MASK	        0x00000200U     /**< ADMA Error */
#define XSDPS_INTR_ERR_TR_MASK		        0x00001000U     /**< Tuning Error */
#define XSDPS_INTR_VEND_SPF_ERR_MASK	    0x0000E000U     /**< Vendor Specific Error */
#define XSDPS_ERROR_INTR_ALL_MASK	        0x0000F3FFU     /**< Mask for error bits */
/*!< @} */

/*!< @name Block Size and Block Count Register
 *
 * This register contains the block count for current transfer,
 * block size and SDMA buffer size.
 * Read/Write except for reserved bits.
 * @{
 */

#define XSDPS_BLK_SIZE_MASK		            0x00000FFFU     /**< Transfer Block Size */
#define XSDPS_SDMA_BUFF_SIZE_MASK	        0x00007000U     /**< Host SDMA Buffer Size */
#define XSDPS_BLK_SIZE_1024		            0x400U
#define XSDPS_BLK_SIZE_2048		            0x800U
#define XSDPS_BLK_CNT_MASK		            0x0000FFFFU     /**< Block Count for Current Transfer */

/*!< @} */

/*!< @name Transfer Mode and Command Register
 *
 * The Transfer Mode register is used to control the data transfers and
 * Command register is used for command generation
 * Read/Write except for reserved bits.
 * @{
 */

#define XSDPS_TM_DMA_EN_MASK		        0x00000001U     /**< DMA Enable */
#define XSDPS_TM_BLK_CNT_EN_MASK	        0x00000002U     /**< Block Count Enable */
#define XSDPS_TM_AUTO_CMD12_EN_MASK	        0x00000004U     /**< Auto CMD12 Enable */
#define XSDPS_TM_DAT_DIR_SEL_MASK	        0x00000010U     /**< Data Transfer Direction Select */
#define XSDPS_TM_MUL_SIN_BLK_SEL_MASK	    0x00000020U     /**< Multi/Single Block Select */

#define XSDPS_CMD_RESP_SEL_MASK		        0x00000003U     /**< Response Type Select */
#define XSDPS_CMD_RESP_NONE_MASK	        0x00000000U     /**< No Response */
#define XSDPS_CMD_RESP_L136_MASK	        0x00000001U     /**< Response length 138 */
#define XSDPS_CMD_RESP_L48_MASK		        0x00000002U     /**< Response length 48 */
#define XSDPS_CMD_RESP_L48_BSY_CHK_MASK	    0x00000003U     /**< Response length 48 & check busy after response */
#define XSDPS_CMD_CRC_CHK_EN_MASK	        0x00000008U     /**< Command CRC Check Enable */
#define XSDPS_CMD_INX_CHK_EN_MASK	        0x00000010U     /**< Command Index Check Enable */
#define XSDPS_DAT_PRESENT_SEL_MASK	        0x00000020U     /**< Data Present Select */
#define XSDPS_CMD_TYPE_MASK		            0x000000C0U     /**< Command Type */
#define XSDPS_CMD_TYPE_NORM_MASK	        0x00000000U     /**< CMD Type - Normal */
#define XSDPS_CMD_TYPE_SUSPEND_MASK	        0x00000040U     /**< CMD Type - Suspend */
#define XSDPS_CMD_TYPE_RESUME_MASK	        0x00000080U     /**< CMD Type - Resume */
#define XSDPS_CMD_TYPE_ABORT_MASK	        0x000000C0U     /**< CMD Type - Abort */
#define XSDPS_CMD_MASK			            0x00003F00U     /**< Command Index Mask - Set to CMD0-63, AMCD0-63 */

/*!< @name Auto CMD Error Status Register
 *
 * This register is read only register which contains
 * information about the error status of Auto CMD 12 and 23.
 * Read Only
 * @{
 */
#define XSDPS_AUTO_CMD12_NT_EX_MASK	        0x0001U         /**< Auto CMD12 Not executed */
#define XSDPS_AUTO_CMD_TOUT_MASK	        0x0002U         /**< Auto CMD Timeout Error */
#define XSDPS_AUTO_CMD_CRC_MASK		        0x0004U         /**< Auto CMD CRC Error */
#define XSDPS_AUTO_CMD_EB_MASK		        0x0008U         /**< Auto CMD End Bit Error */
#define XSDPS_AUTO_CMD_IND_MASK		        0x0010U         /**< Auto CMD Index Error */
#define XSDPS_AUTO_CMD_CNI_ERR_MASK	        0x0080U         /**< Command not issued by Auto CMD12 Error */
/*!< @} */

/*!< @name Host Control2 Register
 *
 * This register contains extended configuration bits.
 * Read Write
 * @{
 */
#define XSDPS_HC2_UHS_MODE_MASK		        0x0007U         /**< UHS Mode select bits */
#define XSDPS_HC2_UHS_MODE_SDR12_MASK	    0x0000U         /**< SDR12 UHS Mode */
#define XSDPS_HC2_UHS_MODE_SDR25_MASK	    0x0001U         /**< SDR25 UHS Mode */
#define XSDPS_HC2_UHS_MODE_SDR50_MASK	    0x0002U         /**< SDR50 UHS Mode */
#define XSDPS_HC2_UHS_MODE_SDR104_MASK	    0x0003U         /**< SDR104 UHS Mode */
#define XSDPS_HC2_UHS_MODE_DDR50_MASK	    0x0004U         /**< DDR50 UHS Mode */
#define XSDPS_HC2_1V8_EN_MASK		        0x0008U         /**< 1.8V Signal Enable */
#define XSDPS_HC2_DRV_STR_SEL_MASK	        0x0030U         /**< Driver Strength Selection */
#define XSDPS_HC2_DRV_STR_B_MASK	        0x0000U         /**< Driver Strength B */
#define XSDPS_HC2_DRV_STR_A_MASK	        0x0010U         /**< Driver Strength A */
#define XSDPS_HC2_DRV_STR_C_MASK	        0x0020U         /**< Driver Strength C */
#define XSDPS_HC2_DRV_STR_D_MASK	        0x0030U         /**< Driver Strength D */
#define XSDPS_HC2_EXEC_TNG_MASK		        0x0040U         /**< Execute Tuning */
#define XSDPS_HC2_SAMP_CLK_SEL_MASK	        0x0080U         /**< Sampling Clock Selection */
#define XSDPS_HC2_ASYNC_INTR_EN_MASK	    0x4000U         /**< Asynchronous Interrupt Enable */
#define XSDPS_HC2_PRE_VAL_EN_MASK	        0x8000U         /**< Preset Value Enable */

/*!< @} */

/*!< @name Capabilities Register
 *
 * Capabilities register is a read only register which contains
 * information about the host controller.
 * Sufficient if read once after power on.
 * Read Only
 * @{
 */
#define XSDPS_CAP_TOUT_CLK_FREQ_MASK	    0x0000003FU     /**< Timeout clock freq select */
#define XSDPS_CAP_TOUT_CLK_UNIT_MASK	    0x00000080U     /**< Timeout clock unit - MHz/KHz */
#define XSDPS_CAP_MAX_BLK_LEN_MASK	        0x00030000U     /**< Max block length */
#define XSDPS_CAP_MAX_BLK_LEN_512B_MASK	    0x00000000U     /**< Max block 512 bytes */
#define XSDPS_CAP_MAX_BL_LN_1024_MASK	    0x00010000U     /**< Max block 1024 bytes */
#define XSDPS_CAP_MAX_BL_LN_2048_MASK	    0x00020000U     /**< Max block 2048 bytes */
#define XSDPS_CAP_MAX_BL_LN_4096_MASK	    0x00030000U     /**< Max block 4096 bytes */

#define XSDPS_CAP_EXT_MEDIA_BUS_MASK	    0x00040000U     /**< Extended media bus */
#define XSDPS_CAP_ADMA2_MASK		        0x00080000U     /**< ADMA2 support */
#define XSDPS_CAP_HIGH_SPEED_MASK	        0x00200000U     /**< High speed support */
#define XSDPS_CAP_SDMA_MASK		            0x00400000U     /**< SDMA support */
#define XSDPS_CAP_SUSP_RESUME_MASK	        0x00800000U     /**< Suspend/Resume support */
#define XSDPS_CAP_VOLT_3V3_MASK		        0x01000000U     /**< 3.3V support */
#define XSDPS_CAP_VOLT_3V0_MASK		        0x02000000U     /**< 3.0V support */
#define XSDPS_CAP_VOLT_1V8_MASK		        0x04000000U     /**< 1.8V support */

#define XSDPS_CAP_SYS_BUS_64_MASK	        0x10000000U     /**< 64 bit system bus  support */

/*!< Spec 2.0 */
#define XSDPS_CAP_INTR_MODE_MASK	        0x08000000U     /**< Interrupt mode support */
#define XSDPS_CAP_SPI_MODE_MASK		        0x20000000U     /**< SPI mode */
#define XSDPS_CAP_SPI_BLOCK_MODE_MASK	    0x40000000U     /**< SPI block mode */

/*!< Spec 3.0 */
#define XSDPS_CAPS_ASYNC_INTR_MASK	        0x20000000U     /**< Async Interrupt support */
#define XSDPS_CAPS_SLOT_TYPE_MASK	        0xC0000000U     /**< Slot Type */
#define XSDPS_CAPS_REM_CARD			        0x00000000U     /**< Removable Slot */
#define XSDPS_CAPS_EMB_SLOT			        0x40000000U     /**< Embedded Slot */
#define XSDPS_CAPS_SHR_BUS			        0x80000000U     /**< Shared Bus Slot */

#define XSDPS_ECAPS_SDR50_MASK		        0x00000001U     /**< SDR50 Mode support */
#define XSDPS_ECAPS_SDR104_MASK		        0x00000002U     /**< SDR104 Mode support */
#define XSDPS_ECAPS_DDR50_MASK		        0x00000004U     /**< DDR50 Mode support */
#define XSDPS_ECAPS_DRV_TYPE_A_MASK	        0x00000010U     /**< DriverType A support */
#define XSDPS_ECAPS_DRV_TYPE_C_MASK	        0x00000020U     /**< DriverType C support */
#define XSDPS_ECAPS_DRV_TYPE_D_MASK	        0x00000040U     /**< DriverType D support */
#define XSDPS_ECAPS_TMR_CNT_MASK	        0x00000F00U     /**< Timer Count for Re-tuning */
#define XSDPS_ECAPS_USE_TNG_SDR50_MASK	    0x00002000U     /**< SDR50 Mode needs tuning */
#define XSDPS_ECAPS_RE_TNG_MODES_MASK	    0x0000C000U     /**< Re-tuning modes support */
#define XSDPS_ECAPS_RE_TNG_MODE1_MASK	    0x00000000U     /**< Re-tuning mode 1 */
#define XSDPS_ECAPS_RE_TNG_MODE2_MASK	    0x00004000U     /**< Re-tuning mode 2 */
#define XSDPS_ECAPS_RE_TNG_MODE3_MASK	    0x00008000U     /**< Re-tuning mode 3 */
#define XSDPS_ECAPS_CLK_MULT_MASK	        0x00FF0000U     /**< Clock Multiplier value for Programmable clock mode */
#define XSDPS_ECAPS_SPI_MODE_MASK	        0x01000000U     /**< SPI mode */
#define XSDPS_ECAPS_SPI_BLK_MODE_MASK	    0x02000000U     /**< SPI block mode */

/*!< @name Present State Register
 *
 * Gives the current status of the host controller
 * Read Only
 * @{
 */

#define XSDPS_PSR_INHIBIT_CMD_MASK	        0x00000001U     /**< Command inhibit - CMD */
#define XSDPS_PSR_INHIBIT_DAT_MASK	        0x00000002U     /**< Command Inhibit - DAT */
#define XSDPS_PSR_DAT_ACTIVE_MASK	        0x00000004U     /**< DAT line active */
#define XSDPS_PSR_RE_TUNING_REQ_MASK	    0x00000008U     /**< Re-tuning request */
#define XSDPS_PSR_WR_ACTIVE_MASK	        0x00000100U     /**< Write transfer active */
#define XSDPS_PSR_RD_ACTIVE_MASK	        0x00000200U     /**< Read transfer active */
#define XSDPS_PSR_BUFF_WR_EN_MASK	        0x00000400U     /**< Buffer write enable */
#define XSDPS_PSR_BUFF_RD_EN_MASK	        0x00000800U     /**< Buffer read enable */
#define XSDPS_PSR_CARD_INSRT_MASK	        0x00010000U     /**< Card inserted */
#define XSDPS_PSR_CARD_STABLE_MASK	        0x00020000U     /**< Card state stable */
#define XSDPS_PSR_CARD_DPL_MASK		        0x00040000U     /**< Card detect pin level */
#define XSDPS_PSR_WPS_PL_MASK		        0x00080000U     /**< Write protect switch pin level */
#define XSDPS_PSR_DAT30_SG_LVL_MASK	        0x00F00000U     /**< Data 3:0 signal lvl */
#define XSDPS_PSR_CMD_SG_LVL_MASK	        0x01000000U     /**< Cmd Line signal lvl */
#define XSDPS_PSR_DAT74_SG_LVL_MASK	        0x1E000000U     /**< Data 7:4 signal lvl */

/*!< @name Host Controller Version Register
 *
 * This register is read only register which contains
 * Host Controller and Vendor Specific version.
 * Read Only
 * @{
 */
#define XSDPS_HC_VENDOR_VER		            0xFF00U         /**< Vendor Specification version mask */
#define XSDPS_HC_SPEC_VER_MASK		        0x00FFU         /**< Host Specification version mask */
#define XSDPS_HC_SPEC_V3		            0x0002U
#define XSDPS_HC_SPEC_V2		            0x0001U
#define XSDPS_HC_SPEC_V1		            0x0000U

#define XSDPS_BLK_SIZE_512_MASK	            0x200U

#define XSDPS_WIDTH_8		                8U              /*!< The maximun of bus width supported: eMMC */
#define XSDPS_WIDTH_4		                4U              /*!< The maximun of bus width supported: SD */

#define XSDPS_CARD_SD		                1U
#define XSDPS_CARD_MMC		                2U
#define XSDPS_CARD_SDIO		                3U
#define XSDPS_CARD_SDCOMBO	                4U
#define XSDPS_CHIP_EMMC		                5U

#define HIGH_SPEED_SUPPORT	                0x2U
#define UHS_SDR50_SUPPORT	                0x4U
#define WIDTH_4_BIT_SUPPORT	                0x4U
#define SD_CLK_25_MHZ		                25000000U
#define SD_CLK_19_MHZ		                19000000U
#define SD_CLK_26_MHZ		                26000000U

/*!< @name ADMA2 Descriptor related definitions
 *
 * ADMA2 Descriptor related definitions
 * @{
 */

#define XSDPS_DESC_MAX_LENGTH               65536U

#define XSDPS_DESC_VALID     	            (0x1U << 0)
#define XSDPS_DESC_END       	            (0x1U << 1)
#define XSDPS_DESC_INT       	            (0x1U << 2)
#define XSDPS_DESC_TRAN  	                (0x2U << 4)

/*!< For changing clock frequencies */
#define XSDPS_CLK_400_KHZ					400000U		    /*!< 400 KHZ */
#define XSDPS_CLK_50_MHZ					50000000U	    /*!< 50 MHZ */
#define XSDPS_CLK_52_MHZ					52000000U	    /*!< 52 MHZ */
#define XSDPS_SD_VER_1_0					0x1U		    /*!< SD ver 1 */
#define XSDPS_SD_VER_2_0					0x2U		    /*!< SD ver 2 */
#define XSDPS_SCR_BLKCNT					1U
#define XSDPS_SCR_BLKSIZE					8U
#define XSDPS_1_BIT_WIDTH					0x1U
#define XSDPS_4_BIT_WIDTH					0x2U
#define XSDPS_8_BIT_WIDTH					0x3U
#define XSDPS_UHS_SPEED_MODE_SDR12			0x0U
#define XSDPS_UHS_SPEED_MODE_SDR25			0x1U
#define XSDPS_UHS_SPEED_MODE_SDR50			0x2U
#define XSDPS_UHS_SPEED_MODE_SDR104			0x3U
#define XSDPS_UHS_SPEED_MODE_DDR50			0x4U
#define XSDPS_HIGH_SPEED_MODE				0x5U
#define XSDPS_DEFAULT_SPEED_MODE			0x6U
#define XSDPS_HS200_MODE					0x7U
#define XSDPS_DDR52_MODE					0x4U
#define XSDPS_SWITCH_CMD_BLKCNT				1U
#define XSDPS_SWITCH_CMD_BLKSIZE			64U
#define XSDPS_SWITCH_CMD_HS_GET				0x00FFFFF0U
#define XSDPS_SWITCH_CMD_HS_SET				0x80FFFFF1U
#define XSDPS_SWITCH_CMD_SDR12_SET			0x80FFFFF0U
#define XSDPS_SWITCH_CMD_SDR25_SET			0x80FFFFF1U
#define XSDPS_SWITCH_CMD_SDR50_SET			0x80FFFFF2U
#define XSDPS_SWITCH_CMD_SDR104_SET			0x80FFFFF3U
#define XSDPS_SWITCH_CMD_DDR50_SET			0x80FFFFF4U
#define XSDPS_EXT_CSD_CMD_BLKCNT			1U
#define XSDPS_EXT_CSD_CMD_BLKSIZE			512U
#define XSDPS_TUNING_CMD_BLKCNT				1U
#define XSDPS_TUNING_CMD_BLKSIZE			64U

#define XSDPS_HIGH_SPEED_MAX_CLK			50000000U
#define XSDPS_UHS_SDR104_MAX_CLK			208000000U
#define XSDPS_UHS_SDR50_MAX_CLK				100000000U
#define XSDPS_UHS_DDR50_MAX_CLK				50000000U
#define XSDPS_UHS_SDR25_MAX_CLK				50000000U
#define XSDPS_UHS_SDR12_MAX_CLK				25000000U

#define ADDRESS_BEYOND_32BIT	            0x100000000U

#define XSDPS_RESP_NONE	                    (XSDPS_CMD_RESP_NONE_MASK)
#define XSDPS_RESP_R1		                (XSDPS_CMD_RESP_L48_MASK | XSDPS_CMD_CRC_CHK_EN_MASK | XSDPS_CMD_INX_CHK_EN_MASK)
#define XSDPS_RESP_R1B	                    (XSDPS_CMD_RESP_L48_BSY_CHK_MASK | XSDPS_CMD_CRC_CHK_EN_MASK | XSDPS_CMD_INX_CHK_EN_MASK)
#define XSDPS_RESP_R2		                (XSDPS_CMD_RESP_L136_MASK | XSDPS_CMD_CRC_CHK_EN_MASK)
#define XSDPS_RESP_R3		                (XSDPS_CMD_RESP_L48_MASK)
#define XSDPS_RESP_R6		                (XSDPS_CMD_RESP_L48_BSY_CHK_MASK | XSDPS_CMD_CRC_CHK_EN_MASK | XSDPS_CMD_INX_CHK_EN_MASK)

/*!< This typedef contains configuration information for the device. */
typedef struct 
{
    kuint16_t DeviceId;										/*!< Unique ID  of device */
    kuint32_t BaseAddress;									/*!< Base address of the device */
    kuint32_t InputClockHz;									/*!< Input clock frequency */
    kuint32_t CardDetect;									/*!< Card Detect */
    kuint32_t WriteProtect;									/*!< Write Protect */
    kuint32_t BusWidth;										/*!< Bus Width */
    kuint32_t BankNumber;									/*!< MIO Bank selection for SD */
    kuint32_t HasEMIO;										/*!< If SD is connected to EMIO */
    kuint8_t IsCacheCoherent; 								/*!< If SD is Cache Coherent or not */

} XSdPs_Config;

/*!< ADMA2 descriptor table */
typedef struct 
{
    kuint16_t Attribute;									/*!< Attributes of descriptor */
    kuint16_t Length;										/*!< Length of current dma transfer */
    kuint64_t Address;										/*!< Address of current dma transfer */

} __packed XSdPs_Adma2Descriptor;

/*!<
 * The XSdPs driver instance data. The user is required to allocate a
 * variable of this type for every SD device in the system. A pointer
 * to a variable of this type is then passed to the driver API functions.
 */
typedef struct 
{
    XSdPs_Config sgrt_cfg;									/*!< Configuration structure */
    kuint32_t Host_Caps;									/*!< Capabilities of host controller */
    kuint32_t Host_CapsExt;									/*!< Extended Capabilities */
    kuint32_t HCS;											/*!< High capacity support in card */
    kuint8_t  CardType;										/*!< Type of card - SD/MMC/eMMC */
    kuint8_t  Card_Version;									/*!< Card version */
    kuint8_t  HC_Version;									/*!< Host controller version */
    kuint8_t  BusWidth;										/*!< Current operating bus width */
    kuint32_t BusSpeed;										/*!< Current operating bus speed */
    kuint8_t  Switch1v8;									/*!< 1.8V Switch support */
    kuint32_t CardID[4];									/*!< Card ID Register */
    kuint32_t RelCardAddr;									/*!< Relative Card Address */
    kuint32_t CardSpecData[4];								/*!< Card Specific Data Register */
    kuint32_t SectorCount;									/*!< Sector Count */
    kuint32_t SdCardConfig;									/*!< Sd Card Configuration Register */
    kuint32_t Mode;											/*!< Bus Speed Mode */
    kuint32_t TransferMode;
    kbool_t IsReady;										/*!< Device is initialized and ready */

    XSdPs_Adma2Descriptor Adma2_DescrTbl[32] __align(32);

    kuint64_t Dma64BitAddr;									/*!< 64 Bit DMA Address */

} XSdPs;

/*!< ---------------------------------------------------------------------
                                Dynclk                                   
 ---------------------------------------------------------------------- */
#define CLK_BIT_WEDGE                       13
#define CLK_BIT_NOCOUNT                     12
#define ERR_CLKDIVIDER                      (1 << CLK_BIT_WEDGE | 1 << CLK_BIT_NOCOUNT)
#define ERR_CLKCOUNTCALC                    0xFFFFFFFF             //This value is used to signal an error

#define OFST_DYNCLK_CTRL                    0x0
#define OFST_DYNCLK_STATUS                  0x4
#define OFST_DYNCLK_CLK_L                   0x8
#define OFST_DYNCLK_FB_L                    0x0C
#define OFST_DYNCLK_FB_H_CLK_H              0x10
#define OFST_DYNCLK_DIV                     0x14
#define OFST_DYNCLK_LOCK_L                  0x18
#define OFST_DYNCLK_FLTR_LOCK_H             0x1C

#define BIT_DYNCLK_START                    0
#define BIT_DYNCLK_RUNNING                  0

typedef struct 
{
    kuint32_t clk0L;
    kuint32_t clkFBL;
    kuint32_t clkFBH_clk0H;
    kuint32_t divclk;
    kuint32_t lockL;
    kuint32_t fltr_lockH;

} ClkConfig;

typedef struct 
{
    kfloat_t freq;
    kuint32_t fbmult;
    kuint32_t clkdiv;
    kuint32_t maindiv;

} ClkMode;

/*!< ---------------------------------------------------------------------
                                VTC                                   
 ---------------------------------------------------------------------- */
typedef kuint32_t XAxiVdma_Bd[XAXIVDMA_BD_MINIMUM_ALIGNMENT_WD];

/*!<
 * Callback type for general interrupts
 *
 * @param   CallBackRef is a callback reference passed in by the upper layer
 *          when setting the callback functions, and passed back to the
 *          upper layer when the callback is called.
 * @param   InterruptTypes indicates the detailed type(s) of the interrupt.
 *          Its value equals 'OR'ing one or more XAXIVDMA_IXR_* values defined
 *          in xaxivdma_hw.h
 */
typedef void (*XAxiVdma_CallBack) (void *CallBackRef, kuint32_t InterruptTypes);

/*!<
 * Callback type for Error interrupt.
 *
 * @param   CallBackRef is a callback reference passed in by the upper layer
 *          when setting the callback function, and it is passed back to the
 *          upper layer when the callback is called.
 * @param   ErrorMask is a bit mask indicating the cause of the error. Its
 *          value equals 'OR'ing one or more XAXIVDMA_IXR_* values defined in
 *          xaxivdma_hw.h
 */
typedef void (*XAxiVdma_ErrorCallBack) (void *CallBackRef, kuint32_t ErrorMask);

/*!<
 * This typedef contains the hardware configuration information for a VDMA
 * device. Each VDMA device should have a configuration structure associated
 * with it.
 */
typedef struct 
{
    kuint16_t DeviceId;                                     /**< DeviceId is the unique ID  of the device */
    kuint32_t BaseAddress;                                  /**< BaseAddress is the physical base address of the device's registers */

    kuint16_t MaxFrameStoreNum;                             /**< The maximum number of Frame Stores */

    kint32_t HasMm2S;                                       /**< Whether hw build has read channel */
    kint32_t HasMm2SDRE;                                    /**< Read channel supports unaligned transfer */
    kint32_t Mm2SWordLen;                                   /**< Read channel word length */
    kint32_t HasS2Mm;                                       /**< Whether hw build has write channel */
    kint32_t HasS2MmDRE;                                    /**< Write channel supports unaligned transfer */
    kint32_t S2MmWordLen;                                   /**< Write channel word length */
    kint32_t HasSG;                                         /**< Whether hardware has SG engine */
    kint32_t EnableVIDParamRead;					        /**< Read Enable for video parameters in direct register mode */
    kint32_t UseFsync;	                                    /**< DMA operations synchronized to Frame Sync */
    kint32_t FlushonFsync;	                                /**< VDMA Transactions are flushed & channel states reset on Frame Sync */
    kint32_t Mm2SBufDepth;	                                /**< Depth of Read Channel Line Buffer FIFO */
    kint32_t S2MmBufDepth;	                                /**< Depth of Write Channel Line Buffer FIFO */
    kint32_t Mm2SGenLock;	                                /**< Mm2s Gen Lock Mode */
    kint32_t S2MmGenLock;	                                /**< S2Mm Gen Lock Mode */
    kint32_t InternalGenLock;                               /**< Internal Gen Lock */
    kint32_t S2MmSOF;	                                    /**< S2MM Start of Flag Enable */
    kint32_t Mm2SStreamWidth;                               /**< MM2S TData Width */
    kint32_t S2MmStreamWidth;                               /**< S2MM TData Width */
    kint32_t Mm2SThresRegEn;                                /**< MM2S Threshold Register Enable Flag This corresponds to C_ENABLE_DEBUG_INFO_1 configuration parameter */
    kint32_t Mm2SFrmStoreRegEn;                             /**< MM2S Frame Store Register Enable Flag This corresponds to C_ENABLE_DEBUG_INFO_5 configuration parameter */
    kint32_t Mm2SDlyCntrEn;	                                /**< MM2S Delay Counter (Control Reg) Enable Flag This corresponds to C_ENABLE_DEBUG_INFO_6 configuration parameter */
    kint32_t Mm2SFrmCntrEn;                                 /**< MM2S Frame Counter (Control Reg) Enable Flag This corresponds to C_ENABLE_DEBUG_INFO_7 configuration parameter */
    kint32_t S2MmThresRegEn;                                /**< S2MM Threshold Register Enable Flag This corresponds to C_ENABLE_DEBUG_INFO_9 configuration parameter */
    kint32_t S2MmFrmStoreRegEn;                             /**< S2MM Frame Store Register Enable Flag This corresponds to C_ENABLE_DEBUG_INFO_13 configuration parameter */
    kint32_t S2MmDlyCntrEn;	                                /**< S2MM Delay Counter (Control Reg) Enable Flag This corresponds to C_ENABLE_DEBUG_INFO_14  configuration parameter */
    kint32_t S2MmFrmCntrEn;	                                /**< S2MM Frame Counter (Control Reg) Enable Flag This corresponds to C_ENABLE_DEBUG_INFO_15 configuration parameter */
    kint32_t EnableAllDbgFeatures;                          /**< Enable all Debug features This corresponds to C_ENABLE_DEBUG_ALL configuration parameter */
    kint32_t AddrWidth;		                                /**< Address Width */
    kuint8_t HasVFlip;		                                /**< Whether hardware has Vertical Flip enabled(c_enable_vert_flip) */

} XAxiVdma_Config;

/*!<
 * Channel callback functions
 */
typedef struct 
{
    XAxiVdma_CallBack CompletionCallBack; 			        /**< Call back for completion intr */
    void *CompletionRef;                  			        /**< Call back ref */

    XAxiVdma_ErrorCallBack ErrCallBack;   			        /**< Call back for error intr */
    void *ErrRef;                         			        /**< Call back ref */

} XAxiVdma_ChannelCallBack;

/*!< The DMA channel is only visible to driver files
 */
typedef struct 
{
    kuint32_t ChanBase;       						        /*!< Base address for this channel */
    kuint32_t InstanceBase;   						        /*!< Base address for the whole device */
    kuint32_t StartAddrBase;  						        /*!< Start address register array base */

    kbool_t IsValid;        						        /*!< Whether the channel has been initialized */
    kint32_t FlushonFsync;							        /*!< VDMA Transactions are flushed & channel states reset on Frame Sync */
    kint32_t HasSG;          						        /*!< Whether hardware has SG engine */
    kint32_t IsRead;         						        /*!< Read or write channel */
    kint32_t HasDRE;         						        /*!< Whether support unaligned transfer */
    kint32_t LineBufDepth;							        /*!< Depth of Channel Line Buffer FIFO */
    kint32_t LineBufThreshold;						        /*!< Threshold point at which Channel Line almost empty flag asserts high */
    kint32_t WordLength;     						        /*!< Word length */
    kint32_t NumFrames;								        /*!< Number of frames to work on */

    kuint32_t HeadBdPhysAddr; 						        /*!< Physical address of the first BD */
    kuint32_t HeadBdAddr;     						        /*!< Virtual address of the first BD */
    kuint32_t TailBdPhysAddr; 						        /*!< Physical address of the last BD */
    kuint32_t TailBdAddr;     						        /*!< Virtual address of the last BD */
    kint32_t Hsize;          						        /*!< Horizontal size */
    kint32_t Vsize;          						        /*!< Vertical size saved for no-sg mode hw start */

    kint32_t AllCnt;         						        /*!< Total number of BDs */

    kint32_t GenLock;								        /*!< Mm2s Gen Lock Mode */
    kint32_t S2MmSOF;								        /*!< S2MM Start of Flag */
    kint32_t StreamWidth;     						        /*!< Stream Width */
    XAxiVdma_Bd BDs[XAXIVDMA_MAX_FRAMESTORE] __align(32);	/*Statically allocated BDs */
    kuint32_t DbgFeatureFlags; 						        /*!< Debug Parameter Flags */
    kint32_t AddrWidth;
    kint32_t direction;								        /*!< Determines whether Read or write channel */
    kuint8_t HasVFlip;  							        /*!< Whether hardware has Vertical Flip enabled */

}XAxiVdma_Channel;

/*!<
 * The XAxiVdma_DmaSetup structure contains all the necessary information to
 * start a frame write or read.
 *
 */
typedef struct 
{
    kint32_t VertSizeInput;                                 /**< Vertical size input */
    kint32_t HoriSizeInput;                                 /**< Horizontal size input */
    kint32_t Stride;                                        /**< Stride */
    kint32_t FrameDelay;                                    /**< Frame Delay */

    kint32_t EnableCircularBuf;                             /**< Circular Buffer Mode? */
    kint32_t EnableSync;                                    /**< Gen-Lock Mode? */
    kint32_t PointNum;                                      /**< Master we synchronize with */
    kint32_t EnableFrameCounter;                            /**< Frame Counter Enable */
    kuint32_t FrameStoreStartAddr[XAXIVDMA_MAX_FRAMESTORE]; /**< Start Addresses of Frame Store Buffers. */
    kint32_t FixedFrameStoreAddr;                           /**< Fixed Frame Store Address index */
    kint32_t GenLockRepeat;                                 /**< Gen-Lock Repeat? */
    kuint8_t EnableVFlip;	                                /**< Vertical Flip state */

} XAxiVdma_DmaSetup;

/*!< Duplicate layout of XAxiVdma_DmaSetup
 *
 * So to remove the dependency on xaxivdma.h
 */
typedef struct {
    kint32_t VertSizeInput;                                 /**< Vertical size input */
    kint32_t HoriSizeInput;                                 /**< Horizontal size input */
    kint32_t Stride;                                        /**< Stride */
    kint32_t FrameDelay;                                    /**< Frame Delay */

    kint32_t EnableCircularBuf;                             /**< Circular Buffer Mode? */
    kint32_t EnableSync;                                    /**< Gen-Lock Mode? */
    kint32_t PointNum;                                      /**< Master we synchronize with */
    kint32_t EnableFrameCounter;                            /**< Frame Counter Enable */
    kuint32_t FrameStoreStartAddr[XAXIVDMA_MAX_FRAMESTORE]; /**< Start Addresses of Frame Store Buffers. */
    kint32_t FixedFrameStoreAddr;                           /**< Fixed Frame Store Address index */
    kint32_t GenLockRepeat;                                 /**< Gen-Lock Repeat? */
    kuint8_t EnableVFlip;	                                /**< Vertical Flip state */

}XAxiVdma_ChannelSetup;

/*!<
 * The XAxiVdma driver instance data.
 */
typedef struct 
{
    kuint32_t BaseAddr;                   			        /**< Memory address for this device */
    kint32_t HasSG;                      			        /**< Whether hardware has SG engine */
    kbool_t IsReady;                    			        /**< Whether driver is initialized */

    kint32_t MaxNumFrames;                			        /**< Number of frames to work on */
    kint32_t HasMm2S;                    			        /**< Whether hw build has read channel */
    kint32_t HasMm2SDRE;                 			        /**< Whether read channel has DRE */
    kint32_t HasS2Mm;                    			        /**< Whether hw build has write channel */
    kint32_t HasS2MmDRE;                 			        /**< Whether write channel has DRE */
    kint32_t EnableVIDParamRead;	    			        /**< Read Enable for video parameters in direct register mode */
    kint32_t UseFsync;       	    				        /**< DMA operations synchronized to Frame Sync */
    kint32_t InternalGenLock;  	    				        /**< Internal Gen Lock */
    
    XAxiVdma_ChannelCallBack ReadCallBack;  		        /**< Call back for read channel */
    XAxiVdma_ChannelCallBack WriteCallBack; 		        /**< Call back for write channel */

    XAxiVdma_Channel ReadChannel;  					        /**< Channel to read from memory */
    XAxiVdma_Channel WriteChannel; 					        /**< Channel to write to memory */
    
    kint32_t AddrWidth;		  						        /**< Address Width */

} XAxiVdma;

/*
 * This driver currently supports 3 frames.
 */
#define DISPLAY_NUM_FRAMES                  1

typedef enum 
{
	NR_XIL_DISPLAY_STOPPED = 0,
	NR_XIL_DISPLAY_RUNNING = 1

} DisplayState;

typedef void (*XVtc_CallBack)(void *CallBackRef, kuint32_t Mask);
typedef void (*XVtc_ErrorCallBack)(void *CallBackRef, kuint32_t ErrorMask);

typedef struct 
{
	kchar_t label[64];                                      /*!< Label describing the resolution */
	kuint32_t width;		                                /*!< Width(horizon) of the active video frame */
	kuint32_t height; 	                                    /*!< Height(vertical) of the active video frame */
	kuint32_t hps; 		                                    /*!< Start time of Horizontal sync pulse, in pixel clocks (active width + H. front porch) */
	kuint32_t hpe; 		                                    /*!< End time of Horizontal sync pulse, in pixel clocks (active width + H. front porch + H. sync width) */
	kuint32_t hmax; 		                                /*!< Total number of pixel clocks per line (active width + H. front porch + H. sync width + H. back porch) */
	kuint32_t hpol; 		                                /*!< hsync pulse polarity */
	kuint32_t vps;		                                    /*!< Start time of Vertical sync pulse, in lines (active height + V. front porch) */
	kuint32_t vpe;		                                    /*!< End time of Vertical sync pulse, in lines (active height + V. front porch + V. sync width) */
	kuint32_t vmax; 		                                /*!< Total number of lines per frame (active height + V. front porch + V. sync width + V. back porch) */
	kuint32_t vpol; 		                                /*!< vsync pulse polarity */
	kfloat_t freq; 	                                        /*!< Pixel Clock frequency */

} VideoMode;

/*!<
 * This typedef contains configuration information for a VTC core.
 * Each VTC device should have a configuration structure associated
 */
typedef struct 
{
	kuint16_t DeviceId;		                                /**< DeviceId is the unique ID of the VTC core */
	kuint32_t BaseAddress;	                                /**< BaseAddress is the physical base address of the core's registers */

} XVtc_Config;

/*!<
 * The XVtc driver instance data. An instance must be allocated for each
 * VTC core in use.
 */
typedef struct 
{
	XVtc_Config Config;	                                    /**< Hardware Configuration */
	kbool_t IsReady;		                                /**< Core and the driver instance are initialized */

	/*!< Interrupt callbacks*/
	XVtc_CallBack FrameSyncCallBack;	                    /**< Callback for Frame Sync interrupt */
	void *FrameSyncRef;		                                /**< To be passed to the Frame Sync interrupt callback */

	XVtc_CallBack LockCallBack;	                            /**< Callback for Signal Lock interrupt */
	void *LockRef;			                                /**< To be passed to the Signal Lock interrupt callback */

	XVtc_CallBack DetectorCallBack;	                        /**< Callback for Detector interrupt */
	void *DetectorRef;		                                /**< To be passed to the Detector interrupt callback */

	XVtc_CallBack GeneratorCallBack;	                    /**< Callback for Generator interrupt */
	void *GeneratorRef;		                                /**< To be passed to the Generator interrupt callback */

	XVtc_ErrorCallBack ErrCallBack;	                        /**< Callback for Error interrupt */
	void *ErrRef;			                                /**< To be passed to the Error interrupt callback */

} XVtc;

typedef struct 
{
	kuint32_t dynClkAddr; 				                    /*!< Physical Base address of the dynclk core */
	XAxiVdma *vdma; 				                        /*!< VDMA driver struct */
	XAxiVdma_DmaSetup vdmaConfig;                           /*!< VDMA channel configuration */
	XVtc vtc; 					                            /*!< VTC driver struct */
	VideoMode vMode; 				                        /*!< Current video mode */
	kuint8_t *framePtr[DISPLAY_NUM_FRAMES];                 /*!< Array of pointers to the frame buffers */
	kuint32_t stride; 					                    /*!< The line stride of the frame buffers, in bytes */
	kfloat_t pxlFreq; 				                        /*!< Frequency of clock currently being generated, maybe not exactly with vMode.freq */
	kuint32_t curFrame; 					                /*!< Current frame being displayed */
	DisplayState state; 			                        /*!< Indicates if the Display is currently running */

} DisplayCtrl;

/*!< The functions */
TARGET_EXT kint32_t zynq7_post_config(void);
TARGET_EXT kchar_t *zynq7_message_info_get(kuint32_t key);

TARGET_EXT void perf_start_clock(void);
TARGET_EXT void perf_disable_clock(void);
TARGET_EXT void perf_reset_clock(void);
TARGET_EXT void perf_reset_and_start_timer(void); 
TARGET_EXT kuint32_t get_number_of_cycles_for_delay(kuint32_t delay);

/*!< L2 Cache */
TARGET_EXT void Xil_L2CacheSync(void);
TARGET_EXT void Xil_L1DCacheFlushLine(kuaddr_t adr);
TARGET_EXT void Xil_L2WriteDebugCtrl(kuint32_t Value);
TARGET_EXT void Xil_L2CacheFlushLine(kuaddr_t adr);
TARGET_EXT void Xil_DCacheFlushRange(kuaddr_t adr, kuint32_t len);
TARGET_EXT void Xil_DCacheInvalidateRange(kuaddr_t adr, kuint32_t len);

/*!< Gpio */
TARGET_EXT XGpioPs_Config *XGpioPs_LookupConfig(kuint16_t DeviceId);
TARGET_EXT kint32_t XGpioPs_CfgInitialize(XGpioPs *sprt_gpio, XGpioPs_Config *sprt_cfg, kuint32_t address);
TARGET_EXT void XGpioPs_GetBankPin(kuint32_t PinNumber, kuint8_t *BankNumber, kuint8_t *PinNumberInBank);
TARGET_EXT kint32_t XGpioPs_GetDirectionPin(XGpioPs *sprt_gpio, kuint32_t Pin);
TARGET_EXT kint32_t XGpioPs_SetDirectionPin(XGpioPs *sprt_gpio, kuint32_t Pin, kuint32_t Direction);
TARGET_EXT kint32_t XGpioPs_GetOutputEnablePin(XGpioPs *sprt_gpio, kuint32_t Pin);
TARGET_EXT kint32_t XGpioPs_SetOutputEnablePin(XGpioPs *sprt_gpio, kuint32_t Pin, kbool_t OpEnable);
TARGET_EXT kint32_t XGpioPs_ReadPin(XGpioPs *sprt_gpio, kuint32_t Pin);
TARGET_EXT kint32_t XGpioPs_WritePin(XGpioPs *sprt_gpio, kuint32_t Pin, kuint32_t Data);

/*!< Timer */
TARGET_EXT XScuTimer_Config *XScuTimer_LookupConfig(kuint16_t DeviceId);
TARGET_EXT kint32_t XScuTimer_CfgInitialize(XScuTimer *sprt_scutimer, XScuTimer_Config *sprt_config, kuint32_t BaseAddr);
TARGET_EXT void XScuTimer_Start(XScuTimer *sprt_scutimer);
TARGET_EXT void XScuTimer_Stop(XScuTimer *sprt_scutimer);
TARGET_EXT void XScuTimer_LoadTimer(XScuTimer *sprt_scutimer, kuint32_t value);
TARGET_EXT void XScuTimer_RestartTimer(XScuTimer *sprt_scutimer);
TARGET_EXT void XScuTimer_EnableAutoReload(XScuTimer *sprt_scutimer);
TARGET_EXT void XScuTimer_DisableAutoReload(XScuTimer *sprt_scutimer);
TARGET_EXT void XScuTimer_EnableInterrupt(XScuTimer *sprt_scutimer);
TARGET_EXT void XScuTimer_DisableInterrupt(XScuTimer *sprt_scutimer);
TARGET_EXT kuint32_t XScuTimer_GetInterruptStatus(XScuTimer *sprt_scutimer);
TARGET_EXT void XScuTimer_ClearInterruptStatus(XScuTimer *sprt_scutimer);

/*!< Uart */
TARGET_EXT XUartPs_Config *XUartPs_LookupConfig(kuint16_t DeviceId);
TARGET_EXT void XUartPs_EnableUart(XUartPs *sprt_uart);
TARGET_EXT void XUartPs_DisableUart(XUartPs *sprt_uart);
TARGET_EXT kint32_t XUartPs_SetBaudRate(XUartPs *sprt_uart, kuint32_t BaudRate);
TARGET_EXT kint32_t XUartPs_CfgInitialize(XUartPs *sprt_uart, XUartPs_Config *sprt_cfg, kuint32_t address);
TARGET_EXT kbool_t XUartPs_IsSendFull(kuint32_t address);
TARGET_EXT kbool_t XUartPs_IsSendEmpty(kuint32_t BaseAddress);
TARGET_EXT kbool_t XUartPs_IsRecvFull(kuint32_t BaseAddress);
TARGET_EXT kbool_t XUartPs_IsRecvEmpty(kuint32_t address);
TARGET_EXT kint32_t XUartPs_SendBuffer(XUartPs *sprt_uart);
TARGET_EXT kint32_t XUartPs_ReceiveBuffer(XUartPs *sprt_uart);
TARGET_EXT kint32_t XUartPs_Send(XUartPs *sprt_uart, kuint8_t *BufferPtr, kuint32_t NumBytes);
TARGET_EXT kint32_t XUartPs_Recv(XUartPs *sprt_uart, kuint8_t *BufferPtr, kuint32_t NumBytes);

/*!< Sd */
TARGET_EXT XSdPs_Config *XSdPs_LookupConfig(kuint16_t DeviceId);
TARGET_EXT kint32_t XSdPs_Change_ClkFreq(XSdPs *sprt_sd, kuint32_t SelFreq);
TARGET_EXT kint32_t XSdPs_CfgInitialize(XSdPs *sprt_sd, XSdPs_Config *sprt_cfg, kuint32_t address);
TARGET_EXT kbool_t XSdPs_IsCardDetected(XSdPs *sprt_sd);
TARGET_EXT kint32_t XSdPs_Change_BusWidth(XSdPs *sprt_sd, kuint32_t width);
TARGET_EXT kint32_t XSdPs_SetBlkSize(XSdPs *sprt_sd, kuint32_t BlkCnt, kuint32_t BlkSize);
TARGET_EXT void XSdPs_SetupADMA2DescTbl(XSdPs *sprt_sd, kuint32_t BlkCnt, const kuint8_t *Buff);

/*!< AxiVdma & Vtc */
TARGET_EXT XAxiVdma_Config *XAxiVdma_LookupConfig(kuint16_t DeviceId);
TARGET_EXT kint32_t XAxiVdma_CfgInitialize(XAxiVdma *sprt_vdma, XAxiVdma_Config *sprt_cfg, kuint32_t EffectiveAddr);
TARGET_EXT XVtc_Config *XVtc_LookupConfig(kuint16_t DeviceId);
TARGET_EXT kint32_t XVtc_CfgInitialize(XVtc *sprt_vtc, XVtc_Config *sprt_vcfg, kuint32_t EffectiveAddr);
TARGET_EXT kint32_t DisplayInitialize(DisplayCtrl *sprt_disp, XAxiVdma *sprt_vdma, kuint16_t vtcId, 
                                kuint32_t dynClkAddr, kuint8_t *framePtr[DISPLAY_NUM_FRAMES], 
                                kuint32_t stride, VideoMode *sprt_vmode);
TARGET_EXT kint32_t DisplayChangeFrameBuffer(DisplayCtrl *sprt_disp, kuint32_t FrameAddr, kusize_t FrameSize);
TARGET_EXT kint32_t DisplayStart(DisplayCtrl *sprt_disp);
TARGET_EXT kint32_t DisplayStop(DisplayCtrl *sprt_dispctrl);

/*!< EmacPs */


#ifdef __cplusplus
    }
#endif

#endif /*!< __ZYNQ7_PERIPH_H */

/*!< end of file */
