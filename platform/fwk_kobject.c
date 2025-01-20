/*
 * Platform Object Defines
 *
 * File Name:   fwk_kboject.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.06.10
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <platform/fwk_kobj.h>
#include <platform/fwk_inode.h>
#include <kernel/spinlock.h>

/*!< The globals */
static struct fwk_kset sgrt_fwk_kset_root;
static kbool_t is_fwk_root_existed = false;

/*!< API function */
/*!
 * @brief   join the new kobject to kset
 * @param   sprt_kobj
 * @retval  errno
 * @note    none
 */
static kint32_t fwk_kobject_join_to_kset(struct fwk_kobject *sprt_kobj)
{
    struct fwk_kobject *sprt_parent;
    struct fwk_kobject *sprt_each;
    kint32_t retval;

    if (!sprt_kobj->sprt_kset)
        return -ER_FAULT;

    spin_lock(&sprt_kobj->sgrt_lock);
    
    foreach_list_next_entry(sprt_each, &sprt_kobj->sprt_kset->sgrt_list, sgrt_link)
    {
        if (!sprt_each->name && !sprt_kobj->name)
        {
            retval = -ER_EMPTY;
            goto fail;
        }

        if (!strcmp(sprt_kobj->name, sprt_each->name))
        {
            retval = -ER_EXISTED;
            goto fail;
        }
    }
    
    sprt_parent = sprt_kobj->sprt_parent;
    if (!sprt_parent)
        sprt_parent = &sprt_kobj->sprt_kset->sgrt_kobj;
    
    list_head_add_tail(&sprt_kobj->sprt_kset->sgrt_list, &sprt_kobj->sgrt_link);
    sprt_kobj->sprt_parent = sprt_parent;
    spin_unlock(&sprt_kobj->sgrt_lock);

    return ER_NORMAL;

fail:
    spin_unlock(&sprt_kobj->sgrt_lock);
    return retval;
}

/*!
 * @brief   delete the kobject from kset
 * @param   sprt_kobj
 * @retval  none
 * @note    none
 */
static void fwk_kobject_detach_from_kset(struct fwk_kobject *sprt_kobj)
{
    if (!sprt_kobj->sprt_kset)
        return;

    spin_lock(&sprt_kobj->sgrt_lock);
    list_head_del_safe(&sprt_kobj->sprt_kset->sgrt_list, &sprt_kobj->sgrt_link);
    spin_unlock(&sprt_kobj->sgrt_lock);
}

/*!
 * @brief   join the new kobject to kset, and create inode
 * @param   sprt_kobj
 * @retval  errno
 * @note    none
 */
static kint32_t fwk_kobject_build_inode(struct fwk_kobject *sprt_kobj)
{
    struct fwk_inode *sprt_inode;
    kint32_t retval;

    retval = fwk_kobject_join_to_kset(sprt_kobj);
    if (retval)
        return retval;

    sprt_inode = fwk_mk_inode(sprt_kobj, NR_TYPE_NONE, -1);
    if (!isValid(sprt_inode))
    {
        fwk_kobject_detach_from_kset(sprt_kobj);
        return PTR_ERR(sprt_inode);
    }

    sprt_kobj->sprt_inode = sprt_inode;

    return ER_NORMAL;
}

/*!
 * @brief   initialize kobject
 * @param   sprt_kobj
 * @retval  none
 * @note    none
 */
void fwk_kobject_init(struct fwk_kobject *sprt_kobj)
{
    fwk_kref_init(&sprt_kobj->sgrt_ref);
    init_list_head(&sprt_kobj->sgrt_link);
    spin_lock_init(&sprt_kobj->sgrt_lock);
    sprt_kobj->is_dir = false;
    sprt_kobj->sprt_kset = mrt_nullptr;
}

/*!
 * @brief   create a new kobject
 * @param   none
 * @retval  kobject
 * @note    none
 */
struct fwk_kobject *fwk_kobject_create(void)
{
    struct fwk_kobject *sprt_kobj;

    sprt_kobj = kzalloc(sizeof(*sprt_kobj), GFP_KERNEL);
    if (!isValid(sprt_kobj))
        return mrt_nullptr;

    fwk_kobject_init(sprt_kobj);

    return sprt_kobj;
}

/*!
 * @brief   add a new kobject
 * @param   sprt_kobj, sprt_parent, fmt (name)
 * @retval  errno
 * @note    none
 */
kint32_t fwk_kobject_add(struct fwk_kobject *sprt_kobj, struct fwk_kobject *sprt_parent, const kchar_t *fmt, ...)
{
    va_list sprt_list;
    kint32_t retval;

    if (!sprt_kobj || !is_fwk_root_existed)
        return -ER_NOMEM;

    sprt_kobj->sprt_parent = sprt_parent;

    va_start(sprt_list, fmt);
    fwk_kobject_set_name_args(sprt_kobj, fmt, sprt_list);
    va_end(sprt_list);

    if (!sprt_kobj->sprt_kset)
        sprt_kobj->sprt_kset = &sgrt_fwk_kset_root;

    retval = fwk_kobject_build_inode(sprt_kobj);
    if (retval)
        fwk_kobject_del_name(sprt_kobj);

    return retval;
}

/*!
 * @brief   add a new kobject
 * @param   sprt_kobj, sprt_parent, fmt (unformatted name)
 * @retval  errno
 * @note    none
 */
kint32_t fwk_kobject_add_vargs(struct fwk_kobject *sprt_kobj, struct fwk_kobject *sprt_parent, const kchar_t *fmt, va_list sprt_list)
{
    kint32_t retval;

    if (!sprt_kobj || !is_fwk_root_existed)
        return -ER_NOMEM;

    sprt_kobj->sprt_parent = sprt_parent;
    fwk_kobject_set_name_args(sprt_kobj, fmt, sprt_list);

    if (!sprt_kobj->sprt_kset)
        sprt_kobj->sprt_kset = &sgrt_fwk_kset_root;

    retval = fwk_kobject_build_inode(sprt_kobj);
    if (retval)
        fwk_kobject_del_name(sprt_kobj);

    return retval;
}

/*!
 * @brief   delete kobject
 * @param   sprt_kobj
 * @retval  none
 * @note    none
 */
void fwk_kobject_del(struct fwk_kobject *sprt_kobj)
{
    fwk_rm_inode(sprt_kobj->sprt_inode);
    fwk_kobject_del_name(sprt_kobj);
    fwk_kobject_detach_from_kset(sprt_kobj);

    sprt_kobj->sprt_inode = mrt_nullptr;
}

/*!
 * @brief   release kobject
 * @param   sprt_kobj
 * @retval  none
 * @note    none
 */
void fwk_kobject_destroy(struct fwk_kobject *sprt_kobj)
{
    fwk_kobject_del(sprt_kobj);
    kfree(sprt_kobj);
}

/*!
 * @brief   create directory or file
 * @param   sprt_parent, name
 * @retval  kobject created
 * @note    dir: the format of name is "xxx/xxx", such as "dev/"; file: '/' should be moved, such as "dev"
 */
static struct fwk_kobject *__fwk_kobject_populate_dir(struct fwk_kobject *sprt_parent, const kchar_t *name)
{
    struct fwk_kset *sprt_kset;
    struct fwk_kobject *sprt_kobj;
    kchar_t *new_name;
    kuint32_t lenth;
    
    if (!sprt_parent)
        sprt_parent = &sgrt_fwk_kset_root.sgrt_kobj;
    new_name = kstrchr(name, '/');
    
    if (new_name)
    {
        sprt_kset = fwk_kset_create(name, sprt_parent);
        if (!isValid(sprt_kset))
            goto fail;
        
        sprt_kobj = &sprt_kset->sgrt_kobj;

        lenth = (kuint32_t)(new_name - name);
        fwk_kobject_del_name(sprt_kobj);
        
        new_name = kmalloc(lenth + 1, GFP_KERNEL);
        if (!isValid(new_name))
            goto dir_fail;
        
        kstrlcpy(new_name, name, lenth + 1);
        sprt_kobj->name = new_name;

        sprt_kobj->sprt_kset = mrt_fwk_kset_get(sprt_parent);
        if (!sprt_kobj->sprt_kset || fwk_kset_register(sprt_kset))
            goto dir_fail;
        
        goto pass;
        
dir_fail:
        fwk_kobject_del(sprt_kobj);
        kfree(sprt_kset);
        goto fail;
    }
    
    sprt_kobj = fwk_kobject_create();
    if (!isValid(sprt_kobj))
        goto fail;
    
    sprt_kobj->sprt_kset = mrt_fwk_kset_get(sprt_parent);
    if (!sprt_kobj->sprt_kset || fwk_kobject_add(sprt_kobj, sprt_parent, name))
    {
        kfree(sprt_kobj);
        goto fail;
    }
    
pass:
    return sprt_kobj;
    
fail:
    return mrt_nullptr;
}

/*!
 * @brief   populate directory or file
 * @param   sprt_head, name
 * @retval  kobject created
 * @note    dir: the format of name is "xxx/xxx", such as "dev/"; file: '/' should be moved, such as "dev"
 */
struct fwk_kobject *fwk_kobject_populate(struct fwk_kobject *sprt_head, const kchar_t *name)
{
    kchar_t *str_start, *str_end;
    kusize_t lenth;
    struct fwk_kobject *sprt_kobj;
    struct fwk_kset *sprt_kset;
    kbool_t found, is_root;

    if (!is_fwk_root_existed)
        return ERR_PTR(-ER_FORBID);

    sprt_kobj = sprt_head;
    sprt_kset = sprt_kobj ? mrt_fwk_kset_get(sprt_kobj) : &sgrt_fwk_kset_root;
    if (!sprt_kset)
        return ERR_PTR(-ER_NOMEM);

    is_root = (sprt_kset == (&sgrt_fwk_kset_root));
    str_start = str_end = (kchar_t *)name;

    if (is_root)
    {
        if (*str_start != '/')
            return ERR_PTR(-ER_FAULT);

        if (*(str_start + 1) == '\0')
            return ERR_PTR(-ER_FAULT);
    }

    /*!< for example: 123/yyx.txt, or 123/, or /123/, ... */
    while (str_end && (*str_end != '\0'))
    {
        str_start = is_root ? (str_end + 1) : str_end;
        if (*str_start == '\0')
            return sprt_kobj;
        
        /*!< if *(str_end + 1) == '/', the format of name maybe "//" */
        if (*str_start == '/')
            return ERR_PTR(-ER_FAULT);

        /*!< if '/' can be found, str_start is a directory */
        str_end = kstrchr(str_start, '/');
        is_root = true;
        found = false;
        lenth = str_end ? (kusize_t)(str_end - str_start) : kstrlen(str_start);

        spin_lock(&sprt_kset->sgrt_kobj.sgrt_lock);
        foreach_list_next_entry(sprt_kobj, &sprt_kset->sgrt_list, sgrt_link)
        {
            if (!sprt_kobj->name)
                continue;

            if (!strncmp(str_start, sprt_kobj->name, lenth))
            {
                /*!< str_end ? directory : file; if file is existed, file is repeated */
                if (!str_end)
                {
                    if (!sprt_kobj->is_dir)
                    {
                        spin_unlock(&sprt_kset->sgrt_kobj.sgrt_lock);
                        return ERR_PTR(-ER_EXISTED);
                    }
                    
                    continue;
                }
                
                /*!< otherwise, it is a directory */
                /*!< it is possible that file and directory have the same name */
                if (!sprt_kobj->is_dir)
                    continue;

                spin_unlock(&sprt_kset->sgrt_kobj.sgrt_lock);
                
                found = true;
                sprt_kset = mrt_fwk_kset_get(sprt_kobj);
                goto out;
            }
        }

        spin_unlock(&sprt_kset->sgrt_kobj.sgrt_lock);

out:
        if (!found)
        {
            sprt_kobj = __fwk_kobject_populate_dir(&sprt_kset->sgrt_kobj, str_start);
            if (!isValid(sprt_kobj))
                goto fail;
            
            sprt_kset = mrt_fwk_kset_get(sprt_kobj);
        }
    }
    
    return sprt_kobj;

fail:
    return ERR_PTR(-ER_ERROR);
}

/*!
 * @brief   find kobject by directory path (name)
 * @param   sprt_head, name
 * @retval  kobject found
 * @note    dir: the format of name is "xxx/xxx", such as "dev/"; file: '/' should be moved, such as "dev"
 */
struct fwk_kobject *fwk_find_kobject_by_path(struct fwk_kobject *sprt_head, const kchar_t *name)
{
    kchar_t *str_start, *str_end;
    kusize_t lenth;
    struct fwk_kobject *sprt_kobj;
    struct fwk_kset *sprt_kset;
    kbool_t found, is_root;

    sprt_kset = sprt_head ? mrt_fwk_kset_get(sprt_head) : &sgrt_fwk_kset_root;
    if (!sprt_kset || !is_fwk_root_existed)
        return mrt_nullptr;

    is_root = (sprt_kset == (&sgrt_fwk_kset_root));
    str_start = str_end = (kchar_t *)name;

    if (is_root)
    {
        if (*str_start != '/')
            return mrt_nullptr;

        if (*(str_start + 1) == '\0')
            return &sprt_kset->sgrt_kobj;
    }

    /*!< for example: 123/yyx.txt, or 123/, or /123/, ... */
    while (str_end && (*str_end != '\0'))
    {
        /*!< (*str_end) must be '/', if *(str_end + 1) is also '/', error occurred */
        str_start = is_root ? (str_end + 1) : str_end;
        if (*str_start == '\0')
            return &sprt_kset->sgrt_kobj;
        
        /*!< if *(str_end + 1) == '/', the format of name maybe "//" */
        if (*str_start == '/')
            break;

        /*!< if '/' can be found, str_start is a directory */
        str_end = kstrchr(str_start, '/');
        is_root = true;
        found = false;
        lenth = str_end ? (kusize_t)(str_end - str_start) : kstrlen(str_start);

        spin_lock(&sprt_kset->sgrt_kobj.sgrt_lock);

        foreach_list_next_entry(sprt_kobj, &sprt_kset->sgrt_list, sgrt_link)
        {
            if (!sprt_kobj->name)
                continue;

            if (!strncmp(str_start, sprt_kobj->name, lenth))
            {               
                /*!< str_end ? directory : file; if file is existed, file is repeated */
                if (!str_end)
                {
                    if (!sprt_kobj->is_dir)
                    {
                        spin_unlock(&sprt_kset->sgrt_kobj.sgrt_lock);
                        return sprt_kobj;
                    }
                    
                    continue;
                }
                
                /*!< otherwise, it is a directory */
                /*!< it is possible that file and directory have the same name */
                if (!sprt_kobj->is_dir)
                    continue;

                spin_unlock(&sprt_kset->sgrt_kobj.sgrt_lock);
                
                found = true;
                sprt_kset = mrt_fwk_kset_get(sprt_kobj);
                goto out;
            }
        }

        spin_unlock(&sprt_kset->sgrt_kobj.sgrt_lock);

out:
        if (!found || !sprt_kset)
            break;
    }

    return mrt_nullptr;
}

/*!
 * @brief   set kobject's name (unformatted)
 * @param   sprt_kobj, fmt
 * @retval  errno
 * @note    none
 */
kint32_t fwk_kobject_set_name_args(struct fwk_kobject *sprt_kobj, const kchar_t *fmt, va_list sprt_list)
{
    kchar_t *ptr;
    
    /*!< name is already defined */
    if (sprt_kobj->name && !fmt)
        return ER_NORMAL;

    ptr = vasprintk_safe(fmt, mrt_nullptr, sprt_list);
    if (!isValid(ptr))
        return -ER_NOMEM;

    if (sprt_kobj->name)
        kfree(sprt_kobj->name);

    sprt_kobj->name = ptr;
    return ER_NORMAL;
}

/*!
 * @brief   set kobject's name (unformatted)
 * @param   sprt_kobj, fmt
 * @retval  errno
 * @note    none
 */
kint32_t fwk_kobject_set_name(struct fwk_kobject *sprt_kobj, const kchar_t *fmt, ...)
{
    va_list sprt_list;
    kint32_t retval;

    va_start(sprt_list, fmt);
    retval = fwk_kobject_set_name_args(sprt_kobj, fmt, sprt_list);
    va_end(sprt_list);

    return retval;
}

/*!
 * @brief   re-set kobject's name (unformatted)
 * @param   sprt_kobj, fmt
 * @retval  errno
 * @note    none
 */
kint32_t fwk_kobject_rename(struct fwk_kobject *sprt_kobj, const kchar_t *fmt, ...)
{
    va_list sprt_list;
    kint32_t retval;

    va_start(sprt_list, fmt);
    retval = fwk_kobject_set_name_args(sprt_kobj, fmt, sprt_list);
    va_end(sprt_list);

    return retval;
}

/*!
 * @brief   delete kobject's name
 * @param   sprt_kobj
 * @retval  none
 * @note    none
 */
void fwk_kobject_del_name(struct fwk_kobject *sprt_kobj)
{
    if (sprt_kobj->name)
        kfree(sprt_kobj->name);

    sprt_kobj->name = mrt_nullptr;
}

/*!
 * @brief   get kobject's name
 * @param   sprt_kobj
 * @retval  name
 * @note    none
 */
kchar_t *fwk_kobject_get_name(struct fwk_kobject *sprt_kobj)
{
    return sprt_kobj->name;
}

/*!
 * @brief   kobject get
 * @param   sprt_kobj
 * @retval  kobject
 * @note    none
 */
struct fwk_kobject *fwk_kobject_get(struct fwk_kobject *sprt_kobj)
{
    fwk_kref_get(&sprt_kobj->sgrt_ref);
    return sprt_kobj;
}

/*!
 * @brief   kobject put
 * @param   sprt_kobj
 * @retval  none
 * @note    none
 */
void fwk_kobject_put(struct fwk_kobject *sprt_kobj)
{
    fwk_kref_put(&sprt_kobj->sgrt_ref);
}

/*!
 * @brief   check if kobject is using now
 * @param   sprt_kobj
 * @retval  non-zero: using; 0: closed
 * @note    none
 */
kbool_t fwk_kobject_is_referrd(struct fwk_kobject *sprt_kobj)
{
    return !fwk_kref_is_zero(&sprt_kobj->sgrt_ref);
}

/*!
 * @brief   initialize kset
 * @param   sprt_kset
 * @retval  none
 * @note    none
 */
void fwk_kset_init(struct fwk_kset *sprt_kset)
{
    init_list_head(&sprt_kset->sgrt_list);
    fwk_kobject_init(&sprt_kset->sgrt_kobj);
}

/*!
 * @brief   create kset
 * @param   name, sprt_parent
 * @retval  kset created
 * @note    none
 */
struct fwk_kset *fwk_kset_create(const kchar_t *name, struct fwk_kobject *sprt_parent)
{
    struct fwk_kset *sprt_kset;

    sprt_kset = kzalloc(sizeof(*sprt_kset), GFP_KERNEL);
    if (!isValid(sprt_kset))
        return mrt_nullptr;

    if (fwk_kobject_set_name(&sprt_kset->sgrt_kobj, "%s", name))
    {
        kfree(sprt_kset);
        return mrt_nullptr;
    }

    sprt_kset->sgrt_kobj.sprt_parent = sprt_parent;
    sprt_kset->sgrt_kobj.sprt_kset = mrt_nullptr;

    return sprt_kset;
}

/*!
 * @brief   register kset
 * @param   sprt_kset
 * @retval  errno
 * @note    none
 */
kint32_t fwk_kset_register(struct fwk_kset *sprt_kset)
{
    struct fwk_kobject *sprt_kobj;
    struct fwk_kset *sprt_temp;

    if (!is_fwk_root_existed)
        return -ER_FORBID;

    sprt_kobj = &sprt_kset->sgrt_kobj;
    sprt_temp = sprt_kobj->sprt_kset;

    fwk_kset_init(sprt_kset);
    sprt_kobj->is_dir = true;
    sprt_kobj->sprt_kset = sprt_temp;

    if (!sprt_temp)
        sprt_kobj->sprt_kset = &sgrt_fwk_kset_root;
    
    return fwk_kobject_build_inode(&sprt_kset->sgrt_kobj);
}

/*!
 * @brief   create and register kset
 * @param   name, sprt_parent
 * @retval  kset created
 * @note    none
 */
struct fwk_kset *fwk_kset_create_and_register(const kchar_t *name, struct fwk_kobject *sprt_parent)
{
    struct fwk_kset *sprt_kset;

    sprt_kset = fwk_kset_create(name, sprt_parent);
    if (!isValid(sprt_kset))
        return mrt_nullptr;

    if (fwk_kset_register(sprt_kset))
    {
        fwk_kobject_del_name(&sprt_kset->sgrt_kobj);
        kfree(sprt_kset);

        return mrt_nullptr;
    }

    return sprt_kset;
}

/*!
 * @brief   unregister kset
 * @param   sprt_kset
 * @retval  none
 * @note    none
 */
void fwk_kset_unregister(struct fwk_kset *sprt_kset)
{
    if (!mrt_list_head_empty(&sprt_kset->sgrt_list))
        return;

    fwk_kobject_del(&sprt_kset->sgrt_kobj);
    fwk_kset_init(sprt_kset);
}

/*!
 * @brief   unregister kset and kobject
 * @param   sprt_kobj
 * @retval  none
 * @note    none
 */
void fwk_kset_kobject_remove(struct fwk_kobject *sprt_kobj)
{
    if (!sprt_kobj->is_dir)
        fwk_kobject_destroy(sprt_kobj);
    else
    {
        struct fwk_kset *sprt_kset = mrt_fwk_kset_get(sprt_kobj);
        fwk_kset_unregister(sprt_kset);
        kfree(sprt_kset);
    }
}

/*!
 * @brief   get root address
 * @param   none
 * @retval  fwk_kset
 * @note    none
 */
struct fwk_kset *fwk_kset_get_root(void)
{
    return &sgrt_fwk_kset_root;
}

/*!< ------------------------------------------------------- */
/*!
 * @brief   root directory init
 * @param   none
 * @retval  errno
 * @note    none
 */
kint32_t __plat_init fwk_kobject_root_init(void)
{
    struct fwk_kset *sprt_kset = &sgrt_fwk_kset_root;
    
    fwk_kset_init(sprt_kset);
    fwk_kobject_set_name(&sprt_kset->sgrt_kobj, "/");
    sprt_kset->sgrt_kobj.sprt_parent = mrt_nullptr;
    sprt_kset->sgrt_kobj.sprt_kset = mrt_nullptr;
    sprt_kset->sgrt_kobj.is_dir = true;

    is_fwk_root_existed = true;

    return ER_NORMAL;
}

/*!< end of file */
