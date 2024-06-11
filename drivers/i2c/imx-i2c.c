/*
 * Template of Character Device : I2C of imx6ull
 *
 * File Name:   imx_i2c.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.05.10
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/of/fwk_of.h>
#include <platform/of/fwk_of_device.h>
#include <platform/fwk_platdrv.h>
#include <platform/fwk_uaccess.h>
#include <platform/clk/fwk_clk.h>
#include <platform/i2c/fwk_i2c_dev.h>
#include <platform/i2c/fwk_i2c_core.h>
#include <platform/i2c/fwk_i2c_algo.h>
#include <kernel/mutex.h>
#include <kernel/wait.h>

#include <asm/imx6/imx6ull_pins.h>
#include <asm/imx6/imx6ull_periph.h>

/*!< The defines */
typedef struct imx_i2c_drv_data
{
	struct fwk_i2c_adapter sgrt_adap;
	void *reg;
    kuint32_t freq;

    struct fwk_clk *sprt_clk;
    kbool_t is_lastMsgs;

    kint32_t irq;
    kbool_t is_wake;

    struct wait_queue_head sgrt_wqh;
    struct mutex_lock sgrt_lock;

} srt_imx_i2c_drv_data_t;

/*!< 地址寄存器 */
struct imx_i2c_iar_reg
{
    __IO kuint16_t rw    : 1;                       /*!< bit 0 */
    __IO kuint16_t addr  : 7;                       /*!< bit 1:7 */
    __IO kuint16_t rsvd1 : 8;                       /*!< bit 8:15 */
};

/*!< 频率寄存器 */
struct imx_i2c_ifr_reg
{
    __IO kuint16_t freq  : 6;                       /*!< bit 0:5 */
    __IO kuint16_t rsvd1 : 2;                       /*!< bit 6:7 */
    __IO kuint16_t rsvd2 : 8;                       /*!< bit 8:15 */
};

/*!< 控制寄存器 */
struct imx_i2c_icr_reg
{
    __IO kuint16_t rsvd1 : 2;                       /*!< bit 0:1 保留 */
    __IO kuint16_t rsta  : 1;                       /*!< bit 2   重复开始信号: 1, 产生信号 */
    __IO kuint16_t txak  : 1;                       /*!< bit 3   传输应答使能: 0, ack; 1, noack */
    __IO kuint16_t mtx   : 1;                       /*!< bit 4   传输方向选择: 0, rx;  1, tx */
    __IO kuint16_t msta  : 1;                       /*!< bit 5   主从模式选择: 0, slave; 1, master */
    __IO kuint16_t iien  : 1;                       /*!< bit 6   中断使能, 为1有效 */
    __IO kuint16_t ien   : 1;                       /*!< bit 7   iic使能, 为1有效 */
    __IO kuint16_t rsvd2 : 8;                       /*!< bit 8:15 */
};

/*!< 状态寄存器 */
struct imx_i2c_isr_reg
{
    __IO kuint16_t rxak  : 1;                       /*!< bit 0   应答信号标志位: 0, ack; 1, noack */
    __IO kuint16_t iif   : 1;                       /*!< bit 1   iic中断挂起标志位, 为1有效 */
    __IO kuint16_t rsvd1 : 1;                       /*!< bit 2   保留 */
    __IO kuint16_t srw   : 1;                       /*!< bit 3   从机读写标志位: 0表示给从机写数据, 1表示读数据 */
    __IO kuint16_t ial   : 1;                       /*!< bit 4   仲裁丢失位, 为1有效 */
    __IO kuint16_t ibb   : 1;                       /*!< bit 5   iic总线忙标志位, 为1表示忙, 0表示空闲 */
    __IO kuint16_t iaas  : 1;                       /*!< bit 6   为1表示iar.addr为从设备地址 */
    __IO kuint16_t icf   : 1;                       /*!< bit 7   数据传输状态位: 0, 正在传输; 1, 传输完成 */
    __IO kuint16_t rsvd2 : 8;                       /*!< bit 8:15 */
};

/*!< 数据寄存器 */
struct imx_i2c_idr_reg
{
    __IO kuint16_t data  : 8;                       /*!< bit 0:7 */
    __IO kuint16_t rsvd1 : 8;                       /*!< bit 8:15 */
};

__align(4) struct imx_i2c_reg
{
	struct imx_i2c_iar_reg sgrt_iar;
    kuint8_t rsvd_0[2];

	struct imx_i2c_ifr_reg sgrt_ifr;
    kuint8_t rsvd_1[2];

	struct imx_i2c_icr_reg sgrt_icr;
    kuint8_t rsvd_2[2];

	struct imx_i2c_isr_reg sgrt_isr;
    kuint8_t rsvd_3[2];

	struct imx_i2c_idr_reg sgrt_idr;
};

enum __ERT_IMX_I2C_ACK_STATUS
{
    NR_I2C_ACK = 0,
    NR_I2C_NACK = 1,
};

/*!< The globals */
/*!<
 * PLL2 = 528 MHz
 * PLL2_PFD2 = 528 MHz * 18 / 24 = 396 MHz
 * IPG_CLK_ROOT = (PLL2_PFD2 / ahb_podf )/ ipg_podf = (396 MHz/3)/2 = 66MHz
 * PER_CLK_ROOT = IPG_CLK_ROOT/perclk_podf = 66MHz/1 = 66MHz
 * I2C clock Frequency = (PERCLK_ROOT frequency)/(division factor corresponding to IFDR)
 */
static const kuint16_t g_imx_i2c_ifdr_field[][2] =
{
    { 0x00, 30 },   { 0x10, 288 },  { 0x20, 22 },   { 0x30, 160 },
    { 0x01, 32 },   { 0x11, 320 },  { 0x21, 24 },   { 0x31, 192 },
    { 0x02, 36 },   { 0x12, 384 },  { 0x22, 26 },   { 0x32, 224 },
    { 0x03, 42 },   { 0x13, 480 },  { 0x23, 28 },   { 0x33, 256 },
    { 0x04, 48 },   { 0x14, 576 },  { 0x24, 32 },   { 0x34, 320 },
    { 0x05, 52 },   { 0x15, 640 },  { 0x25, 36 },   { 0x35, 384 },
    { 0x06, 60 },   { 0x16, 768 },  { 0x26, 40 },   { 0x36, 448 },
    { 0x07, 72 },   { 0x17, 960 },  { 0x27, 44 },   { 0x37, 512 },
    { 0x08, 80 },   { 0x18, 1152 }, { 0x28, 48 },   { 0x38, 640 },
    { 0x09, 88 },   { 0x19, 1280 }, { 0x29, 56 },   { 0x39, 768 },
    { 0x0a, 104 },  { 0x1a, 1536 }, { 0x2a, 64 },   { 0x3a, 896 },
    { 0x0b, 128 },  { 0x1b, 1920 }, { 0x2b, 72 },   { 0x3b, 1024 },
    { 0x0c, 144 },  { 0x1c, 2304 }, { 0x2c, 80 },   { 0x3c, 1280 },
    { 0x0d, 160 },  { 0x1d, 2560 }, { 0x2d, 96 },   { 0x3d, 1536 },
    { 0x0e, 192 },  { 0x1e, 3072 }, { 0x2e, 112 },  { 0x3e, 1792 },
    { 0x0f, 240 },  { 0x1f, 3840 }, { 0x2f, 128 },  { 0x3f, 2048 },
};

/*!< API function */
static kint16_t imx_i2c_find_frequency(kuint32_t freq)
{
    kusize_t num_field = ARRAY_SIZE(g_imx_i2c_ifdr_field);
    kuint16_t div, field, last_temp, cur_temp; 
    kuint16_t idx, idx_satisfy;

    /*!< ipg_clk: 66MHz */
    div = 66000000 / freq;
    last_temp = (kuint16_t)(~0);
    idx_satisfy = last_temp;

    for (idx = 0; idx < num_field; idx++)
    {
        field = g_imx_i2c_ifdr_field[idx][1];
        if (field == div)
        {
            idx_satisfy = idx;
            break;
        }

        cur_temp = mrt_usub(field, div);
        if (cur_temp < last_temp)
        {
            last_temp = cur_temp;
            idx_satisfy = idx;
        }
    }

    if (idx_satisfy < num_field)
        return g_imx_i2c_ifdr_field[idx_satisfy][0];

    return -ER_NOTFOUND;
}

static void imx_i2c_adap_set_ack(struct imx_i2c_reg *sprt_i2c, kuint32_t ack)
{
    if (ack == NR_I2C_NACK)
        sprt_i2c->sgrt_icr.txak = true;
    else if (ack == NR_I2C_ACK)
        sprt_i2c->sgrt_icr.txak = false;
}

static kuint32_t imx_i2c_adap_get_ack(struct imx_i2c_reg *sprt_i2c)
{
    return sprt_i2c->sgrt_isr.rxak ? NR_I2C_NACK : NR_I2C_ACK;
}

static void imx_i2c_adap_write_data(struct imx_i2c_reg *sprt_i2c, kuint8_t data)
{
    sprt_i2c->sgrt_idr.data = data;
}

static kuint8_t imx_i2c_adap_read_data(struct imx_i2c_reg *sprt_i2c)
{
    return sprt_i2c->sgrt_idr.data;
}

static kbool_t imx_i2c_adap_wait_complete(struct fwk_i2c_adapter *sprt_adap, kuint32_t timeout)
{
    struct imx_i2c_drv_data *sprt_data;
    struct imx_i2c_reg *sprt_i2c;

	sprt_data = fwk_i2c_adapter_get_drvdata(sprt_adap);
	sprt_i2c = (struct imx_i2c_reg *)sprt_data->reg;

    wait_event_interruptible_timeout(&sprt_data->sgrt_wqh, 
                                    sprt_i2c->sgrt_isr.icf && (sprt_i2c->sgrt_isr.iif || sprt_data->is_wake), 
                                    timeout);
    sprt_data->is_wake = false;

    return true;
}

static kint32_t imx_i2c_adap_for_busy(struct imx_i2c_reg *sprt_i2c, kbool_t ways)
{
    kutime_t expires = jiffies + msecs_to_jiffies(500);

    for (;;)
    {
        /*!< ways = 1: i2c is running, bus should be busy */
        if (ways && sprt_i2c->sgrt_isr.ibb)
            break;

        /*!< ways = 0: i2c is stopped, bus should be idle */
        if (!ways && !sprt_i2c->sgrt_isr.ibb)
            break;

        if (mrt_time_after(jiffies, expires))
            return -ER_TIMEOUT;

        schedule_thread();
    }

    return ER_NORMAL;
}

static void imx_i2c_adap_clear_intr(struct imx_i2c_reg *sprt_i2c)
{
    sprt_i2c->sgrt_isr.iif = false;
}

/*!
 * @brief  imx_i2c_adap_check_ack
 * @param  None
 * @retval None
 * @note   iic功能检查
 */
static kbool_t imx_i2c_adap_check_ack(struct imx_i2c_reg *sprt_i2c)
{
    /*!< 1. 仲裁丢失位检查 */
    if (sprt_i2c->sgrt_isr.ial)
    {
        sprt_i2c->sgrt_isr.ial = false;

        /*!< 应重启iic */
        sprt_i2c->sgrt_icr.ien = false;
        sprt_i2c->sgrt_icr.ien = true;

        return false;
    }

    /*!< 2. 无应答 */
    if (NR_I2C_NACK == imx_i2c_adap_get_ack(sprt_i2c))
        return false;

    return true;
}

/*!
 * @brief  imx_i2c_adap_start
 * @param  None
 * @retval None
 * @note   iic起始位
 * 总线空闲时, SCL & SDA均为高电平, 此时若SDA产生下降沿, 表示iic开始
 */
static kint32_t imx_i2c_adap_start(struct fwk_i2c_adapter *sprt_adap)
{
    struct imx_i2c_drv_data *sprt_data;
    struct imx_i2c_reg *sprt_i2c;

    sprt_data = fwk_i2c_adapter_get_drvdata(sprt_adap);
    sprt_i2c = (struct imx_i2c_reg *)sprt_data->reg;

    /*!< open clock */
    fwk_clk_prepare_enable(sprt_data->sprt_clk);

    sprt_i2c->sgrt_isr.iif = false;
    sprt_i2c->sgrt_isr.ial = false;

    sprt_i2c->sgrt_icr.ien = true;
    mrt_resetw(&sprt_i2c->sgrt_isr);

    delay_us(100);
    
    /*!< 设置iic工作在主机模式 */
    sprt_i2c->sgrt_icr.msta = true;
    
    /*!< 判断iic总线是否处于忙状态. 空闲: 0, 忙: 1 */
    if (imx_i2c_adap_for_busy(sprt_i2c, true))
        goto fail;

    /*!< set direction to "send" */
    sprt_i2c->sgrt_icr.mtx  = true;
//  sprt_i2c->sgrt_icr.iien = true;

    imx_i2c_adap_set_ack(sprt_i2c, NR_I2C_NACK);

    /*!< 等待传输完成 */
//  if (!imx_i2c_adap_wait_complete(sprt_adap, 100))
//        goto fail;

    return ER_NORMAL;

fail:
    fwk_clk_disable_unprepare(sprt_data->sprt_clk);
    return -ER_NREADY;
}

/*!
 * @brief  imx_i2c_adap_restart
 * @param  None
 * @retval None
 * @note   iic重新开始信号
 * 总线空闲时, SCL & SDA均为高电平, 此时若SDA产生下降沿, 表示iic开始
 */
static kint32_t imx_i2c_adap_restart(struct fwk_i2c_adapter *sprt_adap)
{
    struct imx_i2c_drv_data *sprt_data;
    struct imx_i2c_reg *sprt_i2c;

	sprt_data = fwk_i2c_adapter_get_drvdata(sprt_adap);
	sprt_i2c = (struct imx_i2c_reg *)sprt_data->reg;

    /*!< only master mode can send restart signal */
    if (!sprt_i2c->sgrt_icr.msta)
        return -ER_CHECKERR;

    /*!< if i2c is running, bus will be busy; otherwise, i2c bus is stopped */
    if (imx_i2c_adap_for_busy(sprt_i2c, true))
        return -ER_WILDPTR;

    /*!< set restart */
    sprt_i2c->sgrt_icr.rsta = true;
    delay_us(100);

    return ER_NORMAL;
}

/*!
 * @brief  imx_i2c_adap_stop
 * @param  None
 * @retval None
 * @note   iic停止位
 * 总线工作时, SCL低电平时SDA变化; 若SCL高电平时SDA变化(上升沿), 表示iic停止;
 * 此时SCL & SDA回到高电平状态, 总线恢复空闲
 */
static void imx_i2c_adap_stop(struct fwk_i2c_adapter *sprt_adap)
{    
    struct imx_i2c_drv_data *sprt_data;
    struct imx_i2c_reg *sprt_i2c;

	sprt_data = fwk_i2c_adapter_get_drvdata(sprt_adap);
	sprt_i2c = (struct imx_i2c_reg *)sprt_data->reg;

    /*!< 清除设置 */
    sprt_i2c->sgrt_icr.msta = false;
    sprt_i2c->sgrt_icr.mtx  = false;
    sprt_i2c->sgrt_icr.txak = false;

    /*!< 等待忙结束: 循环比较费时间 */
    imx_i2c_adap_for_busy(sprt_i2c, false);

    sprt_i2c->sgrt_icr.ien = false;
    sprt_i2c->sgrt_icr.iien = false;

    fwk_clk_disable_unprepare(sprt_data->sprt_clk);
}

/*!
 * @brief  imx_i2c_adap_write
 * @param  None
 * @retval None
 * @note   iic写时序
 */
static kint32_t imx_i2c_adap_write(struct imx_i2c_drv_data *sprt_data, kuint16_t slave, const kubuffer_t *buffer, kusize_t size)
{
	struct imx_i2c_reg *sprt_i2c;

	sprt_i2c = (struct imx_i2c_reg *)sprt_data->reg;

    /*!< 设置传输方向为发送 */
    sprt_i2c->sgrt_icr.mtx = true;

    imx_i2c_adap_write_data(sprt_i2c, slave << 1);
    if (!imx_i2c_adap_wait_complete(&sprt_data->sgrt_adap, 100))
        return -ER_TIMEOUT;

    /* 手动清除中断标志位 */
    imx_i2c_adap_clear_intr(sprt_i2c);

    if (!imx_i2c_adap_check_ack(sprt_i2c))
        return -ER_RDATA_FAILD;

    while (size--)
    {
        imx_i2c_adap_write_data(sprt_i2c, *buffer++);

        /*!< 等待数据传输完成 */
        if (!imx_i2c_adap_wait_complete(&sprt_data->sgrt_adap, 100))
            return -ER_TIMEOUT;

        imx_i2c_adap_clear_intr(sprt_i2c);

        if (!imx_i2c_adap_check_ack(sprt_i2c))
            return -ER_RDATA_FAILD;
    }

    return ER_NORMAL;
}

/*!
 * @brief  imx_i2c_adap_read
 * @param  None
 * @retval None
 * @note   iic读时序
 */
static kint32_t imx_i2c_adap_read(struct imx_i2c_drv_data *sprt_data, kuint16_t slave, kubuffer_t *buffer, kusize_t size)
{
	struct imx_i2c_reg *sprt_i2c;
    kuint32_t count;

	sprt_i2c = (struct imx_i2c_reg *)sprt_data->reg;

    /*!< 设置传输方向为发送 */
    sprt_i2c->sgrt_icr.mtx = true;

    imx_i2c_adap_write_data(sprt_i2c, (slave << 1) | 0x01);
    if (!imx_i2c_adap_wait_complete(&sprt_data->sgrt_adap, 100))
        return -ER_TIMEOUT;

    /*!< 清除中断标志位 */
    imx_i2c_adap_clear_intr(sprt_i2c);

    if (!imx_i2c_adap_check_ack(sprt_i2c))
        return -ER_RDATA_FAILD;

    imx_i2c_adap_set_ack(sprt_i2c, (size > 1) ? NR_I2C_ACK : NR_I2C_NACK);

    /*!< 设置传输方向为接收 */
    sprt_i2c->sgrt_icr.mtx = false;

    /*!< 空读数据, 可清除data里面的内容 */
    imx_i2c_adap_read_data(sprt_i2c);

    for (count = 0; count < size; count++)
    {
        /*!< 等待数据传输完成 */
        if (!imx_i2c_adap_wait_complete(&sprt_data->sgrt_adap, 100))
            return -ER_TIMEOUT;

        imx_i2c_adap_clear_intr(sprt_i2c);

        /*!< 当只剩最后一个数据时, 应终止读, 以防i2c时钟错乱 */
        if (count == (size - 1))
        {
            if (!sprt_data->is_lastMsgs)
                sprt_i2c->sgrt_icr.mtx = true;
            else
            {
                /*!< 停止 */
                sprt_i2c->sgrt_icr.mtx = false;
                sprt_i2c->sgrt_icr.msta = false;

                if (imx_i2c_adap_for_busy(sprt_i2c, false))
                    return -ER_BUSY;
            }
        }
        /*!< 倒数第2个数据, 发送nack */
        else if (count == (size - 2))
            imx_i2c_adap_set_ack(sprt_i2c, NR_I2C_NACK);
        else
            imx_i2c_adap_set_ack(sprt_i2c, NR_I2C_ACK);

        *(buffer++) = imx_i2c_adap_read_data(sprt_i2c);
    }

    return ER_NORMAL;
}

static kint32_t __imx_i2c_adap_xfer(struct fwk_i2c_adapter *sprt_adap, struct fwk_i2c_msg *sprt_msg)
{
	struct imx_i2c_drv_data *sprt_data;

	sprt_data = fwk_i2c_adapter_get_drvdata(sprt_adap);

    if (sprt_msg->flags & FWK_I2C_M_RD)
    {
        if (imx_i2c_adap_read(sprt_data, sprt_msg->addr, sprt_msg->ptr_buf, sprt_msg->len))
            return -ER_TRXERR;
    }
    else
        imx_i2c_adap_write(sprt_data, sprt_msg->addr, sprt_msg->ptr_buf, sprt_msg->len);

    return ER_NORMAL;
}

static irq_return_t imx_i2c_adap_isr(void *ptrDev)
{
    struct imx_i2c_drv_data *sprt_data;
    struct imx_i2c_reg *sprt_i2c;

	sprt_data = fwk_i2c_adapter_get_drvdata(ptrDev);
	sprt_i2c = (struct imx_i2c_reg *)sprt_data->reg;

    if (sprt_i2c->sgrt_isr.iif)
    {
        sprt_data->is_wake = true;
        imx_i2c_adap_clear_intr(sprt_i2c);

        wake_up_interruptible(&sprt_data->sgrt_wqh);
    }

    return 0;
}

/*!
 * @brief  imx_i2c_adap_initial
 * @param  None
 * @retval None
 * @note   初始化iic
 */
static void imx_i2c_adap_initial(struct fwk_i2c_adapter *sprt_adap)
{
    struct imx_i2c_drv_data *sprt_data;
    struct imx_i2c_reg *sprt_i2c;
    kint16_t field;

	sprt_data = fwk_i2c_adapter_get_drvdata(sprt_adap);
	sprt_i2c = (struct imx_i2c_reg *)sprt_data->reg;

    /*!< 关闭iic */
    sprt_i2c->sgrt_icr.ien = false;

    /*!< write zero to clear */
    mrt_resetw(&sprt_i2c->sgrt_isr);

    /*!< config frequency */
    field = imx_i2c_find_frequency(sprt_data->freq);
    if (field < 0)
        return;

    sprt_i2c->sgrt_ifr.freq = field;

    /*!< 启用iic */
    sprt_i2c->sgrt_icr.ien = true;
}

static kint32_t imx_i2c_adap_xfer(struct fwk_i2c_adapter *sprt_adap, struct fwk_i2c_msg *sprt_msgs, kint32_t num)
{
	struct imx_i2c_drv_data *sprt_data;
    kuint32_t idx;
    kint32_t retval;

	sprt_data = fwk_i2c_adapter_get_drvdata(sprt_adap);

    mutex_lock(&sprt_data->sgrt_lock);

    /*!< 开始信号 */
    retval = imx_i2c_adap_start(sprt_adap);
    if (retval)
        goto out;

    for (idx = 0; idx < num; idx++) 
    {
        sprt_data->is_lastMsgs = ((idx + 1) == num);

        if (idx)
        {
            /*!< 重新开始 */
            retval = imx_i2c_adap_restart(sprt_adap);
            if (retval)
                goto END;
        }

        retval = __imx_i2c_adap_xfer(sprt_adap, sprt_msgs + idx);
        if (retval < 0)
            goto END;
    }

END:
    /*!< 停止信号 */
    imx_i2c_adap_stop(sprt_adap);
    sprt_data->is_lastMsgs = false;

out:
    mutex_unlock(&sprt_data->sgrt_lock);
    return retval;
}

static const struct fwk_i2c_algo sgrt_imx_i2c_algo = 
{
	.master_xfer = imx_i2c_adap_xfer,
};

/*!< --------------------------------------------------------------------- */
/*!
 * @brief   imx_i2c_driver_probe
 * @param   sprt_dev
 * @retval  errno
 * @note    none
 */
static kint32_t imx_i2c_driver_probe(struct fwk_platdev *sprt_pdev)
{
	struct imx_i2c_drv_data *sprt_data;
	struct fwk_i2c_adapter *sprt_adap;
	struct fwk_device *sprt_dev;
	kuaddr_t reg;
	kint32_t retval;

	sprt_data = kzalloc(sizeof(*sprt_data), GFP_KERNEL);
	if (!isValid(sprt_data))
		return -ER_NOMEM;

	sprt_dev = &sprt_pdev->sgrt_dev;
	sprt_adap = &sprt_data->sgrt_adap;
    mutex_init(&sprt_data->sgrt_lock);

	reg = fwk_platform_get_address(sprt_pdev, 0);
	sprt_data->reg = fwk_io_remap((void *)reg);
	if (!sprt_data->reg)
		goto fail1;

    sprt_data->irq = fwk_platform_get_irq(sprt_pdev, 0);
    if (sprt_data->irq < 0)
        goto fail2;

    retval = fwk_of_property_read_u32(sprt_pdev->sgrt_dev.sprt_node, "clock_frequency", &sprt_data->freq);
    if (retval || !sprt_data->freq)
        sprt_data->freq = 100000;

    sprt_data->sprt_clk = fwk_clk_get(sprt_dev, mrt_nullptr);
    if (!sprt_data->sprt_clk)
        goto fail2;

	sprt_adap->sgrt_dev.sprt_parent = sprt_dev;
	sprt_adap->sgrt_dev.sprt_node = sprt_dev->sprt_node;

	sprt_adap->sprt_algo = &sgrt_imx_i2c_algo;
	sprt_adap->algo_data = sprt_data;
	sprt_adap->id = (sprt_pdev->id < 0) ? fwk_of_get_alias_id(sprt_dev->sprt_node) : sprt_pdev->id;
	sprintk(sprt_adap->name, "imx,i2c-%d", sprt_adap->id);
    init_waitqueue_head(&sprt_data->sgrt_wqh);

	retval = fwk_i2c_add_adapter(sprt_adap);
	if (retval < 0)
		goto fail3;

	fwk_platform_set_drvdata(sprt_pdev, sprt_data);
    fwk_i2c_adapter_set_drvdata(sprt_adap, sprt_data);

    fwk_clk_prepare_enable(sprt_data->sprt_clk);
    imx_i2c_adap_initial(sprt_adap);
    fwk_clk_disable_unprepare(sprt_data->sprt_clk);

    if (fwk_request_irq(sprt_data->irq, imx_i2c_adap_isr, IRQ_TYPE_NONE, "imx,i2c", sprt_adap))
        goto fail3;

	return ER_NORMAL;

fail3:
    fwk_clk_put(sprt_data->sprt_clk);
fail2:
	fwk_io_unmap(sprt_data->reg);
fail1:
	kfree(sprt_data);

	return -ER_ERROR;
}

/*!
 * @brief   imx_i2c_driver_remove
 * @param   sprt_dev
 * @retval  errno
 * @note    none
 */
static kint32_t imx_i2c_driver_remove(struct fwk_platdev *sprt_pdev)
{
	struct imx_i2c_drv_data *sprt_data;

    sprt_data = fwk_platform_get_drvdata(sprt_pdev);
    if (!sprt_data)
        return ER_NORMAL;

    fwk_free_irq(sprt_data->irq, &sprt_data->sgrt_adap);
    fwk_i2c_del_adapter(&sprt_data->sgrt_adap);
    fwk_clk_disable_unprepare(sprt_data->sprt_clk);
    fwk_clk_put(sprt_data->sprt_clk);
    fwk_io_unmap(sprt_data->reg);
    kfree(sprt_data);
    fwk_platform_set_drvdata(sprt_pdev, mrt_nullptr);

	return ER_NORMAL;
}

/*!< device id for device-tree */
static const struct fwk_of_device_id sgrt_imx_i2c_driver_ids[] =
{
	{ .compatible = "fsl,imx6ul-i2c", },
	{},
};

/*!< platform instance */
static struct fwk_platdrv sgrt_imx_i2c_platdriver =
{
	.probe	= imx_i2c_driver_probe,
	.remove	= imx_i2c_driver_remove,
	
	.sgrt_driver =
	{
		.name 	= "fsl, i2c",
		.id 	= -1,
		.sprt_of_match_table = sgrt_imx_i2c_driver_ids,
	},
};

/*!< --------------------------------------------------------------------- */
/*!
 * @brief   imx_i2c_driver_init
 * @param   none
 * @retval  errno
 * @note    none
 */
kint32_t __fwk_init imx_i2c_driver_init(void)
{
	return fwk_register_platdriver(&sgrt_imx_i2c_platdriver);
}

/*!
 * @brief   imx_i2c_driver_exit
 * @param   none
 * @retval  none
 * @note    none
 */
void __fwk_exit imx_i2c_driver_exit(void)
{
	fwk_unregister_platdriver(&sgrt_imx_i2c_platdriver);
}

IMPORT_PATTERN_INIT(imx_i2c_driver_init);
IMPORT_PATTERN_EXIT(imx_i2c_driver_exit);

/*!< end of file */
