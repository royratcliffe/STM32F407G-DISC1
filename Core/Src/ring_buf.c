/*!
 * \file ring_buf.c
 * \brief Ring buffer functions.
 * \details Implements ring buffer functions for putting and getting data,
 * including contiguous and discontiguous access methods. Also includes
 * functions for acknowledging put and get operations, as well as resetting
 * the buffer.
 * \copyright 2024, 2025, Roy Ratcliffe, Northumberland, United Kingdom
 *
 * SPDX-License-Identifier: MIT
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

#include "ring_buf.h"

#include <memory.h>

/*!
 * \brief Clamp a value to a specified limit.
 * \details Clamps the value pointed to by \p clamp to \p limit if it exceeds
 * the limit.
 * \param clamp Pointer to the value to clamp.
 * \param limit The limit to clamp the value to.
 * \returns None.
 */
static inline void ring_buf_clamp(ring_buf_size_t *clamp,
                                  ring_buf_size_t limit) {
  if (*clamp > limit)
    *clamp = limit;
}

/*!
 * \brief Head index of a zone.
 * \details Used as the wrap size when claiming. The wrap size equals the head
 * relative to the base.
 * \param zone Ring buffer zone.
 * \return Head index.
 */
static inline ring_buf_size_t
ring_buf_zone_head(const struct ring_buf_zone *zone) {
  return zone->head - zone->base;
}

/*!
 * \brief Tail index of a zone.
 * \param zone Ring buffer zone.
 * \return Tail index.
 */
static inline ring_buf_size_t
ring_buf_zone_tail(const struct ring_buf_zone *zone) {
  return zone->tail - zone->base;
}

/*!
 * \brief Claim size of a zone.
 * \details The claim size is the difference between head and tail.
 * \param zone Ring buffer zone.
 * \return Claim size.
 */
static inline ring_buf_size_t
ring_buf_zone_claim(const struct ring_buf_zone *zone) {
  return zone->head - zone->tail;
}

static inline void ring_buf_zone_reset(struct ring_buf_zone *zone,
                                       ring_buf_ptrdiff_t base) {
  zone->base = zone->head = zone->tail = base;
}

void ring_buf_reset(struct ring_buf *buf, ring_buf_ptrdiff_t base) {
  ring_buf_zone_reset(&buf->put, base);
  ring_buf_zone_reset(&buf->get, base);
}

ring_buf_size_t ring_buf_put_claim(struct ring_buf *buf, void **space,
                                   ring_buf_size_t size) {
  ring_buf_ptrdiff_t base = buf->put.base;
  ring_buf_size_t head = ring_buf_zone_head(&buf->put);
  if (head >= buf->size) {
    base += buf->size;
    head -= buf->size;
  }
  ring_buf_clamp(&size, buf->size - head);
  ring_buf_clamp(&size, ring_buf_free_space(buf));
  if (space)
    *space = (uint8_t *)buf->space + (buf->put.head - base);
  buf->put.head += size;
  return size;
}

int ring_buf_put_ack(struct ring_buf *buf, ring_buf_size_t size) {
  ring_buf_size_t claim = ring_buf_zone_claim(&buf->put);
  if (size > claim)
    return -EINVAL;
  buf->put.head = (buf->put.tail += size);
  if (ring_buf_zone_tail(&buf->put) >= buf->size)
    buf->put.base += buf->size;
  return 0;
}

ring_buf_size_t ring_buf_get_claim(struct ring_buf *buf, void **space,
                                   ring_buf_size_t size) {
  ring_buf_ptrdiff_t base = buf->get.base;
  ring_buf_size_t head = ring_buf_zone_head(&buf->get);
  if (head >= buf->size) {
    base += buf->size;
    head -= buf->size;
  }
  ring_buf_clamp(&size, buf->size - head);
  ring_buf_clamp(&size, ring_buf_used_space(buf));
  if (space)
    *space = (uint8_t *)buf->space + (buf->get.head - base);
  buf->get.head += size;
  return size;
}

int ring_buf_get_ack(struct ring_buf *buf, ring_buf_size_t size) {
  ring_buf_size_t claim = ring_buf_zone_claim(&buf->get);
  if (size > claim)
    return -EINVAL;
  buf->get.head = (buf->get.tail += size);
  if (ring_buf_zone_tail(&buf->get) >= buf->size)
    buf->get.base += buf->size;
  return 0;
}

ring_buf_size_t ring_buf_put(struct ring_buf *buf, const void *data,
                             ring_buf_size_t size) {
  ring_buf_size_t ack = 0U, claim;
  do {
    void *space;
    claim = ring_buf_put_claim(buf, &space, size);
    (void)memcpy(space, data, claim);
    *(const uint8_t **)&data += claim;
    ack += claim;
  } while (claim && (size -= claim));
  return ack;
}

ring_buf_size_t ring_buf_get(struct ring_buf *buf, void *data,
                             ring_buf_size_t size) {
  ring_buf_size_t ack = 0U, claim;
  do {
    void *space;
    claim = ring_buf_get_claim(buf, &space, size);
    if (data) {
      (void)memcpy(data, space, claim);
      *(uint8_t **)&data += claim;
    }
    ack += claim;
  } while (claim && (size -= claim));
  return ack;
}

int ring_buf_put_all(struct ring_buf *buf, const void *data,
                     ring_buf_size_t size) {
  ring_buf_size_t ack = ring_buf_put(buf, data, size);
  int err = ack < size ? -EMSGSIZE : 0;
  if (err < 0)
    ack = 0U;
  (void)ring_buf_put_ack(buf, ack);
  return err;
}

int ring_buf_get_all(struct ring_buf *buf, void *data, ring_buf_size_t size) {
  ring_buf_size_t ack = ring_buf_get(buf, data, size);
  int err = ack < size ? -EAGAIN : 0;
  if (err < 0)
    ack = 0U;
  (void)ring_buf_get_ack(buf, ack);
  return err;
}
