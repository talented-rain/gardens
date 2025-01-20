/*
 * Spin Lock Interface Defines
 *
 * File Name:   spinlock.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.04.05
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __SPIN_LOCK_H
#define __SPIN_LOCK_H

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The includes */
#include <common/atomic_types.h>
#include <kernel/kernel.h>

/*!< The defines */
typedef struct spin_lock
{
	struct atomic sgrt_atc;
	kuint32_t flag;

} srt_spin_lock_t;

#define DECLARE_SPIN_LOCK(lock)	\
	struct spin_lock lock = { .sgrt_atc = ATOMIC_INIT(), .flag = 0 }

#define SPIN_LOCK_INIT()	\
	{ .sgrt_atc = ATOMIC_INIT(), .flag = 0 }

/*!< The functions */
extern void spin_lock_init(struct spin_lock *sprt_lock);
extern void spin_lock(struct spin_lock *sprt_lock);
extern void spin_unlock(struct spin_lock *sprt_lock);
extern kint32_t spin_try_lock(struct spin_lock *sprt_lock);
extern void spin_lock_irq(struct spin_lock *sprt_lock);
extern kint32_t spin_try_lock_irq(struct spin_lock *sprt_lock);
extern void spin_unlock_irq(struct spin_lock *sprt_lock);
extern void spin_lock_irqsave(struct spin_lock *sprt_lock);
extern kint32_t spin_try_lock_irqsave(struct spin_lock *sprt_lock);
extern void spin_unlock_irqrestore(struct spin_lock *sprt_lock);

/*!< API functions */
/*!
 * @brief   check if spinlock is locked
 * @param   sprt_lock
 * @retval  locked(true) / unlocked(false)
 * @note    none
 */
static inline kbool_t spin_is_locked(struct spin_lock *sprt_lock)
{
	return !!ATOMIC_READ(&sprt_lock->sgrt_atc);
}

#ifdef __cplusplus
    }
#endif

#endif /* __SPIN_LOCK_H */
