/*
 * Read/Write Lock Interface Defines
 *
 * File Name:   rw_lock.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2025.01.21
 *
 * Copyright (c) 2025   Yang Yujun <yujiantianhu@163.com>
 *
 * \note:
 * 1. if sgrt_read is locked, forbidden to write, but allowed to read for everyone;
 * 2. if sgrt_write is locked, forbidden to read and write until being unlocked
 */

#ifndef __RW_LOCK_H
#define __RW_LOCK_H

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The includes */
#include <common/atomic_types.h>
#include <kernel/kernel.h>

/*!< The defines */
typedef struct rw_lock
{
    struct atomic sgrt_read;
    struct atomic sgrt_write;

} srt_rw_lock_t;

#define RW_LOCK_INIT()					{ .sgrt_read = ATOMIC_INIT(), .sgrt_write = ATOMIC_INIT() }

/*!< The functions */
extern void rw_lock_init(struct rw_lock *sprt_lock);
extern void rw_lock(struct rw_lock *sprt_lock);
extern kint32_t rw_try_lock(struct rw_lock *sprt_lock);
extern void rw_unlock(struct rw_lock *sprt_lock);

extern void rd_lock(struct rw_lock *sprt_lock);
extern kint32_t rd_try_lock(struct rw_lock *sprt_lock);
extern void rd_unlock(struct rw_lock *sprt_lock);

extern void wr_lock(struct rw_lock *sprt_lock);
extern kint32_t wr_try_lock(struct rw_lock *sprt_lock);
extern void wr_unlock(struct rw_lock *sprt_lock);

/*!< API functions */
/*!
 * @brief   check if read_lock is locked
 * @param   sprt_lock
 * @retval  locked(true) / unlocked(false)
 * @note    none
 */
static inline kbool_t rd_is_locked(struct rw_lock *sprt_lock)
{
	return !!ATOMIC_READ(&sprt_lock->sgrt_read);
}

/*!
 * @brief   check if write_lock is locked
 * @param   sprt_lock
 * @retval  locked(true) / unlocked(false)
 * @note    none
 */
static inline kbool_t wr_is_locked(struct rw_lock *sprt_lock)
{
	return !!ATOMIC_READ(&sprt_lock->sgrt_write);
}

/*!
 * @brief   check if rw_lock is locked
 * @param   sprt_lock
 * @retval  locked(true) / unlocked(false)
 * @note    none
 */
static inline kbool_t rw_is_locked(struct rw_lock *sprt_lock)
{
    return rd_is_locked(sprt_lock) || wr_is_locked(sprt_lock);
}

#ifdef __cplusplus
    }
#endif

#endif /* __RW_LOCK_H */
