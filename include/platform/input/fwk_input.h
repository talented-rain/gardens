/*
 * Input Type Defines
 *
 * File Name:   fwk_input.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.05.12
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_INPUT_H_
#define __FWK_INPUT_H_

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The includes */
#include <platform/fwk_basic.h>

/*!< The defines */
typedef struct fwk_input_event
{
	kuint32_t type;
	kuint32_t code;
	kint32_t value;

} srt_fwk_input_event_t;

enum __ERT_FWK_INPUT_TYPE
{
	NR_INPUT_TYPE_SYN = 0,
	NR_INPUT_TYPE_KEY,
	NR_INPUT_TYPE_REL,
	NR_INPUT_TYPE_ABS,

	NR_INPUT_TYPE_MAX,
	NR_INPUT_TYPE_CNT = NR_INPUT_TYPE_MAX,
};

enum __ERT_FWK_INPUT_SYN_CODE
{
	NR_INPUT_SYN_REPORT = 0,
	NR_INPUT_SYN_CONFIG,
	NR_INPUT_SYN_MT_REPORT,

	NR_INPUT_SYN_MAX,
	NR_INPUT_SYN_CNT = NR_INPUT_SYN_MAX
};

enum __ERT_FWK_INPUT_KEY_CODE
{
	/*!< line 1 */
	NR_INPUT_KEY_ESC = 0,
	NR_INPUT_KEY_F1,
	NR_INPUT_KEY_F2,
	NR_INPUT_KEY_F3,
	NR_INPUT_KEY_F4,
	NR_INPUT_KEY_F5,
	NR_INPUT_KEY_F6,
	NR_INPUT_KEY_F7,
	NR_INPUT_KEY_F8,
	NR_INPUT_KEY_F9,
	NR_INPUT_KEY_F10,
	NR_INPUT_KEY_F11,
	NR_INPUT_KEY_F12,
	NR_INPUT_KEY_INSERT,
	NR_INPUT_KEY_PRTSC,
	NR_INPUT_KEY_DEL,

	/*!< line 2 */
	NR_INPUT_KEY_1,
	NR_INPUT_KEY_2,
	NR_INPUT_KEY_3,
	NR_INPUT_KEY_4,
	NR_INPUT_KEY_5,
	NR_INPUT_KEY_6,
	NR_INPUT_KEY_7,
	NR_INPUT_KEY_8,
	NR_INPUT_KEY_9,
	NR_INPUT_KEY_0,
	NR_INPUT_KEY_MINUS,
	NR_INPUT_KEY_EQUAL,
	NR_INPUT_KEY_BACKSPACE,

	/*!< line 3 */
	NR_INPUT_KEY_TAB,
	NR_INPUT_KEY_Q,
	NR_INPUT_KEY_W,
	NR_INPUT_KEY_E,
	NR_INPUT_KEY_R,
	NR_INPUT_KEY_T,
	NR_INPUT_KEY_Y,
	NR_INPUT_KEY_U,
	NR_INPUT_KEY_I,
	NR_INPUT_KEY_O,
	NR_INPUT_KEY_P,
	NR_INPUT_KEY_LBRACE,
	NR_INPUT_KEY_RBRACE,

	/*!< line 4 */
	NR_INPUT_KEY_CAPSLK,
	NR_INPUT_KEY_A,
	NR_INPUT_KEY_S,
	NR_INPUT_KEY_D,
	NR_INPUT_KEY_F,
	NR_INPUT_KEY_G,
	NR_INPUT_KEY_H,
	NR_INPUT_KEY_J,
	NR_INPUT_KEY_K,
	NR_INPUT_KEY_L,
	NR_INPUT_KEY_SEMICOLON,
	NR_INPUT_KEY_APOSTROPHE,
	NR_INPUT_KEY_ENTER,

	/*!< line 5 */
	NR_INPUT_KEY_LSHIFT,
	NR_INPUT_KEY_Z,
	NR_INPUT_KEY_X,
	NR_INPUT_KEY_C,
	NR_INPUT_KEY_V,
	NR_INPUT_KEY_B,
	NR_INPUT_KEY_N,
	NR_INPUT_KEY_M,
	NR_INPUT_KEY_COMMA,
	NR_INPUT_KEY_DOT,
	NR_INPUT_KEY_SLASH,
	NR_INPUT_KEY_RSHIFT,

	/*!< line 6 */
	NR_INPUT_KEY_LCTRL,
	NR_INPUT_KEY_FN,
	NR_INPUT_KEY_WIN,
	NR_INPUT_KEY_LALT,
	NR_INPUT_KEY_SPACE,
	NR_INPUT_KEY_RALT,
	NR_INPUT_KEY_RCTRL,
	NR_INPUT_KEY_LEFT,
	NR_INPUT_KEY_UP,
	NR_INPUT_KEY_DOWN,
	NR_INPUT_KEY_RIGHT,

	NR_INPUT_KEY_MAX,
	NR_INPUT_KEY_CNT = NR_INPUT_KEY_MAX
};

enum __ERT_FWK_INPUT_BTN_CODE
{
	NR_INPUT_BTN_TOUCH = NR_INPUT_KEY_CNT,
	NR_INPUT_BTN_0,
	NR_INPUT_BTN_1,
	NR_INPUT_BTN_2,
	NR_INPUT_BTN_3,
	NR_INPUT_BTN_4,
	NR_INPUT_BTN_5,
	NR_INPUT_BTN_6,
	NR_INPUT_BTN_7,
	NR_INPUT_BTN_8,
	NR_INPUT_BTN_9,

	NR_INPUT_BTN_MAX,
	NR_INPUT_BTN_CNT = NR_INPUT_BTN_MAX - NR_INPUT_KEY_CNT,
};

enum __ERT_FWK_INPUT_REL_CODE
{
	NR_INPUT_REL_X = 0,
	NR_INPUT_REL_Y,
	NR_INPUT_REL_Z,

	NR_INPUT_REL_MAX,
	NR_INPUT_REL_CNT = NR_INPUT_REL_MAX
};

enum __ERT_FWK_INPUT_ABS_CODE
{
	NR_INPUT_ABS_X = 0,
	NR_INPUT_ABS_Y,
	NR_INPUT_ABS_Z,

	NR_INPUT_ABS_MT_X,
	NR_INPUT_ABS_MT_Y,
	NR_INPUT_ABS_MT_Z,

	NR_INPUT_ABS_MAX,
	NR_INPUT_ABS_CNT = NR_INPUT_ABS_MAX
};

/*!< API functions */
/*!
 * @brief   fill sprt_event
 * @param   sprt_event, type, code, and value
 * @retval  none
 * @note    none
 */
static inline void fwk_input_set_event(struct fwk_input_event *sprt_event, kuint32_t type, kuint32_t code, kint32_t value)
{
	sprt_event->type = type;
	sprt_event->code = code;
	sprt_event->value = value;
}

#ifdef __cplusplus
    }
#endif

#endif /*!< __FWK_INPUT_H_ */
