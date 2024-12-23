/*
 * String Function Declare
 *
 * File Name:   term.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.09.26
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __TERM_H
#define __TERM_H

/*!< The includes */
#include <common/basic_types.h>
#include <common/error_types.h>
#include <common/generic.h>
#include <common/ascii.h>
#include <platform/input/fwk_kbd.h>

/*!< The defines */
#define TERM_MSG_RECV_LEN                               1024

typedef void (*term_cmd_fn_t)(void);

struct term_cmd
{
    kchar_t name[32];

    kint32_t (*do_excute)(struct term_cmd *, kint32_t argc, kchar_t **argv);
    void (*help)(void);

    struct list_head sgrt_link;
};

/*!< The functions */
TARGET_EXT struct term_cmd *term_cmd_allocate(const kchar_t *name, nrt_gfp_t gfp_mask);
TARGET_EXT void term_cmd_free(struct term_cmd *sprt_cmd);

TARGET_EXT struct term_cmd *term_cmd_find_by_name(kchar_t *name);
TARGET_EXT kint32_t term_cmd_add(struct term_cmd *sprt_cmd);
TARGET_EXT void term_cmd_del(struct term_cmd *sprt_cmd);

TARGET_EXT kchar_t *term_cmdline_get(void);
TARGET_EXT void term_cmdline_excute(kint32_t argc, kchar_t **argv);
TARGET_EXT void term_cmdline_distribute(const kchar_t *cmdline);

TARGET_EXT void term_cmd_print_login(void);
TARGET_EXT void term_cmd_login_init(const kchar_t *login, const kchar_t *host);
TARGET_EXT void term_cmd_set_login(const kchar_t *login);
TARGET_EXT void term_cmd_set_host(const kchar_t *host);

/*!< commands */
TARGET_EXT void term_cmd_add_help(void);
TARGET_EXT void term_cmd_add_info(void);
TARGET_EXT void term_cmd_add_ttc(void);
TARGET_EXT void term_cmd_add_user(void);
TARGET_EXT void term_cmd_add_kill(void);

#endif /* __TERM_H */
