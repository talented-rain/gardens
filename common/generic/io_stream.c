/*
 * IO Stream General Function
 *
 * File Name:   io_stream.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.09.28
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <configs/configs.h>
#include <common/io_stream.h>
#include <common/api_string.h>
#include <common/mem_manage.h>
#include <kernel/kernel.h>
#include <kernel/spinlock.h>
#include <platform/fwk_mempool.h>

/*!< The globals */

/*!< API function */
/*!
 * @brief   io_putc
 * @param   ch
 * @retval  none
 * @note    character output
 */
__weak void io_putc(const kubyte_t ch)
{

}

/*!
 * @brief   io_putstr
 * @param   string
 * @retval  none
 * @note    string output
 */
__weak void io_putstr(const kubyte_t *msgs, kusize_t size)
{

}

/*!
 * @brief   io_getc
 * @param   ch
 * @retval  none
 * @note    character read
 */
__weak kubyte_t io_getc(kubyte_t *ch)
{
    return 0;
}

/*!
 * @brief   io_getstr
 * @param   string
 * @retval  none
 * @note    string read
 */
__weak kssize_t io_getstr(kubyte_t *msgs, kusize_t size)
{
    return 0;
}

/*!
 * @brief   printk
 * @param   ptr_fmt
 * @retval  none
 * @note    string output
 */
void printk(const kchar_t *ptr_fmt, ...)
{
#if defined(CONFIG_PRINT_LEVEL)
    va_list ptr_list;
    kubyte_t *ptr_buf, level[2] = {};
    kusize_t size;
//  kuint32_t i;

    va_start(ptr_list, ptr_fmt);
    ptr_buf = (kubyte_t *)lv_vasprintk(ptr_fmt, &size, level, ptr_list);
    va_end(ptr_list);

    if (!isValid(ptr_buf))
        return;

    /*!< if level is not set, default PRINT_LEVEL_WARNING */
    if (*(PRINT_LEVEL_SOH) != *level)
        memcpy(level, PRINT_LEVEL_WARNING, sizeof(level));

    if (*(level + 1) > *((kubyte_t *)(CONFIG_PRINT_LEVEL)))
        return;

//	for (i = 0; i < size; i++)
//		io_putc(*(ptr_buf + i));

    io_putstr(ptr_buf, size);
    kfree(ptr_buf);
#endif
}

/*!
 * @brief   find first bit that equaled to value
 * @param   bitmap: array
 * @param	start: base
 * @param	total_bit: bitmap lenth
 * @param	value: 0 or 1
 * @retval  index (bit position); -1: not found
 * @note    find bit on bitmap
 */
static kint32_t bitmap_find_first_bit(kuint8_t *bitmap, kuint32_t start, kusize_t total_bits, kbool_t value)
{
    kuint32_t index, bit_mask, area_mask;
    kuint32_t bit_per_map = sizeof(*bitmap) << 3;

    /*!< search by per 8 bits */
    for (index = start; index < total_bits; index++)
    {
        area_mask = mrt_udiv(index, bit_per_map);
        bit_mask = mrt_urem(index, bit_per_map);

        if ((!!(*(bitmap + area_mask) & (1 << bit_mask))) == value)
            break;
    }

    return (index >= total_bits) ? -1 : index;
}

/*!
 * @brief   request multiple bits that equaled to value
 * @param   bitmap: array
 * @param	start: base
 * @param	total_bit: bitmap lenth
 * @param	nr: count, index ~ (index + nr)
 * @param	value: 0 or 1
 * @retval  0: request successfully; > 0: the first bit failed
 * @note    request multiple consecutive bits on bitmap
 */
static kint32_t bitmap_find_nr_bit(kuint8_t *bitmap, kuint32_t start, kusize_t total_bits, kuint32_t nr, kbool_t value)
{
    kuint32_t end = start + nr;
    kint32_t index;

    if (end >= total_bits)
        return -1;
    
    /*!<
     * found (!value), return current index (the first bit that does not meet the condition)
     * indicate that no consecutive bits are avaliable, i.e request nr bits failed
     */
    index = bitmap_find_first_bit((kuint8_t *)bitmap, start, end, !value);
    return (index < 0) ? 0 : index;
}

/*!
 * @brief   set start ~ (start + nr) to value (0 or 1)
 * @param   ...
 * @retval  none
 * @note    (start + nr) < total_bits
 */
static void bitmap_set_nr_bit(kuint8_t *bitmap, kuint32_t start, kusize_t total_bits, kuint32_t nr, kbool_t value)
{
    kuint32_t end = start + nr;
    kuint32_t index, bit_mask, area_mask;
    kuint32_t bit_per_map = sizeof(*bitmap) << 3;

    if (end >= total_bits)
        return;

    for (index = start; index < end; index++)
    {
        area_mask = mrt_udiv(index, bit_per_map);
        bit_mask = mrt_urem(index, bit_per_map);

        if (value)
            *(bitmap + area_mask) |= (1 << bit_mask);
        else
            *(bitmap + area_mask) &= ~(1 << bit_mask);
    }
}

/*!
 * @brief   find first bit that equaled to 0
 * @param   ...
 * @retval  none
 * @note    none
 */
kint32_t bitmap_find_first_zero_bit(void *bitmap, kuint32_t start, kusize_t total_bits)
{
    return bitmap_find_first_bit((kuint8_t *)bitmap, start, total_bits, false);
}

/*!
 * @brief   find first bit that equaled to 1
 * @param   ...
 * @retval  none
 * @note    none
 */
kint32_t bitmap_find_first_valid_bit(void *bitmap, kuint32_t start, kusize_t total_bits)
{
    return bitmap_find_first_bit((kuint8_t *)bitmap, start, total_bits, true);
}

/*!
 * @brief   request multiple consecutive bits that equaled to 0
 * @param   ...
 * @retval  none
 * @note    none
 */
kint32_t bitmap_find_nr_zero_bit(void *bitmap, kuint32_t start, kusize_t total_bits, kuint32_t nr)
{
    return bitmap_find_nr_bit((kuint8_t *)bitmap, start, total_bits, nr, false);
}

/*!
 * @brief   request multiple consecutive bits that equaled to 1
 * @param   ...
 * @retval  none
 * @note    none
 */
kint32_t bitmap_find_nr_valid_bit(void *bitmap, kuint32_t start, kusize_t total_bits, kuint32_t nr)
{
    return bitmap_find_nr_bit((kuint8_t *)bitmap, start, total_bits, nr, true);
}

/*!
 * @brief   set start ~ (start + nr) to value 0
 * @param   ...
 * @retval  none
 * @note    (start + nr) < total_bits
 */
void bitmap_set_nr_bit_zero(void *bitmap, kuint32_t start, kusize_t total_bits, kuint32_t nr)
{
    bitmap_set_nr_bit((kuint8_t *)bitmap, start, total_bits, nr, false);
}

/*!
 * @brief   set start ~ (start + nr) to value 1
 * @param   ...
 * @retval  none
 * @note    (start + nr) < total_bits
 */
void bitmap_set_nr_bit_valid(void *bitmap, kuint32_t start, kusize_t total_bits, kuint32_t nr)
{
    bitmap_set_nr_bit((kuint8_t *)bitmap, start, total_bits, nr, true);
}

/* end of file */
