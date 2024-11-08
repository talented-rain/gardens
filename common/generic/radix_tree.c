/*
 * Radix Tree General Function
 *
 * File Name:   radix_tree.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.05.06
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <common/generic.h>
#include <common/radix_tree.h>
#include <common/io_stream.h>
#include <platform/fwk_mempool.h>

/*!< The defines */

/*!< API functions */
/*!
 * @brief   get the number of valid bits (bit = 1)
 * @param   number
 * @retval  none
 * @note    for example: number = 0x1010 (16bits), valid bits = 2; zero bits = 30
 */
static kuint16_t radix_tree_numlen(kuint32_t number)
{
    kint16_t i;
    kuint16_t result, count = 0;

    /*!< get the numer of zero-bit from high to low */
    for (i = 31; i >= 0; i--) 
    {
        if (number & (1UL << i))
            break;

        count++;
    }

    count = 32 - count;
    if (count <= MAX_BRANCH)
        return MAX_BRANCH;

    /*!< if it is the n-power of 2, such as 1, 2, 4, 8, ... */
    if (isPower2(MAX_BRANCH))
        return mrt_align(count, MAX_BRANCH);
    
    result = udiv_remainder(count, MAX_BRANCH);
    return result ? (count + MAX_BRANCH - result) : count;
}

/*!
 * @brief   update number
 * @param   number
 * @retval  none
 * @note    none
 */
static kuint32_t radix_tree_advance(kuint32_t number)
{
    return (number >> MAX_BRANCH);
}

/*!
 * @brief   get the number of groups
 * @param   number
 * @retval  none
 * @note    for example: number = 0x1010 (16bits), MAX_BRANCH = 2, the groups = 16 / 2 = 8
 */
kuint16_t get_radix_node_branch(kuint32_t number)
{
    return (number & ((1 << MAX_BRANCH) - 1));
}

/*!
 * @brief   allocate one radix_node
 * @param   sprt_tree, sprt_par
 * @retval  none
 * @note    none
 */
struct radix_node *allocate_radix_node(struct radix_tree *sprt_tree, struct radix_node *sprt_par)
{
    struct radix_node *sprt_node;
    kuint32_t i;

    sprt_node = (struct radix_node *)sprt_tree->alloc(sizeof(*sprt_node));
    if (!isValid(sprt_node))
        return mrt_nullptr;
    
    sprt_node->sprt_parent = sprt_par;
    sprt_node->sprt_link = mrt_nullptr;

    for (i = 0; i < (1 << MAX_BRANCH); i++)
        sprt_node->sgrt_branches[i] = mrt_nullptr;

    return sprt_node;
}

/*!
 * @brief   find a radix_node in sprt_tree
 * @param   sprt_tree, number
 * @retval  none
 * @note    none
 */
struct radix_node *find_radix_node(struct radix_tree *sprt_tree, kuint32_t number)
{
    struct radix_node *sprt_node;
    kuint32_t value = number;
    kuint16_t lenth = radix_tree_numlen(number);
    kuint16_t i = 0, offset = 0;

    foreach_radix_tree(sprt_node, sprt_tree, offset)
    {
        if (i >= lenth)
            break;

        i += MAX_BRANCH;
        offset = sprt_tree->get(value);
        value = radix_tree_advance(value);
    }

    if (sprt_node)
        return sprt_node->sprt_link ? sprt_node : mrt_nullptr;

//  print_debug("find node failed, index is: 0x%x\n", number);

    return mrt_nullptr;
}

/*!
 * @brief   find a radix_node in sprt_tree
 * @param   sprt_tree, number
 * @retval  none
 * @note    none
 */
struct radix_link *radix_tree_look_up(struct radix_tree *sprt_tree, kuint32_t number)
{
    struct radix_node *sprt_node;

    sprt_node = find_radix_node(sprt_tree, number);
    if (!sprt_node)
        return mrt_nullptr;

    return sprt_node->sprt_link;
}

/*!
 * @brief   add a new radix_node to sprt_tree
 * @param   sprt_tree, number
 * @retval  none
 * @note    none
 */
void radix_tree_add(struct radix_tree *sprt_tree, kuint32_t number, struct radix_link *sprt_link)
{
    struct radix_node *sprt_node, *sprt_temp;
    kuint32_t value = number;
    kuint16_t lenth = radix_tree_numlen(number);
    kuint16_t i, offset = 0;

    if (!sprt_link)
        return;

    if (find_radix_node(sprt_tree, number))
        return;

    for (i = 0, sprt_node = &sprt_tree->sgrt_node; i < lenth; i += MAX_BRANCH) 
    {
        offset = sprt_tree->get(value);
        value = radix_tree_advance(value);

        if (!sprt_node->sgrt_branches[offset]) 
        {
            sprt_temp = allocate_radix_node(sprt_tree, sprt_node);
            if (!isValid(sprt_temp))
                return;

            sprt_node->sgrt_branches[offset] = sprt_temp;
        }

//      print_debug("%s: lenth: %d, i = %d, sprt_node: %p, offset: 0x%x\n", __FUNCTION__, lenth, i, sprt_node->sgrt_branches[offset], offset);

        sprt_link->depth = i;
        sprt_node = sprt_node->sgrt_branches[offset];
    }

    if (sprt_node != &sprt_tree->sgrt_node)
        sprt_node->sprt_link = sprt_link;

//  print_debug("add new node successfully, index is: 0x%x\n", number);
}

/*!
 * @brief   delete radix_node from sprt_tree with recursion
 * @param   sprt_tree, number
 * @retval  none
 * @note    none
 */
static void __del_radix_node(struct radix_tree *sprt_tree, struct radix_node *sprt_node, struct radix_node *sprt_child)
{
    kuint32_t i, count = 0;
    
    if (!sprt_node)
        return;

    for (i = 0; i < (1 << MAX_BRANCH); i++) 
    {
        if (sprt_node->sgrt_branches[i] == sprt_child) 
        {
            sprt_node->sgrt_branches[i] = mrt_nullptr;

            if (count)
                goto out;
        }

        if (sprt_node->sgrt_branches[i])
            count++;
    }

    if (count)
        goto out;

    if (!sprt_node->sprt_link) 
    {
        __del_radix_node(sprt_tree, sprt_node->sprt_parent, sprt_node);

        if (sprt_node != &sprt_tree->sgrt_node)
            sprt_tree->free(sprt_node);
    }

out:
    return;
}

/*!
 * @brief   delete a group of radix_nodes from sprt_tree
 * @param   sprt_tree, number
 * @retval  none
 * @note    none
 */
void radix_tree_del(struct radix_tree *sprt_tree, kuint32_t number)
{
    struct radix_node *sprt_node;
    kuint32_t value = number;
    kuint32_t lenth = radix_tree_numlen(number);
    kuint32_t i, offset = 0;

    for (i = 0, sprt_node = &sprt_tree->sgrt_node; i < lenth; i += MAX_BRANCH) 
    {
        if (!sprt_node)
            return;
        
        offset = sprt_tree->get(value);
        value = radix_tree_advance(value);
        sprt_node = sprt_node->sgrt_branches[offset];
    }

    if (!sprt_node ||
        !sprt_node->sprt_link ||
        (sprt_node == &sprt_tree->sgrt_node)) 
    {
        print_debug("nothing needs to delete\n");
        return;
    }

    sprt_node->sprt_link = mrt_nullptr;
    __del_radix_node(sprt_tree, sprt_node, mrt_nullptr);
}


/*!< end of file */
