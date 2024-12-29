/*
 * Gloabl Definition For R/W
 *
 * File Name:   io_stream.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.09.28
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __IO_STREAM_H
#define __IO_STREAM_H

/*!< The includes */
#include <common/generic.h>

/*!< The defines */
#define __IO                                            volatile
#define __IOW                                           volatile
#define __IOR                                           volatile const

/*!< defines a general union format */
#define UNION_SRT_FORMAT_DECLARE(urt_name, srt_name)  \
typedef union urt_name  \
{   \
    kuint32_t all;  \
    srt_name  bit;  \
} urt_name##_t;

#define mrt_trans_urt_data(urt)                     	((urt)->all)
#define mrt_reset_urt_data(urt)                			((urt)->all = 0U)
#define mrt_write_urt_data(urt, data)          			((urt)->all = (data))
#define mrt_trans_urt_bits(urt, member)  			    ((urt)->bit.member)
#define mrt_write_urt_bits(urt, member, data)  			((urt)->bit.member = (data))

/*!< for a word */
#define mrt_writel(data, addr)                          do { *((volatile kuint32_t *)(addr)) = (kuint32_t)(data); } while (0)
#define mrt_readl(addr)                                 ( (*((volatile kuint32_t *)(addr))))
#define mrt_resetl(addr)                                do { *((volatile kuint32_t *)(addr)) = 0U; } while (0)
#define mrt_setbitl(bit, addr)                          do { *((volatile kuint32_t *)(addr)) |= (kuint32_t)(bit); } while (0)
#define mrt_clrbitl(bit, addr)                          do { *((volatile kuint32_t *)(addr)) &= ~((kuint32_t)(bit)); } while (0)
#define mrt_getbitl(bit, addr)                          ( (*((volatile kuint32_t *)(addr)) & ((kuint32_t)(bit))))
#define mrt_getbit_u32(mask, nr, addr)                  ( (mrt_getbitl(mask, addr) >> (nr)))
#define mrt_setfieldl(bit, mask, addr)  \
do {   \
    mrt_clrbitl(mask, addr);    \
    mrt_setbitl(bit, addr); \
} while (0)

/*!< for a half word */
#define mrt_writew(data, addr)                          do { *((volatile kuint16_t *)(addr)) = (kuint16_t)(data); } while (0)
#define mrt_readw(addr)                                 ( (*((volatile kuint16_t *)(addr))))
#define mrt_resetw(addr)                                do { *((volatile kuint16_t *)(addr)) = 0U; } while (0)
#define mrt_setbitw(bit, addr)                          do { *((volatile kuint16_t *)(addr)) |= (kuint16_t)(bit); } while (0)
#define mrt_clrbitw(bit, addr)                          do { *((volatile kuint16_t *)(addr)) &= ~((kuint16_t)(bit)); } while (0)
#define mrt_getbitw(bit, addr)                          ( (*((volatile kuint16_t *)(addr)) & ((kuint16_t)(bit))))
#define mrt_getbit_u16(mask, nr, addr)                  ( (mrt_getbitw(mask, addr) >> (nr)))
#define mrt_setfieldw(bit, mask, addr)  \
{   \
    mrt_clrbitw(mask, addr);    \
    mrt_setbitw(bit, addr); \
}

/*!< for a byte */
#define mrt_writeb(data, addr)                          do { *((volatile kuint8_t *)(addr)) = (kuint8_t)(data); } while (0)
#define mrt_readb(addr)                                 ( (*((volatile kuint8_t *)(addr))))
#define mrt_resetb(addr)                                do { *((volatile kuint8_t *)(addr)) = 0U; } while (0)
#define mrt_setbitb(bit, addr)                          do { *((volatile kuint8_t *)(addr)) |= (kuint8_t)(bit); } while (0)
#define mrt_clrbitb(bit, addr)                          do { *((volatile kuint8_t *)(addr)) &= ~((kuint8_t)(bit)); } while (0)
#define mrt_getbitb(bit, addr)                          ( (*((volatile kuint8_t *)(addr)) & ((kuint8_t)(bit))))
#define mrt_getbit_u8(mask, nr, addr)                   ( (mrt_getbitb(mask, addr) >> (nr)))
#define mrt_setfieldb(bit, mask, addr)  \
{   \
    mrt_clrbitb(mask, addr);    \
    mrt_setbitb(bit, addr); \
}

/*!
 * @note mrt_isBitSetl: like mrt_getbitl(0x01 | 0x02, addr) = 0x02, it is not equaling to (0x01 | 0x02), and (!=0)
 * @note mrt_isBitResetl: mrt_getbitl(0x01 | 0x02, addr) must equal to zero.
 */
#define mrt_isBitSetl(bit, addr)                        ( (bit) == mrt_getbitl(bit, addr))
#define mrt_isBitResetl(bit, addr)                      ( 0U == mrt_getbitl(bit, addr))
#define mrt_isBitSetw(bit, addr)                        ( (bit) == mrt_getbitw(bit, addr))
#define mrt_isBitResetw(bit, addr)                      ( 0U == mrt_getbitw(bit, addr))
#define mrt_isBitSetb(bit, addr)                        ( (bit) == mrt_getbitb(bit, addr))
#define mrt_isBitResetb(bit, addr)                      ( 0U == mrt_getbitb(bit, addr))

/*!< set bit to 32-bit array */
#define mrt_setbit_towords(integer, addr)	\
do {   \
    *((volatile kuint32_t *)(addr) + mrt_word_offset(integer)) |= mrt_bit(mrt_bit_offset(integer)); \
} while (0)

/*!< clear bit to 32-bit array */
#define mrt_clrbit_towords(integer, addr)	\
do {   \
    *((volatile kuint32_t *)(addr) + mrt_word_offset(integer)) &= ~mrt_bit(mrt_bit_offset(integer));    \
} while (0)

/*!< git bit from 32-bit array */
#define mrt_getbit_fromwords(integer, addr)	\
    (*((volatile kuint32_t *)(addr) + mrt_word_offset(integer)) & mrt_bit(mrt_bit_offset(integer)))

#define mrt_isBitSetWords(integer, addr)                ( mrt_bit(mrt_bit_offset(integer)) == mrt_getbit_fromwords(bit, addr))
#define mrt_isBitResetWords(integer, addr)              ( 0U == mrt_getbit_fromwords(bit, addr))

/*!< print levels */
#define PRINT_LEVEL_SOH                                 "\001"
#define PRINT_LEVEL_EMERG                               PRINT_LEVEL_SOH "0"
#define PRINT_LEVEL_ALERT                               PRINT_LEVEL_SOH "1"
#define PRINT_LEVEL_CRIT                                PRINT_LEVEL_SOH "2"
#define PRINT_LEVEL_ERR                                 PRINT_LEVEL_SOH "3"
#define PRINT_LEVEL_WARNING                             PRINT_LEVEL_SOH "4"
#define PRINT_LEVEL_NOTICE                              PRINT_LEVEL_SOH "5"
#define PRINT_LEVEL_INFO                                PRINT_LEVEL_SOH "6"
#define PRINT_LEVEL_DEBUG                               PRINT_LEVEL_SOH "7"

/*!< The functions */
TARGET_EXT void io_putc(const kubyte_t ch);
TARGET_EXT void io_putstr(const kubyte_t *msgs, kusize_t size);
TARGET_EXT kubyte_t io_getc(kubyte_t *ch);
TARGET_EXT kssize_t io_getstr(kubyte_t *msgs, kusize_t size);
TARGET_EXT void printk(const kchar_t *ptr_fmt, ...);

#define print_err(fmt, ...)                             printk(PRINT_LEVEL_ERR fmt, ##__VA_ARGS__)
#define print_warn(fmt, ...)                            printk(PRINT_LEVEL_WARNING fmt, ##__VA_ARGS__)
#define print_info(fmt, ...)                            printk(PRINT_LEVEL_INFO fmt, ##__VA_ARGS__)
#define print_debug(fmt, ...)                           printk(PRINT_LEVEL_DEBUG fmt, ##__VA_ARGS__)

#define ERR_OUT(flag, fmt, ...)   \
    do {    \
        if (!!(flag))    \
            print_err("%s: %d: " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);   \
    } while (0)

#define WARN_OUT(flag, fmt, ...)    \
    do {    \
        if (!!(flag))    \
            print_warn("%s: %d: " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);   \
    } while (0)

#define INFO_OUT(flag, fmt, ...)    \
    do {    \
        if (!!(flag))    \
            print_info("%s: %d: " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);   \
    } while (0)

#define DEBUG_OUT(flag, fmt, ...)   \
    do {    \
        if (!!(flag))    \
            print_debug("%s: %d: " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__);   \
    } while (0)

TARGET_EXT kint32_t bitmap_find_first_zero_bit(void *bitmap, kuint32_t start, kusize_t total_bits);
TARGET_EXT kint32_t bitmap_find_first_valid_bit(void *bitmap, kuint32_t start, kusize_t total_bits);
TARGET_EXT kint32_t bitmap_find_nr_zero_bit(void *bitmap, kuint32_t start, kusize_t total_bits, kuint32_t nr);
TARGET_EXT kint32_t bitmap_find_nr_valid_bit(void *bitmap, kuint32_t start, kusize_t total_bits, kuint32_t nr);
TARGET_EXT void bitmap_set_nr_bit_zero(void *bitmap, kuint32_t start, kusize_t total_bits, kuint32_t nr);
TARGET_EXT void bitmap_set_nr_bit_valid(void *bitmap, kuint32_t start, kusize_t total_bits, kuint32_t nr);

/*!< API function */
/*!
 * @brief   api_bit_mask
 * @param   index
 * @retval  bit
 * @note    tranform: val to bit; for example, number 4 is equaled to bit 4
 */
__force_inline static inline kuint32_t api_bit_mask(kuint8_t index)
{
	kuint32_t result;

	__asm__ __volatile__ (
		" mov %0, #1		\n\t"
		" and %1, #0x1f		\n\t"
		" lsl %0, %0, %1	\n\t"
		: "=&r"(result)
		: "r"(index)
		: "cc"
	);

    return result;
}

/*!
 * @brief   setbit_to_words
 * @param   nr, addr
 * @retval  none
 * @note    set bit to array
 */
static inline void setbit_to_words(kuint32_t nr, kuint32_t *addr)
{
	kuint32_t pos = (nr >> 5);
    kuint32_t *p_addr = addr + pos;

    pos = api_bit_mask(nr);
    mrt_setbitl(pos, p_addr);
}

/*!
 * @brief   clrbit_to_words
 * @param   nr, addr
 * @retval  none
 * @note    reset bit to array
 */
static inline void clrbit_to_words(kuint32_t nr, kuint32_t *addr)
{
	kuint32_t pos = (nr >> 5);
    kuint32_t *p_addr = addr + pos;

    pos = api_bit_mask(nr);
    mrt_clrbitl(pos, p_addr);
}

/*!
 * @brief   getbit_from_words
 * @param   lenth: the number of bits
 * @retval  none
 * @note    get bit from array
 */
static inline kuint32_t getbit_from_words(kuint32_t nr, kuint32_t lenth, kuint32_t *addr)
{
    kuint32_t pos, result = 0U;
    kuint32_t *p_addr;

    while (lenth--)
    {
        pos = (nr + lenth - 1) >> 5;
        p_addr = addr + pos;
        pos = api_bit_mask(nr + lenth - 1);
        
        result <<= 1;
        result |= mrt_isBitSetl(pos, p_addr);
    }

    return result;
}

/*!
 * @brief   io_writel
 * @param   none
 * @retval  none
 * @note    write SoC register
 */
static inline void io_writel(kuaddr_t addr, kuint32_t val)
{
    *((volatile kuaddr_t *)addr) = val;
}

/*!
 * @brief   io_readl
 * @param   none
 * @retval  none
 * @note    read SoC register
 */
static inline kuint32_t io_readl(kuaddr_t addr, kuint32_t *val)
{
    if (val)
        *val = *((kuaddr_t *)addr);

    return *((kuaddr_t *)addr);
}

/*!
 * @brief   io_setbit
 * @param   none
 * @retval  none
 * @note    write SoC register
 */
static inline void io_setbit(kuaddr_t addr, kuint32_t val)
{
    *((volatile kuaddr_t *)addr) |= val;
}

/*!
 * @brief   io_clrbit
 * @param   none
 * @retval  none
 * @note    write SoC register
 */
static inline void io_clrbit(kuaddr_t addr, kuint32_t val)
{
    *((volatile kuaddr_t *)addr) &= ~val;
}

#endif /* __IO_STREAM_H */
