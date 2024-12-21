/*
 * Delay Function Declare
 *
 * File Name:   time.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.10.09
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __TIME_H
#define __TIME_H

/*!< The includes */
#include <common/basic_types.h>
#include <common/error_types.h>
#include <common/generic.h>
#include <configs/configs.h>
#include <common/list_types.h>

/*!< The globals */
typedef kutype_t 	kutime_t;
typedef kstype_t 	kstime_t;

TARGET_EXT kutime_t jiffies;
TARGET_EXT kutime_t jiffies_out;

TARGET_EXT kutime_t *ptr_systick_counter;
TARGET_EXT kutime_t g_delay_timer_counter;

/*!< The defines */
#define TICK_HZ                                             CONFIG_HZ
#define mrt_jiffies                                         (*ptr_systick_counter)

#define JIFFIES_INITVAL                                     (86400000 - 1)
#define JIFFIES_MAX                                         ((kutime_t)(~0))

#define TIMER_DELAY_COUNTER                                 (g_delay_timer_counter)
#define TIMER_DELAY_COUNTER_INIT                            (0U)
#define TIMER_DELAY_COUNTER_MAX                             (0x00ffffffU)                   /*!< = 16777215 */
#define TIMER_DELAY_COUNTER_MAJOR                           ((kuint32_t)(((TIMER_DELAY_COUNTER) >> 24U) & 0x000000ffU))
#define TIMER_DELAY_COUNTER_MINOR                           ((kuint32_t)((TIMER_DELAY_COUNTER) & (TIMER_DELAY_COUNTER_MAX)))

struct time_spec
{
    /*!< time = tv_sec  */
	kutime_t tv_sec;			        	                /*!< seconds */
	kutime_t tv_nsec;		                                /*!< nanoseconds */
};

#define mrt_is_timespec_empty(t)                            (((t)->tv_sec == 0) && ((t)->tv_nsec == 0))

struct timer_list 
{
	struct list_head sgrt_link;
	kutime_t expires;

	void (*entry)(kuint32_t args);
	kuint32_t data;
};

#define TIMER_INITIALIZER(_entry, _expires, _data)		\
{   \
    .expires = _expires,    \
    .entry = _entry,    \
    .data = _data,  \
}

#define DEFINE_TIMER(_name, _entry, _expires, _data)		\
	struct timer_list _name = TIMER_INITIALIZER(_entry, _expires, _data)

#define mrt_setup_timer(timer, fn, data)    \
	do {    \
		init_list_head(&(timer)->sgrt_link);  \
		(timer)->entry = (fn); \
		(timer)->data = (data);   \
	} while (0)

#define mrt_time_check_type(a, b)   \
	const kutime_t _a = 0;	\
	const kutime_t _b = 0;	\
	(void)(&_a == &(a));	\
    (void)(&_b == &(b));

/*!< a > b ? true : false */
#define mrt_time_after(a, b)    \
({  \
    mrt_time_check_type(a, b)   \
	((a) > (b));   \
})

/*!< a >= b ? true : false */
#define mrt_time_after_eq(a, b)	\
({  \
    mrt_time_check_type(a, b)   \
	((a) >= (b));   \
})

#define mrt_time_before(a, b)                               mrt_time_after(b, a)            /*!< a < b ? true : false */
#define mrt_time_before_eq(a, b)                            mrt_time_after_eq(b, a)         /*!< a <= b ? true : false */

/*!< The functions */
TARGET_EXT void simple_delay_timer_initial(void);
TARGET_EXT void simple_delay_timer_runs(void);

TARGET_EXT void delay_cnt(kuint32_t n);
TARGET_EXT void delay_s(kuint32_t n_s);
TARGET_EXT void delay_ms(kuint32_t n_ms);
TARGET_EXT void delay_us(kuint32_t n_us);
TARGET_EXT void wait_secs(kuint32_t seconds);
TARGET_EXT void wait_msecs(kuint32_t milseconds);
TARGET_EXT void wait_usecs(kuint32_t useconds);

TARGET_EXT void setup_timer(struct timer_list *sprt_timer, void (*entry)(kuint32_t), kuint32_t data);
TARGET_EXT void add_timer(struct timer_list *sprt_timer);
TARGET_EXT void del_timer(struct timer_list *sprt_timer);
TARGET_EXT kbool_t find_timer(struct timer_list *sprt_timer);
TARGET_EXT void mod_timer(struct timer_list *sprt_timer, kutime_t expires);
TARGET_EXT void do_timer_event(void);

/*!< API functions */
/*!
 * @brief   jiffies increment
 * @param   none
 * @retval  none
 * @note    for jiffies counter (called by timer irq handler)
 */
static inline void get_time_counter(void)
{
    jiffies = (jiffies >= JIFFIES_MAX) ? 0 : (jiffies + 1);
    jiffies_out = jiffies ? jiffies_out : (jiffies_out + 1);
}

/*!
 * @brief   convert jiffies to seconds
 * @param   jiffies
 * @retval  s
 * @note    none
 */
static inline kuint32_t jiffies_to_secs(const kutime_t j)
{
    return (j / TICK_HZ);
}

/*!
 * @brief   convert jiffies to milseconds
 * @param   jiffies
 * @retval  ms
 * @note    none
 */
static inline kuint32_t jiffies_to_msecs(const kutime_t j)
{
    return ((j * 1000) / TICK_HZ);
}

/*!
 * @brief   convert jiffies to micro seconds
 * @param   jiffies
 * @retval  us
 * @note    none
 */
static inline kuint32_t jiffies_to_usecs(const kutime_t j)
{
    return ((j * 1000 * 1000) / TICK_HZ);
}

/*!
 * @brief   convert jiffies to nseconds
 * @param   jiffies
 * @retval  ns
 * @note    none
 */
static inline kuint64_t jiffies_to_nsecs(const kutime_t j)
{
    return ((j * 1000 * 1000 * 1000) / TICK_HZ);
}

/*!
 * @brief   convert seconds to jiffies
 * @param   s
 * @retval  jiffies
 * @note    none
 */
static inline kutime_t secs_to_jiffies(const kuint32_t s)
{
    return (s * TICK_HZ);
}

/*!
 * @brief   convert milseconds to jiffies
 * @param   ms
 * @retval  jiffies
 * @note    none
 */
static inline kutime_t msecs_to_jiffies(const kuint32_t m)
{
    return m ? (((m * TICK_HZ) / 1000) + 1) : 0;
}

/*!
 * @brief   convert micro seconds to jiffies
 * @param   us
 * @retval  jiffies
 * @note    none
 */
static inline kutime_t usecs_to_jiffies(const kuint32_t u)
{
    return u ? (((u * TICK_HZ) / 1000 / 1000) + 1) : 0;
}

/*!
 * @brief   convert nseconds to jiffies
 * @param   ns
 * @retval  jiffies
 * @note    none
 */
static inline kutime_t nsecs_to_jiffies(const kuint64_t n)
{
    return n ? (((n * TICK_HZ) / 1000 / 1000 / 1000) + 1) : 0;
}

/*!
 * @brief   time_spec ---> ms
 * @param   sprt_tm
 * @retval  ms
 * @note    none
 */
static inline kutime_t time_spec_to_msecs(struct time_spec *sprt_tm)
{
    return ((sprt_tm->tv_sec * 1000) + (sprt_tm->tv_nsec / 1000 / 1000));
}

/*!
 * @brief   ms ---> time_spec
 * @param   sprt_tm, m
 * @retval  sprt_tm
 * @note    none
 */
static inline struct time_spec *msecs_to_time_spec(struct time_spec *sprt_tm, const kuint32_t m)
{
    sprt_tm->tv_sec  = m / 1000;
    sprt_tm->tv_nsec = (m - (sprt_tm->tv_sec * 1000)) * 1000 * 1000;
    
    return sprt_tm;
}

#endif /* __TIME_H */
