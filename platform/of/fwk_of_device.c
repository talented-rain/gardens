/*
 * Hardware Abstraction Layer Device-Tree / Platform Device Convertion
 *
 * File Name:   fwk_of_device.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.06.14
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <platform/of/fwk_of_device.h>
#include <platform/irq/fwk_irq_domain.h>

/*!< -------------------------------------------------------------------------- */
/*!< The defines */

/*!< -------------------------------------------------------------------------- */
/*!< The globals */

/*!< -------------------------------------------------------------------------- */
/*!< The functions */
static kint32_t fwk_of_platform_default_populate(struct fwk_device_node *sprt_node);
static kint32_t fwk_of_platform_populate(struct fwk_device_node *sprt_node, const struct fwk_of_device_id *sprt_matches);
static kint32_t fwk_of_platform_bus_create(struct fwk_device_node *sprt_node, const struct fwk_of_device_id *sprt_matches, kbool_t strict);
static struct fwk_platdev *fwk_of_platform_device_create_pdata(struct fwk_device_node *sprt_node);

/*!< -------------------------------------------------------------------------- */
/*!< API function */
/*!
 * @brief   fwk_of_platform_populate_init
 * @param   none
 * @retval  none
 * @note    none
 */
kint32_t __plat_init fwk_of_platform_populate_init(void)
{
    return fwk_of_platform_default_populate(mrt_nullptr);
}

/*!< Specify the following nodes to be converted to platform_device */
const struct fwk_of_device_id sgrt_fwk_of_default_bus_match_table[] =
{
    { .compatible = "simple-bus", },
    { .compatible = "simple-mfd", },
    { .compatible = "isa", },

    /*!< Empty terminated list */
    {} 
};

/*!
 * @brief   fwk_of_platform_default_populate
 * @param   none
 * @retval  none
 * @note    The default matching table mode
 */
static kint32_t fwk_of_platform_default_populate(struct fwk_device_node *sprt_node)
{
    return fwk_of_platform_populate(sprt_node, sgrt_fwk_of_default_bus_match_table);
}

/*!
 * @brief   fwk_of_platform_populate
 * @param   none
 * @retval  none
 * @note    Take out each node and convert it to platform_device: You can start the conversion from a specific node
 */
static kint32_t fwk_of_platform_populate(struct fwk_device_node *sprt_node, const struct fwk_of_device_id *sprt_matches)
{
    struct fwk_device_node *sprt_head = mrt_nullptr;
    struct fwk_device_node *sprt_list = mrt_nullptr;
    kint32_t retval;

    sprt_head = isValid(sprt_node) ? sprt_node : fwk_of_find_node_by_path("/");
    retval = -ER_ERROR;

    if (!isValid(sprt_head))
        return -ER_NOMEM;

    /*!< Take out each node */
    FOREACH_CHILD_OF_DT_NODE(sprt_head, sprt_list)
    {
        retval = fwk_of_platform_bus_create(sprt_list, sprt_matches, true);
        if (!retval)
        {
            /*!< do nothing */
        }
    }

    return retval;
}

/*!
 * @brief   fwk_of_platform_bus_create
 * @param   none
 * @retval  none
 * @note    Node properties match
 */
static kint32_t fwk_of_platform_bus_create(struct fwk_device_node *sprt_node, const struct fwk_of_device_id *sprt_matches, kbool_t strict)
{
    struct fwk_device_node *sprt_head;
    struct fwk_device_node *sprt_list;
    struct fwk_platdev *sprt_platdev;
    kint32_t retval;

    /*!< Don't have "compatible" property? The current node does not convert */
    if (strict && (!isValid(fwk_of_get_property(sprt_node, "compatible", mrt_nullptr))))
        return -ER_FAULT;

    /*!< 
     * Check: If this node is a grandson of the root node;
     * Judgment: Converted? Nodes that have already been converted cannot be converted repeatedly 
     */
    /*!< This flag is only used for recursion */
    if (fwk_of_node_check_flag(sprt_node, NR_OfNodePopulatedBus))
        return -ER_FAULT;

    /*!< Convert this node */
    sprt_platdev = fwk_of_platform_device_create_pdata(sprt_node);
    if (!isValid(sprt_platdev))
        return -ER_NOMEM;

    /*!< 
     * Check: Whether you need to convert the child nodes under this node
     * If the value of the compatible attribute is the same as that of one of the matches, 
     * all the children under this node must also be converted to platform device
     */
    if (!fwk_of_match_node(sprt_matches, sprt_node))
        return ER_NORMAL;

    /*!< All child nodes are converted to platform_device */
    sprt_head = sprt_node;

    /*!< Recursion: Handles child nodes */
    FOREACH_CHILD_OF_DT_NODE(sprt_head, sprt_list)
    {
        retval = fwk_of_platform_bus_create(sprt_list, sprt_matches, strict);
        if (!retval)
        {
            /*!< do nothing */
        }
    }

    /*!< This will only be executed if the child nodes under this node also need to be converted */
    fwk_of_node_set_flag(sprt_node, NR_OfNodePopulatedBus);

    return ER_NORMAL;
}

/*!
 * @brief   fwk_of_platform_device_create_pdata
 * @param   none
 * @retval  none
 * @note    Convert the specified node to platform_device and hook it to the bus
 */
static struct fwk_platdev *fwk_of_platform_device_create_pdata(struct fwk_device_node *sprt_node)
{
    struct fwk_platdev *sprt_platdev;
    struct fwk_resources sgrt_resources;
    struct fwk_resources *sprt_resources = mrt_nullptr;
    kint32_t retval;
    kuint32_t i, num_res = 0, num_irq = 0;

    if (!isValid(sprt_node))
        return mrt_nullptr;

    /*!< Check the status attribute of this node, and do not convert the attribute if the attribute value is disabled */
    /*!< 
     * Check: If this node is a child of the root node;
     * Judgment: Converted? Nodes that have already been converted cannot be converted repeatedly 
     */
    if (!fwk_of_device_is_avaliable(sprt_node) || fwk_of_node_check_flag(sprt_node, NR_OfNodePopulated))
        return mrt_nullptr;

    sprt_platdev = (struct fwk_platdev *)kzalloc(sizeof(struct fwk_platdev), GFP_KERNEL);
    if (!isValid(sprt_platdev))
        return mrt_nullptr;

    /*!< Determine the size of sprt_resources to be allocated */
    while (!fwk_of_address_to_resource(sprt_node, num_res, &sgrt_resources))
        num_res++;

    num_irq = fwk_of_irq_count(sprt_node);

    if ((num_res + num_irq) > 0)
    {
        sprt_resources = (struct fwk_resources *)kzalloc((num_res + num_irq) * sizeof(struct fwk_resources), GFP_KERNEL);
        sprt_platdev->num_resources	= num_res + num_irq;
        sprt_platdev->sprt_resources = sprt_resources;

        /*!< Fill sprt_resources */
        for (i = 0; i < num_res; i++)
            fwk_of_address_to_resource(sprt_node, i, (sprt_resources + i));

        /*!< error can be permitted */
        fwk_of_irq_to_resource_table(sprt_node, sprt_resources + num_res, num_irq);
    }

    /*!< Register to the device bus */
    retval = fwk_of_register_platdevice(sprt_node, sprt_platdev);
    if (retval)
    {
        /*!< Hook failed */
        if (isValid(sprt_resources))
            kfree(sprt_resources);

        kfree(sprt_platdev);
        return mrt_nullptr;
    }

    /*!< Stored as private data */
    sprt_node->data	= sprt_platdev;
    /*!< Set the conversion flag */
    fwk_of_node_set_flag(sprt_node, NR_OfNodePopulated);

    return sprt_platdev;
}

/*!
 * @brief   fwk_of_register_platdevice
 * @param   none
 * @retval  none
 * @note    none
 */
kint32_t fwk_of_register_platdevice(struct fwk_device_node *sprt_node, struct fwk_platdev *sprt_platdev)
{
    if (!isValid(sprt_platdev))
        return -ER_FAULT;

    if (fwk_device_initial(&sprt_platdev->sgrt_dev))
        return -ER_FAILD;

    /*!< Fill platform_device */
    sprt_platdev->name = sprt_node->full_name;
    sprt_platdev->id = fwk_of_get_alias_id(sprt_node);

    sprt_platdev->sgrt_dev.init_name = sprt_node->full_name;
    sprt_platdev->sgrt_dev.sprt_node = sprt_node;
    sprt_platdev->sgrt_dev.release = mrt_nullptr;
    sprt_platdev->sgrt_dev.sprt_parent = mrt_nullptr;

    /*!< Bus hook-up */
    sprt_platdev->sgrt_dev.sprt_bus = &sgrt_fwk_platform_bus_type;

    return fwk_device_add(&sprt_platdev->sgrt_dev);
}

/*!
 * @brief   fwk_of_unregister_platdevice
 * @param   none
 * @retval  none
 * @note    none
 */
void fwk_of_unregister_platdevice(struct fwk_device_node *sprt_node)
{
    struct fwk_platdev *sprt_platdev = (struct fwk_platdev *)sprt_node->data;

    if (!isValid(sprt_platdev))
        return;

    /*!< Log out the device from the bus */
    fwk_device_del(&sprt_platdev->sgrt_dev);

    if (isValid(sprt_platdev->sprt_resources))
        kfree(sprt_platdev->sprt_resources);

    kfree(sprt_platdev);
    sprt_node->data	= mrt_nullptr;
}

/*!< -------------------------------------------------------------------------- */
/*!
 * @brief   fwk_of_address_to_resource
 * @param   none
 * @retval  none
 * @note    reg property data conversion
 */
kint32_t fwk_of_address_to_resource(struct fwk_device_node *sprt_node, kuint32_t index, struct fwk_resources *sprt_res)
{
    kuint32_t cells_of_addr, cells_of_size;
    kuint32_t address, size;
    kchar_t *regName;
    kint32_t retval;

    /*!< Get value of "#address-cells" */
    cells_of_addr = fwk_of_n_addr_cells(sprt_node);
    /*!< Get value of "#size-cells" */
    cells_of_size = fwk_of_n_size_cells(sprt_node);

    if ((1 < cells_of_addr) || (1 < cells_of_size))
        return -ER_UNVALID;

    /*!< Search for the reg address */
    retval = fwk_of_property_read_u32_index(sprt_node, "reg", index << 1, &address);
    if (retval)
        return -ER_FAULT;

    /*!< Search for the reg length */
    retval = fwk_of_property_read_u32_index(sprt_node, "reg", (index << 1) + 1, &size);
    if (retval)
        return -ER_FAULT;

    /*!< reg-names are not necessarily defined by the device tree */
    retval = fwk_of_property_read_string_index(sprt_node, "reg-names", index, &regName);
    if (retval)
        regName	= sprt_node->full_name;

    sprt_res->name	 = regName;
    sprt_res->start	 = address;
    sprt_res->end	 = address + size - 1;
    sprt_res->type	|= NR_DEVICE_RESOURCE_MEM;

    return ER_NORMAL;
}

/*!
 * @brief   fwk_of_irq_to_resource_table
 * @param   none
 * @retval  none
 * @note    interrupt property data conversion
 */
kint32_t fwk_of_irq_to_resource_table(struct fwk_device_node *sprt_node, struct fwk_resources *sprt_res, kuint32_t nr_irqs)
{
    kint32_t virq;
    kuint32_t index;
    kchar_t *regName;

    for (index = 0; index < nr_irqs; index++, sprt_res++)
    {
        virq = fwk_irq_of_parse_and_map(sprt_node, index);
        if (virq < 0)
            return -ER_FAILD;

        /*!< reg-names are not necessarily defined by the device tree */
        if (fwk_of_property_read_string_index(sprt_node, "interrupt-names", index, &regName))
            regName	= sprt_node->full_name;

        sprt_res->name	 = regName;
        sprt_res->start	 = virq;
        sprt_res->end	 = virq;
        sprt_res->type	|= NR_DEVICE_RESOURCE_IRQ;
    }

    return ER_NORMAL;
}

/*!
 * @brief   get resources from device_node
 * @param   none
 * @retval  none
 * @note    none
 */
struct fwk_resources *fwk_platform_get_resources(struct fwk_platdev *sprt_pdev, kuint32_t index, kuint32_t type)
{
    struct fwk_resources *sprt_res, *sprt_max;
    kuint32_t cnt = 0;

    if (!isValid(sprt_pdev) || (index > sprt_pdev->num_resources))
        return mrt_nullptr;

    sprt_max = sprt_pdev->sprt_resources + sprt_pdev->num_resources;

    for (sprt_res = sprt_pdev->sprt_resources; sprt_res < sprt_max; sprt_res++)
    {
        cnt += (!!(type & sprt_res->type)) ? 1 : 0;

        /*!< found */
        if (cnt > index)
            return sprt_res;
    }

    return mrt_nullptr;
}

/*!
 * @brief   get reg address resources from device_node
 * @param   none
 * @retval  none
 * @note    none
 */
kuaddr_t fwk_platform_get_address(struct fwk_platdev *sprt_pdev, kuint32_t index)
{
    struct fwk_resources *sprt_res;

    sprt_res = fwk_platform_get_resources(sprt_pdev, index, NR_DEVICE_RESOURCE_MEM);
    return sprt_res ? sprt_res->start : 0;
}

/*!
 * @brief   get irq resources from device_node
 * @param   none
 * @retval  none
 * @note    none
 */
kint32_t fwk_platform_get_irq(struct fwk_platdev *sprt_pdev, kuint32_t index)
{
    struct fwk_resources *sprt_res;

    sprt_res = fwk_platform_get_resources(sprt_pdev, index, NR_DEVICE_RESOURCE_IRQ);
    return sprt_res ? sprt_res->start : -1;
}

/*!
 * @brief   get address from device_node
 * @param   none
 * @retval  none
 * @note    if platform_device doesn't built, you should use it to get address
 */
void *fwk_of_iomap(struct fwk_device_node *sprt_node, kuint32_t index)
{
    struct fwk_resources sgrt_res;

    if (fwk_of_address_to_resource(sprt_node, index, &sgrt_res))
        return mrt_nullptr;

    return (void *)fwk_address_map(&sgrt_res);
}

/*!< end of file */
