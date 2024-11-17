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
#include "../component/botton_text.h"
#include "app.h"

/*!< The defines */


/*!< The globals */


/*!< API functions */
/*!
 * @brief  start up
 * @param  sprt_dctrl
 * @retval none
 * @note   none
 */
void lvgl_task_startup(struct fwk_disp_ctrl *sprt_dctrl)
{
    lv_init();
    lv_port_disp_init(sprt_dctrl);

    /*!< botton */
    lvgl_botton_text(sprt_dctrl);
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

    if (time != jiffies)
    {
        time = jiffies;
        lv_timer_handler();
    }
}

/*!< end of file */
