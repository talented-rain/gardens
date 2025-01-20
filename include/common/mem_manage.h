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

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The includes */
#include "generic.h"
#include "list_types.h"
#include <kernel/kernel.h>

/*!< The defines */
#define MEMORY_POOL_MAGIC                      (0xdfa69fe3)

typedef struct m_area
{
    void *base;
    kusize_t size;
    kuaddr_t offset;

} srt_m_area_t;

/*!< memory block */
typedef struct mem_block
{
    kuint32_t magic;                            /*!< magic number, it should be set to "MEMORY_POOL_MAGIC" */
    kuaddr_t base;								/*!< current start address */
    kusize_t lenth;								/*!< current block total lenth(unit: byte), including header of memory info */
    kusize_t remain;							/*!< the lenth of remaining usable memoty(unit: byte) */

    struct mem_block *sprt_prev;				/*!< the first address of last memory block */
    struct mem_block *sprt_next;				/*!< the first address of next memory block */

    struct list_head sgrt_link;

} srt_mem_block_t;

#define IS_MEMORYPOOL_VALID(this)               ((this)->magic == MEMORY_POOL_MAGIC)
#define MEM_BLOCK_HEADER_SIZE                   (mrt_num_align4(sizeof(struct mem_block)))  /*!< 32bytes */

enum __ERT_MEM_TYPE
{
    NR_MEM_1Bytes = 0,
    NR_MEM_2Bytes,
    NR_MEM_4Bytes,
    NR_MEM_8Bytes,
    NR_MEM_16Bytes,

    NR_MEM_LowerBytes = NR_MEM_16Bytes,         /*!< [0, 31] */
    NR_MEM_32Bytes,                             /*!< [32, 63] */
    NR_MEM_64Bytes,                             /*!< [64, 127] */
    NR_MEM_128Bytes,                            /*!< [128, 255] */
    NR_MEM_256Bytes,                            /*!< [256, 511] */
    NR_MEM_512Bytes,                            /*!< [512, 1023] */
    NR_MEM_1024Bytes,                           /*!< [1024, 2047] */
    NR_MEM_2048Bytes,                           /*!< [2048, 4095] */
    NR_MEM_4096Bytes,                           /*!< [4096, 8191] */
    NR_MEM_8192Bytes,                           /*!< [8192, ...] */
    NR_MEM_HighBytes = NR_MEM_8192Bytes,

    NR_MEM_NUM,

    NR_MEM_LowerLimit = 1U << (NR_MEM_LowerBytes + 1),
    NR_MEM_HighLimit = 1U << NR_MEM_HighBytes,
};

typedef struct mem_hash
{
    struct list_head sgrt_list;

} srt_mem_hash_t;

/*!< memory information of global management */
typedef struct mem_info
{
    kuaddr_t base;                                          
    kusize_t lenth;
    
    struct mem_block *sprt_mem;
    struct mem_hash sgrt_hash[NR_MEM_NUM];

    void *(*alloc)(struct mem_info *sprt_info, kusize_t size);
    void (*free)(struct mem_info *sprt_info, void *ptr_mem);

} srt_mem_info_t;

/*!< The functions */
extern kint32_t memory_simple_block_create(struct mem_info *sprt_info, kuaddr_t mem_addr, kusize_t size);
extern void memory_simple_block_destroy(struct mem_info *sprt_info);

extern kint32_t memory_block_create(struct mem_info *sprt_info, kuaddr_t mem_addr, kusize_t size);
extern void memory_block_destroy(struct mem_info *sprt_info);

/*!< malloc */
extern kbool_t malloc_block_initial(void);
extern kbool_t malloc_block_self_defines(kuaddr_t base, kusize_t size);
extern void malloc_block_destroy(void);

/*!< assembly */
extern kuaddr_t __memset_ex(kuaddr_t _start, kuaddr_t _end, kuint32_t data);

extern void *memset_ex(void *__s, unsigned int __c, size_t __n);

/*!< API function */
/*!
 * @brief   kmemset
 * @param   dest, data, size
 * @retval  none
 * @note    similar to "memset"
 */
static inline void kmemset(void *dest, kuint8_t data, kusize_t size)
{
    kuaddr_t start_addr, end_addr;
    kuaddr_t result;

    if (!dest)
        return;
    
    start_addr = (kuaddr_t)dest;
    end_addr   = (kuaddr_t)(start_addr + size);

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
}

/*!
 * @brief   kmemset_ex
 * @param   dest, data, size
 * @retval  none
 * @note    "memset" just allow set 1-byte data, but this func can set 4-bytes
 */
static inline void kmemset_ex(void *dest, kuint32_t data, kusize_t size)
{
    kuaddr_t start_addr, end_addr;
//  kuaddr_t result;

    if (!dest)
        return;
    
    start_addr = (kuaddr_t)dest;
    end_addr   = (kuaddr_t)(start_addr + size);

    /*!< address must be 4 bytes alignment */
    if ((start_addr & 0x3) ||
        (end_addr & 0x3))
    {
        kmemset(dest, data, size);
        return;
    }

#if 0
    __asm__ __volatile__ (
        "   ldr %0, [%1]        \n\t"
        " 1:                    \n\t"
        "   cmp %0, %2          \n\t"
        "   bhs 2f              \n\t"
        "   str %3, [%0]        \n\t"
        "   add %0, #4          \n\t"
        "   b 1b                \n\t"
        " 2:                    \n\t"
        "   str %0, [%1]        \n\t"
        : "=&r"(result)
        : "r"(&start_addr), "r"(end_addr), "r"(data)
        : "cc", "memory"
    );
#else
    __memset_ex((kuaddr_t)&start_addr, end_addr, data);
#endif
}

/*!
 * @brief   kmemzero
 * @param   dest, size
 * @retval  none
 * @note    fill memory with zero 
 */
static inline void kmemzero(void *dest, kusize_t size)
{
    kmemset(dest, 0, size);
}
#define mrt_bzero(addr, size)       kmemzero(addr, size)

/*!
 * @brief   kmemcmp
 * @param   s1, s2, size
 * @retval  none
 * @note    compare between two string
 */
static inline kuint8_t kmemcmp(const void *s1, const void *s2, kusize_t size)
{
    kuaddr_t s1_addr, s2_addr, end_addr;
    kuaddr_t result1, result2;
    kuint8_t data1, data2, flag = 0;

    s1_addr = (kuaddr_t)s1;
    s2_addr = (kuaddr_t)s2;
    end_addr = (kuaddr_t)(s2_addr + size);

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

    return flag;
}

/*!
 * @brief   kmemcpy
 * @param   dest, src, size
 * @retval  none
 * @note    clone. unit: byte
 */
static inline void *kmemcpy(void *dest, const void *src, kusize_t size)
{
    kuaddr_t s1_addr, s2_addr, end_addr;
    kuaddr_t result1, result2;
    kuint8_t data;

    if (!dest || !src)
        return mrt_nullptr;

    s1_addr = (kuaddr_t)dest;
    s2_addr = (kuaddr_t)src;
    end_addr = (kuaddr_t)(s2_addr + size);

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

    return (void *)s1_addr;
}

/*!
 * @brief   copy *val to *addr
 * @param   addr, val
 * @retval  none
 * @note    if addr or val is not in 4-bytes-alignment, using u32_set2u8()
 */
static inline void u32_set2u8(void *addr, void *val)
{
#if 1
    kmemcpy(addr, val, sizeof(kuint32_t));

#else
    *((kuint8_t *)addr++) = *((kuint8_t *)val++);
    *((kuint8_t *)addr++) = *((kuint8_t *)val++);
    *((kuint8_t *)addr++) = *((kuint8_t *)val++);
    *((kuint8_t *)addr++) = *((kuint8_t *)val++);
#endif
}

/*!
 * @brief   copy *val to *addr
 * @param   addr, val
 * @retval  none
 * @note    if addr or val is not in 2-bytes-alignment, using u16_set2u8()
 */
static inline void u16_set2u8(void *addr, void *val)
{
#ifdef __cplusplus
    kmemcpy(addr, val, sizeof(kuint16_t));

#else
    *((kuint8_t *)addr++) = *((kuint8_t *)val++);
    *((kuint8_t *)addr++) = *((kuint8_t *)val++);
#endif
}

/*!
 * @brief   copy val to *addr
 * @param   addr, val
 * @retval  none
 * @note    none
 */
#define mrt_u32_set(addr, val)   \
    do {    \
        __asm__ __volatile__ (  \
            " mov %0, %1  "   \
            : "=r"(*(addr)) \
            : "r"(val)  \
            : "cc", "memory"    \
        );  \
    } while (0);

static inline __force_inline 
void u32_set(kuint32_t *addr, kuint32_t offset, kuint32_t val)
{
    mrt_u32_set(addr + offset, val);
}

/*!
 * @brief   copy val to *addr
 * @param   addr, val
 * @retval  none
 * @note    none
 */
#define mrt_u16_set(addr, val)   \
    do {    \
        __asm__ __volatile__ (  \
            " mov.w %0, %1  "   \
            : "=r"(*(addr)) \
            : "r"(val)  \
            : "cc", "memory"    \
        );  \
    } while (0);

static inline __force_inline 
void u16_set(kuint16_t *addr, kuint32_t offset, kuint16_t val)
{
    mrt_u16_set(addr + offset, val);
}

/*!
 * @brief   copy val to *addr
 * @param   addr, val
 * @retval  none
 * @note    none
 */
#define mrt_u8_set(addr, val)   \
    do {    \
        __asm__ __volatile__ (  \
            " mov.b %0, %1  "   \
            : "=r"(*(addr)) \
            : "r"(val)  \
            : "cc", "memory"    \
        );  \
    } while (0);

static inline __force_inline 
void u8_set(kuint8_t *addr, kuint32_t offset, kuint8_t val)
{
    mrt_u8_set(addr + offset, val);
}

#ifdef __cplusplus
    }
#endif

#endif  /* __MEM_MANAGE_H */
