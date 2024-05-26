/*
 * Memory Control
 *
 * File Name:   mem_manage.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.09.16
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __MEM_MANAGE_H
#define __MEM_MANAGE_H

/*!< The includes */
#include "generic.h"
#include <kernel/kernel.h>

/*!< The defines */
#define MEMORY_POOL_MAGIC                                   (0xdfa69fe3)

/*!< memory block */
typedef struct mem_block
{
    kuint32_t magic;                                        /*!< magic number, it should be set to "MEMORY_POOL_MAGIC" */
	kuaddr_t base;											/*!< current start address */
	kusize_t lenth;										    /*!< current block total lenth(unit: byte), including header of memory info */
	kusize_t remain;										/*!< the lenth of remaining usable memoty(unit: byte) */

	struct mem_block *sprt_prev;						/*!< the first address of last memory block */
	struct mem_block *sprt_next;						/*!< the first address of next memory block */

#define IS_MEMORYPOOL_VALID(this)                           ((this)->magic == MEMORY_POOL_MAGIC)

} srt_mem_block_t;

/*!< memory information of global management */
typedef struct mem_info
{
	kuaddr_t base;                                          
	kusize_t lenth;

	struct mem_block *sprt_mem;
} srt_mem_info_t;

#define MEM_BLOCK_HEADER_SIZE								(mrt_num_align4(sizeof(struct mem_block)))

/*!< The functions */
TARGET_EXT kint32_t memory_simple_block_create(struct mem_info *sprt_info, kuaddr_t mem_addr, kusize_t size);
TARGET_EXT void memory_simple_block_destroy(struct mem_info *sprt_info);
TARGET_EXT void *alloc_spare_simple_memory(void *ptr_head, kusize_t size);
TARGET_EXT void free_employ_simple_memory(void *ptr_head, void *ptr_mem);

/*!< malloc */
TARGET_EXT kbool_t malloc_block_initial(void);
TARGET_EXT kbool_t malloc_block_self_defines(kuaddr_t base, kusize_t size);
TARGET_EXT void malloc_block_destroy(void);

/*!< API function */
/*!
 * @brief   memory_set
 * @param   dest, data, size
 * @retval  none
 * @note    similar to "memset"
 */
static inline void memory_set(void *dest, kuint8_t data, kusize_t size)
{
    kuaddr_t start_addr, end_addr;
    kuaddr_t result;

    if (!dest)
        return;
    
    start_addr = (kuaddr_t)dest;
    end_addr   = (kuaddr_t)(start_addr + size);

    mrt_preempt_disable();

#if 0
    __asm__ __volatile__ (
        "   push {r7 - r9}      \n\t"
        "   ldr r7, [%0]        \n\t"
        "   mov r8, %1          \n\t"
        "   mov r9, %2          \n\t"
        " 1:                    \n\t"
        "   cmp r7, r8          \n\t"
        "   bhs 2f              \n\t"
        "   strb r9, [r7]       \n\t"
        "   add r7, #1          \n\t"
        "   b 1b                \n\t"
        " 2:                    \n\t"
        "   pop {r7 - r9}       \n\t"
        : 
        : "r"(&start_addr), "r"(end_addr), "r"(data)
        : "cc", "memory"
    );

#else
    __asm__ __volatile__ (
        "   ldr %0, [%1]        \n\t"
        " 1:                    \n\t"
        "   cmp %0, %2          \n\t"
        "   bhs 2f              \n\t"
        "   strb %3, [%0]       \n\t"
        "   add %0, #1          \n\t"
        "   b 1b                \n\t"
        " 2:                    \n\t"
        "   str %0, [%1]        \n\t"
        : "=&r"(result)
        : "r"(&start_addr), "r"(end_addr), "r"(data)
        : "cc", "memory"
    );

#endif

    mrt_preempt_enable();
}

/*!
 * @brief   memory_reset
 * @param   dest, size
 * @retval  none
 * @note    fill memory with zero 
 */
static inline void memory_reset(void *dest, kusize_t size)
{
    memory_set(dest, 0, size);
}

/*!
 * @brief   memory_compare
 * @param   s1, s2, size
 * @retval  none
 * @note    compare between two string
 */
static inline kuint8_t memory_compare(const void *s1, const void *s2, kusize_t size)
{
    kuaddr_t s1_addr, s2_addr, end_addr;
    kuaddr_t result1, result2;
    kuint8_t data1, data2, flag = 0;

    s1_addr = (kuaddr_t)s1;
    s2_addr = (kuaddr_t)s2;
    end_addr = (kuaddr_t)(s2_addr + size);

#if 0
    __asm__ __volatile__ (
        "   push {r4 - r9}      \n\t"
        "   mov r5, %3          \n\t"
        "   mov r6, #0          \n\t"
        "   ldr r7, [%4]        \n\t"
        "   ldr r8, [%5]        \n\t"
        " 1:                    \n\t"
        "   cmp r6, r5          \n\t"
        "   bhs 2f              \n\t"
        "   ldrb r9, [r7]       \n\t"
        "   ldrb r4, [r8]       \n\t"
        "   add r6, #1          \n\t"
        "   add r7, #1          \n\t"
        "   add r8, #1          \n\t"
        "   cmp r4, r9          \n\t"
        "   beq 1b              \n\t"
        "   mov %2, #1          \n\t"
        " 2:                    \n\t"
        "   pop {r4 - r9}       \n\t"
        : "=r"(data1), "=r"(data2), "=r"(flag)
        : "r"(size), "r"(&s1_addr), "r"(&s2_addr)
        : "memory"
    );

#else
    __asm__ __volatile__ (
        "   ldr %0, [%4]        \n\t"
        "   ldr %1, [%5]        \n\t"
        " 1:                    \n\t"
        "   cmp %1, %6          \n\t"
        "   bhs 3f              \n\t"
        "   ldrb %2, [%0]       \n\t"
        "   ldrb %3, [%1]       \n\t"
        "   cmp %2, %3          \n\t"
        "   bne 2f              \n\t"
        "   add %0, #1          \n\t"
        "   add %1, #1          \n\t"
        "   b 1b                \n\t"
        " 2:                    \n\t"
        "   mov %2, #1          \n\t"
        "   str %2, [%7]        \n\t"
        " 3:                    \n\t"
        "   str %0, [%4]        \n\t"
        "   str %1, [%5]        \n\t"
        : "=&r"(result1), "=&r"(result2), "=&r"(data1), "=&r"(data2)
        : "r"(&s1_addr), "r"(&s2_addr), "r"(end_addr), "r"(&flag)
        : "cc", "memory"
    );

#endif

    return flag;
}

/*!
 * @brief   memory_copy
 * @param   dest, src, size
 * @retval  none
 * @note    clone. unit: byte
 */
static inline void *memory_copy(void *dest, const void *src, kusize_t size)
{
    kuaddr_t s1_addr, s2_addr, end_addr;
    kuaddr_t result1, result2;
    kuint8_t data;

    if (!dest || !src)
        return mrt_nullptr;

    s1_addr = (kuaddr_t)dest;
    s2_addr = (kuaddr_t)src;
    end_addr = (kuaddr_t)(s2_addr + size);

#if 0
    __asm__ __volatile__ (
        "   push {r5 - r9}      \n\t"
        "   ldr r5, [%1]        \n\t"
        "   ldr r6, [%2]        \n\t"
        "   mov r7, %0          \n\t"
        "   mov r8, #0          \n\t"
        " 1:                    \n\t"
        "   cmp r8, r7          \n\t"
        "   bhs 2f              \n\t"
        "   ldrb r9, [r6]       \n\t"
        "   strb r9, [r5]       \n\t"
        "   add r6, #1          \n\t"
        "   add r5, #1          \n\t"
        "   add r8, #1          \n\t"
        "   b 1b                \n\t"
        " 2:                    \n\t"
        "   str r5, [%1]        \n\t"
        "   str r6, [%2]        \n\t"
        "   pop {r5 - r9}       \n\t"
        : 
        : "r"(size), "r"(&s1_addr), "r"(&s2_addr)
        : "cc", "memory"
    );

#else
    __asm__ __volatile__ (
        "   ldr %0, [%3]        \n\t"
        "   ldr %1, [%4]        \n\t"
        " 1:                    \n\t"
        "   cmp %1, %5          \n\t"
        "   bhs 2f              \n\t"
        "   ldrb %2, [%1]       \n\t"
        "   strb %2, [%0]       \n\t"
        "   add %0, #1          \n\t"
        "   add %1, #1          \n\t"
        "   b 1b                \n\t"
        " 2:                    \n\t"
        "   str %0, [%3]        \n\t"
        "   str %1, [%4]        \n\t"
        : "=&r"(result1), "=&r"(result2), "=&r"(data)
        : "r"(&s1_addr), "r"(&s2_addr), "r"(end_addr)
        : "cc", "memory"
    );

#endif

    return (void *)s1_addr;
}

#endif  /* __MEM_MANAGE_H */
