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
#define PQ_QUEUE_LEN				4096
#define PQ_QUEUE_NUM				2

typedef struct pq_queue
{
    void *data[PQ_QUEUE_LEN];
    kint32_t head;
    kint32_t tail;
    kint32_t len;

} srt_pq_t;

/*!< The functions */
TARGET_EXT struct pq_queue *pq_queue_create(void);
TARGET_EXT kint32_t pq_queue_put(struct pq_queue *sprt_pq, void *data);
TARGET_EXT void *pq_queue_get(struct pq_queue *sprt_pq);
TARGET_EXT kint32_t pq_queue_get_size(struct pq_queue *sprt_pq);

#endif /* __QUEUE_H */
