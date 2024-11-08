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
#define MEMORY_POOL_SIZE                    ((kusize_t)((kuaddr_t)(&__mem_pool_end) - (kuaddr_t)(&__mem_pool_start)))

/*!< framebuffer */
TARGET_EXT kuaddr_t __fb_dram_start;
TARGET_EXT kuaddr_t __fb_dram_end;

#define FBUFFER_DRAM_BASE                   ((kuaddr_t)&__fb_dram_start)
#define FBUFFER_DRAM_SIZE                   ((kusize_t)((kuaddr_t)(&__fb_dram_end) - (kuaddr_t)(&__fb_dram_start)))

#endif /* __BOOT_TEXT_H */
