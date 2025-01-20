/*
 * User Thread Instance (display task) Interface
 *
 * File Name:   display_task.c
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

#include "../task.h"
#include "test_task.h"

using namespace tsk;
using namespace stream;

/*!< The defines */
#define DISPLAY_TASK_STACK_SIZE                      THREAD_STACK_PAGE(1)    /*!< 1 page (4kbytes) */

class crt_disp_task_t;
class crt_disp_base_t;
class crt_disp_bmp_t;
class crt_disp_text_t;

/*!< The functions */
static void display_task_clear(crt_disp_base_t &cgrt_this);
static void display_task_cursor(crt_disp_base_t &cgrt_this, 
                        kuint32_t x_start, kuint32_t y_start, kuint32_t x_end, kuint32_t y_end);
static kssize_t display_task_text(crt_disp_task_t &cgrt_dtsk, crt_disp_text_t &cgrt_text,
                        struct fs_stream *sprt_file);

/*!< The defines */
/*!< base class for display */
class crt_disp_base_t 
{
    friend void display_task_clear(crt_disp_base_t &cgrt_this);
    friend void display_task_cursor(crt_disp_base_t &cgrt_this, 
                        kuint32_t x_start, kuint32_t y_start, kuint32_t x_end, kuint32_t y_end);

public:
    crt_disp_base_t(crt_disp_task_t &cgrt_dtsk);
    ~crt_disp_base_t() {}

protected:
    struct fwk_disp_ctrl sgrt_dctrl;
    struct fwk_disp_info sgrt_disp;
};

/*!< bmp class */
class crt_disp_bmp_t : virtual public crt_disp_base_t
{
public:
    crt_disp_bmp_t(crt_disp_task_t &cgrt_dtsk) 
        : crt_disp_base_t(cgrt_dtsk)
        , bmp(mrt_nullptr) 
    {}
    ~crt_disp_bmp_t() {}

    void set_src(const kchar_t *bmp)
    {
        this->bmp = bmp;
    }
    void show(void);

private:
    const kchar_t *bmp;
};

/*!< text class */
class crt_disp_text_t : virtual public crt_disp_base_t
{
    friend kssize_t display_task_text(crt_disp_task_t &cgrt_dtsk, crt_disp_text_t &cgrt_text,
                        struct fs_stream *sprt_file);
    
public:
    crt_disp_text_t(crt_disp_task_t &cgrt_dtsk, kuint32_t pages)
        : crt_disp_base_t(cgrt_dtsk)
        , text(mrt_nullptr)
        , num(0)
        , cur_text(0)
        , pages(pages)
        , cur_page(0)
    {
        this->text_pages = new kuint32_t[pages];
        if (this->text_pages)
            memset(this->text_pages, 0, pages * sizeof(kuint32_t));
    }
    ~crt_disp_text_t() 
    {
        if (this->text_pages)
            delete[] this->text_pages;
    }

    void set_src(const kchar_t **text, kuint32_t num)
    {
        this->text = text;
        this->num  = num;
    }

    void show(crt_disp_task_t &cgrt_dtsk);

private:
    const kchar_t **text;
    kuint32_t num;
    kuint32_t cur_text;

    kuint32_t *text_pages;
    kuint32_t pages;
    kuint32_t cur_page;
};

/*!< major class for display task */
class crt_disp_task_t 
{
    friend crt_disp_base_t;
    friend crt_disp_bmp_t;
    friend crt_disp_text_t;

public:
    crt_disp_task_t(crt_task_t *cprt_task, const kchar_t *file, kuint32_t mode);
    ~crt_disp_task_t();

    crt_task_t *cprt_task;
    kint32_t fd;

private:
    struct fwk_fb_fix_screen_info sgrt_fix;
	struct fwk_fb_var_screen_info sgrt_var;
    kuint32_t *fb_buffer1;
    kuint32_t *fb_buffer2;
};

/*!< The globals */
static kchar_t g_display_buffer[((1920 * 1080) / (FWK_FONT_16 * FWK_FONT_16)) * 2 + 4] __align(4);
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
 * @brief	constructor of crt_disp_task_t
 * @param  	none
 * @retval 	none
 * @note   	none
 */
crt_disp_task_t::crt_disp_task_t(crt_task_t *cprt_task, const kchar_t *file, kuint32_t mode)
    : cprt_task(cprt_task)
    , fb_buffer1(mrt_nullptr)
    , fb_buffer2(mrt_nullptr)
{
    fd = virt_open(file, mode);
    if (fd < 0)
        return;

    virt_ioctl(fd, NR_FB_IOGET_VARINFO, &sgrt_var);
    virt_ioctl(fd, NR_FB_IOGET_FIXINFO, &sgrt_fix);

    fb_buffer1 = (kuint32_t *)virt_mmap(mrt_nullptr, sgrt_fix.smem_len, 0, 0, fd, 0);
    if (!isValid(fb_buffer1))
        goto fail2;

    fb_buffer2 = (kuint32_t *)virt_mmap(mrt_nullptr, sgrt_fix.smem_len, 0, 0, fd, sgrt_fix.smem_len);
    if (!isValid(fb_buffer2))
        goto fail3;

    return;

    virt_munmap(fb_buffer2, sgrt_fix.smem_len);
fail3:
    virt_munmap(fb_buffer1, sgrt_fix.smem_len);
fail2:
    virt_close(fd);

    fd = -1;
    fb_buffer1 = fb_buffer2 = mrt_nullptr;
}

/*!
 * @brief	destructor of crt_disp_task_t
 * @param  	none
 * @retval 	none
 * @note   	none
 */
crt_disp_task_t::~crt_disp_task_t()
{
    if (fd < 0)
        return;

    virt_munmap(fb_buffer2, sgrt_fix.smem_len);
    virt_munmap(fb_buffer1, sgrt_fix.smem_len);
    virt_close(fd);
}

/*!
 * @brief  initial display common settings
 * @param  none
 * @retval none
 * @note   do display
 */
static void display_task_settings_init(struct fwk_font_setting *sprt_set)
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
 * @brief  clear full screen
 * @param  none
 * @retval none
 * @note   do display
 */
static void display_task_clear(crt_disp_base_t &cgrt_this)
{
    struct fwk_disp_ctrl &sgrt_dctrl = cgrt_this.sgrt_dctrl;
    fwk_display_clear(sgrt_dctrl.sprt_di, sgrt_dctrl.sgrt_set.background);
}

/*!
 * @brief  set cursor
 * @param  none
 * @retval none
 * @note   do display
 */
static void display_task_cursor(crt_disp_base_t &cgrt_this, 
                        kuint32_t x_start, kuint32_t y_start, kuint32_t x_end, kuint32_t y_end)
{
    struct fwk_disp_ctrl &sgrt_dctrl = cgrt_this.sgrt_dctrl;
    struct fwk_disp_info *sprt_disp = sgrt_dctrl.sprt_di;
    struct fwk_font_setting *sprt_set = &sgrt_dctrl.sgrt_set;

    fwk_display_set_cursor(&sgrt_dctrl, x_start, y_start, x_end, y_end);
    fwk_display_fill_rectangle(sprt_disp, x_start, y_start, 
                                        x_end, y_end, sprt_set->background);
}

/*!
 * @brief	constructor of crt_disp_base_t
 * @param  	none
 * @retval 	none
 * @note   	none
 */
crt_disp_base_t::crt_disp_base_t(crt_disp_task_t &cgrt_dtsk)
{
    struct fwk_disp_ctrl &sgrt_dctrl = this->sgrt_dctrl;
    struct fwk_font_setting &sgrt_set = this->sgrt_dctrl.sgrt_set;

    sgrt_dctrl.sprt_di = &this->sgrt_disp;
    if (cgrt_dtsk.fd < 0)
        return;

    display_task_settings_init(&sgrt_set);
    fwk_display_ctrl_init(&this->sgrt_disp, 
                    cgrt_dtsk.fb_buffer1, 
                    cgrt_dtsk.fb_buffer2, 
                    cgrt_dtsk.sgrt_fix.smem_len, 
                    cgrt_dtsk.sgrt_var.xres, 
                    cgrt_dtsk.sgrt_var.yres, 
                    cgrt_dtsk.sgrt_var.bits_per_pixel);
}

/*!
 * @brief  display logo
 * @param  none
 * @retval none
 * @note   do display
 */
void crt_disp_bmp_t::show(void)
{
    struct fwk_disp_ctrl &sgrt_dctrl = this->sgrt_dctrl;
    struct fwk_disp_info *sprt_disp = sgrt_dctrl.sprt_di;
    struct fs_stream *sprt_file;
    struct fwk_bmp_ctrl sgrt_bctl;
    kuint8_t bytes_per_pixel;
    kssize_t size;

    if (!this->bmp)
        return;

    display_task_clear(*this);
    display_task_cursor(*this, 0, 0, sprt_disp->width, sprt_disp->height);

    sprt_file = file_open(this->bmp, O_RDONLY);
    if (!isValid(sprt_file))
        return;

    bytes_per_pixel = sprt_disp->bpp >> 3;
    size = file_read(sprt_file, sprt_disp->buffer_bak, 
                sprt_disp->width * sprt_disp->height * bytes_per_pixel);
    if (size <= 0)
        goto END;

    fwk_bitmap_ctrl_init(&sgrt_bctl, sprt_disp, 0, 0);
    fwk_display_whole_bitmap(&sgrt_bctl, (const kuint8_t *)sprt_disp->buffer_bak);

END:
    file_close(sprt_file);
}

/*!
 * @brief  display text
 * @param  none
 * @retval none
 * @note   do display
 */
static kssize_t display_task_text(crt_disp_task_t &cgrt_dtsk, crt_disp_text_t &cgrt_text,
                        struct fs_stream *sprt_file)
{
    struct mailbox &sgrt_mb = cgrt_dtsk.cprt_task->get_mailbox();
    struct fwk_disp_ctrl &sgrt_dctrl = cgrt_text.sgrt_dctrl;
    struct fwk_disp_info *sprt_disp = sgrt_dctrl.sprt_di;
    struct mail *sprt_mail;
    struct fwk_fb_var_screen_info sgrt_var;
    kuint8_t op = 0;
    kssize_t size, offset = 0;
    kusize_t page_index = 0;

    fwk_display_frame_exchange(sprt_disp);
    display_task_clear(cgrt_text);
    display_task_cursor(cgrt_text, 0, 0, sprt_disp->width, sprt_disp->height);

    page_index = cgrt_text.cur_page;
    offset = cgrt_text.text_pages[page_index];
    while (!IS_DISP_FRAME_FULL(&sgrt_dctrl))
    {
        file_lseek(sprt_file, offset);
        size = file_read(sprt_file, g_display_buffer, sizeof(g_display_buffer) - 4);
        if (size <= 0)
            return 0;

        g_display_buffer[size] = '\0';
        offset += fwk_display_word(&sgrt_dctrl, g_display_buffer);
    }

    virt_ioctl(cgrt_dtsk.fd, NR_FB_IOGET_VARINFO, &sgrt_var);
    if (!sgrt_var.yoffset)
        sgrt_var.yoffset += sgrt_var.yres;
    else
        sgrt_var.yoffset = 0;
    
    virt_ioctl(cgrt_dtsk.fd, NR_FB_IOSET_VARINFO, &sgrt_var);

    do {
        sprt_mail = mail_recv(&sgrt_mb, 0);
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
                if (page_index < cgrt_text.pages)
                    page_index++;
                cgrt_text.text_pages[page_index] = offset;
            }
            else
                op = 0;
        }

        mail_recv_finish(sprt_mail);

    } while (!op);

    cgrt_text.cur_page = page_index;
    return size;
}

/*!
 * @brief  show text
 * @param  none
 * @retval none
 * @note   do display
 */
void crt_disp_text_t::show(crt_disp_task_t &cgrt_dtsk)
{
    struct fs_stream *sprt_file;

    if (!this->text_pages)
        return;

    sprt_file = file_open(this->text[this->cur_text], O_RDONLY);
    if (!isValid(sprt_file))
        goto END;

    this->cur_page = 0;
    memset_ex(this->text_pages, 0, this->pages);

    while (display_task_text(cgrt_dtsk, *this, sprt_file) > 0);
    file_close(sprt_file);

END:
    if ((this->cur_text++) >= this->num)
        this->cur_text = 0;
}

/*!
 * @brief  display task
 * @param  none
 * @retval none
 * @note   do display
 */
static void *display_task_entry(void *args)
{
    crt_task_t *cprt_this = (crt_task_t *)args;
    crt_disp_task_t cgrt_dtsk(cprt_this, "/dev/fb0", O_RDWR);
    crt_disp_bmp_t cgrt_logo(cgrt_dtsk);
    crt_disp_text_t cgrt_txt(cgrt_dtsk, 1024);

    if (cgrt_dtsk.fd < 0)
        goto fail;

    cgrt_logo.set_src(g_display_logo);
    cgrt_txt.set_src(g_display_ebook_list, ARRAY_SIZE(g_display_ebook_list));

    mrt_preempt_disable();
    cgrt_logo.show();
    mrt_preempt_enable();
    schedule_delay(5);

    for (;;)
    {
        cgrt_txt.show(cgrt_dtsk);
        schedule_delay_ms(200);
    }

fail:
    schedule_self_suspend();

    return args;
}

/*!
 * @brief	create display app task
 * @param  	none
 * @retval 	error code
 * @note   	none
 */
kint32_t display_task_init(void)
{
    static kuint8_t g_display_task_stack[DISPLAY_TASK_STACK_SIZE];

    crt_task_t *cprt_task = new crt_task_t("display task", 
                                            display_task_entry, 
                                            g_display_task_stack, 
                                            sizeof(g_display_task_stack),
                                            THREAD_PROTY_DEFAULT,
                                            100);
    if (!cprt_task)
        return -ER_FAILD;

    struct mailbox &sgrt_mb = cprt_task->get_mailbox();
    mailbox_init(&sgrt_mb, cprt_task->get_self(), "display-task-mailbox");

    return ER_NORMAL;
}

/*!< end of file */
