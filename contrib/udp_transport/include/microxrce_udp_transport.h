/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     pkg_micro_ros
 *
 * @{
 * @file
 * @brief       UDP transport used by the Micro XRCE DDS client to communicate with the agent.
 *
 * Micro ROS uses Micro XRCE DDS as it's middleware. Micro XRCE DDS uses a client and agent
 * architecture. The client communicates with the agent which publishes the client's
 * messages to the global data space.
 * The [eProsima website](https://micro-xrce-dds.docs.eprosima.com/en/latest/transport.html#micro-xrce-dds-client)
 * details how a custom client transport like this can be implemented.
 *
 * @author      Leonard Herbst <leonard.herbst@tu-dresden.de>
 *
 */
#pragma once

#include "net/sock/udp.h"
#include <uxr/client/client.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct {
        sock_udp_ep_t remote;
    } udp_transport_params_t;

    int udp_set_custom_transport(char *ip_str, uint32_t port);
    bool udp_transport_open(struct uxrCustomTransport *transport);
    bool udp_transport_close(struct uxrCustomTransport *transport);
    size_t udp_transport_write(struct uxrCustomTransport *transport, const uint8_t * buf, size_t len, uint8_t * err);
    size_t udp_transport_read(struct uxrCustomTransport *transport, uint8_t* buf, size_t len, int timeout, uint8_t* err);



#ifdef __cplusplus
}
#endif
