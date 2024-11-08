/*
 * Board Configuration
 *
 * File Name:   implicit_call.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.09.14
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __IMPLICIT_CALL_H
#define __IMPLICIT_CALL_H

/*!< The includes */
#include "boot_text.h"

/*!<
 * __attribute__((used)) const dync_init_t plat_##x __attribute__((section(".dync_init."#n))) = (dync_init_t)x
 * __attribute__((used)) const dync_exit_t plat_##x __attribute__((section(".dync_exit."#n))) = (dync_exit_t)x
 */
#define IMPORT_DYNC_INIT_ENTRY(prefix, x, sec)		__used const dync_init_t prefix##_##x sec = (dync_init_t)x
#define IMPORT_DYNC_EXIT_ENTRY(prefix, x, sec)		__used const dync_exit_t prefix##_##x sec = (dync_exit_t)x

enum __ERT_DYNC_SECTION_DEF
{
	NR_DYNC_SEC_STATIC		= 0,					/*!< save persistent params */
	NR_DYNC_SEC_EARLY		= 1,					/*!< board early initial */
	NR_DYNC_SEC_LATE		= 2,					/*!< board late initial */
	NR_DYNC_SEC_KERNEL		= 3,					/*!< kernel initial */
	NR_DYNC_SEC_ROOTFS		= 4,					/*!< root filesystem initial */
	NR_DYNC_SEC_PLATFORM	= 5,					/*!< platform(global or general) initial */
	NR_DYNC_SEC_PATTERN		= 6,					/*!< make platform reality */
	NR_DYNC_SEC_DEVICE		= 7,					/*!< platform device */
	NR_DYNC_SEC_DRIVER		= 8,					/*!< platform driver */
	NR_DYNC_SEC_OTHERS		= 9,					/*!< others, like temprory varibles, functions, and so on */
	NR_DYNC_SEC_BLK_END,

	/*!< real start and end address */
	NR_DYNC_SEC_START,
	NR_DYNC_SEC_END,
};

/*!< *(.dync_init.0), *(.dync_exit.0) */
#define __DYNC_STC_INIT_SEC							__DYNC_INIT_SEC(0)
#define __DYNC_STC_EXIT_SEC							__DYNC_EXIT_SEC(0)
/*!< *(.dync_init.1), *(.dync_exit.1) */
#define __DYNC_EARLY_INIT_SEC						__DYNC_INIT_SEC(1)
#define __DYNC_EARLY_EXIT_SEC						__DYNC_EXIT_SEC(1)
/*!< *(.dync_init.2), *(.dync_exit.2) */
#define __DYNC_LATE_INIT_SEC						__DYNC_INIT_SEC(2)
#define __DYNC_LATE_EXIT_SEC						__DYNC_EXIT_SEC(2)
/*!< *(.dync_init.3), *(.dync_exit.3) */
#define __DYNC_KERNEL_INIT_SEC						__DYNC_INIT_SEC(3)
#define __DYNC_KERNEL_EXIT_SEC						__DYNC_EXIT_SEC(3)
/*!< *(.dync_init.4), *(.dync_exit.4) */
#define __DYNC_RTFS_INIT_SEC						__DYNC_INIT_SEC(4)
#define __DYNC_RTFS_EXIT_SEC						__DYNC_EXIT_SEC(4)
/*!< *(.dync_init.5), *(.dync_exit.5) */
#define __DYNC_PLAT_INIT_SEC						__DYNC_INIT_SEC(5)
#define __DYNC_PLAT_EXIT_SEC						__DYNC_EXIT_SEC(5)
/*!< *(.dync_init.6), *(.dync_exit.6) */
#define __DYNC_PATT_INIT_SEC						__DYNC_INIT_SEC(6)
#define __DYNC_PATT_EXIT_SEC						__DYNC_EXIT_SEC(6)
/*!< *(.dync_init.7), *(.dync_exit.7) */
#define __DYNC_DEV_INIT_SEC							__DYNC_INIT_SEC(7)
#define __DYNC_DEV_EXIT_SEC							__DYNC_EXIT_SEC(7)
/*!< *(.dync_init.8), *(.dync_exit.8) */
#define __DYNC_DRV_INIT_SEC							__DYNC_INIT_SEC(8)
#define __DYNC_DRV_EXIT_SEC							__DYNC_EXIT_SEC(8)
/*!< *(.dync_init.9), *(.dync_exit.9) */
#define __DYNC_PARA_INIT_SEC						__DYNC_INIT_SEC(9)
#define __DYNC_PARA_EXIT_SEC						__DYNC_EXIT_SEC(9)

/*!< persistent params */
#define __static_init								__DYNC_STC_INIT_SEC
#define __static_exit								__DYNC_STC_EXIT_SEC
/*!< temprory params */
#define __dync_init									__DYNC_PARA_INIT_SEC
#define __dync_exit									__DYNC_PARA_EXIT_SEC

/*!< platform(global or general) */
#define __init										__dync_init
#define __exit										__dync_exit

#define __plat_init									__init
#define __plat_exit									__exit

#define __fwk_init									__init
#define __fwk_exit									__exit

/*!< sections list*/
TARGET_EXT const dync_init_t *dync_init_sections[];
TARGET_EXT const dync_exit_t *dync_exit_sections[];

/*!< take every member from start to end */
#define mrt_foreach_member(ptr, start, end)	\
	for (ptr = start; ptr < end; ptr++)

/*!< take every member from start to end */
/*!< add judgement: if (ptr != 0) */
#define mrt_foreach_dync_secs(ptr, start, end)	\
	mrt_foreach_member(ptr, start, end)	\
		if (ptr)

#define FOREACH_DYNC_SEC_ALL(ptr, start, end)		mrt_foreach_dync_secs(ptr, start, end)
#define FOREACH_DYNC_SEC_ANY(ptr, domain, level)	mrt_foreach_dync_secs(ptr, (domain)[level], (domain)[level + 1])
#define FOREACH_DYNC_SEC_INIT(ptr, level)			FOREACH_DYNC_SEC_ANY(ptr, dync_init_sections, level)
#define FOREACH_DYNC_SEC_EXIT(ptr, level)			FOREACH_DYNC_SEC_ANY(ptr, dync_exit_sections, level)

/* for recycle */
/* init */
#define mrt_foreach_init(ptr)						FOREACH_DYNC_SEC_INIT(ptr, NR_DYNC_SEC_START)
#define mrt_foreach_early_init(ptr)					FOREACH_DYNC_SEC_INIT(ptr, NR_DYNC_SEC_EARLY)
#define mrt_foreach_late_init(ptr)					FOREACH_DYNC_SEC_INIT(ptr, NR_DYNC_SEC_LATE)
#define mrt_foreach_kernel_init(ptr)				FOREACH_DYNC_SEC_INIT(ptr, NR_DYNC_SEC_KERNEL)
#define mrt_foreach_rootfs_init(ptr)				FOREACH_DYNC_SEC_INIT(ptr, NR_DYNC_SEC_ROOTFS)
#define mrt_foreach_platform_init(ptr)				FOREACH_DYNC_SEC_INIT(ptr, NR_DYNC_SEC_PLATFORM)
#define mrt_foreach_pattern_init(ptr)				FOREACH_DYNC_SEC_INIT(ptr, NR_DYNC_SEC_PATTERN)
#define mrt_foreach_device_init(ptr)				FOREACH_DYNC_SEC_INIT(ptr, NR_DYNC_SEC_DEVICE)
#define mrt_foreach_driver_init(ptr)				FOREACH_DYNC_SEC_INIT(ptr, NR_DYNC_SEC_DRIVER)

/* exit */
#define mrt_foreach_exit(ptr)						FOREACH_DYNC_SEC_EXIT(ptr, NR_DYNC_SEC_START)
#define mrt_foreach_early_exit(ptr)					FOREACH_DYNC_SEC_EXIT(ptr, NR_DYNC_SEC_EARLY)
#define mrt_foreach_late_exit(ptr)					FOREACH_DYNC_SEC_EXIT(ptr, NR_DYNC_SEC_LATE)
#define mrt_foreach_kernel_exit(ptr)				FOREACH_DYNC_SEC_EXIT(ptr, NR_DYNC_SEC_KERNEL)
#define mrt_foreach_rootfs_exit(ptr)				FOREACH_DYNC_SEC_EXIT(ptr, NR_DYNC_SEC_ROOTFS)
#define mrt_foreach_platform_exit(ptr)				FOREACH_DYNC_SEC_EXIT(ptr, NR_DYNC_SEC_PLATFORM)
#define mrt_foreach_pattern_exit(ptr)				FOREACH_DYNC_SEC_EXIT(ptr, NR_DYNC_SEC_PATTERN)
#define mrt_foreach_device_exit(ptr)				FOREACH_DYNC_SEC_EXIT(ptr, NR_DYNC_SEC_DEVICE)
#define mrt_foreach_driver_exit(ptr)				FOREACH_DYNC_SEC_EXIT(ptr, NR_DYNC_SEC_DRIVER)

/* import */
/* init */
#define IMPORT_EARLY_INIT(x)					    IMPORT_DYNC_INIT_ENTRY(early,x, __DYNC_EARLY_INIT_SEC)
#define IMPORT_LATE_INIT(x)					        IMPORT_DYNC_INIT_ENTRY(late, x, __DYNC_LATE_INIT_SEC)
#define IMPORT_KERNEL_INIT(x)					    IMPORT_DYNC_INIT_ENTRY(os,	 x, __DYNC_KERNEL_INIT_SEC)
#define IMPORT_ROOTFS_INIT(x)					    IMPORT_DYNC_INIT_ENTRY(rtfs, x, __DYNC_RTFS_INIT_SEC)
#define IMPORT_PLATFORM_INIT(x)				        IMPORT_DYNC_INIT_ENTRY(plat, x, __DYNC_PLAT_INIT_SEC)
#define IMPORT_PATTERN_INIT(x)				        IMPORT_DYNC_INIT_ENTRY(patt, x, __DYNC_PATT_INIT_SEC)
#define IMPORT_DEVICE_INIT(x)					    IMPORT_DYNC_INIT_ENTRY(bsp,  x, __DYNC_DEV_INIT_SEC)
#define IMPORT_DRIVER_INIT(x)					    IMPORT_DYNC_INIT_ENTRY(drv,  x, __DYNC_DRV_INIT_SEC)

/* exit */
#define IMPORT_EARLY_EXIT(x)				        IMPORT_DYNC_EXIT_ENTRY(early,x, __DYNC_EARLY_EXIT_SEC)
#define IMPORT_LATE_EXIT(x)					        IMPORT_DYNC_EXIT_ENTRY(late, x, __DYNC_LATE_EXIT_SEC)
#define IMPORT_KERNEL_EXIT(x)				        IMPORT_DYNC_EXIT_ENTRY(os, 	 x, __DYNC_KERNEL_EXIT_SEC)
#define IMPORT_ROOTFS_EXIT(x)				        IMPORT_DYNC_EXIT_ENTRY(rtfs, x, __DYNC_RTFS_EXIT_SEC)
#define IMPORT_PLATFORM_EXIT(x)				        IMPORT_DYNC_EXIT_ENTRY(plat, x, __DYNC_PLAT_EXIT_SEC)
#define IMPORT_PATTERN_EXIT(x)				        IMPORT_DYNC_EXIT_ENTRY(patt, x, __DYNC_PATT_EXIT_SEC)
#define IMPORT_DEVICE_EXIT(x)				        IMPORT_DYNC_EXIT_ENTRY(bsp,  x, __DYNC_DEV_EXIT_SEC)
#define IMPORT_DRIVER_EXIT(x)				        IMPORT_DYNC_EXIT_ENTRY(drv,  x, __DYNC_DRV_EXIT_SEC)

/*!< The functions */
TARGET_EXT kint32_t dync_initcall_run_list(const kuint32_t section);
TARGET_EXT void dync_exitcall_run_list(const kuint32_t section);

TARGET_EXT kint32_t board_early_initcall(void);
TARGET_EXT kint32_t board_late_initcall(void);
TARGET_EXT kint32_t system_kernel_initcall(void);
TARGET_EXT kint32_t filesystem_initcall(void);
TARGET_EXT kint32_t platform_built_initcall(void);
TARGET_EXT kint32_t platform_reality_initcall(void);
TARGET_EXT kint32_t hardware_device_initcall(void);
TARGET_EXT kint32_t abstract_driver_initcall(void);

TARGET_EXT void board_early_exitcall(void);
TARGET_EXT void board_late_exitcall(void);
TARGET_EXT void system_kernel_exitcall(void);
TARGET_EXT void filesystem_exitcall(void);
TARGET_EXT void platform_built_exitcall(void);
TARGET_EXT void platform_reality_exitcall(void);
TARGET_EXT void hardware_device_exitcall(void);
TARGET_EXT void abstract_driver_exitcall(void);

TARGET_EXT kint32_t run_machine_initcall(void);
TARGET_EXT kint32_t run_platform_initcall(void);


#endif /* __IMPLICIT_CALL_H */
