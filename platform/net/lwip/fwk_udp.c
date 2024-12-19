/*
 * Copyright (c) 2016 Stephan Linz <linz@li-pro.net>, Li-Pro.Net
 * All rights reserved.
 *
 * Based on examples provided by
 * Iwan Budi Kusnanto <ibk@labhijau.net> (https://gist.github.com/iwanbk/1399729)
 * Juri Haberland <juri@sapienti-sat.org> (https://lists.gnu.org/archive/html/lwip-users/2007-06/msg00078.html)
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of and a contribution to the lwIP TCP/IP stack.
 *
 * Credits go to Adam Dunkels (and the current maintainers) of this software.
 *
 * Stephan Linz rewrote this file to get a basic echo example.
 */

/**
 * @file
 * UDP echo server example using raw API.
 *
 * Echos all bytes sent by connecting client,
 * and passively closes when client is done.
 *
 */

#include <common/queue.h>
#include <platform/fwk_mempool.h>
#include <platform/fwk_uaccess.h>
#include <platform/net/fwk_lwip.h>
#include <platform/net/fwk_netif.h>

#if LWIP_UDP

struct lwip_udp_data
{
    struct udp_pcb *sprt_upcb;
    struct pbuf *sprt_buf;

    ip_addr_t *sprt_ipaddr;
    kuint16_t port;

    struct pq_data sgrt_pqd;
};

static void lwip_udp_raw_free(struct pq_data *sprt_pqd)
{
    struct lwip_udp_data *sprt_data;

    sprt_data = mrt_container_of(sprt_pqd, struct lwip_udp_data, sgrt_pqd);
    pbuf_free(sprt_data->sprt_buf);
    kfree(sprt_data);
}

static kbool_t lwip_udp_raw_check(struct pq_data *sprt_pqd, kusize_t limit)
{
    struct lwip_udp_data *sprt_data;

    sprt_data = mrt_container_of(sprt_pqd, struct lwip_udp_data, sgrt_pqd);
    return !!(sprt_data->sprt_buf->len <= limit);
}

static struct lwip_udp_data *lwip_udp_raw_poll(struct udp_pcb *sprt_upcb, kusize_t len)
{
    struct pq_queue *sprt_pq = (struct pq_queue *)sprt_upcb->recv_arg;
    struct pq_data *sprt_pqd;

    sprt_pqd = pq_dequeue_with_chk(sprt_pq, len);
    if (isValid(sprt_pqd))
        return mrt_container_of(sprt_pqd, struct lwip_udp_data, sgrt_pqd);

    return sprt_pqd ? ERR_PTR(-ER_LACK) : mrt_nullptr;
}

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

#endif /* LWIP_UDP */
