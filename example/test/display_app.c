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
#include <kernel/mailbox.h>
#include <fs/fs_intr.h>

#include "test_app.h"

/*!< The defines */
#define DISPLAY_APP_THREAD_STACK_SIZE                       THREAD_STACK_PAGE(1)    /*!< 1 page (4kbytes) */

/*!< The globals */
static tid_t g_display_app_tid;
static struct thread_attr sgrt_display_app_attr;
static kuint32_t g_display_app_stack[DISPLAY_APP_THREAD_STACK_SIZE];

static kchar_t g_display_buffer[((1920 * 1080) / (FWK_FONT_16 * FWK_FONT_16)) * 2 + 4] __align(4);
static struct mailbox sgrt_display_app_mailbox;

static kuint32_t g_display_text_pages[1024];
static kuint32_t g_text_cur_page;
static const kchar_t *g_display_ebook_list[] =
{
    "/media/FAT32_2/home/fox/text/tangzhan.txt",
    "/media/FAT32_2/home/fox/text/lianjian.txt",
    "/media/FAT32_2/home/fox/text/feidao.txt",
    "/media/FAT32_2/home/fox/text/tianlong.txt",
};
static const kchar_t *g_display_logo = "/media/FAT32_2/home/fox/picture/1.bmp";

/*!< API functions */
/*!
 * @brief  initial display common settings
 * @param  none
 * @retval none
 * @note   do display
 */
static void display_app_settings_init(struct fwk_font_setting *sprt_set)
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
 * @brief  clear display
 * @param  none
 * @retval none
 * @note   do display
 */
static void display_app_clear(struct fwk_disp_ctrl *sprt_dctrl)
{
    fwk_display_clear(sprt_dctrl->sprt_di, sprt_dctrl->sgrt_set.background);
}

/*!
 * @brief  display offset
 * @param  none
 * @retval none
 * @note   do display
 */
static void display_app_cursor(struct fwk_disp_ctrl *sprt_dctrl, 
                        kuint32_t x_start, kuint32_t y_start, kuint32_t x_end, kuint32_t y_end)
{
    struct fwk_disp_info *sprt_disp = sprt_dctrl->sprt_di;
    struct fwk_font_setting *sprt_set = &sprt_dctrl->sgrt_set;

    fwk_display_set_cursor(sprt_dctrl, x_start, y_start, x_end, y_end);
    fwk_display_fill_rectangle(sprt_disp, x_start, y_start, 
                                        x_end, y_end, sprt_set->background);
}

/*!
 * @brief  display logo
 * @param  none
 * @retval none
 * @note   do display
 */
static void display_app_logo(struct fwk_disp_ctrl *sprt_dctrl)
{
    struct fwk_disp_info *sprt_disp = sprt_dctrl->sprt_di;
    struct fs_stream *sprt_file;
    struct fwk_bmp_ctrl sgrt_bctl;
    kuint8_t bytes_per_pixel;
    kssize_t size;

    display_app_clear(sprt_dctrl);
    display_app_cursor(sprt_dctrl, 0, 0, sprt_disp->width, sprt_disp->height);

    sprt_file = file_open(g_display_logo, O_RDONLY);
    if (!isValid(sprt_file))
        return;

    bytes_per_pixel = sprt_disp->bpp >> 3;
    size = file_read(sprt_file, sprt_disp->buffer_bak, sprt_disp->width * sprt_disp->height * bytes_per_pixel);
    if (size <= 0)
        goto END;

    fwk_bitmap_ctrl_init(&sgrt_bctl, sprt_disp, 0, 0);
    fwk_display_whole_bitmap(&sgrt_bctl, sprt_disp->buffer_bak);

END:
    file_close(sprt_file);
}

/*!
 * @brief  display text
 * @param  none
 * @retval none
 * @note   do display
 */
static kssize_t display_app_text(kint32_t fd, struct mailbox *sprt_mb,
                        struct fs_stream *sprt_file, struct fwk_disp_ctrl *sprt_dctrl)
{
    struct fwk_disp_info *sprt_disp = sprt_dctrl->sprt_di;
    struct mail *sprt_mail;
    struct fwk_fb_var_screen_info sgrt_var;
    kuint8_t op = 0;
    kssize_t size, offset = 0;
    kusize_t page_index = 0;

    fwk_display_frame_exchange(sprt_disp);
    display_app_clear(sprt_dctrl);
    display_app_cursor(sprt_dctrl, 0, 0, sprt_disp->width, sprt_disp->height);

    page_index = g_text_cur_page;
    offset = g_display_text_pages[page_index];
    while (!IS_DISP_FRAME_FULL(sprt_dctrl))
    {
        file_lseek(sprt_file, offset);
        size = file_read(sprt_file, g_display_buffer, sizeof(g_display_buffer) - 4);
        if (size <= 0)
            return 0;

        g_display_buffer[size] = '\0';
        offset += fwk_display_word(sprt_dctrl, g_display_buffer);
    }

    virt_ioctl(fd, NR_FB_IOGET_VARINFO, &sgrt_var);
    if (!sgrt_var.yoffset)
        sgrt_var.yoffset += sgrt_var.yres;
    else
        sgrt_var.yoffset = 0;
    
    virt_ioctl(fd, NR_FB_IOSET_VARINFO, &sgrt_var);

    do {
        sprt_mail = mail_recv(sprt_mb, 0);
        if (!isValid(sprt_mail))
        {
            schedule_delay_ms(200);
            continue;
        }

        op = 1;
        if (sprt_mail->sprt_msg->type == NR_MAIL_TYPE_SERIAL)
        {
            kchar_t *buffer = (kchar_t *)sprt_mail->sprt_msg[0].buffer;

            if (!kstrncmp(buffer, "up", 2))
                page_index = page_index ? (page_index - 1) : 0;
            else if (!kstrncmp(buffer, "down", 4)) 
            {
                if (page_index < sizeof(g_display_text_pages))
                    page_index++;
                g_display_text_pages[page_index] = offset;
            }
            else
                op = 0;
        }

        mail_recv_finish(sprt_mail);

    } while (!op);

    g_text_cur_page = page_index;
    return size;
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
    struct mailbox *sprt_mb = &sgrt_display_app_mailbox;
    struct fwk_fb_fix_screen_info sgrt_fix = {};
	struct fwk_fb_var_screen_info sgrt_var = {};
    kuint32_t *fb_buffer1, *fb_buffer2;
    struct fwk_disp_ctrl sgrt_dctrl;
    struct fwk_disp_info sgrt_disp;
    struct fs_stream *sprt_file;
    kuint8_t index = 0;

    mailbox_init(sprt_mb, mrt_current->tid, "display-app-mailbox");

    sgrt_dctrl.sprt_di = &sgrt_disp;
    display_app_settings_init(&sgrt_dctrl.sgrt_set);

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
                        sgrt_var.xres, sgrt_var.yres, sgrt_var.bits_per_pixel);

    mrt_preempt_disable();
    display_app_logo(&sgrt_dctrl);
    mrt_preempt_enable();
    schedule_delay(5);

    for (;;)
    {
        sprt_file = file_open(g_display_ebook_list[index], O_RDONLY);
        if (!isValid(sprt_file))
            goto loop2;

        g_text_cur_page = 0;
        memset_ex(g_display_text_pages, 0, sizeof(g_display_text_pages));

        while (display_app_text(fd, sprt_mb, sprt_file, &sgrt_dctrl) > 0);

        file_close(sprt_file);

    loop2:
        if ((index++) > sizeof(g_display_ebook_list))
            index = 0;

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
kint32_t display_app_init(void)
{
    struct thread_attr *sprt_attr = &sgrt_display_app_attr;
    kint32_t retval;

	sprt_attr->detachstate = THREAD_CREATE_JOINABLE;
	sprt_attr->inheritsched	= THREAD_INHERIT_SCHED;
	sprt_attr->schedpolicy = THREAD_SCHED_FIFO;

    /*!< thread stack */
	thread_set_stack(sprt_attr, mrt_nullptr, &g_display_app_stack[0], sizeof(g_display_app_stack));
    /*!< lowest priority */
	thread_set_priority(sprt_attr, THREAD_PROTY_DEFAULT);
    /*!< default time slice */
    thread_set_time_slice(sprt_attr, 100);

    /*!< register thread */
    retval = thread_create(&g_display_app_tid, sprt_attr, display_app_entry, mrt_nullptr);
    if (!retval)
        thread_set_name(g_display_app_tid, "display_app_entry");

    return retval;
}

/*!< end of file */
