/*
 * Hardware Abstraction Layer USB Interface
 *
 * File Name:   fwk_ch9.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.11.29
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_CH9_H_
#define __FWK_CH9_H_

/*!< The includes */
#include <platform/fwk_basic.h>

/*!< The defines */
#define USB_DT_ENDIAN_BYTE32(x)								mrt_le32_to_cpu(x)
#define USB_DT_ENDIAN_BYTE16(x)								mrt_le16_to_cpu(x)

/*!< Desciptor types */
enum __ERT_USB_DESC_TYPE
{
    /*!< standard */
    NR_USB_DescTypeDevice = 0x01,
    NR_USB_DescTypeConfig = 0x02,
    NR_USB_DescTypeString = 0x03,
    NR_USB_DescTypeInterface = 0x04,
    NR_USB_DescTypeEndpoint = 0x05,
    NR_USB_DescTypeDevQualiffier = 0x06,
    NR_USB_DescTypeOtherSpeedConfig = 0x07,
    NR_USB_DescTypeInterfacePower = 0x08,
    NR_USB_DescTypeHID = 0x21,
    NR_USB_DescTypeHIDReport = 0x22,
    NR_USB_DescTypePhysical = 0x23,
    NR_USB_DescTypeHub = 0x29,

#define FWK_USB_DT_DEVICE			                		NR_USB_DescTypeDevice
#define FWK_USB_DT_CONFIG			                		NR_USB_DescTypeConfig
#define FWK_USB_DT_STRING			                		NR_USB_DescTypeString
#define FWK_USB_DT_INTERFACE		                		NR_USB_DescTypeInterface
#define FWK_USB_DT_ENDPOINT			                		NR_USB_DescTypeEndpoint
};

/*!<
 * USB����4����, ����������ΪVbus(+5V, Red Line), D-(White Line), D+(Green Line), GND(Black Line)
 * for LS USB, �豸D-������1.5K, ����D+, D-�������ӵ�, LS�豸����ʱ, ����D-���豸D-ǿ������;
 * for HS/FS USB, �豸D+������1.5K, ����D+, D-�����ӵ�, HS/FS�豸����ʱ, ����D+���豸D+ǿ������;
 * ���D+��D-�Ƿ�����, ���ɷֱ�USB�豸����(LS or FS)
 * (D+��������, �������Ƚ���ʶ��ΪFS�豸, ֮��ͨ��"Chirp����"���������ֻ�����ʶ��FS��HS�豸)
 * ��������⵽D+��D-��ƽ����[������һ��ʱ��], ����Ϊ�����豸����, ���������ں��ʵ�ʱ�������״̬���в����ж��豸���ٶ�(LS or FS)
 * 
 * for LS USB:
 * 		����ź�0: D+ == 0 && D- == 1, that is, D+ < 0.3V && D- > 2.8V
 * 		����ź�1: D+ == 1 && D- == 0, that is, D+ > 2.8V && D- < 0.3V
 * 		SE0: D+ == 0 && D- == 0, that is, D+ < 0.3V && D- < 0.3V
 * 		J̬: ����ź�0
 * 		K̬: ����ź�1
 * 		����״̬: J̬
 * for FS USB:
 * 		����ź�0: D+ == 0 && D- == 1, that is, D+ < 0.3V && D- > 2.8V
 * 		����ź�1: D+ == 1 && D- == 0, that is, D+ > 2.8V && D- < 0.3V
 *		SE0: D+ == 0 && D- == 0, that is, D+ < 0.3V && D- < 0.3V
 * 		J̬: ����ź�1
 * 		K̬: ����ź�0
 * 		����״̬: J̬
 * 		��FS�豸�Ĳ���ź�0/1��LS�豸��ͬ, ��J-K״̬�෴. ʵ�������ɵ�ƽ����������, ˭����, ˭����J̬, �����෴�ļ�ΪK̬
 * for HS USB:
 * 		����ź�0: D+ == 0 && D- == 1, that is, (-0.01V < D+ < 0.01V) && ( 0.36V < D- > 0.44V)
 * 		����ź�1: D+ == 1 && D- == 0, that is, ( 0.36V < D+ < 0.44V) && (-0.01V < D- > 0.01V)
 * 		J̬: ����ź�1
 * 		K̬: ����ź�0
 * 		Chirp J̬: ( 0.7V < D+ <  1.1V) && ( 0.7V < D- <  1.1V)
 * 		Chirp K̬: (-0.9V < D+ < -0.5V) && (-0.9V < D+ < -0.5V)
 * 		����״̬: SE0 (D+ == 0 && D- == 0) (ʵ������ΪHSģʽ��, D+��Ͽ�����, ��Ϊ45ŷķ����ӵ�)
 */
/*!< Device Status */
enum __ERT_USB_DEVICE_STATUS
{
    /*!< ����״̬, ��USB�豸�Ƿ����������ӻ�Ͽ� */
    NR_USB_DevStatusAttached = mrt_bit(0),

    /*!< 
     * �ϵ�״̬
     * USB�豸��Դ�������ⲿ, ���֮Ϊ�Թ����豸(self-powerd);
     * USB�豸��Դ��������������, ���֮Ϊ���߹����豸(������������bmAttributes��ʾ�˹��緽ʽ)
     * ֻ�е��豸���ӵ�USB������, ��Vbus��Դ���豸�ϵ�ʱ�豸�Ž����ϵ�״̬
     */
    NR_USB_DevStatusPowered = mrt_bit(1),

    /*!<
     * Ĭ��״̬
     * 1) �豸�����, ���յ��������ߵĸ�λ�ź�֮ǰ, ����Ӧ�κ���������;
     * 2) �豸��λ��ɺ�, �豸����Ĭ��״̬;
     * 3) HS �� FS�豸�ĵ�������һ��, ֻ���豸��λ�ɹ���, HS�豸������Ҫ�ɹ�����Ӧ�ض���������������ȷ��Ϣ
     */
    NR_USB_DevStatusDefault = mrt_bit(2),

    /*!<
     * ��ַ״̬
     * ����USB�豸�����������豸�����ú�ʹ��Ĭ�ϵ�ַ;
     * ÿһ���豸��������λ������������һ��Ψһ�ĵ�ַ, ������Ϻ�, �豸�����ַ״̬;
     * ��USB�豸������ʱ, �豸�ĵ�ַ���ֲ���;
     * �����豸��ǰ�Ƿ������Ψһ��ַ������ʹ��Ĭ�ϵ�ַ, �ڵ�ַ״̬��, USB�豸ֻ��Ӧ��Ĭ�Ϲܵ��ϵ�����
     */
    NR_USB_DevStatusAddress = mrt_bit(3),

    /*!<
     * ����״̬
     * ��ʹ��USB�豸�Ĺ���֮ǰ, �������ø��豸; ������ɺ�, �豸��������״̬
     */
    NR_USB_DevStatusConfigured = mrt_bit(4),

    /*!<
     * ����״̬
     * �豸��ָ��ʱ����û�й۲쵽���������ݴ���ʱ, �Զ��������״̬;
     * ����ʱ, USB�豸�ᱣ���κ�֮ǰ��״̬, �����ϵ�״̬, Ĭ��״̬, ��ַ״̬, ����״̬;
     * �������ӵļ������˿ڹ���ʱ, USB�豸����Ҳ���������״̬;
     * USB�豸��ͨ��Զ�̻��ѵ��ź���������, ���Լ��˳�����״̬. ��������������, USB�豸�Ὣ�Ƿ�֧��Զ�̻��ѵ��������������
     */
    NR_USB_DevStatusSuspended = mrt_bit(5),

    /*!< ��������, ��ʱ�������豸���������շ����� */
    NR_USB_DevStatusNormal = (NR_USB_DevStatusAttached | NR_USB_DevStatusPowered |
                                NR_USB_DevStatusDefault | NR_USB_DevStatusAddress | NR_USB_DevStatusConfigured),
};

/*!< 1. Device descriptor */
typedef struct fwk_usb_device_desc
{
    kuint8_t  bLength;                                      /*!< ���ṹ���С */
    kuint8_t  bDescriptorType;                              /*!< ����������: FWK_USB_DT_DEVICE */

    kuint16_t bcdUSB;                                       /*!< usb�汾��, ��ʽΪ0xJJMN(JJ-��Ҫ�汾��; M: ��Ҫ�汾��; N: ��Ҫ�汾��). ��USB3.11: �汾�� = 0x0311 */
    kuint8_t  bDeviceClass;                                 /*!< �豸�� */
    kuint8_t  bDeviceSubClass;                              /*!< �豸������ */
    kuint8_t  bDeviceProtocol;                              /*!< �豸Э�� */
    kuint8_t  bMaxPacketSize0;                              /*!< �˵�0������С(����Ϊ8/16/32/64����֮һ) */
    kuint16_t idVendor;                                     /*!< ����id, ��USBЭ�����; ������Ҫ��USB���� */
    kuint16_t idProduct;                                    /*!< ��Ʒid, �����Զ��� */
    kuint16_t bcdDevice;                                    /*!< �豸�������(�汾��), �����Զ��� */
    kuint8_t  iManufacturer;                                /*!< �豸�����ַ�������. Ϊ0��ʾ�� */
    kuint8_t  iProduct;                                     /*!< ��Ʒ����. Ϊ0��ʾ�� */
    kuint8_t  iSerialNumber;                                /*!< �豸���к��ַ�������. Ϊ0��ʾ�� */
    kuint8_t  bNumConfigurations;                           /*!< ���õĸ���. һ��Ϊ1 */

} __packed srt_fwk_usb_device_desc_t;

/*!< for bLength */
#define FWK_USB_DT_DEVICE_SIZE		                		(18)

/*!< for bDeviceClass */
enum __ERT_USB_DEVICE_DESC_CLASS
{
    NR_USB_DevDescClassPerInterface = 0x00,
    NR_USB_DevDescClassAudio = 0x01,
    NR_USB_DevDescClassComm = 0x02,
    NR_USB_DevDescClassHID = 0x03,
    NR_USB_DevDescClassPhysical = 0x05,
    NR_USB_DevDescClassStillImage = 0x06,
    NR_USB_DevDescClassPrinter = 0x07,
    NR_USB_DevDescClassMassStorage = 0x08,
    NR_USB_DevDescClassHUB = 0x09,
    NR_USB_DevDescClassCdcData = 0x0a,
    NR_USB_DevDescClassCscid = 0x0b,
    NR_USB_DevDescClassContentSec = 0x0d,
    NR_USB_DevDescClassVideo = 0x0e,
    NR_USB_DevDescClassWirelessCtrl = 0xe0,
    NR_USB_DevDescClassMisc = 0xef,
    NR_USB_DevDescClassAppSpec = 0xfe,
    NR_USB_DevDescClassVendorSpec = 0xff,
};

/*!<
 * for bMaxPacketSize0
 * �˵�0�������ɴ����ֽ���, �������Ĵ�С. USBЭ��涨, �˵�0���8�ֽ�, �˵��������С��USB�ٶȵȼ������������й�.
 * ���ƴ���һ��ʹ�ö˵�0, �������8�ֽ�, ȫ�ٺ͸��������64�ֽ�
 * 
 * HS: High Speed
 * FS: Full Speed
 * LS: Low  Speed
 */
#define FWK_USB_CTRL_HS_MAX_PACKET_SIZE						(64)
#define FWK_USB_CTRL_FS_MAX_PACKET_SIZE						(64)
#define FWK_USB_CTRL_LS_MAX_PACKET_SIZE						(8)
#define FWK_USB_BULK_HS_MAX_PACKET_SIZE						(512)
#define FWK_USB_BULK_FS_MAX_PACKET_SIZE						(64)
#define FWK_USB_INTR_HS_MAX_PACKET_SIZE						(1024)
#define FWK_USB_INTR_FS_MAX_PACKET_SIZE						(64)
#define FWK_USB_INTR_LS_MAX_PACKET_SIZE						(8)
#define FWK_USB_ISOC_HS_MAX_PACKET_SIZE						(1024)
#define FWK_USB_ISOC_FS_MAX_PACKET_SIZE						(1023)

/*!< 2. Configure  descriptor */
typedef struct fwk_usb_config_desc
{
    kuint8_t  bLength;                                      /*!< ���ṹ���С */
    kuint8_t  bDescriptorType;                              /*!< ����������: FWK_USB_DT_CONFIG */

    kuint16_t wTotalLength;                                 /*!< ��������, ��Ϣ���ܳ��� (���������������ܳ���) */
    kuint8_t  bNumInterfaces;                               /*!< �ӿڵĸ��� */
    kuint8_t  bConfigurationValue;                          /*!< Set_Configuration��������Ĳ���ֵ, ���ڱ�ʾ��ǰ�Ǽ������� */
    kuint8_t  iConfiguration;                               /*!< ���������õ��ַ���������ֵ. 0��ʾ���ַ��� */
    kuint8_t  bmAttributes;                                 /*!< ����ģʽ��ѡ�� */
    kuint8_t  bMaxPower;                                    /*!< �豸��������ȡ�������� */

} __packed srt_fwk_usb_config_desc_t;

/*!< for bLength */
#define FWK_USB_DT_CONFIG_SIZE		                		(9)

/*!< for bmAttributes */
enum __ERT_USB_CONFIG_DESC_ATTR
{
    NR_USB_ConfigDescAttrBattery = mrt_bit(4),

    /*!< 1: �豸֧��Զ�̻��� */
    NR_USB_ConfigDescAttrWakeUp = mrt_bit(5),

    /*!< 0: �Թ���; 1: ���߹��� */
    NR_USB_ConfigDescAttrSelfPower = mrt_bit(6),

    /*!< ����λ, Ĭ��Ϊ1 */
    NR_USB_ConfigDescAttrDefault = mrt_bit(7),
};

/*!< for bMaxPower */
/*!< ��λ: 2mA, ��bMaxPower = x / 2mA */
#define FWK_USB_DT_CONFIG_MAX_CURRENT(x)					((x) >> 1)

/*!< 3. Interface descriptor */
typedef struct fwk_usb_interface_desc
{
    kuint8_t  bLength;                                      /*!< ���ṹ���С */
    kuint8_t  bDescriptorType;                              /*!< ����������: FWK_USB_DT_INTERFACE */

    kuint8_t  bInterfaceNumber;                             /*!< �ýӿڵı��(��0��ʼ) */
    kuint8_t  bAlternateSetting;                            /*!< ���õĽӿ����������. һ�㲻��, Ϊ0���� */
    kuint8_t  bNumEndpoints;                                /*!< �ýӿ�ʹ�õĶ˵������������˵�0 */
    kuint8_t  bInterfaceClass;                              /*!< �ýӿ�ʹ�õ��� */
    kuint8_t  bInterfaceSubClass;                           /*!< �ýӿ�ʹ�õ����� */
    kuint8_t  bInterfaceProtocol;                           /*!< �ýӿ�ʹ�õ�Э�� */
    kuint8_t  iInterface;                                   /*!< �����ýӿڵ��ַ�������ֵ. 0��ʾ�� */

} __packed srt_fwk_usb_interface_desc_t;

/*!< for bLength */
#define FWK_USB_DT_INTERFACE_SIZE		            		(9)

/*!< 4. Endpoint descriptor */
typedef struct fwk_usb_endpoint_desc
{
    kuint8_t  bLength;                                      /*!< �˵��������ֽڴ�С(FWK_USB_DT_ENDPOINT_SIZE) */
    kuint8_t  bDescriptorType;                              /*!< ���������ͱ��: FWK_USB_DT_ENDPOINT */

    kuint8_t  bEndpointAddress;                             /*!< �˵��ַ������������� */
    kuint8_t  bmAttributes;                                 /*!< ����, �����˵�Ĵ�������: ����, �ж�, ����, ͬ�� */
    kuint16_t wMaxPacketSize;                               /*!< �˵���/����������С */
    kuint8_t  bInterval;                                    /*!< ������ѯ�˵��ʱ���� */

    /*!<
     * These two are only in audio endpoints,
     * use FWK_USB_DT_ENDPOINT_SIZE * _SIZE in bLength, not sizeof
     */
    kuint8_t  bRefresh;
    kuint8_t  bSynchAddress;

} __packed srt_fwk_usb_endpoint_desc_t;

/*!< for bLength */
#define FWK_USB_DT_ENDPOINT_SIZE                    		(7)
#define FWK_USB_DT_ENDPOINT_AUDIO_SIZE              		(9)	    /*!< Audio extension */

/*!< 
 * for bEndpointAddress
 * bit[3:0]: �˵���
 * bit[6:4]: ����, Ĭ��Ϊ0
 * bit7: ���ݴ��䷽ʽ(���ƶ˵㲻��������), 0: output; 1: input
 */
/*!< endpoint number: 0 ~ 15 */
#define FWK_USB_ENDPOINT_NUMBER_MASK                		(0x0f)  
#define FWK_USB_ENDPOINT_NUMBER(x)							((x) & FWK_USB_ENDPOINT_NUMBER_MASK)

/*!< direction; 1: input; 0: output */
enum __ERT_USB_ENDPOINT_DIRECTION
{
    NR_USB_EndpointAddrDirOut = 0,
    NR_USB_EndpointAddrDirIn,

#define FWK_USB_ENDPOINT_DIR_MASK                   		(0x80)  
#define FWK_USB_ENDPOINT_DIR(x)                   			(((x) << 7) & FWK_USB_ENDPOINT_DIR_MASK)
#define FWK_USB_ENDPOINT_DIR_IN                   			FWK_USB_ENDPOINT_DIR(NR_USB_EndpointAddrDirIn)
#define FWK_USB_ENDPOINT_DIR_OUT                   			FWK_USB_ENDPOINT_DIR(NR_USB_EndpointAddrDirOut)
};

#define FWK_USB_ENDPOINT_ADDRESS(dir, number)				(FWK_USB_ENDPOINT_DIR(dir) + FWK_USB_ENDPOINT_NUMBER(number))

/*!< 
 * for bmAttributes
 * bit[1:0]: ��������
 * bit[7:2]: ...
 */
enum __ERT_USB_ENDPOINT_DESC_ATTR
{
    /*!< Control: ���ƴ��� */
    NR_USB_EndPointDescXferCtrl = mrt_bit(0),

    /*!< Isochronous: ͬ������ */
    NR_USB_EndPointDescXferIsoc = mrt_bit(1),

    /*!< Bulk: �������� */
    NR_USB_EndPointDescXferBulk = mrt_bit(2),

    /*!< Interrupt: �жϴ��� */
    NR_USB_EndPointDescXferInterrupt = mrt_bit(0) | mrt_bit(1),

#define FWK_USB_ENDPOINT_XFERTYPE_MASK	            		(0x03)
};


#endif /*!< __FWK_CH9_H_ */
