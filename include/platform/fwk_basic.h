/*
 * Platform Basic Defines
 *
 * File Name:   fwk_basic.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.05.22
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_BASIC_H_
#define __FWK_BASIC_H_

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The includes */
#include <configs/configs.h>
#include <common/generic.h>
#include <common/list_types.h>
#include <common/trie_tree.h>
#include <common/radix_tree.h>
#include <common/api_string.h>
#include <common/io_stream.h>
#include <common/time.h>
#include <board/board.h>
#include <boot/boot_text.h>
#include <boot/implicit_call.h>
#include <platform/fwk_mempool.h>

/*!< The defines */
enum __ERT_DEVICE_TYPE
{
	NR_TYPE_NONE = 0,

	/*!< character device */
	NR_TYPE_CHRDEV	= 1,
	/*!< block device */
	NR_TYPE_BLKDEV,
	/*!< network device */
	NR_TYPE_NETDEV,
};

enum __ERT_CHRDEV_MAJOR
{
	NR_CHRDEV_DUMMY_MAJOR = 0,

	NR_STDIN_MAJOR,
	NR_STDOUT_MAJOR,
	NR_STDERR_MAJOR,
	NR_DEBUG_MAJOR,

	NR_LED_MAJOR,
	NR_KEY_MAJOR,
	NR_INPUT_MAJOR,
	NR_RTC_MAJOR,
	NR_FBDEV_MAJOR,
	NR_MISC_MAJOR,
	NR_TSC_MAJOR,
	NR_USB_MAJOR,

	NR_CHRDEV_MAJOR_MAX,
};

#define USE_VIRTUAL_MEM_ADDR						(0)						/*!< Whether virtual memory is used. 0: not use, virtual memory = physic memory */

#define DEVICE_MAX_NUM								(255)					/*!< Maximum number of device numbers */
#define DEVICE_MAJOR_BASE							(3)						/*!< The primary device number starts (0, 1, 2 are reserved.)） */
#define DEVICE_NAME_LEN								(32)					/*!< The maximum number of bytes allowed for a device name */

/*!< Maximum number of secondary devices: 2 ^ 20, Maximum number of primary devices: min(DEVICE_MAX_NUM, 2 ^ 12) */
#define DEV_NUM_BITMASK(bits)                   	((1u << (bits)) - 1)
#define DEV_MINOR_BITS                          	(20)
#define DEV_MAJOR_BITS                          	(32 - (DEV_MINOR_BITS))
#define MKE_DEV_NUM(major, minor)               	((kuint32_t)(((major) << (DEV_MINOR_BITS)) | (minor)))
#define GET_DEV_MAJOR(devNum)                   	((kuint32_t)((devNum) >> (DEV_MINOR_BITS)))
#define GET_DEV_MINOR(devNum)                   	((kuint32_t)((devNum) & (DEV_NUM_BITMASK(DEV_MINOR_BITS))))

#define FWK_IOC_NONE								(0U)
#define FWK_IOC_WRITE								(1U)
#define FWK_IOC_READ								(2U)

#define FWK_IOC_NRBITS								(8)
#define FWK_IOC_NRSHIFT	            				(0)
#define FWK_IOC_NRMASK								(0xff)

#define FWK_IOC_TYPEBITS							(8)
#define FWK_IOC_TYPESHIFT	            			(FWK_IOC_NRSHIFT + FWK_IOC_NRBITS)
#define FWK_IOC_TYPEMASK							(0xff)

#define FWK_IOC_SIZEBITS							(14)
#define FWK_IOC_SIZESHIFT	            			(FWK_IOC_TYPESHIFT + FWK_IOC_TYPEBITS)
#define FWK_IOC_SIZEMASK							(0x3fff)

#define FWK_IOC_DIRBITS								(2)
#define FWK_IOC_DIRSHIFT	            			(FWK_IOC_SIZESHIFT + FWK_IOC_SIZEBITS)
#define FWK_IOC_DIRMASK								(0x03)

#define FWK_IOC_TYPECHECK(t)           				sizeof(t)

#define FWK_IOC(dir, type, nr, size) \
	(((dir)  << FWK_IOC_DIRSHIFT) | ((type) << FWK_IOC_TYPESHIFT) | \
	 ((nr)   << FWK_IOC_NRSHIFT)  | ((size) << FWK_IOC_SIZESHIFT))

#define FWK_IOWR(type, nr, size)           			FWK_IOC(FWK_IOC_READ | FWK_IOC_WRITE, (type), (nr), (FWK_IOC_TYPECHECK(size)))
#define FWK_IOW(type, nr, size)           			FWK_IOC(FWK_IOC_WRITE, (type), (nr), (FWK_IOC_TYPECHECK(size)))
#define FWK_IOR(type, nr, size)           			FWK_IOC(FWK_IOC_READ, (type), (nr), (FWK_IOC_TYPECHECK(size)))
#define FWK_ION(type, nr, size)           			FWK_IOC(FWK_IOC_NONE, (type), (nr), (FWK_IOC_TYPECHECK(size)))

#define FWK_IOC_DIR(nr)   							(((nr) >> FWK_IOC_DIRSHIFT) & FWK_IOC_DIRMASK)
#define FWK_IOC_SIZE(nr)   							(((nr) >> FWK_IOC_SIZESHIFT) & FWK_IOC_SIZEMASK)
#define FWK_IOC_TYPE(nr)   							(((nr) >> FWK_IOC_TYPESHIFT) & FWK_IOC_TYPEMASK)
#define FWK_IOC_NR(nr)   							(((nr) >> FWK_IOC_NRSHIFT) & FWK_IOC_NRMASK)

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

/*!< params saved for kernel */
#define TAG_PARAM_VIDEO         0
struct video_params
{
    struct m_area sgrt_hz12x12;
    struct m_area sgrt_hz16x16;
    struct m_area sgrt_hz32x32;
};

#define TAG_PARAM_FDT			1
struct fdt_params
{
	struct m_area sgrt_fdt;
};

struct tag_header
{
    kint32_t type;
    kusize_t size;
};

struct tag_params
{
    struct tag_header sgrt_hdr;

    union
    {
        struct video_params sgrt_vdp;
		struct fdt_params sgrt_fdt;
    } u;
};
#define TAG_PARAM_NEXT(tag) \
            (struct tag_params *)((void *)(tag) + (tag)->sgrt_hdr.size)

/*!< The globals */
extern struct video_params *sprt_fwk_video_params;
extern struct fdt_params *sprt_fwk_fdt_params;

/*!< API functions */
/*!
 * @brief   get mapped address
 * @param   none
 * @retval  none
 * @note    reserved interface (phy_addr ---> virt_addr)
 */
static inline void *fwk_io_remap(void *phy_addr, kusize_t size)
{
	return (phy_addr && size) ? phy_addr : mrt_nullptr;
}

/*!
 * @brief   put mapped address
 * @param   none
 * @retval  none
 * @note    reserved interface (release virt_addr)
 */
static inline void fwk_io_unmap(void *virt_addr)
{

}

#ifdef __cplusplus
    }
#endif

#endif /*!< __FWK_BASIC_H_ */
