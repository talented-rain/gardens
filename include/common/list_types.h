/*
 * List Defines (Singly List and Doubly List)
 *
 * File Name:   list_types.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.09.26
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __LIST_TYPES_H
#define __LIST_TYPES_H

/*!< The includes */
#include "generic.h"
#include <platform/fwk_mempool.h>

/*!< The defines */
/*!< Singly List */
struct list
{
	struct list *sprt_next;
};
typedef struct list srt_list_t;

#define LIST_INIT()	\
{	\
	.sprt_next = mrt_nullptr,	\
}

#define DECLARE_LIST(list)	\
	struct list list = LIST_INIT

#define DECLARE_LIST_PTR(list)	\
	struct list *list = mrt_nullptr

/*!< search */
#define foreach_list_next(head, tail, ptr_list, ptr_next)	\
	for (ptr_list = head; (ptr_list != tail); ptr_list = (ptr_list)->ptr_next)

#define foreach_list_from_head(head, tail, ptr_list)			foreach_list_next(head, tail, ptr_list, sprt_next)
#define foreach_list_from_next(head, tail, ptr_list)			foreach_list_next((head)->sprt_next, tail, ptr_list, sprt_next)

/*!< searching for "tail = head" */
#define foreach_list_even_head(head, ptr_list)					foreach_list_from_head(head, head, ptr_list)
#define foreach_list_even_next(head, ptr_list)					foreach_list_from_next(head, head, ptr_list)

/*!< searching for "tail = mrt_nullptr" */
#define foreach_list_odd(head, ptr_list, ptr_next)				foreach_list_next(head, mrt_nullptr, ptr_list, ptr_next)
#define foreach_list_odd_head(head, ptr_list)					foreach_list_from_head(head, mrt_nullptr, ptr_list)
#define foreach_list_odd_next(head, ptr_list)					foreach_list_from_next(head, mrt_nullptr, ptr_list)

#define mrt_list_add_tail(tail, list)							{ (list)->sprt_next = mrt_nullptr; (tail)->sprt_next = (list); }
#define mrt_list_add_head(head, list)							{ (list)->sprt_next = (head)->sprt_next; (head)->sprt_next = (list); }
#define mrt_list_del_any(prev, list)							{ (prev)->sprt_next = (list)->sprt_next; }

/*!< delete and free all nodes */
#define mrt_list_delete_all(head, prev, list)	\
{	\
	list = head;	\
	while (isValid(list))	\
	{	\
		prev	= list;	\
		list	= list->sprt_next;	\
		kfree(prev);	\
	}	\
}

/*!< Doubly List */
struct list_head
{
	struct list_head *sprt_prev;
	struct list_head *sprt_next;
};
typedef struct list_head srt_list_head_t;

#define LIST_HEAD_INIT(list)	\
{	\
	.sprt_prev = (list),	\
	.sprt_next = (list),	\
}

#define DECLARE_LIST_HEAD(list)	\
						struct list_head list = LIST_HEAD_INIT(&list)

/*!< list pointer defines */
#define DECLARE_LIST_HEAD_PTR_INIT(ptr_list, list)	\
						struct list_head *ptr_list = list;
#define DECLARE_LIST_HEAD_PTR(ptr_list)							DECLARE_LIST_HEAD_PTR_INIT(ptr_list, mrt_nullptr)

/*!< get every member from list */
#define mrt_next_list_head(head, list)							(list = ((list)->sprt_next != head) ? (list)->sprt_next : mrt_nullptr)

/*!< judge if the list has only head */
#define IS_LIST_HEAD_SELF_HEAD(ptr_list)	\
	(((ptr_list)->sprt_prev == (ptr_list)) && ((ptr_list)->sprt_next == (ptr_list)))
#define mrt_list_head_empty(head)								IS_LIST_HEAD_SELF_HEAD(head)

/*!< get parent of every member from list */
#define mrt_list_head_parent(parent, list, type, member)	\
({	\
	mrt_next_list_head(parent, list);	\
	(isValid(list)) ? mrt_container_of(list, type, member) : mrt_nullptr;	\
})

/*!< two-way retrieval */
#define foreach_list_head(ptr_left, ptr_right, ptr_head)	\
	for (ptr_right = (ptr_head)->sprt_next, ptr_left = (ptr_head)->sprt_prev;	\
		((ptr_right != ptr_head) && (ptr_left != ptr_head));	\
		ptr_right = (ptr_right)->sprt_next, ptr_left = (ptr_left)->sprt_prev)

/*!< search forward */
#define foreach_list_head_forward(ptr_list, ptr_head)	\
	for (ptr_list = (ptr_head)->sprt_next; ptr_list != ptr_head; ptr_list = (ptr_list)->sprt_next)

/*!< search backward */
#define foreach_list_head_backward(ptr_list, ptr_head)	\
	for (ptr_list = (ptr_head)->sprt_prev; ptr_list != ptr_head; ptr_list = (ptr_list)->sprt_prev)

#define mrt_list_first_entry(head, type, member)				mrt_container_of((head)->sprt_next, type, member)
#define mrt_list_last_entry(head, type, member)					mrt_container_of((head)->sprt_prev, type, member)
#define mrt_list_next_entry(pos, member)						mrt_container_of((pos)->member.sprt_next, typeof(*(pos)), member)
#define mrt_list_prev_entry(pos, member)						mrt_container_of((pos)->member.sprt_prev, typeof(*(pos)), member)

#define mrt_list_first_valid_entry(ptr, type, member)			(mrt_list_head_empty(ptr) ? mrt_nullptr : mrt_list_first_entry(ptr, type, member))

/*!<
 * usage: 
 * 	struct dev {
 * 		struct list_head list;
 * 	}
 * 
 *  struct devices {
 * 		struct list_head head;
 * 	}
 * 
 *  struct dev sgrt_dev;
 *  struct devices sgrt_devices;
 * 
 * 	===> list_head_add_tail(&sgrt_devices.head, &sgrt_dev.list);
 *  ===> struct dev *sprt_dev;
 *  ===> foreach_list_entry(sprt_dev, &sgrt_devices.head, list)
 */
#define foreach_list_next_entry(pos, head, member)	\
	for (pos = mrt_list_first_entry(head, typeof(*pos), member);	\
	     &(pos->member) != (head);	\
	     pos = mrt_list_next_entry(pos, member))

#define foreach_list_prev_entry(pos, head, member)	\
	for (pos = mrt_list_last_entry(head, typeof(*pos), member);	\
	     &(pos->member) != (head);	\
	     pos = mrt_list_prev_entry(pos, member))

/* get list and next list, and then delete current list from list_head */
#define foreach_list_next_entry_safe(pos, temp, head, member)	\
	for (pos = mrt_list_first_entry(head, typeof(*pos), member),	\
		temp = mrt_list_next_entry(pos, member);	\
	     &(pos->member) != (head);	\
	     pos = temp, temp = mrt_list_next_entry(temp, member))

/* get list and prev list, and then delete current list from list_head */
#define foreach_list_prev_entry_safe(pos, temp, head, member)	\
	for (pos = mrt_list_last_entry(head, typeof(*pos), member),	\
		temp = mrt_list_prev_entry(pos, member);	\
	     &(pos->member) != (head);	\
	     pos = temp, temp = mrt_list_prev_entry(temp, member))

/*!< API functions */
/*!
 * @brief   initialize list
 * @param   sprt_list
 * @retval  none
 * @note    none
 */
static inline void init_list_head(struct list_head *sprt_list)
{
	sprt_list->sprt_next = sprt_list;
	sprt_list->sprt_prev = sprt_list;
}

/*!
 * @brief   check if the member is existed in target list
 * @param   sprt_head, sprt_list
 * @retval  errno
 * @note    none
 */
static inline kint32_t list_head_for_each(struct list_head *sprt_head, struct list_head *sprt_list)
{
	struct list_head *ptr_right;
	struct list_head *ptr_left;

	foreach_list_head(ptr_left, ptr_right, sprt_head)
	{
		if ((ptr_left == sprt_list) || (ptr_right == sprt_list))
			return ER_NORMAL;

		if (ptr_left == ptr_right)
			break;
	}

	return -ER_ERROR;
}

/*!
 * @brief   insert a new member behind list head
 * @param   sprt_head, sprt_list
 * @retval  none
 * @note    none
 */
static inline void list_head_add_head(struct list_head *sprt_head, struct list_head *sprt_list)
{
	struct list_head *ptr_next = sprt_head->sprt_next;

	sprt_list->sprt_prev = sprt_head;
	sprt_head->sprt_next = sprt_list;

	sprt_list->sprt_next = ptr_next;
	ptr_next->sprt_prev	 = sprt_list;
}

/*!
 * @brief   insert a new member before list head
 * @param   sprt_head, sprt_list
 * @retval  none
 * @note    none
 */
static inline void list_head_add_tail(struct list_head *sprt_head, struct list_head *sprt_list)
{
	struct list_head *ptr_prev = sprt_head->sprt_prev;

	sprt_list->sprt_prev = ptr_prev;
	ptr_prev->sprt_next	 = sprt_list;

	sprt_list->sprt_next = sprt_head;
	sprt_head->sprt_prev = sprt_list;
}

/*!
 * @brief   delete a member behind list head
 * @param   sprt_head
 * @retval  none
 * @note    none
 */
static inline void list_head_del_head(struct list_head *sprt_head)
{
	struct list_head *ptr_next = sprt_head->sprt_next->sprt_next;

	sprt_head->sprt_next = ptr_next;
	ptr_next->sprt_prev	 = sprt_head;
}

/*!
 * @brief   delete a member behind list head
 * @param   sprt_head
 * @retval  none
 * @note    none
 */
static inline void list_head_del_tail(struct list_head *sprt_head)
{
	struct list_head *ptr_prev = sprt_head->sprt_prev->sprt_prev;

	sprt_head->sprt_prev = ptr_prev;
	ptr_prev->sprt_next	 = sprt_head;
}

/*!
 * @brief   delete a member
 * @param   sprt_list
 * @retval  none
 * @note    none
 */
static inline void list_head_del(struct list_head *sprt_list)
{
	struct list_head *ptr_prev = sprt_list->sprt_prev;
	struct list_head *ptr_next = sprt_list->sprt_next;

	ptr_prev->sprt_next	= ptr_next;
	ptr_next->sprt_prev	= ptr_prev;

	init_list_head(sprt_list);
}

/*!
 * @brief   delete a member which is in the middle of list
 * @param   sprt_list
 * @retval  none
 * @note    none
 */
static inline void list_head_del_safe(struct list_head *sprt_head, struct list_head *sprt_list)
{
	struct list_head *ptr_prev = sprt_list->sprt_prev;
	struct list_head *ptr_next = sprt_list->sprt_next;

	if (!list_head_for_each(sprt_head, sprt_list))
	{
		ptr_prev->sprt_next	= ptr_next;
		ptr_next->sprt_prev	= ptr_prev;

		init_list_head(sprt_list);
	}
}

#endif  /* __LIST_TYPES_H */
