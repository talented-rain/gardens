/*
 * Simple Delay General Function
 *
 * File Name:   delay.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.10.09
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <configs/configs.h>
#include <common/time.h>

/*!< The defines */
#define DELAY_SIMPLE_COUNTER_PER_MS								(0x7ff)
#define DELAY_SIMPLE_COUNTER_PER_S								(DELAY_SIMPLE_COUNTER_PER_MS * 1000)
#define DELAY_SIMPLE_COUNTER_PER_US								(DELAY_SIMPLE_COUNTER_PER_MS / 1000)

/*!< The globals */
volatile kutime_t jiffies = JIFFIES_INITVAL;
volatile kutime_t jiffies_out = 0;

kutime_t *ptr_systick_counter = mrt_nullptr;
kutime_t g_delay_timer_counter = 0;

static kuint32_t g_simple_delay_timer = 0;
static kuint32_t g_simple_timeout_cnt = 0;

static DECLARE_LIST_HEAD(sgrt_global_timer_list);

/*!< API function */
/*!
 * @brief   simple_delay_timer_initial
 * @param   none
 * @retval  none
 * @note    delay timer counter initial
 */
void simple_delay_timer_initial(void)
{
	g_simple_delay_timer = TIMER_DELAY_COUNTER_INIT;
	g_simple_timeout_cnt = TIMER_DELAY_COUNTER_INIT;

	g_delay_timer_counter = TIMER_DELAY_COUNTER_INIT;
}

/*!
 * @brief   simple_delay_timer_runs
 * @param   none
 * @retval  none
 * @note    delay timer counter excute
 */
void simple_delay_timer_runs(void)
{
    if ((g_simple_delay_timer++) >= TIMER_DELAY_COUNTER_MAX)
    {
        g_simple_delay_timer = TIMER_DELAY_COUNTER_INIT;
        g_simple_timeout_cnt = (g_simple_timeout_cnt >= 255) ? TIMER_DELAY_COUNTER_INIT : (g_simple_timeout_cnt + 1);
    }

	g_delay_timer_counter = mrt_bit_mask(g_simple_timeout_cnt, ~TIMER_DELAY_COUNTER_MAX, 24U) + g_simple_delay_timer;
}

/*!
 * @brief   delay_cnt
 * @param   n
 * @retval  none
 * @note    delay n counters
 */
void delay_cnt(kuint32_t n)
{
	while (n--)
		mrt_delay_nop();
}

/*!
 * @brief   delay_s
 * @param   n_s
 * @retval  none
 * @note    delay n_s seconds
 */
__weak void delay_s(kuint32_t n_s)
{
	while (n_s--)
		delay_cnt(DELAY_SIMPLE_COUNTER_PER_S);
}

/*!
 * @brief   delay_ms
 * @param   n_ms
 * @retval  none
 * @note    delay n_ms miliseconds
 */
__weak void delay_ms(kuint32_t n_ms)
{
	while (n_ms--)
		delay_cnt(DELAY_SIMPLE_COUNTER_PER_MS);
}

/*!
 * @brief   delay_us
 * @param   n_us
 * @retval  none
 * @note    delay n_us microseconds
 */
__weak void delay_us(kuint32_t n_us)
{
	while (n_us--)
		delay_cnt(DELAY_SIMPLE_COUNTER_PER_US);
}

/*!
 * @brief   delay (unit: s)
 * @param   seconds
 * @retval  none
 * @note    none
 */
void wait_secs(kuint32_t seconds)
{
    kutime_t count = jiffies + secs_to_jiffies(seconds);
    
    while (mrt_time_before(jiffies, count));
}

/*!
 * @brief   delay (unit: ms)
 * @param   milseconds
 * @retval  none
 * @note    none
 */
void wait_msecs(kuint32_t milseconds)
{
    kutime_t count = jiffies + msecs_to_jiffies(milseconds);
    
    while (mrt_time_before(jiffies, count));
}

/*!
 * @brief   delay (unit: us)
 * @param   useconds
 * @retval  none
 * @note    none
 */
void wait_usecs(kuint32_t useconds)
{
    kutime_t count = jiffies + usecs_to_jiffies(useconds);
    
    while (mrt_time_before(jiffies, count));
}

/*!
 * @brief   initial timer
 * @param   sprt_timer: timer
 * @param	entry: timeout function
 * @param	data: argument for timeout function
 * @retval  none
 * @note    none
 */
void setup_timer(struct timer_list *sprt_timer, void (*entry)(kuint32_t), kuint32_t data)
{
	if (!isValid(sprt_timer))
		return;

	mrt_setup_timer(sprt_timer, entry, data);
}

/*!
 * @brief   add timer to global list
 * @param   sprt_timer
 * @retval  none
 * @note    systick interrupt will traverses the global list
 */
void add_timer(struct timer_list *sprt_timer)
{
	if ((!isValid(sprt_timer)) || 
		(!sprt_timer->expires))
		return;

	list_head_add_tail(&sgrt_global_timer_list, &sprt_timer->sgrt_link);
}

/*!
 * @brief   delete timer from global list
 * @param   sprt_timer
 * @retval  none
 * @note    none
 */
void del_timer(struct timer_list *sprt_timer)
{
	if (!isValid(sprt_timer))
		return;

	list_head_del_safe(&sgrt_global_timer_list, &sprt_timer->sgrt_link);
}

/*!
 * @brief   find timer from global list
 * @param   sprt_timer
 * @retval  none
 * @note    1: found; 0: not found
 */
kbool_t find_timer(struct timer_list *sprt_timer)
{
	struct timer_list *sprt_any;

	foreach_list_next_entry(sprt_any, &sgrt_global_timer_list, sgrt_link)
	{
		if (sprt_timer == sprt_any)
			return true;
	}

	return false;
}

/*!
 * @brief   modilfy timer period
 * @param   sprt_timer
 * @param	expires: period
 * @retval  none
 * @note    if timer has not been added to list, add it right away
 */
void mod_timer(struct timer_list *sprt_timer, kutime_t expires)
{
	if (!isValid(sprt_timer))
		return;

	sprt_timer->expires = expires;
	
#if 0
	if (!find_timer(sprt_timer)) {
#else
	if (mrt_list_head_empty(&sprt_timer->sgrt_link)) {
#endif
		add_timer(sprt_timer);
	}
}

/*!
 * @brief   timer handler
 * @param   none
 * @param	none
 * @retval  none
 * @note    called by systick interrupt
 */
void do_timer_event(void)
{
	struct timer_list *sprt_timer;

	foreach_list_next_entry(sprt_timer, &sgrt_global_timer_list, sgrt_link)
	{
		if (!sprt_timer->expires)
			continue;

		if (mrt_time_after(jiffies, sprt_timer->expires))
		{
			if (sprt_timer->entry)
				sprt_timer->entry(sprt_timer->data);
		}
	}
}

/* end of file */
