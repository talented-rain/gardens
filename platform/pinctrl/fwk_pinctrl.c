/*
 * Hardware Abstraction Layer Pinctrl Interface
 *
 * File Name:   fwk_pinctrl.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.04.15
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <platform/fwk_pinctrl.h>

/*!< The defines */
typedef struct fwk_pinctrl_dev_info
{
    struct fwk_device *sprt_dev;
    struct fwk_pinctrl *sprt_pctl;

    struct fwk_pinctrl_state *sprt_default;
    struct fwk_pinctrl_state *sprt_idle;
    struct fwk_pinctrl_state *sprt_sleep;

} srt_fwk_pinctrl_dev_info_t;

typedef struct fwk_pinctrl_dt_map 
{
	struct list_head sgrt_link;
	struct fwk_pinctrl_dev *sprt_pctldev;
	struct fwk_pinctrl_maps *sprt_maps;
	kuint32_t num_maps;

} srt_fwk_pinctrl_dt_map_t;

/*!< The globals */
static DECLARE_LIST_HEAD(sgrt_fwk_pinctrl_list);
static DECLARE_LIST_HEAD(sgrt_fwk_pinctrl_dev_list);
static DECLARE_LIST_HEAD(sgrt_fwk_pinctrl_map_list);

#define foreach_fwk_pinctrl_maps(maps, map, list) \
    foreach_list_next_entry(maps, &sgrt_fwk_pinctrl_map_list, list) \
        if ((map = maps->sprt_maps))

/*!< API function */
/*!
 * @brief   find pinctrl by device node
 * @param   sprt_node: controller node
 * @retval  pinctrl_dev found
 * @note    none
 */
__unused
static struct fwk_pinctrl *fwk_get_pinctrl_from_node(struct fwk_device_node *sprt_node)
{
    struct fwk_pinctrl *sprt_pctl;

    foreach_list_next_entry(sprt_pctl, &sgrt_fwk_pinctrl_list, sgrt_link)
    {
        if (sprt_pctl->sprt_dev->sprt_node == sprt_node)
            return sprt_pctl;
    }

    return mrt_nullptr;
}

/*!
 * @brief   find pinctrl_dev by device node
 * @param   sprt_node: controller node
 * @retval  pinctrl_dev found
 * @note    none
 */
static struct fwk_pinctrl_dev *fwk_get_pinctrl_dev_from_node(struct fwk_device_node *sprt_node)
{
    struct fwk_pinctrl_dev *sprt_pctldev;

    foreach_list_next_entry(sprt_pctldev, &sgrt_fwk_pinctrl_dev_list, sgrt_link)
    {
        if (sprt_pctldev->sprt_dev->sprt_node == sprt_node)
            return sprt_pctldev;
    }

    return mrt_nullptr;
}

/*!
 * @brief   find pinctrl_dev by device name (from global list)
 * @param   name: sprt_dev->name
 * @retval  pinctrl_dev found
 * @note    none
 */
static struct fwk_pinctrl_dev *fwk_get_pinctrl_dev_from_name(const kchar_t *name)
{
    struct fwk_pinctrl_dev *sprt_pctldev;

    foreach_list_next_entry(sprt_pctldev, &sgrt_fwk_pinctrl_dev_list, sgrt_link)
    {
        if (!strcmp(mrt_dev_get_name(sprt_pctldev->sprt_dev), name))
            return sprt_pctldev;
    }

    return mrt_nullptr;
}

/*!
 * @brief   release pinctrl map
 * @param   sprt_pctldev: pinctrl dev
 * @param   sprt_map: map that will be released
 * @retval  none
 * @note    none
 */
static void fwk_pinctrl_map_free(struct fwk_pinctrl_dev *sprt_pctldev, struct fwk_pinctrl_map *sprt_map, kuint32_t num_maps)
{
    const struct fwk_pinctrl_ops *sprt_pctlops;

    if (!sprt_map || !num_maps)
        return;

    sprt_pctlops = sprt_pctldev ? sprt_pctldev->sprt_desc->sprt_pctlops : mrt_nullptr;

    if (sprt_pctlops && sprt_pctlops->dt_free_map)
        sprt_pctlops->dt_free_map(sprt_pctldev, sprt_map, num_maps);
    else
        kfree(sprt_map);
}

/*!
 * @brief   release pinctrl maps
 * @param   sprt_pctldev: pinctrl dev
 * @param   sprt_maps: maps that will be released
 * @retval  none
 * @note    none
 */
static void fwk_pinctrl_maps_free(struct fwk_pinctrl_dev *sprt_pctldev, struct fwk_pinctrl_maps *sprt_maps)
{
    if (!sprt_maps)
        return;

    /*!< release sprt_map */
    fwk_pinctrl_map_free(sprt_pctldev, sprt_maps->sprt_maps, sprt_maps->num_maps);

    /*!< detach from global list */
    list_head_del(&sprt_maps->sgrt_link);
    kfree(sprt_maps);
}

/*!
 * @brief   release pinctrl
 * @param   sprt_pctl: current pinctrl
 * @retval  none
 * @note    none
 */
static void fwk_pinctrl_free(struct fwk_pinctrl *sprt_pctl)
{
    struct fwk_pinctrl_dev *sprt_pctldev;
    struct fwk_pinctrl_state *sprt_state, *sprt_statetemp;
    struct fwk_pinctrl_maps *sprt_maps;
    struct fwk_pinctrl_dt_map *sprt_dt_maps, *sprt_dt_maptemp;
    struct fwk_pinctrl_setting *sprt_setting, *sprt_settemp;

    if (!sprt_pctl)
        return;

    /*!< 1, release sprt_setting and sprt_state */
    if (!mrt_list_head_empty(&sprt_pctl->sgrt_states))
    {
        foreach_list_next_entry_safe(sprt_state, sprt_statetemp, &sprt_pctl->sgrt_states, sgrt_link)
        {
            foreach_list_next_entry_safe(sprt_setting, sprt_settemp, &sprt_state->sgrt_settings, sgrt_link)
            {
                list_head_del(&sprt_setting->sgrt_link);
                kfree(sprt_setting);
            }

            list_head_del(&sprt_state->sgrt_link);
            kfree(sprt_state);
        }
    }

    if (mrt_list_head_empty(&sprt_pctl->sgrt_dt_maps))
        return;

    /*!< 2, release sprt_map and sprt_maps */
    foreach_list_next_entry_safe(sprt_dt_maps, sprt_dt_maptemp, &sprt_pctl->sgrt_dt_maps, sgrt_link)
    {
        sprt_maps = sprt_dt_maps->sprt_maps;
        if (!sprt_maps)
            continue;
        
        sprt_pctldev = sprt_dt_maps->sprt_pctldev;
        fwk_pinctrl_maps_free(sprt_pctldev, sprt_maps);

        list_head_del(&sprt_dt_maps->sgrt_link);
        kfree(sprt_dt_maps);
    }
}

/*!
 * @brief   find pinctrl state
 * @param   sprt_pctl: current pinctrl
 * @param   name: "default", "idle", or "sleep"
 * @retval  state found
 * @note    none
 */
static struct fwk_pinctrl_state *fwk_pinctrl_find_state(struct fwk_pinctrl *sprt_pctl, const kchar_t *name)
{
    struct fwk_pinctrl_state *sprt_state;

    if (mrt_list_head_empty(&sprt_pctl->sgrt_states))
        return mrt_nullptr;

    foreach_list_next_entry(sprt_state, &sprt_pctl->sgrt_states, sgrt_link)
    {
        if (!strcmp(name, sprt_state->name))
            return sprt_state;
    }

    return mrt_nullptr;
}

/*!
 * @brief   create pinctrl states
 * @param   sprt_pctl: current pinctrl
 * @param   pinctl_name: property name
 * @param   state_name: pinctrl-names = "default", "idle", or "sleep"
 * @retval  pinctrl_state
 * @note    none
 */
static struct fwk_pinctrl_state *fwk_pinctrl_create_state(struct fwk_pinctrl *sprt_pctl, 
                                                    const kchar_t *pinctl_name, const kchar_t *state_name)
{
    struct fwk_pinctrl_state *sprt_state;
    struct fwk_device_node *sprt_node, *sprt_cfg, *sprt_par;
    struct fwk_of_property *sprt_prop;
    struct fwk_pinctrl_dev *sprt_pctldev;
    struct fwk_pinctrl_map *sprt_map;
    struct fwk_pinctrl_maps *sprt_maps;
    const struct fwk_pinctrl_ops *sprt_pctlops;
    struct fwk_pinctrl_dt_map *sprt_dt_maps;

    kuint32_t idx, grps, lenth, phandle, num_maps;

#define FWK_PINCTRL_SET_MAP_NAME(len)          (((state_name) && (len)) ? (state_name) : FWK_PINCTRL_STATE_DUMMY)

    if (!sprt_pctl)
        return mrt_nullptr;

    /*!< get current node */
    sprt_node = sprt_pctl->sprt_dev->sprt_node;

    /*!< property over */
    sprt_prop = fwk_of_find_property(sprt_node, pinctl_name, &lenth);
    if (!isValid(sprt_prop))
        return mrt_nullptr;

    /*!< 
     * lenth >> 2 : how many phandles here
     * for example: pinctrl-0 = <&pinctrl_gpio1 &pinctrl_gpio2>
     * ===> the number of phandles is 2 (lenth = 8bytes), grp = [0, 1]
     */
    for (grps = 0; grps < (lenth >> 2); grps++)
    {
        /*!< fwk_of_property_read_u32_index(sprt_node, propname, grps, &phandle) */
        phandle = FDT_TO_ARCH_ENDIAN32(*((kuint32_t *)sprt_prop->value + grps));

        sprt_cfg = fwk_of_find_node_by_phandle(mrt_nullptr, phandle);
        if (!sprt_cfg)
            goto fail;

        /*!< find pinctrl dev */
        foreach_fwk_of_parent(sprt_par, sprt_cfg)
        {
            /*!< judge if sprt_pctldev->sprt_dev->sprt_node is equaled to sprt_par */
            sprt_pctldev = fwk_get_pinctrl_dev_from_node(sprt_par);
            if (sprt_pctldev)
                break;

            /*!< not found */
            if (!sprt_pctldev && (sprt_par == fwk_of_node_root()))
                goto fail;
        }

        sprt_pctlops = sprt_pctldev->sprt_desc->sprt_pctlops;
        if (!sprt_pctlops || !sprt_pctlops->dt_node_to_map)
            goto fail;

        /*!< allocate sprt_map from memory and initialize it */
        if (sprt_pctlops->dt_node_to_map(sprt_pctldev, sprt_cfg, &sprt_map, &num_maps))
            goto fail;

        for (idx = 0; idx < num_maps; idx++)
        {
            /*!< sprt_map->name <===> sprt_state->name: "default", "sleep", ... */
            sprt_map[idx].dev_name = mrt_dev_get_name(sprt_pctl->sprt_dev);
            sprt_map[idx].name = FWK_PINCTRL_SET_MAP_NAME(lenth);
            sprt_map[idx].ctrl_dev_name = mrt_dev_get_name(sprt_pctldev->sprt_dev);
        }

        /*!< 
         * each group is represented by a sprt_maps; 
         * each pin is represented by a sprt_map;
         * a group consists of multiple pins (1 mux + n conf); 
         * num_maps is the number of sprt_map array, num_maps = the number of "mux" + the number of "conf"
         * sprt_maps->sprt_map = &sprt_map[0]
         */
        sprt_maps = (struct fwk_pinctrl_maps *)kzalloc(sizeof(*sprt_maps), GFP_KERNEL);
        if (!isValid(sprt_maps))
        {
            fwk_pinctrl_map_free(sprt_pctldev, sprt_map, num_maps);
            goto fail;
        }

        sprt_maps->sprt_maps = sprt_map;
        sprt_maps->num_maps  = num_maps;
        list_head_add_tail(&sgrt_fwk_pinctrl_map_list, &sprt_maps->sgrt_link);

        /*!< each sprt_dt_maps links to sprt_pctl */
        sprt_dt_maps = (struct fwk_pinctrl_dt_map *)kzalloc(sizeof(*sprt_dt_maps), GFP_KERNEL);
        if (!isValid(sprt_dt_maps))
        {
            fwk_pinctrl_maps_free(sprt_pctldev, sprt_maps);
            goto fail;
        }

        sprt_dt_maps->sprt_maps = sprt_maps;
        sprt_dt_maps->num_maps = 1;
        sprt_dt_maps->sprt_pctldev = sprt_pctldev;
        list_head_add_tail(&sprt_pctl->sgrt_dt_maps, &sprt_dt_maps->sgrt_link);
    }

    sprt_state = (struct fwk_pinctrl_state *)kzalloc(sizeof(*sprt_state), GFP_KERNEL);
    if (!isValid(sprt_state))
        goto fail;

    /*!< name: "default", "idle", or "sleep" */
    sprt_state->name = FWK_PINCTRL_SET_MAP_NAME(lenth);
    init_list_head(&sprt_state->sgrt_settings);

    return sprt_state;

fail:
    fwk_pinctrl_free(sprt_pctl);
    return mrt_nullptr;

#undef FWK_PINCTRL_SET_MAP_NAME
}

/*!
 * @brief   create pinctrl states
 * @param   sprt_pctl: current pinctrl
 * @retval  error code
 * @note    none
 */
static kint32_t fwk_pinctrl_add_state(struct fwk_pinctrl *sprt_pctl)
{
    struct fwk_pinctrl_state *sprt_state;
    struct fwk_device_node *sprt_node;
    struct fwk_of_property *sprt_prop;
    kchar_t propname[16], *state_name;
    kuint32_t idx;

    if (!sprt_pctl)
        return -ER_NODEV;

    /*!< get current node */
    sprt_node = sprt_pctl->sprt_dev->sprt_node;

    /*!< pinctrl-0, pinctrl-1, ..., pinctrl-$(idx) */
    for (idx = 0; ; idx++)
    {
        memset(propname, 0, sizeof(propname));
        sprintk(propname, "pinctrl-%d", idx);

        /*!< sprt_prop == null ? property over */
        sprt_prop = fwk_of_find_property(sprt_node, propname, mrt_nullptr);
        if (!isValid(sprt_prop))
        {
            if (!idx)
                goto fail;
            else
                break;
        }

        fwk_of_property_read_string_index(sprt_node, "pinctrl-names", idx, &state_name);
        sprt_state = fwk_pinctrl_find_state(sprt_pctl, state_name);
        if (sprt_state)
            continue;

        sprt_state = fwk_pinctrl_create_state(sprt_pctl, propname, state_name);
        if (!isValid(sprt_state))
            goto fail;

        list_head_add_tail(&sprt_pctl->sgrt_states, &sprt_state->sgrt_link);
    }

    return ER_NORMAL;

fail:
    fwk_pinctrl_free(sprt_pctl);
    return -ER_FAILD;
}

/*!
 * @brief   create pinctrl settings
 * @param   sprt_pctl: current pinctrl
 * @retval  error code
 * @note    none
 */
static kint32_t fwk_pinctrl_add_setting(struct fwk_pinctrl *sprt_pctl)
{
    struct fwk_pinctrl_setting *sprt_setting;
    struct fwk_pinctrl_state *sprt_state;
    struct fwk_pinctrl_map *sprt_map = mrt_nullptr;
    struct fwk_pinctrl_maps *sprt_maps = mrt_nullptr;
    kuint32_t i;
    kint32_t retval = 0;

    if (mrt_list_head_empty(&sgrt_fwk_pinctrl_map_list))
        return -ER_EMPTY;

    /*!< get each sprt_maps */
    foreach_fwk_pinctrl_maps(sprt_maps, sprt_map, sgrt_link)
    {
        for (i = 0; i < sprt_maps->num_maps; i++, sprt_map++)
        {
            /*!< judge if it is belongs to current device */
            if (strcmp(sprt_map->dev_name, mrt_dev_get_name(sprt_pctl->sprt_dev)))
                continue;

            /*!< found sprt_map; find state next*/
            sprt_state = fwk_pinctrl_find_state(sprt_pctl, sprt_map->name);
            if (!sprt_state)
                continue;

            sprt_setting = (struct fwk_pinctrl_setting *)kzalloc(sizeof(*sprt_setting), GFP_KERNEL);
            if (!isValid(sprt_setting))
                goto fail;

            /*!< type: mux or conf */
            sprt_setting->type = sprt_map->type;
            sprt_setting->dev_name = sprt_map->dev_name;

            sprt_setting->sprt_pctldev = fwk_get_pinctrl_dev_from_name(sprt_map->ctrl_dev_name);
            if (!sprt_setting->sprt_pctldev)
            {
                kfree(sprt_setting);
                goto fail;
            }

            switch (sprt_setting->type)
            {
                case NR_FWK_PINCTRL_PIN_MUX:
                    retval = fwk_pinmux_map_to_setting(sprt_map, sprt_setting);
                    break;

                case NR_FWK_PINCTRL_PIN_CONF:
                    retval = fwk_pinconf_map_to_setting(sprt_map, sprt_setting);
                    break;

                default: break;
            }

            if (retval)
            {
                kfree(sprt_setting);
                goto fail;
            }

            list_head_add_tail(&sprt_state->sgrt_settings, &sprt_setting->sgrt_link);
        }
    }
   
    return ER_NORMAL;

fail:
    fwk_pinctrl_free(sprt_pctl);
    return -ER_FAILD;
}

/*!
 * @brief   find pinctrl if it is exsited
 * @param   sprt_dev: current device (sprt_pctl->dev may equal to it)
 * @retval  pinctrl that was found
 * @note    all pinctrls are linked to global list, you should loop up from it
 */
static struct fwk_pinctrl *fwk_find_pinctrl(struct fwk_device *sprt_dev)
{
    struct fwk_pinctrl *sprt_pctl;

    if (!sprt_dev || mrt_list_head_empty(&sgrt_fwk_pinctrl_list))
        return mrt_nullptr;

    foreach_list_next_entry(sprt_pctl, &sgrt_fwk_pinctrl_list, sgrt_link)
    {
        /*!< compare sprt_dev */
        if (sprt_pctl->sprt_dev == sprt_dev)
            return sprt_pctl;
    }

    return mrt_nullptr;
}

/*!
 * @brief   create a new pinctrl
 * @param   sprt_dev: current device
 * @retval  pinctrl that was allocated
 * @note    none
 */
static struct fwk_pinctrl *fwk_create_pinctrl(struct fwk_device *sprt_dev)
{
    struct fwk_pinctrl *sprt_pctl;

    if (!sprt_dev)
        return mrt_nullptr;

    sprt_pctl = (struct fwk_pinctrl *)kzalloc(sizeof(*sprt_pctl), GFP_KERNEL);
    if (!isValid(sprt_pctl))
        return mrt_nullptr;

    /*!< save current device, you can find pinctrl by comparing sprt_dev in the future */
    sprt_pctl->sprt_dev = sprt_dev;
    init_list_head(&sprt_pctl->sgrt_states);
    init_list_head(&sprt_pctl->sgrt_dt_maps);

    /*!< create state by "pinctrl-names"; all states are linked to sprt_pctl->sgrt_states */
    if (fwk_pinctrl_add_state(sprt_pctl))
        goto fail1;

    /*!< 
     * create setting for per state
     * for example, assuming that there are n pin-configurations for a group in device_node:
     *      setting for mux: at least one;
     *      setting for conf: at least n (the number of pin configurations)
     * therefore, the number of settings for a state is: n + 1
     */
    if (fwk_pinctrl_add_setting(sprt_pctl))
        goto fail2;

    /*!< link to global list */
    list_head_add_tail(&sgrt_fwk_pinctrl_list, &sprt_pctl->sgrt_link);

    return sprt_pctl;

fail2:
    fwk_pinctrl_free(sprt_pctl);
fail1:
    kfree(sprt_pctl);
    return mrt_nullptr;
}

/*!
 * @brief   register pinctrl
 * @param   sprt_desc: device operations
 * @param   sprt_dev: current device
 * @param   driver_data: driver private data
 * @retval  pinctrl_dev
 * @note    none
 */
struct fwk_pinctrl_dev *fwk_pinctrl_register(struct fwk_pinctrl_desc *sprt_desc, struct fwk_device *sprt_dev, void *driver_data)
{
    struct fwk_pinctrl_dev *sprt_pctldev;
    struct fwk_pinctrl *sprt_pctl;
    struct fwk_pinctrl_state *sprt_state;

    if (!sprt_desc)
        return mrt_nullptr;

    if ((!sprt_desc->sprt_pctlops) || (!sprt_desc->sprt_pmxops) || (!sprt_desc->sprt_confops))
        return mrt_nullptr;

    sprt_pctldev = (struct fwk_pinctrl_dev *)kzalloc(sizeof(*sprt_pctldev), GFP_KERNEL);
    if (!isValid(sprt_pctldev))
        return mrt_nullptr;

    sprt_pctldev->sprt_desc = sprt_desc;
    sprt_pctldev->sprt_dev = sprt_dev;
    sprt_pctldev->driver_data = driver_data;
    mutex_init(&sprt_pctldev->sgrt_mutex);
    init_list_head(&sprt_pctldev->sgrt_gpio_ranges);

    list_head_add_tail(&sgrt_fwk_pinctrl_dev_list, &sprt_pctldev->sgrt_link);

    /*!< init pinctrl state after adding to sgrt_fwk_pinctrl_dev_list */
    sprt_pctl = fwk_pinctrl_get(sprt_dev);
    if (!sprt_pctl)
        goto fail1;

    sprt_pctldev->sprt_pctl = sprt_pctl;
    sprt_pctldev->sprt_hog_sleep = fwk_pinctrl_lookup_state(sprt_pctl, FWK_PINCTRL_STATE_SLEEP);

    sprt_state = fwk_pinctrl_lookup_state(sprt_pctl, FWK_PINCTRL_STATE_DEFAULT);
    if (!sprt_state)
        goto END;

    if (fwk_pinctrl_select_state(sprt_pctl, sprt_state))
        goto fail2;

    sprt_pctldev->sprt_hog_default = sprt_state;
    
END:
    return sprt_pctldev;

fail2:
    fwk_pinctrl_put(sprt_pctl);
fail1:
    list_head_del(&sprt_pctldev->sgrt_link);
    kfree(sprt_pctldev);

    return mrt_nullptr;
}

/*!
 * @brief   register pinctrl
 * @param   sprt_desc: device operations
 * @param   sprt_dev: current device
 * @param   driver_data: driver private data
 * @retval  pinctrl_dev
 * @note    none
 */
void fwk_pinctrl_unregister(struct fwk_pinctrl_dev *sprt_pctldev)
{
    if (!sprt_pctldev)
        return;

    if (sprt_pctldev->sprt_pctl)
        fwk_pinctrl_put(sprt_pctldev->sprt_pctl);

    list_head_del(&sprt_pctldev->sgrt_link);
    kfree(sprt_pctldev);
}

/*!
 * @brief   device binds pinctrl
 * @param   sprt_dev: current device 
 * @retval  errno
 * @note    pinctrl-device-info ===> sprt_dev->pinctrl_info = sprt_info
 */
kint32_t fwk_pinctrl_bind_pins(struct fwk_device *sprt_dev)
{
    struct fwk_pinctrl_dev_info *sprt_info;
    struct fwk_pinctrl *sprt_pctl;
    struct fwk_pinctrl_state *sprt_state;

    sprt_info = kzalloc(sizeof(*sprt_info), GFP_KERNEL);
    if (!sprt_info)
        return -ER_NOMEM;

    /*!< init pinctrl state after adding to sgrt_fwk_pinctrl_dev_list */
    sprt_pctl = fwk_pinctrl_get(sprt_dev);
    if (!sprt_pctl)
        goto fail1;

    sprt_dev->sprt_pctlinfo = sprt_info;
    sprt_info->sprt_pctl = sprt_pctl;
    sprt_info->sprt_idle = fwk_pinctrl_lookup_state(sprt_pctl, FWK_PINCTRL_STATE_IDLE);
    sprt_info->sprt_sleep = fwk_pinctrl_lookup_state(sprt_pctl, FWK_PINCTRL_STATE_SLEEP);

    sprt_state = fwk_pinctrl_lookup_state(sprt_pctl, FWK_PINCTRL_STATE_DEFAULT);
    if (!sprt_state)
        goto END;

    if (fwk_pinctrl_select_state(sprt_pctl, sprt_state))
        goto fail2;

    sprt_info->sprt_default = sprt_state;

END:
    return ER_NORMAL;

fail2:
    fwk_pinctrl_put(sprt_pctl);
fail1:
    kfree(sprt_info);
    return -ER_ERROR;
}

/*!
 * @brief   device unbinds pinctrl
 * @param   sprt_dev: current device 
 * @retval  errno
 * @note    release sprt_dev->pinctrl_info
 */
void fwk_pinctrl_unbind_pins(struct fwk_device *sprt_dev)
{
    struct fwk_pinctrl_dev_info *sprt_info;
    
    sprt_info = sprt_dev->sprt_pctlinfo;
    if (!sprt_info)
        return;

    if (sprt_info->sprt_sleep)
        fwk_pinctrl_select_state(sprt_info->sprt_pctl, sprt_info->sprt_sleep);
    else if (sprt_info->sprt_idle)
        fwk_pinctrl_select_state(sprt_info->sprt_pctl, sprt_info->sprt_idle);

    fwk_pinctrl_put(sprt_info->sprt_pctl);
    kfree(sprt_info);
    sprt_dev->sprt_pctlinfo = mrt_nullptr;
}

/*!
 * @brief   release pinctrl
 * @param   sprt_pctl: pinctrl 
 * @retval  none
 * @note    none
 */
void fwk_pinctrl_put(struct fwk_pinctrl *sprt_pctl)
{
    fwk_pinctrl_free(sprt_pctl);
    list_head_del(&sprt_pctl->sgrt_link);
    kfree(sprt_pctl);
}

/*!
 * @brief   request pinctrl
 * @param   sprt_dev: current device 
 * @retval  sprt_pctl
 * @note    none
 */
struct fwk_pinctrl *fwk_pinctrl_get(struct fwk_device *sprt_dev)
{
    struct fwk_pinctrl *sprt_pctl;

    sprt_pctl = fwk_find_pinctrl(sprt_dev);
    if (sprt_pctl)
        return sprt_pctl;

    return fwk_create_pinctrl(sprt_dev);
}

/*!
 * @brief   look up pinctrl state
 * @param   sprt_pctl: pinctrl
 * @param   state_name: "default", "sleep" or "idle"
 * @retval  pinctrl state structure
 * @note    none
 */
struct fwk_pinctrl_state *fwk_pinctrl_lookup_state(struct fwk_pinctrl *sprt_pctl, const kchar_t *state_name)
{
    struct fwk_pinctrl_state *sprt_state;

    if (!sprt_pctl || mrt_list_head_empty(&sprt_pctl->sgrt_states))
        return mrt_nullptr;

    if (sprt_pctl->sprt_state && !strcmp(sprt_pctl->sprt_state->name, state_name))
        return sprt_pctl->sprt_state;

    foreach_list_next_entry(sprt_state, &sprt_pctl->sgrt_states, sgrt_link)
    {
        if (!strcmp(sprt_state->name, state_name))
            return sprt_state;
    }

    return mrt_nullptr;
}

/*!
 * @brief   select pinctrl state
 * @param   sprt_pctl: pinctrl
 * @param   sprt_state: state that will be selected
 * @retval  error code
 * @note    none
 */
kint32_t fwk_pinctrl_select_state(struct fwk_pinctrl *sprt_pctl, struct fwk_pinctrl_state *sprt_state)
{
    struct fwk_pinctrl_setting *sprt_setting;
    struct fwk_pinctrl_state *sprt_cur;
    struct fwk_pinctrl_dev *sprt_pctldev;
	const struct fwk_pinmux_ops *sprt_pmxops;
	const struct fwk_pinconf_ops *sprt_confops;
    kint32_t retval;

    if (!sprt_pctl || !sprt_state)
        return -ER_NODEV;

    sprt_cur = sprt_pctl->sprt_state;

    if (sprt_cur == sprt_state)
        return -ER_EXISTED;

    /*!< disable current pins */
    if (sprt_cur)
    {
        foreach_list_next_entry(sprt_setting, &sprt_cur->sgrt_settings, sgrt_link)
        {
            /*!< do nothing */
        }
    }

    /*!< enable new pin configurations */
    foreach_list_next_entry(sprt_setting, &sprt_state->sgrt_settings, sgrt_link)
    {
        sprt_pctldev = sprt_setting->sprt_pctldev;
        sprt_pmxops = sprt_pctldev->sprt_desc->sprt_pmxops;
        sprt_confops = sprt_pctldev->sprt_desc->sprt_confops;

        if (!sprt_pmxops || !sprt_confops)
            return -ER_NSUPPORT;

        retval = false;

        switch (sprt_setting->type)
        {
            /*!< each setting may only have one mux */
            case NR_FWK_PINCTRL_PIN_MUX:
                if (sprt_pmxops->set_mux)
                    retval = sprt_pmxops->set_mux(sprt_pctldev, 
                                        sprt_setting->ugrt_data.sgrt_mux.func, 
                                        sprt_setting->ugrt_data.sgrt_mux.group);
                break;

            case NR_FWK_PINCTRL_PIN_CONF:
                if (sprt_confops->pin_config_set)
                    retval = sprt_confops->pin_config_set(sprt_pctldev, 
                                        sprt_setting->ugrt_data.sgrt_configs.group_or_pin, 
                                        sprt_setting->ugrt_data.sgrt_configs.configs,
                                        sprt_setting->ugrt_data.sgrt_configs.num_configs);
                break;

            default:
                return -ER_FAULT;
        }

        if (retval)
            return retval;
    }

    sprt_pctl->sprt_state = sprt_state;

    return ER_NORMAL;
}

/*!< end of file */
