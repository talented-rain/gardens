/*
 * Display Generic API
 *
 * File Name:   fwk_disp.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.04.13
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_DISP_H_
#define __FWK_DISP_H_

/*!< The includes */
#include <platform/fwk_basic.h>
#include <kernel/mutex.h>
#include <kernel/spinlock.h>

#include "fwk_font.h"
#include "fwk_rgbmap.h"

/*!< The globals */

/*!< The defines */
typedef struct fwk_disp_info
{
    void *buffer;
    void *buffer_bak;
    kusize_t buf_size;                      /*!< unit: byte */
    kuint32_t width;
    kuint32_t height;
    kuint8_t bpp;

    struct mutex_lock sgrt_lock;

} srt_fwk_disp_info_t;

typedef struct fwk_disp_ctrl
{
    struct fwk_disp_info *sprt_di;
    struct fwk_font_setting sgrt_set;

    kuint32_t x_start;
    kuint32_t y_start;
    kuint32_t x_end;
    kuint32_t y_end;

    kuint32_t x_next;
    kuint32_t y_next;

} srt_fwk_disp_ctrl_t;

#define IS_DISP_FRAME_FULL(sprt_dctrl)  \
        ((sprt_dctrl->x_next == sprt_dctrl->x_start) && \
            (sprt_dctrl->y_next >= sprt_dctrl->y_end))

#define mrt_fwk_disp_write_fb16(buffer, offset, data) \
    do {    \
        *(((kuint16_t *)(buffer)) + (offset)) = data;   \
    } while (0)

#define mrt_fwk_disp_write_fb32(buffer, offset, data) \
    do {    \
        *(((kuint32_t *)(buffer)) + (offset)) = data;   \
    } while (0)

#define mrt_fwk_disp_write_fb24(buffer, offset, data)               mrt_fwk_disp_write_fb32(buffer, offset, data)

#define mrt_fwk_disp_write_pixel(buffer, offset, bpp, data)   \
    do {    \
        if ((bpp) == FWK_RGB_PIXEL32) \
            mrt_fwk_disp_write_fb32(buffer, offset, data);  \
        else if ((bpp) == FWK_RGB_PIXEL24) \
            mrt_fwk_disp_write_fb24(buffer, offset, data);  \
        else if ((bpp) == FWK_RGB_PIXEL16)    \
            mrt_fwk_disp_write_fb16(buffer, offset, data);  \
    } while (0)

#define mrt_fwk_disp_bpp_get(bpp)                                   (((bpp) == FWK_RGB_PIXEL24) ? FWK_RGB_PIXEL32 : (bpp))
#define mrt_fwk_disp_advance_pos(x, y, x_max)                       ((y) * (x_max) + (x))
#define mrt_fwk_disp_convert_rgb(src, dest, data)   \
            (((src) == (dest)) ? (data) : fwk_display_convert_rgbbit(src, dest, data))

/*!< The functions */
TARGET_EXT kuint32_t fwk_display_convert_rgbbit(kuint8_t srctype, kuint8_t destype, kuint32_t data);
TARGET_EXT void fwk_display_set_cursor(struct fwk_disp_ctrl *sprt_dctrl, 
                            kuint32_t x_start, kuint32_t y_start, kuint32_t x_end, kuint32_t y_end);
TARGET_EXT void fwk_display_write_straight_line(struct fwk_disp_info *sprt_disp, kuint32_t x_start, kuint32_t y_start, 
                            kuint32_t x_end, kuint32_t y_end, kuint32_t data);
TARGET_EXT void fwk_display_write_rectangle(struct fwk_disp_info *sprt_disp, kuint32_t x_start, kuint32_t y_start, 
                            kuint32_t x_end, kuint32_t y_end, kuint32_t data);
TARGET_EXT void fwk_display_fill_rectangle(struct fwk_disp_info *sprt_disp, kuint32_t x_start, kuint32_t y_start, 
                            kuint32_t x_end, kuint32_t y_end, kuint32_t data);
TARGET_EXT void fwk_display_clear(struct fwk_disp_info *sprt_disp, kuint32_t data);
TARGET_EXT kusize_t fwk_display_word(struct fwk_disp_ctrl *sprt_dctrl, const kchar_t *fmt, ...);

TARGET_EXT void fwk_display_ctrl_init(struct fwk_disp_info *sprt_disp, void *fbuffer, 
                          void *fbuffer2, kusize_t size, kuint32_t width, kuint32_t height, kuint32_t bpp);

/*!< API functions */
/*!
 * @brief   get position real-time
 * @param   xpos, ypos
 * @retval  none
 * @note    none
 */
static inline __force_inline 
kuint32_t fwk_display_advance_position(kuint32_t xpos, kuint32_t ypos, kuint32_t x_max)
{
    return mrt_fwk_disp_advance_pos(xpos, ypos, x_max);
}

/*!
 * @brief   write a pixel
 * @param   buffer: framebuffer
 * @param   offset: buffer offset
 * @param   data: rgb color
 * @retval  none
 * @note    none
 */
static inline __force_inline 
void fwk_display_write_pixel(void *buffer, kuint32_t offset, kuint8_t bpp, kuint32_t data)
{
    mrt_fwk_disp_write_pixel(buffer, offset, bpp, data);
}

/*!
 * @brief   write one data
 * @param   buffer: framebuffer
 * @param   offset: buffer offset
 * @param   data: rgb color
 * @retval  none
 * @note    combine format-converter and pixel writting
 */
static inline 
void fwk_display_write_frame_data(void *buffer, kuint32_t offset, kuint8_t bpp, kuint32_t data)
{
    kuint32_t rgb_data;
    kuint8_t  pixelbits;

    pixelbits = mrt_fwk_disp_bpp_get(bpp);
    rgb_data  = mrt_fwk_disp_convert_rgb(FWK_RGB_PIXELBIT, pixelbits, data);

    mrt_fwk_disp_write_pixel(buffer, offset, pixelbits, rgb_data);
}

/*!
 * @brief   write a pixel point
 * @param   sprt_disp: screen information
 * @param   xpos: x-direction position
 * @param   ypos: y-direction position
 * @param   data: rgb color
 * @retval  none
 * @note    none
 */
static inline 
void fwk_display_write_point(struct fwk_disp_info *sprt_disp, kuint32_t xpos, kuint32_t ypos, kuint32_t data)
{
    kuint32_t offset, rgb_data;
    kuint8_t  pixelbits;

    offset = mrt_fwk_disp_advance_pos(xpos, ypos, sprt_disp->width);
    pixelbits = mrt_fwk_disp_bpp_get(sprt_disp->bpp);
    rgb_data  = mrt_fwk_disp_convert_rgb(FWK_RGB_PIXELBIT, pixelbits, data);

    mrt_fwk_disp_write_pixel(sprt_disp->buffer, offset, pixelbits, rgb_data);
}

/*!
 * @brief  exchange framebuffer
 * @param  sprt_disp
 * @retval none
 * @note   none
 */
static inline 
void fwk_display_frame_exchange(struct fwk_disp_info *sprt_disp)
{
    void *buffer = sprt_disp->buffer_bak;

    if (!buffer)
        return;

    sprt_disp->buffer_bak = sprt_disp->buffer;
    sprt_disp->buffer = buffer;
}

#endif /*!< __FWK_DISP_H_ */
