/*
 * Read/Write Lock Interface
 *
 * File Name:   rw_lock.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2025.01.21
 *
 * Copyright (c) 2025   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The globals */
#include <kernel/kernel.h>
#include <kernel/rw_lock.h>
#include <kernel/sched.h>

/*!< The defines */


/*!< The functions */


/*!< API functions */
/*!
 * @brief   initial rw_lock lock
 * @param   sprt_lock
 * @retval  none
 * @note    set count = 0
 */
void rw_lock_init(struct rw_lock *sprt_lock)
{
    if (sprt_lock)
    {
        ATOMIC_SET(&sprt_lock->sgrt_read, 0);
        ATOMIC_SET(&sprt_lock->sgrt_write, 0);
    }
}

/*!
 * @brief   rw_lock lock
 * @param   sprt_lock
 * @retval  none
 * @note    if it has been locked, schedule another thread; otherwise, lock it
 */
void rw_lock(struct rw_lock *sprt_lock)
{
    if (!mrt_current)
        return;

    while (rw_is_locked(sprt_lock))
        schedule_thread();
    
    atomic_inc(&sprt_lock->sgrt_read);
    atomic_inc(&sprt_lock->sgrt_write);
}

/*!
 * @brief   rw_lock lock
 * @param   sprt_lock
 * @retval  none
 * @note    if it has been locked, return directly
 */
kint32_t rw_try_lock(struct rw_lock *sprt_lock)
{
    if (!mrt_current)
        return -ER_FORBID;

    if (rw_is_locked(sprt_lock))
        return -ER_BUSY;
    
    atomic_inc(&sprt_lock->sgrt_read);
    atomic_inc(&sprt_lock->sgrt_write);

    return ER_NORMAL;
}

/*!
 * @brief   rw_lock unlock
 * @param   sprt_lock
 * @retval  none
 * @note    none
 */
void rw_unlock(struct rw_lock *sprt_lock)
{
    if (!mrt_current || !rw_is_locked(sprt_lock))
        return;
    
    atomic_dec(&sprt_lock->sgrt_read);
    atomic_dec(&sprt_lock->sgrt_write);
}

/*!
 * @brief   read lock
 * @param   sprt_lock
 * @retval  none
 * @note    if it has been locked, schedule another thread; otherwise, lock it
 */
void rd_lock(struct rw_lock *sprt_lock)
{
    if (!mrt_current)
        return;

    while (wr_is_locked(sprt_lock))
        schedule_thread();
    
    atomic_inc(&sprt_lock->sgrt_read);
}

/*!
 * @brief   rw_lock lock
 * @param   sprt_lock
 * @retval  none
 * @note    if it has been locked, return directly
 */
kint32_t rd_try_lock(struct rw_lock *sprt_lock)
{
    if (!mrt_current)
        return -ER_FORBID;

    if (wr_is_locked(sprt_lock))
        return -ER_BUSY;
    
    atomic_inc(&sprt_lock->sgrt_read);

    return ER_NORMAL;
}

/*!
 * @brief   rw_lock unlock
 * @param   sprt_lock
 * @retval  none
 * @note    none
 */
void rd_unlock(struct rw_lock *sprt_lock)
{
    if (!mrt_current || !rd_is_locked(sprt_lock))
        return;
    
    atomic_dec(&sprt_lock->sgrt_read);
}

/*!
 * @brief   write lock lock
 * @param   sprt_lock
 * @retval  none
 * @note    if it has been locked, schedule another thread; otherwise, lock it
 */
void wr_lock(struct rw_lock *sprt_lock)
{
    if (!mrt_current)
        return;

    while (rw_is_locked(sprt_lock))
        schedule_thread();
    
    atomic_inc(&sprt_lock->sgrt_write);
}

/*!
 * @brief   write lock lock
 * @param   sprt_lock
 * @retval  none
 * @note    if it has been locked, return directly
 */
kint32_t wr_try_lock(struct rw_lock *sprt_lock)
{
    if (!mrt_current)
        return -ER_FORBID;

    if (rw_is_locked(sprt_lock))
        return -ER_BUSY;
    
    atomic_inc(&sprt_lock->sgrt_write);

    return ER_NORMAL;
}

/*!
 * @brief   write lock unlock
 * @param   sprt_lock
 * @retval  none
 * @note    none
 */
void wr_unlock(struct rw_lock *sprt_lock)
{
    if (!mrt_current || !rw_is_locked(sprt_lock))
        return;
    
    atomic_dec(&sprt_lock->sgrt_write);
}

/*!< end of file */
