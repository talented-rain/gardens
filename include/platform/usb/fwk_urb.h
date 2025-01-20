/*
 * Hardware Abstraction Layer USB Interface
 *
 * File Name:   fwk_usb.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.11.30
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_URB_H_
#define __FWK_URB_H_

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The includes */
#include <platform/fwk_basic.h>

/*!< The defines */
/*!<
 *  USB use NRZI Code (Non-Return to Zero Inverted Code) and Bit Stuffing to indicate data:
 *      Logic 0: D+ and D- level inverted, that is, J state to K state, or K state to J state
 *      Logic 1: D+ and D- level remain unchanged, that is, J state to J state, or K state to K state
 *      When there are 7 consecutive 1s, insert a 0 after the 6th 1; and the receiver will automatically remove the inserted 0 when receiving.
 * 
 *  Packet: 
 *	    SOP + SYNC + Packet Content (PID + Address + Frame Number + Data + CRC) + EOP
 * 	    SOP: start of packet. D+ and D- lines drive from idle state to opposite logic level (J-state to K-state)
 *      SYNC: for Full/Low Speed USB, it's value is 0x01; but for High Speed USB, it is 0x00000001 (K-J-K-J-K-J-K-K)
 *      PID: consists of a 4-digit type field(bit[3:0]) and a 4-digit check field(bit[7:4]). check field = the complement of the type field
 *      Address: Endpoint address filed(bit[10:7]) + Device address filed(bit[6:0])
 *      Frame Number: occupy 11 bits, maximum is 0x7ff
 *      Data: 0 ~ 1024bytes, LSB first
 *      CRC: occupy 5 bits. 
 *              Token CRC = (x ^ 5) + (x ^ 2) + 1
 *              Data  CRC = (x ^ 16) + (x ^ 15) + (x ^ 2) + 1
 *      EOP: end of packet. D+ and D- lines drive to logic low level (SE0, keep 2 bits), and then drive to J state (idle state)
 */
#define FWK_URB_SYNC                                                    (0x01)

/*!< Packet type */
enum __ERT_URB_PID_TYPE
{
/*!< 1. Super packet, bit[1:0] = 0b00 */
#define FWK_URB_SUPER_PACKET_MASK                                       (0x00)
    NR_URB_PidSuperTypeNone = 0x00,                                     /*!< Reserved, not used */
    NR_URB_PidSuperTypePing = 0x04,                                     /*!< Ping test (for Token Packet) */
    NR_URB_PidSuperTypeSplit = 0x08,                                    /*!< Split transaction (for Token Packet) */
    NR_URB_PidSuperTypePre = 0x0c,                                      /*!< Precursor (for Token Packet) */
    NR_URB_PidSuperTypeErr = 0x0c,                                      /*!< Error (for HandShake Packet) */

/*!< 2. Token packet, bit[1:0] = 0b01 */
#define FWK_URB_TOKEN_PACKET_MASK                                       (0x01)
    NR_URB_PidTokenTypeOut = 0x01,                                      /*!< Ask device that host will send data */
    NR_URB_PidTokenTypeIn = 0x09,                                       /*!< Ask device taht host will recieve data */
    NR_URB_PidTokenTypeSof = 0x05,                                      /*!< Start of frame */
    NR_URB_PidTokenTypeSetup = 0x0d,                                    /*!< Control transfer will begin */

/*!< 3. Handshake packet, bit[1:0] = 0b10 */
#define FWK_URB_HANDSHAKE_PACKET_MASK                                   (0x02)
    NR_URB_PidHandShakeTypeAck = 0x02,                                  /*!< Ackonwledge(ACK): transfer completed */
    NR_URB_PidHandShakeTypeNAck = 0x0a,                                 /*!< Not Ackonwledge(NAK): device does not ready to send/recv */
    NR_URB_PidHandShakeTypeNYet = 0x06,                                 /*!< Not ready(NYET/ERR): just for HS device, not ready or error */
    NR_URB_PidHandShakeTypeStall = 0x0e,                                /*!< Suspend: can not transfer */

/*!< 4. Data packet, bit[1:0] = 0b11 */
#define FWK_URB_DATA_PACKET_MASK                                        (0x03)
    NR_URB_PidDataTypeData0 = 0x03,                                     /*!< Data packet of data0 */
    NR_URB_PidDataTypeData1 = 0x0b,                                     /*!< Data packet of data1 */
    NR_URB_PidDataTypeData2 = 0x07,                                     /*!< Data packet of data2 */
    NR_URB_PidDataTypeMData = 0x0f,

/*!< 
 * PID = Origin Code(low 4bit) + Reverse Code(high 4bit), However, the data is transferred from low to high. 
 * For instance, SETUP packet pid is 0x0d(0b1101), it's reverse code is 0x02(0b0010);
 * ===> PID = (0x02 << 4) | (0x0d) = 0x2d(0b00101101)
 * Because low bit will be transferred first, the transmission order is: 10110100 (0xb4)
 */
#define FWK_URB_PID_NUMBER(x)                                           (((TO_REVERSE(x) << 4) & 0xf0) | ((x) & 0x0f))
#define FWK_URB_PID_TRANS_NUMBER(x)                                     (reverse_bit(FWK_URB_PID_NUMBER(x)))

#define FWK_URB_CHECK_PACKET_TYPE(pid)                                  ((pid) & (0x03))
#define FWK_URB_IS_SUPER_PACKET(pid)                                    mrt_is_bitequal(pid, FWK_URB_SUPER_PACKET_MASK)
#define FWK_URB_IS_TOKEN_PACKET(pid)                                    mrt_is_bitequal(pid, FWK_URB_TOKEN_PACKET_MASK)
#define FWK_URB_IS_HANDSHAKE_PACKET(pid)                                mrt_is_bitequal(pid, FWK_URB_HANDSHAKE_PACKET_MASK)
#define FWK_URB_IS_DATA_PACKET(pid)                                     mrt_is_bitequal(pid, FWK_URB_DATA_PACKET_MASK)
};

/*!< Frame number maximum */
#define FWK_URB_FRAME_NUMBER_MAX                                        (0x7ff)

typedef struct fwk_urb_sof
{
    kuint8_t pid;                                                       /*!< Fixed to 0xa5 */
    kuint16_t frame_number;                                             /*!< Add 1 per 1ms for LS/FS; but per 125us for HS */
    kuint8_t crc;

} srt_fwk_urb_sof_t;

/*!<
 * Setup packet(pid is 0xb4): only used in control transfer, notify device that host will send a data0 packet to it's control endpoint.
 * Out packet(pid is 0x87): notify device that host will send a data0/data1 packet to it's endpoint.
 * In packet(pid is 0x96): notify device that host will receive a data0/data1 packet from it's endpoint.
 */
typedef struct fwk_urb_token
{
    kuint8_t pid;
    kuint8_t address;
    kuint8_t endpoint;
    kuint8_t crc;

} srt_fwk_urb_token_t;

/*!<
 * Data0 packet(pid is 0xc3)
 * CRC caculation does not include pid.
 * 
 * 数据切换同步和重传(by data0 and data1)
 * (发送数据前, 主机状态(A) = 设备状态(B) = 0)
 *  1, 正确数据传输同步
 *      a) if (A == 0):
 *          主机发送DATA0给设备; 设备如正确接收, B = 1, 并给主机发送应答包; 主机如正确接收, A = 1
 *      b) if (A == 1):
 *          主机如还要继续发送数据包, 则发送DATA1; 设备如正确接收, B = 0, 并给主机发送应答包; 主机如正确接收, A = 0
 *  2, 数据重传
 *          主机发送DATA0, 若设备发现数据存在问题, 设备状态不变(B仍为0), 并给主机发送非应答数据包, 主机接收到非应答数据包后, 主机状态也不变(A仍为0)
 *          主机一段时间后启动重传, 仍然发送DATA0, 直到设备接收正常, A/B状态翻转
 *  3, 应答包故障
 *          设备正确接收DATA0后给主机发送应答包, 若应答包故障, 主机不做状态变化 ===> 即A = 0, B = 1;
 *          主机一段时间后启动重传, 仍然发送DATA0, 由于设备状态B = 1, 故不会对本次数据包进行接收, 直接给主机发送应答包, 直到应答正常. 主机状态翻转
 *  (A = 0时, 主机发送DATA0; A = 1时, 主机发送DATA1)
 */
typedef struct fwk_urb_data
{
    kuint8_t pid;
    void *ptrData;
    kuint16_t crc;

} srt_fwk_urb_data_t;

/*!< 
 * Handshake packet(just obtain pid)
 * 1) 批量传输/中断传输
 *  for IN transaction:
 *      a) 主机发送IN令牌包给设备;
 *      b) 设备如有数据, 将发送数据给主机; 
 *         若设备未准备好数据, 将发送NAK握手包; 
 *         若设备不能进行传输, 将发送STALL握手包
 *      c) 主机正确收到数据后, 主机会给设备发送应答包ACK, IN事务完成
 *  for OUT transation:
 *      a) 主机发送OUT令牌包给设备;
 *      b) 主机发送DATA数据包给设备;
 *      c) 设备若未准备好接收主机的数据, 则给主机发送NAK握手包; 
 *         若设备不能进行传输, 会给主机发送STALL握手包; 
 *         若设备正确接收数据, 则给主机发送ACK握手包; 
 *         在高速USB(HS)中, 若设备未准备好, 则给主机发送NYET握手包; 
 *         若设备出错, 则发ERR握手包
 *  for PING transation (HS):
 *      a) 主机发送PING令牌包给设备;
 *      b) 若设备正确接收数据, 则给主机发送ACK握手包;
 *         若设备没有准备好发送/接收数据, 则给主机发送NAK握手包;
 *         若设备不能进行传输, 则给主机发送STALL握手包
 * 2) 控制传输
 *      a) 成功: ACK
 *      b) 失败: ERR
 * 3) 同步传输
 *      无握手包
 */
typedef struct fwk_urb_handshake
{
    kuint8_t pid;

} srt_fwk_urb_handshake_t;

typedef struct fwk_urb_packet
{
    kuint8_t sop;
    kuint32_t sync;
    
    union
    {
        struct fwk_urb_sof sgrt_sof;
        struct fwk_urb_token sgrt_token;
        struct fwk_urb_data sgrt_data;
        struct fwk_urb_handshake sgrt_hsk;

    } ugrt_frame;

    kuint8_t eop;

} srt_fwk_urb_packet_t;

/*!< ---------------------------------------------------------------------- */
/*!<
 * Control Transmission
 *  (Consist of 3 transactions)
 *  1) SETUP transaction: SETUP token packet(sgrt_token) + DATA0 packet(sgrt_data) + ACK handshake packet(sgrt_hsk)
 *  2) Data transaction: consist of 0 or more IN/OUT transaction (Bulk transmission)
 *  3) Status transaction: consist of 1 OUT/IN transaction (Bulk transmission)
 */
/*!< SETUP transaction (DATA0) */
typedef struct fwk_urb_setup_data
{
    kuint8_t bmRequestType;                                             /*!< Refer to "__ERT_URB_SETUP_REQTYPE" */
    kuint8_t bRequest;                                                  /*!< Specific request, refer to "__ERT_URB_SETUP_STDREQ" */
    kuint16_t wValue;                                                   /*!< Word-sized field that varies according to request */
    kuint16_t wIndex;                                                   /*!< Word-sized field that varies according to request; typically used to pass an index or offset */
    kuint16_t wLenth;                                                   /*!< Number of bytes to transfer if there is a Data stage(if not, wLenth will be set to 0) */

} srt_fwk_urb_setup_data_t;

/*!< for bmRequestType */
enum __ERT_URB_SETUP_REQTYPE
{
    /*!< Data transfer direction */
    NR_URB_SetupReqTransDir = mrt_bit(7),                          /*!< 0: host to device; 1: device to host */

    /*!< Command Type */
    NR_URB_SetupReqTypeStd = 0,                                    /*!< Standard Command */
    NR_URB_SetupReqTypeClass = mrt_bit(5),                         /*!< Class Request Command */
    NR_URB_SetupReqTypeVendor = mrt_bit(6),                        /*!< User-Defined Command */
    NR_URB_SetupReqTypeResvd = mrt_bit(5) | mrt_bit(6),            /*!< Reserved Command */

    /*!< Recipient Type */
    NR_URB_SetupReqRecDevice = 0,                                  /*!< to device */
    NR_URB_SetupReqRecInterface = mrt_bit(0),                      /*!< to interface */
    NR_URB_SetupReqRecEndpoint = mrt_bit(1),                       /*!< to endpoint */
    NR_URB_SetupReqRecOther = mrt_bit(0) | mrt_bit(1),
};

/*!< for bRequest */
enum __ERT_URB_SETUP_STDREQ
{
    NR_URB_SetupStdReqGetStatus = 0,
    NR_URB_SetupStdReqCreateFeature,
    NR_URB_SetupStdReqResvd1,
    NR_URB_SetupStdReqSetFeature,
    NR_URB_SetupStdReqResvd2,
    NR_URB_SetupStdReqSetAddress,
    NR_URB_SetupStdReqGetDescriptor,
    NR_URB_SetupStdReqSetDescriptor,
    NR_URB_SetupStdReqGetConfig,
    NR_URB_SetupStdReqSetConfig,
    NR_URB_SetupStdReqGetInterface,
    NR_URB_SetupStdReqSetInterface,
    NR_URB_SetupStdReqSynchFrame,
};

/*!< for wIndex */
/*!< Output Endpoint */
#define FWK_URB_SETUP_WINDEX_ED_OUT                                 (0)
/*!< Input Endpoint */
#define FWK_URB_SETUP_WINDEX_ED_IN                                  (0x80)

#define FWK_URB_SETUP_WINDEX_ED_NUMBER(x)                           ((x) & 0x000f)
#define FWK_URB_SETUP_WINDEX_IF_NUMBER(x)                           ((x) & 0x00f0)

/*!
 * 控制传输: USB枚举过程
 * (以下请求名称忽略NR_URB前缀, 如NR_URB_SetupStdReqGetStatus简化为SetupStdReqGetStatus)
 * 	1) 设备上电: USB插入端口或系统启动时设备上电
 * 	2) Hub检测电压变化, 报告主机: hub利用自身中断端点将信息反馈给主机, 告诉主机有设备连接
 * 	3) 主机了解连接设备: 主机发送一个"SetupStdReqGetStatus"请求给hub, 以了解本次状态改变的确切含义(连接 or 断开)
 * 	4) 主机检测所插入的设备是全速还是低速
 * 	5) 主机通过hub复位设备: 新设备连接后, 至少等待100ms以保证插入完成及设备稳定; 
 * 	   主机之后向Hub发送Set_Port_Frame请求, 以复位该设备连接的端口(D+ = D- = 低电平, 持续至少10ms)
 * 	6) 主机进一步检测全速设备是否支持高速模式: HS和FS均默认以FS运行, 若检测到其为FS设备, 将再次进行高速检测. 如支持HS, 则切换到HS
 *     (检测方法为KJ-Chirp序列, 要求主机和设备均支持HS; 切换至HS时, USB设备将断开D+的上拉电路)
 *     (第5步和第9步中, 主机通过发送SE0来复位设备, 若此时设备工作在HS模式, 将重新连接D+的上拉电路, 切换回FS模式)
 * 	7) 通过Hub建立主机和设备之间的信息管道: 主机不停向hub发送"SetupStdReqGetStatus"请求, 以查询设备是否复位成功. 若成功, 设备进入默认状态. 
 * 	   当前通信采用控制传输, 默认管道为: 地址0, 端点0, 总线最大电流100mA
 * 	8) 主机获取默认控制管道的最大数据包长度: 主机使用默认地址0和端点0向设备发送"SetupStdReqGetDescriptor"请求获取设备描述符, 得到bMaxPackSize0
 * 	9) 主机请求Hub再次复位设备
 * 	a) 主机给设备分配一个新地址: 主机发送"SetupStdReqSetAddress"请求向设备发送一个唯一的地址. 设备进入地址状态
 * 	b) 主机获取并解析设备描述符信息: 主机发送"SetupStdReqGetDescriptor"请求读取设备描述符
 * 	c) 主机获取字符串描述符
 * 	d) 主机获取标准配置描述符: 主机发送"SetupStdReqGetConfig"请求来获取标准配置描述符
 * 	e) 主机获取配置描述符集合: 主机根据标准配置描述符的wTotalLength, 发送"SetupStdReqGetConfig"请求获取配置描述符集合
 * 	f) 主机为设备挂载驱动并选择一个配置: 主机发送"SetupStdReqSetConfig"请求来正式确定选择设备的哪个配置作为工作配置(一般只有一个配置被定义). 设备进入配置状态
 */

/*!< ---------------------------------------------------------------------- */
/*!<
 * Isochronous Transmission
 * (Consist of one or more transactions)
 * Note: 
 *      a) A FS device or host controller should be able to accept either DATA0 or DATA1 PIDs in data packets;
 *      b) A FS device or host controller should only send DATA0 PIDs in data packets;
 *      c) A HS host controller must be able to accept and send DATA0, DATA1, DATA2, or MDATA PIDs in data packets;
 *      d) A HS device with at most 1 transaction per microframe must only send DATA0 PIDs in data packets;
 *      e) A HS device with high-bandwith endpoints (e.g., one that has more than 1 transactions per microframe) must be able to accept or send DATA0, DATA1, DATA2, or MDATA PIDs in data packets.
 */

/*!< ---------------------------------------------------------------------- */
/*!<
 * Interrupt Transmission
 * (Consist of one transaction)
 *  中断端点在端点描述符中要报告主机对此端点的查询时间, 主机会保证在小于这个时间间隔的范围内安排一次传输, 如host主机1ms内向鼠标设备请求一次数据.
 *  for FS endpoint, 中断传输的时间间隔在1ms ~ 255ms之间;
 *  for LS endpoint, 时间间隔限制在10ms ~ 255ms之间;
 *  for HS endpoint, 时间间隔为(2 ^ (bInterval - 1)) * 125us, bInterval的值在1 ~ 16之间
 */

typedef struct fwk_urb_class_ops
{
    /*!< Class type*/
    kuint32_t type;

    /*!< Class driver initialization- entry  of the class driver */
    kint32_t (*classInit)(kuint8_t, void *, kuint32_t *);

    /*!< Class driver de-initialization*/
    kint32_t (*classDeinit)(kuint32_t);

    /*!< Class driver event callback*/
    kint32_t (*classEvent)(void *, kuint32_t, void *); 
    
} srt_fwk_urb_class_ops_t;

/*!< USB device controller interface structure */
typedef struct fwk_urb_ops
{
    /*!< Controller initialization */
    kint32_t (*init)(kuint8_t, void *, kuint32_t *);    

    /*!< Controller de-initialization */
    kint32_t (*exit)(kuint32_t);   

    /*!< Controller send data */
    kint32_t (*send)(void *, kuint8_t, kuint8_t *, kuint32_t); 

    /*!< Controller receive data */
    kint32_t (*recv)(void *, kuint8_t, kuint8_t *, kuint32_t);      

    /*!< Controller cancel transfer */
    kint32_t (*cancel)(void *, kuint8_t);   

    /*!< Controller control */
    kint32_t (*ctrl)(void *, kuint32_t, void *); 

} srt_fwk_urb_ops_t;

#ifdef __cplusplus
    }
#endif

#endif /*!< __FWK_URB_H_ */