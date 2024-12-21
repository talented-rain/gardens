/*
 * Kernel Thread MailBox Defines
 *
 * File Name:   mailbox.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.05.25
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __KERNEL_MAILBOX_H_
#define __KERNEL_MAILBOX_H_

/*!< The includes */
#include <kernel/kernel.h>
#include <kernel/thread.h>
#include <kernel/mutex.h>

/*!< The defines */
enum __ERT_KERNEL_MAIL_TYPE
{
    NR_MAIL_TYPE_SYN = 0,
    NR_MAIL_TYPE_KEY,
    NR_MAIL_TYPE_ABS,
    NR_MAIL_TYPE_REL,
    NR_MAIL_TYPE_SERIAL,
};

struct mail_msg
{
    kuint32_t type;
    kuint32_t code;

    kuint8_t *buffer;
    kuint32_t size;
};

enum __ERT_KERNEL_MAIL_STATUS
{
    NR_MAIL_NONE = 0,
    NR_MAIL_ALREADY_READ,
    NR_MAIL_DENIED,
    NR_MAIL_DISCARD,
    NR_MAIL_ERROR,
};

struct mail
{
    const kchar_t *src_name;

    struct mail_msg *sprt_msg;
    kuint32_t num_msgs;

    kuint32_t status;
    struct list_head sgrt_link;

    struct mutex_lock sgrt_lock;
};

#define MAILBOX_NAME_LEN                        (32)

struct mailbox
{
    kchar_t name[MAILBOX_NAME_LEN];
    tid_t tid;

    kuint32_t num_mails;
    struct list_head sgrt_mail;

    struct list_head sgrt_link;
    struct mutex_lock sgrt_lock;
};

/*!< The functions */
TARGET_EXT struct mailbox *mailbox_find(const kchar_t *name);
TARGET_EXT void mailbox_insert(struct mailbox *sprt_mb);

TARGET_EXT kint32_t mailbox_init(struct mailbox *sprt_mb, tid_t tid, const kchar_t *name);
TARGET_EXT void mailbox_deinit(struct mailbox *sprt_mb);
TARGET_EXT struct mailbox *mailbox_create(tid_t tid, const kchar_t *name);
TARGET_EXT void mailbox_destroy(struct mailbox *sprt_mb);

TARGET_EXT void mail_init(struct mailbox *sprt_mb, struct mail *sprt_mail);
TARGET_EXT struct mail *mail_create(struct mailbox *sprt_mb);
TARGET_EXT void mail_destroy(struct mailbox *sprt_mb, struct mail *sprt_mail);
TARGET_EXT kint32_t mail_send(const kchar_t *mb_name, struct mail *sprt_mail);
TARGET_EXT struct mail *mail_recv(struct mailbox *sprt_mb, kutime_t timeout);
TARGET_EXT void mail_recv_finish(struct mail *sprt_mail);

#endif /* __KERNEL_MAILBOX_H_ */
