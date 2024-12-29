/*
 * Display Generic Application Interface
 *
 * File Name:   fwk_disp.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.04.13
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/video/fwk_disp.h>
#include <platform/video/fwk_rgbmap.h>
#include <platform/video/fwk_font.h>
#include <kernel/kernel.h>
#include <kernel/mutex.h>

/*!< The defines */

/*!< The globals */
struct video_params *sprt_fwk_video_params;

/*!< API function */
/*!
 * @brief   convert rgb between rgb565 and rgb888
 * @param   srctype: current format
 * @param   destype: destination format
 * @param   data: rgb color
 * @retval  destination rgb color
 * @note    none
 */
kuint32_t fwk_display_convert_rgbbit(kuint8_t srctype, kuint8_t destype, kuint32_t data)
{
    kuint32_t r_data, g_data, b_data;
    kuint32_t result = data;

    /*!< if equaled, it does not need to convert */
    if (srctype == destype)
        goto END;

    switch (destype)
    {
        /*!< RGB888 ---> RGB565 */
        case FWK_RGB_PIXEL16:
            b_data = (data & 0xff) >> 3;
            g_data = ((data >> 8) & 0xff) >> 2;
            r_data = ((data >> 16) & 0xff) >> 3;

            result = ((r_data << 11) | (g_data << 5) | b_data) & 0x0000ffff;
            break;

        case FWK_RGB_PIXEL24:
            if (srctype == FWK_RGB_PIXEL32)
                goto END;

        case FWK_RGB_PIXEL32:
            if (srctype == FWK_RGB_PIXEL24)
                goto END;

            /*!< RGB565 ---> RGB888 */
            b_data = (data & 0xff) << 3;
            g_data = ((data >> 5) & 0xff) << 2;
            r_data = ((data >> 11) & 0xff) << 3;

            result = (r_data << 16) | (g_data << 8) | b_data;
            break;

        default:
            break;
    }

END:
    return result;
}

/*!
 * @brief   set current position
 * @param   sprt_dctrl
 * @param   x_start, y_start, x_end, y_end
 * @retval  none
 * @note    none
 */
void fwk_display_set_cursor(struct fwk_disp_ctrl *sprt_dctrl, 
                        kuint32_t x_start, kuint32_t y_start, kuint32_t x_end, kuint32_t y_end)
{
    struct fwk_disp_info *sprt_di = sprt_dctrl->sprt_di;
    struct fwk_font_setting *sprt_set = &sprt_dctrl->sgrt_set;

    sprt_dctrl->x_start = sprt_dctrl->x_next = x_start + sprt_set->left_spacing;
    sprt_dctrl->y_start = sprt_dctrl->y_next = y_start + sprt_set->upper_spacing;

    sprt_dctrl->x_end = (x_end < sprt_di->width) ? x_end : sprt_di->width;
    if (sprt_dctrl->x_end > sprt_set->right_spacing)
        sprt_dctrl->x_end -= sprt_set->right_spacing;

    sprt_dctrl->y_end = (y_end < sprt_di->height) ? y_end : sprt_di->height;
    if (sprt_dctrl->y_end > sprt_set->down_spacing)
        sprt_dctrl->y_end -= sprt_set->down_spacing;
}

/*!
 * @brief   write one line
 * @param   sprt_disp: screen information
 * @param   x_start, x_end: x-direction position
 * @param   y_start: y_end: y-direction position
 * @param   data: rgb color
 * @retval  none
 * @note    none
 */
void fwk_display_write_straight_line(struct fwk_disp_info *sprt_disp, kuint32_t x_start, kuint32_t y_start, 
                                                        kuint32_t x_end, kuint32_t y_end, kuint32_t data)
{
    kint8_t  x_inc, y_inc, is_x_max;
	kint32_t x_distance, y_distance, distance_max, distance_min, distance_err, pcnt;

    x_distance = mrt_usub(x_end, x_start);
    y_distance = mrt_usub(y_end, y_start);
	
    is_x_max = !!(x_distance >= y_distance);

	x_inc = (x_end >= x_start) ? 1 : -1;
    y_inc = (y_end >= y_start) ? 1 : -1;

    distance_max = is_x_max ? x_distance : y_distance;
    distance_min = is_x_max ? y_distance : x_distance;
	
    /*!< initialize err */
	distance_err = (-distance_max);

    mutex_lock(&sprt_disp->sgrt_lock);

    /*!< draw line */
	for (pcnt = 0; pcnt <= distance_max; pcnt++)
	{
		fwk_display_write_point(sprt_disp, x_start, y_start, data);
		
		if (is_x_max)
			x_start += x_inc;
		else
			y_start += y_inc;
		
        /*!< 
         * When the initial value of distance_err is set to - status_max, 
         * the statement must be placed before determining whether it is greater than 0 
         */
		distance_err += (distance_min << 1);
		if (distance_err >= 0)
		{
			distance_err -= (distance_max << 1);
			if (is_x_max)
				y_start += y_inc;
			else
				x_start += x_inc;
		}
	}

    mutex_unlock(&sprt_disp->sgrt_lock);
}

/*!
 * @brief   write one rectangle
 * @param   sprt_disp: screen information
 * @param   x_start, x_end: x-direction position
 * @param   y_start: y_end: y-direction position
 * @param   data: rgb color
 * @retval  none
 * @note    none
 */
void fwk_display_write_rectangle(struct fwk_disp_info *sprt_disp, kuint32_t x_start, kuint32_t y_start, 
                                                    kuint32_t x_end, kuint32_t y_end, kuint32_t data)
{
    fwk_display_write_straight_line(sprt_disp, x_start, y_start, x_end,   y_start, data);
	fwk_display_write_straight_line(sprt_disp, x_start, y_start, x_start, y_end,   data);
	fwk_display_write_straight_line(sprt_disp, x_start, y_end,   x_end,   y_end,   data);
	fwk_display_write_straight_line(sprt_disp, x_end,   y_start, x_end,   y_end,   data);    
}

/*!
 * @brief   fill one rectangle
 * @param   sprt_disp: screen information
 * @param   x_start, x_end: x-direction position
 * @param   y_start: y_end: y-direction position
 * @param   data: rgb color
 * @retval  none
 * @note    none
 */
void fwk_display_fill_rectangle(struct fwk_disp_info *sprt_disp, kuint32_t x_start, kuint32_t y_start, 
                                                    kuint32_t x_end, kuint32_t y_end, kuint32_t data)
{
    kuint32_t offset;
    kuint32_t x_cnt, y_cnt, width, height, xy_temp;
    kuint8_t pixelbits;
    kuint32_t rgb_data;

    if ((x_start > sprt_disp->width) || (y_start > sprt_disp->height) || 
        (x_end > sprt_disp->width) || (y_end > sprt_disp->height))
        return;

    if (x_end < x_start)
    {
        xy_temp = x_end;
        x_end   = x_start;
        x_start = xy_temp;
    }

    if (y_end < y_start)
    {
        xy_temp = y_end;
        y_end   = y_start;
        y_start = xy_temp;
    }

    width  = mrt_usub(x_end, x_start);
    height = mrt_usub(y_end, y_start);

    pixelbits = mrt_fwk_disp_bpp_get(sprt_disp->bpp);
    rgb_data  = mrt_fwk_disp_convert_rgb(FWK_RGB_PIXELBIT, pixelbits, data);

    mutex_lock(&sprt_disp->sgrt_lock);

    for (y_cnt = 0; y_cnt < height; y_cnt++)
    {
        offset = mrt_fwk_disp_advance_pos(x_start, y_start + y_cnt, sprt_disp->width);

        if ((pixelbits == FWK_RGB_PIXEL24) ||
            (pixelbits == FWK_RGB_PIXEL32))
            memset_ex(sprt_disp->buffer + (offset << 2), rgb_data, (width << 2));
        else
        {
            for (x_cnt = 0; x_cnt < width; x_cnt++)
                mrt_fwk_disp_write_pixel(sprt_disp->buffer, x_cnt + offset, pixelbits, rgb_data);
        }
    }

    mutex_unlock(&sprt_disp->sgrt_lock);
}

/*!
 * @brief   clear all
 * @param   sprt_disp: screen information
 * @param   data: rgb color
 * @retval  none
 * @note    none
 */
void fwk_display_clear(struct fwk_disp_info *sprt_disp, kuint32_t data)
{
    kuint8_t pixelbits;
    kuint32_t rgb_data;

    pixelbits = mrt_fwk_disp_bpp_get(sprt_disp->bpp);
    rgb_data  = mrt_fwk_disp_convert_rgb(FWK_RGB_PIXELBIT, pixelbits, data);

    memset_ex(sprt_disp->buffer, rgb_data, sprt_disp->buf_size);
}

/*!
 * @brief   get offset of ascii lib
 * @param   ptr_lib: ascii lib
 * @param   size: format
 * @param   ch: current character
 * @retval  ptr_lib + offset
 * @note    none
 */
static void *fwk_font_ascii_lseek(kuint8_t *ptr_lib, kuint32_t size, kuint8_t ch)
{
    kuint32_t pixel_bits, pixel_bytes;
    kuint32_t offset;

    pixel_bits = size >> 1;
    pixel_bits = mrt_align(pixel_bits, 8);
    pixel_bytes = (pixel_bits >> 3);

    /*!< ascii font starts with a space and needs to be subtracted from the space to determine which character it is */
    offset  = ch - ASCII_SPACE;
    offset *= size * pixel_bytes;

    return ptr_lib ? (ptr_lib + offset) : mrt_nullptr;
}

/*!
 * @brief   get offset of HZ lib
 * @param   ptr_lib: HZ lib
 * @param   font: format
 * @param   size: format
 * @param   ch: current character
 * @retval  ptr_lib + offset
 * @note    none
 */
static void *fwk_fontlib_lseek(kuint8_t *ptr_lib, kuint32_t font, kuint32_t size, kuint32_t word)
{
    kuint32_t pixel_bits, pixel_bytes;
    kuint32_t area_code, bit_code;
    kuint32_t offset = 0;

    /*!< dot matrix data is stored in bytes; pixel_bits: dots of one line; pixel_bytes: bytes of one line (dots / 8) */
    pixel_bits = size;
    pixel_bytes = (pixel_bits >> 3);

    switch (font)
    {
        case NR_FWK_FONT_SONG:
            area_code = (word >> 8) & 0xff;
            bit_code = word & 0xff;

            /*!< 
             * area code
             * the first Chinese character in the font is 0xa1a1, which means:
             *      a1 (high byte) is the 0th region, and the 0th character in each region is a1 (low byte) 
             * therefore, the search position is subtracted from the first character position
             */
            area_code -= AREA_FIRST_CODE;
            bit_code -= BITS_FIRST_CODE;

            /*!< 
            * every zone stores 94 chinese characters;
            * for HZ-song16, a line needs 16 pixels = 2 bytes; 16 lines = 2 * 16 = 32bytes
            * offset = (area_code * 94 + bit_code) * 32; 
            */
            offset  = (area_code << 7) - (area_code << 5) - (area_code << 1) + bit_code;
            offset *= size * pixel_bytes;

            break;

        case NR_FWK_FONT_XINGKAI:
            break;

        default: break;
    }

    return ptr_lib ? (ptr_lib + offset) : mrt_nullptr;
}

/*!
 * @brief   write one string
 * @param   sprt_disp: screen information
 * @param   sprt_settings: display settings
 * @param   x_start, x_end: x-direction position
 * @param   y_start: y_end: y-direction position
 * @param   fmt: string text
 * @retval  bytes written
 * @note    none
 */
static kusize_t __fwk_display_dot_matrix_word(struct fwk_disp_ctrl *sprt_dctrl, const kchar_t *fmt)
{
    struct fwk_disp_info *sprt_disp;
    struct fwk_font_setting *sprt_settings;
    kuint8_t *ptr_ch, *ptr_dmatx;
    kuint32_t x_pos, y_pos;
    kuint32_t flib_index;
    kuint32_t x_cnt, y_cnt, x_data, x_inc, byte_cnt, arr_inc;
    kuint32_t x_end, y_end;

    if ((!fmt) || (!sprt_dctrl))
        return 0;

    sprt_disp = sprt_dctrl->sprt_di;
    sprt_settings = &sprt_dctrl->sgrt_set;

    x_pos  = sprt_dctrl->x_next;
    y_pos  = sprt_dctrl->y_next;
    x_end  = sprt_dctrl->x_end;
    y_end  = sprt_dctrl->y_end;

    ptr_ch = (kuint8_t *)fmt;

    while ('\0' != *(ptr_ch))
    {
        /*!< 
         * reserve a width of one character; 
         * if the x direction exceeds the specified length, line breaks will occur; 
         * if the y direction exceeds the specified width, it will exit the display;
         *                                                 it can also be modified to clear screen and change pages 
         */
        if (((x_pos + sprt_settings->size) > x_end) ||
            ('\n' == *(ptr_ch)))
        {
            x_pos  = sprt_dctrl->x_start;
            y_pos += (sprt_settings->size + sprt_settings->line_spacing);
        }
        if ((y_pos + sprt_settings->size) > y_end)
        {
            break;
        }

        ptr_dmatx  = mrt_nullptr;
        flib_index = *(ptr_ch++);

        if ((flib_index < ASCII_SPACE) || (flib_index == ASCII_MAX))
            continue;

        /*!< Chinese character */
        if (flib_index & 0x80)
        {
            if (!(*ptr_ch & 0x80))
                continue;

            flib_index = ((flib_index << 8) | (*(ptr_ch++))) & 0xffff;

        /*!< for Unicode, GB2312, ..., FWK_FONT_HZ_3BYTES = 0 */
        #if FWK_FONT_HZ_3BYTES
            if (!(*ptr_ch & 0x80))
                continue;

            flib_index = ((flib_index << 8) | (*(ptr_ch++))) & 0xffffff;
        #endif

            if (!sprt_settings->ptr_hz)
                continue;

            x_inc = sprt_settings->size;
            ptr_dmatx = fwk_fontlib_lseek(sprt_settings->ptr_hz, sprt_settings->font, sprt_settings->size, flib_index);
        }
        
        /*!< ASCII */
        else
        {
            if (!sprt_settings->ptr_ascii)
                continue;

            x_inc = sprt_settings->size >> 1;
            ptr_dmatx = fwk_font_ascii_lseek(sprt_settings->ptr_ascii, sprt_settings->size, (kuint8_t)flib_index);
        }

        if (!ptr_dmatx)
            continue;

        arr_inc = x_inc >> 3;
        mutex_lock(&sprt_disp->sgrt_lock);

        /*!< draw pixel points */
        for (y_cnt = 0; y_cnt < sprt_settings->size; y_cnt++)
        {
            for (byte_cnt = 0; byte_cnt < arr_inc; byte_cnt++)
            {
                x_data = *(ptr_dmatx + byte_cnt + y_cnt * (arr_inc));

                for (x_cnt = 0; x_cnt < 8; x_cnt++)
                {
                    if (x_data & 0x80)
                        fwk_display_write_point(sprt_disp, x_pos + (byte_cnt << 3) + x_cnt, y_pos + y_cnt, sprt_settings->color);

                    x_data <<= 1;
                }
            }
        }

        mutex_unlock(&sprt_disp->sgrt_lock);
        x_pos += (x_inc + sprt_settings->word_spacing);
    }

    sprt_dctrl->x_next = x_pos;
    sprt_dctrl->y_next = y_pos;

    return (kusize_t)(ptr_ch - (kuint8_t *)fmt);
}

/*!
 * @brief   write one string
 * @param   sprt_disp: screen information
 * @param   sprt_settings: display settings
 * @param   x_start, x_end: x-direction position
 * @param   y_start: y_end: y-direction position
 * @param   fmt: string text
 * @retval  none
 * @note    none
 */
kusize_t fwk_display_word(struct fwk_disp_ctrl *sprt_dctrl, const kchar_t *fmt, ...)
{
//  va_list ptr_list;
//  kint8_t buffer[8];
//  kusize_t size = 0;

//  va_start(ptr_list, fmt);

//  while (*(fmt + size) != '\0')
//  {
//      size = do_fmt_convert(buffer, mrt_nullptr, fmt, ptr_list, sizeof(buffer));
        return __fwk_display_dot_matrix_word(sprt_dctrl, fmt);
//  }
//  va_end(ptr_list);
}

/*!
 * @brief   fill one rectangle
 * @param   sprt_disp: screen information
 * @param   fbuffer: framebuffer
 * @param   size: format
 * @param   width: x-direction maximum
 * @param   height: y-direction maximum
 * @param   bpp: bytes per color
 * @retval  sprt_disp
 * @note    none
 */
void fwk_display_ctrl_init(struct fwk_disp_info *sprt_disp, void *fbuffer, 
                          void *fbuffer2, kusize_t size, kuint32_t width, kuint32_t height, kuint32_t bpp)
{
    if (!isValid(sprt_disp))
        return;
    
    sprt_disp->buffer = fbuffer;
    sprt_disp->buffer_bak = fbuffer2;
    sprt_disp->buf_size = size;
    sprt_disp->width = width;
    sprt_disp->height = height;
    sprt_disp->bpp = bpp;

    mutex_init(&sprt_disp->sgrt_lock);
}

/*!< end of file */
