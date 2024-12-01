/*
 * Hardware Abstraction Layer Net Interface
 *
 * File Name:   fwk_netdev.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.07.06
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/net/fwk_netdev.h>

/*!< API function */
/*!
 * @brief   Allocate network device
 * @param   none
 * @retval  none
 * @note    none
 */
struct fwk_net_device *fwk_alloc_netdev_mq(kint32_t sizeof_priv, const kchar_t *name,
													void (*setup) (struct fwk_net_device *sprt_dev), kuint32_t queue_count)
{
	struct fwk_net_device *sprt_netdev;
	struct fwk_netdev_queue *sprt_tx;

	sprt_netdev = (struct fwk_net_device *)kzalloc(sizeof(struct fwk_net_device), GFP_KERNEL);
	if (!isValid(sprt_netdev))
		return mrt_nullptr;

	sprt_tx = (struct fwk_netdev_queue *)kzalloc(queue_count * sizeof(struct fwk_netdev_queue), GFP_KERNEL);
	if (!isValid(sprt_tx))
	{
		kfree(sprt_netdev);
		return mrt_nullptr;
	}

	/*!< register send queue */
	sprt_netdev->sprt_tx = sprt_tx;
	sprt_netdev->num_tx_queues = queue_count;
	sprt_netdev->real_num_tx_queues = queue_count;

	/*!< initial recieve queue */
	memset(&sprt_netdev->sgrt_rcu, 0, sizeof(struct fwk_netdev_queue));

	if (setup)
		setup(sprt_netdev);

	kstrcpy(sprt_netdev->name, name);

	return sprt_netdev;
}

/*!
 * @brief   relese network device
 * @param   none
 * @retval  none
 * @note    none
 */
void fwk_free_netdev(struct fwk_net_device *sprt_dev)
{
	if (!isValid(sprt_dev))
		return;

	/*!< release send queue first */
	if (isValid(sprt_dev->sprt_tx))
		kfree(sprt_dev->sprt_tx);

	kfree(sprt_dev);
}

/*!< end of file */
