/*
 * User Thread Instance Tables
 *
 * File Name:   thread_table.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.04.02
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include "task.h"
#include "test/test_task.h"
#include "demo/demo_task.h"

using namespace tsk;

/*!< The defines */
typedef kint32_t (*thread_init_t)(void);

class proc_table 
{
public:
    proc_table() {}
    ~proc_table() {}

    static const thread_init_t g_test_tables[];
    static const thread_init_t g_demo_tables[];
    
    static void init_thread_table(const thread_init_t tables[]);
};

/*!< The globals */
const thread_init_t proc_table::g_test_tables[] =
{
//  console_task_init,
    light_task_init,
    button_task_init,
//  display_task_init,
    tsc_task_init,
    env_monitor_init,

    test_task_init,
    
    /*!< end */
    mrt_nullptr,
};

const thread_init_t proc_table::g_demo_tables[] =
{   
    /*!< applications */
    lvgl_task_init,
    lwip_task_init,
    
    /*!< end */
    mrt_nullptr,
};

/*!< API functions */
/*!
 * @brief	constructor
 * @param  	none
 * @retval 	none
 * @note   	task create
 */
crt_task_t::crt_task_t(const kchar_t *name, void *(*task_entry)(void *), 
            kuint8_t *stack, kuint32_t size, kuint32_t prio, kuint32_t tslice)
        : tid(-1)
        , stack_base(stack)
        , stack_size(size)
        , isdync(false)
{
    kint32_t retval;

    if (!name || !(*name) || !task_entry)
        return;

    this->sgrt_attr.detachstate = THREAD_CREATE_JOINABLE;
    this->sgrt_attr.inheritsched = THREAD_INHERIT_SCHED;
    this->sgrt_attr.schedpolicy = THREAD_SCHED_FIFO;

    if (!stack) {
        kuint8_t *ptr = new kuint8_t[stack_size];
        if (!ptr)
            return;

        this->isdync = true;
        this->stack_base = ptr;
    }

    /*!< thread stack */
    thread_set_stack(&this->sgrt_attr, mrt_nullptr, this->stack_base, this->stack_size);

    /*!< lowest priority */
    thread_set_priority(&this->sgrt_attr, prio);
    /*!< default time slice */
    thread_set_time_slice(&this->sgrt_attr, tslice);

    /*!< register thread */
    retval = thread_create(&this->tid, &sgrt_attr, task_entry, (void *)this);
    if (retval) {
        if (this->isdync)
            delete[] this->stack_base;

        return;
    }

    thread_set_name(this->tid, name);
    stream::cout << "create new task: " << name << stream::endl;
}

/*!
 * @brief	destructor
 * @param  	none
 * @retval 	none
 * @note   	task delete
 */
crt_task_t::~crt_task_t()
{
    /*!< unregister thread */

    /*!< clean attr */
    thread_attr_destroy(&this->sgrt_attr);

    if (this->isdync)
        delete[] this->stack_base;

    this->tid = -1;
    this->stack_base = mrt_nullptr;
    this->stack_size = 0;
    this->isdync = false;
}

/*!
 * @brief	init thread tables
 * @param  	none
 * @retval 	none
 * @note   	only for user thread; called by init_proc
 */
void proc_table::init_thread_table(const thread_init_t tables[])
{
    const thread_init_t *per_entry;
    
    for (per_entry = tables; *per_entry; per_entry++)
    {
        /*!< ignore returns error */
        (*per_entry)();
    }
}

/*!
 * @brief	main
 * @param  	none
 * @retval 	none
 * @note   	create all application task
 */
kint32_t main(kint32_t argc, kchar_t **argv)
{
    proc_table::init_thread_table(proc_table::g_test_tables);
    proc_table::init_thread_table(proc_table::g_demo_tables);

    return 0;
}

/*!< end of file */
