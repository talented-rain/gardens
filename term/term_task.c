/*
 * Terminal Task Interface
 *
 * File Name:   term_task.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.12.21
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <platform/fwk_basic.h>
#include <kernel/kernel.h>
#include <kernel/sched.h>
#include <kernel/thread.h>
#include <kernel/instance.h>
#include <kernel/mailbox.h>
#include <term/term.h>

/*!< The defines */
struct term_kbd_priv;

#define TERM_TASK_THREAD_STACK_SIZE             THREAD_STACK_HALF(1)    /*!< 1/2 page (1kbytes) */

struct term_kbd_handle
{
    kint32_t key;
    void (*do_cmd) (struct term_kbd_priv *sprt_priv, kuint32_t *inc);
};

struct term_kbd_priv
{  
    kubyte_t *msg;

    kint32_t key;
    kuint8_t len;
};

/*!< The globals */
static tid_t g_term_task_tid;
static struct thread_attr sgrt_term_task_attr;
static kuint32_t g_term_task_stack[TERM_TASK_THREAD_STACK_SIZE];
static struct mailbox sgrt_term_task_mailbox;

static kubyte_t g_term_cmdline[TERM_MSG_RECV_LEN];
static kuint32_t g_term_inc;

/*!< API functions */
/*!
 * @brief   get cmdline address
 * @param   none
 * @retval  g_term_cmdline
 * @note    none
 */
kchar_t *term_cmdline_get(void)
{
    return (kchar_t *)&g_term_cmdline[0];
}

/*!
 * @brief   delete repeat char
 * @param   sprt_priv, *offset (size of cmdline)
 * @retval  none
 * @note    none
 */
static void term_kbd_repeat(struct term_kbd_priv *sprt_priv, kuint32_t *offset)
{   
    if ((sprt_priv->key >= CHAR_ASC_SPACE) || 
        (sprt_priv->key < CHAR_ASC_DEL))
    {
        kbyte_t key = (kbyte_t)sprt_priv->key;
        kuint32_t cur_idx;
        kubyte_t *msg;

        cur_idx = *offset;
        msg = sprt_priv->msg;
        if ((*offset) && (msg[cur_idx - 1] == key))
            return;

        msg[cur_idx] = key;
        (*offset)++;
    }
}

/*!
 * @brief   Ctrl + C
 * @param   sprt_priv, *offset (size of cmdline)
 * @retval  none
 * @note    none
 */
static void term_kbd_pause(struct term_kbd_priv *sprt_priv, kuint32_t *offset)
{
    kubyte_t *msg = sprt_priv->msg;

    /*!< echo is the first task */
    term_cmd_print_login();

    *offset = 0;
    *(msg + *offset) = '\0';
}

/*!
 * @brief   \n
 * @param   sprt_priv, *offset (size of cmdline)
 * @retval  none
 * @note    none
 */
static void term_kbd_enter(struct term_kbd_priv *sprt_priv, kuint32_t *offset)
{
    kuint32_t cur_idx = *offset;
    kubyte_t *msg = sprt_priv->msg;

    *(msg + cur_idx) = '\0';
    if (*offset)
    {
        if (*(msg + cur_idx - 1) == CHAR_ASC_SPACE)
            *(msg + cur_idx - 1) = '\0';

        /*!< call function */
        term_cmdline_distribute((const kchar_t *)msg);
    }

    term_cmd_print_login();
    *offset = 0;
}

/*!
 * @brief   Tab
 * @param   sprt_priv, *offset (size of cmdline)
 * @retval  none
 * @note    none
 */
static void term_kbd_tab(struct term_kbd_priv *sprt_priv, kuint32_t *offset)
{
    kubyte_t *msg = sprt_priv->msg;

    *offset = 0;
    *(msg + *offset) = '\0';
}

/*!< super key */
static struct term_kbd_handle sgrt_term_kbd_handles[] =
{
    { CHAR_ASC_SPACE, term_kbd_repeat },

    { CHAR_ASC_CR, term_kbd_enter },                      /*!< enter */
    { CHAR_ASC_ETX, term_kbd_pause },                     /*!< ctrl + c */
    { CHAR_ASC_BS, mrt_nullptr },                         /*!< backspace (ctrl + b) */
    { CHAR_ASC_DEL, mrt_nullptr },                        /*!< delete */
    { CHAR_ASC_HT, term_kbd_tab },                        /*!< tab */
};

/*!
 * @brief   send char that received just now
 * @param   msg
 * @retval  none
 * @note    none
 */
static void term_echo(kint32_t msg)
{
    switch (msg)
    {
        case CHAR_ASC_SPACE ... CHAR_ASC_TILDE:
            io_putc((const kbyte_t)msg);
            break;

        case CHAR_ASC_CR:
        case CHAR_ASC_ETX:
            io_putc(CHAR_ASC_CR);
            break;

        default: break;
    }
}

/*!
 * @brief   save input char to cmdline[]
 * @param   msg, len
 * @retval  none
 * @note    none
 */
static void term_cmdline(kint32_t msg, kuint8_t len)
{
    struct term_kbd_handle *sprt_kbdh;
    kuint32_t num, idx;

    /*!< exclude ' ' */
    if ((msg > CHAR_ASC_SPACE) && 
        (msg < CHAR_ASC_DEL))
    {
        g_term_cmdline[g_term_inc] = (kubyte_t)msg;
        g_term_inc++;

        return;
    }

    sprt_kbdh = &sgrt_term_kbd_handles[0];
    num = ARRAY_SIZE(sgrt_term_kbd_handles);

    for (idx = 0; idx < num; idx++)
    {
        if ((msg == sprt_kbdh[idx].key) &&
            (sprt_kbdh[idx].do_cmd))
        {
            struct term_kbd_priv sgrt_priv;

            sgrt_priv.msg = &g_term_cmdline[0];
            sgrt_priv.key = msg;
            sgrt_priv.len = len;

            sprt_kbdh[idx].do_cmd(&sgrt_priv, &g_term_inc);
            return;
        }
    }
}

/*!< Terminal command defines */
static const term_cmd_fn_t g_term_cmd_fn[] =
{
    term_cmd_add_help,

    term_cmd_add_info,
    term_cmd_add_ttc,
    term_cmd_add_user,
    term_cmd_add_kill,

    mrt_nullptr,
};

/*!
 * @brief   call per cmd initialization
 * @param   term_cmd_fn[]
 * @retval  none
 * @note    none
 */
static void term_new_command(const term_cmd_fn_t term_cmd_fn[])
{
    const term_cmd_fn_t *fn;

    for (fn = term_cmd_fn; (*fn); fn++)
        (*fn)();
}

/*!< --------------------------------------------------------------------- */
/*!
 * @brief   term task main
 * @param   args
 * @retval  args
 * @note    none
 */
static void *term_entry(void *args)
{
    struct mailbox *sprt_mb = &sgrt_term_task_mailbox;
    kint32_t msg;
    kssize_t length;

    term_cmd_login_init(CONFIG_DEFAULT_LOGIN, CONFIG_DEFAULT_HOST);

    printk("\n");
    printk("Press Enter and use HeavenFox now\n");
    printk("\n");
    printk("%s", g_term_cmdline);

    mailbox_init(sprt_mb, mrt_current->tid, "term-task-mailbox");

    /*!< system command */
    term_new_command(g_term_cmd_fn);

    for (;;)
    {
        msg = 0;
        length = io_getstr((kubyte_t *)&msg, sizeof(msg));
        if (length <= 0)
            continue;

        term_echo(msg);
        term_cmdline(msg, (kuint8_t)length);
    }

    return args;
}

/*!
 * @brief   term task create
 * @param   none
 * @retval  errno
 * @note    none
 */
kint32_t term_init(void)
{
    struct thread_attr *sprt_attr = &sgrt_term_task_attr;

    sprt_attr->detachstate = THREAD_CREATE_JOINABLE;
    sprt_attr->inheritsched	= THREAD_INHERIT_SCHED;
    sprt_attr->schedpolicy = THREAD_SCHED_FIFO;

    /*!< thread stack */
    thread_set_stack(sprt_attr, mrt_nullptr, g_term_task_stack, sizeof(g_term_task_stack));
    /*!< lowest priority */
    thread_set_priority(sprt_attr, THREAD_PROTY_TERM);
    /*!< default time slice */
    thread_set_time_slice(sprt_attr, THREAD_TIME_DEFUALT);

    /*!< register thread */
    g_term_task_tid = kernel_thread_create(-1, sprt_attr, term_entry, mrt_nullptr);
    if (g_term_task_tid >= 0)
    {
        thread_set_name(g_term_task_tid, "term_entry");
        return ER_NORMAL;
    }

    return -ER_FAILD;
}
