/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024, Roy Ratcliffe, Northumberland, United Kingdom
 */
/*!
 * \file ring_buf_circ.h
 * \brief Circular ring buffer function prototypes.
 * \details Declares functions for putting data into a circular ring buffer.
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

#ifndef __RING_BUF_CIRC_H__
#define __RING_BUF_CIRC_H__

#include <stddef.h>

struct ring_buf;

/*!
 * \brief Put data into a circular buffer.
 * \details If the buffer is full, removes the oldest data to make space.
 * \param buf Ring buffer.
 * \param data Address of bytes to put.
 * \param size Number of bytes to put.
 * \returns 0 on success, \c -EMSGSIZE if the data will not fit.
 * \note Uses \c ring_buf_get rather than \c ring_buf_get_claim, as the former
 * handles discontiguous items. They amount to the same thing when the buffer
 * size is a multiple of the item size.
 */
int ring_buf_put_circ(struct ring_buf *buf, void *data, size_t size);

#endif /* __RING_BUF_CIRC_H__ */
