/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024, Roy Ratcliffe, Northumberland, United Kingdom
 */
/*!
 * \file ring_buf_item.c
 * \brief Implementation of ring buffer item functions.
 * \details This file contains the definitions for functions that handle
 * putting and getting items in a ring buffer, where each item is prefixed with
 * its length.
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

#include "ring_buf_item.h"

int ring_buf_item_put(struct ring_buf *buf, const void *item,
                      ring_buf_item_length_t length) {
  if (sizeof(length) + length > ring_buf_free_space(buf))
    return -EMSGSIZE;
  const ring_buf_size_t claim = ring_buf_put(buf, &length, sizeof(length));
  return claim + ring_buf_put(buf, item, length);
}

int ring_buf_item_get(struct ring_buf *buf, void *item,
                      ring_buf_item_length_t *length) {
  if (ring_buf_is_empty(buf))
    return -EAGAIN;
  const ring_buf_size_t claim = ring_buf_get(buf, length, sizeof(*length));
  return claim + ring_buf_get(buf, item, *length);
}
