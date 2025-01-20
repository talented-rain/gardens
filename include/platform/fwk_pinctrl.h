/*
 * Hardware Abstraction Layer Pinctrl Interface
 *
 * File Name:   fwk_pinctrl.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.04.15
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_PINCTRL_H_
#define __FWK_PINCTRL_H_

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/fwk_platform.h>
#include <platform/of/fwk_of.h>
#include <kernel/kernel.h>
#include <kernel/mutex.h>

/*!< The defines */
typedef struct fwk_gpio_chip srt_fwk_gpio_chip_t;

#define FWK_PINCTRL_STATE_DEFAULT 					"default"
#define FWK_PINCTRL_STATE_IDLE 						"idle"
#define FWK_PINCTRL_STATE_SLEEP 					"sleep"

#define FWK_PINCTRL_STATE_DUMMY						"<dummy>"
#define FWK_PINCTRL_PIN_DUMMY						"<dummy>"

enum __ERT_FWK_PINCTRL_PIN_TYPE
{
	NR_FWK_PINCTRL_PIN_MUX = 0,
	NR_FWK_PINCTRL_PIN_CONF,
};

typedef struct fwk_pinctrl_pin_desc 
{
	kuint32_t number;								/*!< pin number */
	const kchar_t *name;							/*!< pin name */
	void *drv_data;

#define FWK_PINCTRL_PIN(a, b)						{ .number = a, .name = b }
	
} srt_fwk_pinctrl_pin_desc_t;

typedef struct fwk_pinctrl_desc
{
	const kchar_t *name;

	/*!< npins: sizeof(sprt_pins[])*/
	struct fwk_pinctrl_pin_desc const *sprt_pins;
	kuint32_t npins;

	const struct fwk_pinctrl_ops *sprt_pctlops;
	const struct fwk_pinmux_ops *sprt_pmxops;
	const struct fwk_pinconf_ops *sprt_confops;

} srt_fwk_pinctrl_desc_t;

typedef struct fwk_pinctrl_dev 
{
	struct list_head sgrt_link;						/*!< link to global list */
	struct fwk_pinctrl_desc *sprt_desc;
	struct list_head sgrt_gpio_ranges;
	struct fwk_device *sprt_dev;
	struct fwk_pinctrl_state *sprt_hog_default;
	struct fwk_pinctrl_state *sprt_hog_sleep;
	struct mutex_lock sgrt_mutex;
	struct fwk_pinctrl *sprt_pctl;

	void *driver_data;

} srt_fwk_pinctrl_dev_t;

/*!< ----------------------------------------------------------------- */
/*!< pinctrl ctrl operations */
typedef struct fwk_pinctrl_map_mux 
{
	const kchar_t *group;
	const kchar_t *function;

} srt_fwk_pinctrl_map_mux_t;

typedef struct fwk_pinctrl_map_configs 
{
	const kchar_t *group_or_pin;
	kuint32_t *configs;
	kuint32_t num_configs;

} srt_fwk_pinctrl_map_configs_t;

typedef struct fwk_pinctrl_map 
{
	const kchar_t *dev_name;
	const kchar_t *name;
	kuint32_t type;
	const kchar_t *ctrl_dev_name;

	union 
	{
		struct fwk_pinctrl_map_mux sgrt_mux;
		struct fwk_pinctrl_map_configs sgrt_configs;
	} ugrt_data;

} srt_fwk_pinctrl_map_t;

typedef struct fwk_pinctrl_maps 
{
	struct list_head sgrt_link;
	struct fwk_pinctrl_map *sprt_maps;
	kuint32_t num_maps;

} srt_fwk_pinctrl_maps_t;

typedef struct fwk_pinctrl_ops
{
	kint32_t 		(*get_function_groups) (struct fwk_pinctrl_dev *sprt_pctldev,
				  						kuint32_t selector, kuaddr_t **groups, kuint32_t * const num_groups);
	kint32_t 		(*get_groups_count)(struct fwk_pinctrl_dev *sprt_pctldev, kuint32_t func_selector);
	const kchar_t *(*get_group_name) (struct fwk_pinctrl_dev *sprt_pctldev, kuint32_t func_selector, kuint32_t group_selector);
	const kchar_t *(*get_pin_desc)(struct fwk_pinctrl_dev *sprt_pctldev, kuint32_t number);

	kint32_t 		(*dt_node_to_map) (struct fwk_pinctrl_dev *sprt_pctldev, struct fwk_device_node *sprt_node, 
												struct fwk_pinctrl_map **sprt_map, kuint32_t *num_maps);
	void 			(*dt_free_map) (struct fwk_pinctrl_dev *sprt_pctldev, struct fwk_pinctrl_map *sprt_map, kuint32_t num_maps);

} srt_fwk_pinctrl_ops_t;

/*!< ----------------------------------------------------------------- */
/*!< pinctrl pinmux operations */
typedef struct fwk_pinctrl_gpio_range 
{
	struct list_head sgrt_link;
	const kchar_t *name;
	kuint32_t id;
	kuint32_t base;
	kuint32_t pin_base;
	kuint32_t const *pins;
	kuint32_t npins;

	struct fwk_gpio_chip *sprt_gc;

} srt_fwk_pinctrl_gpio_range_t;

typedef struct fwk_pinmux_ops 
{
	kint32_t 		(*request) (struct fwk_pinctrl_dev *sprt_pctldev, kuint32_t offset);
	kint32_t 		(*free) (struct fwk_pinctrl_dev *sprt_pctldev, kuint32_t offset);

	kint32_t 		(*get_functions_count) (struct fwk_pinctrl_dev *sprt_pctldev);
	const kchar_t *(*get_function_name) (struct fwk_pinctrl_dev *sprt_pctldev, kuint32_t selector);
	kint32_t 		(*set_mux) (struct fwk_pinctrl_dev *sprt_pctldev, kuint32_t func_selector, kuint32_t group_selector);
	
	kint32_t 		(*gpio_request_enable) (struct fwk_pinctrl_dev *sprt_pctldev, 
										struct fwk_pinctrl_gpio_range *sprt_range, kuint32_t offset);

	void 			(*gpio_disable_free) (struct fwk_pinctrl_dev *sprt_pctldev, 
										struct fwk_pinctrl_gpio_range *sprt_range, kuint32_t offset);

	kint32_t 		(*gpio_set_direction) (struct fwk_pinctrl_dev *sprt_pctldev, 
										struct fwk_pinctrl_gpio_range *sprt_range, kuint32_t offset, kbool_t input);
	
} srt_fwk_pinmux_ops_t;

/*!< ----------------------------------------------------------------- */
/*!< pinctrl pin configurations operations */
typedef struct fwk_pinconf_ops 
{
	kint32_t 		(*pin_config_get) (struct fwk_pinctrl_dev *sprt_pctldev, kuint32_t pin, kuint32_t *config);
	kint32_t 		(*pin_config_set) (struct fwk_pinctrl_dev *sprt_pctldev, kuint32_t pin, kuint32_t *configs, kuint32_t num_configs);

} srt_fwk_pinconf_ops_t;

/*!< ----------------------------------------------------------------- */
typedef struct fwk_pinctrl_mux 
{
	kuint32_t group;
	kuint32_t func;										/*!< mux data*/

} srt_fwk_pinctrl_mux_t;

typedef struct fwk_pinctrl_configs 
{
	kuint32_t group_or_pin;								/*!< group, such as i2c: sck and sda are one group */
	kuint32_t *configs;
	kuint32_t num_configs;

} srt_fwk_pinctrl_configs_t;

/*!< pin configurations of one pin */
typedef struct fwk_pinctrl_setting 
{
	kuint32_t type;										/*!< refer to "__ERT_FWK_PINCTRL_PIN_TYPE" */
	struct list_head sgrt_link;							/*!< link to fwk_pinctrl_state::sgrt_settings */
	struct fwk_pinctrl_dev *sprt_pctldev;
	const kchar_t *dev_name;

	union 
	{
		struct fwk_pinctrl_mux sgrt_mux;				/*!< a group of pin mux */
		struct fwk_pinctrl_configs sgrt_configs;		/*!< a group of pin conf */
	} ugrt_data;

} srt_fwk_pinctrl_setting_t;

/*!< a state represents a pinctrl (pinctrl-0, pinctrl-1, ...) */
typedef struct fwk_pinctrl_state 
{
	struct list_head sgrt_link;                         /*!< pinctrl_state list, link to fwk_pinctrl::sgrt_states */
	const kchar_t *name;                              /*!< pinctrl-names */
	struct list_head sgrt_settings;                     /*!< list head of fwk_pinctrl_setting::sgrt_link */

} srt_fwk_pinctrl_state_t;

typedef struct fwk_pinctrl
{
	struct list_head sgrt_link;							/*!< link to global list */
	struct fwk_device *sprt_dev;						/*!< sprt_dev == fwk_pinctrl_dev::sprt_dev */
	struct list_head sgrt_states;                       /*!< list head of pinctrl_state::sgrt_link */
	struct fwk_pinctrl_state *sprt_state;               /*!< current usage state, such as "default" */
	struct list_head sgrt_dt_maps;

} srt_fwk_pinctrl_t;

/*!< The functions */
extern struct fwk_pinctrl_dev *fwk_pinctrl_register(struct fwk_pinctrl_desc *sprt_desc, struct fwk_device *sprt_dev, void *driver_data);
extern void fwk_pinctrl_unregister(struct fwk_pinctrl_dev *sprt_pctldev);
extern struct fwk_pinctrl *fwk_pinctrl_get(struct fwk_device *sprt_dev);
extern void fwk_pinctrl_put(struct fwk_pinctrl *sprt_pctl);
extern struct fwk_pinctrl_state *fwk_pinctrl_lookup_state(struct fwk_pinctrl *sprt_pctl, const kchar_t *state_name);
extern kint32_t fwk_pinctrl_select_state(struct fwk_pinctrl *sprt_pctl, struct fwk_pinctrl_state *sprt_state);
extern kint32_t fwk_pinctrl_bind_pins(struct fwk_device *sprt_dev);
extern void fwk_pinctrl_unbind_pins(struct fwk_device *sprt_dev);

extern kint32_t fwk_pinmux_map_to_setting(struct fwk_pinctrl_map const *sprt_map, struct fwk_pinctrl_setting *sprt_setting);
extern kint32_t fwk_pinconf_get_by_name(struct fwk_pinctrl_dev *sprt_pctldev, const kchar_t *name);
extern kint32_t fwk_pinconf_map_to_setting(struct fwk_pinctrl_map const *sprt_map, struct fwk_pinctrl_setting *sprt_setting);

/*!< API functions */
/*!
 * @brief   get driver data from pinctrl device
 * @param   sprt_pctldev
 * @retval  none
 * @note    none
 */
static inline void *fwk_pinctrl_get_drvdata(struct fwk_pinctrl_dev *sprt_pctldev)
{
	return sprt_pctldev->driver_data;
}

#ifdef __cplusplus
    }
#endif

#endif /*!< __FWK_PINCTRL_H_ */
