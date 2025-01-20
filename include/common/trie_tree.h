/*
 * Trie Tree Interface
 *
 * File Name:   trie_tree.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.05.04
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __TRIE_TREE_H
#define __TRIE_TREE_H

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The includes */
#include <common/basic_types.h>
#include <common/error_types.h>
#include <common/generic.h>

/*!< The defines */
#define MAX_TRIE_CHAR                               (95)

#ifndef MAX_TRIE_CHAR
#define MAX_TRIE_CHAR                               ((kuint32_t)('~' - ' ' + 1))
#endif

typedef struct trie_link {
    kuint32_t depth;

} srt_trie_link_t;

typedef struct trie_node {
    struct trie_node *sprt_parent;
    struct trie_node **sprt_branches;

    struct trie_link *sprt_link;

} srt_trie_node_t;

typedef struct trie_tree {
    kint32_t (*get) (kchar_t ch);
    void *(*alloc) (kusize_t size);
    void (*free) (void *ptr);

    kuint32_t size;
    struct trie_node sgrt_node;

} srt_trie_tree_t;

/*!< The functions */
extern kint32_t get_trie_node_branch(kchar_t ch);
extern struct trie_node **create_trie_branch(struct trie_tree *sprt_tree, struct trie_node *sprt_node, kuint32_t size);
extern struct trie_node *allocate_trie_node(struct trie_tree *sprt_tree, struct trie_node *sprt_par, struct trie_node **sprt_branches);
extern struct trie_node *find_trie_node(struct trie_tree *sprt_tree, const char *name);
extern struct trie_link *trie_tree_look_up(struct trie_tree *sprt_tree, const kchar_t *name);
extern void trie_node_add(struct trie_tree *sprt_tree, const kchar_t *name, struct trie_link *sprt_link);
extern void trie_node_del(struct trie_tree *sprt_tree, const kchar_t *name);

/*!< The defines */
#define DECLARE_TRIE_TREE(name, branch, max, get_func, alloc_func, free_func) \
    struct trie_tree name = {   \
        .get = get_func,    \
        .alloc = alloc_func,    \
        .free = free_func,  \
        .size = max, \
        .sgrt_node = { NULL, branch, 0, NULL },    \
    }

#define foreach_trie_tree(sprt_node, sprt_tree, offset) \
    for (sprt_node = &(sprt_tree)->sgrt_node, offset = 0; \
         sprt_node; \
         sprt_node = (sprt_node)->sprt_branches ? (sprt_node)->sprt_branches[offset] : NULL)

#define trie_tree_entry(tree, type, member, name)  \
({  \
    void *ptr_member = (void *)trie_tree_look_up(tree, name);  \
    ptr_member ? mrt_container_of(ptr_member, type, member) : NULL; \
})

#ifdef __cplusplus
    }
#endif

#endif /* __TRIE_TREE_H */
