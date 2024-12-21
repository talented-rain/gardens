/*
 * Board Configuration For Assembly
 *
 * File Name:   board_config.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.09.10
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __BOARD_CONFIG_H
#define __BOARD_CONFIG_H

/*!< The defines */
/*!< Architecture*/
#define __ARCH_ARM                              (0)
#define __ARCH_X86                              (1)
#define __ARCH_RISC_V                           (2)

#define __ARCH_ARMV7                            (7)
#define __ARCH_ARMv8                            (8)

#define __ARCH_ARMV7_M3                         (3)
#define __ARCH_ARMV7_M4                         (4)
#define __ARCH_ARMV7_A7                         (7)
#define __ARCH_ARMV7_A8                         (8)
#define __ARCH_ARMV7_A9                         (9)

#define __ARCH_NUMBER(arch, ver, number)        (((arch) << 31) | ((ver) << 24) | (number))
#define __IS_ARCH_ARMV7(x)                      (__ARCH_ARMV7 == (((x) >> 24) & 0x7f))
#define __ARCH_ARMV7_A7_NUMBER                  __ARCH_NUMBER(__ARCH_ARM, __ARCH_ARMV7, __ARCH_ARMV7_A7)
#define __ARCH_ARMV7_A9_NUMBER                  __ARCH_NUMBER(__ARCH_ARM, __ARCH_ARMV7, __ARCH_ARMV7_A9)

/*!< Endian */
#define BIG_ENDIAN_FORMAT                       (0)
#define LIT_ENDIAN_FORMAT                       (1)

#define __KB_TO_BYTES(x)                        ((x) << 10)
#define __MB_TO_BYTES(x)                        ((x) << 20)
#define __BYTES_TO_KB(x)                        ((x) >> 10)
#define __BYTES_TO_MB(x)                        ((x) >> 20)

#define __ALIGN_DEC_8(x)                        ((x) & (~0x07))
#define __ALIGN_INC_8(x)						(((x) + 0x07) & (~0x07))


#endif /* __BOARD_CONFIG_H */
