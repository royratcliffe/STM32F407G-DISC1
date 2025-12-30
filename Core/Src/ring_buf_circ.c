/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024, Roy Ratcliffe, Northumberland, United Kingdom
 */
/*!
 * \file ring_buf_circ.c
 * \brief Circular ring buffer function implementations.
 * \details Implements functions for putting data into a circular ring buffer.
 * \copyright 2024, 2025, Roy Ratcliffe, Northumberland, United Kingdom
 *
 * Permission is hereby granted, free of charge,  to any person obtaining a
 * copy  of  this  software  and    associated   documentation  files  (the
 * "Software"), to deal in  the   Software  without  restriction, including
 * without limitation the rights to  use,   copy,  modify,  merge, publish,
 * distribute, sublicense, and/or sell  copies  of   the  Software,  and to
 * permit persons to whom the Software is   furnished  to do so, subject to
 * the following conditions:
 *
 *     The above copyright notice and this permission notice shall be
 *     included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT  WARRANTY OF ANY KIND, EXPRESS
 * OR  IMPLIED,  INCLUDING  BUT  NOT   LIMITED    TO   THE   WARRANTIES  OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR   PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS  OR   COPYRIGHT  HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY,  WHETHER   IN  AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM,  OUT  OF   OR  IN  CONNECTION  WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "ring_buf_circ.h"
#include "ring_buf.h"

/*
 * Add a new item to the ring buffer. If the circular buffer is full, remove the
 * oldest item first.
 *
 * Fail if the new data will not fit. This should not happen if the buffer is
 * sized correctly. It will never happen if the buffer size is a multiple of the
 * data size.
 */
int ring_buf_put_circ(struct ring_buf *buf, void *data, size_t size) {
  if (ring_buf_is_full(buf))
    (void)ring_buf_get_ack(buf, ring_buf_get(buf, NULL, size));
  if (size > ring_buf_free_space(buf))
    return -EMSGSIZE;
  return ring_buf_put_ack(buf, ring_buf_put(buf, data, size));
}
