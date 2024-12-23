/*
 * Terminal Core API: Command info
 *
 * File Name:   info.c
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


/*!< The functions */

/*!< API functions */
/*!
 * @brief   cmd 'info': excute function
 * @param   sprt_cmd, argc, argv
 * @retval  errno
 * @note    none
 */
static kint32_t term_cmd_show_info(struct term_cmd *sprt_cmd, kint32_t argc, kchar_t **argv)
{
    switch (argc)
    {
        case 1:
            printk("----------------------------------------------------\n");
            printk("        Kernel:     HeavenFox OS                    \n");
            printk("        Author:     Yang Yujun                      \n");
            printk("        Country:    China                           \n");
            printk("        Province:   GuangXi                         \n");
            printk("        E-mail:     <yujiantianhu@163.com>          \n");
            printk("----------------------------------------------------\n");

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
 * @brief   cmd 'info': help function
 * @param   none
 * @retval  none
 * @note    none
 */
static void term_cmd_info_help(void)
{
    printk("usage: info\n");
}

/*!
 * @brief   cmd 'info' init and add
 * @param   none
 * @retval  none
 * @note    none
 */
void term_cmd_add_info(void)
{
    struct term_cmd *sprt_cmd;

    sprt_cmd = term_cmd_allocate("info", GFP_KERNEL);
    if (!isValid(sprt_cmd))
        return;

    sprt_cmd->do_excute = term_cmd_show_info;
    sprt_cmd->help = term_cmd_info_help;

    term_cmd_add(sprt_cmd);
}
