/*
 * Assembly Header Defines For ARMv7
 *
 * File Name:   asm_config.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.09.17
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __ASM_CONFIG_H
#define __ASM_CONFIG_H

/*!< Registers */
#define ARCH_OFFSET_R0                  (0)
#define ARCH_OFFSET_R1                  (4)
#define ARCH_OFFSET_R2                  (8)
#define ARCH_OFFSET_R3                  (12)
#define ARCH_OFFSET_R4                  (16)
#define ARCH_OFFSET_R5                  (20)
#define ARCH_OFFSET_R6                  (24)
#define ARCH_OFFSET_R7                  (28)
#define ARCH_OFFSET_R8                  (32)
#define ARCH_OFFSET_R9                  (36)
#define ARCH_OFFSET_R10                 (40)
#define ARCH_OFFSET_R11                 (44)
#define ARCH_OFFSET_R12                 (48)
#define ARCH_OFFSET_R13                 (52)
#define ARCH_OFFSET_R14                 (56)
#define ARCH_OFFSET_R15                 (60)

#define ARCH_OFFSET_FP                  (ARCH_OFFSET_R11)
#define ARCH_OFFSET_IP                  (ARCH_OFFSET_R12)
#define ARCH_OFFSET_SP                  (ARCH_OFFSET_R13)
#define ARCH_OFFSET_LR                  (ARCH_OFFSET_R14)
#define ARCH_OFFSET_PC                  (ARCH_OFFSET_R15)
#define ARCH_OFFSET_PSR                 (64)

/*!< 64bytes */
#define ARCH_FRAME_SIZE                 (68)

/*!< CPSR */
/*!< 
 * Equaling to  "mrs r0, cpsr \n orr r0, r0, #CPSR_BIT_I \n msr cpsr, r0" 
 * Or:          "mrs r0, cpsr \n bic r0, r0, #CPSR_BIT_I \n msr cpsr, r0"
 */
#define ARCH_ENABLE_IRQ                 cpsie i
#define ARCH_DISABLE_IRQ                cpsid i
#define ARCH_ENABLE_FIQ                 cpsie f
#define ARCH_DISABLE_FIQ                cpsid f

/*!< Usage:
 * 1)   mrs r0, cpsr                                        @ read cpsr register to r0
 *      bic r0, r0, #0x1f                                   @ clear mode bits
 *      orr r0, r0, #ARCH_SVC_MODE                          @ set the cpu to SVC mode
 *      msr cpsr, r0
 * 2)   cps #ARCH_SVC_MODE                                  @ Equivalent to the first method
 */
#define ARCH_USE_MODE                   (0x10)
#define ARCH_FIQ_MODE                   (0x11)
#define ARCH_IRQ_MODE                   (0x12)
#define ARCH_SVC_MODE                   (0x13)
#define ARCH_MON_MODE                   (0x16)
#define ARCH_ABT_MODE                   (0x17)
#define ARCH_HYP_MODE                   (0x1a)
#define ARCH_UND_MODE                   (0x1b)
#define ARCH_SYS_MODE                   (0x1f)

/*!< Usage: 
 * !CPSR_BIT_T & !CPSR_BIT_J: ARM
 *  CPSR_BIT_T &  CPSR_BIT_J: Thumb
 */
#define CPSR_BIT_M(m)                   ((m) & 0x1f)        /*!< work mode*/
#define CPSR_BIT_T                      (1 << 5)            /*!< Thumb Execution State */
#define CPSR_BIT_F                      (1 << 6)            /*!< FIQ Mask. FIQ Enable: 0; Disable: 1 */
#define CPSR_BIT_I                      (1 << 7)            /*!< IRQ Mask. IRQ Enable: 0; Disable: 1 */
#define CPSR_BIT_A                      (1 << 8)            /*!< ABT Mask. ABT Enable: 0; Disable: 1 */
#define CPSR_BIT_E                      (1 << 9)            /*!< Big Endian: 1; Littile Endian: 0 */
#define CPSR_BIT_J                      (1 << 24)           /*!< Jazelle */
#define CPSR_BIT_Q                      (1 << 27)           /*!< Saturation Condition Flag */
#define CPSR_BIT_V                      (1 << 28)           /*!< Overflow Condition Code Flag */
#define CPSR_BIT_C                      (1 << 29)           /*!< Carry Condition Code Flag */
#define CPSR_BIT_Z                      (1 << 30)           /*!< Zero Condition Code Flag */
#define CPSR_BIT_N                      (1 << 31)           /*!< Negative Condition Code Flag */

/*!< CP15
 * General Usage: mcr/mrc p15, <opc1>, <Rn>, <CRn>, <CRm>, <opc2>
 * For Example: "mrc/mcr p15, 0, r0, c1, c0, 0"
 * 
 * CP15: 0 ~ 15
 *         c0:  ID code program (read only)
 *         c1:  SCTLR (write only)
 *         c2:  Address tlb base
 *         c3:  Access control
 *         c4:  Reserved
 *         c5:  Memory invalid address
 *         c6:  Memory invalid address
 *         c7:  High speed cache and Writting cache
 *         c8:  TLB control. TLB: virtual memory MMU
 *         c9:  High speed cache lock
 *         c10: TLB lock. TLB: virtual memory MMU
 *         c11: TCM ACCESS
 *         c12: VBAR
 *         c13: process ID
 *         c14: Reserved
 *         c15: ......
 * CP14: debug
 * CP10, CP11: Float and Vector Calucation
 * CP8, CP9, CP12, CP13: Reserved for ARM
 * CP0 ~ CP7: Reserved for users
 */
#define READ_CP15_REGISTER(opc1, Rn, CRn, CRm, opc2)    \
                                        mrc p15, opc1, Rn, CRn, CRm, opc2
#define WRITE_CP15_REGISTER(opc1, Rn, CRn, CRm, opc2)    \
                                        mcr p15, opc1, Rn, CRn, CRm, opc2

#define READ_CP15_SCTLR(Rn)             READ_CP15_REGISTER( 0, Rn, c1,  c0,  0)
#define WRITE_CP15_SCTLR(Rn)            WRITE_CP15_REGISTER(0, Rn, c1,  c0,  0)
#define WRITE_CP15_VBAR(Rn)             WRITE_CP15_REGISTER(0, Rn, c12, c0,  0)
#define WRITE_CP15_TLBS(Rn)             WRITE_CP15_REGISTER(0, Rn, c8,  c7,  0)
#define WRITE_CP15_ICACHE(Rn)           WRITE_CP15_REGISTER(0, Rn, c7,  c5,  0)
#define WRITE_CP15_BP(Rn)               WRITE_CP15_REGISTER(0, Rn, c7,  c5,  6)
#define WRITE_CP15_DSB(Rn)              WRITE_CP15_REGISTER(0, Rn, c7,  c10, 4)
#define WRITE_CP15_ISB(Rn)              WRITE_CP15_REGISTER(0, Rn, c7,  c5,  4)
#define WRITE_CP15_CLR_DCACHE_MVA(Rn)   WRITE_CP15_REGISTER(0, Rn, c7,  c14, 1)

#define CP15_SCTLR_BIT_M	            (1 << 0)	        /*!< MMU enable */
#define CP15_SCTLR_BIT_A	            (1 << 1)	        /*!< Alignment abort enable */
#define CP15_SCTLR_BIT_C	            (1 << 2)	        /*!< Dcache enable */
#define CP15_SCTLR_BIT_W	            (1 << 3)	        /*!< Write buffer enable */
#define CP15_SCTLR_BIT_P	            (1 << 4)	        /*!< 32-bit exception handler */
#define CP15_SCTLR_BIT_D	            (1 << 5)	        /*!< 32-bit data address range */
#define CP15_SCTLR_BIT_L	            (1 << 6)	        /*!< Implementation defined */
#define CP15_SCTLR_BIT_B	            (1 << 7)	        /*!< Big endian */
#define CP15_SCTLR_BIT_S	            (1 << 8)	        /*!< System MMU protection */
#define CP15_SCTLR_BIT_R	            (1 << 9)	        /*!< ROM MMU protection */
#define CP15_SCTLR_BIT_F	            (1 << 10)	        /*!< Implementation defined */
#define CP15_SCTLR_BIT_Z	            (1 << 11)	        /*!< Branch Prediction defined */
#define CP15_SCTLR_BIT_I	            (1 << 12)	        /*!< Icache enable */
#define CP15_SCTLR_BIT_V	            (1 << 13)	        /*!< Vectors relocated to 0xffff0000 */
#define CP15_SCTLR_BIT_RR	            (1 << 14)	        /*!< Round Robin cache replacement */
#define CP15_SCTLR_BIT_L4	            (1 << 15)	        /*!< LDR pc can set T bit */
#define CP15_SCTLR_BIT_DT	            (1 << 16)
#define CP15_SCTLR_BIT_IT	            (1 << 18)
#define CP15_SCTLR_BIT_ST	            (1 << 19)
#define CP15_SCTLR_BIT_FI	            (1 << 21)	        /*!< Fast interrupt (lower latency mode) */
#define CP15_SCTLR_BIT_U	            (1 << 22)	        /*!< Unaligned access operation */
#define CP15_SCTLR_BIT_XP	            (1 << 23)	        /*!< Extended page tables */
#define CP15_SCTLR_BIT_VE	            (1 << 24)	        /*!< Vectored interrupts */
#define CP15_SCTLR_BIT_EE	            (1 << 25)	        /*!< Exception (Big) Endian */
#define CP15_SCTLR_BIT_TRE	            (1 << 28)	        /*!< TEX remap enable */
#define CP15_SCTLR_BIT_AFE	            (1 << 29)	        /*!< Access flag enable */
#define CP15_SCTLR_BIT_TE	            (1 << 30)	        /*!< Thumb exception enable */

#endif /* __ASM_CONFIG_H */
