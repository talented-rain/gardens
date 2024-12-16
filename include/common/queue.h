/*
 * String Function Declare
 *
 * File Name:   queue.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.09.26
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __QUEUE_H
#define __QUEUE_H

/*!< The includes */
#include <common/basic_types.h>
#include <common/error_types.h>
#include <common/generic.h>

/*!< The defines */
enum __ERT_PQ_TYPE
{
    NR_PQ_DROP = 0,
    NR_PQ_RING,
};

struct pq_data
{
    void (*release)(struct pq_data *sprt_pqd);
    kbool_t (*dequeue_chk)(struct pq_data *sprt_pqd, kusize_t limit);
};

typedef struct pq_queue
{
    kint32_t type;

    kint32_t head;
    kint32_t tail;
    kint32_t len;

    kusize_t tot_len;
    struct pq_data *sprt_data[];

} srt_pq_t;

/*!< The functions */
TARGET_EXT struct pq_queue *pq_queue_create(kint32_t type, kusize_t data_len);
TARGET_EXT void pq_queue_destroy(struct pq_queue *sprt_pq);
TARGET_EXT kint32_t pq_enqueue(struct pq_queue *sprt_pq, struct pq_data *sprt_data);
TARGET_EXT void *pq_dequeue(struct pq_queue *sprt_pq);
TARGET_EXT void *pq_dequeue_with_chk(struct pq_queue *sprt_pq, kusize_t limit);
TARGET_EXT kint32_t pq_queue_get_size(struct pq_queue *sprt_pq);

#endif /* __QUEUE_H */
