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
#include <platform/video/fwk_rgbmap.h>

#include "app.h"

/*!< The functions */
static void lvgl_time_event(lv_timer_t *sprt_timer);

/*!< The class */
class crt_main_ui_t
{
    friend void lvgl_time_event(lv_timer_t *sprt_timer);

public:
    crt_main_ui_t() {}
    ~crt_main_ui_t() {}

    void setup_desktop(void);
    void setup_status_bar(void);
    void setup_icon(void);

private:
    lv_obj_t *sprt_desktop;

    lv_style_t sgrt_barstyle;
    lv_obj_t *sprt_statusbar;

    lv_style_t sgrt_iconstyle;
    lv_obj_t *sprt_tagicon;
    lv_obj_t *sprt_timicon;
    lv_obj_t *sprt_pwricon;
    lv_obj_t *sprt_bellicon;
    lv_obj_t *sprt_wifiicon;
    lv_obj_t *sprt_bleicon;
    lv_obj_t *sprt_gpsicon;

    lv_timer_t *sprt_timer;
};

/*!< API functions */
void crt_main_ui_t::setup_desktop(void)
{
    lv_obj_t *sprt_obj;

    sprt_obj = lv_obj_create(lv_scr_act());
    if (!sprt_obj)
        return;

    lv_obj_set_pos(sprt_obj, 0, 0);
    lv_obj_set_size(sprt_obj, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(sprt_obj, lv_color_hex(RGB_BLACK), 0);
    lv_obj_set_style_pad_all(sprt_obj, 0, 0);
    lv_obj_set_style_radius(sprt_obj, 0, 0);
    lv_obj_set_style_bg_img_src(sprt_obj, CONFIG_WALL_PAPER, 0);
    lv_obj_set_scrollbar_mode(sprt_obj, LV_SCROLLBAR_MODE_OFF); 
    lv_obj_set_style_border_width(sprt_obj, 0, 0);
    lv_obj_center(sprt_obj);

    this->sprt_desktop = sprt_obj;
}

void crt_main_ui_t::setup_status_bar(void)
{
    lv_obj_t *sprt_obj;
    lv_style_t *sprt_style = &this->sgrt_barstyle;

    sprt_obj = lv_obj_create(this->sprt_desktop);
    if (!sprt_obj)
        return;

    lv_style_init(sprt_style);
    lv_style_set_bg_color(sprt_style, lv_color_hex(RGB_BLACK));
    lv_style_set_bg_img_opa(sprt_style, 0);
    lv_style_set_border_width(sprt_style, 0);

    lv_obj_set_width(sprt_obj, lv_pct(100));
    lv_obj_set_height(sprt_obj, 32);
    lv_obj_add_style(sprt_obj, sprt_style, 0);
    lv_obj_set_align(sprt_obj, LV_ALIGN_TOP_MID);

    this->sprt_statusbar = sprt_obj;
}

void crt_main_ui_t::setup_icon(void)
{
    lv_obj_t *sprt_obj;
    lv_obj_t *sprt_neibh = this->sprt_statusbar;
    lv_style_t *sprt_style = &this->sgrt_iconstyle;

    lv_style_init(sprt_style);
    lv_style_set_text_color(sprt_style, lv_color_hex(RGB_WHITE));
    lv_style_set_text_font(sprt_style, &lv_font_montserrat_16);

    /*!< heavenfox text */
    sprt_obj = lv_label_create(this->sprt_statusbar);
    if (sprt_obj) {
        lv_label_set_text(sprt_obj, "heavenfox");
        lv_obj_add_style(sprt_obj, sprt_style, 0);
        lv_obj_align_to(sprt_obj, this->sprt_statusbar, LV_ALIGN_LEFT_MID, 0, 0);

        this->sprt_tagicon = sprt_obj;
    }

    /*!< time text */
    sprt_obj = lv_label_create(this->sprt_statusbar);
    if (sprt_obj) {
        lv_timer_t *sprt_timer;

//      struct time_clock *sprt_tclk;
//      sprt_tclk = &sgrt_systime_clock;
//      lv_label_set_text_fmt(sprt_obj, "system run time: %d-%d-%d %d:%d:%d:%d",
//              sprt_tclk->year, sprt_tclk->month, sprt_tclk->day,
//              sprt_tclk->hour, sprt_tclk->minute, sprt_tclk->second, sprt_tclk->milsecond);

       lv_label_set_text_fmt(sprt_obj, "yujiantianhu@163.com");

        lv_obj_add_style(sprt_obj, sprt_style, 0);
        lv_obj_align_to(sprt_obj, this->sprt_statusbar, LV_ALIGN_CENTER, 0, 0);

        sprt_timer = lv_timer_create(lvgl_time_event, 10, this);

        this->sprt_timicon = sprt_obj;
        this->sprt_timer = sprt_timer;
    }

    /*!< power icon */
    sprt_obj = lv_label_create(this->sprt_statusbar);
    if (sprt_obj) {
        lv_label_set_text(sprt_obj, LV_SYMBOL_BATTERY_3);
        lv_obj_add_style(sprt_obj, sprt_style, 0);
        lv_obj_align_to(sprt_obj, sprt_neibh, LV_ALIGN_RIGHT_MID, 0, 0);

        sprt_neibh = sprt_obj;
        this->sprt_pwricon = sprt_obj;
    }

    /*!< bell icon */
    sprt_obj = lv_label_create(this->sprt_statusbar);
    if (sprt_obj) {
        lv_label_set_text(sprt_obj, LV_SYMBOL_BELL);
        lv_obj_add_style(sprt_obj, sprt_style, 0);
        lv_obj_align_to(sprt_obj, sprt_neibh, LV_ALIGN_OUT_LEFT_MID, -10, 0);

        sprt_neibh = sprt_obj;
        this->sprt_bellicon = sprt_obj;
    }

    /*!< wifi icon */
    sprt_obj = lv_label_create(this->sprt_statusbar);
    if (sprt_obj) {
        lv_label_set_text(sprt_obj, LV_SYMBOL_WIFI);
        lv_obj_add_style(sprt_obj, sprt_style, 0);
        lv_obj_align_to(sprt_obj, sprt_neibh, LV_ALIGN_OUT_LEFT_MID, -10, 0);

        sprt_neibh = sprt_obj;
        this->sprt_wifiicon = sprt_obj;
    }

    /*!< BlueTooth icon */
    sprt_obj = lv_label_create(this->sprt_statusbar);
    if (sprt_obj) {
        lv_label_set_text(sprt_obj, LV_SYMBOL_BLUETOOTH);
        lv_obj_add_style(sprt_obj, sprt_style, 0);
        lv_obj_align_to(sprt_obj, sprt_neibh, LV_ALIGN_OUT_LEFT_MID, -10, 0);

        sprt_neibh = sprt_obj;
        this->sprt_bleicon = sprt_obj;
    }

    /*!< gps icon */
    sprt_obj = lv_label_create(this->sprt_statusbar);
    if (sprt_obj) {
        lv_label_set_text(sprt_obj, LV_SYMBOL_GPS);
        lv_obj_add_style(sprt_obj, sprt_style, 0);
        lv_obj_align_to(sprt_obj, sprt_neibh, LV_ALIGN_OUT_LEFT_MID, -10, 0);

        sprt_neibh = sprt_obj;
        this->sprt_gpsicon = sprt_obj;
    }
}

static void lvgl_time_event(lv_timer_t *sprt_timer)
{
//  crt_main_ui_t *sprt_this;
//  struct time_clock *sprt_tclk;
//  
//  sprt_this = (crt_main_ui_t *)sprt_timer->user_data;
//  sprt_tclk = &sgrt_systime_clock;
//  lv_label_set_text_fmt(sprt_this->sprt_timicon, "system run time: %d-%d-%d %d:%d:%d:%d",
//          sprt_tclk->year, sprt_tclk->month, sprt_tclk->day,
//          sprt_tclk->hour, sprt_tclk->minute, sprt_tclk->second, sprt_tclk->milsecond);
}

/*!
 * @brief  start up
 * @param  sprt_dctrl
 * @retval none
 * @note   none
 */
void lvgl_task_setup(void *args)
{
    crt_main_ui_t *cprt_ui;

    cprt_ui = new crt_main_ui_t;
    if (!cprt_ui)
        return;

    cprt_ui->setup_desktop();
    cprt_ui->setup_status_bar();
    cprt_ui->setup_icon();
}

/*!
 * @brief  main
 * @param  args
 * @retval none
 * @note   none
 */
void lvgl_task(void *args)
{
    lv_timer_handler();
}

/*!< end of file */
