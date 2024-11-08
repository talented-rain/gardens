/*
 * Dynamic initial/exit defines
 *
 * File Name:   implicit_call.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.11.26
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <common/generic.h>
#include <boot/boot_text.h>
#include <boot/implicit_call.h>

/*!< The defines */
#define DYNAMIC_INIT_EXIT_WAYS                             IT_TRUE		/*!< 1: use API to init/exit; 0: use Macro defines */

/*!< The globals */
const dync_init_t *dync_init_sections[] =
{
    [NR_DYNC_SEC_STATIC]   = &__dync_init_0_start,
    [NR_DYNC_SEC_EARLY]    = &__dync_init_1_start,
    [NR_DYNC_SEC_LATE]     = &__dync_init_2_start,
    [NR_DYNC_SEC_KERNEL]   = &__dync_init_3_start,
    [NR_DYNC_SEC_ROOTFS]   = &__dync_init_4_start,
    [NR_DYNC_SEC_PLATFORM] = &__dync_init_5_start,
    [NR_DYNC_SEC_PATTERN]  = &__dync_init_6_start,
    [NR_DYNC_SEC_DEVICE]   = &__dync_init_7_start,
    [NR_DYNC_SEC_DRIVER]   = &__dync_init_8_start,
    [NR_DYNC_SEC_OTHERS]   = &__dync_init_9_start,

    [NR_DYNC_SEC_START]    = &__dync_init_start,
    [NR_DYNC_SEC_END]      = &__dync_init_end,
};

const dync_exit_t *dync_exit_sections[] =
{
    [NR_DYNC_SEC_STATIC]   = &__dync_exit_0_start,
    [NR_DYNC_SEC_EARLY]    = &__dync_exit_1_start,
    [NR_DYNC_SEC_LATE]     = &__dync_exit_2_start,
    [NR_DYNC_SEC_KERNEL]   = &__dync_exit_3_start,
    [NR_DYNC_SEC_ROOTFS]   = &__dync_exit_4_start,
    [NR_DYNC_SEC_PLATFORM] = &__dync_exit_5_start,
    [NR_DYNC_SEC_PATTERN]  = &__dync_exit_6_start,
    [NR_DYNC_SEC_DEVICE]   = &__dync_exit_7_start,
    [NR_DYNC_SEC_DRIVER]   = &__dync_exit_8_start,
    [NR_DYNC_SEC_OTHERS]   = &__dync_exit_9_start,

    [NR_DYNC_SEC_START]    = &__dync_exit_start,
    [NR_DYNC_SEC_END]      = &__dync_exit_end,
};

/* API functions */
/*!
 * @brief   dync_initcall_run_list
 * @param   none
 * @retval  none
 * @note    call every function from .dync.init 
 */
kint32_t dync_initcall_run_list(const kuint32_t section)
{
	const dync_init_t *pFunc_init;

    if (section >= NR_DYNC_SEC_END)
        return -ER_NOMEM;

	for (pFunc_init = dync_init_sections[section]; (*pFunc_init) && (pFunc_init < dync_init_sections[section + 1]); pFunc_init++)
	{
		if (0 > ((*pFunc_init)()))
			return -ER_ERROR;
	}

	return ER_NORMAL;
}

/*!
 * @brief   dync_exitcall_run_list
 * @param   none
 * @retval  none
 * @note    call every function from .dync.exit 
 */
void dync_exitcall_run_list(const kuint32_t section)
{
	const dync_exit_t *pFunc_exit;

    if (section >= NR_DYNC_SEC_END)
        return;

	for (pFunc_exit = dync_exit_sections[section]; (*pFunc_exit) && (pFunc_exit < dync_exit_sections[section + 1]); pFunc_exit++)
	{
		(*pFunc_exit)();
	}
}

/*!
 * @brief   board_early_initcall
 * @param   none
 * @retval  none
 * @note    if (*pFunc_init)() return error, other initcalls will be stopped immediately
 */
kint32_t board_early_initcall(void)
{
#if DYNAMIC_INIT_EXIT_WAYS
    return dync_initcall_run_list(NR_DYNC_SEC_EARLY);

#else
	dync_init_t *pFunc_init;

	mrt_foreach_early_init(pFunc_init)
	{
		if (0 > ((*pFunc_init)()))
			return -ER_ERROR;
	}

    return ER_NORMAL;

#endif
}

/*!
 * @brief   board_early_exitcall
 * @param   none
 * @retval  none
 * @note    none
 */
void board_early_exitcall(void)
{
#if DYNAMIC_INIT_EXIT_WAYS
    dync_exitcall_run_list(NR_DYNC_SEC_EARLY);

#else
	dync_exit_t *pFunc_exit;

	mrt_foreach_early_exit(pFunc_exit)
	{
        (*pFunc_exit)();
	}

#endif
}

/*!
 * @brief   board_late_initcall
 * @param   none
 * @retval  none
 * @note    if (*pFunc_init)() return error, other initcalls will be stopped immediately
 */
kint32_t board_late_initcall(void)
{
#if DYNAMIC_INIT_EXIT_WAYS
    return dync_initcall_run_list(NR_DYNC_SEC_LATE);

#else
	dync_init_t *pFunc_init;

	mrt_foreach_early_init(pFunc_init)
	{
		if (0 > ((*pFunc_init)()))
			return -ER_ERROR;
	}

    return ER_NORMAL;

#endif
}

/*!
 * @brief   board_late_exitcall
 * @param   none
 * @retval  none
 * @note    none
 */
void board_late_exitcall(void)
{
#if DYNAMIC_INIT_EXIT_WAYS
    dync_exitcall_run_list(NR_DYNC_SEC_LATE);

#else
	dync_exit_t *pFunc_exit;

	mrt_foreach_early_exit(pFunc_exit)
	{
        (*pFunc_exit)();
	}

#endif
}

/*!
 * @brief   system_kernel_initcall
 * @param   none
 * @retval  none
 * @note    if (*pFunc_init)() return error, other initcalls will be stopped immediately
 */
kint32_t system_kernel_initcall(void)
{
#if DYNAMIC_INIT_EXIT_WAYS
    return dync_initcall_run_list(NR_DYNC_SEC_KERNEL);

#else
	dync_init_t *pFunc_init;

	mrt_foreach_early_init(pFunc_init)
	{
		if (0 > ((*pFunc_init)()))
			return -ER_ERROR;
	}

    return ER_NORMAL;

#endif
}

/*!
 * @brief   system_kernel_exitcall
 * @param   none
 * @retval  none
 * @note    none
 */
void system_kernel_exitcall(void)
{
#if DYNAMIC_INIT_EXIT_WAYS
    dync_exitcall_run_list(NR_DYNC_SEC_KERNEL);

#else
	dync_exit_t *pFunc_exit;

	mrt_foreach_early_exit(pFunc_exit)
	{
        (*pFunc_exit)();
	}

#endif
}

/*!
 * @brief   filesystem_initcall
 * @param   none
 * @retval  none
 * @note    if (*pFunc_init)() return error, other initcalls will be stopped immediately
 */
kint32_t filesystem_initcall(void)
{
#if DYNAMIC_INIT_EXIT_WAYS
    return dync_initcall_run_list(NR_DYNC_SEC_ROOTFS);

#else
	dync_init_t *pFunc_init;

	mrt_foreach_early_init(pFunc_init)
	{
		if (0 > ((*pFunc_init)()))
			return -ER_ERROR;
	}

    return ER_NORMAL;

#endif
}

/*!
 * @brief   filesystem_exitcall
 * @param   none
 * @retval  none
 * @note    none
 */
void filesystem_exitcall(void)
{
#if DYNAMIC_INIT_EXIT_WAYS
    dync_exitcall_run_list(NR_DYNC_SEC_ROOTFS);

#else
	dync_exit_t *pFunc_exit;

	mrt_foreach_early_exit(pFunc_exit)
	{
        (*pFunc_exit)();
	}

#endif
}

/*!
 * @brief   platform_built_initcall
 * @param   none
 * @retval  none
 * @note    if (*pFunc_init)() return error, other initcalls will be stopped immediately
 */
kint32_t platform_built_initcall(void)
{
#if DYNAMIC_INIT_EXIT_WAYS
    return dync_initcall_run_list(NR_DYNC_SEC_PLATFORM);

#else
	dync_init_t *pFunc_init;

	mrt_foreach_early_init(pFunc_init)
	{
		if (0 > ((*pFunc_init)()))
			return -ER_ERROR;
	}

    return ER_NORMAL;

#endif
}

/*!
 * @brief   platform_built_exitcall
 * @param   none
 * @retval  none
 * @note    none
 */
void platform_built_exitcall(void)
{
#if DYNAMIC_INIT_EXIT_WAYS
    dync_exitcall_run_list(NR_DYNC_SEC_PLATFORM);

#else
	dync_exit_t *pFunc_exit;

	mrt_foreach_early_exit(pFunc_exit)
	{
        (*pFunc_exit)();
	}

#endif
}

/*!
 * @brief   platform_reality_initcall
 * @param   none
 * @retval  none
 * @note    if (*pFunc_init)() return error, other initcalls will be stopped immediately
 */
kint32_t platform_reality_initcall(void)
{
#if DYNAMIC_INIT_EXIT_WAYS
    return dync_initcall_run_list(NR_DYNC_SEC_PATTERN);

#else
	dync_init_t *pFunc_init;

	mrt_foreach_early_init(pFunc_init)
	{
		if (0 > ((*pFunc_init)()))
			return -ER_ERROR;
	}

    return ER_NORMAL;

#endif
}

/*!
 * @brief   platform_reality_exitcall
 * @param   none
 * @retval  none
 * @note    none
 */
void platform_reality_exitcall(void)
{
#if DYNAMIC_INIT_EXIT_WAYS
    dync_exitcall_run_list(NR_DYNC_SEC_PATTERN);

#else
	dync_exit_t *pFunc_exit;

	mrt_foreach_early_exit(pFunc_exit)
	{
        (*pFunc_exit)();
	}

#endif
}

/*!
 * @brief   hardware_device_initcall
 * @param   none
 * @retval  none
 * @note    if (*pFunc_init)() return error, other initcalls will be stopped immediately
 */
kint32_t hardware_device_initcall(void)
{
#if DYNAMIC_INIT_EXIT_WAYS
    return dync_initcall_run_list(NR_DYNC_SEC_DEVICE);

#else
	dync_init_t *pFunc_init;

	mrt_foreach_early_init(pFunc_init)
	{
		if (0 > ((*pFunc_init)()))
			return -ER_ERROR;
	}

    return ER_NORMAL;

#endif
}

/*!
 * @brief   hardware_device_exitcall
 * @param   none
 * @retval  none
 * @note    none
 */
void hardware_device_exitcall(void)
{
#if DYNAMIC_INIT_EXIT_WAYS
    dync_exitcall_run_list(NR_DYNC_SEC_DEVICE);

#else
	dync_exit_t *pFunc_exit;

	mrt_foreach_early_exit(pFunc_exit)
	{
        (*pFunc_exit)();
	}

#endif
}

/*!
 * @brief   abstract_driver_initcall
 * @param   none
 * @retval  none
 * @note    if (*pFunc_init)() return error, other initcalls will be stopped immediately
 */
kint32_t abstract_driver_initcall(void)
{
#if DYNAMIC_INIT_EXIT_WAYS
    return dync_initcall_run_list(NR_DYNC_SEC_DRIVER);

#else
	dync_init_t *pFunc_init;

	mrt_foreach_early_init(pFunc_init)
	{
		if (0 > ((*pFunc_init)()))
			return -ER_ERROR;
	}

    return ER_NORMAL;

#endif
}

/*!
 * @brief   abstract_driver_exitcall
 * @param   none
 * @retval  none
 * @note    none
 */
void abstract_driver_exitcall(void)
{
#if DYNAMIC_INIT_EXIT_WAYS
    dync_exitcall_run_list(NR_DYNC_SEC_DRIVER);

#else
	dync_exit_t *pFunc_exit;

	mrt_foreach_early_exit(pFunc_exit)
	{
        (*pFunc_exit)();
	}

#endif
}

/*!
 * @brief   run_machine_initcall
 * @param   none
 * @retval  none
 * @note    if (*pFunc_init)() return error, other initcalls will be stopped immediately
 */
kint32_t run_machine_initcall(void)
{
	kint32_t retval;

    retval = board_early_initcall();
    if (retval < 0)
        return retval;

    retval = board_late_initcall();
    if (retval < 0)
        return retval;

	return 0;
}

/*!
 * @brief   run_platform_initcall
 * @param   none
 * @retval  none
 * @note    if (*pFunc_init)() return error, other initcalls will be stopped immediately
 */
kint32_t run_platform_initcall(void)
{
	kint32_t retval;

    /*!< initcall */
    retval = platform_built_initcall();
    if (retval < 0)
        return retval;

    retval = platform_reality_initcall();
    if (retval < 0)
        return retval;

    retval = hardware_device_initcall();
    if (retval < 0)
        return retval;

    retval = abstract_driver_initcall();
    if (retval < 0)
        return retval;

	return 0;
}

/* end of file */
