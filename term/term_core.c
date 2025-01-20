/*
 * Terminal Core API
 *
 * File Name:   term_core.c
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
static kchar_t g_term_cmdline_argv[TERM_MSG_RECV_LEN];

/*!< top command: help */
static DECLARE_LIST_HEAD(sgrt_term_cmd_lists);
 
/*!< The functions */
/*!
 * @brief   find command by name
 * @param   name
 * @retval  term_cmd
 * @note    none
 */
struct term_cmd *term_cmd_find_by_name(kchar_t *name)
{
    struct term_cmd *sprt_cmd;

    if (!name || !(*name))
        goto fail;

    foreach_list_next_entry(sprt_cmd, &sgrt_term_cmd_lists, sgrt_link)
    {
        if (!strcmp(sprt_cmd->name, name))
            return sprt_cmd;
    }

fail:
    return mrt_nullptr;
}

/*!
 * @brief   create new command
 * @param   name: command' name
 * @retval  term_cmd
 * @note    none
 */
struct term_cmd *term_cmd_allocate(const kchar_t *name, nrt_gfp_t gfp_mask)
{
    struct term_cmd *sprt_cmd;
    kssize_t size;

    sprt_cmd = kmalloc(sizeof(*sprt_cmd), gfp_mask);
    if (!isValid(sprt_cmd))
        return sprt_cmd;

    size = strlen(name);
    memcpy(sprt_cmd->name, name, size);
    sprt_cmd->name[size] = '\0';

    sprt_cmd->do_excute = mrt_nullptr;
    sprt_cmd->help = mrt_nullptr;
    init_list_head(&sprt_cmd->sgrt_link);

    return sprt_cmd;
}

/*!
 * @brief   release command
 * @param   sprt_cmd
 * @retval  none
 * @note    none
 */
void term_cmd_free(struct term_cmd *sprt_cmd)
{
    if (!mrt_list_head_empty(&sprt_cmd->sgrt_link))
        term_cmd_del(sprt_cmd);

    kfree(sprt_cmd);
}

/*!
 * @brief   add command
 * @param   sprt_cmd
 * @retval  errno
 * @note    none
 */
kint32_t term_cmd_add(struct term_cmd *sprt_cmd)
{
    if ((!sprt_cmd->name) ||
        (*sprt_cmd->name == '\0') ||
        (kstrchr(sprt_cmd->name, ' ')))
        return -ER_UNVALID;

    if (term_cmd_find_by_name(sprt_cmd->name))
        return -ER_EXISTED;

    list_head_add_tail(&sgrt_term_cmd_lists, &sprt_cmd->sgrt_link);
    return ER_NORMAL;
}

/*!
 * @brief   detach command
 * @param   sprt_cmd
 * @retval  none
 * @note    none
 */
void term_cmd_del(struct term_cmd *sprt_cmd)
{
    list_head_del(&sprt_cmd->sgrt_link);    
}

/*!
 * @brief   call command's handler
 * @param   argc: argument's number
 * @param   argv: arguments
 * @retval  none
 * @note    none
 */
void term_cmdline_excute(kint32_t argc, kchar_t **argv)
{
    struct term_cmd *sprt_cmd;

    if (argc < 1)
        return;

    foreach_list_next_entry(sprt_cmd, &sgrt_term_cmd_lists, sgrt_link)
    {
        if (!strcmp(sprt_cmd->name, argv[0]))
            goto succ;
    }

    printk("can not access the command!\n");
    return;

succ:
    if (sprt_cmd->do_excute)
        sprt_cmd->do_excute(sprt_cmd, argc, argv);
}

/*!
 * @brief   deal with per command
 * @param   cmdline
 * @retval  none
 * @note    none
 */
void term_cmdline_distribute(const kchar_t *cmdline)
{
    kchar_t *msg;
    kusize_t cmdlen;
    kint32_t argc = 0;
    kchar_t *argv[32];

    cmdlen = strlen(cmdline);
    if (!cmdlen)
        return;

    msg = &g_term_cmdline_argv[0];
    memcpy(msg, cmdline, cmdlen + 1);

    do {
        argv[argc++] = msg;

        msg = kstrchr(msg, ' ');
        if (msg)
            *(msg++) = '\0';

    } while (msg && *msg);

    term_cmdline_excute(argc, argv);
}

/*!< ----------------------------------------------------------- */
/*!
 * @brief   cmd 'help': excute function
 * @param   sprt_cmd, argc, argv
 * @retval  errno
 * @note    none
 */
static kint32_t term_cmd_show_help(struct term_cmd *sprt_cmd, kint32_t argc, kchar_t **argv)
{
    struct term_cmd *sprt_per;

    if (argc != 1)
        goto fail;

    printk("command list: >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    foreach_list_next_entry(sprt_per, &sgrt_term_cmd_lists, sgrt_link)
    {
        if (sprt_per->help)
        {
            printk("\'%s\'> ", sprt_per->name);
            sprt_per->help();
        }
    }

    return ER_NORMAL;

fail:
    printk("argument is too many\n");
    return -ER_FAULT;
}

/*!
 * @brief   cmd 'help' init and add
 * @param   none
 * @retval  none
 * @note    none
 */
void term_cmd_add_help(void)
{
    struct term_cmd *sprt_cmd;

    sprt_cmd = term_cmd_allocate("help", GFP_KERNEL);
    if (!isValid(sprt_cmd))
        return;

    sprt_cmd->do_excute = term_cmd_show_help;
    sprt_cmd->help = mrt_nullptr;

    term_cmd_add(sprt_cmd);
}