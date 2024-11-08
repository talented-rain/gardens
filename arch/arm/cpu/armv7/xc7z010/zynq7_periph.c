/*
 * ZYNQ7 Peripheral APIs
 *
 * File Name:   zynq7_periph.c
 * Author:      Yang Yujun (Copy from "Xilinx SDK")
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.06.20
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

/*!< The globals */
static XGpioPs_Config sgrt_xgpio_ps_config_table[XPAR_XGPIOPS_NUM_INSTANCES] =
{
    {
        XPAR_PS7_GPIO_0_DEVICE_ID,
        XPAR_PS7_GPIO_0_BASEADDR
    }
};

static XScuTimer_Config XScuTimer_ConfigTable[XPAR_XSCUTIMER_NUM_INSTANCES] =
{
    {
        XPAR_PS7_SCUTIMER_0_DEVICE_ID,
        XPAR_PS7_SCUTIMER_0_BASEADDR
    }
};

static XUartPs_Config sgrt_xuart_ps_config_table[XPAR_XUARTPS_NUM_INSTANCES] =
{
    {
        XPAR_PS7_UART_0_DEVICE_ID,
        XPAR_PS7_UART_0_BASEADDR,
        XPAR_PS7_UART_0_UART_CLK_FREQ_HZ,
        XPAR_PS7_UART_0_HAS_MODEM
    }
};

static XSdPs_Config sgrt_xsd_ps_config_table[XPAR_XSDPS_NUM_INSTANCES] =
{
    {
        XPAR_PS7_SD_0_DEVICE_ID,
        XPAR_PS7_SD_0_BASEADDR,
        XPAR_PS7_SD_0_SDIO_CLK_FREQ_HZ,
        XPAR_PS7_SD_0_HAS_CD,
        XPAR_PS7_SD_0_HAS_WP,
        XPAR_PS7_SD_0_BUS_WIDTH,
        XPAR_PS7_SD_0_MIO_BANK,
        XPAR_PS7_SD_0_HAS_EMIO,
        XPAR_PS7_SD_0_IS_CACHE_COHERENT
    }
};

/*!< API functions */
void Xil_L2CacheSync(void)
{
    XSdPs_WriteReg(XPS_L2CC_BASEADDR, XPS_L2CC_CACHE_SYNC_OFFSET, 0x0U);
}

void Xil_L1DCacheFlushLine(kuaddr_t adr)
{
    kuint32_t cache_val = 0U;

    mrt_set_cp15(XREG_CP15_CACHE_SIZE_SEL, cache_val);

    cache_val = (kuint32_t)(adr & (~0x1FU));
    mrt_set_cp15(XREG_CP15_CLEAN_INVAL_DC_LINE_MVA_POC, cache_val);

    /* Wait for L1 flush to complete */
    mrt_dsb();
}

void Xil_L2WriteDebugCtrl(kuint32_t Value)
{
    XSdPs_WriteReg(XPS_L2CC_BASEADDR, XPS_L2CC_DEBUG_CTRL_OFFSET, Value);
}

void Xil_L2CacheFlushLine(kuaddr_t adr)
{
    XSdPs_WriteReg(XPS_L2CC_BASEADDR, XPS_L2CC_CACHE_CLEAN_PA_OFFSET, adr);
    XSdPs_WriteReg(XPS_L2CC_BASEADDR, XPS_L2CC_CACHE_INVLD_PA_OFFSET, adr);

    /* synchronize the processor */
    mrt_dsb();
}

void Xil_DCacheFlushRange(kuaddr_t adr, kuint32_t len)
{
    kuint32_t LocalAddr = adr;
    const kuint32_t cacheline = 32U;
    kuint32_t end;
    kuint32_t currmask;
    volatile kuint32_t *L2CCOffset;

    L2CCOffset = (volatile kuint32_t *)(XPS_L2CC_BASEADDR + XPS_L2CC_CACHE_INV_CLN_PA_OFFSET);

    currmask = __get_cpsr();
    __set_cpsr(currmask | CPSR_BIT_I | CPSR_BIT_F);

    if (len) 
    {
        /* Back the starting address up to the start of a cache line
         * perform cache operations until adr+len
         */
        end = LocalAddr + len;
        LocalAddr &= ~(cacheline - 1U);

        while (LocalAddr < end) 
        {
            /* Flush L1 Data cache line */
            mrt_set_cp15(XREG_CP15_CLEAN_INVAL_DC_LINE_MVA_POC, LocalAddr);

        #if (!defined(CONFIG_USE_AMP) || !CONFIG_USE_AMP)
            /* Flush L2 cache line */
            *L2CCOffset = LocalAddr;
           
            /*!< L2 Cache Sync */
            Xil_L2CacheSync();
        #endif
            LocalAddr += cacheline;
        }
    }

    mrt_dsb();
    __set_cpsr(currmask);
}

void Xil_DCacheInvalidateRange(kuaddr_t adr, kuint32_t len)
{
    const kuint32_t cacheline = 32U;
    kuint32_t end;
    kuint32_t tempadr = adr;
    kuint32_t tempend;
    kuint32_t currmask;
    volatile kuint32_t *L2CCOffset;
    kuint32_t cache_val = 0U;

    L2CCOffset = (volatile kuint32_t *)(XPS_L2CC_BASEADDR + XPS_L2CC_CACHE_INVLD_PA_OFFSET);

    currmask = __get_cpsr();
    __set_cpsr(currmask | CPSR_BIT_F);

    if (len) 
    {
        end = tempadr + len;
        tempend = end;

        /* Select L1 Data cache in CSSR */
        mrt_set_cp15(XREG_CP15_CACHE_SIZE_SEL, cache_val);

        if (tempadr & (cacheline - 1U))
        {
            tempadr &= (~(cacheline - 1U));
            Xil_L1DCacheFlushLine(tempadr);

        #if (!defined(CONFIG_USE_AMP) || !CONFIG_USE_AMP)
            /* Disable Write-back and line fills */
            Xil_L2WriteDebugCtrl(0x3U);
            Xil_L2CacheFlushLine(tempadr);
            /* Enable Write-back and line fills */
            Xil_L2WriteDebugCtrl(0x0U);
            Xil_L2CacheSync();
        #endif
            tempadr += cacheline;
        }

        if (tempend & (cacheline - 1U)) 
        {
            tempend &= (~(cacheline - 1U));
            Xil_L1DCacheFlushLine(tempend);

        #if (!defined(CONFIG_USE_AMP) || !CONFIG_USE_AMP)
            /* Disable Write-back and line fills */
            Xil_L2WriteDebugCtrl(0x3U);
            Xil_L2CacheFlushLine(tempend);
            /* Enable Write-back and line fills */
            Xil_L2WriteDebugCtrl(0x0U);
            Xil_L2CacheSync();
        #endif
        }

        while (tempadr < tempend) 
        {
        #if (!defined(CONFIG_USE_AMP) || !CONFIG_USE_AMP)
            /* Invalidate L2 cache line */
            *L2CCOffset = tempadr;
            Xil_L2CacheSync();
        #endif

            /* Invalidate L1 Data cache line */
            mrt_set_cp15(XREG_CP15_INVAL_DC_LINE_MVA_POC, tempadr);
            tempadr += cacheline;
        }
    }

    mrt_dsb();
    __set_cpsr(currmask);
}

/*!
 * @brief   get gpio_config structure
 * @param   DeviceId
 * @retval  XGpioPs_Config
 * @note    none
 */
XGpioPs_Config *XGpioPs_LookupConfig(kuint16_t DeviceId)
{
    XGpioPs_Config *sprt_cfg = &sgrt_xgpio_ps_config_table[0];
    kuint32_t idx;

    for (idx = 0U; idx < XPAR_XGPIOPS_NUM_INSTANCES; idx++) 
    {
        if (sprt_cfg[idx].DeviceId == DeviceId) 
            return sprt_cfg;
    }

    return mrt_nullptr;
}

/*!
 * @brief   fill sprt_gpio
 * @param   sprt_gpio, sprt_cfg
 * @param   address: base address of gpio bank
 * @retval  errno
 * @note    none
 */
kint32_t XGpioPs_CfgInitialize(XGpioPs *sprt_gpio, XGpioPs_Config *sprt_cfg, kuint32_t address)
{
    kuint32_t idx, RegOffset;

    /*!<
     * Set some default values for instance data, don't indicate the device
     * is ready to use until everything has been initialized successfully.
     */
    sprt_gpio->IsReady = false;
    sprt_gpio->sgrt_cfg.BaseAddr = address;
    sprt_gpio->sgrt_cfg.DeviceId = sprt_cfg->DeviceId;

    /*!< Initialize the Bank data based on platform */
    /*!<
     * Max pins in the GPIO device
     * 0 - 31,  Bank 0
     * 32 - 53, Bank 1
     * 54 - 85, Bank 2
     * 86 - 117, Bank 3
     */
    sprt_gpio->MaxPinNum = XGPIOPS_DEVICE_MAX_PIN_NUM;
    sprt_gpio->MaxBanks = XGPIOPS_MAX_BANKS;

    /*!<
     * By default, interrupts are not masked in GPIO. Disable
     * interrupts for all pins in all the 4 banks.
     */
    for (idx = 0U; idx < sprt_gpio->MaxBanks; idx++) 
    {
        /*!<
         * XGPIOPS_REG_MASK_OFFSET: the lenth of one bank
         * XGPIOPS_INTDIS_OFFSET: the RegOffset of intr register
         */
        RegOffset = idx * XGPIOPS_REG_MASK_OFFSET + XGPIOPS_INTDIS_OFFSET;
        XSdPs_WriteReg(sprt_gpio->sgrt_cfg.BaseAddr, RegOffset, 0xffffffffU);
    }

    /*!< Indicate the component is now ready to use. */
    sprt_gpio->IsReady = true;

    return ER_NORMAL;
}

/*!
 * @brief   get bank number and pin number in bank
 * @param   PinNumber: 0 ~ 117
 * @param   BankNumber: 0 ~ 3
 * @param   PinNumberInBank: 0 ~ 31
 * @retval  none
 * @note    convert PinNumber to PinNumberInBank
 */
void XGpioPs_GetBankPin(kuint32_t PinNumber, kuint8_t *BankNumber, kuint8_t *PinNumberInBank)
{
    kuint32_t XGpioPsPinTable[4] = 
    {
        XGPIOPS_BANK0_MAX_PIN + 1,
        XGPIOPS_BANK1_MAX_PIN + 1,
        XGPIOPS_BANK2_MAX_PIN + 1,
        XGPIOPS_BANK3_MAX_PIN + 1
    };

    *BankNumber = 0U;
    while (*BankNumber < 4U) 
    {
        if (PinNumber < XGpioPsPinTable[*BankNumber])
            break;
        (*BankNumber)++;
    }

    /*!<
     * PinNumber:   
     *  for 0 ~ 31, PinNumberInBank = PinNumber;
     *  for 32 ~ 117, such as PinNumber = 35 ===> PinNumberInBank = 3
     */
    if (*BankNumber == 0U)
        *PinNumberInBank = PinNumber;
    else
        *PinNumberInBank = (kuint8_t)(PinNumber % XGpioPsPinTable[*BankNumber - 1]);
}

/*!
 * @brief   get I/O direction of the pin
 * @param   sprt_gpio
 * @param   Pin: 0 ~ 117
 * @retval  errno
 * @note    none
 */
kint32_t XGpioPs_GetDirectionPin(XGpioPs *sprt_gpio, kuint32_t Pin)
{
    kuint8_t Bank;
    kuint8_t PinNumber;
    kuint32_t RegOffset, Value;

    if ((!sprt_gpio) ||
        (!sprt_gpio->IsReady) ||
        (Pin >= sprt_gpio->MaxPinNum))
        return -ER_FAULT;

    /*!< Get the Bank number and Pin number within the bank. */
    XGpioPs_GetBankPin(Pin, &Bank, &PinNumber);

    RegOffset = ((kuint32_t)(Bank) * XGPIOPS_REG_MASK_OFFSET) + XGPIOPS_DIRM_OFFSET;
    Value = XSdPs_ReadReg(sprt_gpio->sgrt_cfg.BaseAddr, RegOffset);

    return !!(Value & mrt_bit(PinNumber));
}

/*!
 * @brief   set I/O direction of the pin
 * @param   sprt_gpio
 * @param   Pin: 0 ~ 117
 * @param   Direction: 0, input; 1, output
 * @retval  errno
 * @note    none
 */
kint32_t XGpioPs_SetDirectionPin(XGpioPs *sprt_gpio, kuint32_t Pin, kuint32_t Direction)
{
    kuint8_t Bank;
    kuint8_t PinNumber;
    kuint32_t DirModeReg, RegOffset;

    if ((!sprt_gpio) ||
        (!sprt_gpio->IsReady) ||
        (Pin >= sprt_gpio->MaxPinNum) ||
        (Direction > 1U))
        return -ER_FAULT;

    /*!< Get the Bank number and Pin number within the bank. */
    XGpioPs_GetBankPin(Pin, &Bank, &PinNumber);

    RegOffset = ((kuint32_t)(Bank) * XGPIOPS_REG_MASK_OFFSET) + XGPIOPS_DIRM_OFFSET;
    DirModeReg = XSdPs_ReadReg(sprt_gpio->sgrt_cfg.BaseAddr, RegOffset);

    /*!< Output Direction */
    if (Direction)
        mrt_setbitl(mrt_bit(PinNumber), &DirModeReg);
    
    /*!< Input Direction */
    else 
        mrt_clrbitl(mrt_bit(PinNumber), &DirModeReg);

    XSdPs_WriteReg(sprt_gpio->sgrt_cfg.BaseAddr, RegOffset, DirModeReg);

    return ER_NORMAL;
}

/*!
 * @brief   get output gain of the pin
 * @param   sprt_gpio
 * @param   Pin: 0 ~ 117
 * @retval  errno
 * @note    none
 */
kint32_t XGpioPs_GetOutputEnablePin(XGpioPs *sprt_gpio, kuint32_t Pin)
{
    kuint8_t Bank;
    kuint8_t PinNumber;
    kuint32_t RegOffset, Value;

    if ((!sprt_gpio) ||
        (!sprt_gpio->IsReady) ||
        (Pin >= sprt_gpio->MaxPinNum))
        return -ER_FAULT;

    /*!< Get the Bank number and Pin number within the bank. */
    XGpioPs_GetBankPin(Pin, &Bank, &PinNumber);

    RegOffset = ((kuint32_t)(Bank) * XGPIOPS_REG_MASK_OFFSET) + XGPIOPS_OUTEN_OFFSET;
    Value = XSdPs_ReadReg(sprt_gpio->sgrt_cfg.BaseAddr, RegOffset);

    return !!(Value & mrt_bit(PinNumber));
}

/*!
 * @brief   enable output gain of the pin
 * @param   sprt_gpio
 * @param   Pin: 0 ~ 117
 * @param   OpEnable: 0, disable; 1, enable
 * @retval  errno
 * @note    none
 */
kint32_t XGpioPs_SetOutputEnablePin(XGpioPs *sprt_gpio, kuint32_t Pin, kbool_t OpEnable)
{
    kuint8_t Bank;
    kuint8_t PinNumber;
    kuint32_t OpEnableReg, RegOffset;

    if ((!sprt_gpio) ||
        (!sprt_gpio->IsReady) ||
        (Pin >= sprt_gpio->MaxPinNum))
        return -ER_FAULT;

    /*!< Get the Bank number and Pin number within the bank. */
    XGpioPs_GetBankPin(Pin, &Bank, &PinNumber);

    RegOffset = ((kuint32_t)(Bank) * XGPIOPS_REG_MASK_OFFSET) + XGPIOPS_OUTEN_OFFSET;
    OpEnableReg = XSdPs_ReadReg(sprt_gpio->sgrt_cfg.BaseAddr, RegOffset);

    /*!< Enable Output Enable */
    if (OpEnable)
        mrt_setbitl(mrt_bit(PinNumber), &OpEnableReg);

    /*!< Disable Output Enable */
    else
        mrt_clrbitl(mrt_bit(PinNumber), &OpEnableReg);

    XSdPs_WriteReg(sprt_gpio->sgrt_cfg.BaseAddr, RegOffset, OpEnableReg);

    return ER_NORMAL;
}

/*!
 * @brief   read value from the pin (DATA_RO)
 * @param   sprt_gpio
 * @param   Pin: 0 ~ 117
 * @retval  errno
 * @note    pin should be configured to input direction
 */
kint32_t XGpioPs_ReadPin(XGpioPs *sprt_gpio, kuint32_t Pin)
{
    kuint32_t RegOffset, Value;
    kuint8_t Bank;
    kuint8_t PinNumber;

    if ((!sprt_gpio) ||
        (!sprt_gpio->IsReady) ||
        (Pin >= sprt_gpio->MaxPinNum))
        return -ER_FAULT;

    /*!< Get the Bank number and Pin number within the bank. */
    XGpioPs_GetBankPin(Pin, &Bank, &PinNumber);

    RegOffset = (kuint32_t)(Bank) * XGPIOPS_DATA_BANK_OFFSET + XGPIOPS_DATA_RO_OFFSET;
    Value = XSdPs_ReadReg(sprt_gpio->sgrt_cfg.BaseAddr, RegOffset);
    
    return !!(Value & mrt_bit(PinNumber));
}

/*!
 * @brief   write value to the pin (DATA_LSW, DATA_MSW)
 * @param   sprt_gpio
 * @param   Pin: 0 ~ 117
 * @param   Data: 0, low level; 1, high level
 * @retval  errno
 * @note    pin should be configured to output direction
 */
kint32_t XGpioPs_WritePin(XGpioPs *sprt_gpio, kuint32_t Pin, kuint32_t Data)
{
    kuint32_t RegOffset;
    kuint32_t Value, Mask;
    kuint8_t Bank;
    kuint8_t PinNumber;
    kuint32_t DataVar = !!Data;

    if ((!sprt_gpio) ||
        (!sprt_gpio->IsReady) ||
        (Pin >= sprt_gpio->MaxPinNum))
        return -ER_FAULT;

    /*!< Get the Bank number and Pin number within the bank. */
    XGpioPs_GetBankPin(Pin, &Bank, &PinNumber);

    if (PinNumber < 16U) 
        RegOffset = XGPIOPS_DATA_LSW_OFFSET;
    else
    {
        /*!< There are only 16 data bits in bit maskable register. */
        PinNumber -= 16;
        RegOffset = XGPIOPS_DATA_MSW_OFFSET;
    }

    RegOffset += ((kuint32_t)(Bank) * XGPIOPS_DATA_MASK_OFFSET);

    /*!<
     * Get the 32 bit value to be written to the Mask/Data register where
     * the upper 16 bits is the mask and lower 16 bits is the data.
     */   
    Value = mrt_bit_nr(DataVar, PinNumber) | 0xffff0000U;
    Mask = ~mrt_bit(PinNumber + 16U);
    XSdPs_WriteReg(sprt_gpio->sgrt_cfg.BaseAddr, RegOffset, mrt_mask(Value, Mask));

    return ER_NORMAL;
}

/*!
 * @brief   get scu_timer config structure
 * @param   DeviceId
 * @retval  XGpioPs_Config
 * @note    none
 */
XScuTimer_Config *XScuTimer_LookupConfig(kuint16_t DeviceId)
{
    XScuTimer_Config *sprt_cfg = &XScuTimer_ConfigTable[0];
    kuint32_t idx;

    for (idx = 0U; idx < XPAR_XUARTPS_NUM_INSTANCES; idx++) 
    {
        if (sprt_cfg[idx].DeviceId == DeviceId) 
            return sprt_cfg;
    }

    return mrt_nullptr;
}

/*!
 * @brief   ScuTimer Initialization
 * @param   BaseAddr: address of timer register
 * @retval  error code
 * @note    none
 */
kint32_t XScuTimer_CfgInitialize(XScuTimer *sprt_scutimer,
                        XScuTimer_Config *sprt_cfg, kuint32_t BaseAddr)
{
    if ((!sprt_scutimer) || 
        (!sprt_cfg))
        return -ER_FAULT;

    /*!<
     * If the device is started, disallow the initialize and return a
     * status indicating it is started. This allows the user to stop the
     * device and reinitialize, but prevents a user from inadvertently
     * initializing.
     */
    if (!sprt_scutimer->IsStarted)
    {
        /*!< Copy configuration into the instance structure. */
        sprt_scutimer->Config.DeviceId = sprt_cfg->DeviceId;

        /*!<
         * Save the base address pointer such that the registers of the block
         * can be accessed and indicate it has not been started yet.
         */
        sprt_scutimer->Config.BaseAddr = BaseAddr;
        sprt_scutimer->IsStarted = false;

        /*!< Indicate the instance is ready to use, successfully initialized. */
        sprt_scutimer->IsReady = true;
    }

    return 0;
}

/*!
 * @brief   ScuTimer start
 * @param   sprt_scutimer
 * @retval  none
 * @note    none
 */
void XScuTimer_Start(XScuTimer *sprt_scutimer)
{
    kint32_t Register;

    if ((!sprt_scutimer) ||
        (!sprt_scutimer->IsReady))
        return;

    /*!< Read the contents of the Control register. */
    Register = mrt_readl(sprt_scutimer->Config.BaseAddr + XSCUTIMER_CONTROL_OFFSET);

    /*!< Set the 'timer enable' bit in the register. */
    Register |= XSCUTIMER_CONTROL_ENABLE_MASK;

    /*!< Update the Control register with the new value. */
    mrt_writel(Register, sprt_scutimer->Config.BaseAddr + XSCUTIMER_CONTROL_OFFSET);

    /*!< Indicate that the device is started. */
    sprt_scutimer->IsStarted = true;
}

/*!
 * @brief   ScuTimer stop
 * @param   sprt_scutimer
 * @retval  none
 * @note    none
 */
void XScuTimer_Stop(XScuTimer *sprt_scutimer)
{
    kint32_t Register;

    if ((!sprt_scutimer) ||
        (!sprt_scutimer->IsReady))
        return;

    /*!< Read the contents of the Control register. */
    Register = mrt_readl(sprt_scutimer->Config.BaseAddr + XSCUTIMER_CONTROL_OFFSET);

    /*!< Set the 'timer enable' bit in the register. */
    Register &= (kuint32_t)(~XSCUTIMER_CONTROL_ENABLE_MASK);

    /*!< Update the Control register with the new value. */
    mrt_writel(Register, sprt_scutimer->Config.BaseAddr + XSCUTIMER_CONTROL_OFFSET);

    /*!< Indicate that the device is started. */
    sprt_scutimer->IsStarted = false;
}

/*!
 * @brief   load timer period
 * @param   sprt_scutimer, value (period)
 * @retval  none
 * @note    none
 */
void XScuTimer_LoadTimer(XScuTimer *sprt_scutimer, kuint32_t value)
{
    mrt_writel(value, sprt_scutimer->Config.BaseAddr + XSCUTIMER_LOAD_OFFSET);
}

/*!
 * @brief   restart timer
 * @param   sprt_scutimer
 * @retval  none
 * @note    none
 */
void XScuTimer_RestartTimer(XScuTimer *sprt_scutimer)
{
    kuint32_t value;

    value = mrt_readl(sprt_scutimer->Config.BaseAddr + XSCUTIMER_LOAD_OFFSET);
    XScuTimer_LoadTimer(sprt_scutimer, value);
}

/*!
 * @brief   enable timer to load period automatically
 * @param   sprt_scutimer
 * @retval  none
 * @note    none
 */
void XScuTimer_EnableAutoReload(XScuTimer *sprt_scutimer)
{
    kuint32_t value;

    value  = mrt_readl(sprt_scutimer->Config.BaseAddr + XSCUTIMER_CONTROL_OFFSET);
    value |= XSCUTIMER_CONTROL_AUTO_RELOAD_MASK;
    mrt_writel(value, sprt_scutimer->Config.BaseAddr + XSCUTIMER_CONTROL_OFFSET);
}

/*!
 * @brief   disable timer to load period automatically
 * @param   sprt_scutimer
 * @retval  none
 * @note    none
 */
void XScuTimer_DisableAutoReload(XScuTimer *sprt_scutimer)
{
    kuint32_t value;

    value  = mrt_readl(sprt_scutimer->Config.BaseAddr + XSCUTIMER_CONTROL_OFFSET);
    value &= ~XSCUTIMER_CONTROL_AUTO_RELOAD_MASK;
    mrt_writel(value, sprt_scutimer->Config.BaseAddr + XSCUTIMER_CONTROL_OFFSET);
}

/*!
 * @brief   timer interrupt enable
 * @param   sprt_scutimer
 * @retval  none
 * @note    none
 */
void XScuTimer_EnableInterrupt(XScuTimer *sprt_scutimer)
{
    kuint32_t value;

    value  = mrt_readl(sprt_scutimer->Config.BaseAddr + XSCUTIMER_CONTROL_OFFSET);
    value |= XSCUTIMER_CONTROL_IRQ_ENABLE_MASK;
    mrt_writel(value, sprt_scutimer->Config.BaseAddr + XSCUTIMER_CONTROL_OFFSET);
}

/*!
 * @brief   timer interrupt disable
 * @param   sprt_scutimer
 * @retval  none
 * @note    none
 */
void XScuTimer_DisableInterrupt(XScuTimer *sprt_scutimer)
{
    kuint32_t value;

    value  = mrt_readl(sprt_scutimer->Config.BaseAddr + XSCUTIMER_CONTROL_OFFSET);
    value &= ~XSCUTIMER_CONTROL_IRQ_ENABLE_MASK;
    mrt_writel(value, sprt_scutimer->Config.BaseAddr + XSCUTIMER_CONTROL_OFFSET);
}

/*!
 * @brief   get timer interrupt status
 * @param   sprt_scutimer
 * @retval  none
 * @note    none
 */
kuint32_t XScuTimer_GetInterruptStatus(XScuTimer *sprt_scutimer)
{
    return mrt_readl(sprt_scutimer->Config.BaseAddr + XSCUTIMER_ISR_OFFSET);
}

/*!
 * @brief   clear timer interrupt status
 * @param   sprt_scutimer
 * @retval  none
 * @note    none
 */
void XScuTimer_ClearInterruptStatus(XScuTimer *sprt_scutimer)
{
    mrt_writel(XSCUTIMER_ISR_EVENT_FLAG_MASK, 
            sprt_scutimer->Config.BaseAddr + XSCUTIMER_ISR_OFFSET);
}

/*!
 * @brief   get the configuration of UartPs
 * @param   DeviceId
 * @retval  XUartPs_Config
 * @note    none
 */
XUartPs_Config *XUartPs_LookupConfig(kuint16_t DeviceId)
{
    XUartPs_Config *sprt_cfg = &sgrt_xuart_ps_config_table[0];
    kuint32_t idx;

    for (idx = 0U; idx < XPAR_XUARTPS_NUM_INSTANCES; idx++) 
    {
        if (sprt_cfg[idx].DeviceId == DeviceId) 
            return sprt_cfg;
    }

    return mrt_nullptr;
}

/*!
 * @brief   enable Tx and Rx
 * @param   sprt_uart
 * @retval  none
 * @note    none
 */
void XUartPs_EnableUart(XUartPs *sprt_uart)
{
    kuint32_t CrReg;

    CrReg = mrt_readl(sprt_uart->sgrt_cfg.BaseAddress + XUARTPS_CR_OFFSET);
    mrt_clrbitl(XUARTPS_CR_EN_DIS_MASK, &CrReg);
    mrt_setbitl(XUARTPS_CR_RX_EN | XUARTPS_CR_TX_EN, &CrReg);
    mrt_writel(CrReg, sprt_uart->sgrt_cfg.BaseAddress + XUARTPS_CR_OFFSET);
}

/*!
 * @brief   disable Tx and Rx
 * @param   sprt_uart
 * @retval  none
 * @note    none
 */
void XUartPs_DisableUart(XUartPs *sprt_uart)
{
    kuint32_t CrReg;

    CrReg = mrt_readl(sprt_uart->sgrt_cfg.BaseAddress + XUARTPS_CR_OFFSET);
    mrt_clrbitl(XUARTPS_CR_EN_DIS_MASK, &CrReg);
    mrt_setbitl(XUARTPS_CR_RX_DIS | XUARTPS_CR_TX_DIS, &CrReg);
    mrt_writel(CrReg, sprt_uart->sgrt_cfg.BaseAddress + XUARTPS_CR_OFFSET);
}

/*!
 * @brief   configure baund rate
 * @param   IterBAUDDIV: Iterator for available baud divisor values
 * @param   BRGR_Value: Calculated value for baud rate generator
 * @param   CalcBaudRate: Calculated baud rate
 * @param   BaudError: Diff between calculated and requested baud rate
 * @param   Best_BRGR: Best value for baud rate generator
 * @param   Best_BAUDDIV: Best value for baud divisor
 * @retval  errno
 * @note    none
 */
kint32_t XUartPs_SetBaudRate(XUartPs *sprt_uart, kuint32_t BaudRate)
{
    kuint32_t IterBAUDDIV, BRGR_Value;
    kuint32_t CalcBaudRate, BaudError;
    kuint32_t Best_BRGR = 0U;
    kuint8_t Best_BAUDDIV = 0U;
    kuint32_t Best_Error = (kuint32_t)(~0U);
    kuint32_t PercentError, ModeReg, InputClk;

    if ((!sprt_uart) ||
        (!sprt_uart->IsReady) ||
        (BaudRate > XUARTPS_MAX_RATE) ||
        (BaudRate < XUARTPS_MIN_RATE))
        return -ER_FAULT;

    /*!<
     * Make sure the baud rate is not impossilby large.
     * Fastest possible baud rate is Input Clock / 2.
     */
    if ((BaudRate * 2) > sprt_uart->sgrt_cfg.InputClockHz)
        return -ER_CHECKERR;

    /*!< Check whether the input clock is divided by 8 */
    ModeReg = mrt_readl(sprt_uart->sgrt_cfg.BaseAddress + XUARTPS_MR_OFFSET);

    InputClk = sprt_uart->sgrt_cfg.InputClockHz;
    if (mrt_isBitSetl(XUARTPS_MR_CLKSEL, &ModeReg))
        InputClk = sprt_uart->sgrt_cfg.InputClockHz / 8;

    /*!<
     * Determine the Baud divider. It can be 4 to 254.
     * Loop through all possible combinations
     */
    for (IterBAUDDIV = 4; IterBAUDDIV < 255; IterBAUDDIV++) 
    {
        /*!<
         * InputClk: 100000000Hz = 100000KHz = 100MHz;
         * BaudRate: such as 115200
         * IterBAUDDIV: 4 ~ 255
         * 
         * for example, IterBAUDDIV = 4:
         *      BRGR_Value = 100000000 / (115200 * (4 + 1)) = 173.611111 = 173
         *      CalcBaudRate = 100000000 / (173 * (4 + 1)) = 115606.9364 = 115606
         *      BaudError = 115606 - 115200 = 406 = Best_Error
         * 
         * the root cause is that the decimal places are lost by using integer calculation instead of decimal calculations,
         * therefore, it is necessary to traverse to find the integer solution with the smallest errors.
         */

        /*!< Calculate the value for BRGR register */
        BRGR_Value = InputClk / (BaudRate * (IterBAUDDIV + 1));

        /*!< Calculate the baud rate from the BRGR value */
        CalcBaudRate = InputClk / (BRGR_Value * (IterBAUDDIV + 1));

        /*!< Avoid unsigned integer underflow */
        BaudError = mrt_usub(BaudRate, CalcBaudRate);

        /*!< Find the calculated baud rate closest to requested baud rate. */
        if (BaudError < Best_Error) 
        {
            Best_BRGR = BRGR_Value;
            Best_BAUDDIV = IterBAUDDIV;
            Best_Error = BaudError;
        }
    }

    /*!< 
     * Make sure the best error is not too large. 
     * error = (|precent_baund - baundrate| / baundrate) * 100%
     * the maximun error allowed: 3%
     */
    PercentError = (Best_Error * 100) / BaudRate;
    if (PercentError > 3)
        return -ER_FAULT;

    /*!< Disable TX and RX to avoid glitches when setting the baud rate. */
    XUartPs_DisableUart(sprt_uart);

    mrt_writel(Best_BRGR, sprt_uart->sgrt_cfg.BaseAddress + XUARTPS_BAUDGEN_OFFSET);
    mrt_writel(Best_BAUDDIV, sprt_uart->sgrt_cfg.BaseAddress + XUARTPS_BAUDDIV_OFFSET);

    /*!< RX and TX SW reset */
    mrt_writel(XUARTPS_CR_TXRST | XUARTPS_CR_RXRST, sprt_uart->sgrt_cfg.BaseAddress + XUARTPS_CR_OFFSET);

    /*!< Enable device */
    XUartPs_EnableUart(sprt_uart);
    sprt_uart->BaudRate = BaudRate;

    return ER_NORMAL;
}

/*!
 * @brief   fill sprt_uart
 * @param   sprt_uart
 * @retval  errno
 * @note    none
 */
kint32_t XUartPs_CfgInitialize(XUartPs *sprt_uart, XUartPs_Config *sprt_cfg, kuint32_t address)
{
    kint32_t Status;
    kuint32_t ModeRegister;

    if (!sprt_uart || !sprt_cfg)
        return -ER_NOMEM;

    /*!< Setup the driver instance using passed in parameters */
    sprt_uart->sgrt_cfg.BaseAddress = address;
    sprt_uart->sgrt_cfg.InputClockHz = sprt_cfg->InputClockHz;
    sprt_uart->sgrt_cfg.ModemPinsConnected = sprt_cfg->ModemPinsConnected;

    sprt_uart->sgrt_txbuf.NextBytePtr = mrt_nullptr;
    sprt_uart->sgrt_txbuf.RemainingBytes = 0U;
    sprt_uart->sgrt_txbuf.RequestedBytes = 0U;

    sprt_uart->sgrt_rxbuf.NextBytePtr = mrt_nullptr;
    sprt_uart->sgrt_rxbuf.RemainingBytes = 0U;
    sprt_uart->sgrt_rxbuf.RequestedBytes = 0U;

    sprt_uart->is_rxbs_error = false;

    /*!< Flag that the driver instance is ready to use */
    sprt_uart->IsReady = true;

    /*!< Set the default baud rate here, can be changed prior to starting the device */
    Status = XUartPs_SetBaudRate(sprt_uart, XUARTPS_DFT_BAUDRATE);
    if (Status)
    {
        sprt_uart->IsReady = 0U;
        return Status;
    }
    
    /*!< Set up the default data format: 8 bit data, 1 stop bit, no parity */
    ModeRegister = mrt_readl(sprt_uart->sgrt_cfg.BaseAddress + XUARTPS_MR_OFFSET);

    /*!< Mask off what's already there */
    mrt_clrbitl(XUARTPS_MR_CHARLEN_MASK | XUARTPS_MR_STOPMODE_MASK | 
                XUARTPS_MR_PARITY_MASK, &ModeRegister);

    /*!< Set the register value to the desired data format */
    mrt_setbitl(XUARTPS_MR_CHARLEN_8_BIT | XUARTPS_MR_STOPMODE_1_BIT | 
                XUARTPS_MR_PARITY_NONE, &ModeRegister);

    /*!< Write the mode register out */
    mrt_writel(ModeRegister, sprt_uart->sgrt_cfg.BaseAddress + XUARTPS_MR_OFFSET);

    /*!< Set the RX FIFO trigger at 8 data bytes. */
    mrt_writel(0x08U, sprt_uart->sgrt_cfg.BaseAddress + XUARTPS_RXWM_OFFSET);

    /*!< Set the RX timeout to 1, which will be 4 character time */
    mrt_writel(0x01U, sprt_uart->sgrt_cfg.BaseAddress + XUARTPS_RXTOUT_OFFSET);

    /*!< Disable all interrupts, polled mode is the default */
    mrt_writel(XUARTPS_IXR_MASK, sprt_uart->sgrt_cfg.BaseAddress + XUARTPS_IDR_OFFSET);

    return ER_NORMAL;
}

/*!
 * @brief   check if tx fifo is full
 * @param   BaseAddress
 * @retval  1: full; 0, not full
 * @note    none
 */
kbool_t XUartPs_IsSendFull(kuint32_t BaseAddress)
{
    kuint32_t SrReg;

    SrReg = mrt_readl(BaseAddress + XUARTPS_SR_OFFSET);
    return mrt_isBitSetl(XUARTPS_SR_TXFULL, &SrReg);
}

/*!
 * @brief   check if tx fifo is empty
 * @param   BaseAddress
 * @retval  1: empty; 0, not empty
 * @note    none
 */
kbool_t XUartPs_IsSendEmpty(kuint32_t BaseAddress)
{
    kuint32_t SrReg;

    SrReg = mrt_readl(BaseAddress + XUARTPS_SR_OFFSET);
    return mrt_isBitSetl(XUARTPS_SR_TXEMPTY, &SrReg);
}

/*!
 * @brief   check if rx fifo is full
 * @param   BaseAddress
 * @retval  1: full; 0, not full
 * @note    none
 */
kbool_t XUartPs_IsRecvFull(kuint32_t BaseAddress)
{
    kuint32_t SrReg;

    SrReg = mrt_readl(BaseAddress + XUARTPS_SR_OFFSET);
    return mrt_isBitSetl(XUARTPS_SR_RXFULL, &SrReg);
}

/*!
 * @brief   check if rx fifo is empty
 * @param   BaseAddress
 * @retval  1: empty; 0, not empty
 * @note    none
 */
kbool_t XUartPs_IsRecvEmpty(kuint32_t BaseAddress)
{
    kuint32_t SrReg;

    SrReg = mrt_readl(BaseAddress + XUARTPS_SR_OFFSET);
    return mrt_isBitSetl(XUARTPS_SR_RXEMPTY, &SrReg);
}

/*!
 * @brief   send multi-bytes
 * @param   sprt_uart
 * @retval  errno, or the number of bytes sent
 * @note    none
 */
kint32_t XUartPs_SendBuffer(XUartPs *sprt_uart)
{
    kuint32_t SentCount = 0U;
    kuint32_t ImrRegister;
    kuint32_t BaseAddress;

    BaseAddress = sprt_uart->sgrt_cfg.BaseAddress;

    if (!XUartPs_IsRecvEmpty(BaseAddress))
        return -1;

    /*!< wait for last data sending finish */
    while (!XUartPs_IsSendEmpty(BaseAddress));

    /*!<
     * If the TX FIFO is full, send nothing.
     * Otherwise put bytes into the TX FIFO unil it is full, or all of the
     * data has been put into the FIFO.
     */
    while (!XUartPs_IsSendFull(BaseAddress)) 
    {
        if (SentCount >= sprt_uart->sgrt_txbuf.RemainingBytes)
            break;

        /*!< Fill the FIFO from the buffer */
        mrt_writel(sprt_uart->sgrt_txbuf.NextBytePtr[SentCount],
                            BaseAddress + XUARTPS_FIFO_OFFSET);

        /*!< Increment the send count. */
        SentCount++;
    }

    /*!< Update the buffer to reflect the bytes that were sent from it */
    sprt_uart->sgrt_txbuf.NextBytePtr += SentCount;
    sprt_uart->sgrt_txbuf.RemainingBytes -= SentCount;

    /*!<
     * If interrupts are enabled as indicated by the receive interrupt, then
     * enable the TX FIFO empty interrupt, so further action can be taken
     * for this sending.
     */
    ImrRegister = mrt_readl(BaseAddress + XUARTPS_IMR_OFFSET);
    if (!mrt_isBitResetl(XUARTPS_IXR_RXFULL | 
            XUARTPS_IXR_RXEMPTY | XUARTPS_IXR_RXOVR, &ImrRegister))
        mrt_writel(ImrRegister | XUARTPS_IXR_TXEMPTY, BaseAddress + XUARTPS_IER_OFFSET);

    return SentCount;
}

/*!
 * @brief   receive multi-bytes
 * @param   sprt_uart
 * @retval  errno, or the number of bytes received
 * @note    none
 */
kint32_t XUartPs_ReceiveBuffer(XUartPs *sprt_uart)
{
    kuint32_t ReceivedCount = 0U;

    /*!<
     * Loop until there is no more data in RX FIFO or the specified
     * number of bytes has been received
     */
    while (!XUartPs_IsRecvEmpty(sprt_uart->sgrt_cfg.BaseAddress))
    {
        if (ReceivedCount >= sprt_uart->sgrt_rxbuf.RemainingBytes)
            break;

        sprt_uart->sgrt_rxbuf.NextBytePtr[ReceivedCount] = mrt_readl(sprt_uart->sgrt_cfg.BaseAddress + XUARTPS_FIFO_OFFSET);   
        ReceivedCount++;

        delay_ms(1);
    }
    sprt_uart->is_rxbs_error = 0;

    /*!<
     * Update the receive buffer to reflect the number of bytes just
     * received
     */
    if(sprt_uart->sgrt_rxbuf.NextBytePtr)
        sprt_uart->sgrt_rxbuf.NextBytePtr += ReceivedCount;
    sprt_uart->sgrt_rxbuf.RemainingBytes -= ReceivedCount;

    return ReceivedCount;
}

/*!
 * @brief   send multi-bytes
 * @param   sprt_uart
 * @retval  errno, or the number of bytes sent
 * @note    none
 */
kint32_t XUartPs_Send(XUartPs *sprt_uart, kuint8_t *BufferPtr, kuint32_t NumBytes)
{
    if (!sprt_uart || !BufferPtr || !sprt_uart->IsReady)
        return -ER_NOMEM;

    /*!<
     * Disable the UART transmit interrupts to allow this call to stop a
     * previous operation that may be interrupt driven.
     */
    mrt_writel(XUARTPS_IXR_TXEMPTY | XUARTPS_IXR_TXFULL, sprt_uart->sgrt_cfg.BaseAddress + XUARTPS_IDR_OFFSET);

    /*!< Setup the buffer parameters */
    sprt_uart->sgrt_txbuf.RequestedBytes = NumBytes;
    sprt_uart->sgrt_txbuf.RemainingBytes = NumBytes;
    sprt_uart->sgrt_txbuf.NextBytePtr = BufferPtr;

    /*!<
     * Transmit interrupts will be enabled in XUartPs_SendBuffer(), after
     * filling the TX FIFO.
     */
    return XUartPs_SendBuffer(sprt_uart);
}

/*!
 * @brief   receive multi-bytes
 * @param   sprt_uart
 * @retval  errno, or the number of bytes received
 * @note    none
 */
kint32_t XUartPs_Recv(XUartPs *sprt_uart, kuint8_t *BufferPtr, kuint32_t NumBytes)
{
    kuint32_t ReceivedCount;
    kuint32_t ImrRegister;

    if (!sprt_uart || !BufferPtr || !sprt_uart->IsReady)
        return -ER_NOMEM;

    if (XUartPs_IsRecvEmpty(sprt_uart->sgrt_cfg.BaseAddress))
        return -ER_TIMEOUT;

    /*!<
     * Disable all the interrupts.
     * This stops a previous operation that may be interrupt driven
     */
    ImrRegister = mrt_readl(sprt_uart->sgrt_cfg.BaseAddress + XUARTPS_IMR_OFFSET);
    mrt_writel(XUARTPS_IXR_MASK, sprt_uart->sgrt_cfg.BaseAddress + XUARTPS_IDR_OFFSET);

    /*!< Setup the buffer parameters */
    sprt_uart->sgrt_rxbuf.RequestedBytes = NumBytes;
    sprt_uart->sgrt_rxbuf.RemainingBytes = NumBytes;
    sprt_uart->sgrt_rxbuf.NextBytePtr = BufferPtr;

    /*!< Receive the data from the device */
    ReceivedCount = XUartPs_ReceiveBuffer(sprt_uart);

    /*!< Restore the interrupt state */
    mrt_writel(ImrRegister, sprt_uart->sgrt_cfg.BaseAddress + XUARTPS_IER_OFFSET);

    return ReceivedCount;
}

/*!
 * @brief   get sd config structure
 * @param   DeviceId
 * @retval  XSdPs_Config
 * @note    none
 */
XSdPs_Config *XSdPs_LookupConfig(kuint16_t DeviceId)
{
    XSdPs_Config *sprt_cfg = &sgrt_xsd_ps_config_table[0];
    kuint32_t idx;

    for (idx = 0U; idx < XPAR_XSDPS_NUM_INSTANCES; idx++) 
    {
        if (sprt_cfg[idx].DeviceId == DeviceId) 
            return sprt_cfg;
    }

    return mrt_nullptr;
}

kint32_t XSdPs_Change_ClkFreq(XSdPs *sprt_sd, kuint32_t SelFreq)
{
    kuint32_t BaseAddress;
    kuint16_t ClockReg;
    kuint16_t DivCnt;
    kuint16_t Divisor = 0U;
    kuint16_t ExtDivisor;

    if (!sprt_sd || !sprt_sd->IsReady)
        return -ER_NOMEM;

    BaseAddress = sprt_sd->sgrt_cfg.BaseAddress;

    /* Disable clock */
    ClockReg = XSdPs_ReadReg16(BaseAddress, XSDPS_CLK_CTRL_OFFSET);
    mrt_clrbitw(XSDPS_CC_SD_CLK_EN_MASK | XSDPS_CC_INT_CLK_EN_MASK, &ClockReg);
    XSdPs_WriteReg16(BaseAddress, XSDPS_CLK_CTRL_OFFSET, ClockReg);

    if (sprt_sd->HC_Version == XSDPS_HC_SPEC_V3) 
    {
        /* Calculate divisor */
        for (DivCnt = 0x1U; DivCnt <= XSDPS_CC_EXT_MAX_DIV_CNT; DivCnt++) 
        {
            if (((sprt_sd->sgrt_cfg.InputClockHz) / DivCnt) <= SelFreq) 
            {
                Divisor = DivCnt >> 1;
                break;
            }
        }

        /* No valid divisor found for given frequency */
        if (DivCnt > XSDPS_CC_EXT_MAX_DIV_CNT)
            return -ER_FAILD;
    } 
    else 
    {
        /* Calculate divisor */
        DivCnt = 0x1U;
        while (DivCnt <= XSDPS_CC_MAX_DIV_CNT) 
        {
            if (((sprt_sd->sgrt_cfg.InputClockHz) / DivCnt) <= SelFreq) 
            {
                Divisor = DivCnt / 2U;
                break;
            }

            DivCnt = DivCnt << 1U;
        }

        /* No valid divisor found for given frequency */
        if (DivCnt > XSDPS_CC_MAX_DIV_CNT)
            return -ER_FAILD;
    }

    ClockReg = XSdPs_ReadReg16(BaseAddress, XSDPS_CLK_CTRL_OFFSET);
 
    /* Set clock divisor */
    if (sprt_sd->HC_Version == XSDPS_HC_SPEC_V3) 
    {
        mrt_clrbitw(XSDPS_CC_SDCLK_FREQ_SEL_MASK | XSDPS_CC_SDCLK_FREQ_SEL_EXT_MASK, &ClockReg);

        ExtDivisor = Divisor >> 8;
        ExtDivisor <<= XSDPS_CC_EXT_DIV_SHIFT;
        ExtDivisor &= XSDPS_CC_SDCLK_FREQ_SEL_EXT_MASK;

        Divisor <<= XSDPS_CC_DIV_SHIFT;
        Divisor &= XSDPS_CC_SDCLK_FREQ_SEL_MASK;
        ClockReg |= Divisor | ExtDivisor | (kuint16_t)XSDPS_CC_INT_CLK_EN_MASK;
    } 
    else 
    {
        ClockReg = XSdPs_ReadReg16(sprt_sd->sgrt_cfg.BaseAddress, XSDPS_CLK_CTRL_OFFSET);
        ClockReg &= (~XSDPS_CC_SDCLK_FREQ_SEL_MASK);

        Divisor <<= XSDPS_CC_DIV_SHIFT;
        Divisor &= XSDPS_CC_SDCLK_FREQ_SEL_MASK;
        ClockReg |= Divisor | (kuint16_t)XSDPS_CC_INT_CLK_EN_MASK;
    }

    XSdPs_WriteReg16(BaseAddress, XSDPS_CLK_CTRL_OFFSET, ClockReg);

    /* Wait for internal clock to stabilize */
    while (mrt_isBitResetw(XSDPS_CC_INT_CLK_STABLE_MASK, BaseAddress + XSDPS_CLK_CTRL_OFFSET));

    /* Enable SD clock */
    mrt_setbitw(XSDPS_CC_SD_CLK_EN_MASK, BaseAddress + XSDPS_CLK_CTRL_OFFSET);

    return ER_NORMAL;
}

kint32_t XSdPs_CfgInitialize(XSdPs *sprt_sd, XSdPs_Config *sprt_cfg, kuint32_t address)
{
    kint32_t Status;
    kuint8_t PowerLevel;
    kuint16_t ReadReg16;

    if (!sprt_sd || !sprt_cfg)
        return -ER_NOMEM;

    /* Set some default values. */
    sprt_sd->sgrt_cfg.DeviceId = sprt_cfg->DeviceId;
    sprt_sd->sgrt_cfg.BaseAddress = address;
    sprt_sd->sgrt_cfg.InputClockHz = sprt_cfg->InputClockHz;
    sprt_sd->IsReady = true;
    sprt_sd->sgrt_cfg.CardDetect = sprt_cfg->CardDetect;
    sprt_sd->sgrt_cfg.WriteProtect = sprt_cfg->WriteProtect;
    sprt_sd->sgrt_cfg.BusWidth = sprt_cfg->BusWidth;
    sprt_sd->sgrt_cfg.BankNumber = sprt_cfg->BankNumber;
    sprt_sd->sgrt_cfg.HasEMIO = sprt_cfg->HasEMIO;
    sprt_sd->sgrt_cfg.IsCacheCoherent = sprt_cfg->IsCacheCoherent;
    sprt_sd->SectorCount = 0;
    sprt_sd->Mode = XSDPS_DEFAULT_SPEED_MODE;
    sprt_sd->TransferMode = 0U;
    sprt_sd->Dma64BitAddr = 0U;

    /* Disable bus power and issue emmc hw reset */
    ReadReg16 = mrt_getbitw(XSDPS_HC_SPEC_VER_MASK, 
                            sprt_sd->sgrt_cfg.BaseAddress + XSDPS_HOST_CTRL_VER_OFFSET);
    
    if (XSDPS_HC_SPEC_V3 == ReadReg16)
        XSdPs_WriteReg8(sprt_sd->sgrt_cfg.BaseAddress, XSDPS_POWER_CTRL_OFFSET, XSDPS_PC_EMMC_HW_RST_MASK);
    else
        XSdPs_WriteReg8(sprt_sd->sgrt_cfg.BaseAddress, XSDPS_POWER_CTRL_OFFSET, 0x0);

    /* Delay to poweroff card */
    delay_us(1000);

    /* "Software reset for all" is initiated */
    XSdPs_WriteReg8(sprt_sd->sgrt_cfg.BaseAddress, XSDPS_SW_RST_OFFSET, XSDPS_SWRST_ALL_MASK);

    /* Proceed with initialization only after reset is complete */
    while (!mrt_isBitResetb(XSDPS_SWRST_ALL_MASK, sprt_sd->sgrt_cfg.BaseAddress + XSDPS_SW_RST_OFFSET));
    
    /* Host Controller version is read. */
    sprt_sd->HC_Version = (kuint8_t)mrt_getbitw(XSDPS_HC_SPEC_VER_MASK, 
                                            sprt_sd->sgrt_cfg.BaseAddress + XSDPS_HOST_CTRL_VER_OFFSET);

    /*
     * Read capabilities register and update it in Instance pointer.
     * It is sufficient to read this once on power on.
     */
    sprt_sd->Host_Caps = XSdPs_ReadReg(sprt_sd->sgrt_cfg.BaseAddress, XSDPS_CAPS_OFFSET);

    /* Select voltage and enable bus power. */
    if (sprt_sd->HC_Version == XSDPS_HC_SPEC_V3)
    {
        XSdPs_WriteReg8(sprt_sd->sgrt_cfg.BaseAddress, XSDPS_POWER_CTRL_OFFSET, 
                                mrt_mask(XSDPS_PC_BUS_VSEL_3V3_MASK | XSDPS_PC_BUS_PWR_MASK, ~XSDPS_PC_EMMC_HW_RST_MASK));

        /* Delay before issuing the command after emmc reset */
        if (XSDPS_CAPS_EMB_SLOT == mrt_mask(sprt_sd->Host_Caps, XSDPS_CAPS_SLOT_TYPE_MASK))
            delay_us(200);
    }
    else
    {
        XSdPs_WriteReg8(sprt_sd->sgrt_cfg.BaseAddress, XSDPS_POWER_CTRL_OFFSET, 
                                XSDPS_PC_BUS_VSEL_3V3_MASK | XSDPS_PC_BUS_PWR_MASK);
    }

    /* Change the clock frequency to 400 KHz */
    Status = XSdPs_Change_ClkFreq(sprt_sd, XSDPS_CLK_400_KHZ);
    if (Status)
        return Status;

    if (!mrt_isBitResetl(XSDPS_CAP_VOLT_3V3_MASK, &sprt_sd->Host_Caps))
        PowerLevel = XSDPS_PC_BUS_VSEL_3V3_MASK;
    else if (!mrt_isBitResetl(XSDPS_CAP_VOLT_3V0_MASK, &sprt_sd->Host_Caps))
        PowerLevel = XSDPS_PC_BUS_VSEL_3V0_MASK;
    else if (!mrt_isBitResetl(XSDPS_CAP_VOLT_1V8_MASK, &sprt_sd->Host_Caps))
        PowerLevel = XSDPS_PC_BUS_VSEL_1V8_MASK;
    else
        PowerLevel = 0U;

    /* Select voltage based on capability and enable bus power. */
    XSdPs_WriteReg8(sprt_sd->sgrt_cfg.BaseAddress, XSDPS_POWER_CTRL_OFFSET, PowerLevel | XSDPS_PC_BUS_PWR_MASK);

    if (sprt_sd->HC_Version == XSDPS_HC_SPEC_V3) 
    {
        /* Enable ADMA2 in 64bit mode. */
        XSdPs_WriteReg8(sprt_sd->sgrt_cfg.BaseAddress, XSDPS_HOST_CTRL1_OFFSET, XSDPS_HC_DMA_ADMA2_64_MASK);
    } 
    else 
    {
        /*!< 
         * bit5: Bus width (if set to 1, bus width is 8)
         * bit4: Enable ADMA2 in 32bit mode. 
         * bit1: Bus width (if set to 1, bus width is 4)
         */
        XSdPs_WriteReg8(sprt_sd->sgrt_cfg.BaseAddress, XSDPS_HOST_CTRL1_OFFSET, XSDPS_HC_DMA_ADMA2_32_MASK);
    }

    /* Enable all interrupt status except card interrupt initially */
    XSdPs_WriteReg16(sprt_sd->sgrt_cfg.BaseAddress, XSDPS_NORM_INTR_STS_EN_OFFSET, 
                                                mrt_mask(XSDPS_NORM_INTR_ALL_MASK, ~XSDPS_INTR_CARD_MASK));
    XSdPs_WriteReg16(sprt_sd->sgrt_cfg.BaseAddress, XSDPS_ERR_INTR_STS_EN_OFFSET, XSDPS_ERROR_INTR_ALL_MASK);

    /* Disable all interrupt signals by default. */
    XSdPs_WriteReg16(sprt_sd->sgrt_cfg.BaseAddress, XSDPS_NORM_INTR_SIG_EN_OFFSET, 0x0U);
    XSdPs_WriteReg16(sprt_sd->sgrt_cfg.BaseAddress, XSDPS_ERR_INTR_SIG_EN_OFFSET, 0x0U);

    /*
     * Transfer mode register - default value
     * DMA enabled, block count enabled, data direction card to host(read)
     */
    sprt_sd->TransferMode = XSDPS_TM_DMA_EN_MASK | XSDPS_TM_BLK_CNT_EN_MASK | XSDPS_TM_DAT_DIR_SEL_MASK;

    /* Set block size to 512 by default */
    XSdPs_WriteReg16(sprt_sd->sgrt_cfg.BaseAddress, XSDPS_BLK_SIZE_OFFSET, XSDPS_BLK_SIZE_512_MASK);

    return ER_NORMAL;
}

kbool_t XSdPs_IsCardDetected(XSdPs *sprt_sd)
{
    kuint32_t PresentStateReg;

    if (!sprt_sd)
        return false;

    if ((sprt_sd->HC_Version != XSDPS_HC_SPEC_V3) ||
        ((sprt_sd->Host_Caps & XSDPS_CAPS_SLOT_TYPE_MASK) != XSDPS_CAPS_EMB_SLOT)) 
    {
        if (sprt_sd->sgrt_cfg.CardDetect) 
        {
            /*
             * Check the present state register to make sure
             * card is inserted and detected by host controller
             */
            PresentStateReg = XSdPs_ReadReg(sprt_sd->sgrt_cfg.BaseAddress, XSDPS_PRES_STATE_OFFSET);
            if (mrt_isBitResetl(XSDPS_PSR_CARD_INSRT_MASK, &PresentStateReg))
                return false;
        }
    }

    return true;
}

kint32_t XSdPs_Change_BusWidth(XSdPs *sprt_sd, kuint32_t width)
{
    kuint32_t StatusReg;

    if (!sprt_sd || !sprt_sd->IsReady)
        return -ER_FAULT;

    if (sprt_sd->CardType != XSDPS_CARD_SD)
        return -ER_NODEV;

    if ((width != XSDPS_8_BIT_WIDTH) &&
        (width != XSDPS_4_BIT_WIDTH) &&
        (width != XSDPS_1_BIT_WIDTH))
        return -ER_FAULT;

    /*
     * check for bus width for 3.0 controller and return if
     * bus width is <4
     */
    if ((sprt_sd->HC_Version == XSDPS_HC_SPEC_V3) &&
        (sprt_sd->sgrt_cfg.BusWidth < XSDPS_WIDTH_4))
        return ER_NORMAL;

    StatusReg = XSdPs_ReadReg8(sprt_sd->sgrt_cfg.BaseAddress, XSDPS_HOST_CTRL1_OFFSET);

    /* Width setting in controller */
    if (sprt_sd->BusWidth == XSDPS_8_BIT_WIDTH)
        StatusReg |= XSDPS_HC_EXT_BUS_WIDTH;
    else if (sprt_sd->BusWidth == XSDPS_4_BIT_WIDTH)
        StatusReg |= XSDPS_HC_BUS_WIDTH_4;
    else
        StatusReg &= ~(XSDPS_HC_EXT_BUS_WIDTH | XSDPS_HC_BUS_WIDTH_4);

    XSdPs_WriteReg8(sprt_sd->sgrt_cfg.BaseAddress, XSDPS_HOST_CTRL1_OFFSET, (kuint8_t)StatusReg);
    delay_us(100U);

    if (sprt_sd->Mode == XSDPS_DDR52_MODE) 
    {
        StatusReg = XSdPs_ReadReg16(sprt_sd->sgrt_cfg.BaseAddress, XSDPS_HOST_CTRL2_OFFSET);
        StatusReg &= (kuint16_t)(~XSDPS_HC2_UHS_MODE_MASK);
        StatusReg |= sprt_sd->Mode;
        XSdPs_WriteReg16(sprt_sd->sgrt_cfg.BaseAddress, XSDPS_HOST_CTRL2_OFFSET, StatusReg);
    }

    return ER_NORMAL;
}

kint32_t XSdPs_SetBlkSize(XSdPs *sprt_sd, kuint32_t BlkCnt, kuint32_t BlkSize)
{
    XSdPs_WriteReg16(sprt_sd->sgrt_cfg.BaseAddress, XSDPS_BLK_CNT_OFFSET, BlkCnt);
    XSdPs_WriteReg16(sprt_sd->sgrt_cfg.BaseAddress, XSDPS_BLK_SIZE_OFFSET, BlkSize);

    delay_us(100U);
    return ER_NORMAL;
}

void XSdPs_SetupADMA2DescTbl(XSdPs *sprt_sd, kuint32_t BlkCnt, const kuint8_t *Buff)
{
    kuint32_t DescNum = 0;
    kuint32_t BlkSize, TotalSize, RemainSize;

    /* Setup ADMA2 - Write descriptor table and point ADMA SAR to it */
    BlkSize = mrt_getbitw(XSDPS_BLK_SIZE_MASK, sprt_sd->sgrt_cfg.BaseAddress + XSDPS_BLK_SIZE_OFFSET);

    TotalSize = BlkCnt * BlkSize;
    if (!TotalSize)
        return;

    RemainSize = TotalSize;
    do {
        sprt_sd->Adma2_DescrTbl[DescNum].Address =
                (kuint32_t)((kuaddr_t)Buff + (DescNum * XSDPS_DESC_MAX_LENGTH));
        sprt_sd->Adma2_DescrTbl[DescNum].Attribute = XSDPS_DESC_TRAN | XSDPS_DESC_VALID;

        if (RemainSize > XSDPS_DESC_MAX_LENGTH)
        {
            /* This will write '0' to length field which indicates 65536 */
            sprt_sd->Adma2_DescrTbl[DescNum].Length = (kuint16_t)XSDPS_DESC_MAX_LENGTH;
            RemainSize -= XSDPS_DESC_MAX_LENGTH;
        }
        else
        {
            sprt_sd->Adma2_DescrTbl[DescNum].Attribute |= XSDPS_DESC_END;
            sprt_sd->Adma2_DescrTbl[DescNum].Length = (kuint16_t)RemainSize;
            RemainSize = 0;
        }

        DescNum++;

    } while (RemainSize);

    XSdPs_WriteReg(sprt_sd->sgrt_cfg.BaseAddress, XSDPS_ADMA_SAR_OFFSET, 
                                (kuaddr_t)&(sprt_sd->Adma2_DescrTbl[0]));

    if (!sprt_sd->sgrt_cfg.IsCacheCoherent)
        Xil_DCacheFlushRange((kuaddr_t)&(sprt_sd->Adma2_DescrTbl[0]), sizeof(XSdPs_Adma2Descriptor) * 32U);
}


/*!< end of file */
