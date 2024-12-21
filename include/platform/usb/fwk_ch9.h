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
 * USB共有4根线, 从左到右依次为Vbus(+5V, Red Line), D-(White Line), D+(Green Line), GND(Black Line)
 * for LS USB, 设备D-端上拉1.5K, 主机D+, D-均下拉接地, LS设备接入时, 主机D-被设备D-强制拉高;
 * for HS/FS USB, 设备D+端上拉1.5K, 主机D+, D-下拉接地, HS/FS设备接入时, 主机D+被设备D+强制拉高;
 * 检测D+或D-是否被拉高, 即可分辨USB设备类型(LS or FS)
 * (D+若被拉高, 主机会先将其识别为FS设备, 之后通过"Chirp序列"的总线握手机制来识别FS和HS设备)
 * 当主机检测到D+或D-电平拉高[保持了一段时间], 就认为存在设备连接, 主机必须在合适的时间对总线状态进行采样判断设备的速度(LS or FS)
 * 
 * for LS USB:
 * 		差分信号0: D+ == 0 && D- == 1, that is, D+ < 0.3V && D- > 2.8V
 * 		差分信号1: D+ == 1 && D- == 0, that is, D+ > 2.8V && D- < 0.3V
 * 		SE0: D+ == 0 && D- == 0, that is, D+ < 0.3V && D- < 0.3V
 * 		J态: 差分信号0
 * 		K态: 差分信号1
 * 		空闲状态: J态
 * for FS USB:
 * 		差分信号0: D+ == 0 && D- == 1, that is, D+ < 0.3V && D- > 2.8V
 * 		差分信号1: D+ == 1 && D- == 0, that is, D+ > 2.8V && D- < 0.3V
 *		SE0: D+ == 0 && D- == 0, that is, D+ < 0.3V && D- < 0.3V
 * 		J态: 差分信号1
 * 		K态: 差分信号0
 * 		空闲状态: J态
 * 		即FS设备的差分信号0/1与LS设备相同, 但J-K状态相反. 实际上是由电平上拉决定的, 谁上拉, 谁就是J态, 与其相反的即为K态
 * for HS USB:
 * 		差分信号0: D+ == 0 && D- == 1, that is, (-0.01V < D+ < 0.01V) && ( 0.36V < D- > 0.44V)
 * 		差分信号1: D+ == 1 && D- == 0, that is, ( 0.36V < D+ < 0.44V) && (-0.01V < D- > 0.01V)
 * 		J态: 差分信号1
 * 		K态: 差分信号0
 * 		Chirp J态: ( 0.7V < D+ <  1.1V) && ( 0.7V < D- <  1.1V)
 * 		Chirp K态: (-0.9V < D+ < -0.5V) && (-0.9V < D+ < -0.5V)
 * 		空闲状态: SE0 (D+ == 0 && D- == 0) (实际是因为HS模式下, D+会断开上拉, 改为45欧姆电阻接地)
 */
/*!< Device Status */
enum __ERT_USB_DEVICE_STATUS
{
    /*!< 连接状态, 即USB设备是否与主机连接或断开 */
    NR_USB_DevStatusAttached = mrt_bit(0),

    /*!< 
     * 上电状态
     * USB设备电源若来自外部, 则称之为自供电设备(self-powerd);
     * USB设备电源若来自自身集线器, 则称之为总线供电设备(配置描述符的bmAttributes表示了供电方式)
     * 只有当设备连接到USB总线上, 且Vbus电源给设备上电时设备才进入上电状态
     */
    NR_USB_DevStatusPowered = mrt_bit(1),

    /*!<
     * 默认状态
     * 1) 设备供电后, 在收到来自总线的复位信号之前, 不响应任何总线事务;
     * 2) 设备复位完成后, 设备进入默认状态;
     * 3) HS 和 FS设备的电气环境一样, 只是设备复位成功后, HS设备还必须要成功地响应特定描述符并返回正确信息
     */
    NR_USB_DevStatusDefault = mrt_bit(2),

    /*!<
     * 地址状态
     * 所有USB设备在最初供电或设备被重置后使用默认地址;
     * 每一个设备在正常复位后由主机分配一个唯一的地址, 分配完毕后, 设备进入地址状态;
     * 当USB设备被挂起时, 设备的地址保持不变;
     * 无论设备当前是否分配了唯一地址或正在使用默认地址, 在地址状态下, USB设备只响应其默认管道上的请求
     */
    NR_USB_DevStatusAddress = mrt_bit(3),

    /*!<
     * 配置状态
     * 在使用USB设备的功能之前, 必须配置该设备; 配置完成后, 设备进入配置状态
     */
    NR_USB_DevStatusConfigured = mrt_bit(4),

    /*!<
     * 挂起状态
     * 设备在指定时间内没有观察到总线有数据传输时, 自动进入挂起状态;
     * 挂起时, USB设备会保存任何之前的状态, 包括上电状态, 默认状态, 地址状态, 配置状态;
     * 在所连接的集线器端口挂起时, USB设备自身也将进入挂起状态;
     * USB设备可通过远程唤醒电信号请求主机, 让自己退出挂起状态. 在配置描述符中, USB设备会将是否支持远程唤醒的能力报告给主机
     */
    NR_USB_DevStatusSuspended = mrt_bit(5),

    /*!< 正常运行, 此时主机和设备可以正常收发数据 */
    NR_USB_DevStatusNormal = (NR_USB_DevStatusAttached | NR_USB_DevStatusPowered |
                                NR_USB_DevStatusDefault | NR_USB_DevStatusAddress | NR_USB_DevStatusConfigured),
};

/*!< 1. Device descriptor */
typedef struct fwk_usb_device_desc
{
    kuint8_t  bLength;                                      /*!< 本结构体大小 */
    kuint8_t  bDescriptorType;                              /*!< 描述符类型: FWK_USB_DT_DEVICE */

    kuint16_t bcdUSB;                                       /*!< usb版本号, 格式为0xJJMN(JJ-主要版本号; M: 次要版本号; N: 次要版本号). 如USB3.11: 版本号 = 0x0311 */
    kuint8_t  bDeviceClass;                                 /*!< 设备类 */
    kuint8_t  bDeviceSubClass;                              /*!< 设备类子类 */
    kuint8_t  bDeviceProtocol;                              /*!< 设备协议 */
    kuint8_t  bMaxPacketSize0;                              /*!< 端点0最大包大小(仅可为8/16/32/64其中之一) */
    kuint16_t idVendor;                                     /*!< 厂家id, 由USB协议分配; 厂家需要向USB申请 */
    kuint16_t idProduct;                                    /*!< 产品id, 厂家自定义 */
    kuint16_t bcdDevice;                                    /*!< 设备出厂编号(版本号), 厂家自定义 */
    kuint8_t  iManufacturer;                                /*!< 设备厂商字符串索引. 为0表示无 */
    kuint8_t  iProduct;                                     /*!< 产品描述. 为0表示无 */
    kuint8_t  iSerialNumber;                                /*!< 设备序列号字符串索引. 为0表示无 */
    kuint8_t  bNumConfigurations;                           /*!< 配置的个数. 一般为1 */

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
 * 端点0单次最大可传的字节数, 即单包的大小. USB协议规定, 端点0最低8字节, 端点的最大传输大小和USB速度等级及传输类型有关.
 * 控制传输一般使用端点0, 低速最大8字节, 全速和高速最大传输64字节
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
    kuint8_t  bLength;                                      /*!< 本结构体大小 */
    kuint8_t  bDescriptorType;                              /*!< 描述符类型: FWK_USB_DT_CONFIG */

    kuint16_t wTotalLength;                                 /*!< 该配置下, 信息的总长度 (配置描述符集合总长度) */
    kuint8_t  bNumInterfaces;                               /*!< 接口的个数 */
    kuint8_t  bConfigurationValue;                          /*!< Set_Configuration命令所需的参数值, 用于表示当前是几个配置 */
    kuint8_t  iConfiguration;                               /*!< 描述该配置的字符串的索引值. 0表示无字符串 */
    kuint8_t  bmAttributes;                                 /*!< 供电模式的选择 */
    kuint8_t  bMaxPower;                                    /*!< 设备从总线提取的最大电流 */

} __packed srt_fwk_usb_config_desc_t;

/*!< for bLength */
#define FWK_USB_DT_CONFIG_SIZE		                		(9)

/*!< for bmAttributes */
enum __ERT_USB_CONFIG_DESC_ATTR
{
    NR_USB_ConfigDescAttrBattery = mrt_bit(4),

    /*!< 1: 设备支持远程唤醒 */
    NR_USB_ConfigDescAttrWakeUp = mrt_bit(5),

    /*!< 0: 自供电; 1: 总线供电 */
    NR_USB_ConfigDescAttrSelfPower = mrt_bit(6),

    /*!< 保留位, 默认为1 */
    NR_USB_ConfigDescAttrDefault = mrt_bit(7),
};

/*!< for bMaxPower */
/*!< 单位: 2mA, 故bMaxPower = x / 2mA */
#define FWK_USB_DT_CONFIG_MAX_CURRENT(x)					((x) >> 1)

/*!< 3. Interface descriptor */
typedef struct fwk_usb_interface_desc
{
    kuint8_t  bLength;                                      /*!< 本结构体大小 */
    kuint8_t  bDescriptorType;                              /*!< 描述符类型: FWK_USB_DT_INTERFACE */

    kuint8_t  bInterfaceNumber;                             /*!< 该接口的编号(从0开始) */
    kuint8_t  bAlternateSetting;                            /*!< 备用的接口描述符编号. 一般不用, 为0即可 */
    kuint8_t  bNumEndpoints;                                /*!< 该接口使用的端点数，不包括端点0 */
    kuint8_t  bInterfaceClass;                              /*!< 该接口使用的类 */
    kuint8_t  bInterfaceSubClass;                           /*!< 该接口使用的子类 */
    kuint8_t  bInterfaceProtocol;                           /*!< 该接口使用的协议 */
    kuint8_t  iInterface;                                   /*!< 描述该接口的字符串索引值. 0表示无 */

} __packed srt_fwk_usb_interface_desc_t;

/*!< for bLength */
#define FWK_USB_DT_INTERFACE_SIZE		            		(9)

/*!< 4. Endpoint descriptor */
typedef struct fwk_usb_endpoint_desc
{
    kuint8_t  bLength;                                      /*!< 端点描述符字节大小(FWK_USB_DT_ENDPOINT_SIZE) */
    kuint8_t  bDescriptorType;                              /*!< 描述符类型编号: FWK_USB_DT_ENDPOINT */

    kuint8_t  bEndpointAddress;                             /*!< 端点地址及输入输出属性 */
    kuint8_t  bmAttributes;                                 /*!< 属性, 包含端点的传输类型: 控制, 中断, 批量, 同步 */
    kuint16_t wMaxPacketSize;                               /*!< 端点收/发的最大包大小 */
    kuint8_t  bInterval;                                    /*!< 主机查询端点的时间间隔 */

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
 * bit[3:0]: 端点编号
 * bit[6:4]: 保留, 默认为0
 * bit7: 数据传输方式(控制端点不考虑在内), 0: output; 1: input
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
 * bit[1:0]: 传输类型
 * bit[7:2]: ...
 */
enum __ERT_USB_ENDPOINT_DESC_ATTR
{
    /*!< Control: 控制传输 */
    NR_USB_EndPointDescXferCtrl = mrt_bit(0),

    /*!< Isochronous: 同步传输 */
    NR_USB_EndPointDescXferIsoc = mrt_bit(1),

    /*!< Bulk: 批量传输 */
    NR_USB_EndPointDescXferBulk = mrt_bit(2),

    /*!< Interrupt: 中断传输 */
    NR_USB_EndPointDescXferInterrupt = mrt_bit(0) | mrt_bit(1),

#define FWK_USB_ENDPOINT_XFERTYPE_MASK	            		(0x03)
};


#endif /*!< __FWK_CH9_H_ */
