/*
 * Mutex Lock Interface Defines
 *
 * File Name:   mutex.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.04.05
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __MUTEX_H
#define __MUTEX_H

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The includes */
#include <common/atomic_types.h>
#include <kernel/kernel.h>

/*!< The defines */
typedef struct mutex_lock
{
	struct atomic sgrt_atc;

} srt_mutex_lock_t;

#define MUTEX_LOCK_INIT()					{ .sgrt_atc = ATOMIC_INIT() }

/*!< The functions */
extern void mutex_init(struct mutex_lock *sprt_lock);
extern void mutex_lock(struct mutex_lock *sprt_lock);
extern kint32_t mutex_try_lock(struct mutex_lock *sprt_lock);
extern void mutex_unlock(struct mutex_lock *sprt_lock);

/*!< API functions */
/*!
 * @brief   check if mutex is locked
 * @param   sprt_lock
 * @retval  locked(true) / unlocked(false)
 * @note    none
 */
static inline kbool_t mutex_is_locked(struct mutex_lock *sprt_lock)
{
	return !!ATOMIC_READ(&sprt_lock->sgrt_atc);
}

#ifdef __cplusplus
    }
#endif

#endif /* __MUTEX_H */
