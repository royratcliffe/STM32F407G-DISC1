/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024, Roy Ratcliffe, Northumberland, United Kingdom
 */
/*!
 * \file ring_buf_yield.h
 * \brief Header file for ring buffer yielding functions.
 * \details This file contains the declarations for functions that allow
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

#ifndef __RING_BUF_YIELD_H__
#define __RING_BUF_YIELD_H__

#include "ring_buf.h"

/*!
 * \brief Claims ring buffer space and yields it to a callback function.
 * \details This function repeatedly claims buffer spans of the specified size
 * and passes them to the provided yield function until no more spans (of the
 * given size) can be obtained. The yield function is called with the obtained
 * buffer span, a zero-based index indicating the order of the span, and an
 * extra context pointer.
 *
 * \param buf    Pointer to the ring buffer.
 * \param size   Buffer space size to claim for each yield.
 * \param yield  Function to call with the claimed buffer span.
 * \param extra  Extra context pointer to pass to the yield function.
 *
 * \return The number of yielded spans, or the yield result if the latter is not
 * \c -EAGAIN.
 * \note The yield function should return \c -EAGAIN to continue yielding, or
 * any other value to terminate the yielding process.
 * \note This function does not acknowledge the claimed space; the caller is
 * responsible for acknowledging it as needed.
 * \note This function is useful for processing data in a ring buffer in chunks.
 * Iteration stops if the ring buffer runs out of claimable space of the
 * specified size. This includes the case where the buffer is empty, or if the
 * remaining contiguous space is smaller than the requested chunk size.
 */
int ring_buf_get_claim_yield(struct ring_buf *buf, ring_buf_size_t size,
                             int yield(void *space, int index, void *extra),
                             void *extra);

/*!
 * \brief Copies buffer space and yields it to a callback function.
 * \details This function repeatedly copies discontiguous spans from the ring
 * buffer's claim space of the specified size and passes it to the provided
 * yield function until it exhausts the claim zone. The yield function is called
 * with the copied buffer space, an index indicating the order of the space,
 * and an extra context pointer.
 *
 * \param buf    Pointer to the ring buffer.
 * \param data   Pointer to the space to fill. Can be \c NULL if the data is not
 *               needed.
 * \param size   Size of the space to fill.
 * \param yield  Function to call with the filled buffer space.
 * \param extra  Extra context pointer to pass to the yield function.
 *
 * \return The number of yields, or the yield result if it is not \c -EAGAIN.
 */
int ring_buf_get_yield(struct ring_buf *buf, void *data, ring_buf_size_t size,
                       int yield(void *data, int index, void *extra),
                       void *extra);

#endif /* __RING_BUF_YIELD_H__ */
