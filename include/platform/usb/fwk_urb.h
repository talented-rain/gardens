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
    NR_URB_PidSuperTypeNone = 0x00,                                /*!< Reserved, not used */
    NR_URB_PidSuperTypePing = 0x04,                                /*!< Ping test (for Token Packet) */
    NR_URB_PidSuperTypeSplit = 0x08,                               /*!< Split transaction (for Token Packet) */
    NR_URB_PidSuperTypePre = 0x0c,                                 /*!< Precursor (for Token Packet) */
    NR_URB_PidSuperTypeErr = 0x0c,                                 /*!< Error (for HandShake Packet) */

/*!< 2. Token packet, bit[1:0] = 0b01 */
#define FWK_URB_TOKEN_PACKET_MASK                                       (0x01)
    NR_URB_PidTokenTypeOut = 0x01,                                 /*!< Ask device that host will send data */
    NR_URB_PidTokenTypeIn = 0x09,                                  /*!< Ask device taht host will recieve data */
    NR_URB_PidTokenTypeSof = 0x05,                                 /*!< Start of frame */
    NR_URB_PidTokenTypeSetup = 0x0d,                               /*!< Control transfer will begin */

/*!< 3. Handshake packet, bit[1:0] = 0b10 */
#define FWK_URB_HANDSHAKE_PACKET_MASK                                   (0x02)
    NR_URB_PidHandShakeTypeAck = 0x02,                             /*!< Ackonwledge(ACK): transfer completed */
    NR_URB_PidHandShakeTypeNAck = 0x0a,                            /*!< Not Ackonwledge(NAK): device does not ready to send/recv */
    NR_URB_PidHandShakeTypeNYet = 0x06,                            /*!< Not ready(NYET/ERR): just for HS device, not ready or error */
    NR_URB_PidHandShakeTypeStall = 0x0e,                           /*!< Suspend: can not transfer */

/*!< 4. Data packet, bit[1:0] = 0b11 */
#define FWK_URB_DATA_PACKET_MASK                                       (0x03)
    NR_URB_PidDataTypeData0 = 0x03,                                /*!< Data packet of data0 */
    NR_URB_PidDataTypeData1 = 0x0b,                                /*!< Data packet of data1 */
    NR_URB_PidDataTypeData2 = 0x07,                                /*!< Data packet of data2 */
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
 * �����л�ͬ�����ش�(by data0 and data1)
 * (��������ǰ, ����״̬(A) = �豸״̬(B) = 0)
 *  1, ��ȷ���ݴ���ͬ��
 *      a) if (A == 0):
 *          ��������DATA0���豸; �豸����ȷ����, B = 1, ������������Ӧ���; ��������ȷ����, A = 1
 *      b) if (A == 1):
 *          �����绹Ҫ�����������ݰ�, ����DATA1; �豸����ȷ����, B = 0, ������������Ӧ���; ��������ȷ����, A = 0
 *  2, �����ش�
 *          ��������DATA0, ���豸�������ݴ�������, �豸״̬����(B��Ϊ0), �����������ͷ�Ӧ�����ݰ�, �������յ���Ӧ�����ݰ���, ����״̬Ҳ����(A��Ϊ0)
 *          ����һ��ʱ��������ش�, ��Ȼ����DATA0, ֱ���豸��������, A/B״̬��ת
 *  3, Ӧ�������
 *          �豸��ȷ����DATA0�����������Ӧ���, ��Ӧ�������, ��������״̬�仯 ===> ��A = 0, B = 1;
 *          ����һ��ʱ��������ش�, ��Ȼ����DATA0, �����豸״̬B = 1, �ʲ���Ա������ݰ����н���, ֱ�Ӹ���������Ӧ���, ֱ��Ӧ������. ����״̬��ת
 *  (A = 0ʱ, ��������DATA0; A = 1ʱ, ��������DATA1)
 */
typedef struct fwk_urb_data
{
    kuint8_t pid;
    void *ptrData;
    kuint16_t crc;

} srt_fwk_urb_data_t;

/*!< 
 * Handshake packet(just obtain pid)
 * 1) ��������/�жϴ���
 *  for IN transaction:
 *      a) ��������IN���ư����豸;
 *      b) �豸��������, ���������ݸ�����; 
 *         ���豸δ׼��������, ������NAK���ְ�; 
 *         ���豸���ܽ��д���, ������STALL���ְ�
 *      c) ������ȷ�յ����ݺ�, ��������豸����Ӧ���ACK, IN�������
 *  for OUT transation:
 *      a) ��������OUT���ư����豸;
 *      b) ��������DATA���ݰ����豸;
 *      c) �豸��δ׼���ý�������������, �����������NAK���ְ�; 
 *         ���豸���ܽ��д���, �����������STALL���ְ�; 
 *         ���豸��ȷ��������, �����������ACK���ְ�; 
 *         �ڸ���USB(HS)��, ���豸δ׼����, �����������NYET���ְ�; 
 *         ���豸����, ��ERR���ְ�
 *  for PING transation (HS):
 *      a) ��������PING���ư����豸;
 *      b) ���豸��ȷ��������, �����������ACK���ְ�;
 *         ���豸û��׼���÷���/��������, �����������NAK���ְ�;
 *         ���豸���ܽ��д���, �����������STALL���ְ�
 * 2) ���ƴ���
 *      a) �ɹ�: ACK
 *      b) ʧ��: ERR
 * 3) ͬ������
 *      �����ְ�
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
 * ���ƴ���: USBö�ٹ���
 * (�����������ƺ���NR_URBǰ׺, ��NR_URB_SetupStdReqGetStatus��ΪSetupStdReqGetStatus)
 * 	1) �豸�ϵ�: USB����˿ڻ�ϵͳ����ʱ�豸�ϵ�
 * 	2) Hub����ѹ�仯, ��������: hub���������ж϶˵㽫��Ϣ����������, �����������豸����
 * 	3) �����˽������豸: ��������һ��"SetupStdReqGetStatus"�����hub, ���˽Ȿ��״̬�ı��ȷ�к���(���� or �Ͽ�)
 * 	4) ���������������豸��ȫ�ٻ��ǵ���
 * 	5) ����ͨ��hub��λ�豸: ���豸���Ӻ�, ���ٵȴ�100ms�Ա�֤������ɼ��豸�ȶ�; 
 * 	   ����֮����Hub����Set_Port_Frame����, �Ը�λ���豸���ӵĶ˿�(D+ = D- = �͵�ƽ, ��������10ms)
 * 	6) ������һ�����ȫ���豸�Ƿ�֧�ָ���ģʽ: HS��FS��Ĭ����FS����, ����⵽��ΪFS�豸, ���ٴν��и��ټ��. ��֧��HS, ���л���HS
 *     (��ⷽ��ΪKJ-Chirp����, Ҫ���������豸��֧��HS; �л���HSʱ, USB�豸���Ͽ�D+��������·)
 *     (��5���͵�9����, ����ͨ������SE0����λ�豸, ����ʱ�豸������HSģʽ, ����������D+��������·, �л���FSģʽ)
 * 	7) ͨ��Hub�����������豸֮�����Ϣ�ܵ�: ������ͣ��hub����"SetupStdReqGetStatus"����, �Բ�ѯ�豸�Ƿ�λ�ɹ�. ���ɹ�, �豸����Ĭ��״̬. 
 * 	   ��ǰͨ�Ų��ÿ��ƴ���, Ĭ�Ϲܵ�Ϊ: ��ַ0, �˵�0, ����������100mA
 * 	8) ������ȡĬ�Ͽ��ƹܵ���������ݰ�����: ����ʹ��Ĭ�ϵ�ַ0�Ͷ˵�0���豸����"SetupStdReqGetDescriptor"�����ȡ�豸������, �õ�bMaxPackSize0
 * 	9) ��������Hub�ٴθ�λ�豸
 * 	a) �������豸����һ���µ�ַ: ��������"SetupStdReqSetAddress"�������豸����һ��Ψһ�ĵ�ַ. �豸�����ַ״̬
 * 	b) ������ȡ�������豸��������Ϣ: ��������"SetupStdReqGetDescriptor"�����ȡ�豸������
 * 	c) ������ȡ�ַ���������
 * 	d) ������ȡ��׼����������: ��������"SetupStdReqGetConfig"��������ȡ��׼����������
 * 	e) ������ȡ��������������: �������ݱ�׼������������wTotalLength, ����"SetupStdReqGetConfig"�����ȡ��������������
 * 	f) ����Ϊ�豸����������ѡ��һ������: ��������"SetupStdReqSetConfig"��������ʽȷ��ѡ���豸���ĸ�������Ϊ��������(һ��ֻ��һ�����ñ�����). �豸��������״̬
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
 *  �ж϶˵��ڶ˵���������Ҫ���������Դ˶˵�Ĳ�ѯʱ��, �����ᱣ֤��С�����ʱ�����ķ�Χ�ڰ���һ�δ���, ��host����1ms��������豸����һ������.
 *  for FS endpoint, �жϴ����ʱ������1ms ~ 255ms֮��;
 *  for LS endpoint, ʱ����������10ms ~ 255ms֮��;
 *  for HS endpoint, ʱ����Ϊ(2 ^ (bInterval - 1)) * 125us, bInterval��ֵ��1 ~ 16֮��
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


#endif /*!< __FWK_URB_H_ */
