/*
 * Queue
 *
 * File Name:   queue.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.11.24
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <common/generic.h>
#include <common/queue.h>

/*!< The defines */

/*!< The globals */
static struct pq_queue sgrt_pq_queue[PQ_QUEUE_NUM];
static kint32_t g_pq_cur_index = 0;

/*!< API function */
/*!
 * @brief   create queue
 * @param   none
 * @retval  pq_queue
 * @note    none
 */
struct pq_queue *pq_queue_create(void)
{
    struct pq_queue *sprt_pq;

    if (g_pq_cur_index >= PQ_QUEUE_NUM)
        return mrt_nullptr;

    sprt_pq = &sgrt_pq_queue[g_pq_cur_index++];
    sprt_pq->head = sprt_pq->tail = sprt_pq->len = 0;

    return sprt_pq;
}

kint32_t pq_queue_put(struct pq_queue *sprt_pq, void *data)
{
    if (sprt_pq->len == PQ_QUEUE_LEN)
        return -ER_FULL;

    sprt_pq->data[sprt_pq->head] = data;
    sprt_pq->head = (sprt_pq->head + 1) % PQ_QUEUE_LEN;
    sprt_pq->len++;

    return ER_NORMAL;
}

void *pq_queue_get(struct pq_queue *sprt_pq)
{
    kint32_t ptail;

    if (!sprt_pq->len)
        return mrt_nullptr;

    ptail = sprt_pq->tail;
    sprt_pq->tail = (sprt_pq->tail + 1) % PQ_QUEUE_LEN;
    sprt_pq->len--;

    return sprt_pq->data[ptail];
}

kint32_t pq_queue_get_size(struct pq_queue *sprt_pq)
{
    return sprt_pq->len;
}

/* end of file */
