/*
 * User Thread Instance (display task) Interface
 *
 * File Name:   lvgl_task.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.11.17
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
#include <platform/video/fwk_bitmap.h>
#include <kernel/kernel.h>
#include <kernel/sched.h>
#include <kernel/thread.h>
#include <kernel/sleep.h>
#include <kernel/mutex.h>
#include <kernel/mailbox.h>
#include <fs/fs_intr.h>

#include "../../thread_table.h"
#include "app/app.h"

/*!< The defines */
#define LVGL_TASK_THREAD_STACK_SIZE                       REAL_THREAD_STACK_PAGE(1)    /*!< 1 page (4kbytes) */

/*!< The globals */
static tid_t g_lvgl_task_tid;
static struct real_thread_attr sgrt_lvgl_task_attr;
static kuint32_t g_lvgl_task_stack[LVGL_TASK_THREAD_STACK_SIZE];

/*!< API functions */
/*!
 * @brief  initial display common settings
 * @param  none
 * @retval none
 * @note   do display
 */
static void lvgl_task_settings_init(struct fwk_font_setting *sprt_set)
{
    sprt_set->color = RGB_BLACK;
    sprt_set->background = RGB_WHITE;
    sprt_set->font = NR_FWK_FONT_SONG;
    sprt_set->line_spacing = 8;
    sprt_set->word_spacing = 2;
    sprt_set->ptr_ascii = (void *)g_font_ascii_song16;
    sprt_set->ptr_hz = fwk_font_hz_song16_get()->base;
    sprt_set->size = FWK_FONT_16;

    sprt_set->left_spacing  = 16;
    sprt_set->right_spacing = 8;
    sprt_set->upper_spacing = 8;
    sprt_set->down_spacing  = 16;
}

/*!
 * @brief  display task
 * @param  none
 * @retval none
 * @note   do display
 */
static void *lvgl_task_entry(void *args)
{
    kint32_t fd;
    struct fwk_fb_fix_screen_info sgrt_fix;
	struct fwk_fb_var_screen_info sgrt_var;
    kuint32_t *fb_buffer1, *fb_buffer2;
    struct fwk_disp_ctrl sgrt_dctrl;
    struct fwk_disp_info sgrt_disp;

    sgrt_dctrl.sprt_di = &sgrt_disp;
    lvgl_task_settings_init(&sgrt_dctrl.sgrt_set);

    fd = virt_open("/dev/fb0", O_RDWR);
    if (fd < 0)
        goto fail1;

    virt_ioctl(fd, NR_FB_IOGET_VARINFO, &sgrt_var);
    virt_ioctl(fd, NR_FB_IOGET_FIXINFO, &sgrt_fix);

    fb_buffer1 = (kuint32_t *)virt_mmap(mrt_nullptr, sgrt_fix.smem_len, 0, 0, fd, 0);
    if (!isValid(fb_buffer1))
        goto fail2;

    fb_buffer2 = (kuint32_t *)virt_mmap(mrt_nullptr, sgrt_fix.smem_len, 0, 0, fd, sgrt_fix.smem_len);
    if (!isValid(fb_buffer2))
        goto fail3;

    fwk_display_ctrl_init(&sgrt_disp, fb_buffer1, fb_buffer2, sgrt_fix.smem_len, 
                        sgrt_var.xres, sgrt_var.yres, sgrt_var.bits_per_pixel >> 3);

    lvgl_task_startup(&sgrt_dctrl);

    for (;;)
    {
        lvgl_task(&sgrt_dctrl);
        schedule_delay_ms(200);
    }

    virt_munmap(fb_buffer2, sgrt_fix.smem_len);
fail3:
    virt_munmap(fb_buffer1, sgrt_fix.smem_len);
fail2:
    virt_close(fd);
fail1:
    schedule_self_suspend();

    return args;
}

/*!
 * @brief	create display app task
 * @param  	none
 * @retval 	error code
 * @note   	none
 */
kint32_t lvgl_task_init(void)
{
    struct real_thread_attr *sprt_attr = &sgrt_lvgl_task_attr;
    kint32_t retval;

	sprt_attr->detachstate = REAL_THREAD_CREATE_JOINABLE;
	sprt_attr->inheritsched	= REAL_THREAD_INHERIT_SCHED;
	sprt_attr->schedpolicy = REAL_THREAD_SCHED_FIFO;

    /*!< thread stack */
	real_thread_set_stack(sprt_attr, mrt_nullptr, &g_lvgl_task_stack[0], sizeof(g_lvgl_task_stack));
    /*!< lowest priority */
	real_thread_set_priority(sprt_attr, REAL_THREAD_PROTY_DEFAULT);
    /*!< default time slice */
    real_thread_set_time_slice(sprt_attr, 100);

    /*!< register thread */
    retval = real_thread_create(&g_lvgl_task_tid, sprt_attr, lvgl_task_entry, mrt_nullptr);
    if (!retval)
        real_thread_set_name(g_lvgl_task_tid, "lvgl_task_entry");

    return retval;
}

/*!< end of file */
