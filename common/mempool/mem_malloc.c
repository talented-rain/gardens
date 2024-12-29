/*
 * Memory Allocate Management
 *
 * File Name:   mem_malloc.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.10.02
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <boot/boot_text.h>
#include <common/mem_manage.h>

/*!< The globals */
static struct mem_info sgrt_infoMalloc =
{
    .base	= 0,
    .lenth	= 0,

    .sprt_mem = mrt_nullptr,
};

/*!< API function */
/*!
 * @brief   _sbrk
 * @param   incr
 * @retval  none
 * @note    for "malloc"
 */
__weak kuaddr_t _sbrk(kuint32_t incr)
{
    static kuint8_t *ptr_heapHead = mrt_nullptr;
    kuint8_t *ptr_heapEnd;
    kuint8_t *prev_heap;
    kuaddr_t status;

    /*!< heap end */
    ptr_heapEnd = (kuint8_t *)MEMORY_HEAP_END;

    /*!< heap start */
    if (ptr_heapHead == mrt_nullptr) 
        ptr_heapHead = (kuint8_t *)MEMORY_HEAP_START;
    
    /*!< save memory base address */
    prev_heap = ptr_heapHead;

    if (((ptr_heapHead + incr) <= ptr_heapEnd) && (prev_heap != mrt_nullptr)) 
    {
        ptr_heapHead += incr;
        status = (kuaddr_t)((void *)prev_heap);
    } 
    else
        status = (kuaddr_t) - 1;

    return status;
}

/*!
 * @brief   malloc_block_initial
 * @param   none
 * @retval  none
 * @note    malloc block initial
 */
kbool_t malloc_block_initial(void)
{
    struct mem_info *sprt_info;

    sprt_info = &sgrt_infoMalloc;

    if (isValid(sprt_info->sprt_mem))
        return false;

    memory_simple_block_create(sprt_info, 
                               MEMORY_HEAP_START, 
                               MEMORY_HEAP_END - MEMORY_HEAP_START);

    return true;
}

/*!
 * @brief   malloc_block_self_defines
 * @param   none
 * @retval  none
 * @note    malloc block initial
 */
kbool_t malloc_block_self_defines(kuaddr_t base, kusize_t size)
{
    struct mem_info *sprt_info;

    sprt_info = &sgrt_infoMalloc;

    if (isValid(sprt_info->sprt_mem))
        return false;

    memory_simple_block_create(sprt_info, base, size);

    return true;
}

/*!
 * @brief   malloc_block_destroy
 * @param   none
 * @retval  none
 * @note    malloc block destroy
 */
void malloc_block_destroy(void)
{
    memory_simple_block_destroy(&sgrt_infoMalloc);
}

/*!
 * @brief   malloc
 * @param   __size
 * @retval  none
 * @note    for "malloc"
 */
__weak void *malloc(size_t __size)
{
    return alloc_spare_simple_memory(sgrt_infoMalloc.sprt_mem, __size);
}

/*!
 * @brief   free
 * @param   __ptr
 * @retval  none
 * @note    for "free"
 */
__weak void free(void *__ptr)
{
    free_employ_simple_memory(sgrt_infoMalloc.sprt_mem, __ptr);
}

/*!
 * @brief   memset
 * @param   dst, val, size
 * @retval  none
 * @note    for "memset"
 */
void *memset(void *__s, int __c, size_t __n)
{
    kmemset(__s, (kuint8_t)__c, __n);

    return __s;
}

/*!
 * @brief   memset_ex
 * @param   dst, val, size
 * @retval  none
 * @note    for "memset"
 */
void *memset_ex(void *__s, unsigned int __c, size_t __n)
{
    kmemset_ex(__s, __c, __n);

    return __s;
}

/*!
 * @brief   memcmp
 * @param   __s1, __s2, __n
 * @retval  none
 * @note    for "memcmp"
 */
int memcmp(const void *__s1, const void *__s2, size_t __n)
{
    return kmemcmp(__s1, __s2, __n);
}

/*!
 * @brief   memcpy
 * @param   dst, val, size
 * @retval  none
 * @note    for "memcpy"
 */
void *memcpy(void *__dest, const void *__src, size_t __n)
{
    return kmemcpy(__dest, __src, __n);
}

/* end of file */
