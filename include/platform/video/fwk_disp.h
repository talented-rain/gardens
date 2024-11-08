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

/*!< The globals */

/*!< The defines */
typedef struct fwk_disp_info
{
    void *buffer;
    kusize_t buf_size;                      /*!< unit: byte */
    kuint32_t width;
    kuint32_t height;
    kuint8_t bpp;

    const struct fwk_disp_ops *sprt_ops;
    struct mutex_lock sgrt_lock;

} srt_fwk_disp_info_t;

typedef struct fwk_disp_ops
{
    kuint32_t (*get_offset)(kuint32_t xpos, kuint32_t ypos, kuint32_t x_max);
    kuint32_t (*convert_rgbbit)(kuint8_t srctype, kuint8_t destype, kuint32_t data);

    void (*write_pixel)(void *buffer, kuint32_t offset, kuint8_t bpp, kuint32_t data);
    void (*write_data)(void *buffer, kuint32_t offset, kuint8_t bpp, kuint32_t data);
    void (*write_point)(struct fwk_disp_info *sprt_disp, kuint32_t xpos, kuint32_t ypos, kuint32_t data);

    void (*write_line)(struct fwk_disp_info *sprt_disp, kuint32_t x_start, kuint32_t y_start, 
                                                        kuint32_t x_end, kuint32_t y_end, kuint32_t data);
    void (*write_rectangle)(struct fwk_disp_info *sprt_disp, kuint32_t x_start, kuint32_t y_start, 
                                                        kuint32_t x_end, kuint32_t y_end, kuint32_t data);
    void (*fill_rectangle)(struct fwk_disp_info *sprt_disp, kuint32_t x_start, kuint32_t y_start, 
                                                        kuint32_t x_end, kuint32_t y_end, kuint32_t data);
    void (*clear)(struct fwk_disp_info *sprt_disp, kuint32_t data);

    void (*write_word)(struct fwk_disp_info *sprt_disp, struct fwk_font_setting *sprt_settings,
                                        kuint32_t x_start, kuint32_t y_start, kuint32_t x_end, kuint32_t y_end, const kchar_t *fmt, ...);

} srt_fwk_disp_ops_t;

/*!< The functions */
TARGET_EXT void *fwk_display_ctrl_init(struct fwk_disp_info *sprt_disp,
                          void *fbuffer, kusize_t size, kuint32_t width, kuint32_t height, kuint32_t bpp);

#endif /*!< __FWK_DISP_H_ */
