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

#include "app.h"

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
void lwip_task_startup(void *args)
{
    struct lwip_task_data *sprt_data;
    struct fwk_sockaddr_in sgrt_local;
    struct fwk_sockaddr_in sgrt_ip, sgrt_gw, sgrt_netmask;
    kint32_t sockfd;
    kint32_t retval;

    sprt_data = (struct lwip_task_data *)args;

    sgrt_ip.sin_addr.s_addr = fwk_inet_addr(DEFAULT_IP_ADDRESS);
    sgrt_gw.sin_addr.s_addr = fwk_inet_addr(DEFAULT_GW_ADDRESS);
    sgrt_netmask.sin_addr.s_addr = fwk_inet_addr(DEFAULT_IP_MASK);

    retval = network_link_up("lo", &sgrt_ip, &sgrt_gw, &sgrt_netmask);
    if (retval)
        return;

    sockfd = virt_socket(NET_AF_INET, NR_SOCK_DGRAM, 0);
    if (sockfd < 0)
        goto fail1;

    sgrt_local.sin_port = mrt_htons(7);
    sgrt_local.sin_family = NET_AF_INET;
    sgrt_local.sin_addr.s_addr = fwk_inet_addr(DEFAULT_IP_ADDRESS);
    memset(sgrt_local.zero, 0, sizeof(sgrt_local.zero));

    retval = virt_bind(sockfd, (struct fwk_sockaddr *)&sgrt_local, sizeof(struct fwk_sockaddr));
    if (retval)
        goto fail2;

    sprt_data->fd = sockfd;
    return;

fail2:
    virt_close(sockfd);
fail1:
    network_link_down("lo");
}

/*!
 * @brief  main
 * @param  args
 * @retval none
 * @note   none
 */
void lwip_task(void *args)
{
    struct lwip_task_data *sprt_data;
    struct fwk_sockaddr_in sgrt_remote;
    const kchar_t *msg = "HeavenFox OS will be all the best!";
    fwk_socklen_t addrlen;
    kssize_t len;

    sprt_data = (struct lwip_task_data *)args;
    if (sprt_data->fd < 0)
        return;

    sgrt_remote.sin_port = mrt_htons(7);
    sgrt_remote.sin_family = NET_AF_INET;
    sgrt_remote.sin_addr.s_addr = fwk_inet_addr(DEFAULT_IP_ADDRESS);
    memset(sgrt_remote.zero, 0, sizeof(sgrt_remote.zero));

    len = virt_sendto(sprt_data->fd, msg, strlen(msg) + 1, 0, 
                    (struct fwk_sockaddr *)&sgrt_remote, sizeof(struct fwk_sockaddr));
    if (len <= 0)
    {
        print_warn("send msg failed!\n");
        return;
    }

    /*!< blocking */
    len = virt_recvfrom(sprt_data->fd, sprt_data->rx_buffer, 128, 0, 
                    (struct fwk_sockaddr *)&sgrt_remote, &addrlen);
    if (len <= 0)
    {
        print_warn("recv msg failed!\n");
        return;
    }

    sprt_data->rx_buffer[len] = '\0';
    print_info("recv data is: %s\n", sprt_data->rx_buffer);
}

/*!< end of file */
