/*
 * Hardware Abstraction Layer Net Interface
 *
 * File Name:   fwk_skbuff.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.07.06
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_SKBUFF_H_
#define __FWK_SKBUFF_H_

/*!< The includes */
#include <platform/net/fwk_if.h>
#include <platform/net/fwk_netdev.h>

/*!< The defines */
typedef kuint8_t*			sk_buff_data_t;

#define SKB_DATA_HEAD_LEN(mac_len)                          (mrt_align(mac_len, ARCH_PER_SIZE) - mac_len)

struct fwk_sk_buff
{
    /*!< These two members must be first. */
    struct fwk_sk_buff *sprt_next;                                      /*!< doubly list, pointing to the latter sk_buff */
    struct fwk_sk_buff *sprt_prev;                                      /*!< doubly list, pointing to the previous sk_buff */

    struct fwk_net_device *sprt_ndev;                                   /*!< net_device */

    kuint16_t queue_mapping;

    /*!< 
     * represents the sum of the length of the data area (tail - data) and the length of the data area of the shard structure. 
     * In fact, the length of the data area in this len is a valid length 
     */
    kuint32_t len;

    /*!< 
     * because the protocol headers are not deleted, only valid protocol headers and packet contents are counted. 
     * For example, when at L3, the protocol header length of L2 is not calculated 
     */
    kuint32_t data_len;                           						/*!< represents only the length of the sharded struct data area，so len = (tail - data) + data_len */

    kuint16_t protocol;                        					        /*!< the protocol type of the packet, which identifies whether it is an IP packet, an ARP packet, or some other packet */
    kuint16_t transport_header;                   				        /*!< point to the Layer 4 (Transport Layer, TCP/UDP) header struct */
    kuint16_t network_header;                     				        /*!< point to the Layer 3 (Network Layer, IP/ARP/...) IP header struct */
    kuint16_t mac_header;                         				        /*!< point to the Layer 2 ((Data Link Layer) MAC header */

    /*!< These elements must be at the end, see alloc_skb() for details. */
    sk_buff_data_t tail;                               				    /*!< point to the end of the actual data segment in the packet */
    sk_buff_data_t end;                                				    /*!< point to the end of the entire packet */
    kuint8_t *head;                              					    /*!< point to the start of the entire packet */
    kuint8_t *data;                              					    /*!< point to the start of the actual data segment in the packet */

    /*!< 
     * represents the total length, including the length of the sk_buff itself 
     * and the length of the data area of the shard structure and the shard structure
     */
    kuint32_t truesize;                           						
    srt_atomic_t users;                              					/*!< The number of times skb has been referenced by clones, which is used for memory requests and cloning */
};

struct fwk_sk_buff_head
{
    /*!< These two members must be first. */
    struct fwk_sk_buff *sprt_next;                                      /*!< doubly list, pointing to the latter sk_buff */
    struct fwk_sk_buff *sprt_prev;                                      /*!< doubly list, pointing to the previous sk_buff */

    kint32_t qlen;
};

#define mrt_skbuff_list_empty(head) \
            (((head)->sprt_next == (struct fwk_sk_buff *)(head)) && \
             ((head)->sprt_prev == (struct fwk_sk_buff *)(head)))

#define mrt_skuff_next_entry(head)                  ((!(head) || mrt_skbuff_list_empty(head)) ? mrt_nullptr : (head)->sprt_next)

/*!< The functions */
TARGET_EXT struct fwk_sk_buff *fwk_alloc_skb(kuint32_t data_size, nrt_gfp_t flags);
TARGET_EXT void kfree_skb(struct fwk_sk_buff *sprt_skb);
TARGET_EXT kint32_t fwk_skb_enqueue(struct fwk_sk_buff_head *sprt_head, struct fwk_sk_buff *sprt_skb);
TARGET_EXT struct fwk_sk_buff *fwk_skb_dequeue(struct fwk_sk_buff_head *sprt_head);

/*!< API functions */
/*!
 * @brief   reserve head (len = data - head)
 * @param   sprt_skb
 * @param   len: ip_hdr must be 4-bytes align, therefore, len can be ALIGN4(sizeof(ether_hdr)) - sizeof(ether_hdr)
 * @retval  none
 * @note    len can be 2 usually, and sizeof(ether_hdr) is 14 ===> ip_hdr = 14 + 2 = 16, it's 4-bytes align
 */
static inline void fwk_skb_reserve(struct fwk_sk_buff *sprt_skb, kuint32_t len)
{
    sprt_skb->data += len;
    sprt_skb->tail += len;
}

/*!
 * @brief   increase data length for buffer (keep data, and change tail)
 * @param   sprt_skb
 * @param   len: sizeof(eth_hdr) + sizeof(network_hdr) + sizeof(transport_hdr) + data_len
 * @retval  none
 * @note    none
 */
static inline void *fwk_skb_put(struct fwk_sk_buff *sprt_skb, kuint32_t len)
{
    kuint8_t *tail = sprt_skb->tail + len;
    if (tail > sprt_skb->end)
        return ERR_PTR(-ER_MORE);
    
    sprt_skb->tail = tail;
    sprt_skb->len += len;

    return (void *)sprt_skb->data;
}

/*!
 * @brief   increase data length for buffer (keep tail, and change data)
 * @param   sprt_skb
 * @param   len
 * @retval  none
 * @note    none
 */
static inline void *fwk_skb_push(struct fwk_sk_buff *sprt_skb, kuint32_t len)
{
    kuint8_t *data = sprt_skb->data - len;
    if (data < sprt_skb->head)
        return ERR_PTR(-ER_LACK);
    
    sprt_skb->data = data;
    sprt_skb->len += len;

    return (void *)sprt_skb->data;
}

/*!
 * @brief   decrease data length for buffer (keep tail, and change data)
 * @param   sprt_skb
 * @param   len
 * @retval  none
 * @note    none
 */
static inline void *fwk_skb_pull(struct fwk_sk_buff *sprt_skb, kuint32_t len)
{
    kuint8_t *data = sprt_skb->data + len;
    if (data > sprt_skb->tail)
        return ERR_PTR(-ER_LACK);
    
    sprt_skb->data = data;
    sprt_skb->len -= len;

    return (void *)sprt_skb->data;
}

/*!< ---------------------------------------------------------------------------- */
#define mrt_skb_reset_header(sprt_skb, _member)  \
    do { (sprt_skb)->_member = (sprt_skb)->data - (sprt_skb)->head; } while (0)

#define mrt_skb_set_header(sprt_skb, _member, offset)   \
    do {    \
        mrt_skb_reset_header(sprt_skb, _member);  \
        (sprt_skb)->_member += (offset);  \
    } while (0)

#define mrt_skb_get_header(sprt_skb, _member)                ((sprt_skb)->head + (sprt_skb)->_member)

/*!
 * @brief   reset mac_header
 * @param   sprt_skb
 * @retval  none
 * @note    none
 */
static inline void fwk_skb_reset_mac_header(struct fwk_sk_buff *sprt_skb)
{
    /*!< sprt_skb->mac_header = sprt_skb->data - sprt_skb->head */
    mrt_skb_reset_header(sprt_skb, mac_header);
}

/*!
 * @brief   set the offset between head and mac_header
 * @param   sprt_skb
 * @param   offset: it can be sizeof(eth_hdr) usually
 * @retval  none
 * @note    none
 */
static inline void fwk_skb_set_mac_header(struct fwk_sk_buff *sprt_skb, const kint32_t offset)
{
    /*!< sprt_skb->mac_header += offset */
    mrt_skb_set_header(sprt_skb, mac_header, offset);
}

/*!
 * @brief   get mac_header base address
 * @param   sprt_skb
 * @retval  mac_header base address
 * @note    none
 */
static inline kuint8_t *fwk_skb_mac_header(struct fwk_sk_buff *sprt_skb)
{
    /*!< sprt_skb->head + sprt_skb->mac_header */
    return mrt_skb_get_header(sprt_skb, mac_header);
}

/*!
 * @brief   get the offset between data and mac_header
 * @param   sprt_skb
 * @retval  offset
 * @note    none
 */
static inline kint32_t fwk_skb_mac_offset(struct fwk_sk_buff *sprt_skb)
{
    return fwk_skb_mac_header(sprt_skb) - sprt_skb->data;
}

/*!
 * @brief   reset network_header
 * @param   sprt_skb
 * @retval  none
 * @note    none
 */
static inline void fwk_skb_reset_network_header(struct fwk_sk_buff *sprt_skb)
{
    /*!< sprt_skb->network_header = sprt_skb->data - sprt_skb->head */
    mrt_skb_reset_header(sprt_skb, network_header);
}

/*!
 * @brief   set the offset between head and network_header
 * @param   sprt_skb
 * @param   offset: it can be sizeof(ip_hdr) usually
 * @retval  none
 * @note    none
 */
static inline void fwk_skb_set_network_header(struct fwk_sk_buff *sprt_skb, const kint32_t offset)
{
    mrt_skb_set_header(sprt_skb, network_header, offset);
}

/*!
 * @brief   get network_header base address
 * @param   sprt_skb
 * @retval  network_header base address
 * @note    none
 */
static inline kuint8_t *fwk_skb_network_header(struct fwk_sk_buff *sprt_skb)
{
    return mrt_skb_get_header(sprt_skb, network_header);
}

/*!
 * @brief   get the offset between data and network_header
 * @param   sprt_skb
 * @retval  offset
 * @note    none
 */
static inline kint32_t fwk_skb_network_offset(struct fwk_sk_buff *sprt_skb)
{
    return fwk_skb_network_header(sprt_skb) - sprt_skb->data;
}

/*!
 * @brief   reset transport_header
 * @param   sprt_skb
 * @retval  none
 * @note    none
 */
static inline void fwk_skb_reset_transport_header(struct fwk_sk_buff *sprt_skb)
{
    /*!< sprt_skb->transport_header = sprt_skb->data - sprt_skb->head */
    mrt_skb_reset_header(sprt_skb, transport_header);
}

/*!
 * @brief   set the offset between head and transport_header
 * @param   sprt_skb
 * @param   offset: it can be sizeof(tcp_hdr) or sizeof(udp_hdr) usually
 * @retval  none
 * @note    none
 */
static inline void fwk_skb_set_transport_header(struct fwk_sk_buff *sprt_skb, const kint32_t offset)
{
    mrt_skb_set_header(sprt_skb, transport_header, offset);
}

/*!
 * @brief   get transport_header base address
 * @param   sprt_skb
 * @retval  transport_header base address
 * @note    none
 */
static inline kuint8_t *fwk_skb_transport_header(struct fwk_sk_buff *sprt_skb)
{
    return mrt_skb_get_header(sprt_skb, transport_header);
}

/*!
 * @brief   get the offset between data and transport_header
 * @param   sprt_skb
 * @retval  offset
 * @note    none
 */
static inline kint32_t fwk_skb_transport_offset(struct fwk_sk_buff *sprt_skb)
{
    return fwk_skb_transport_header(sprt_skb) - sprt_skb->data;
}

/*!< ---------------------------------------------------------------------------- */
/*!
 * @brief   initialize skb list
 * @param   sprt_head
 * @retval  none
 * @note    none
 */
static inline void fwk_skb_list_init(struct fwk_sk_buff_head *sprt_head)
{
    sprt_head->sprt_prev = sprt_head->sprt_next = (struct fwk_sk_buff *)sprt_head;
}

/*!
 * @brief   add to tail of the global skb_list
 * @param   sprt_head, sprt_skb
 * @retval  errno
 * @note    none
 */
static inline kint32_t fwk_skb_add_tail(struct fwk_sk_buff_head *sprt_head, struct fwk_sk_buff *sprt_skb)
{
    struct fwk_sk_buff *sprt_prev = sprt_head->sprt_prev;

    sprt_head->sprt_prev = sprt_skb;
    sprt_prev->sprt_next = sprt_skb;
    sprt_skb->sprt_prev  = sprt_prev;
    sprt_skb->sprt_next  = (struct fwk_sk_buff *)sprt_head;

    sprt_head->qlen++;

    return ER_NORMAL;
}

/*!
 * @brief   del from the global skb_list
 * @param   sprt_head, sprt_skb
 * @retval  none
 * @note    none
 */
static inline void fwk_skb_unlink(struct fwk_sk_buff_head *sprt_head, struct fwk_sk_buff *sprt_skb)
{
    struct fwk_sk_buff *sprt_prev = sprt_skb->sprt_prev;
    struct fwk_sk_buff *sprt_next = sprt_skb->sprt_next;

    if (sprt_prev)
        sprt_prev->sprt_next = sprt_next;
    if (sprt_next)
        sprt_next->sprt_prev = sprt_prev;
    
    sprt_skb->sprt_prev = sprt_skb->sprt_next = mrt_nullptr;
    sprt_head->qlen--;
}

#endif /*!< __FWK_SKBUFF_H_ */
