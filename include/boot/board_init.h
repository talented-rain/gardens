/*
 * Board Configuration
 *
 * File Name:   board_init.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.09.10
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __BOARD_INIT_H
#define __BOARD_INIT_H

/*!< The includes */
#include <common/generic.h>
#include <common/api_string.h>
#include <common/io_stream.h>
#include <common/mem_manage.h>
#include <board/board.h>
#include <platform/fwk_basic.h>
#include <platform/fwk_mempool.h>
#include <platform/mmc/fwk_sdcard.h>
#include <boot/boot_text.h>
#include <boot/implicit_call.h>
#include <configs/mach_configs.h>

/*<! The defines */
#define RET_BOOT_ERR                        (1)
#define RET_BOOT_PASS                       (0)

/*<! global data */
struct global_data
{
    kuaddr_t iboot_sp;
    kuaddr_t malloc_addr;
    kuint32_t malloc_len;
    kuaddr_t os_entry;
};
typedef struct global_data srt_gd_t;

typedef kint32_t (*board_init_t) (void);

TARGET_EXT kuaddr_t _SVC_MODE_STACK_BASE;
TARGET_EXT kuaddr_t _SYS_MODE_STACK_BASE;
TARGET_EXT kuaddr_t _IRQ_MODE_STACK_BASE;
TARGET_EXT kuaddr_t _FIQ_MODE_STACK_BASE;
TARGET_EXT kuaddr_t _ABT_MODE_STACK_BASE;
TARGET_EXT kuaddr_t _UND_MODE_STACK_BASE;

/*!< The functions */
/*!< s_init */
TARGET_EXT void s_init(void);

/*!< main initializtion */
TARGET_EXT void board_init_f(void);
TARGET_EXT void board_init_r(void);

/*!< other initialization */
TARGET_EXT kuaddr_t board_init_f_alloc_reserve(kuaddr_t base);
TARGET_EXT void board_init_f_init_reserve(kuaddr_t base);

/*!< initialized by being called by "board_init_f/r" */
TARGET_EXT kint32_t board_init_console(void);
TARGET_EXT kint32_t board_init_light(void);
TARGET_EXT kint32_t board_init_sdmmc(void);
TARGET_EXT kint32_t board_init_systick(void);

/*!< API function */
/*!
 * @brief   board_get_gd
 * @param   none
 * @retval  gd
 * @note    retuen gd address from r9
 */
static inline srt_gd_t *board_get_gd(void)
{
    srt_gd_t *sprt_gd;

    __asm__ __volatile__ (
        " mov %0, r9    "
        : "=&r"(sprt_gd)
    );

    return sprt_gd;
}

/*!
 * @brief   board_initcall_run_list
 * @param   none
 * @retval  none
 * @note    call every function from board 
 */
static inline kint32_t board_initcall_run_list(const board_init_t init_sequence[])
{
	const board_init_t *pFunc_init;

	for (pFunc_init = init_sequence; (*pFunc_init); pFunc_init++)
	{
		if ((*pFunc_init)())
			return RET_BOOT_ERR;
	}

	return RET_BOOT_PASS;
}

#endif /* __BOARD_INIT_H */
