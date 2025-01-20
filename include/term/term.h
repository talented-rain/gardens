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

#ifdef __cplusplus
    extern "C" {
#endif

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
extern struct term_cmd *term_cmd_allocate(const kchar_t *name, nrt_gfp_t gfp_mask);
extern void term_cmd_free(struct term_cmd *sprt_cmd);

extern struct term_cmd *term_cmd_find_by_name(kchar_t *name);
extern kint32_t term_cmd_add(struct term_cmd *sprt_cmd);
extern void term_cmd_del(struct term_cmd *sprt_cmd);

extern kchar_t *term_cmdline_get(void);
extern void term_cmdline_excute(kint32_t argc, kchar_t **argv);
extern void term_cmdline_distribute(const kchar_t *cmdline);

extern void term_cmd_print_login(void);
extern void term_cmd_login_init(const kchar_t *login, const kchar_t *host);
extern void term_cmd_set_login(const kchar_t *login);
extern void term_cmd_set_host(const kchar_t *host);

/*!< commands */
extern void term_cmd_add_help(void);
extern void term_cmd_add_info(void);
extern void term_cmd_add_ts(void);
extern void term_cmd_add_ttc(void);
extern void term_cmd_add_user(void);
extern void term_cmd_add_kill(void);

#ifdef __cplusplus
    }
#endif

#endif /* __TERM_H */
