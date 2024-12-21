/*
 * Display Bitmap Interface
 *
 * File Name:   fwk_bitmap.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.06.29
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_BITMAP_H_
#define __FWK_BITMAP_H_

/*!< The includes */
#include <platform/fwk_basic.h>

/*!< The defines */
/*!< image dot matrix */
typedef struct fwk_dotmat_header
{
    kuint8_t  scan_type;					/*!< image scaning type, 0b0000: 水平扫描; 0b1111, 垂直扫描 */
    kuint8_t  pixelbit;						/*!< bits of per pixel color: 4、8、16、24 */
    kuint16_t width;						/*!< image width, unit: pixel */
    kuint16_t height;						/*!< image height, unit: pixel */

} srt_fwk_dotmat_header_t;

/*!< BitMap */
#pragma pack(push)
#pragma pack(1)

/*!< The first 54 bytes of the BMP image are the file information headers, and the BmpFileHeader of the bitmap file is the first 14 bytes */
typedef struct fwk_bmp_file_header
{
    kuint16_t picType;               		/*!< the content is fixed to 0x42, 0x4D, i.e., ASCII character 'B', 'M', i.e., BMP format */
    kuint32_t bmpSize;               		/*!< BMP size (unit: byte) */

    kuint16_t reserved1;             		/*!< reserved */
    kuint16_t reserved2;             		/*!< reserved */
    kuint32_t offsetbit;             		/*!< BMP file header + BMP info header + color palette */

} srt_fwk_bmp_file_header_t;

#pragma pack(pop)

/*!< Bitmap information header (BmpInfoHeader) is the first 15~54 bytes */
typedef struct fwk_bmp_info_header
{
    kuint32_t infoSize;              		/*!< the size of the entire bitmap info header in bytes, i.e. the size of the BmpInfoHeader struct */
    kint32_t width;                 		/*!< image width (unit: pixel) */

    /*!< 
     * image height (unit: pixel). Positive number indicates a positive direction (starting from the bottom left corner), 
     * and a negative number indicates a reverse direction (starting from the upper left corner) 
     */
    kint32_t height;                		
    kuint16_t plane;                 		/*!< color flat book, generally fixed at 1 */
    kuint16_t pixelbit;              		/*!< number of pixel color bits: 1、4、8、16、24. (From the header of the file, this entry is at the 28th byte) */
    kuint32_t compression;           		/*!< compression properties of the data */
    kuint32_t imageSize;             		/*!< the size of the image data (unit: byte) */
    kuint32_t X_pixelPerM;           		/*!< horizontal resolution (unit: pixels/meter) */
    kuint32_t Y_pixelPerM;           		/*!< vertical resolution (unit: pixels/meter) */
    kuint32_t colorIndex;            		/*!< the number of color indexes in the palette. 16-bit and 24-bit true color without color palette. This is 0 */
    kuint32_t importanIndex;         		/*!< the number of indexes that have an important impact on the image, 0 indicates that all are important */

} srt_fwk_bmp_info_header_t;

typedef struct fwk_bmp_ctrl
{
    struct fwk_disp_info *sprt_disp;
    struct fwk_bmp_info_header sgrt_bi;

    kuint32_t x_start;
    kuint32_t y_start;
    kuint32_t x_next;
    kuint32_t y_next;

} srt_fwk_bmp_ctrl_t;

/*!< The functions */
TARGET_EXT kint32_t fwk_bitmap_get_and_check(struct fwk_bmp_ctrl *sprt_bctl, const kuint8_t *image);
TARGET_EXT kint32_t fwk_display_bitmap(struct fwk_bmp_ctrl *sprt_bctl, const kuint8_t *image, kusize_t size);
TARGET_EXT void fwk_display_whole_bitmap(struct fwk_bmp_ctrl *sprt_bctl, const kuint8_t *image);

/*!< API functions */
/*!
 * @brief   initialize fwk_bmp_ctrl
 * @param   sprt_bctl, ...
 * @retval  none
 * @note    none
 */
static inline void fwk_bitmap_ctrl_init(struct fwk_bmp_ctrl *sprt_bctl, 
                            struct fwk_disp_info *sprt_disp, kuint32_t x_start, kuint32_t y_start)
{
    sprt_bctl->x_start = x_start;
    sprt_bctl->y_start = y_start;
    sprt_bctl->x_next = sprt_bctl->y_next = 0;
    sprt_bctl->sprt_disp = sprt_disp;
    
    memset(&sprt_bctl->sgrt_bi, 0, sizeof(sprt_bctl->sgrt_bi));
}

#endif /*!< __FWK_BITMAP_H_ */
