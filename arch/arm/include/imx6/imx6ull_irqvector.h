/*
 * IMX6ULL IRQ Vector Defines
 *
 * File Name:   imx6ull_irqvector.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.11.15
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __IMX6ULL_IRQVECTOR_H
#define __IMX6ULL_IRQVECTOR_H

#include <asm/armv7/ca7_gic.h>
#include <common/basic_types.h>
#include <common/error_types.h>
#include <common/generic.h>

/*!< The defines */
typedef enum imx_irq_number 
{
    /*!< Auxiliary constants */
    NR_IMX_NotAvail_IRQn                = -128,             /*!< Not available device specific interrupt */

    /*!< Core interrupts */
    NR_IMX_Software0_IRQn               = 0,                /*!< Cortex-A7 Software Generated Interrupt 0 */
    NR_IMX_Software1_IRQn               = 1,                /*!< Cortex-A7 Software Generated Interrupt 1 */
    NR_IMX_Software2_IRQn               = 2,                /*!< Cortex-A7 Software Generated Interrupt 2 */
    NR_IMX_Software3_IRQn               = 3,                /*!< Cortex-A7 Software Generated Interrupt 3 */
    NR_IMX_Software4_IRQn               = 4,                /*!< Cortex-A7 Software Generated Interrupt 4 */
    NR_IMX_Software5_IRQn               = 5,                /*!< Cortex-A7 Software Generated Interrupt 5 */
    NR_IMX_Software6_IRQn               = 6,                /*!< Cortex-A7 Software Generated Interrupt 6 */
    NR_IMX_Software7_IRQn               = 7,                /*!< Cortex-A7 Software Generated Interrupt 7 */
    NR_IMX_Software8_IRQn               = 8,                /*!< Cortex-A7 Software Generated Interrupt 8 */
    NR_IMX_Software9_IRQn               = 9,                /*!< Cortex-A7 Software Generated Interrupt 9 */
    NR_IMX_Software10_IRQn              = 10,               /*!< Cortex-A7 Software Generated Interrupt 10 */
    NR_IMX_Software11_IRQn              = 11,               /*!< Cortex-A7 Software Generated Interrupt 11 */
    NR_IMX_Software12_IRQn              = 12,               /*!< Cortex-A7 Software Generated Interrupt 12 */
    NR_IMX_Software13_IRQn              = 13,               /*!< Cortex-A7 Software Generated Interrupt 13 */
    NR_IMX_Software14_IRQn              = 14,               /*!< Cortex-A7 Software Generated Interrupt 14 */
    NR_IMX_Software15_IRQn              = 15,               /*!< Cortex-A7 Software Generated Interrupt 15 */
    
    NR_IMX_VirtualMaintenance_IRQn      = 25,               /*!< Cortex-A7 Virtual Maintenance Interrupt */
    NR_IMX_HypervisorTimer_IRQn         = 26,               /*!< Cortex-A7 Hypervisor Timer Interrupt */
    NR_IMX_VirtualTimer_IRQn            = 27,               /*!< Cortex-A7 Virtual Timer Interrupt */
    NR_IMX_LegacyFastInt_IRQn           = 28,               /*!< Cortex-A7 Legacy nFIQ signal Interrupt */
    NR_IMX_SecurePhyTimer_IRQn          = 29,               /*!< Cortex-A7 Secure Physical Timer Interrupt */
    NR_IMX_NonSecurePhyTimer_IRQn       = 30,               /*!< Cortex-A7 Non-secure Physical Timer Interrupt */
    NR_IMX_LegacyIRQ_IRQn               = 31,               /*!< Cortex-A7 Legacy nIRQ Interrupt */

    /*!< Device specific interrupts */
    NR_IMX_IOMUXC_IRQn                  = 32,               /*!< General Purpose Register 1 from IOMUXC. Used to notify cores on exception condition while boot. */
    NR_IMX_DAP_IRQn                     = 33,               /*!< Debug Access Port interrupt request. */
    NR_IMX_SDMA_IRQn                    = 34,               /*!< SDMA interrupt request from all channels. */
    NR_IMX_TSC_IRQn                     = 35,               /*!< TSC interrupt. */
    NR_IMX_SNVS_IRQn                    = 36,               /*!< Logic OR of SNVS_LP and SNVS_HP interrupts. */
    NR_IMX_LCDIF_IRQn                   = 37,               /*!< LCDIF sync interrupt. */
    NR_IMX_RNGB_IRQn                    = 38,               /*!< RNGB interrupt. */
    NR_IMX_CSI_IRQn                     = 39,               /*!< CMOS Sensor Interface interrupt request. */
    NR_IMX_PXP_IRQ0_IRQn                = 40,               /*!< PXP interrupt pxp_irq_0. */
    NR_IMX_SCTR_IRQ0_IRQn               = 41,               /*!< SCTR compare interrupt ipi_int[0]. */
    NR_IMX_SCTR_IRQ1_IRQn               = 42,               /*!< SCTR compare interrupt ipi_int[1]. */
    NR_IMX_WDOG3_IRQn                   = 43,               /*!< WDOG3 timer reset interrupt request. */
    NR_IMX_Reserved44_IRQn              = 44,               /*!< Reserved */
    NR_IMX_APBH_IRQn                    = 45,               /*!< DMA Logical OR of APBH DMA channels 0-3 completion and error interrupts. */
    NR_IMX_WEIM_IRQn                    = 46,               /*!< WEIM interrupt request. */
    NR_IMX_RAWNAND_BCH_IRQn             = 47,               /*!< BCH operation complete interrupt. */
    NR_IMX_RAWNAND_GPMI_IRQn            = 48,               /*!< GPMI operation timeout error interrupt. */
    NR_IMX_UART6_IRQn                   = 49,               /*!< UART6 interrupt request. */
    NR_IMX_PXP_IRQ1_IRQn                = 50,               /*!< PXP interrupt pxp_irq_1. */
    NR_IMX_SNVS_Consolidated_IRQn       = 51,               /*!< SNVS consolidated interrupt. */
    NR_IMX_SNVS_Security_IRQn           = 52,               /*!< SNVS security interrupt. */
    NR_IMX_CSU_IRQn                     = 53,               /*!< CSU interrupt request 1. Indicates to the processor that one or more alarm inputs were asserted. */
    NR_IMX_USDHC1_IRQn                  = 54,               /*!< USDHC1 (Enhanced SDHC) interrupt request. */
    NR_IMX_USDHC2_IRQn                  = 55,               /*!< USDHC2 (Enhanced SDHC) interrupt request. */
    NR_IMX_SAI3_RX_IRQn                 = 56,               /*!< SAI3 interrupt ipi_int_sai_rx. */
    NR_IMX_SAI3_TX_IRQn                 = 57,               /*!< SAI3 interrupt ipi_int_sai_tx. */
    NR_IMX_UART1_IRQn                   = 58,               /*!< UART1 interrupt request. */
    NR_IMX_UART2_IRQn                   = 59,               /*!< UART2 interrupt request. */
    NR_IMX_UART3_IRQn                   = 60,               /*!< UART3 interrupt request. */
    NR_IMX_UART4_IRQn                   = 61,               /*!< UART4 interrupt request. */
    NR_IMX_UART5_IRQn                   = 62,               /*!< UART5 interrupt request. */
    NR_IMX_eCSPI1_IRQn                  = 63,               /*!< eCSPI1 interrupt request. */
    NR_IMX_eCSPI2_IRQn                  = 64,               /*!< eCSPI2 interrupt request. */
    NR_IMX_eCSPI3_IRQn                  = 65,               /*!< eCSPI3 interrupt request. */
    NR_IMX_eCSPI4_IRQn                  = 66,               /*!< eCSPI4 interrupt request. */
    NR_IMX_I2C4_IRQn                    = 67,               /*!< I2C4 interrupt request. */
    NR_IMX_I2C1_IRQn                    = 68,               /*!< I2C1 interrupt request. */
    NR_IMX_I2C2_IRQn                    = 69,               /*!< I2C2 interrupt request. */
    NR_IMX_I2C3_IRQn                    = 70,               /*!< I2C3 interrupt request. */
    NR_IMX_UART7_IRQn                   = 71,               /*!< UART-7 ORed interrupt. */
    NR_IMX_UART8_IRQn                   = 72,               /*!< UART-8 ORed interrupt. */
    NR_IMX_Reserved73_IRQn              = 73,               /*!< Reserved */
    NR_IMX_USB_OTG2_IRQn                = 74,               /*!< USBO2 USB OTG2 */
    NR_IMX_USB_OTG1_IRQn                = 75,               /*!< USBO2 USB OTG1 */
    NR_IMX_USB_PHY1_IRQn                = 76,               /*!< UTMI0 interrupt request. */
    NR_IMX_USB_PHY2_IRQn                = 77,               /*!< UTMI1 interrupt request. */
    NR_IMX_DCP_IRQ_IRQn                 = 78,               /*!< DCP interrupt request dcp_irq. */
    NR_IMX_DCP_VMI_IRQ_IRQn             = 79,               /*!< DCP interrupt request dcp_vmi_irq. */
    NR_IMX_DCP_SEC_IRQ_IRQn             = 80,               /*!< DCP interrupt request secure_irq. */
    NR_IMX_TEMPMON_IRQn                 = 81,               /*!< Temperature Monitor Temperature Sensor (temperature greater than threshold) interrupt request. */
    NR_IMX_ASRC_IRQn                    = 82,               /*!< ASRC interrupt request. */
    NR_IMX_ESAI_IRQn                    = 83,               /*!< ESAI interrupt request. */
    NR_IMX_SPDIF_IRQn                   = 84,               /*!< SPDIF interrupt. */
    NR_IMX_Reserved85_IRQn              = 85,               /*!< Reserved */
    NR_IMX_PMU_IRQ1_IRQn                = 86,               /*!< Brown-out event on either the 1.1, 2.5 or 3.0 regulators. */
    NR_IMX_GPT1_IRQn                    = 87,               /*!< Logical OR of GPT1 rollover interrupt line, input capture 1 and 2 lines, output compare 1, 2, and 3 interrupt lines. */
    NR_IMX_EPIT1_IRQn                   = 88,               /*!< EPIT1 output compare interrupt. */
    NR_IMX_EPIT2_IRQn                   = 89,               /*!< EPIT2 output compare interrupt. */
    NR_IMX_GPIO1_INT7_IRQn              = 90,               /*!< INT7 interrupt request. */
    NR_IMX_GPIO1_INT6_IRQn              = 91,               /*!< INT6 interrupt request. */
    NR_IMX_GPIO1_INT5_IRQn              = 92,               /*!< INT5 interrupt request. */
    NR_IMX_GPIO1_INT4_IRQn              = 93,               /*!< INT4 interrupt request. */
    NR_IMX_GPIO1_INT3_IRQn              = 94,               /*!< INT3 interrupt request. */
    NR_IMX_GPIO1_INT2_IRQn              = 95,               /*!< INT2 interrupt request. */
    NR_IMX_GPIO1_INT1_IRQn              = 96,               /*!< INT1 interrupt request. */
    NR_IMX_GPIO1_INT0_IRQn              = 97,               /*!< INT0 interrupt request. */
    NR_IMX_GPIO1_Combined_0_15_IRQn     = 98,               /*!< Combined interrupt indication for GPIO1 signals 0 - 15. */
    NR_IMX_GPIO1_Combined_16_31_IRQn    = 99,               /*!< Combined interrupt indication for GPIO1 signals 16 - 31. */
    NR_IMX_GPIO2_Combined_0_15_IRQn     = 100,              /*!< Combined interrupt indication for GPIO2 signals 0 - 15. */
    NR_IMX_GPIO2_Combined_16_31_IRQn    = 101,              /*!< Combined interrupt indication for GPIO2 signals 16 - 31. */
    NR_IMX_GPIO3_Combined_0_15_IRQn     = 102,              /*!< Combined interrupt indication for GPIO3 signals 0 - 15. */
    NR_IMX_GPIO3_Combined_16_31_IRQn    = 103,              /*!< Combined interrupt indication for GPIO3 signals 16 - 31. */
    NR_IMX_GPIO4_Combined_0_15_IRQn     = 104,              /*!< Combined interrupt indication for GPIO4 signals 0 - 15. */
    NR_IMX_GPIO4_Combined_16_31_IRQn    = 105,              /*!< Combined interrupt indication for GPIO4 signals 16 - 31. */
    NR_IMX_GPIO5_Combined_0_15_IRQn     = 106,              /*!< Combined interrupt indication for GPIO5 signals 0 - 15. */
    NR_IMX_GPIO5_Combined_16_31_IRQn    = 107,              /*!< Combined interrupt indication for GPIO5 signals 16 - 31. */
    NR_IMX_Reserved108_IRQn             = 108,              /*!< Reserved */
    NR_IMX_Reserved109_IRQn             = 109,              /*!< Reserved */
    NR_IMX_Reserved110_IRQn             = 110,              /*!< Reserved */
    NR_IMX_Reserved111_IRQn             = 111,              /*!< Reserved */
    NR_IMX_WDOG1_IRQn                   = 112,              /*!< WDOG1 timer reset interrupt request. */
    NR_IMX_WDOG2_IRQn                   = 113,              /*!< WDOG2 timer reset interrupt request. */
    NR_IMX_KPP_IRQn                     = 114,              /*!< Key Pad interrupt request. */
    NR_IMX_PWM1_IRQn                    = 115,              /*!< hasRegInstance(`PWM1`)?`Cumulative interrupt line for PWM1. Logical OR of rollover, compare, and FIFO waterlevel crossing interrupts.`:`Reserved`) */
    NR_IMX_PWM2_IRQn                    = 116,              /*!< hasRegInstance(`PWM2`)?`Cumulative interrupt line for PWM2. Logical OR of rollover, compare, and FIFO waterlevel crossing interrupts.`:`Reserved`) */
    NR_IMX_PWM3_IRQn                    = 117,              /*!< hasRegInstance(`PWM3`)?`Cumulative interrupt line for PWM3. Logical OR of rollover, compare, and FIFO waterlevel crossing interrupts.`:`Reserved`) */
    NR_IMX_PWM4_IRQn                    = 118,              /*!< hasRegInstance(`PWM4`)?`Cumulative interrupt line for PWM4. Logical OR of rollover, compare, and FIFO waterlevel crossing interrupts.`:`Reserved`) */
    NR_IMX_CCM_IRQ1_IRQn                = 119,              /*!< CCM interrupt request ipi_int_1. */
    NR_IMX_CCM_IRQ2_IRQn                = 120,              /*!< CCM interrupt request ipi_int_2. */
    NR_IMX_GPC_IRQn                     = 121,              /*!< GPC interrupt request 1. */
    NR_IMX_Reserved122_IRQn             = 122,              /*!< Reserved */
    NR_IMX_SRC_IRQn                     = 123,              /*!< SRC interrupt request src_ipi_int_1. */
    NR_IMX_Reserved124_IRQn             = 124,              /*!< Reserved */
    NR_IMX_Reserved125_IRQn             = 125,              /*!< Reserved */
    NR_IMX_CPU_PerformanceUnit_IRQn     = 126,              /*!< Performance Unit interrupt ~ipi_pmu_irq_b. */
    NR_IMX_CPU_CTI_Trigger_IRQn         = 127,              /*!< CTI trigger outputs interrupt ~ipi_cti_irq_b. */
    NR_IMX_SRC_Combined_IRQn            = 128,              /*!< Combined CPU wdog interrupts (4x) out of SRC. */
    NR_IMX_SAI1_IRQn                    = 129,              /*!< SAI1 interrupt request. */
    NR_IMX_SAI2_IRQn                    = 130,              /*!< SAI2 interrupt request. */
    NR_IMX_Reserved131_IRQn             = 131,              /*!< Reserved */
    NR_IMX_ADC1_IRQn                    = 132,              /*!< ADC1 interrupt request. */
    NR_IMX_ADC_5HC_IRQn                 = 133,              /*!< ADC_5HC interrupt request. */
    NR_IMX_Reserved134_IRQn             = 134,              /*!< Reserved */
    NR_IMX_Reserved135_IRQn             = 135,              /*!< Reserved */
    NR_IMX_SJC_IRQn                     = 136,              /*!< SJC interrupt from General Purpose register. */
    NR_IMX_CAAM_Job_Ring0_IRQn          = 137,              /*!< CAAM job ring 0 interrupt ipi_caam_irq0. */
    NR_IMX_CAAM_Job_Ring1_IRQn          = 138,              /*!< CAAM job ring 1 interrupt ipi_caam_irq1. */
    NR_IMX_QSPI_IRQn                    = 139,              /*!< QSPI1 interrupt request ipi_int_ored. */
    NR_IMX_TZASC_IRQn                   = 140,              /*!< TZASC (PL380) interrupt request. */
    NR_IMX_GPT2_IRQn                    = 141,              /*!< Logical OR of GPT2 rollover interrupt line, input capture 1 and 2 lines, output compare 1, 2 and 3 interrupt lines. */
    NR_IMX_CAN1_IRQn                    = 142,              /*!< Combined interrupt of ini_int_busoff,ini_int_error,ipi_int_mbor,ipi_int_txwarning and ipi_int_waken */
    NR_IMX_CAN2_IRQn                    = 143,              /*!< Combined interrupt of ini_int_busoff,ini_int_error,ipi_int_mbor,ipi_int_txwarning and ipi_int_waken */
    NR_IMX_Reserved144_IRQn             = 144,              /*!< Reserved */
    NR_IMX_Reserved145_IRQn             = 145,              /*!< Reserved */
    NR_IMX_PWM5_IRQn                    = 146,              /*!< Cumulative interrupt line. OR of Rollover Interrupt line, Compare Interrupt line and FIFO Waterlevel crossing interrupt line */
    NR_IMX_PWM6_IRQn                    = 147,              /*!< Cumulative interrupt line. OR of Rollover Interrupt line, Compare Interrupt line and FIFO Waterlevel crossing interrupt line */
    NR_IMX_PWM7_IRQn                    = 148,              /*!< Cumulative interrupt line. OR of Rollover Interrupt line, Compare Interrupt line and FIFO Waterlevel crossing interrupt line */
    NR_IMX_PWM8_IRQn                    = 149,              /*!< Cumulative interrupt line. OR of Rollover Interrupt line, Compare Interrupt line and FIFO Waterlevel crossing interrupt line */
    NR_IMX_ENET1_IRQn                   = 150,              /*!< ENET1 interrupt */
    NR_IMX_ENET1_1588_IRQn              = 151,              /*!< ENET1 1588 Timer interrupt [synchronous] request. */
    NR_IMX_ENET2_IRQn                   = 152,              /*!< ENET2 interrupt */
    NR_IMX_ENET2_1588_IRQn              = 153,              /*!< MAC 0 1588 Timer interrupt [synchronous] request. */
    NR_IMX_Reserved154_IRQn             = 154,              /*!< Reserved */
    NR_IMX_Reserved155_IRQn             = 155,              /*!< Reserved */
    NR_IMX_Reserved156_IRQn             = 156,              /*!< Reserved */
    NR_IMX_Reserved157_IRQn             = 157,              /*!< Reserved */
    NR_IMX_Reserved158_IRQn             = 158,              /*!< Reserved */
    NR_IMX_PMU_IRQ2_IRQn                = 159,              /*!< Brown-out event on either core, gpu or soc regulators. */

    NR_IMX_IRQn_Max,                                        /*!< IRQ number count */

} srt_imx_irq_number_t;


#endif /* __IMX6ULL_IRQVECTOR_H */
