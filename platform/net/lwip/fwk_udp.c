/*
 * NetWork UDP Interface
 *
 * File Name:   fwk_udp.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.12.13
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <common/queue.h>
#include <platform/fwk_mempool.h>
#include <platform/fwk_uaccess.h>
#include <platform/net/fwk_lwip.h>
#include <platform/net/fwk_netif.h>

/*!< The globals */
struct lwip_udp_data
{
    struct udp_pcb *sprt_upcb;
    struct pbuf *sprt_buf;

    ip_addr_t *sprt_ipaddr;
    kuint16_t port;

    struct pq_data sgrt_pqd;
};

/*!< API functions */
/*!
 * @brief   release lwip_udp_data
 * @param   sprt_pqd (member of ring queue)
 * @retval  none
 * @note    none
 */
static void lwip_udp_raw_free(struct pq_data *sprt_pqd)
{
    struct lwip_udp_data *sprt_data;

    sprt_data = mrt_container_of(sprt_pqd, struct lwip_udp_data, sgrt_pqd);
    pbuf_free(sprt_data->sprt_buf);
    kfree(sprt_data);
}

/*!
 * @brief   check the size of recv buffer is enough
 * @param   sprt_pqd (member of ring queue)
 * @param   limit (size of recv buffer)
 * @retval  1: enough; 0: no
 * @note    none
 */
static kbool_t lwip_udp_raw_check(struct pq_data *sprt_pqd, kusize_t limit)
{
    struct lwip_udp_data *sprt_data;

    sprt_data = mrt_container_of(sprt_pqd, struct lwip_udp_data, sgrt_pqd);
    return !!(sprt_data->sprt_buf->len <= limit);
}

/*!
 * @brief   get every rx data from queue with poll ways
 * @param   sprt_upcb, len (buffer's length)
 * @retval  udp data
 * @note    none
 */
static struct lwip_udp_data *lwip_udp_raw_poll(struct udp_pcb *sprt_upcb, kusize_t len)
{
    struct pq_queue *sprt_pq = (struct pq_queue *)sprt_upcb->recv_arg;
    struct pq_data *sprt_pqd;

    sprt_pqd = pq_dequeue_with_chk(sprt_pq, len);
    if (isValid(sprt_pqd))
        return mrt_container_of(sprt_pqd, struct lwip_udp_data, sgrt_pqd);

    return sprt_pqd ? ERR_PTR(-ER_LACK) : mrt_nullptr;
}

/*!
 * @brief   recv callback
 * @param   sprt_upcb, arg, ...
 * @retval  none
 * @note    none
 */
static void __lwip_udp_raw_recv(void *arg, struct udp_pcb *sprt_upcb, struct pbuf *sprt_buf,
                            const ip_addr_t *sprt_ipaddr, u16_t port)
{
    struct pq_queue *sprt_pq = (struct pq_queue *)arg;
    struct lwip_udp_data *sprt_data;

    if (!sprt_buf || !sprt_pq)
        return;
    
    sprt_data = kmalloc(sizeof(*sprt_data), GFP_KERNEL);
    if (!isValid(sprt_data))
        return;

    sprt_data->sprt_upcb = sprt_upcb;
    sprt_data->sprt_ipaddr = (ip_addr_t *)sprt_ipaddr;
    sprt_data->port = mrt_htons(port);
    sprt_data->sprt_buf = sprt_buf;

    sprt_data->sgrt_pqd.release = lwip_udp_raw_free;
    sprt_data->sgrt_pqd.dequeue_chk = lwip_udp_raw_check;

    pq_enqueue(sprt_pq, &sprt_data->sgrt_pqd);
}

/*!
 * @brief   called by socket_recvfrom
 * @param   sprt_upcb, buf, ...
 * @retval  size
 * @note    read with blocking
 */
kssize_t lwip_udp_raw_recvfrom(struct udp_pcb *sprt_upcb, void *buf, 
                            kusize_t size, ip_addr_t *sprt_src, u16_t *port)
{
    struct lwip_udp_data *sprt_data;
    void *payload;
    kssize_t len;

    if (!size)
        return -ER_LACK;

    /*!< read one frame */
    do {
        sprt_data = lwip_udp_raw_poll(sprt_upcb, size);
        if (PTR_ERR(sprt_data) == (-ER_LACK))
        {
            print_err("%s: recv buffer is too small\n", __FUNCTION__);
            return -ER_LACK;
        }
        if (!sprt_data)
            continue;

        payload = sprt_data->sprt_buf->payload;
        len = sprt_data->sprt_buf->len;
        
        memcpy(sprt_src, sprt_data->sprt_ipaddr, sizeof(*sprt_src));
        *port = sprt_data->port;
        if (len)
            fwk_copy_to_user(buf, payload, len);

        lwip_udp_raw_free(&sprt_data->sgrt_pqd);
        break;

    } while (1);

    return len;
}

/*!
 * @brief   called by socket_sendto
 * @param   sprt_upcb, buf, ...
 * @retval  size
 * @note    send (application layer ---> lwip ---> drivers)
 */
kssize_t lwip_udp_raw_sendto(struct udp_pcb *sprt_upcb, const ip_addr_t *sprt_dest, 
                            u16_t dest_port, const void *buf, kusize_t size)
{
    struct pbuf *sprt_buf;
    err_t err;

    sprt_buf = pbuf_alloc(PBUF_TRANSPORT, size, PBUF_POOL);
    if (!sprt_buf)
    {
        print_err("allocate lwip pbuf failed!\n");
        return -ER_NOMEM;
    }

    memcpy(sprt_buf->payload, buf, size);
    err = udp_sendto(sprt_upcb, sprt_buf, sprt_dest, dest_port);
    if (err != ERR_OK)
    {
        pbuf_free(sprt_buf);
        return -ER_SDATA_FAILD;
    }

    return size;
}

/*!
 * @brief   udp pcb init
 * @param   sprt_ip, port
 * @retval  udp_pcb
 * @note    create rx ring queue for application layer
 */
struct udp_pcb *lwip_udp_raw_bind(const ip_addr_t *sprt_ip, u16_t port)
{
    struct udp_pcb *sprt_upcb;
    struct pq_queue *sprt_pq;
    err_t err;
    
    sprt_pq = pq_queue_create(NR_PQ_RING, 1024);
    if (!isValid(sprt_pq))
        return ERR_PTR(-ER_NOMEM);

    sprt_upcb = udp_new_ip_type(IPADDR_TYPE_ANY);
    if (!sprt_upcb)
        goto fail;

    err = udp_bind(sprt_upcb, sprt_ip, port);
    if (err == ERR_OK) 
    {
        udp_recv(sprt_upcb, __lwip_udp_raw_recv, sprt_pq);
        return sprt_upcb;
    }

    udp_remove(sprt_upcb);

fail:
    pq_queue_destroy(sprt_pq);
    return ERR_PTR(-ER_FAILD);
}

/* end of file */
