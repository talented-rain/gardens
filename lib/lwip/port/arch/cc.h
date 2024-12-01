/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 * Author: Stefano Oliveri <stefano.oliveri@st.com>
 *
 */
#ifndef __CC_H__
#define __CC_H__

#include <common/generic.h>
#include <common/io_stream.h>

#undef LWIP_NOASSERT

#ifdef __REDLIB__
#define LWIP_NO_INTTYPES_H 1
#endif

#if (!defined(BYTE_ORDER) || (BYTE_ORDER != LITTLE_ENDIAN))
#undef  BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif

typedef kuint8_t    u8_t;
typedef kint8_t     s8_t;
typedef kuint16_t   u16_t;
typedef kint16_t    s16_t;
typedef kuint32_t   u32_t;
typedef kint32_t    s32_t;
typedef kuint32_t   mem_ptr_t;

#if __GNUC__
#define PACK_STRUCT_BEGIN
#elif defined(__IAR_SYSTEMS_ICC__)
#define PACK_STRUCT_BEGIN _Pragma("pack(1)")
#elif defined(__arm__) && defined(__ARMCC_VERSION)
#define PACK_STRUCT_BEGIN __packed
#else
#define PACK_STRUCT_BEGIN
#endif

#if __GNUC__
#define PACK_STRUCT_STRUCT __packed
#elif defined(__IAR_SYSTEMS_ICC__)
#define PACK_STRUCT_STRUCT
#elif defined(__arm__) && defined(__ARMCC_VERSION)
#define PACK_STRUCT_STRUCT
#else
#define PACK_STRUCT_STRUCT
#endif

#if __GNUC__
#define PACK_STRUCT_END
#elif defined(__IAR_SYSTEMS_ICC__)
#define PACK_STRUCT_END _Pragma("pack()")
#elif defined(__arm__) && defined(__ARMCC_VERSION)
#define PACK_STRUCT_END
#else
#define PACK_STRUCT_END
#endif

#define PACK_STRUCT_FIELD(x) x

// Platform specific diagnostic output
#include "sys_arch.h"

// non-fatal, print a message.
#define LWIP_PLATFORM_DIAG(x)                     do { print_debug x; } while(0)
// fatal, print message and abandon execution.
#define LWIP_PLATFORM_ASSERT(x)                   print_warn(x)

#endif /* __CC_H__ */
