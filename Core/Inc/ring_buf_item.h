/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024, Roy Ratcliffe, Northumberland, United Kingdom
 */
/*!
 * \file ring_buf_item.h
 * \brief Header file for ring buffer item functions.
 * \details This file contains the declarations for functions that handle
 * putting and getting items in a ring buffer, where each item is prefixed with
 * its length as a \c ring_buf_item_length_t 16-bit unsigned integer.
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

#ifndef __RING_BUF_ITEM_H__
#define __RING_BUF_ITEM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ring_buf.h"

/*!
 * \brief Size of the ring buffer's item length.
 * \details The length is the size of the item in bytes.
 */
typedef uint16_t ring_buf_item_length_t;

/*!
 * \brief Puts an item's length and content.
 * \param buf Address of the ring buffer.
 * \param item Address of item to put.
 * \param length Number of bytes to put.
 * \retval Zero or greater on success, negative on error.
 * \retval -EMSGSIZE if the buffer has insufficient space to put the item's
 * length and data.
 * \note Does \e not auto-acknowledge the put claim.
 */
int ring_buf_item_put(struct ring_buf *buf, const void *item,
                      ring_buf_item_length_t length);

/*!
 * \brief Gets an item from a ring buffer.
 * \param buf Address of the ring buffer.
 * \param item Address of the item. Reserve sufficient space for the incoming
 * bytes. There must be space for the largest possible item, since the largest
 * one possible could be next.
 * \param length Address of the length of the item on success.
 * \retval 0 on success.
 * \retval -EAGAIN if the buffer is empty.
 * \note The two-phase get claim cannot fail since a previous put succeeded. Do
 * \e not mix item-based puts with plain puts.
 */
int ring_buf_item_get(struct ring_buf *buf, void *item,
                      ring_buf_item_length_t *length);

#ifdef __cplusplus
}
#endif

#endif /* __RING_BUF_ITEM_H__ */
