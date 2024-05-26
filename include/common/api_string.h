/*
 * String Function Declare
 *
 * File Name:   api_string.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.09.26
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __API_STRING_H
#define __API_STRING_H

/*!< The includes */
#include <common/basic_types.h>
#include <common/error_types.h>
#include <common/generic.h>

/*!< The defines */
#define mrt_to_str(x)                                           #x
#define mrt_to_split_str(x, y, z)                               x##y##z

typedef struct
{
    const kchar_t *str;
    kusize_t size;

} srt_string_t;

/*!< The functions */
TARGET_EXT kusize_t get_integrater_lenth(kuint64_t value);
TARGET_EXT kusize_t get_string_lenth(const void *ptr_src);
TARGET_EXT void do_string_split(void *ptr_dst, kuint32_t offset, const void *ptr_src);
TARGET_EXT kint8_t *do_string_copy(void *ptr_dst, const void *ptr_src);
TARGET_EXT kint8_t *do_string_n_copy(void *ptr_dst, const void *ptr_src, kuint32_t size);
TARGET_EXT kuint32_t do_string_n_copy_safe(void *ptr_dst, const void *ptr_src, kuint32_t size);
TARGET_EXT kbool_t do_string_compare(const void *ptr_dst, const void *ptr_src);
TARGET_EXT kbool_t do_string_n_compare(const void *ptr_dst, const void *ptr_src, kuint32_t size);
TARGET_EXT void do_string_reverse(void *ptr_src, kuint32_t size);
TARGET_EXT kusize_t convert_number_to_string(void *ptr_dst, kuint64_t value);
TARGET_EXT kuint32_t seek_char_in_string(const void *ptr_src, kuint8_t ch);
TARGET_EXT kusize_t do_fmt_convert(void *ptr_buf, kubyte_t *ptr_level, const kchar_t *ptr_fmt, va_list ptr_list, kusize_t size);

TARGET_EXT kint32_t sprintk(void *ptr_buf, const kchar_t *ptr_fmt, ...);
TARGET_EXT kuint32_t kstrlen(const kchar_t *__s);
TARGET_EXT kchar_t *kstrcpy(kchar_t *__dest, const kchar_t *__src);
TARGET_EXT kchar_t *kstrncpy(kchar_t *__dest, const kchar_t *__src, kusize_t __n);
TARGET_EXT kusize_t kstrlcpy(kchar_t *__dest, const kchar_t *__src, kusize_t __n);
TARGET_EXT kint32_t kstrcmp(const kchar_t *__s1, const kchar_t *__s2);
TARGET_EXT kint32_t kstrncmp(const kchar_t *__s1, const kchar_t *__s2, kusize_t __n);

#endif /* __API_STRING_H */
