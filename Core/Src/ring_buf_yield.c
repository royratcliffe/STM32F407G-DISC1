/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024, Roy Ratcliffe, Northumberland, United Kingdom
 */
/*!
 * \file ring_buf_yield.c
 * \brief Implementation of ring buffer yielding functions.
 * \details This file contains the definitions for functions that allow
 * yielding of ring buffer space to a callback function.
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

#include "ring_buf_yield.h"

int ring_buf_get_claim_yield(struct ring_buf *buf, ring_buf_size_t size,
                             int yield(void *space, int index, void *extra),
                             void *extra) {
  int index = 0;
  void *space;
  while (ring_buf_get_claim(buf, &space, size) == size) {
    /*
     * Call the yield function with the claimed buffer span.
     * If the yield function returns anything other than -EAGAIN,
     * terminate the yielding process and return that value.
     */
    int yielded = yield(space, index, extra);
    if (yielded != -EAGAIN)
      return yielded;
    index++;
  }
  return index;
}

int ring_buf_get_yield(struct ring_buf *buf, void *data, ring_buf_size_t size,
                       int yield(void *data, int index, void *extra),
                       void *extra) {
  int index = 0;
  while (ring_buf_get(buf, data, size) == size) {
    int yielded = yield(data, index, extra);
    if (yielded != -EAGAIN)
      return yielded;
    index++;
  }
  return index;
}
