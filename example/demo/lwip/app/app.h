/*
 * User Thread Instance Tables
 *
 * File Name:   app.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.11.21
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __LWIP_APP_H_
#define __LWIP_APP_H_

#ifdef __cplusplus

/*!< The globals */
#include <common/basic_types.h>
#include <common/error_types.h>
#include <common/generic.h>
#include <common/stdload.h>
#include <common/list_types.h>
#include <platform/fwk_basic.h>
#include <platform/net/fwk_if.h>
#include <platform/net/fwk_netif.h>
#include <platform/net/fwk_socket.h>

/*!< The defines */
class crt_lwip_data_t
{
public:
    crt_lwip_data_t() {}
    ~crt_lwip_data_t() {}

    kint32_t fd;
    kuint8_t rx_buffer[128];
};

/*!< The globals */

/*!< The functions */
extern void lwip_task_startup(crt_lwip_data_t &sgrt_data);
extern void lwip_task(crt_lwip_data_t &sgrt_data);

#endif

#endif /* __LWIP_APP_H_ */
