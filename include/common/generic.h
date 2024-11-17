/*
 * General Defines
 *
 * File Name:   generic.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.09.10
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __GENERAL_H
#define __GENERAL_H

/*!< The includes */
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <malloc.h>

#include "basic_types.h"
#include "error_types.h"

/*!< The defines */
/*!< general */
#define mrt_isNull(x)									(mrt_nullptr == (x))
#define mrt_notNull(x)									(mrt_nullptr != (x))
#define mrt_isValid(x)									(x)

/*!< bit calc */
#define mrt_bit_nr(val, nr)								((kuint32_t)(((kuint32_t)(val)) << (nr)))
#define mrt_bit(nr)							        	mrt_bit_nr(1, (nr))
#define mrt_mask(val, mask)								((kuint32_t)(val) & (mask))
#define mrt_bit_mask(val, mask, nr)						(mrt_bit_nr(val, nr) & (mask))
#define mrt_bit_mask_nr(val, mask, nr)					mrt_bit_mask(val, mrt_bit_nr(mask, nr), nr)

#define mrt_setbit(val, nr)								((val) |=  mrt_bit(nr))
#define mrt_clrbit(val, nr)								((val) &= ~mrt_bit(nr))
#define mrt_is_bitequal(val, mask)						((mask) == ((val) & (mask)))
#define mrt_is_bitset(val, mask)						((val) & (mask))
#define mrt_is_bitreset(val, mask)						(0 == ((val) & (mask)))

/*!< compare at least two number */
#define CMP_GT2(a, b, c, d)								(((a) > (b)) ? (c) : (d))
#define CMP_LT2(a, b, c, d)								(((a) < (b)) ? (c) : (d))
#define CMP_GTS(a, b, d)								CMP_GT2(a, b, a, d)
#define CMP_LTS(a, b, d)								CMP_LT2(a, b, a, d)
#define CMP_MAX2(a, b)									CMP_GT2(a, b, a, b)
#define CMP_MIN2(a, b)									CMP_LT2(a, b, a, b)

/*!< correct usage for comparing both variable */
#define mrt_cmp_gt(a, b, c, d)	\
({	\
	const typeof(a) _a = (a);	\
	const typeof(b) _b = (b);	\
	const typeof(c) _c = (c);	\
	const typeof(d) _d = (d);	\
	(void)(&_a == &_b);	\
	CMP_GT2(_a, _b, _c, _d);	\
})

#define mrt_cmp_lt(a, b, c, d)	\
({	\
	const typeof(a) _a = (a);	\
	const typeof(b) _b = (b);	\
	const typeof(c) _c = (c);	\
	const typeof(d) _d = (d);	\
	(void)(&_a == &_b);	\
	CMP_LT2(_a, _b, _c, _d);	\
})

#define mrt_ret_max_super(a, b, d)						mrt_cmp_gt(a, b, a, d)
#define mrt_ret_min_super(a, b, d)						mrt_cmp_lt(a, b, a, d)
#define mrt_ret_max2(a, b)								mrt_cmp_gt(a, b, a, b)
#define mrt_ret_min2(a, b)								mrt_cmp_lt(a, b, a, b)

#define mrt_abs(a)										((typeof(a))(((a) < 0) ? -(a) : (a)))
#define mrt_usub(a, b)	\
({	\
	const typeof(a) _a = (a);	\
	const typeof(b) _b = (b);	\
	(void)(&_a == &_b);	\
	((_a) > (_b)) ? ((_a) - (_b)) : ((_b) - (_a));	\
})

#define ERT_NUM(x)                              		NR_##x
#define ERT_DEFINE(x, value)							NR_##x = value

/*!<
 * Byte Alignment: for data that has already been aligned, the data remains unchanged after alignment.
 * Take 4 bytes as an example: 00, 04, 08, what they have in common is that the lower 2 bits must be zero. (0b11 = 0x03 = (4 - 1));
 * So the principle of alignment is to make the corresponding bit 0.
 */
#define mrt_align(x, mask)								(((x) + ((mask) - 1)) & (~((mask) - 1)))
#define mrt_is_aligned(x, mask)							((x) & ((typeof(x))(mask) - 1) == 0)
#define mrt_align4(x)									mrt_align(x, 4)

#define mrt_num_align(x, mask)							((typeof(x))mrt_align((x), (typeof(x))(mask)))
#define mrt_ptr_align(p, mask)							((typeof(p))mrt_align((kuaddr_t)(p), (kuaddr_t)(mask)))
#define mrt_num_align4(x)								mrt_num_align((x), 4)
#define mrt_ptr_align4(p)								mrt_ptr_align((p), 4)
#define mrt_num_align8(x)								mrt_num_align((x), 8)
#define mrt_ptr_align8(p)								mrt_ptr_align((p), 8)

#define mrt_ralign(x, mask)								((x) & (~((mask) - 1)))

/*!< Position bit offset on 32-bit array */
#define mrt_word_offset(integer)						((kuint32_t)((integer) >> 5U))
#define mrt_bit_offset(integer)							((kuint32_t)(integer) & 0x1fU)

/*!< Get the number of arrays */						
#define ARRAY_SIZE(arr)									(sizeof(arr) / sizeof((arr)[0]))
#define mrt_array_size(arr)								ARRAY_SIZE(arr)

/*!< Calculate the offset of the member in the struct */
#define mrt_member_offset(type, member)					((kusize_t)(&((type *)0)->member))
/*!< Return the handler of parent struct */
#define mrt_to_parent_handler(ptr, type, member)	\
({	\
	const typeof(((type *)0)->member) *ptr_member = (ptr);	\
	(type *)((char *)ptr_member - mrt_member_offset(type, member));	\
})
#define mrt_container_of(ptr, type, member)				mrt_to_parent_handler(ptr, type, member)

/*!< swap high byte and low byte for 2 bytes val */
#define TO_CONVERT_BYTE16(x)	\
(	\
	(((kuint16_t)(x) & (kuint16_t)0x00ff) << 8)	|	\
	(((kuint16_t)(x) & (kuint16_t)0xff00) >> 8)		\
)

/*!< swap high byte and low byte for 4 bytes val */
#define TO_CONVERT_BYTE32(x)	\
(	\
	(((kuint32_t)(x) & (kuint32_t)0x000000ff) << 24)	|	\
	(((kuint32_t)(x) & (kuint32_t)0x0000ff00) << 8)		|	\
	(((kuint32_t)(x) & (kuint32_t)0x00ff0000) >> 8)		|	\
	(((kuint32_t)(x) & (kuint32_t)0xff000000) >> 24)		\
)

/*!< swap high bit and low bit for a byte */
#define TO_CONVERT_BIT8(x)	\
(	\
	(((kuint8_t)(x) & (kuint8_t)0b00000001) << 7)	|	\
	(((kuint8_t)(x) & (kuint8_t)0b00000010) << 5)	|	\
	(((kuint8_t)(x) & (kuint8_t)0b00000100) << 3)	|	\
	(((kuint8_t)(x) & (kuint8_t)0b00001000) << 1)	|	\
	(((kuint8_t)(x) & (kuint8_t)0b00010000) >> 1)	|	\
	(((kuint8_t)(x) & (kuint8_t)0b00100000) >> 3)	|	\
	(((kuint8_t)(x) & (kuint8_t)0b01000000) >> 5)	|	\
	(((kuint8_t)(x) & (kuint8_t)0b10000000) >> 7)		\
)

/*!< bits of per byte */
#define RET_BITS_PER_INT								(32)
#define RET_BITS_PER_LONG								(64)

/*!< bytes of per type */
#define RET_BYTES_PER_INT								(RET_BITS_PER_INT  >> 3)
#define RET_BYTES_PER_LONG								(RET_BITS_PER_LONG >> 3)

#define COUNT_INC(x)									do { (x)++; } while (0)
#define COUNT_DEC(x)									do { (x) ? (x)-- : (void)0; } while (0)

/*!< get inverse */
#define TO_REVERSE(x)									(~(x))
#define mrt_reverse(x)									TO_REVERSE(x)
/*!< get complement */
#define TO_COMPLEMENT(x)								(TO_REVERSE(x) + 1)
#define mrt_complement(x)								TO_COMPLEMENT(x)

/*!< run code retry */
#define mrt_run_code_retry(timeout, run_code) \
{   \
    kuint32_t time_cnt = (timeout);   \
    do  \
    {   \
        run_code    \
    } while (--time_cnt);   \
}

/*!< The functions */
TARGET_EXT kutype_t udiv_integer(kutype_t divied, kutype_t div);
TARGET_EXT kstype_t sdiv_integer(kstype_t divied, kstype_t div);
TARGET_EXT kutype_t udiv_remainder(kutype_t divied, kutype_t div);
TARGET_EXT kutype_t dec_to_hex(kchar_t *buf, kutype_t number, kbool_t mode);
TARGET_EXT kutype_t dec_to_binary(kchar_t *buf, kutype_t number);

#define mrt_udiv(divied, div)	\
({	\
	typeof(divied) _divied = (divied);	\
	typeof(div) _div = (div);	\
	(void)(&_divied == &_div);	\
	(typeof(divied))udiv_integer(_divied, _div);	\
})

#define mrt_sdiv(divied, div)	\
({	\
	typeof(divied) _divied = (divied);	\
	typeof(div) _div = (div);	\
	(void)(&_divied == &_div);	\
	(typeof(divied))sdiv_integer(_divied, _div);	\
})

#define mrt_urem(divied, div)	\
({	\
	typeof(divied) _divied = (divied);	\
	typeof(div) _div = (div);	\
	(void)(&_divied == &_div);	\
	(typeof(divied))udiv_remainder(_divied, _div);	\
})

/*!< API function */
/*!
 * @brief   judge if ptr is valid
 * @param   ptr
 * @retval  yes or not
 * @note    none
 */
__force_inline static inline kbool_t isValid(const void *ptr)
{
	return ((!!((kuaddr_t)ptr)) && !IS_ERR(ptr));
}

/*!
 * @brief   judge if the number-power of 2
 * @param   number
 * @retval  yes or not
 * @note    none
 */
__force_inline static inline kbool_t isPower2(kutype_t number)
{
	/*!<
	 * for example:
	 *	0:	return false
	 *	1:	= pow(2, 0), return true
	 *	2:	= pow(2, 1), 0b10 & 0b01 = 0 == 0, return true
	 *	3:	= ???, 0b11 & 0b10 = 0b10 != 0, return false
	 *	4:	= pow(2, 2), 0b100 & 0b011 = 0 == 0, return true
	 *	...
	 *
	 * all in all, it is equivalent to mrt_is_aligned(number, number)
	 * (when only one bit is 1, it is an n-pwoer of 2; that is, it is aligned with itself)
	 */
	return (number && (0 == (number & (number - 1))));
}

/*!
 * @brief   api_reverse_byte32
 * @param   val
 * @retval  result
 * @note    swap high byte and low byte for 4 bytes val
 */
static inline kuint32_t api_reverse_byte32(kuint32_t val)
{
    kuint32_t result;

    __asm__ __volatile__ (
		" rev %0, %1	"
		: "=&r"(result)
		: "r"(val)
		: "cc"
	);

    return result;
}

/*!
 * @brief   api_reverse_byte16
 * @param   val
 * @retval  result
 * @note    swap high byte and low byte for 2 bytes val
 */
static inline kuint16_t api_reverse_byte16(kuint16_t val)
{
    kuint16_t result;

    __asm__ __volatile__ (
		" rev16 %0, %1	"
		: "=&r"(result)
		: "r"(val)
		: "cc"
	);

    return result;
}

/*!
 * @brief   api_reverse_bit
 * @param   val
 * @retval  result
 * @note    swap high bit and low bit for a byte
 */
static inline kuint8_t api_reverse_bit(kuint8_t val)
{
    kuint8_t result, i;

	for (i = 8, result = 0; i > 0; i--)
	{
		result <<= 1;
		result |= (val & 0x01);
		val >>= 1;
	}

    return result;
}


#endif  /* __GENERAL_H */
