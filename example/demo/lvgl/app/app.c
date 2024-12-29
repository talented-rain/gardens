/*
 * User Thread Instance (button task) Interface
 *
 * File Name:   button_app.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.05.25
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The globals */
#include "../porting/lv_port_disp_template.h"
#include "../porting/lv_port_fs_template.h"
#include "../component/widget.h"
#include "../component/botton_text.h"

#include "app.h"

/*!< The defines */


/*!< The globals */
static lv_obj_t *sprt_lvgl_img;

static const kchar_t *g_lvgl_bmp_list[] =
{
    "/media/FAT32_2/home/fox/picture/1.bmp",
    "/media/FAT32_2/home/fox/picture/2.bmp",
    "/media/FAT32_2/home/fox/picture/3.bmp",
    "/media/FAT32_2/home/fox/picture/4.bmp",
    "/media/FAT32_2/home/fox/picture/5.bmp"
};

/*!< API functions */
/*!
 * @brief  start up
 * @param  sprt_dctrl
 * @retval none
 * @note   none
 */
void lvgl_task_startup(void *args)
{
    struct fwk_disp_ctrl *sprt_dctrl = (struct fwk_disp_ctrl *)args;

    lv_init();
    lv_port_disp_init(sprt_dctrl);
    lv_port_fs_init();

    /*!< widget */
//  lvgl_widget_draw(sprt_dctrl);

    /*!< botton */
//  lvgl_botton_text(sprt_dctrl);

    sprt_lvgl_img = lv_img_create(lv_scr_act());
    lv_obj_center(sprt_lvgl_img);
}

/*!
 * @brief  main
 * @param  args
 * @retval none
 * @note   none
 */
void lvgl_task(void *args)
{
    static kutime_t time = 0;
    static kuint32_t index = 0;

    if (time != jiffies)
    {
        time = jiffies;
        if (index >= ARRAY_SIZE(g_lvgl_bmp_list))
            index = 0;

        lv_img_set_src(sprt_lvgl_img, g_lvgl_bmp_list[index++]);
        lv_timer_handler();
    }
}

/*!< end of file */
