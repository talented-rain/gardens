/*
 * User Thread Instance (button task) Interface
 *
 * File Name:   botton_text.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.11.17
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The globals */
#include <lvgl/lvgl.h>
#include "botton_text.h"

/*!< The defines */


/*!< The globals */


/*!< API functions */
/*!
 * @brief  start up
 * @param  sprt_dctrl
 * @retval none
 * @note   none
 */
void lvgl_botton_text(void *args)
{
    /*!< botton */
    lv_obj_t *myBtn = lv_btn_create(lv_scr_act());
    lv_obj_set_pos(myBtn, 10, 10);
    lv_obj_set_size(myBtn, 120, 50);
   
    /*!< text1 (on the botton) */
    lv_obj_t *label_btn = lv_label_create(myBtn);
    lv_obj_align(label_btn, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(label_btn, "Test");
 
    /*!< text2 */
    lv_obj_t *myLabel = lv_label_create(lv_scr_act());
    lv_label_set_text(myLabel, "Hello world!");
    lv_obj_align(myLabel, LV_ALIGN_CENTER, 0, 0);
    lv_obj_align_to(myBtn, myLabel, LV_ALIGN_OUT_TOP_MID, 0, -20);
}

/*!< end of file */
