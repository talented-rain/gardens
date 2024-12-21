/*
 * Board Configuration
 *
 * File Name:   boot_text.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.09.13
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __BOOT_TEXT_H
#define __BOOT_TEXT_H

/*!< The includes */
#include <common/generic.h>
#include <common/io_stream.h>
#include <board/board_config.h>

/*!< dynamic init/exit sections*/
#define __DYNC_INIT_SEC(n)							__section(".dync_init."#n)
#define __DYNC_EXIT_SEC(n)							__section(".dync_exit."#n)

/*!< typedef dync_init/dync_exit */
typedef kint32_t (*dync_init_t) (void);
typedef void (*dync_exit_t) (void);

/*!< sram memory area*/
TARGET_EXT kuaddr_t __ram_ddr_start;
TARGET_EXT kuaddr_t __ram_ddr_end;

#define PROGRAM_RAM_START                           ((kuaddr_t)(&__ram_ddr_start))
#define PROGRAM_RAM_SIZE                            ((kusize_t)((kuaddr_t)(&__ram_ddr_end) - (kuaddr_t)(&__ram_ddr_start)))

/*!< VBAR */
TARGET_EXT kuaddr_t __vector_table;

#define VECTOR_TABLE_BASE                           ((kuaddr_t)&__vector_table)

/*!< dync_init */
TARGET_EXT dync_init_t __dync_init_start;
TARGET_EXT dync_init_t __dync_init_end;
TARGET_EXT dync_init_t __dync_init_0_start;
TARGET_EXT dync_init_t __dync_init_1_start;
TARGET_EXT dync_init_t __dync_init_2_start;
TARGET_EXT dync_init_t __dync_init_3_start;
TARGET_EXT dync_init_t __dync_init_4_start;
TARGET_EXT dync_init_t __dync_init_5_start;
TARGET_EXT dync_init_t __dync_init_6_start;
TARGET_EXT dync_init_t __dync_init_7_start;
TARGET_EXT dync_init_t __dync_init_8_start;
TARGET_EXT dync_init_t __dync_init_9_start;

/*!< dync_exit */
TARGET_EXT dync_exit_t __dync_exit_start;
TARGET_EXT dync_exit_t __dync_exit_end;
TARGET_EXT dync_exit_t __dync_exit_0_start;
TARGET_EXT dync_exit_t __dync_exit_1_start;
TARGET_EXT dync_exit_t __dync_exit_2_start;
TARGET_EXT dync_exit_t __dync_exit_3_start;
TARGET_EXT dync_exit_t __dync_exit_4_start;
TARGET_EXT dync_exit_t __dync_exit_5_start;
TARGET_EXT dync_exit_t __dync_exit_6_start;
TARGET_EXT dync_exit_t __dync_exit_7_start;
TARGET_EXT dync_exit_t __dync_exit_8_start;
TARGET_EXT dync_exit_t __dync_exit_9_start;

/*< heap */
TARGET_EXT kuaddr_t __heap_start;
TARGET_EXT kuaddr_t __heap_end;

/*!< Macro */
#define MEMORY_HEAP_START                   ((kuaddr_t)(&__heap_start))
#define MEMORY_HEAP_END                     ((kuaddr_t)(&__heap_end))

/*!< stack */
TARGET_EXT kuaddr_t __stack_start;
TARGET_EXT kuaddr_t __stack_end;
TARGET_EXT kuaddr_t __svc_stack_start;
TARGET_EXT kuaddr_t __svc_stack_end;
TARGET_EXT kuaddr_t __sys_stack_start;
TARGET_EXT kuaddr_t __sys_stack_end;
TARGET_EXT kuaddr_t __irq_stack_start;
TARGET_EXT kuaddr_t __irq_stack_end;
TARGET_EXT kuaddr_t __fiq_stack_start;
TARGET_EXT kuaddr_t __fiq_stack_end;
TARGET_EXT kuaddr_t __abt_stack_start;
TARGET_EXT kuaddr_t __abt_stack_end;
TARGET_EXT kuaddr_t __und_stack_start;
TARGET_EXT kuaddr_t __und_stack_end;

/*!< Macro */
#define SVC_MODE_STACK_BASE                 ((kuaddr_t)(&__svc_stack_end))
#define SVC_MODE_STACK_SIZE                 ((kusize_t)((kuaddr_t)(&__svc_stack_end) - (kuaddr_t)(&__svc_stack_start)))
#define SYS_MODE_STACK_BASE                 ((kuaddr_t)(&__sys_stack_end))
#define SYS_MODE_STACK_SIZE                 ((kusize_t)((kuaddr_t)(&__sys_stack_end) - (kuaddr_t)(&__sys_stack_start)))
#define FIQ_MODE_STACK_BASE                 ((kuaddr_t)(&__fiq_stack_end))
#define FIQ_MODE_STACK_SIZE                 ((kusize_t)((kuaddr_t)(&__fiq_stack_end) - (kuaddr_t)(&__fiq_stack_start)))
#define IRQ_MODE_STACK_BASE                 ((kuaddr_t)(&__irq_stack_end))
#define IRQ_MODE_STACK_SIZE                 ((kusize_t)((kuaddr_t)(&__irq_stack_end) - (kuaddr_t)(&__irq_stack_start)))
#define ABT_MODE_STACK_BASE                 ((kuaddr_t)(&__abt_stack_end))
#define ABT_MODE_STACK_SIZE                 ((kusize_t)((kuaddr_t)(&__abt_stack_end) - (kuaddr_t)(&__abt_stack_start)))
#define UND_MODE_STACK_BASE                 ((kuaddr_t)(&__und_stack_end))
#define UND_MODE_STACK_SIZE                 ((kusize_t)((kuaddr_t)(&__und_stack_end) - (kuaddr_t)(&__und_stack_start)))

/*!< memory pool */
TARGET_EXT kuaddr_t __mem_pool_start;
TARGET_EXT kuaddr_t __mem_pool_end;

#define MEMORY_POOL_BASE                    ((kuaddr_t)&__mem_pool_start)
#define MEMORY_POOL_END                     ((kuaddr_t)&__mem_pool_end)
#define MEMORY_POOL_SIZE                    ((kusize_t)((kuaddr_t)(&__mem_pool_end) - (kuaddr_t)(&__mem_pool_start)))

/*!< framebuffer */
TARGET_EXT kuaddr_t __fb_dram_start;
TARGET_EXT kuaddr_t __fb_dram_end;

#define FBUFFER_DRAM_BASE                   ((kuaddr_t)&__fb_dram_start)
#define FBUFFER_DRAM_SIZE                   ((kusize_t)((kuaddr_t)(&__fb_dram_end) - (kuaddr_t)(&__fb_dram_start)))

/*!< network */
TARGET_EXT kuaddr_t __sk_buffer_start;
TARGET_EXT kuaddr_t __sk_buffer_end;

#define SK_BUFFER_BASE                      ((kuaddr_t)&__sk_buffer_start)
#define SK_BUFFER_SIZE                      ((kusize_t)((kuaddr_t)(&__sk_buffer_end) - (kuaddr_t)(&__sk_buffer_start)))

/*!< API functions */
/*!
 * @brief   print text and section information
 * @param   none
 * @retval  none
 * @note    it should be called by board_init_r()
 */
static inline void boot_text_print(void)
{
    print_info("memory pool base address: %x, size = %d KB\n", MEMORY_POOL_BASE, __BYTES_TO_KB(MEMORY_POOL_SIZE));
    if (MEMORY_POOL_END <= MEMORY_POOL_BASE)
    {
        /*!< memory pool is only used after kernel starting; bootloader uses another areas */
        print_err("error: there is not enough memory avaliable for memory pool!\n");
        mrt_assert(false);
    }
    
    print_info("framebuffer base address: %x, size = %d KB\n", FBUFFER_DRAM_BASE, __BYTES_TO_KB(FBUFFER_DRAM_SIZE));
    print_info("sock buffer base address: %x, size = %d KB\n", SK_BUFFER_BASE, __BYTES_TO_KB(SK_BUFFER_SIZE));

    /*!< stack */
    print_info("svc stack   top  address: %x, size = %d KB\n", SVC_MODE_STACK_BASE, __BYTES_TO_KB(SVC_MODE_STACK_SIZE));
//  print_info("usr stack   top  address: %x, size = %d KB\n", SYS_MODE_STACK_BASE, __BYTES_TO_KB(SYS_MODE_STACK_SIZE));
    print_info("sys stack   top  address: %x, size = %d KB\n", SYS_MODE_STACK_BASE, __BYTES_TO_KB(SYS_MODE_STACK_SIZE));
    print_info("abt stack   top  address: %x, size = %d KB\n", ABT_MODE_STACK_BASE, __BYTES_TO_KB(ABT_MODE_STACK_SIZE));
    print_info("irq stack   top  address: %x, size = %d KB\n", IRQ_MODE_STACK_BASE, __BYTES_TO_KB(IRQ_MODE_STACK_SIZE));
    print_info("fiq stack   top  address: %x, size = %d KB\n", FIQ_MODE_STACK_BASE, __BYTES_TO_KB(FIQ_MODE_STACK_SIZE));
    print_info("und stack   top  address: %x, size = %d KB\n", UND_MODE_STACK_BASE, __BYTES_TO_KB(UND_MODE_STACK_SIZE));

    /*!< heap */
    print_info("__brk heap  base address: %x, size = %d KB\n", MEMORY_HEAP_START, __BYTES_TO_KB(MEMORY_HEAP_END));
}

#endif /* __BOOT_TEXT_H */
