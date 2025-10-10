/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <log.h>

#include <rmw_microros/rmw_microros.h>

#include "tsrb.h"
#include "ztimer.h"

#include "microxrce_serial_transport.h"

#define RINGBUFFER_SIZE 2048

static uint8_t _buffer_in[RINGBUFFER_SIZE];

static tsrb_t ringbuffer_incoming = TSRB_INIT(_buffer_in);

static serial_transport_params_t serial_params;

int serial_set_custom_transport(uart_t dev, uint32_t baud)
{
    printf("Initializing the uart transport: with dev %u and a baud rate of %lu!\n", dev, baud);
    serial_params.dev	= UART_DEV(dev);
    serial_params.baud 	= baud;
    rmw_ret_t ret = rmw_uros_set_custom_transport(
        true,
        (void *) &serial_params,
        serial_transport_open,
        serial_transport_close,
        serial_transport_write,
        serial_transport_read
    );
    if (ret != RMW_RET_OK) {
        LOG_ERROR("Failed setting the custom udp transport!\n");
        return -1;
    }
    return 0;
}

static void uart_receive_cb(void *arg, uint8_t data)
{
    (void) arg;

    int re = tsrb_add_one(&ringbuffer_incoming, data);
    if (re == -1) {
        LOG(LOG_WARNING, "micro_ros: Serial transport ring buffer full!");
    }
}

bool serial_transport_open(struct uxrCustomTransport *transport)
{
    serial_transport_params_t *params = (serial_transport_params_t*) transport->args;
    int res = 0;

    res = uart_init(UART_DEV(params->dev), params->baud, uart_receive_cb, NULL);

    if (res == -ENOTSUP) {
        printf("micro_ros: Unsupported baudrate %lu!\n", params->baud);
        return false;
    }
    else if (res == -ENODEV) {
        puts("micro_ros: No surch device!");
        return false;
    }
    else if (res != 0) {
        printf("micro_ros: Error during uart_init: %d\n", res);
        return false;
    }

    return true;
}

bool serial_transport_close(struct uxrCustomTransport *transport)
{
    (void) transport;

    return true;
}

size_t serial_transport_write(struct uxrCustomTransport *transport,
                              const uint8_t * buf, size_t len, uint8_t * err)
{
    (void) err;

    serial_transport_params_t *params = (serial_transport_params_t*) transport->args;
    /* uart_writes blocks until everything has been written */
    uart_write(params->dev, buf, len);

    return len;
}

size_t serial_transport_read(struct uxrCustomTransport *transport, uint8_t* buf,
                             size_t len, int timeout, uint8_t* err)
{
    (void) transport;
    (void) err;

    int32_t time_spent = 0;
    size_t read = 0;
    size_t buf_size = tsrb_avail(&ringbuffer_incoming);

    while (buf_size < len && time_spent < timeout) {
        time_spent++;
        ztimer_sleep(ZTIMER_MSEC, 1);
        buf_size = tsrb_avail(&ringbuffer_incoming);
    }

    read = tsrb_get(&ringbuffer_incoming, buf, len);
    return read;
}
