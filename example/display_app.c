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
#include <platform/video/fwk_bitmap.h>
#include <kernel/kernel.h>
#include <kernel/sched.h>
#include <kernel/thread.h>
#include <kernel/sleep.h>
#include <kernel/mutex.h>
#include <fs/fs_intr.h>

#include "thread_table.h"

/*!< The defines */
#define DISPLAY_APP_THREAD_STACK_SIZE                       REAL_THREAD_STACK_PAGE(1)    /*!< 1 page (4kbytes) */
#define DISPLAY_APP_FILE_BUF_LEN                            (1920 * 4)

typedef struct display_para
{
    struct fwk_disp_info sgrt_disp;
    struct fwk_font_setting sgrt_set;

} srt_dispaly_para_t;

/*!< The globals */
static real_thread_t g_display_app_tid;
static struct real_thread_attr sgrt_display_app_attr;
static kuint32_t g_display_app_stack[DISPLAY_APP_THREAD_STACK_SIZE];

static kuint8_t g_file_for_display_buffer[DISPLAY_APP_FILE_BUF_LEN] __align(4);

static const kchar_t *g_bmp_file_list[] =
{
    "/media/FAT32_2/home/fox/picture/1.bmp",
    "/media/FAT32_2/home/fox/picture/2.bmp",
};

/*!< API functions */
/*!
 * @brief  initial display common settings
 * @param  none
 * @retval none
 * @note   do display
 */
static void user_display_settings_init(struct fwk_font_setting *sprt_set)
{
    sprt_set->color = RGB_BLACK;
    sprt_set->background = RGB_WHITE;
    sprt_set->font = NR_FWK_FONT_SONG;
    sprt_set->line_spacing = 2;
    sprt_set->ptr_ascii = (void *)g_font_ascii_song16;
    sprt_set->ptr_hz = mrt_nullptr;
    sprt_set->size = FWK_FONT_16;
}

/*!
 * @brief  clear display
 * @param  none
 * @retval none
 * @note   do display
 */
static void user_display_clear(struct display_para *sprt_para)
{
    struct fwk_disp_info *sprt_disp = &sprt_para->sgrt_disp;
    struct fwk_font_setting *sprt_set = &sprt_para->sgrt_set;

    if (sprt_disp->sprt_ops->clear)
        sprt_disp->sprt_ops->clear(sprt_disp, sprt_set->background);
}

/*!
 * @brief  display task
 * @param  none
 * @retval none
 * @note   do display
 */
static void user_display_word(struct display_para *sprt_para, kuint32_t x_start, kuint32_t y_start, 
                                kuint32_t x_end, kuint32_t y_end, const kchar_t *fmt, ...)
{
    struct fwk_disp_info *sprt_disp = &sprt_para->sgrt_disp;
    struct fwk_font_setting *sprt_set = &sprt_para->sgrt_set;

    if (sprt_disp->sprt_ops->fill_rectangle)
        sprt_disp->sprt_ops->fill_rectangle(sprt_disp, x_start, y_start, 
                                        x_end, y_end, sprt_set->background);

    if (sprt_disp->sprt_ops->write_word)
        sprt_disp->sprt_ops->write_word(sprt_disp, sprt_set, x_start, y_start, x_end, y_end, fmt);
}

/*!
 * @brief  display pattern
 * @param  none
 * @retval none
 * @note   do display
 */
static void user_display_graphic(struct display_para *sprt_para, kuint32_t x_start, 
                            kuint32_t y_start, kuint32_t x_end, kuint32_t y_end)
{
    struct fwk_disp_info *sprt_disp = &sprt_para->sgrt_disp;
    struct fwk_font_setting *sprt_set = &sprt_para->sgrt_set;
    
    if (sprt_disp->sprt_ops->write_line)
        sprt_disp->sprt_ops->write_line(sprt_disp, x_start, y_start, 
                                        x_end, y_end, sprt_set->color);

    if (sprt_disp->sprt_ops->write_rectangle)
        sprt_disp->sprt_ops->write_rectangle(sprt_disp, x_start, y_start, 
                                        x_end, y_end, sprt_set->background);
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
    kuint32_t *fb_buffer1 = mrt_nullptr;
    kuint32_t *fb_buffer2 = mrt_nullptr;
    struct fwk_disp_info *sprt_disp;
    struct fwk_font_setting *sprt_set;
    struct display_para sgrt_para;
    struct fs_stream *sprt_filp;
    kuint8_t *fs_buffer = &g_file_for_display_buffer[0];
    struct fwk_bmp_ctrl sgrt_bctl;
    kssize_t size, offset;

    sprt_disp = &sgrt_para.sgrt_disp;
    sprt_set  = &sgrt_para.sgrt_set;

    user_display_settings_init(sprt_set);

    for (;;)
    {        
        kuint32_t *fb_buffer;
        kuint8_t index = 0;

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

        while (true)
        {
            fb_buffer = index ? fb_buffer1 : fb_buffer2;
            fwk_display_ctrl_init(sprt_disp, fb_buffer, sgrt_fix.smem_len, 
                                sgrt_var.xres, sgrt_var.yres, sgrt_var.bits_per_pixel >> 3);
            fwk_bitmap_ctrl_init(&sgrt_bctl, sprt_disp, 0, 0);

            sprt_filp = file_open(g_bmp_file_list[index], O_RDONLY);
            if (!isValid(sprt_filp))
                goto loop1;

            size = file_read(sprt_filp, fs_buffer, sizeof(sgrt_bctl.sgrt_bi) + 14);
            if (!size)
                goto loop2;
 
            offset = fwk_bitmap_get_and_check(&sgrt_bctl, fs_buffer);
            if (offset < 0)
                goto loop2;

            do {
                file_lseek(sprt_filp, offset);

                size = file_read(sprt_filp, fs_buffer, DISPLAY_APP_FILE_BUF_LEN);
                if (size <= 0)
                    break;

                fwk_display_bitmap(&sgrt_bctl, fs_buffer, size);
                offset += size;

            } while (size > 0);

            file_lseek(sprt_filp, 0);

            sgrt_var.yoffset = index ? 0 : sgrt_var.yres;
            virt_ioctl(fd, NR_FB_IOSET_VARINFO, &sgrt_var);

        loop2:
            file_close(sprt_filp);
        loop1:
            schedule_delay_ms(500);
            index ^= 0x01;
        }

    fail3:
        virt_munmap(fb_buffer1, sgrt_fix.smem_len);
    fail2:
        virt_close(fd);
    fail1:
        schedule_self_suspend();    
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
    real_thread_set_time_slice(sprt_attr, 100);

    /*!< register thread */
    retval = real_thread_create(&g_display_app_tid, sprt_attr, display_app_entry, mrt_nullptr);
    if (!retval)
        real_thread_set_name(g_display_app_tid, "display_app_entry");

    return retval;
}

/*!< end of file */
