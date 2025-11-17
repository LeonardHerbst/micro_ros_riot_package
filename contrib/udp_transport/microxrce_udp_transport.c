/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <stdio.h>

#include <rmw_microros/rmw_microros.h>
#include "microxrce_udp_transport.h"

#include "net/af.h"
#include "net/protnum.h"
#include "net/ipv6/addr.h"
#include "log.h"

static sock_udp_t sock;
static sock_udp_ep_t local = SOCK_IPV6_EP_ANY;

static udp_transport_params_t udp_params;

#define MAIN_QUEUE_SIZE     (16)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

int udp_set_custom_transport(char *ip_str, uint32_t port)
{
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    udp_params.remote.family = AF_INET6;
    udp_params.remote.netif = SOCK_ADDR_ANY_NETIF;
    udp_params.remote.port = port;
    if (ipv6_addr_from_str((ipv6_addr_t *)&udp_params.remote.addr.ipv6, ip_str) == NULL) {
        LOG_ERROR("Invalid IPv6 address: %s\n", ip_str);
        return -1;
    }
    rmw_ret_t ret = rmw_uros_set_custom_transport(
        false,
        (void *) &udp_params,
        udp_transport_open,
        udp_transport_close,
        udp_transport_write,
        udp_transport_read
    );
    if (ret != RMW_RET_OK) {
        LOG_ERROR("Failed setting the custom udp transport!\n");
        return -1;
    }
    return 0;
}

bool udp_transport_open(struct uxrCustomTransport *transport)
{
    udp_transport_params_t *params = (udp_transport_params_t*) transport->args;

    local.port = 0;
    if (sock_udp_create(&sock, &local, &params->remote, 0) < 0) {
        LOG_ERROR("Error creating UDP sock\n");
        return false;
    }

    return true;
}

bool udp_transport_close(struct uxrCustomTransport *transport)
{
    (void) transport;

    sock_udp_close(&sock);

    return true;
}

size_t udp_transport_write(struct uxrCustomTransport *transport, const uint8_t * buf, size_t len, uint8_t * err)
{
    (void) err;
    (void) transport;

    ssize_t bytes_send = sock_udp_send(&sock, (void *) buf, len, NULL);

    if (bytes_send >= 0) {
        *err = 0;
        return bytes_send;
    }

    switch (bytes_send)
    {
    case -EADDRINUSE:
        LOG_ERROR("Error: No available ephemeral ports or socket has no local endpoint.\n");
        break;
    case -EAFNOSUPPORT:
        LOG_ERROR("Error: Address family of remote endpoint is not supported.\n");
        break;
    case -EHOSTUNREACH:
        LOG_ERROR("Error: Remote endpoint is not reachable.\n");
        break;
    case -EINVAL:
        LOG_ERROR("Error: Invalid address, port is 0, or network interface mismatch.\n");
        break;
    case -ENOMEM:
        LOG_ERROR("Error: Not enough memory to send data.\n");
        break;
    case -ENOTCONN:
        LOG_ERROR("Error: Socket is not connected and no remote endpoint was specified.\n");
        break;
    default:
        LOG_ERROR("Error: Unknown error occurred during UDP send.\n");
        break;
    }
    *err = 1;
    return 0;
}

size_t udp_transport_read(struct uxrCustomTransport *transport, uint8_t* buf, size_t len, int timeout, uint8_t* err)
{
    (void) transport;
    (void) err;

    ssize_t bytes_received = sock_udp_recv(&sock, (void*) buf, len, timeout * US_PER_MS, NULL);

    if (bytes_received >= 0) {
        *err = 0;
        return bytes_received;
    }

    switch (bytes_received)
    {
    case -EADDRNOTAVAIL:
        LOG_ERROR("Error: Socket has no local address configured.\n");
        break;
    case -EAGAIN:
        break;
    case -EINVAL:
        LOG_ERROR("Error: Invalid remote address or socket improperly initialized or closed.\n");
        break;
    case -ENOBUFS:
        LOG_ERROR("Error: Receive buffer too small for incoming data.\n");
        break;
    case -ENOMEM:
        LOG_ERROR("Error: Not enough memory to receive data.\n");
        break;
    case -EPROTO:
        LOG_ERROR("Error: Packet source does not match expected remote address.\n");
        break;
    case -ETIMEDOUT:
        LOG_DEBUG("Error: Receive operation timed out.\n");
        break;
    default:
        LOG_ERROR("Error: Unknown error occurred during UDP receive.\n");
        break;
    }
    *err = 1;
    return 0;
}