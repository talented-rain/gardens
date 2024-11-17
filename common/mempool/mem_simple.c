/*
 * Memory Simply Management
 *
 * File Name:   mem_simple.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.09.25
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <common/error_types.h>
#include <common/mem_manage.h>

/*!< The functions */
static struct mem_block *check_employ_simple_memory(void *ptr_head, void *ptr_mem);

/*!< API function */
/*!
 * @brief   memory_block_create
 * @param   sprt_info, mem_addr, size
 * @retval  none
 * @note    build simple memory block
 */
kint32_t memory_simple_block_create(struct mem_info *sprt_info, kuaddr_t mem_addr, kusize_t size)
{
    struct mem_block *sprt_block;
    kusize_t header_size;

    /*!< header size */
    header_size	= MEM_BLOCK_HEADER_SIZE;

    if ((!isValid(sprt_info)) || (size <= header_size))
        return -ER_UNVALID;

    /*!< if sprt_mem is exsited, it is not allow to create again */
    if (isValid(sprt_info->sprt_mem))
        return -ER_UNVALID;

    /*!< 8 bytes align */
    sprt_info->base	= mrt_num_align8(mem_addr);
    sprt_info->lenth = size - (sprt_info->base - mem_addr);
    
    sprt_block = (struct mem_block *)sprt_info->base;
    sprt_block->base = sprt_info->base + header_size;

    /*!< size = header size + memory size, so we can let sprt_block->lenth equal to total lenth */
    sprt_block->lenth = sprt_info->lenth;
    sprt_block->remain = sprt_info->lenth;
    sprt_block->sprt_prev = mrt_nullptr;
    sprt_block->sprt_next = mrt_nullptr;
    sprt_block->magic = MEMORY_POOL_MAGIC;

    sprt_info->sprt_mem	= sprt_block;

    return ER_NORMAL;
}

/*!
 * @brief   memory_simple_block_destroy
 * @param   sprt_info
 * @retval  none
 * @note    destroy memory block which is created
 */
void memory_simple_block_destroy(struct mem_info *sprt_info)
{
    if (!isValid(sprt_info))
        return;

    /*!< clear all memory blocks */
    memory_reset((void *)sprt_info->base, sprt_info->lenth);
    memory_reset((void *)sprt_info, sizeof(struct mem_info));
}

/*!
 * @brief   alloc_spare_simple_memory
 * @param   ptr_head, size
 * @retval  avaliable memory block pointer
 * @note    allocate spare memory space
 */
void *alloc_spare_simple_memory(void *ptr_head, kusize_t size)
{
    struct mem_block *sprt_start;
    struct mem_block *sprt_block;
    struct mem_block *sprt_new;
    kusize_t header_size, lenth, offset;
    void *ptr_mem = mrt_nullptr;

    if (!isValid(ptr_head))
        return mrt_nullptr;

    sprt_start  = (struct mem_block *)ptr_head;
    header_size	= MEM_BLOCK_HEADER_SIZE;

    /*!< 8 bytes alignment for memory block lenth */
    /*!< Build complete space: lenth + header size, adapted to sprt_block->lenth */
    lenth  = mrt_num_align8(size);
    lenth += header_size;

    /*!< Several methods for dividing memory:
     * 1. After allocating memory, the original memory is not divided, and the original lenth is retained; --->
     *	  it only returns the memory address. When the seconed allocating comes, it will split the remaining memory block, --->
     *    and create a new block (the remaining memory is inclued by the new memory block)
     * 2. Split and seprate the allocated memory inmediately, and two blocks are generated for the split and remaining memory.
     *
     * it takes the first method here.
     */

    for (sprt_block = sprt_start; isValid(sprt_block); sprt_block = sprt_block->sprt_next)
    {
        /*!< Remaining memory is unable to be applied, find next avaliable block */
        if (sprt_block->remain >= lenth)
        {
            /*!< p = base + (lenth - header_size - remain) + header_size = base + lenth - remain */
            offset 	= sprt_block->lenth - sprt_block->remain;
            ptr_mem = (void *)((kuint8_t *)sprt_block->base + offset);

            /*!< Build a new memory block */
            if (sprt_block->lenth != sprt_block->remain)
            {
                /*!< Move to the head of memory block */
                sprt_new = (struct mem_block *)((kuint8_t *)ptr_mem - header_size);
                sprt_new->base = (kuaddr_t)ptr_mem;
                sprt_new->lenth = sprt_block->remain;
                sprt_new->remain = sprt_block->remain - lenth;
                sprt_new->sprt_prev = sprt_block;
                sprt_new->sprt_next = sprt_block->sprt_next;
                sprt_new->magic = MEMORY_POOL_MAGIC;

                sprt_block->lenth = offset;
                sprt_block->remain -= sprt_new->lenth;

                if (sprt_block->sprt_next)
                	sprt_block->sprt_next->sprt_prev = sprt_new;
                
                sprt_block->sprt_next = sprt_new;
            }
            else
            {
                /*!< Update the lenth of memory that is avaliable */
                sprt_block->remain -= lenth;
            }

            break;
        }
    }

    return ptr_mem;
}

/*!
 * @brief   check_employ_simple_memory
 * @param   ptr_mem
 * @retval  none
 * @note    check if ptr_mem was allocated from ptr_head
 */
static struct mem_block *check_employ_simple_memory(void *ptr_head, void *ptr_mem)
{
    struct mem_block *sprt_block, *sprt_start = mrt_nullptr;

    /*!< Point to the head of info */
    sprt_block = (struct mem_block *)((kuint8_t *)ptr_mem - MEM_BLOCK_HEADER_SIZE);
    if ((!isValid(sprt_block)) || (!IS_MEMORYPOOL_VALID(sprt_block)))
        return mrt_nullptr;

    /*!< check if ptr_mem was allocated from ptr_head */
    for (sprt_start = (struct mem_block *)ptr_head; isValid(sprt_start); sprt_start = sprt_start->sprt_next)
    {
        if (sprt_start->base == (kuaddr_t)ptr_mem)
            break;
    }

    if (isValid(sprt_start))
        return sprt_block;

    return mrt_nullptr;
}

/*!
 * @brief   free_employ_simple_memory
 * @param   ptr_mem
 * @retval  none
 * @note    free memory block which is employed
 */
void free_employ_simple_memory(void *ptr_head, void *ptr_mem)
{
    struct mem_block *sprt_prev;
    struct mem_block *sprt_next;
    struct mem_block *sprt_block;

    if (!isValid(ptr_mem))
        return;

    /*!< Point to the head of info */
    sprt_block = check_employ_simple_memory(ptr_head, ptr_mem);
    if (!sprt_block)
        return;

    sprt_prev = sprt_block->sprt_prev;
    sprt_next = sprt_block->sprt_next;

    /*!< Update memory space */
    sprt_block->remain = sprt_block->lenth;

    if (isValid(sprt_prev))
    {
        /*!< Check if the current memory block is idle, if yes, merge into the last neighboring memory block */
        sprt_prev->lenth += sprt_block->lenth;
        sprt_prev->remain += sprt_block->remain;
        sprt_prev->sprt_next = sprt_next;

        sprt_block = sprt_prev;
    }

    if (isValid(sprt_next))
    {
//      /*!< Check if the next neighboring memory block is idle, if yes, the idle memory block should be merged */
//      if (sprt_next->lenth == sprt_next->remain)
//      {
//          sprt_block->sprt_next = sprt_next->sprt_next;
//          sprt_block->lenth += sprt_next->lenth;
//          sprt_block->remain += sprt_next->remain;
//      }

        sprt_next->sprt_prev = sprt_block;
    }
}

/* end of file */
