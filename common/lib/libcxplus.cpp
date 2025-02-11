/*
 * C++ Standard Lib Reload
 *
 * File Name:   libcxplus.cpp
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2025.01.07
 *
 * Copyright (c) 2025   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The globals */
#include <common/libcxplus.h>
#include <common/io_stream.h>

/*!< The defines */

/*!< The globals */
BEGIN_NAMESPACE(bsc)

istream cin;
ostream cout;
ostream cerr;

END_NAMESPACE(bsc)

/*!< The functions */
/*!
 * @brief  reload "new": ptr = new int
 * @param  size
 * @retval none
 * @note   none
 */
void *operator new(size_t size)
{
    void *ptr = kmalloc(size, GFP_KERNEL);
    return isValid(ptr) ? ptr : nullptr;
}

/*!
 * @brief  reload "new[]": ptr = new int[10]
 * @param  size
 * @retval none
 * @note   none
 */
void *operator new[](size_t size)
{
    void *ptr = kmalloc(size, GFP_KERNEL);
    return isValid(ptr) ? ptr : nullptr;
}

/*!
 * @brief  reload "delete": delete ptr
 * @param  ptr
 * @retval none
 * @note   none
 */
void operator delete(void *ptr)
{
    if (ptr)
        kfree(ptr);
}

/*!
 * @brief  reload "delete": delete ptr
 * @param  ptr
 * @retval none
 * @note   none
 */
void operator delete[](void *ptr)
{
    if (ptr)
        kfree(ptr);
}

/*!< ------------------------------------------------------------------- */
/*!
 * @brief  called by "main"
 * @param  none
 * @retval none
 * @note   none
 */
void iostream_init(void)
{
}

/*!< end of file */
