/*
 * Hardware Abstraction Layer USB HID Interface
 *
 * File Name:   fwk_hid.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.12.31
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_HID_H_
#define __FWK_HID_H_

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The includes */
#include <platform/fwk_basic.h>
#include "fwk_ch9.h"
#include "fwk_urb.h"

/*!< The defines */
#define FWK_USB_DT_HID                                          (NR_USB_DescTypeHID)
#define FWK_USB_DT_HID_REPORT                                   (NR_USB_DescTypeHIDReport)
#define FWK_USB_HID_CLASS                                       (NR_USB_DevDescClassHID)

#define FWK_USB_HID_SUBCLASS_NONE                               (0x00)
#define FWK_USB_HID_SUBCLASS_GRUB                               (0x01)

/*!< if SubClass is not "FWK_USB_HID_SUBCLASS_GRUB", protocol value is invalid */
#define FWK_USB_HID_PROTOCOL_KEYBOARD                           (0x01)
#define FWK_USB_HID_PROTOCOL_MOUSE                              (0x02)

typedef struct fwk_usb_hid_desc
{
    kuint8_t  bLenth;                                           /*!< 该描述符字节数长度（由下级描述符多少决定） */
    kuint8_t  bDescriptorType;                                  /*!< 描述符类型: FWK_USB_DT_HID */
    kuint16_t bcdHID;                                           /*!< HID协议版本 */
    kuint8_t  bCountryCode;                                     /*!< 国家代码 */
    kuint8_t  bNumDescriptor;                                   /*!< HID设备支持的下级描述符的数量(也就是HID设备特有的报告描述符和物理描述符的综合) */
    kuint8_t  bSubDescriptorType;                               /*!< 下级描述符类型(第一个必须是报告描述符: FWK_USB_DT_HID_REPORT) */
    kuint16_t wDescriptorLength;                                /*!< 下级描述符的长度 */
} srt_fwk_usb_hid_desc_t;

#define FWK_USB_DT_HID_SIZE                                     (9)

/*!< HID Packet */
/*!< for bRequest */
enum __ERT_URB_HID_CLASS_REQ
{
    NR_URB_HidClassReqGetReport = 0x01,
    NR_URB_HidClassReqGetIdle = 0x02,
    NR_URB_HidClassReqGetProtocol = 0x03,
    NR_URB_HidClassReqSetReport = 0x09,
    NR_URB_HidClassReqSetIdle = 0x0a,
    NR_URB_HidClassReqSetProtocol = 0x0b,
};

#ifdef __cplusplus
    }
#endif

#endif /*!< __FWK_HID_H_ */