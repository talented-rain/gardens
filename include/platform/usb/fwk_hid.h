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
    kuint8_t  bLenth;                                           /*!< ���������ֽ������ȣ����¼����������پ����� */
    kuint8_t  bDescriptorType;                                  /*!< ����������: FWK_USB_DT_HID */
    kuint16_t bcdHID;                                           /*!< HIDЭ��汾 */
    kuint8_t  bCountryCode;                                     /*!< ���Ҵ��� */
    kuint8_t  bNumDescriptor;                                   /*!< HID�豸֧�ֵ��¼�������������(Ҳ����HID�豸���еı������������������������ۺ�) */
    kuint8_t  bSubDescriptorType;                               /*!< �¼�����������(��һ�������Ǳ���������: FWK_USB_DT_HID_REPORT) */
    kuint16_t wDescriptorLength;                                /*!< �¼��������ĳ��� */
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



#endif /*!< __FWK_HID_H_ */
