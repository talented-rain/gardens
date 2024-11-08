/*
 * Framebuffer Driver Top Interface
 *
 * File Name:   fwk_fbmem.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.07.01
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_FBMEM_H_
#define __FWK_FBMEM_H_

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/fwk_platform.h>

/*!< The defines */
/*!< Maximum supported resolution */
#define FWK_FB_MAX_DPI									(1920 * 1080)
/*!< Each pixel supports up to 32 bits */
#define FWK_FB_MAX_BPP									(32)

/*!< 
 * The RGB bit field of the fb cache, which describes how each pixel display buffer is organized, 
 * in RGB565 mode, R takes 5 bits = bit[11:15] G takes 6 bits = bit[10:5] B takes 5 bits = bit[4:0] 
 */
struct fwk_fb_bitfield
{
    kuint32_t offset;        							/*!< Bitfield offset: red = 11, green = 5, blue = 0 */
    kuint32_t length;       							/*!< Bitfield lenth: red = 5,  green = 6, blue = 5 */
    kuint32_t msb_right; 								/*!< (msb_right != 0) ===> MSB on the right. Generally, this value is set to 0 */
};

/*!< LCD fix parameters */
struct fwk_fb_fix_screen_info
{
    kchar_t id[16];										/*!< LCD identifier in the form of a string */
    kuaddr_t smem_start;								/*!< The starting physical address of the framebuffer memory */
    kuint32_t smem_len;									/*!< The length of the framebuffer memory */
    kuint32_t type;										/*!< The type of frame buffer storage, i.e. the way in which image data is stored in the frame buffer */
    kuint32_t type_aux;									/*!< Subtype of the framebuffer store */
    kuint32_t visual;									/*!< Screen color mode, such as black and white mode, true color mode, etc */

    kuint32_t line_lenth;								/*!< The number of bytes in a row */
};

/*!< LCD variable parameters */
struct fwk_fb_var_screen_info
{
    kuint32_t xres;										/*!< Visual screen resolution: x-direction width */
    kuint32_t yres;										/*!< Visual screen resolution: y-direction width */
    kuint32_t xres_virtual;								/*!< Virtual screen resolution: x-direction width */
    kuint32_t yres_virtual;								/*!< Virtual screen resolution: y-direction width */
    kuint32_t xoffset;									/*!< X-Direction: The offset between the virtual screen and the visual screen */
    kuint32_t yoffset;									/*!< Y-Direction: The offset between the virtual screen and the visual screen */

    kuint32_t bits_per_pixel;							/*!< bpp, The number of bits required per pixel, such as 16, 24, 32, ... */

    struct fwk_fb_bitfield red;							/*!< RGB red bit field */
    struct fwk_fb_bitfield green;						/*!< RGB green bit field */
    struct fwk_fb_bitfield blue;						/*!< RGB blue bit field */
    struct fwk_fb_bitfield transp;						/*!< RGB deaphaneity bit field, which is only present when (bpp == 32) */

    kuint32_t height;									/*!< Screen height (unit: ms) */
    kuint32_t width;									/*!< Screen width (unit: ms) */

    kuint32_t pixclock;									/*!< Pixel clock, unit: ps(picoseconds) */

    /*!<
     * The time (unit: pixclock) from the end of the line synchronization signal hsync to the start of drawing a new line image is actually HBP, 
     * the horizontal synchronization trailing edge
     */
    kuint32_t left_margin;
    /*!<
     * The time (unit: pixclock) from the end of one line of image drawing to the start of the line synchronization signal of the next line is actually HFP, 
     * the horizontal synchronization front
     */
    kuint32_t right_margin;
    /*!<
     * The time from the end of the frame synchronization signal vsync to the start of drawing a new image (unit: pixclock) is actually VBP, 
     * vertical synchronization trailing edge
     */
    kuint32_t upper_margin;
    /*!<
     * The time (unit: pixclock) from the end of one frame to the start of the frame synchronization signal of the next frame is actually VFP, 
     * vertical Sync Frontier
     */
    kuint32_t lower_margin;

    kuint32_t hsync_len;								/*!< The pulse width (unit: pixclock) of the horizontal synchronization signal hsync, i.e. hpw */
    kuint32_t vsync_len;								/*!< The pulse width (unit: PixClock) of the V-sync signal vsync, i.e. VPW */
};

enum __ERT_FB_IOCTL_CMD
{
    NR_FB_IOGET_VARINFO	= FWK_IOR('F', 0, struct fwk_fb_var_screen_info),
    NR_FB_IOSET_VARINFO = FWK_IOW('F', 1, struct fwk_fb_var_screen_info),
    NR_FB_IOGET_FIXINFO = FWK_IOR('F', 2, struct fwk_fb_fix_screen_info),
};

typedef struct fwk_fb_info
{
    kint32_t node;										/*!< minor */
    kint32_t flags;

    struct fwk_fb_fix_screen_info sgrt_fix;				/*!< fix parameters */
    struct fwk_fb_var_screen_info sgrt_var;				/*!< variadics */

    const struct fwk_fb_oprts *sprt_fbops;				/*!< operation API */
    struct fwk_device *sprt_dev;						/*!< device */
    struct fwk_device *sprt_idev;

    kuint8_t *screen_base;								/*!< base address of the virtual memory */
    kuint32_t screen_size;								/*!< virtual memory size */

    void *ptr_par;										/*!< private data of the framebuffer device */
    
} srt_fwk_fb_info_t;

struct fwk_fb_oprts
{
    kint32_t (*fb_open) (struct fwk_fb_info *sprt_info, kint32_t user);
    kint32_t (*fb_release) (struct fwk_fb_info *sprt_info, kint32_t user);

    kssize_t (*fb_read) (struct fwk_fb_info *sprt_info, kbuffer_t *ptr_buf, kusize_t count, kuint8_t *ptr_offset);
    kssize_t (*fb_write) (struct fwk_fb_info *sprt_info, const kbuffer_t *ptr_buf, kusize_t count, kuint8_t *ptr_offset);
    kint32_t (*fb_ioctl) (struct fwk_fb_info *sprt_info, kuint32_t cmd, kuaddr_t arg);
    kint32_t (*fb_mmap) (struct fwk_fb_info *sprt_info, struct fwk_vm_area *vma);
};

/*!< The functions */
/*!< -------------------------------------------------------------- */
TARGET_EXT kint32_t fwk_fbmem_init(void);
TARGET_EXT void fwk_fbmem_exit(void);

/*!< -------------------------------------------------------------- */
TARGET_EXT struct fwk_fb_info *fwk_framebuffer_alloc(kusize_t size, struct fwk_device *sprt_dev);
TARGET_EXT void fwk_framebuffer_release(struct fwk_fb_info *sprt_fb_info);
TARGET_EXT kint32_t fwk_register_framebuffer(struct fwk_fb_info *sprt_fb_info);
TARGET_EXT void fwk_unregister_framebuffer(struct fwk_fb_info *sprt_fb_info);
TARGET_EXT struct fwk_fb_info *fwk_get_fb_info(kuint32_t idx);
TARGET_EXT struct fwk_fb_info *fwk_file_fb_info(struct fwk_file *sprt_file);

/*!< API functions */
/*!
 * @brief   get driver data from fb device
 * @param   sprt_fb_info
 * @retval  ptr_par
 * @note    none
 */
static inline void *fwk_fb_get_drvdata(struct fwk_fb_info *sprt_fb_info)
{
    return sprt_fb_info ? sprt_fb_info->ptr_par : mrt_nullptr;
}

#endif /*!< __FWK_FBMEM_H_ */
