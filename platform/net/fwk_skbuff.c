/*
 * NetWork Interface
 *
 * File Name:   fwk_lwip.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.11.23
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/net/fwk_if.h>
#include <platform/net/fwk_netdev.h>
#include <platform/net/fwk_skbuff.h>

/*!< The defines */

/*!< The globals */


/*!< API functions */
struct fwk_sk_buff *fwk_alloc_skb(kuint32_t data_size, nrt_gfp_t flags)
{
    struct fwk_sk_buff *sprt_skb;
    kuint8_t *data;
    kusize_t skb_size;

    skb_size  = mrt_align(sizeof(*sprt_skb), sizeof(kutype_t));
    data_size = mrt_align(data_size, sizeof(kutype_t));
    sprt_skb  = kmalloc(skb_size + data_size, GFP_GET_FLAG(flags) | GFP_SOCK);
    if (!isValid(sprt_skb))
        return ERR_PTR(-ER_NOMEM);

    data = (kuint8_t *)sprt_skb + skb_size;

    memset(sprt_skb, 0, mrt_offsetof(struct fwk_sk_buff, tail));
    sprt_skb->truesize = data_size;
    sprt_skb->head = sprt_skb->data = sprt_skb->tail = data;
    sprt_skb->end = sprt_skb->head + data_size;

    sprt_skb->mac_header = (typeof(sprt_skb->mac_header))(~0U);
    sprt_skb->network_header = (typeof(sprt_skb->network_header))(~0U);
    sprt_skb->transport_header = (typeof(sprt_skb->transport_header))(~0U);
    
    ATOMIC_SET(&sprt_skb->users, 1);
    fwk_skb_list_init((struct fwk_sk_buff_head *)sprt_skb);

    return sprt_skb;
}

void kfree_skb(struct fwk_sk_buff *sprt_skb)
{
    if (!sprt_skb)
        return;
    if (ATOMIC_READ(&sprt_skb->users) > 1)
        return;

    kfree(sprt_skb);
}

kint32_t fwk_skb_enqueue(struct fwk_sk_buff_head *sprt_head, struct fwk_sk_buff *sprt_skb)
{
    return fwk_skb_add_tail(sprt_head, sprt_skb);
}

struct fwk_sk_buff *fwk_skb_dequeue(struct fwk_sk_buff_head *sprt_head)
{
    struct fwk_sk_buff *sprt_skb;

    sprt_skb = mrt_skuff_next_entry(sprt_head);
    if (sprt_skb)
        fwk_skb_unlink(sprt_head, sprt_skb);

    return sprt_skb;
}

/*!< end of file */
