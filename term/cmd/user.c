/*
 * Terminal Core API: Command help
 *
 * File Name:   help.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.12.22
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <platform/fwk_basic.h>
#include <term/term.h>

/*!< The defines */


/*!< The globals */
static kchar_t g_term_cmd_login[128];
static kuint16_t g_term_cmd_login_len;

/*!< The functions */


/*!< API functions */
/*!
 * @brief   show login@host: \0
 * @param   none
 * @retval  none
 * @note    none
 */
void term_cmd_print_login(void)
{
    io_putstr((const kubyte_t *)g_term_cmd_login, g_term_cmd_login_len);
}

/*!
 * @brief   set host and login name
 * @param   login/host: name
 * @retval  none
 * @note    none
 */
void term_cmd_login_init(const kchar_t *login, const kchar_t *host)
{
    kchar_t *msg = &g_term_cmd_login[0];
    kssize_t len = strlen(login);

    memcpy(msg, login, len);
    msg += len;
    *(msg++) = '@';

    len = strlen(host);
    memcpy(msg, host, len);

    msg += len;
    *(msg++) = ':';
    *(msg++) = ' ';

    *msg = '\0';
    g_term_cmd_login_len = (kuint16_t)(msg - &g_term_cmd_login[0]);
}

/*!
 * @brief   set login name
 * @param   login: name
 * @retval  none
 * @note    none
 */
void term_cmd_set_login(const kchar_t *login)
{
    kchar_t *cmdline = &g_term_cmd_login[0];
    kchar_t *host, *mark;
    kssize_t login_len, host_len;

    host = kstrchr(cmdline, '@');
    if (!host)
        return;

    host++;
    mark = kstrchr(host, ':');
    host_len = (kssize_t)(mark - host);

    login_len = strlen(login);
    if ((login_len + 1) > (kssize_t)(host - cmdline))
        kstrncpyr(cmdline + login_len + 1, host, host_len);
    else
        kstrncpy(cmdline + login_len + 1, host, host_len);

    kstrncpy(cmdline, login, login_len);
    cmdline[login_len] = '@';

    login_len += (host_len + 1);
    cmdline[login_len] = ':';
    cmdline[login_len + 1] = ' ';
    cmdline[login_len + 2] = '\0';

    g_term_cmd_login_len = login_len + 2;
}

/*!
 * @brief   set host name
 * @param   host: name
 * @retval  none
 * @note    none
 */
void term_cmd_set_host(const kchar_t *host)
{
    kchar_t *cmdline = &g_term_cmd_login[0];
    kchar_t *base;
    kssize_t host_len;

    base = kstrchr(cmdline, '@');
    if (!base)
        return;

    base++;
    host_len = strlen(host);
    memcpy(base, host, host_len);

    base[host_len] = ':';
    base[host_len + 1] = ' ';
    base[host_len + 2] = '\0';

    g_term_cmd_login_len = (kuint16_t)(base - cmdline + host_len + 2);
}

/*!
 * @brief   cmd 'user': excute function
 * @param   sprt_cmd, argc, argv
 * @retval  errno
 * @note    none
 */
static kint32_t term_cmd_set_user(struct term_cmd *sprt_cmd, kint32_t argc, kchar_t **argv)
{
    switch (argc)
    {
        case 3:
            if (!strcmp(argv[1], "login"))
                term_cmd_set_login(argv[2]);
            else if (!strcmp(argv[1], "host"))
                term_cmd_set_host(argv[2]);
            else
                goto fail;

            break;

        case 2:
            if (!strcmp(argv[1], "--help"))
                sprt_cmd->help();
            else
                goto fail;

            break;

        default:
            goto fail;
    }

    return ER_NORMAL;

fail:
    printk("argument error, try entering \'%s --help\' to get usage\n", argv[0]);
    return -ER_FAULT;
}

/*!
 * @brief   cmd 'user': help function
 * @param   none
 * @retval  none
 * @note    none
 */
static void term_cmd_user_help(void)
{
    printk("usage: user [login/host] [name]\n");
}

/*!
 * @brief   cmd 'user' init and add
 * @param   none
 * @retval  none
 * @note    none
 */
void term_cmd_add_user(void)
{
    struct term_cmd *sprt_cmd;

    sprt_cmd = term_cmd_allocate("user", GFP_KERNEL);
    if (!isValid(sprt_cmd))
        return;

    sprt_cmd->do_excute = term_cmd_set_user;
    sprt_cmd->help = term_cmd_user_help;

    term_cmd_add(sprt_cmd);
}
