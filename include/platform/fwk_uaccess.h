/*
 * User Space and Kernel Interface
 *
 * File Name:   fwk_uaccess.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.07.02
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_UACCESS_H_
#define __FWK_UACCESS_H_

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The includes */
#include <platform/fwk_basic.h>

/*!< The defines */
struct fwk_vm_area
{
    kuaddr_t virt_addr;
    kusize_t size;
    kuint32_t offset;
};

/*!< The functions */
extern kusize_t fwk_copy_from_user(void *ptr_dst, const void *ptr_user, kusize_t size);
extern kusize_t fwk_copy_to_user(void *ptr_user, void *ptr_Src, kusize_t size);

#ifdef __cplusplus
    }
#endif

#endif /*!< __FWK_UACCESS_H_ */
