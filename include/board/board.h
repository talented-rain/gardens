/*
 * Board Configuration For Assembly
 *
 * File Name:   board.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.09.10
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __BOARD_H
#define __BOARD_H

/*!< The includes */
#include <configs/configs.h>
#include <common/generic.h>

/*!< API function */
/*!
 * @brief   be32_to_cpu
 * @param   val
 * @retval  convert result
 * @note    return the endian-converted result
 */
static inline kuint32_t be32_to_cpu(kuint32_t val)
{
#if (defined(CONFIG_LITTILE_ENDIAN) && CONFIG_LITTILE_ENDIAN)
    return api_reverse_byte32(val);

#else
    return val;

#endif
}

/*!
 * @brief   be32_to_cpup
 * @param   ptr
 * @retval  convert result
 * @note    return the endian-converted result
 */
static inline kuint32_t be32_to_cpup(void *ptr)
{
    return be32_to_cpu(*(kuint32_t *)ptr);
}

#define mrt_be32_to_cpu(val)                            be32_to_cpu(val)
#define mrt_be32_to_cpup(ptr)                           be32_to_cpup(ptr)
#define mrt_cpu_to_be32(val)                            be32_to_cpu(val)
#define mrt_cpup_to_be32(ptr)                           be32_to_cpup(ptr)

/*!
 * @brief   le32_to_cpu
 * @param   val
 * @retval  convert result
 * @note    return the endian-converted result
 */
static inline kuint32_t le32_to_cpu(kuint32_t val)
{
#if (defined(CONFIG_LITTILE_ENDIAN) && (!CONFIG_LITTILE_ENDIAN))
    return api_reverse_byte32(val);

#else
    return val;

#endif
}

/*!
 * @brief   le32_to_cpup
 * @param   ptr
 * @retval  convert result
 * @note    return the endian-converted result
 */
static inline kuint32_t le32_to_cpup(void *ptr)
{
    return le32_to_cpu(*(kuint32_t *)ptr);
}

#define mrt_le32_to_cpu(val)							le32_to_cpu(val)
#define mrt_le32_to_cpup(val)							le32_to_cpup(val)
#define mrt_cpu_to_le32(val)							le32_to_cpu(val)
#define mrt_cpup_to_le32(val)							le32_to_cpup(val)

/*!
 * @brief   be16_to_cpu
 * @param   val
 * @retval  convert result
 * @note    return the endian-converted result
 */
static inline kuint16_t be16_to_cpu(kuint16_t val)
{
#if (defined(CONFIG_LITTILE_ENDIAN) && CONFIG_LITTILE_ENDIAN)
    return api_reverse_byte16(val);

#else
    return val;

#endif
}
#define mrt_be16_to_cpu(val)                            be16_to_cpu(val)
#define mrt_cpu_to_be16(val)                            be16_to_cpu(val)

/*!
 * @brief   le16_to_cpu
 * @param   val
 * @retval  convert result
 * @note    return the endian-converted result
 */
static inline kuint16_t le16_to_cpu(kuint16_t val)
{
#if (defined(CONFIG_LITTILE_ENDIAN) && (!CONFIG_LITTILE_ENDIAN))
    return api_reverse_byte16(val);

#else
    return val;

#endif
}
#define mrt_le16_to_cpu(val)							le16_to_cpu(val)
#define mrt_cpu_to_le16(val)							le16_to_cpu(val)

#endif /* __BOARD_COMMON_H */
