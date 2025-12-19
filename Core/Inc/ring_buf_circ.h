/*!
 * \file ring_buf_circ.h
 * \brief Circular ring buffer function prototypes.
 */

#pragma once

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
