/*
 * Kernel MailBox Interface
 *
 * File Name:   mailbox.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.05.25
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The globals */
#include <common/basic_types.h>
#include <common/error_types.h>
#include <common/generic.h>
#include <common/io_stream.h>
#include <kernel/kernel.h>
#include <kernel/sched.h>
#include <kernel/thread.h>
#include <kernel/sleep.h>
#include <kernel/mutex.h>
#include <kernel/mailbox.h>

/*!< The defines */

/*!< The globals */
static DECLARE_LIST_HEAD(sgrt_kernel_mailboxs);

/*!< API functions */
/*!
 * @brief   find mailbox which named "name"
 * @param   name
 * @retval  sprt_mb
 * @note    none
 */
struct mailbox *mailbox_find(const kchar_t *name)
{
    struct mailbox *sprt_mb;

    if (!name || !(*name))
        return mrt_nullptr;

    foreach_list_next_entry(sprt_mb, &sgrt_kernel_mailboxs, sgrt_link)
    {
        if (!strcmp(sprt_mb->name, name))
            return sprt_mb;
    }

    return mrt_nullptr;
}

/*!
 * @brief   add new mailbox to global list
 * @param   sprt_mb
 * @retval  none
 * @note    none
 */
void mailbox_insert(struct mailbox *sprt_mb)
{
    struct mailbox *sprt_box;
    struct list_head *sprt_last = mrt_nullptr;

    if (mrt_list_head_empty(&sgrt_kernel_mailboxs))
        goto END;

    foreach_list_next_entry(sprt_box, &sgrt_kernel_mailboxs, sgrt_link)
    {
        if (sprt_box->tid > sprt_mb->tid)
            break;

        sprt_last = &sprt_box->sgrt_link;
    }

    if (!sprt_last)
        goto END;
    else
        list_head_add_tail(sprt_last, &sprt_mb->sgrt_link);

    return;

END:
    list_head_add_tail(&sgrt_kernel_mailboxs, &sprt_mb->sgrt_link);
}

/*!
 * @brief   initial mailbox
 * @param   sprt_mb, tid, name
 * @retval  none
 * @note    none
 */
kint32_t mailbox_init(struct mailbox *sprt_mb, real_thread_t tid, const kchar_t *name)
{
    struct mailbox *sprt_box;
    kchar_t label[MAILBOX_NAME_LEN] = {};

    if (name && (*name != '\0'))
        kstrlcpy(label, name, MAILBOX_NAME_LEN);
    else
        sprintk(label, "mailbox-tid-%d", tid);

    /*!< if name is registered */
    sprt_box = mailbox_find(label);
    if (sprt_box)
        return -ER_EXISTED;

    sprt_mb->num_mails = 0;
    sprt_mb->tid = tid;
    kstrlcpy(sprt_mb->name, label, MAILBOX_NAME_LEN);
    init_list_head(&sprt_mb->sgrt_mail);
    mutex_init(&sprt_mb->sgrt_lock);
    
    mailbox_insert(sprt_mb);

    return ER_NORMAL;
}

/*!
 * @brief   delete mailbox
 * @param   sprt_mb
 * @retval  none
 * @note    none
 */
void mailbox_deinit(struct mailbox *sprt_mb)
{
    list_head_del(&sprt_mb->sgrt_link);
    mutex_init(&sprt_mb->sgrt_lock);
}

/*!
 * @brief   create mailbox
 * @param   tid, name
 * @retval  mailbox created
 * @note    none
 */
struct mailbox *mailbox_create(real_thread_t tid, const kchar_t *name)
{
    struct mailbox *sprt_mb;

    sprt_mb = kmalloc(sizeof(*sprt_mb), GFP_KERNEL);
    if (!isValid(sprt_mb))
        return sprt_mb;

    if (mailbox_init(sprt_mb, tid, name))
    {
        kfree(sprt_mb);
        return ERR_PTR(-ER_FAILD);
    }

    return sprt_mb;
}

/*!
 * @brief   destroy mailbox
 * @param   sprt_mb
 * @retval  none
 * @note    none
 */
void mailbox_destroy(struct mailbox *sprt_mb)
{
    if (!sprt_mb)
        return;

    mailbox_deinit(sprt_mb);
    kfree(sprt_mb);
}

/*!< ------------------------------------------------------------- */
/*!
 * @brief   create mail
 * @param   sprt_mb
 * @retval  mail created
 * @note    none
 */
struct mail *mail_create(struct mailbox *sprt_mb)
{
    struct mail *sprt_mail;

    sprt_mail = kzalloc(sizeof(*sprt_mail), GFP_KERNEL);
    if (!isValid(sprt_mail))
        return sprt_mail;

    sprt_mail->src_name = sprt_mb->name;
    mutex_init(&sprt_mail->sgrt_lock);

    return sprt_mail;
}

/*!
 * @brief   destroy mail
 * @param   sprt_mb, sprt_mail
 * @retval  none
 * @note    none
 */
void mail_destroy(struct mailbox *sprt_mb, struct mail *sprt_mail)
{
    if (!sprt_mail)
        return;

    mutex_lock(&sprt_mail->sgrt_lock);
    list_head_del(&sprt_mail->sgrt_link);

    if (sprt_mb && !strcmp(sprt_mb->name, sprt_mail->src_name))
        kfree(sprt_mail);
    else
        mutex_unlock(&sprt_mail->sgrt_lock);
}

/*!
 * @brief   send mail
 * @param   mb_name, sprt_mail
 * @retval  errno
 * @note    none
 */
kint32_t mail_send(const kchar_t *mb_name, struct mail *sprt_mail)
{
    struct mailbox *sprt_mb;

    if (mrt_unlikely(!sprt_mail) || mrt_unlikely(!sprt_mail->sprt_msg))
        return -ER_NOMEM;

    sprt_mb = mailbox_find(mb_name);
    if (!isValid(sprt_mb))
        return PTR_ERR(sprt_mb);

    /*!< if the last mail with the same address is being read, you should wait a while before sending new */
    mutex_lock(&sprt_mail->sgrt_lock);

    mutex_lock(&sprt_mb->sgrt_lock);
    list_head_add_tail(&sprt_mb->sgrt_mail, &sprt_mail->sgrt_link);
    sprt_mb->num_mails++;
    mutex_unlock(&sprt_mb->sgrt_lock);

    sprt_mail->status = NR_MAIL_NONE;
    mutex_unlock(&sprt_mail->sgrt_lock);

    return ER_NORMAL;
}

/*!
 * @brief   recieve mail
 * @param   sprt_mb, sprt_mail, timeout
 * @retval  errno
 * @note    none
 */
kint32_t mail_recv(struct mailbox *sprt_mb, struct mail *sprt_mail, kutime_t timeout)
{
    struct mail *sprt_recv;
    struct mail_msg *sprt_msg;
    kuint8_t *buffer;
    kuint32_t msg_idx;
    kint32_t retval;

    if (!sprt_mb || !sprt_mail)
        return -ER_NOMEM;

    while (mrt_list_head_empty(&sprt_mb->sgrt_mail))
    {
        if (!timeout)
            return -ER_EMPTY;

        schedule_delay_ms(timeout);
    }

    /*!< get each mail */
    mutex_lock(&sprt_mb->sgrt_lock);
    sprt_recv = mrt_list_first_entry(&sprt_mb->sgrt_mail, typeof(*sprt_recv), sgrt_link);
    mutex_unlock(&sprt_mb->sgrt_lock);

    mutex_lock(&sprt_recv->sgrt_lock);

    /*!< make sure that sprt_recv is still valid */
    if (mrt_list_head_empty(&sprt_recv->sgrt_link))
        return -ER_EMPTY;

    /*!< clear the last status */
    mail_recv_finish(sprt_mail);
    sprt_mail->status = NR_MAIL_NONE;

    if ((!sprt_recv->num_msgs) || (!sprt_recv->sprt_msg))
    {
        retval = -ER_EMPTY;
        sprt_recv->status = NR_MAIL_ERROR;
        goto END;
    }

    sprt_msg = kcalloc(sizeof(*sprt_msg), sprt_recv->num_msgs, GFP_KERNEL);
    if (!isValid(sprt_msg))
    {
        retval = PTR_ERR(sprt_msg);
        goto END;
    }

    sprt_mail->num_msgs = sprt_recv->num_msgs;
    sprt_mail->sprt_msg = sprt_msg;

    for (msg_idx = 0; msg_idx < sprt_mail->num_msgs; msg_idx++)
    {
        sprt_msg[msg_idx].type = sprt_recv->sprt_msg[msg_idx].type;
        sprt_msg[msg_idx].code = sprt_recv->sprt_msg[msg_idx].code;
        sprt_msg[msg_idx].size = sprt_recv->sprt_msg[msg_idx].size;

        buffer = kcalloc(sizeof(*buffer), sprt_msg[msg_idx].size, GFP_KERNEL);
        if (!isValid(buffer))
        {
            buffer = mrt_nullptr;
            sprt_msg[msg_idx].size = 0;
        }
        else
            memcpy(buffer, sprt_recv->sprt_msg[msg_idx].buffer, sizeof(*buffer) * sprt_msg[msg_idx].size);
        
        sprt_msg[msg_idx].buffer = buffer;           
    }

    retval = sprt_mail->num_msgs;
    sprt_recv->status = NR_MAIL_ALREADY_READ;

END:
    sprt_mb->num_mails--;
    mutex_unlock(&sprt_recv->sgrt_lock);
    mail_destroy(sprt_mb, sprt_recv);

    return retval;
}

/*!
 * @brief   deal with the aftermath
 * @param   sprt_mail
 * @retval  none
 * @note    none
 */
void mail_recv_finish(struct mail *sprt_mail)
{
    kuint32_t idx;

    if (sprt_mail->sprt_msg)
    {
        for (idx = 0; idx < sprt_mail->num_msgs; idx++)
        {
            if (sprt_mail->sprt_msg[idx].buffer)
                kfree(sprt_mail->sprt_msg[idx].buffer);
        }

        kfree(sprt_mail->sprt_msg);
        sprt_mail->sprt_msg = mrt_nullptr;
        sprt_mail->num_msgs = 0;
    }

    sprt_mail->src_name = mrt_nullptr;
    sprt_mail->status = NR_MAIL_ALREADY_READ;
    mutex_init(&sprt_mail->sgrt_lock);
}

/*!< end of file */
