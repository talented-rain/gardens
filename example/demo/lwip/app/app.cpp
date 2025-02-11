/*
 * User Thread Instance (lwip task) Interface
 *
 * File Name:   app.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.11.21
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The globals */
#include <platform/fwk_fcntl.h>
#include <platform/net/fwk_ether.h>
#include <platform/net/fwk_if.h>
#include <kernel/mailbox.h>

#include "../../../task.h"
#include "app.h"

using namespace bsc;
using namespace tsk;

/*!< The defines */
#define DEFAULT_IP_ADDRESS          "192.168.253.206"
#define DEFAULT_IP_MASK             "255.255.255.0"
#define DEFAULT_GW_ADDRESS          "192.168.253.1"

/*!< The globals */

/*!< API functions */
/*!
 * @brief  start up
 * @param  sprt_dctrl
 * @retval none
 * @note   none
 */
void crt_lwip_data_t::startup(void)
{
    struct fwk_sockaddr_in sgrt_local;
    struct fwk_sockaddr_in sgrt_ip, sgrt_gw, sgrt_netmask;
    kint32_t sockfd;
    kint32_t retval;

    sgrt_ip.sin_addr.s_addr = fwk_inet_addr(DEFAULT_IP_ADDRESS);
    sgrt_gw.sin_addr.s_addr = fwk_inet_addr(DEFAULT_GW_ADDRESS);
    sgrt_netmask.sin_addr.s_addr = fwk_inet_addr(DEFAULT_IP_MASK);

    retval = net_link_up("lo", &sgrt_ip, &sgrt_gw, &sgrt_netmask);
    if (retval)
        return;

    sockfd = net_socket(NET_AF_INET, NR_SOCK_DGRAM, 0);
    if (sockfd < 0)
        goto fail1;

    sgrt_local.sin_port = mrt_htons(7);
    sgrt_local.sin_family = NET_AF_INET;
    sgrt_local.sin_addr.s_addr = fwk_inet_addr(DEFAULT_IP_ADDRESS);
    memset(sgrt_local.zero, 0, sizeof(sgrt_local.zero));

    retval = socket_bind(sockfd, (struct fwk_sockaddr *)&sgrt_local, sizeof(struct fwk_sockaddr));
    if (retval)
        goto fail2;

    this->fd = sockfd;
    return;

fail2:
    virt_close(sockfd);
fail1:
    net_link_down("lo");
}

/*!
 * @brief  main
 * @param  args
 * @retval none
 * @note   none
 */
void crt_lwip_data_t::excute(void)
{
    struct fwk_sockaddr_in sgrt_remote;
    const kchar_t *msg = "HeavenFox OS will be all the best!";
    fwk_socklen_t addrlen;
    kssize_t len;
    crt_task_t *cprt_this = (crt_task_t *)this->args;
    struct mailbox &sgrt_mb = cprt_this->get_mailbox();
    struct mail *sprt_mail;

    if (this->fd < 0)
        return;

    sgrt_remote.sin_port = mrt_htons(7);
    sgrt_remote.sin_family = NET_AF_INET;
    sgrt_remote.sin_addr.s_addr = fwk_inet_addr(DEFAULT_IP_ADDRESS);
    memset(sgrt_remote.zero, 0, sizeof(sgrt_remote.zero));

    len = socket_sendto(this->fd, msg, strlen(msg) + 1, 0, 
                    (struct fwk_sockaddr *)&sgrt_remote, sizeof(struct fwk_sockaddr));
    if (len <= 0)
    {
        cout << "send msg failed!" << endl;
        return;
    }

    /*!< blocking */
    len = socket_recvfrom(this->fd, this->rx_buffer, 128, 0, 
                    (struct fwk_sockaddr *)&sgrt_remote, &addrlen);
    if (len <= 0)
    {
        cout << "recv msg failed!" << endl;
        return;
    }

    this->rx_buffer[len] = '\0';

    sprt_mail = mail_recv(&sgrt_mb, 0);
    if (!isValid(sprt_mail))
        goto END;

    if (sprt_mail->sprt_msg->type == NR_MAIL_TYPE_SERIAL)
    {
        kchar_t *buffer = (kchar_t *)sprt_mail->sprt_msg[0].buffer;

        if (!kstrncmp(buffer, "echo", 4))
            this->echo_cnt++;
    }

    mail_recv_finish(sprt_mail);

END:
    if (this->echo_cnt)
    {
        this->echo_cnt--;
        cout << "recv data is: " << this->rx_buffer << endl;
    }
}

/*!< end of file */
