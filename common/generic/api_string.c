/*
 * String General Function
 *
 * File Name:   api_string.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.09.26
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <common/api_string.h>
#include <common/io_stream.h>
#include <platform/fwk_mempool.h>

/*!< API function */
/*!
 * @brief   get_integrater_lenth
 * @param   value
 * @retval  none
 * @note    calculate the lenth of integrater
 */
kusize_t get_integrater_lenth(kuint64_t value)
{
    kuint64_t num;
    kusize_t lenth;

    num = value;
    lenth = 0;

    /*!< Dividing by 10 and taking remainder */
    do
    {
        num = num / 10;
        lenth++;

    } while (num);

    return lenth;
}

/*!
 * @brief   get_string_lenth
 * @param   ptr_src
 * @retval  none
 * @note    calculate the lenth of string, does not include '\0'
 */
kusize_t get_string_lenth(const void *ptr_src)
{
    kuint8_t *ptr_ch;

    ptr_ch = (kuint8_t *)ptr_src;

    while ('\0' != *(ptr_ch++));

    return ((kusize_t)(ptr_ch - (kuint8_t *)ptr_src) - 1);
}

/*!
 * @brief   do_string_split
 * @param   ptr_dst, offset, ptr_src
 * @retval  none
 * @note    split two strings
 */
void do_string_split(void *ptr_dst, kuint32_t offset, const void *ptr_src)
{
    kuint8_t *ptr_ch;
    kuint8_t *ptr_buf;

    ptr_ch  = (kuint8_t *)ptr_src;
    ptr_buf	= (kuint8_t *)ptr_dst + offset;

    while ('\0' != *ptr_ch)
        *(ptr_buf++) = *(ptr_ch++);
}

/*!
 * @brief   do_string_copy
 * @param   ptr_dst, ptr_src
 * @retval  none
 * @note    copy string to another string
 */
kchar_t *do_string_copy(void *ptr_dst, const void *ptr_src)
{
    kuint8_t *ptr_ch;
    kuint8_t *ptr_buf;

    ptr_ch  = (kuint8_t *)ptr_src;
    ptr_buf	= (kuint8_t *)ptr_dst;

    do
    {
        *(ptr_buf++) = *(ptr_ch++);

    } while ('\0' != *ptr_ch);

    return (kchar_t *)ptr_dst;
}

/*!
 * @brief   do_string_n_copy
 * @param   ptr_dst, offset, ptr_src
 * @retval  none
 * @note    copy n char to another string
 */
kchar_t *do_string_n_copy(void *ptr_dst, const void *ptr_src, kuint32_t size)
{
    kuint8_t *ptr_ch;
    kuint8_t *ptr_buf;

    ptr_ch  = (kuint8_t *)ptr_src;
    ptr_buf	= (kuint8_t *)ptr_dst;

    while ('\0' != *ptr_ch && (size--))
        *(ptr_buf++) = *(ptr_ch++);

    return (kchar_t *)ptr_dst;
}

/*!
 * @brief   do_string_n_copy_safe
 * @param   ptr_dst, offset, ptr_src
 * @retval  none
 * @note    copy n char to another string
 */
kuint32_t do_string_n_copy_safe(void *ptr_dst, const void *ptr_src, kuint32_t size)
{
    kuint8_t *ptr_ch;
    kuint8_t *ptr_buf;
    kuint32_t lenth;

    ptr_ch  = (kuint8_t *)ptr_src;
    ptr_buf	= (kuint8_t *)ptr_dst;
    lenth = get_string_lenth(ptr_src);

    while (('\0' != *ptr_ch) && size)
    {
        if (--size)
        {
            *(ptr_buf++) = *(ptr_ch++);
            continue;
        }

        *ptr_buf = '\0';
    }

    return lenth;
}

/*!
 * @brief   do_string_compare
 * @param   ptr_dst, ptr_src
 * @retval  none
 * @note    compare string
 */
kbool_t do_string_compare(void *ptr_dst, const void *ptr_src)
{
    kuint8_t *ptr_ch;
    kuint8_t *ptr_buf;

    ptr_ch  = (kuint8_t *)ptr_src;
    ptr_buf	= (kuint8_t *)ptr_dst;

    do
    {
        if (*(ptr_buf++) != *(ptr_ch++))
            return true;

    } while ('\0' != *ptr_ch);

    return false;	
}

/*!
 * @brief   do_string_n_compare
 * @param   ptr_dst, ptr_src, size
 * @retval  none
 * @note    compare n char
 */
kbool_t do_string_n_compare(void *ptr_dst, const void *ptr_src, kuint32_t size)
{
    kuint8_t *ptr_ch;
    kuint8_t *ptr_buf;

    ptr_ch  = (kuint8_t *)ptr_src;
    ptr_buf	= (kuint8_t *)ptr_dst;

    if (!size)
        return true;

    do
    {
        if (*(ptr_buf++) != *(ptr_ch++))
            return true;

    } while (('\0' != *ptr_ch) && (--size));

    return false;	
}

/*!
 * @brief   do_string_reverse
 * @param   value
 * @retval  none
 * @note    reverse string
 */
void do_string_reverse(void *ptr_src, kuint32_t size)
{
    kuint8_t *ptr_head, *ptr_tail;
    kuint8_t ch;

    /*!< ptr_head points to string head, ptr_tail points to end symbol '\0' */
    ptr_head = (kuint8_t *)ptr_src;
    ptr_tail = (kuint8_t *)ptr_src + ((size) ? size : get_string_lenth(ptr_src));

    /*!< if the number of characters is lower than 2, it's not necessary to reverse */
    if ((kuint32_t)(ptr_tail - ptr_head) <= 1)
        return;

    ptr_tail--;

    while (ptr_tail > ptr_head)
    {
        ch				= *(ptr_head);
        *(ptr_head++) 	= *(ptr_tail);
        *(ptr_tail--) 	= ch;
    }
}

/*!
 * @brief   convert_number_to_string
 * @param   ptr_dst, value
 * @retval  none
 * @note    integrater convert to string
 */
kusize_t convert_number_to_string(void *ptr_dst, kuint64_t value)
{
    kuint8_t *ptr_buf;
    kuint64_t num;
    kusize_t lenth;

    ptr_buf = (kuint8_t *)ptr_dst;
    num		= value;
    lenth	= 0;

    /*!< Dividing by 10 and taking remainder */
    do
    {
        num = num / 10;
        lenth++;

        if (isValid(ptr_dst))
        {
            *(ptr_buf++) = (value - ((num << 1) + (num << 3))) + '0';
            value = value / 10;
        }

    } while (num);

    /*!< reverse string */
    if (isValid(ptr_dst))
        do_string_reverse(ptr_dst, lenth);

    return lenth;
}

/*!
 * @brief   seek_char_in_string
 * @param   ptr_src, ch
 * @retval  the character position in string
 * @note    find a character position
 */
kchar_t *seek_char_in_string(const void *ptr_src, kchar_t ch)
{
    kchar_t *ptr_ch;

    ptr_ch = (kchar_t *)ptr_src;

    while (*ptr_ch != '\0')
    {
        if (ch == *ptr_ch)
            return ptr_ch;

        ptr_ch++;
    }

    return mrt_nullptr;
}

/*!
 * @brief   seek_char_by_pos
 * @param   ptr_src, index
 * @retval  the character position in string
 * @note    find a character position
 */
kchar_t *seek_char_by_pos(const void *ptr_src, kuint32_t index)
{
    kchar_t *ptr_ch, *ptr_end;

    ptr_ch  = (kchar_t *)ptr_src;
    ptr_end = ptr_ch + index;

    while (*ptr_ch != '\0')
    {
        if (ptr_ch == ptr_end)
            return ptr_ch;

        ptr_ch++;
    }

    return mrt_nullptr;
}

/*!
 * @brief   do_fmt_convert
 * @param   ptr_buf, ptr_level, ptr_fmt, ptr_list
 * @retval  none
 * @note    string format conversion
 */
kusize_t do_fmt_convert(void *ptr_buf, kubyte_t *ptr_level, const kchar_t *ptr_fmt, va_list ptr_list, kusize_t size)
{
    kchar_t *ptr_data, *ptr_args;
    kubyte_t ch;
    kusize_t lenth, count;
    kuint64_t i, num;

    ptr_data = (kchar_t *)ptr_buf;
    lenth = 0;
    
    if (size < 1)
        return 0;

    size -= 1;

    /*!< calculate numbers first */
    for (i = 0; *(ptr_fmt + i) != '\0'; i++)
    {
        ch = *(ptr_fmt + i);

        if (!i && (ch == *(PRINT_LEVEL_SOH)))
        {
            i++;

            if (ptr_level)
            {
                *ptr_level = *(PRINT_LEVEL_SOH);
                *(ptr_level + 1) = *(ptr_fmt + i);
            }

            continue;
        }

        if (ch == '\n')
        {
            /*!< jump over */
        }

        /*!< ------------------------------------------------------------------ */
        /*!< Aim to '%' */
        if (ch != '%')
        {
            if ((lenth + 1) > size)
                break;

            if (isValid(ptr_buf))
                *(ptr_data++) = ch;

            lenth++;

            continue;
        }

        /*!< if (ch == '%') */
        i++;
        ch = *(ptr_fmt + i);

        switch (ch)
        {
            case 'c':
                num = (kchar_t)va_arg(ptr_list, kuint32_t);

                if ((lenth + 1) > size)
                    break;

                if (isValid(ptr_buf))
                    *(ptr_data++) = num + ' ';

                lenth += 1;
                break;

            case 'd':
                num = (kuint32_t)va_arg(ptr_list, kuint32_t);
                count = convert_number_to_string(mrt_nullptr, num);

                if ((lenth + count) > size)
                    break;

                convert_number_to_string(ptr_data, num);

                if (isValid(ptr_buf))
                    ptr_data += count;

                lenth += count;
                break;

            case 'l':
                if (*(ptr_fmt + i + 1) == 'd')
                {
                    num = (kuint64_t)va_arg(ptr_list, kuint64_t);
                    count = convert_number_to_string(mrt_nullptr, num);

                    if ((lenth + count) > size)
                        break;

                    if (isValid(ptr_buf))
                    {
                        convert_number_to_string(ptr_data, num);
                        ptr_data += count;
                    }

                    i++;
                    lenth += count;
                }
                else
                {
                    if ((lenth + 1) > size)
                        break;

                    if (isValid(ptr_buf))
                        *(ptr_data++) = '%';

                    i--;
                    lenth++;
                }
                break;

            case 's':
                ptr_args = (kchar_t *)va_arg(ptr_list, kchar_t *);
                count	 = get_string_lenth(ptr_args);

                if ((lenth + count) > size)
                    break;

                if (isValid(ptr_buf))
                {
                    do_string_split(ptr_data, 0, ptr_args);
                    ptr_data += count;
                }

                lenth += count;

                break;

            case 'x':
            case 'p':
                num = (kutype_t)va_arg(ptr_list, kutype_t);
                count = dec_to_hex(mrt_nullptr, num, false);

                if ((lenth + count) > size)
                    break;

                dec_to_hex(ptr_data, num, false);

                if (isValid(ptr_buf))
                    ptr_data += count;

                lenth += count;
                break;

            case 'b':
                num = (kutype_t)va_arg(ptr_list, kutype_t);
                count = dec_to_binary(mrt_nullptr, num);

                if ((lenth + count) > size)
                    break;

                dec_to_binary(ptr_data, num);

                if (isValid(ptr_buf))
                    ptr_data += count;

                lenth += count;
                break;

            default:
                if ((lenth + 1) > size)
                    break;

                if (isValid(ptr_buf))
                    *(ptr_data++) = '%';

                i--;
                lenth++;
                break;
        }
    }

    if (isValid(ptr_buf))
        *ptr_data = '\0';

    return lenth;
}

/*!
 * @brief   vasprintk
 * @param   ptr_buf, ptr_fmt
 * @retval  none
 * @note    String format conversion
 */
kchar_t *vasprintk(const kchar_t *ptr_fmt, kusize_t *size, va_list sprt_list)
{
    va_list sprt_copy;
    kchar_t *ptr;
    kusize_t lenth;

    if (!ptr_fmt)
        return mrt_nullptr;

    va_copy(sprt_copy, sprt_list);
    lenth = do_fmt_convert(mrt_nullptr, mrt_nullptr, ptr_fmt, sprt_copy, (kusize_t)(~0));
    va_end(sprt_copy);

    ptr = kmalloc(lenth + 1, GFP_KERNEL);
    if (!isValid(ptr))
        return mrt_nullptr;

    do_fmt_convert(ptr, mrt_nullptr, ptr_fmt, sprt_list, lenth + 1);
    if (size)
        *size = lenth;

    return ptr;
}

/*!
 * @brief   lv_vasprintk
 * @param   ptr_fmt, size
 * @retval  none
 * @note    String format conversion
 */
kchar_t *lv_vasprintk(const kchar_t *ptr_fmt, kusize_t *size, kubyte_t *ptr_lv, va_list sprt_list)
{
    va_list sprt_copy;
    kchar_t *ptr;
    kusize_t lenth;

    if (!ptr_fmt)
        return mrt_nullptr;

    va_copy(sprt_copy, sprt_list);
    lenth = do_fmt_convert(mrt_nullptr, mrt_nullptr, ptr_fmt, sprt_copy, (kusize_t)(~0));
    va_end(sprt_copy);

    ptr = kmalloc(lenth + 1, GFP_KERNEL);
    if (!isValid(ptr))
        return mrt_nullptr;

    do_fmt_convert(ptr, ptr_lv, ptr_fmt, sprt_list, lenth + 1);
    if (size)
        *size = lenth;

    return ptr;
}

/*!
 * @brief   sprintk
 * @param   ptr_buf, ptr_fmt
 * @retval  none
 * @note    String format conversion
 */
kint32_t sprintk(void *ptr_buf, const kchar_t *ptr_fmt, ...)
{
    va_list ptr_list;
    kusize_t size;

    va_start(ptr_list, ptr_fmt);
    size = do_fmt_convert(ptr_buf, mrt_nullptr, ptr_fmt, ptr_list, (kusize_t)(~0));
    va_end(ptr_list);

    return size;
}

/*!< -------------------------------------------------------------------- */
#if (0)
/*!
 * @brief   strlen
 * @param   none
 * @retval  none
 * @note    return string lenth
 */
__weak size_t strlen(const char *__s)
{
    return (size_t)get_string_lenth(__s);
}

/*!
 * @brief   strcpy
 * @param   none
 * @retval  none
 * @note    copy src to dest
 */
__weak char *strcpy(char *__dest, const char *__src)
{
    return do_string_copy(__dest, __src);
}

/*!
 * @brief   strncpy
 * @param   none
 * @retval  none
 * @note    copy src to dest
 */
__weak char *strncpy(char *__dest, const char *__src, size_t __n)
{
    return do_string_n_copy(__dest, __src, __n);
}

/*!
 * @brief   strlcpy
 * @param   none
 * @retval  none
 * @note    copy src to dest
 */
__weak unsigned int strlcpy(char *__dest, const char *__src, size_t __n)
{
    return do_string_n_copy_safe(__dest, __src, __n);
}

/*!
 * @brief   strcmp
 * @param   none
 * @retval  none
 * @note    compare s1 and s2
 */
__weak int strcmp(const char *__s1, const char *__s2)
{
    return do_string_compare(__s1, __s2);
}

/*!
 * @brief   strncmp
 * @param   none
 * @retval  none
 * @note    compare s1 and s2
 */
__weak int strncmp(const char *__s1, const char *__s2, size_t __n)
{
    return do_string_n_compare(__s1, __s2, __n);
}

#else
/*!
 * @brief   strlen
 * @param   none
 * @retval  none
 * @note    return string lenth
 */
__weak kuint32_t kstrlen(const kchar_t *__s)
{
    return (kuint32_t)get_string_lenth(__s);
}

/*!
 * @brief   strcpy
 * @param   none
 * @retval  none
 * @note    copy src to dest
 */
__weak kchar_t *kstrcpy(kchar_t *__dest, const kchar_t *__src)
{
    return do_string_copy(__dest, __src);
}

/*!
 * @brief   strncpy
 * @param   none
 * @retval  none
 * @note    copy src to dest
 */
__weak kchar_t *kstrncpy(kchar_t *__dest, const kchar_t *__src, kusize_t __n)
{
    return do_string_n_copy(__dest, __src, __n);
}

/*!
 * @brief   strlcpy
 * @param   none
 * @retval  none
 * @note    copy src to dest
 */
__weak kusize_t kstrlcpy(kchar_t *__dest, const kchar_t *__src, kusize_t __n)
{
    return do_string_n_copy_safe(__dest, __src, __n);
}

/*!
 * @brief   strcmp
 * @param   none
 * @retval  none
 * @note    compare s1 and s2
 */
__weak kint32_t kstrcmp(kchar_t *__s1, const kchar_t *__s2)
{
    return do_string_compare(__s1, __s2);
}

/*!
 * @brief   strncmp
 * @param   none
 * @retval  none
 * @note    compare s1 and s2
 */
__weak kint32_t kstrncmp(kchar_t *__s1, const kchar_t *__s2, kusize_t __n)
{
    return do_string_n_compare(__s1, __s2, __n);
}

/*!
 * @brief   kstrchr
 * @param   none
 * @retval  none
 * @note    locate where the first "ch" appears
 */
__weak kchar_t *kstrchr(const kchar_t *__s1, kchar_t ch)
{
    return seek_char_in_string(__s1, ch);
}

/*!
 * @brief   kstrcat
 * @param   none
 * @retval  none
 * @note    locate where the index "ch" appears
 */
__weak kchar_t *kstrcat(const kchar_t *__s1, kuint32_t index)
{
    return seek_char_by_pos(__s1, index);
}

#endif


/* end of file */
