/*
 * Generic Configuration
 *
 * File Name:   mach_configs.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.12.16
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __MACH_CONFIGS_H
#define __MACH_CONFIGS_H

#include "autoconf.h"

#if defined(CONFIG_ARCH_ARMV7)
#include <asm/armv7/asm_config.h>
#endif
#include <board/board_config.h>

/*!< The defines */
#define CONFIG_STACK_WITH_LDS                   (1)                 /*!< configure stack by .lds */

#define CONFIG_HZ                               (100UL)             /*!< timer interrupt per 10ms*/
#define CONFIG_USE_AMP                          (0)

#endif /* __MACH_CONFIGS_H */
