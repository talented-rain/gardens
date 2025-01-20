/*
 * General Error Type Defines
 *
 * File Name:   error_types.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.09.10
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __ERROR_TYPES_H
#define __ERROR_TYPES_H

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The includes */
#include "basic_types.h"
#include <assert.h>

/*!< The defines */
/*!< error code */
enum __ERT_ERROR_CODE
{
	/*!< --------------------------------------------------------- */
	/*!< 0: well */
	ER_NORMAL = 0,

	/*!< --------------------------------------------------------- */
	/*!< use with negative */
	ER_ERROR,
	ER_RETRY,
	ER_PERMIT,
	ER_NOMEM,
	ER_UNVALID,
	ER_EMPTY,
	ER_FULL,
	ER_NULLPTR,
	ER_WILDPTR,
	ER_FAULT,
	ER_MORE,
	ER_LACK,
	ER_NOTALIGN,
	ER_EXISTED,
	ER_NOTFOUND,
	ER_USED,
	ER_LOCKED,
	ER_FORBID,

	/*!< hardware */
	ER_IOERR,
	ER_NSUPPORT,
	ER_NREADY,
	ER_FAILD,
	ER_TIMEOUT,
	ER_NODEV,
	ER_BUSY,
	ER_CHECKERR,

	/*!< comunication */
	ER_TRXERR,
	ER_SEND_FAILD,
	ER_RECV_FAILD,
	ER_SCMD_FAILD,
	ER_RCMD_FAILD,
	ER_SDATA_FAILD,
	ER_RDATA_FAILD,
	ER_STOP_FAILD,					
};

/*!< The functions */
extern void deal_assert_fail(const kchar_t *__assertion, const kchar_t *__file,
			   					kuint32_t __line, const kchar_t *__function)
								__THROW __attribute__ ((__noreturn__));
/*!< The defines */
#define mrt_void()									((void)(0))
#define mrt_assert(x)								((x) ? mrt_void() : deal_assert_fail(#x, __FILE__, __LINE__, __ASSERT_FUNCTION))

#define mrt_jump_to(label, run_codes)	\
	do {	\
		run_codes;	\
		goto label;	\
	} while (0)

/*!< API functions */
/*!
 * @brief   judge if ptr is valid
 * @param   ptr
 * @retval  none
 * @note    none
 */
__force_inline static inline kbool_t IS_ERR(const void *ptr)
{
	/*!< the highest 4KB (0xfffff000 ~ 0xffffffff) is used to deal with exceptions */
	return ((kuaddr_t)ptr >= (kuaddr_t)(-(4UL << 10)));
}

/*!
 * @brief   convert error code to address
 * @param   error code
 * @retval  ptr
 * @note    none
 */
__force_inline static inline void *ERR_PTR(kstype_t code)
{
	return (void *)code;
}

/*!
 * @brief   convert ptr to error code
 * @param   ptr
 * @retval  error code
 * @note    none
 */
__force_inline static inline kstype_t PTR_ERR(const void *ptr)
{
	return ptr ? (kstype_t)ptr : -ER_NOMEM;
}

#ifdef __cplusplus
	}
#endif

#endif /* __ERROR_TYPES_H */
