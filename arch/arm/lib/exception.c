/*
 * ARM V7 Exception API Function
 *
 * File Name:   exception.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.09.10
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <asm/exception.h>
#include <common/error_types.h>
#include <common/io_stream.h>

/*!< The globals*/
kuaddr_t prefecth_abort_addr;
kuaddr_t data_abort_addr;

/*!< API function */
/*!
 * @brief   exec_undefined_handler
 * @param   none
 * @retval  none
 * @note    undefined exception
 */
void exec_undefined_handler(void)
{
    print_err("%s: program instruction undefine\n", __FUNCTION__);
//  mrt_assert(false);
}

/*!
 * @brief   exec_prefetch_abort_handler
 * @param   none
 * @retval  none
 * @note    prefetch abort exception
 */
void exec_prefetch_abort_handler(void)
{
    print_err("%s: lr \'%x\' cause fault\n", __FUNCTION__, prefecth_abort_addr);
//  mrt_assert(false);
}

/*!
 * @brief   exec_data_abort_handler
 * @param   none
 * @retval  none
 * @note    data abort exception
 */
void exec_data_abort_handler(void)
{
    print_err("%s: lr \'%x\' cause fault\n", __FUNCTION__, data_abort_addr);
//  mrt_assert(false);
}

/*!
 * @brief   exec_unused_handler
 * @param   none
 * @retval  none
 * @note    unused exception
 */
void exec_unused_handler(void)
{
    mrt_assert(false);
}

/* end of file*/
