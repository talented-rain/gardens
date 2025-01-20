/*
 * Memory Block Management
 *
 * File Name:   mem_block.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.01.19
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <common/error_types.h>
#include <common/mem_manage.h>

/*!< The defines */

/*!< The functions */
static struct mem_block *check_employ_memory(void *ptr_head, void *ptr_mem);
static void *alloc_spare_memory(struct mem_info *sprt_info, kusize_t size);
static void free_employ_memory(struct mem_info *sprt_info, void *ptr_mem);

/*!< API function */
/*!
 * @brief   select a correct hash list
 * @param   sprt_info, total_size
 * @retval  mem_hash
 * @note    determine a hash list to add a new block
 */
static struct mem_hash *memory_block_get_hash(struct mem_info *sprt_info, kusize_t total_size)
{
    kint32_t index = NR_MEM_HighBytes;

    if (total_size >= NR_MEM_HighLimit)
        return &sprt_info->sgrt_hash[NR_MEM_HighBytes];

    if (total_size < NR_MEM_LowerLimit)
        return &sprt_info->sgrt_hash[NR_MEM_LowerBytes];

    while (!(total_size & (1U << index)))
        index--;

    return &sprt_info->sgrt_hash[index];
}

/*!
 * @brief   add the new block to hash list
 * @param   sprt_info, sprt_hash, sprt_block
 * @retval  none
 * @note    if sprt_hash is valid, add to it directly
 */
static void memory_block_attach(struct mem_info *sprt_info, struct mem_hash *sprt_hash, struct mem_block *sprt_block)
{
    struct mem_block *sprt_per;

    if (!sprt_block->remain)
        return;

    if (!sprt_hash)
        sprt_hash = memory_block_get_hash(sprt_info, sprt_block->remain);
    
    init_list_head(&sprt_block->sgrt_link);

    /*!< case 1: list is empty */
    if (mrt_list_head_empty(&sprt_hash->sgrt_list))
    {
        list_head_add_head(&sprt_hash->sgrt_list, &sprt_block->sgrt_link);
        return;
    }

    /*!< case 2: head < adress of block < tail */
    foreach_list_next_entry(sprt_per, &sprt_hash->sgrt_list, sgrt_link)
    {
        /*!< low address in front for easy access next time */
        if (sprt_block < sprt_per)
        {
            list_head_add_tail(&sprt_per->sgrt_link, &sprt_block->sgrt_link);
            return;
        }
    }

    /*!< case 3: block's address is the largest */
    list_head_add_tail(&sprt_hash->sgrt_list, &sprt_block->sgrt_link);
}

/*!
 * @brief   detached from hash list
 * @param   sprt_block
 * @retval  none
 * @note    none
 */
static void memory_block_detach(struct mem_block *sprt_block)
{
    list_head_del(&sprt_block->sgrt_link);
}

/*!
 * @brief   memory_block_create
 * @param   sprt_info, mem_addr, size
 * @retval  none
 * @note    build memory block
 */
kint32_t memory_block_create(struct mem_info *sprt_info, kuaddr_t mem_addr, kusize_t size)
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
    for (kint32_t index = 0; index < NR_MEM_NUM; index++)
    {
        struct mem_hash *sprt_hash;

        sprt_hash = &sprt_info->sgrt_hash[index];
        init_list_head(&sprt_hash->sgrt_list);
    }

    sprt_info->alloc = alloc_spare_memory;
    sprt_info->free = free_employ_memory;

    memory_block_attach(sprt_info, mrt_nullptr, sprt_block);
    return ER_NORMAL;
}

/*!
 * @brief   memory_block_destroy
 * @param   sprt_info
 * @retval  none
 * @note    destroy memory block which is created
 */
void memory_block_destroy(struct mem_info *sprt_info)
{
    if (!isValid(sprt_info))
        return;

    /*!< clear all memory blocks */
    kmemzero((void *)sprt_info->base, sprt_info->lenth);
    kmemzero((void *)sprt_info, sizeof(struct mem_info));
}

/*!
 * @brief   find a hash list with enough space
 * @param   sprt_info, index(index of hash list), total_size
 * @retval  none
 * @note    none
 */
static struct mem_block *__memory_block_get_avaliable(struct mem_info *sprt_info, kint32_t index, kusize_t total_size)
{
    struct mem_block *sprt_block;
    struct mem_hash *sprt_hash = &sprt_info->sgrt_hash[index];

    foreach_list_next_entry(sprt_block, &sprt_hash->sgrt_list, sgrt_link) 
    {
        if (sprt_block->remain >= total_size)
            return sprt_block;
    }

    return mrt_nullptr;
}

/*!
 * @brief   find a hash list with enough space
 * @param   sprt_info, index(index of hash list), total_size
 * @retval  mem_block
 * @note    if the preferred hash list space is insufficient, select the next hash list
 */
static struct mem_block *memory_block_get_avaliable(struct mem_info *sprt_info, kusize_t total_size)
{
    struct mem_block *sprt_block;
    kint32_t index = NR_MEM_HighBytes;

    if (total_size >= NR_MEM_HighLimit)
        return __memory_block_get_avaliable(sprt_info, NR_MEM_HighBytes, total_size);

    if (total_size < NR_MEM_LowerLimit)
    {
        sprt_block = __memory_block_get_avaliable(sprt_info, NR_MEM_LowerBytes, total_size);
        if (sprt_block)
            return sprt_block;

        total_size = NR_MEM_LowerLimit;
    }

    /*!< for example: total_size = 32, index will be 5 */
    while (!(total_size & (1U << index)))
        index--;

    while (index < NR_MEM_NUM)
    {
        sprt_block = __memory_block_get_avaliable(sprt_info, index, total_size);
        if (sprt_block)
            return sprt_block;

        index++;
    }

    return mrt_nullptr;
}

/*!
 * @brief   alloc_spare_memory
 * @param   ptr_head, size
 * @retval  avaliable memory block pointer
 * @note    allocate spare memory space
 */
static void *alloc_spare_memory(struct mem_info *sprt_info, kusize_t size)
{
    struct mem_block *sprt_block;
    struct mem_block *sprt_new;
    kusize_t header_size, lenth, offset;
    void *ptr_mem = mrt_nullptr;

    if (!isValid(sprt_info) ||
        !sprt_info->sprt_mem)
        return mrt_nullptr;

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

    /*!< find a block with enough space left */
    sprt_block = memory_block_get_avaliable(sprt_info, lenth);
    if (!sprt_block)
        return mrt_nullptr;

    memory_block_detach(sprt_block);

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
        memory_block_attach(sprt_info, mrt_nullptr, sprt_new);
    }
    else
    {
    	sprt_block->magic = MEMORY_POOL_MAGIC;

        /*!< Update the lenth of memory that is avaliable */
        sprt_block->remain -= lenth;
    }

    memory_block_attach(sprt_info, mrt_nullptr, sprt_block);
    return ptr_mem;
}

/*!
 * @brief   check_employ_memory
 * @param   ptr_mem
 * @retval  none
 * @note    check if ptr_mem was allocated from ptr_head
 */
static struct mem_block *check_employ_memory(void *ptr_head, void *ptr_mem)
{
    struct mem_block *sprt_block;

    /*!< Point to the head of info */
    sprt_block = (struct mem_block *)((kuint8_t *)ptr_mem - MEM_BLOCK_HEADER_SIZE);
    if ((!isValid(sprt_block)) || (!IS_MEMORYPOOL_VALID(sprt_block)))
        return mrt_nullptr;

#if 1
    __RESERVED(ptr_head);

    if (sprt_block->base != (kuaddr_t)ptr_mem)
        return mrt_nullptr;

    return sprt_block;

#else
    /*!< check if ptr_mem was allocated from ptr_head */
    for (struct mem_block *sprt_start = (struct mem_block *)ptr_head; 
        isValid(sprt_start); sprt_start = sprt_start->sprt_next)
    {
        if (sprt_start->base == (kuaddr_t)ptr_mem)
            return sprt_block;
    }

    return mrt_nullptr;
#endif
}

/*!
 * @brief   free_employ_memory
 * @param   ptr_mem
 * @retval  none
 * @note    free memory block which is employed
 */
static void free_employ_memory(struct mem_info *sprt_info, void *ptr_mem)
{
    struct mem_block *sprt_prev;
    struct mem_block *sprt_next;
    struct mem_block *sprt_block;

    if (!isValid(ptr_mem))
        return;

    /*!< Point to the head of info */
    sprt_block = check_employ_memory(sprt_info->sprt_mem, ptr_mem);
    if (!sprt_block)
        return;

    sprt_block->magic = 0;

    sprt_prev = sprt_block->sprt_prev;
    sprt_next = sprt_block->sprt_next;

    /*!< Update memory space */
    sprt_block->remain = sprt_block->lenth;
    memory_block_detach(sprt_block);

    if (isValid(sprt_prev))
    {
        /*!< Check if the current memory block is idle, if yes, merge into the last neighboring memory block */
        sprt_prev->lenth += sprt_block->lenth;
        sprt_prev->remain += sprt_block->remain;
        sprt_prev->sprt_next = sprt_next;

        sprt_block->sprt_prev = mrt_nullptr;
        sprt_block = sprt_prev;
        memory_block_detach(sprt_block);
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
  
    memory_block_attach(sprt_info, mrt_nullptr, sprt_block);
}

/* end of file */
