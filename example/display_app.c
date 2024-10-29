/*
 * User Thread Instance (display task) Interface
 *
 * File Name:   display_app.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.04.01
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The globals */
#include <common/basic_types.h>
#include <common/error_types.h>
#include <common/generic.h>
#include <common/io_stream.h>
#include <platform/fwk_fcntl.h>
#include <platform/video/fwk_fbmem.h>
#include <platform/video/fwk_font.h>
#include <platform/video/fwk_disp.h>
#include <platform/video/fwk_rgbmap.h>
#include <kernel/kernel.h>
#include <kernel/sched.h>
#include <kernel/thread.h>
#include <kernel/sleep.h>
#include <kernel/mutex.h>

#include "thread_table.h"

/*!< The defines */
#define DISPLAY_APP_THREAD_STACK_SIZE                       REAL_THREAD_STACK_PAGE(1)    /*!< 1 page (4kbytes) */

/*!< The globals */
static real_thread_t g_display_app_tid;
static struct real_thread_attr sgrt_display_app_attr;
static kuint32_t g_display_app_stack[DISPLAY_APP_THREAD_STACK_SIZE];
static struct mutex_lock sgrt_display_app_lock;

/*!< API functions */
/*!
 * @brief  initial display common settings
 * @param  none
 * @retval none
 * @note   do display
 */
static void display_initial_settings(struct fwk_font_setting *sprt_settings)
{
    sprt_settings->color = RGB_BLACK;
    sprt_settings->background = RGB_WHITE;
    sprt_settings->font = NR_FWK_FONT_SONG;
    sprt_settings->line_spacing = 2;
    sprt_settings->ptr_ascii = (void *)g_font_ascii_song16;
    sprt_settings->ptr_hz = mrt_nullptr;
    sprt_settings->size = FWK_FONT_16;
}

/*!
 * @brief  clear display
 * @param  none
 * @retval none
 * @note   do display
 */
static void display_clear(struct fwk_disp_info *sprt_disp)
{
    struct fwk_font_setting sgrt_settings;

    display_initial_settings(&sgrt_settings);

    if (sprt_disp->sprt_ops->clear)
        sprt_disp->sprt_ops->clear(sprt_disp, sgrt_settings.background);
}

/*!
 * @brief  fill display
 * @param  none
 * @retval none
 * @note   do display
 */
static void display_fill(struct fwk_disp_info *sprt_disp, kuint32_t color)
{
    if (sprt_disp->sprt_ops->clear)
        sprt_disp->sprt_ops->clear(sprt_disp, color);
}

/*!
 * @brief  display task
 * @param  none
 * @retval none
 * @note   do display
 */
static void display_word(struct fwk_disp_info *sprt_disp, kuint32_t x_start, kuint32_t y_start, kuint32_t x_end, kuint32_t y_end, const kchar_t *fmt, ...)
{
    struct fwk_font_setting sgrt_settings;

    display_initial_settings(&sgrt_settings);
    sgrt_settings.color = RGB_RED;

    if (sprt_disp->sprt_ops->fill_rectangle)
        sprt_disp->sprt_ops->fill_rectangle(sprt_disp, x_start, y_start, x_end, y_end, sgrt_settings.background);

    if (sprt_disp->sprt_ops->write_word)
        sprt_disp->sprt_ops->write_word(sprt_disp, &sgrt_settings, x_start, y_start, x_end, y_end, fmt);
}

/*!
 * @brief  display pattern
 * @param  none
 * @retval none
 * @note   do display
 */
static void display_graphic(struct fwk_disp_info *sprt_disp, kuint32_t x_start, kuint32_t y_start, kuint32_t x_end, kuint32_t y_end)
{
    struct fwk_font_setting sgrt_settings;

    display_initial_settings(&sgrt_settings);
    
    if (sprt_disp->sprt_ops->write_line)
    {
        sgrt_settings.color = RGB_BLUE;
        sprt_disp->sprt_ops->write_line(sprt_disp, x_start, y_start, x_end, y_end, sgrt_settings.color);
    }

    if (sprt_disp->sprt_ops->write_rectangle)
    {
        sgrt_settings.color = RGB_PURPLE;
        sprt_disp->sprt_ops->write_rectangle(sprt_disp, x_start, y_start, x_end, y_end, sgrt_settings.color);
    }
}

/*!
 * @brief  display task
 * @param  none
 * @retval none
 * @note   do display
 */
static void *display_app_entry(void *args)
{
    kint32_t fd;
    struct fwk_fb_fix_screen_info sgrt_fix = {};
	struct fwk_fb_var_screen_info sgrt_var = {};
    kuint32_t *fbuffer = mrt_nullptr;
    struct fwk_disp_info sgrt_disp;

    real_thread_set_name(__FUNCTION__);
    mutex_init(&sgrt_display_app_lock);

    do {
        fd = virt_open("/dev/fb0", O_RDWR);
        if (fd < 0)
            schedule_thread();

        virt_ioctl(fd, NR_FB_IOGetVScreenInfo, &sgrt_var);
        virt_ioctl(fd, NR_FB_IOGetFScreenInfo, &sgrt_fix);

        fbuffer = (kuint32_t *)virt_mmap(mrt_nullptr, sgrt_fix.smem_len, 0, 0, fd, 0);
        if (!isValid(fbuffer))
        {
            virt_close(fd);
            schedule_thread();
        }
    
    } while (!isValid(fbuffer));

    fwk_display_initial_info(&sgrt_disp, fbuffer, sgrt_fix.smem_len, sgrt_var.xres, sgrt_var.yres, FWK_RGB_PIXEL32);

    display_clear(&sgrt_disp);
    display_graphic(&sgrt_disp, sgrt_disp.width / 4, (sgrt_disp.height * 3) / 8, (sgrt_disp.width * 3) / 4, (sgrt_disp.height * 7) / 8);

    virt_munmap(fbuffer, sgrt_fix.smem_len);
    virt_close(fd);

    for (;;)
    {
        fd = virt_open("/dev/fb0", 0);
        if (fd < 0)
            goto END;

        fbuffer = (kuint32_t *)virt_mmap(mrt_nullptr, sgrt_fix.smem_len, 0, 0, fd, 0);
        if (!isValid(fbuffer))
        {
            virt_close(fd);
            goto END;
        }

        sgrt_disp.buffer = fbuffer;
        
        display_fill(&sgrt_disp, RGB_BLUE);
        display_word(&sgrt_disp, 140, 80, 
                                 140 + 20 * 8, 80 + 16, "welcome to use!");
        schedule_delay_ms(500);
        
        display_fill(&sgrt_disp, RGB_PURPLE);
        display_word(&sgrt_disp, 140, 80, 
                                 140 + 20 * 8, 80 + 16, "happly every day!");

        virt_munmap(fbuffer, sgrt_fix.smem_len);
        virt_close(fd);

END:
        schedule_delay_ms(500);
    }

    return args;
}

/*!
 * @brief	create display app task
 * @param  	none
 * @retval 	error code
 * @note   	none
 */
kint32_t display_app_init(void)
{
    struct real_thread_attr *sprt_attr = &sgrt_display_app_attr;
    kint32_t retval;

	sprt_attr->detachstate = REAL_THREAD_CREATE_JOINABLE;
	sprt_attr->inheritsched	= REAL_THREAD_INHERIT_SCHED;
	sprt_attr->schedpolicy = REAL_THREAD_SCHED_FIFO;

    /*!< thread stack */
	real_thread_set_stack(sprt_attr, mrt_nullptr, &g_display_app_stack[0], sizeof(g_display_app_stack));
    /*!< lowest priority */
	real_thread_set_priority(sprt_attr, REAL_THREAD_PROTY_DEFAULT);
    /*!< default time slice */
    real_thread_set_time_slice(sprt_attr, REAL_THREAD_TIME_DEFUALT);

    /*!< register thread */
    retval = real_thread_create(&g_display_app_tid, sprt_attr, display_app_entry, mrt_nullptr);
    return (retval < 0) ? retval : 0;
}

/*!< end of file */
