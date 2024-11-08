/*
 * C/C++ Header Defines For ARMv7
 *
 * File Name:   gcc_config.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.09.30
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __GCC_CONFIG_H
#define __GCC_CONFIG_H

#include "asm_config.h"
#include <common/basic_types.h>

/*!< The defines */
/*!
 * @brief  	mrt_enable_cpu_irq
 * @param  	none
 * @retval 	none
 * @note   	enable irq
 */
#define mrt_enable_cpu_irq()    \
{   \
    __asm__ __volatile__ (  \
        " cpsie i   "   \
        :   \
        :   \
        : "memory"  \
    );   \
}

/*!
 * @brief  	mrt_disable_cpu_irq
 * @param  	none
 * @retval 	none
 * @note   	disable irq
 */
#define mrt_disable_cpu_irq()    \
    do {   \
        __asm__ __volatile__ (  \
            " cpsid i   "   \
            :   \
            :   \
            : "memory"  \
        );   \
    } while (0)

/*!
 * @brief  	mrt_enable_cpu_fiq
 * @param  	none
 * @retval 	none
 * @note   	enable fiq
 */
#define mrt_enable_cpu_fiq()    \
    do {   \
        __asm__ __volatile__ (  \
            " cpsie f   "   \
            :   \
            :   \
            : "memory"  \
        );   \
    } while (0)

/*!
 * @brief  	mrt_disable_cpu_fiq
 * @param  	none
 * @retval 	none
 * @note   	disable fiq
 */
#define mrt_disable_cpu_fiq()    \
    do {   \
        __asm__ __volatile__ (  \
            " cpsid f   "   \
            :   \
            :   \
            : "memory"  \
        );   \
    } while (0)

/*!
 * @brief  	mrt_nop
 * @param  	none
 * @retval 	none
 * @note   	do nothing
 */
#define mrt_nop()                                   do { __asm__ __volatile__ ("nop"); } while (0)
#define mrt_delay_nop()                             mrt_nop()

/*!
 * @brief  	mrt_wfi
 * @param  	none
 * @retval 	none
 * @note   	wait for Interrupt
 */
#define mrt_wfi()                                   do { __asm__ __volatile__ ("wfi"); } while (0)

/*!
 * @brief  	mrt_wfe
 * @param  	none
 * @retval 	none
 * @note   	wait for Event
 */
#define mrt_wfe()                                   do { __asm__ __volatile__ ("wfe"); } while (0)

/*!
 * @brief  	mrt_sev
 * @param  	none
 * @retval 	none
 * @note   	send Event
 */
#define mrt_sev()                                   do { __asm__ __volatile__ ("sev"); } while (0)

/*!
 * @brief  	mrt_barrier
 * @param  	none
 * @retval 	none
 * @note   	barrier
 */
#define mrt_barrier()                               do { __asm__ __volatile__ ("": : : "memory"); } while (0)

/*!
 * @brief  	mrt_isb
 * @param  	none
 * @retval 	none
 * @note   	Instruction Synchronization Barrier
 */
#define mrt_isb()    \
    do {   \
        __asm__ __volatile__ (  \
            " isb 0xf   "   \
            :   \
            :   \
            : "memory"  \
        );   \
    } while (0)

/*!
 * @brief  	mrt_dsb
 * @param  	none
 * @retval 	none
 * @note   	Data Synchronization Barrier
 */
#define mrt_dsb()    \
    do {   \
        __asm__ __volatile__ (  \
            " dsb 0xf   "   \
            :   \
            :   \
            : "memory"  \
        );   \
    } while (0)

/*!
 * @brief  	mrt_dmb
 * @param  	none
 * @retval 	none
 * @note   	Data Memory Synchronization Barrier
 */
#define mrt_dmb()    \
    do {   \
        __asm__ __volatile__ (  \
            " dmb 0xf   "   \
            :   \
            :   \
            : "memory"  \
        );   \
    } while (0)

/*!
 * @brief  	mrt_set_cp15
 * @param  	op: such as "0, %0, c0, c0, 0"
 * @retval 	none
 * @note   	write rn to cp15
 */
#define mrt_set_cp15(op, rn)    \
    do {    \
        __asm__ __volatile__ (  \
            " mcr p15, "op   \
            :   \
            : "r"(rn)   \
            : "memory"  \
        );  \
    } while (0)

/*!
 * @brief  	mrt_get_cp15
 * @param  	op: such as "0, %0, c0, c0, 0"
 * @retval 	none
 * @note   	read cp15 to rn
 */
#define mrt_get_cp15(op, rn)    \
    do {    \
        __asm__ __volatile__ (  \
            " mrc p15, "op   \
            : "=r"(rn)  \
            :   \
            : "memory"  \
        );  \
    } while (0)

/*!< API functions */
/*!
 * @brief  	__get_primask
 * @param  	none
 * @retval 	none
 * @note   	read super register: PRIMASK
 */
static inline kuint32_t __get_primask(void)
{
    kuint32_t result;

    __asm__ __volatile__ (
        " mrs %0, primask   "
        : "=r"(result)
    );

    return result;
}

/*!
 * @brief  	__set_primask
 * @param  	none
 * @retval 	none
 * @note   	write super register: PRIMASK
 */
static inline void __set_primask(kuint32_t result)
{
    __asm__ __volatile__ (
        " msr primask, %0   "
        :
        : "r"(result)
        : "memory"
    );
}

/*!
 * @brief  	__get_cp15_sctlr
 * @param  	none
 * @retval 	none
 * @note   	read cp15 SCTLR
 */
static inline kuint32_t __get_cp15_sctlr(void)
{
    kuint32_t result;

    __asm__ __volatile__ (
        " mrc p15, 0, %0, c1, c0, 0  "
        : "=r"(result)
    );

    return result;
}

/*!
 * @brief  	__set_cp15_sctlr
 * @param  	none
 * @retval 	none
 * @note   	write cp15 SCTLR
 */
static inline void __set_cp15_sctlr(kuint32_t data)
{
    __asm__ __volatile__ (
        " mcr p15, 0, %0, c1, c0, 0  "
        :
        : "r"(data)
        : "memory"
    );
}

/*!
 * @brief  	__set_cp15_vbar
 * @param  	none
 * @retval 	none
 * @note   	set cp15 vector base address
 */
static inline void __set_cp15_vbar(kuint32_t address)
{
    __asm__ __volatile__ (
        " mcr p15, 0, %0, c12, c0, 0 "
        :
        : "r"(address)
        : "memory"
    );
}

/*!
 * @brief  	__get_cp15_cbar
 * @param  	none
 * @retval 	none
 * @note   	get cp15 vector base address
 */
static inline kuint32_t __get_cp15_cbar(void)
{
    kuint32_t result;

    __asm__ __volatile__ (
        " mrc p15, 4, %0, c15, c0, 0 "
        : "=r"(result)
        : 
        : "memory"
    );

    return result;
}

/*!
 * @brief  	__set_cp15_tlbs
 * @param  	none
 * @retval 	none
 * @note   	set cp15 TLBS
 */
static inline void __set_cp15_tlbs(kuint32_t result)
{
    __asm__ __volatile__ (
        " mcr p15, 0, %0, c8, c7, 0  "
        :
        : "r"(result)
        : "memory"
    );
}

/*!
 * @brief  	__set_cp15_icache
 * @param  	none
 * @retval 	none
 * @note   	set cp15 I-Cache
 */
static inline void __set_cp15_icache(kuint32_t result)
{
    __asm__ __volatile__ (
        " mcr p15, 0, %0, c7, c5, 0  "
        :
        : "r"(result)
        : "memory"
    );
}

/*!
 * @brief  	__set_cp15_bp
 * @param  	none
 * @retval 	none
 * @note   	set cp15 Branch Prediction
 */
static inline void __set_cp15_bp(kuint32_t result)
{
    __asm__ __volatile__ (
        " mcr p15, 0, %0, c7, c5, 6  "
        :
        : "r"(result)
        : "memory"
    );
}

/*!
 * @brief  	__set_cp15_dsb
 * @param  	none
 * @retval 	none
 * @note   	set cp15 Data Synchronization Barrier
 */
static inline void __set_cp15_dsb(kuint32_t result)
{
    __asm__ __volatile__ (
        " mcr p15, 0, %0, c7, c10, 4 "
        :
        : "r"(result)
        : "memory"
    );
}

/*!
 * @brief  	__set_cp15_isb
 * @param  	none
 * @retval 	none
 * @note   	set cp15 Intruction Synchronization Barrier
 */
static inline void __set_cp15_isb(kuint32_t result)
{
    __asm__ __volatile__ (
        " mcr p15, 0, %0, c7, c5, 4  "
        :
        : "r"(result)
        : "memory"
    );
}

/*!
 * @brief  	__get_cpsr
 * @param  	none
 * @retval 	cpsr
 * @note   	get cpsr
 */
static inline kuint32_t __get_cpsr(void)
{
    kuint32_t result = 0;

    __asm__ __volatile__ (
        " mrs %0, cpsr  "
        : "=&r"(result)
        :
        : "memory"
    );

    return result;
}

/*!
 * @brief  	__set_cpsr
 * @param  	cpsr
 * @retval 	none
 * @note   	set cpsr
 */
static inline void __set_cpsr(kuint32_t result)
{
    __asm__ __volatile__ (
        " msr cpsr, %0  "
        : 
        : "r"(result)
        : "memory"
    );
}

#endif /* __GCC_CONFIG_H */
