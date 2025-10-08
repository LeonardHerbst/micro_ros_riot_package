/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <time.h>
#include <stdio.h>
#include <timex.h>

#include "ztimer.h"

int clock_gettime(clockid_t clock_id, struct timespec * tp)
{
    (void) clock_id;

    uint64_t now_ms = ztimer_now(ZTIMER_MSEC);

    tp->tv_sec = (time_t) now_ms / MS_PER_SEC;
    tp->tv_nsec = (uint32_t) now_ms % MS_PER_SEC;

    return 0;
}
