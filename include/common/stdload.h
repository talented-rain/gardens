/*
 * C++ Standard Lib Reload
 *
 * File Name:   stdload.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2025.01.07
 *
 * Copyright (c) 2025   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __STDLOAD_H
#define __STDLOAD_H

/*!< The globals */
#include <common/generic.h>
#include <platform/fwk_basic.h>

/*!< The defines */
#define BEGIN_NAMESPACE(name)                       namespace name {
#define END_NAMESPACE(name)                         }

class string {
public:
    string() : real_size(-1), lenth(1024), isdync(true)
    {
        buffer = new kuint8_t[lenth];
    }

    string(kuint8_t *buf, kssize_t size)
        : real_size(-1)
        , buffer(buf)
        , lenth(size)
        , isdync(false)
    {}

    ~string() 
    {
        if (isdync && buffer)
            delete[] buffer;
    }

    kuint8_t *get_buf(void)
    {
        return buffer;
    }

    kssize_t get_length(void)
    {
        return lenth;
    }

    kssize_t real_size;

private:
    kuint8_t *buffer;
    kssize_t lenth;
    kbool_t isdync;
};

/*!< ------------------------------------------------------------- */
BEGIN_NAMESPACE(stream)
/*!< ------------------------------------------------------------- */

static inline void endl(void)
{
    io_putc('\n');
}

class ostream {
public:
    ostream() {}
    ~ostream() {}

    ostream &operator<<(const kchar_t ch)
    {
        io_putc((const kubyte_t)ch);        
        return *this;
    }

    ostream &operator<<(const kchar_t *str)
    {
        io_putstr((const kubyte_t *)str, strlen(str));
        return *this;
    }

    ostream &operator<<(const kubyte_t *str)
    {
        io_putstr(str, strlen((const kchar_t *)str));
        return *this;
    }

    ostream &operator<<(kint32_t i)
    {
        printk("%d", i);
        return *this;
    }

    ostream &operator<<(kuint32_t i)
    {
        printk("%d", i);
        return *this;
    }

    ostream &operator<<(void (*fn)(void))
    {
        fn();
        return *this;
    }
};

class istream {
public:
    istream() {}
    ~istream() {}

    istream &operator>>(kchar_t &ch)
    {
        io_getc((kubyte_t *)&ch);        
        return *this;
    }

    istream &operator>>(kubyte_t &ch)
    {
        io_getc(&ch);        
        return *this;
    }

    istream &operator>>(string &str)
    {
        if (str.get_buf())
            str.real_size = io_getstr(str.get_buf(), str.get_length());
        
        return *this;
    }

    istream &operator>>(void (*fn)(void))
    {
        fn();
        return *this;
    }
};

extern istream cin;
extern ostream cout;
extern ostream cerr;

/*!< ------------------------------------------------------------- */
END_NAMESPACE(stream)
/*!< ------------------------------------------------------------- */

#endif

/*!< end of file */
