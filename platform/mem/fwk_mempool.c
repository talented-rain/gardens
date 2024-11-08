/*
 * Memory Allocate Management
 *
 * File Name:   mem_kalloc.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.10.02
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <boot/boot_text.h>
#include <platform/fwk_mempool.h>
#include <kernel/sched.h>
#include <kernel/wait.h>
#include <kernel/spinlock.h>

/*!< The defines */
struct fwk_mempool
{
    const kchar_t *name;
    kuint32_t mask;
    struct mem_info *sprt_info;

    struct wait_queue_head sgrt_wqh;
    struct spin_lock sgrt_lock;
};

#define FWK_MEMPOOL_KERNEL              0
#define FWK_MEMPOOL_FB_DRAM             1
#define FWK_MEMPOOL_TYPE_MAX            2

/*!< The globals */
static struct mem_info sgrt_kernel_mem_info[FWK_MEMPOOL_TYPE_MAX] = {};

static struct fwk_mempool sgrt_kernel_mempool[FWK_MEMPOOL_TYPE_MAX] =
{
    {
        .name = "kernel heap",
        .mask = NR_KMEM_NORMAL,
        .sprt_info = &sgrt_kernel_mem_info[FWK_MEMPOOL_KERNEL],
    },
    {
        .name = "framebuffer",
        .mask = NR_KMEM_FBUFFER,
        .sprt_info = &sgrt_kernel_mem_info[FWK_MEMPOOL_FB_DRAM],
    },
};

/*!< API function */
/*!
 * @brief   fwk_mempool_initial
 * @param   none
 * @retval  none
 * @note    kernel memory block initial
 */
kbool_t fwk_mempool_initial(void)
{
    struct fwk_mempool *sprt_pool;
    struct mem_info *sprt_info;
    kint32_t retval;

    /*!< ------------------------------------------------------------ */
    sprt_pool = &sgrt_kernel_mempool[FWK_MEMPOOL_KERNEL];
    sprt_info = sprt_pool->sprt_info;
    retval = memory_simple_block_create(sprt_info, MEMORY_POOL_BASE, MEMORY_POOL_SIZE);
    if (retval)
        return false;
    
    init_waitqueue_head(&sprt_pool->sgrt_wqh);
    spin_lock_init(&sprt_pool->sgrt_lock);

    /*!< ------------------------------------------------------------ */
    sprt_pool = &sgrt_kernel_mempool[FWK_MEMPOOL_FB_DRAM];
    sprt_info = sprt_pool->sprt_info;
    memory_simple_block_create(sprt_info, FBUFFER_DRAM_BASE, FBUFFER_DRAM_SIZE);
    init_waitqueue_head(&sprt_pool->sgrt_wqh);
    spin_lock_init(&sprt_pool->sgrt_lock);

    return true;
}

/*!
 * @brief   memory_block_self_defines
 * @param   none
 * @retval  none
 * @note    memory block initial
 */
kbool_t memory_block_self_defines(kuaddr_t base, kusize_t size)
{
    struct fwk_mempool *sprt_pool = &sgrt_kernel_mempool[FWK_MEMPOOL_KERNEL];
    struct mem_info *sprt_info = sprt_pool->sprt_info;

    if (isValid(sprt_info->sprt_mem))
        return false;

    memory_simple_block_create(sprt_info, base, size);

    return true;
}

/*!
 * @brief   memory_block_self_destroy
 * @param   none
 * @retval  none
 * @note    memory block destroy
 */
void memory_block_self_destroy(void)
{
    struct fwk_mempool *sprt_pool = &sgrt_kernel_mempool[FWK_MEMPOOL_KERNEL];
    struct mem_info *sprt_info = sprt_pool->sprt_info;

    memory_simple_block_destroy(sprt_info);
    init_waitqueue_head(&sprt_pool->sgrt_wqh);
    spin_lock_init(&sprt_pool->sgrt_lock);
}

/*!
 * @brief   kmget_size
 * @param   flags
 * @retval  none
 * @note    read memory total lenth
 */
__weak kssize_t kmget_size(ert_fwk_mempool_t flags)
{
    struct fwk_mempool *sprt_pool = mrt_nullptr;
    kuint32_t index;

    for (index = 0; index < FWK_MEMPOOL_TYPE_MAX; index++)
    {
        if (flags & sgrt_kernel_mempool[index].mask)
        {
            if (!sprt_pool)
                sprt_pool = &sgrt_kernel_mempool[index];
            else
                return -ER_UNVALID;
        }
    }

    if (!sprt_pool)
        return -ER_NOTFOUND;

    return sprt_pool->sprt_info->lenth;
}

/*!
 * @brief   kmalloc
 * @param   __size
 * @retval  none
 * @note    kernel memory pool allocate
 */
__weak void *kmalloc(size_t __size, ert_fwk_mempool_t flags)
{
    struct fwk_mempool *sprt_pool = mrt_nullptr;
    struct mem_info *sprt_info;
    void *p = mrt_nullptr;
    kuint32_t index;

    for (index = 0; index < FWK_MEMPOOL_TYPE_MAX; index++)
    {
        if (flags & sgrt_kernel_mempool[index].mask)
        {
            if (!sprt_pool)
                sprt_pool = &sgrt_kernel_mempool[index];
            else
                return p;
        }
    }

    if (!sprt_pool)
        return p;
    
    if (flags & NR_KMEM_WAIT)
        wait_event(&sprt_pool->sgrt_wqh, !spin_is_locked(&sprt_pool->sgrt_lock));

    spin_lock_irqsave(&sprt_pool->sgrt_lock);

    sprt_info = sprt_pool->sprt_info;
    p = alloc_spare_simple_memory(sprt_info->sprt_mem, __size);
    if (!isValid(p))
    {
        p = mrt_nullptr;
        goto END;
    }

    if (flags & NR_KMEM_ZERO)
        memory_reset(p, __size);

END:
    spin_unlock_irqrestore(&sprt_pool->sgrt_lock);

    return p;
}

/*!
 * @brief   kcalloc
 * @param   __size, __n
 * @retval  none
 * @note    kernel memory pool allocate (array)
 */
__weak void *kcalloc(size_t __size, size_t __n, ert_fwk_mempool_t flags)
{
    return kmalloc(__size * __n, flags);
}

/*!
 * @brief   kzalloc
 * @param   __size
 * @retval  none
 * @note    kernel memory pool allocate, and reset automatically
 */
__weak void *kzalloc(size_t __size, ert_fwk_mempool_t flags)
{
    return kmalloc(__size, flags | GFP_ZERO);
}

/*!
 * @brief   kfree
 * @param   __ptr
 * @retval  none
 * @note    kernel memory pool free
 */
__weak void kfree(void *__ptr)
{
    struct fwk_mempool *sprt_pool = mrt_nullptr;
    struct mem_info *sprt_info = mrt_nullptr;
    kuint32_t index;

    for (index = 0; index < FWK_MEMPOOL_TYPE_MAX; index++)
    {
        sprt_pool = &sgrt_kernel_mempool[index];
        sprt_info = sprt_pool->sprt_info;

        if ((__ptr >= (void *)sprt_info->base) &&
            (__ptr <  (void *)(sprt_info->base + sprt_info->lenth)))
            break;
    }

    /*!< not found: out of pool */
    if (index == FWK_MEMPOOL_TYPE_MAX)
        return;

    spin_lock_irqsave(&sprt_pool->sgrt_lock);
    free_employ_simple_memory(sprt_info->sprt_mem, __ptr);
    spin_unlock_irqrestore(&sprt_pool->sgrt_lock);
}

/* end of file */
