/*
 * Platform Object Defines
 *
 * File Name:   fwk_kboj.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.05.18
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_KOBJ_H_
#define __FWK_KOBJ_H_

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The includes */
#include <platform/fwk_basic.h>
#include <kernel/mutex.h>
#include <kernel/spinlock.h>

/*!< The defines */
struct fwk_inode;

struct fwk_kobject
{
	kchar_t *name;
	struct atomic sgrt_ref;

	struct list_head sgrt_link;
	struct fwk_kobject *sprt_parent;
	struct fwk_kset *sprt_kset;
	struct fwk_inode *sprt_inode;

	kbool_t is_dir;
	kbool_t is_disk;
	
	struct spin_lock sgrt_lock;
};

struct fwk_kset
{
	struct fwk_kobject sgrt_kobj;
	struct list_head sgrt_list;
};

#define mrt_fwk_kset_get(sprt_kobj)	\
	(((sprt_kobj) && (sprt_kobj)->is_dir) ? mrt_container_of(sprt_kobj, struct fwk_kset, sgrt_kobj) : mrt_nullptr)

struct fwk_probes
{
	kuint32_t devNum;
	kuint32_t range;

	void *data;
	struct fwk_probes *sprt_next;
};

struct fwk_kobj_map
{
	struct mutex_lock sgrt_mutex;
	struct fwk_probes *sprt_probes[DEVICE_MAX_NUM];
};

/*!< The globals */
extern struct fwk_kobj_map *sprt_fwk_chrdev_map;
extern struct fwk_kobj_map *sprt_fwk_blkdev_map;

/*!< The functions */
/*!< -------------------------------------------------------------- */
extern kint32_t fwk_kobjmap_init(void);
extern void fwk_kobjmap_del(void);

extern kint32_t fwk_kobj_map(struct fwk_kobj_map *sprt_domain, kuint32_t devNum, kuint32_t range, void *data);
extern kint32_t fwk_kobj_unmap(struct fwk_kobj_map *sprt_domain, kuint32_t devNum, kuint32_t range);
extern void *fwk_kobjmap_lookup(struct fwk_kobj_map *sprt_domain, kuint32_t devNum);

/*!< -------------------------------------------------------------- */
extern kint32_t fwk_kobject_root_init(void);

extern void fwk_kobject_init(struct fwk_kobject *sprt_kobj);
extern struct fwk_kobject *fwk_kobject_create(void);
extern void fwk_kobject_del(struct fwk_kobject *sprt_kobj);
extern void fwk_kobject_destroy(struct fwk_kobject *sprt_kobj);
extern kint32_t fwk_kobject_add(struct fwk_kobject *sprt_kobj, struct fwk_kobject *sprt_parent, const kchar_t *fmt, ...);
extern kint32_t fwk_kobject_add_vargs(struct fwk_kobject *sprt_kobj, struct fwk_kobject *sprt_parent, const kchar_t *fmt, va_list sprt_list);
extern struct fwk_kobject *fwk_kobject_populate(struct fwk_kobject *sprt_head, const kchar_t *name);
extern struct fwk_kobject *fwk_find_kobject_by_path(struct fwk_kobject *sprt_head, const kchar_t *name);
extern kint32_t fwk_kobject_set_name(struct fwk_kobject *sprt_kobj, const kchar_t *fmt, ...);
extern kint32_t fwk_kobject_set_name_args(struct fwk_kobject *sprt_kobj, const kchar_t *fmt, va_list sprt_list);
extern kint32_t fwk_kobject_rename(struct fwk_kobject *sprt_kobj, const kchar_t *fmt, ...);
extern void fwk_kobject_del_name(struct fwk_kobject *sprt_kobj);
extern kchar_t *fwk_kobject_get_name(struct fwk_kobject *sprt_kobj);
extern struct fwk_kobject *fwk_kobject_get(struct fwk_kobject *sprt_kobj);
extern void fwk_kobject_put(struct fwk_kobject *sprt_kobj);
extern kbool_t fwk_kobject_is_referrd(struct fwk_kobject *sprt_kobj);

extern void fwk_kset_init(struct fwk_kset *sprt_kset);
extern struct fwk_kset *fwk_kset_create(const kchar_t *name, struct fwk_kobject *sprt_parent);
extern kint32_t fwk_kset_register(struct fwk_kset *sprt_kset);
extern struct fwk_kset *fwk_kset_create_and_register(const kchar_t *name, struct fwk_kobject *sprt_parent);
extern void fwk_kset_unregister(struct fwk_kset *sprt_kset);
extern void fwk_kset_kobject_remove(struct fwk_kobject *sprt_kobj);
extern struct fwk_kset *fwk_kset_get_root(void);

/*!< API functions */
/*!
 * @brief   initial kref
 * @param   sprt_kref
 * @retval  none
 * @note    none
 */
static inline void fwk_kref_init(struct atomic *sprt_kref)
{
	ATOMIC_SET(sprt_kref, 0);
}

/*!
 * @brief   increase kref
 * @param   sprt_kref
 * @retval  none
 * @note    none
 */
static inline void fwk_kref_get(struct atomic *sprt_kref)
{
	atomic_inc(sprt_kref);
}

/*!
 * @brief   decrease kref
 * @param   sprt_kref
 * @retval  none
 * @note    none
 */
static inline void fwk_kref_put(struct atomic *sprt_kref)
{
	ATOMIC_READ(sprt_kref) ? atomic_dec(sprt_kref) : (void)0;
}

/*!
 * @brief   check if kref is the smallest
 * @param   sprt_kref
 * @retval  none
 * @note    none
 */
static inline kbool_t fwk_kref_is_zero(struct atomic *sprt_kref)
{
	return !!ATOMIC_READ(sprt_kref);
}

#ifdef __cplusplus
    }
#endif

#endif /*!< __FWK_KOBJ_H_ */
