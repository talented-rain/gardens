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
#include <common/libcxplus.h>
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
    crt_lwip_data_t(void *args) : args(args), echo_cnt(0) {}
    ~crt_lwip_data_t() {}

    void startup(void);
    void excute(void);

    kint32_t fd;
    kuint8_t rx_buffer[128];

    void *args;
    kint32_t echo_cnt;
};

/*!< The globals */

/*!< The functions */

#endif

#endif /* __LWIP_APP_H_ */
