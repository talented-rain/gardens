/**
 * @file lv_port_disp_templ.c
 * 
 * for HeavenFox OS
 *  ---- Yang Yujun
 */

/*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp_template.h"
#include <stdbool.h>

/*********************
 *      DEFINES
 *********************/
// #ifndef MY_DISP_HOR_RES
//     #warning Please define or replace the macro MY_DISP_HOR_RES with the actual screen width, default value 320 is used for now.
//     #define MY_DISP_HOR_RES    320
// #endif

// #ifndef MY_DISP_VER_RES
//     #warning Please define or replace the macro MY_DISP_HOR_RES with the actual screen height, default value 240 is used for now.
//     #define MY_DISP_VER_RES    240
// #endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static kint32_t disp_init(struct fwk_disp_ctrl *sprt_dctrl);

static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//        const lv_area_t * fill_area, lv_color_t color);

/**********************
 *  STATIC VARIABLES
 **********************/
static struct timer_list sgrt_lvgl_tick_timer;
static kint32_t g_lvgl_fbdev_fd = -1;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(struct fwk_disp_ctrl *sprt_dctrl)
{
    struct fwk_disp_info *sprt_disp;

    sprt_disp = sprt_dctrl->sprt_di;
    if (!isValid(sprt_disp))
        return;

    /*-------------------------
     * Initialize your display
     * -----------------------*/
    if (disp_init(sprt_dctrl))
        return;

    /*-----------------------------
     * Create a buffer for drawing
     *----------------------------*/

    /**
     * LVGL requires a buffer where it internally draws the widgets.
     * Later this buffer will passed to your display driver's `flush_cb` to copy its content to your display.
     * The buffer has to be greater than 1 display row
     *
     * There are 3 buffering configurations:
     * 1. Create ONE buffer:
     *      LVGL will draw the display's content here and writes it to your display
     *
     * 2. Create TWO buffer:
     *      LVGL will draw the display's content to a buffer and writes it your display.
     *      You should use DMA to write the buffer's content to the display.
     *      It will enable LVGL to draw the next part of the screen to the other buffer while
     *      the data is being sent form the first buffer. It makes rendering and flushing parallel.
     *
     * 3. Double buffering
     *      Set 2 screens sized buffers and set disp_drv.full_refresh = 1.
     *      This way LVGL will always provide the whole rendered screen in `flush_cb`
     *      and you only need to change the frame buffer's address.
     */

#if 0
    /* Example for 1) */
    static lv_disp_draw_buf_t draw_buf_dsc_1;
    static lv_color_t buf_1[MY_DISP_HOR_RES * 10];                          /*A buffer for 10 rows*/
    lv_disp_draw_buf_init(&draw_buf_dsc_1, buf_1, NULL, MY_DISP_HOR_RES * 10);   /*Initialize the display buffer*/

    /* Example for 2) */
    static lv_disp_draw_buf_t draw_buf_dsc_2;
    static lv_color_t buf_2_1[MY_DISP_HOR_RES * 10];                        /*A buffer for 10 rows*/
    static lv_color_t buf_2_2[MY_DISP_HOR_RES * 10];                        /*An other buffer for 10 rows*/
    lv_disp_draw_buf_init(&draw_buf_dsc_2, buf_2_1, buf_2_2, MY_DISP_HOR_RES * 10);   /*Initialize the display buffer*/

    /* Example for 3) also set disp_drv.full_refresh = 1 below*/
    static lv_disp_draw_buf_t draw_buf_dsc_3;
    static lv_color_t buf_3_1[MY_DISP_HOR_RES * MY_DISP_VER_RES];            /*A screen sized buffer*/
    static lv_color_t buf_3_2[MY_DISP_HOR_RES * MY_DISP_VER_RES];            /*Another screen sized buffer*/
    lv_disp_draw_buf_init(&draw_buf_dsc_3, buf_3_1, buf_3_2,
                          MY_DISP_VER_RES * LV_VER_RES_MAX);   /*Initialize the display buffer*/

#else

    static lv_disp_draw_buf_t draw_buf_dsc_4;
    lv_disp_draw_buf_init(&draw_buf_dsc_4, sprt_disp->buffer_bak, sprt_disp->buffer,
                          sprt_disp->height * sprt_disp->width);   /*Initialize the display buffer*/

#endif

    /*-----------------------------------
     * Register the display in LVGL
     *----------------------------------*/

    static lv_disp_drv_t disp_drv;                         /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/

    /*Set up the functions to access to your display*/

    /*Set the resolution of the display*/
    disp_drv.hor_res = sprt_disp->width;
    disp_drv.ver_res = sprt_disp->height;

    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = disp_flush;

    /*Set a display buffer*/
    disp_drv.draw_buf = &draw_buf_dsc_4;

    disp_drv.user_data = (void *)sprt_dctrl;

    /*Required for Example 3)*/
    //disp_drv.full_refresh = 1;

    /* Fill a memory array with a color if you have GPU.
     * Note that, in lv_conf.h you can enable GPUs that has built-in support in LVGL.
     * But if you have a different GPU you can use with this callback.*/
    //disp_drv.gpu_fill_cb = gpu_fill;

    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void lvgl_disp_tick_inc(kuint32_t args)
{
    struct timer_list *sprt_tim = (struct timer_list *)args;

    lv_tick_inc(10);
    mod_timer(sprt_tim, jiffies + msecs_to_jiffies(10));
}

/*Initialize your display and the required peripherals.*/
static kint32_t disp_init(struct fwk_disp_ctrl *sprt_dctrl)
{
    /*You code here*/
    struct timer_list *sprt_tim = &sgrt_lvgl_tick_timer;
    kint32_t fd;
    struct fwk_fb_fix_screen_info sgrt_fix;
	struct fwk_fb_var_screen_info sgrt_var;
    kuint32_t *fb_buffer1, *fb_buffer2;
    struct fwk_disp_info *sprt_disp;

    sprt_disp = sprt_dctrl->sprt_di;
    kmemzero(&sprt_dctrl->sgrt_set, sizeof(struct fwk_font_setting));

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

    g_lvgl_fbdev_fd = fd;
    fwk_display_ctrl_init(sprt_disp, fb_buffer1, fb_buffer2, sgrt_fix.smem_len, 
                        sgrt_var.xres, sgrt_var.yres, sgrt_var.bits_per_pixel);

    /*!< add timer tick */
    setup_timer(sprt_tim, lvgl_disp_tick_inc, (kuint32_t)sprt_tim);
    sprt_tim->expires = jiffies + msecs_to_jiffies(10);
    add_timer(sprt_tim);

    return ER_NORMAL;

    virt_munmap(fb_buffer2, sgrt_fix.smem_len);
fail3:
    virt_munmap(fb_buffer1, sgrt_fix.smem_len);
fail2:
    virt_close(fd);
fail1:
    return -ER_FAILD;
}

volatile bool disp_flush_enabled = true;

/* Enable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_enable_update(void)
{
    disp_flush_enabled = true;
}

/* Disable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_disable_update(void)
{
    disp_flush_enabled = false;
}

/*Flush the content of the internal buffer the specific area on the display
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_disp_flush_ready()' has to be called when finished.*/
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    if (disp_flush_enabled) {
        /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/

        struct fwk_disp_ctrl *sprt_dctrl;
        struct fwk_disp_info *sprt_disp;
        kuint8_t  pixelbits;
        lv_disp_t * sprt_refr;
        lv_disp_draw_buf_t *sprt_draw;
        
        sprt_dctrl = (struct fwk_disp_ctrl *)disp_drv->user_data;
        sprt_disp  = sprt_dctrl->sprt_di;
        pixelbits  = mrt_fwk_disp_bpp_get(sprt_disp->bpp);

        sprt_refr = _lv_refr_get_disp_refreshing();
        sprt_draw = lv_disp_get_draw_buf(sprt_refr);

        if ((pixelbits == 32) &&
            (!disp_drv->direct_mode) &&
            (sprt_draw->flushing_last)) {
            kint32_t fd = g_lvgl_fbdev_fd;
            struct fwk_fb_var_screen_info sgrt_var;

            if (fd < 0)
                goto END;

            fwk_display_frame_exchange(sprt_disp);
            virt_ioctl(fd, NR_FB_IOGET_VARINFO, &sgrt_var);
            if (!sgrt_var.yoffset)
                sgrt_var.yoffset += sgrt_var.yres;
            else
                sgrt_var.yoffset = 0;
            
            virt_ioctl(fd, NR_FB_IOSET_VARINFO, &sgrt_var);
        }
        else {
            kuint32_t offset, length;

            pixelbits >>= 3;
            for (int32_t y = area->y1; y <= area->y2; y++) {
                offset = mrt_fwk_disp_advance_pos(area->x1, y, sprt_disp->width);
                length = area->x2 - area->x1 + 1;

                memcpy(sprt_disp->buffer + offset * pixelbits, color_p, length * pixelbits);
                color_p += length;
            }
        }
    }

END:
    /*IMPORTANT!!!
     *Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp_drv);
}

/*OPTIONAL: GPU INTERFACE*/

/*If your MCU has hardware accelerator (GPU) then you can use it to fill a memory with a color*/
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//                    const lv_area_t * fill_area, lv_color_t color)
//{
//    /*It's an example code which should be done by your GPU*/
//    int32_t x, y;
//    dest_buf += dest_width * fill_area->y1; /*Go to the first line*/
//
//    for(y = fill_area->y1; y <= fill_area->y2; y++) {
//        for(x = fill_area->x1; x <= fill_area->x2; x++) {
//            dest_buf[x] = color;
//        }
//        dest_buf+=dest_width;    /*Go to the next line*/
//    }
//}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
