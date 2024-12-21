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
#include <platform/fwk_mempool.h>

/*!< The defines */

/*!< The globals */

/*!< API function */
/*!
 * @brief   create queue
 * @param   type: NR_PQ_DROP/NR_PQ_RING
 * @param   data_len: length of queue member
 * @retval  pq_queue
 * @note    none
 */
struct pq_queue *pq_queue_create(kint32_t type, kusize_t data_len)
{
    struct pq_queue *sprt_pq;

    sprt_pq = kmalloc(sizeof(*sprt_pq) + data_len * sizeof(struct pq_data), GFP_KERNEL);
    if (!isValid(sprt_pq))
        return mrt_nullptr;

    sprt_pq->type = type;
    sprt_pq->tot_len = data_len;
    sprt_pq->head = sprt_pq->tail = sprt_pq->len = 0;

    return sprt_pq;
}

/*!
 * @brief   destroy queue
 * @param   sprt_pq
 * @retval  none
 * @note    sprt_pqd->release: function used to destroy sprt_pqd
 */
void pq_queue_destroy(struct pq_queue *sprt_pq)
{
    struct pq_data *sprt_pqd;

    if (!sprt_pq->len)
        goto END;

    while (sprt_pq->len--)
    {
        sprt_pqd = sprt_pq->sprt_data[sprt_pq->tail];
        sprt_pq->tail = (sprt_pq->tail + 1) % sprt_pq->tot_len;

        if (sprt_pqd->release)
            sprt_pqd->release(sprt_pqd);
    }

END:
    kfree(sprt_pq);
}

/*!
 * @brief   queue add new member
 * @param   sprt_pq, sprt_data (new)
 * @retval  error code
 * @note    if it is a ring queue and the queue is full, delete the oldest member
 */
kint32_t pq_enqueue(struct pq_queue *sprt_pq, struct pq_data *sprt_data)
{
    if (sprt_pq->len < sprt_pq->tot_len)
        sprt_pq->len++;
    else
    {
        if (sprt_pq->type != NR_PQ_RING)
            return -ER_FULL;
        else
        {
            struct pq_data *sprt_old;

            sprt_old = sprt_pq->sprt_data[sprt_pq->tail];
            sprt_pq->tail = (sprt_pq->tail + 1) % sprt_pq->tot_len;

            if (sprt_old->release)
                sprt_old->release(sprt_old);
        }
    }

    sprt_pq->sprt_data[sprt_pq->head] = sprt_data;
    sprt_pq->head = (sprt_pq->head + 1) % sprt_pq->tot_len;       

    return ER_NORMAL;
}

/*!
 * @brief   get member from queue
 * @param   sprt_pq
 * @retval  member
 * @note    none
 */
void *pq_dequeue(struct pq_queue *sprt_pq)
{
    struct pq_data *sprt_pqd;

    if (!sprt_pq->len)
        return mrt_nullptr;

    sprt_pqd = sprt_pq->sprt_data[sprt_pq->tail];
    sprt_pq->tail = (sprt_pq->tail + 1) % sprt_pq->tot_len;
    sprt_pq->len--;

    return sprt_pqd;
}

/*!
 * @brief   get member from queue after checking
 * @param   sprt_pq
 * @param   limit: the size of buffer (if buffer is too small, forbiden dequeue)
 * @retval  member
 * @note    none
 */
void *pq_dequeue_with_chk(struct pq_queue *sprt_pq, kusize_t limit)
{
    struct pq_data *sprt_pqd;

    if (!sprt_pq->len)
        return mrt_nullptr;

    sprt_pqd = sprt_pq->sprt_data[sprt_pq->tail];
    if (sprt_pqd->dequeue_chk)
    {
        if (!sprt_pqd->dequeue_chk(sprt_pqd, limit))
            return ERR_PTR(-ER_LACK);
    }

    sprt_pq->tail = (sprt_pq->tail + 1) % sprt_pq->tot_len;
    sprt_pq->len--;

    return sprt_pqd;
}

/*!
 * @brief   get current number of members
 * @param   sprt_pq
 * @retval  lenth
 * @note    none
 */
kint32_t pq_queue_get_size(struct pq_queue *sprt_pq)
{
    return sprt_pq->len;
}

/* end of file */
