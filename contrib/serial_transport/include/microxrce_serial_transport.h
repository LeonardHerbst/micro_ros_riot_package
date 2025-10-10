/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     pkg_micro_ros
 *
 * @{
 * @file
 * @brief       Serial transport used by the Micro XRCE DDS client to communicate with the agent.
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

#include "periph/uart.h"
#include <uxr/client/client.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief   Connection parameters
     */
    typedef struct {
        uart_t dev;
        uint32_t baud;
    } serial_transport_params_t;


    int serial_set_custom_transport(uart_t dev, uint32_t baud);

    /**
     * @brief   Opening the serial transport.
     *
     * @param[in]   transport    Pointer to the custom transport context.
     *
     * @retval      true         if successful
     * @retval      false        on errors
     */
    bool serial_transport_open(struct uxrCustomTransport *transport);

    /**
     * @brief   Closing the serial transport.
     *
     * @param[in]   transport    Pointer to the custom transport context.
     *
     * @retval      true         always
     */
    bool serial_transport_close(struct uxrCustomTransport *transport);

    /**
     * @brief   Writing to the transport.
     *
     * @param[in]   transport   Pointer to the custom transport context
     * @param[in]   buf         Pointer to the buffer containing the data to write
     * @param[in]   len         Number of bytes to write
     * @param[out]  err         Pointer to write error codes to
     *
     * @retval      Number of bytes written.
     */
    size_t serial_transport_write(struct uxrCustomTransport *transport, const uint8_t * buf,
                                  size_t len, uint8_t * err);

    /**
     * @brief   Reading from the transport.
     *
     * @param[in]   transport   Pointer to the custom transport context
     * @param[in]   buf         Pointer to the buffer the read data should be written to
     * @param[in]   len         Number of bytes to read
     * @param[out]  err         Pointer to write error codes to
     *
     * @retval      Number of bytes read.
     */
    size_t serial_transport_read(struct uxrCustomTransport *transport, uint8_t* buf,
                                  size_t len, int timeout, uint8_t* err);

#ifdef __cplusplus
}
#endif
/** @} */
