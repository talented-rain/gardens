/*
 * Spin Lock Interface
 *
 * File Name:   spinlock.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.04.05
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The globals */
#include <kernel/kernel.h>
#include <kernel/spinlock.h>
#include <kernel/sched.h>

/*!< The defines */


/*!< The functions */


/*!< API functions */
/*!
 * @brief   initial spin lock
 * @param   sprt_lock
 * @retval  none
 * @note    set count = 0
 */
void spin_lock_init(struct spin_lock *sprt_lock)
{
    if (isValid(sprt_lock))
        ATOMIC_SET(&sprt_lock->sgrt_atc, 0);
}

/*!
 * @brief   spin lock
 * @param   sprt_lock
 * @retval  none
 * @note    if it has been locked, schedule another thread
 */
void spin_lock(struct spin_lock *sprt_lock)
{
    while (spin_is_locked(sprt_lock));
    
    mrt_preempt_disable();
    mrt_barrier();
    atomic_inc(&sprt_lock->sgrt_atc);
}

/*!
 * @brief   spin unlock
 * @param   sprt_lock
 * @retval  none
 * @note    none
 */
void spin_unlock(struct spin_lock *sprt_lock)
{
    if (!spin_is_locked(sprt_lock))
        return;

    atomic_dec(&sprt_lock->sgrt_atc);
    mrt_barrier();
    mrt_preempt_enable();
}

/*!
 * @brief   try spin lock
 * @param   sprt_lock
 * @retval  none
 * @note    if it has been locked, return right away
 */
kint32_t spin_try_lock(struct spin_lock *sprt_lock)
{
    if (spin_is_locked(sprt_lock))
        return -ER_LOCKED;

    mrt_preempt_disable();
    mrt_barrier();
    atomic_inc(&sprt_lock->sgrt_atc);
    
    return ER_NORMAL;
}

/*!< for SMP (if is single core, do not need to use them) */
/*!
 * @brief   spin lock and disable irq
 * @param   sprt_lock
 * @retval  none
 * @note    none
 */
void spin_lock_irq(struct spin_lock *sprt_lock)
{
    spin_lock(sprt_lock);
    mrt_disable_cpu_irq();
}

/*!
 * @brief   try spin lock and disable irq
 * @param   sprt_lock
 * @retval  1: lock success; 0: lock fail
 * @note    none
 */
kint32_t spin_try_lock_irq(struct spin_lock *sprt_lock)
{
    if (spin_try_lock(sprt_lock))
        return -ER_LOCKED;

    mrt_barrier();
    mrt_disable_cpu_irq();
    return ER_NORMAL;
}

/*!
 * @brief   spin unlock and enable irq
 * @param   sprt_lock
 * @retval  none
 * @note    none
 */
void spin_unlock_irq(struct spin_lock *sprt_lock)
{
    if (!spin_is_locked(sprt_lock))
        return;
    
    spin_unlock(sprt_lock);
    mrt_barrier();
    mrt_enable_cpu_irq();
}

/*!
 * @brief   spin lock and save current irq status
 * @param   sprt_lock
 * @retval  none
 * @note    none
 */
void spin_lock_irqsave(struct spin_lock *sprt_lock)
{
    spin_lock(sprt_lock);
    sprt_lock->flag = __get_cpsr();
    mrt_disable_cpu_irq();
}

/*!
 * @brief   try spin lock and save current irq status
 * @param   sprt_lock
 * @retval  1: lock success; 0: lock fail
 * @note    none
 */
kint32_t spin_try_lock_irqsave(struct spin_lock *sprt_lock)
{
    if (spin_try_lock(sprt_lock))
        return -ER_LOCKED;

    sprt_lock->flag = __get_cpsr();
    sprt_lock->flag &= CPSR_BIT_I;

    mrt_barrier();
    mrt_disable_cpu_irq();

    return ER_NORMAL;
}

/*!
 * @brief   spin unlock and restore irq status
 * @param   sprt_lock
 * @retval  none
 * @note    none
 */
void spin_unlock_irqrestore(struct spin_lock *sprt_lock)
{
    if (!spin_is_locked(sprt_lock))
        return;
    
    spin_unlock(sprt_lock);
    mrt_barrier();

    /*!< bit4 ~ bit0 is mode bit, which are not equaled to 0 */
    if (!(sprt_lock->flag & CPSR_BIT_I))
        mrt_enable_cpu_irq();

    sprt_lock->flag = 0;
}

/*!< end of file */
