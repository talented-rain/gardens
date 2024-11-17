/*
 * Baisc Types Defines
 *
 * File Name:   basic_types.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.09.10
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __BASIC_TYPES_H
#define __BASIC_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

/*!< The defines */
#define TARGET_EXT              extern

#if !defined(false)
#define false                   (0)
#elif (0 != false)
#undef false
#define false                   (0)
#endif

#if !defined(true)
#define true                    (1)
#elif (1 != true)
#undef true
#define true                    (1)
#endif

#define mrt_false               (false)
#define mrt_true                (true)
#define mrt_boolen(x)           (!!(x))

#ifdef __cplusplus
#define mrt_nullptr             (nullptr)
#else
#define mrt_nullptr             ((void *)0)
#endif

#define IT_FALSE                (mrt_false)
#define IT_TRUE                 (mrt_true)
#define IT_BOOLEN(x)            (mrt_boolen(x))
#define IT_NULL                 (mrt_nullptr)

typedef enum ert_bool
{
    NR_FALSE = false,
    NR_TRUE  = true

} ert_bool_t;

#if defined(__cplusplus) || defined(bool)
typedef bool                    kbool_t;
#else
typedef ert_bool_t              kbool_t;
#endif

#define mrt_to_kbool(x)         ((x) ? NR_TRUE : NR_FALSE)

/*!< basic types */
typedef                 char    kint8_t;
typedef unsigned        char    kuint8_t;
typedef                 short   kint16_t;
typedef unsigned        short   kuint16_t;
typedef                 int     kint32_t;
typedef unsigned        int     kuint32_t;
typedef          long   long    kint64_t;
typedef unsigned long   long    kuint64_t;
typedef                 float   kfloat_t;
typedef                 double  kdouble_t;

/*!< super types */
typedef                 char    kchar_t;
typedef                 char    kbyte_t;
typedef unsigned        char    kubyte_t;
typedef				    char	kbuffer_t;
typedef	unsigned 	    char	kubuffer_t;
typedef				    int		kssize_t;
typedef	unsigned 	    int		kusize_t;
typedef unsigned        long    kuaddr_t;
typedef unsigned        long    kutype_t;
typedef                 long    kstype_t;

#define __used		            __attribute__((used))
#define __weak                  __attribute__((weak))
#define __packed                __attribute__((packed))
#define __align(x)              __attribute__((__aligned__(x)))
#define __section(x)            __attribute__((section(x)))
#define __force                 __attribute__((force))

#if defined(__GNUC__)
    #define __force_inline      __attribute__((always_inline))
#else 
    #define __force_inline
#endif

#define mrt_likely(x)           __builtin_expect(mrt_boolen(x), true)
#define mrt_unlikely(x)         __builtin_expect(mrt_boolen(x), false)

#ifndef mrt_likely
#define mrt_likely(x)           (__builtin_constant_p(x) ? mrt_boolen(x) : __branch_check__(x, true))
#endif

#ifndef mrt_unlikely
#define mrt_check_unlikely(x)   (__builtin_constant_p(x) ? mrt_boolen(x) : __branch_check__(x, false))
#endif

#endif  /* __BASIC_TYPES_H */
