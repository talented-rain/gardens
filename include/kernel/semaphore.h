/*
 * Semaphore Interface Defines
 *
 * File Name:   semaphore.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.06.01
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __SEMAPHORE_H
#define __SEMAPHORE_H

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The includes */
#include <common/atomic_types.h>
#include <kernel/kernel.h>

/*!< The defines */
typedef struct semaphore
{
	struct atomic sgrt_atc;

} srt_semaphore_t;

#define SEMAPHORE_INITIALIZE()	\
{ 	\
	.sgrt_atc = ATOMIC_INIT()	\
}

/*!< The functions */
extern void sema_init(struct semaphore *sprt_sem, kuint32_t val);
extern void sema_down(struct semaphore *sprt_sem);
extern kint32_t sema_down_try_lock(struct semaphore *sprt_sem);
extern void sema_up(struct semaphore *sprt_sem);

/*!< API functions */
/*!
 * @brief   check if semaphore is locked
 * @param   sprt_sem
 * @retval  locked(true) / unlocked(false)
 * @note    none
 */
static inline kbool_t sema_is_locked(struct semaphore *sprt_sem)
{
	return (0 == ATOMIC_READ(&sprt_sem->sgrt_atc));
}

#ifdef __cplusplus
    }
#endif

#endif /* __SEMAPHORE_H */
