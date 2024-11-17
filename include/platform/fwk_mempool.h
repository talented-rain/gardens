/*
 * Memory Control For Kernel
 *
 * File Name:   fwk_mempool.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.10.02
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_MEMPOOL_H
#define __FWK_MEMPOOL_H

/*!< The includes */
#include <common/generic.h>
#include <common/mem_manage.h>

/*!< The defines */
typedef enum ert_fwk_mempool
{
    NR_KMEM_ZERO = mrt_bit(0),
    NR_KMEM_WAIT = mrt_bit(1),
    NR_KMEM_NOWAIT = 0,

    NR_KMEM_NORMAL = mrt_bit(2),                                /*!< memory for kernel heap */
    NR_KMEM_FBUFFER = mrt_bit(3),                               /*!< memory for framebuffer */
    NR_KMEM_FIXDATA = mrt_bit(4),                               /*!< memory for fixed data */

    NR_KMEM_KERNEL = NR_KMEM_WAIT | NR_KMEM_NORMAL,
    NR_KMEM_ATOMIC = NR_KMEM_NOWAIT | NR_KMEM_NORMAL,
    NR_KMEM_DRAM   = NR_KMEM_WAIT | NR_KMEM_FBUFFER,
    NR_KMEM_FIXED  = NR_KMEM_NOWAIT | NR_KMEM_FIXDATA,

} ert_fwk_mempool_t;

#define GFP_ZERO                                NR_KMEM_ZERO
#define GFP_KERNEL                              NR_KMEM_KERNEL
#define GFP_ATOMIC                              NR_KMEM_ATOMIC
#define GFP_DRAM                                NR_KMEM_DRAM
#define GFP_FIXED                               NR_KMEM_FIXED

/*!< The functions */
TARGET_EXT kbool_t fwk_mempool_initial(void);
TARGET_EXT kssize_t kmget_size(ert_fwk_mempool_t flags);
TARGET_EXT void *kmalloc(size_t __size, ert_fwk_mempool_t flags);
TARGET_EXT void *kcalloc(size_t __size, size_t __n, ert_fwk_mempool_t flags);
TARGET_EXT void *kzalloc(size_t __size, ert_fwk_mempool_t flags);
TARGET_EXT void kfree(void *__ptr);

TARGET_EXT kbool_t memory_block_self_defines(kint32_t flags, kuaddr_t base, kusize_t size);
TARGET_EXT void memory_block_self_destroy(kint32_t flags);

#endif  /* __FWK_MEMPOOL_H */
