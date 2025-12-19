/*!
 * \file ring_buf.h
 * \copyright Roy Ratcliffe, Northumberland, United Kingdom
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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>

typedef ptrdiff_t ring_buf_ptrdiff_t;

typedef size_t ring_buf_size_t;

#define RING_BUF_SIZE_MAX ((ring_buf_size_t)PTRDIFF_MIN)

/*!
 * \defgroup ring_buf_zone Ring Buffer Zone
 * \ingroup ring_buf_zone
 * \{
 */

/*!
 * \brief Ring buffer zone.
 * \details A ring buffer contains two zones: one for putting data and one for
 * getting data. Each zone has a base, head, and tail pointer. Pointer here
 * refers to a signed zero-based index (ring_buf_ptrdiff_t) relative to the
 * buffer's start. The base pointer marks the beginning of the zone, while the
 * head pointer marks the end of the zone. The tail pointer is used to track the
 * position of the oldest data in the zone.
 * \note This structure needs to exist within the header.
 */
struct ring_buf_zone {
  ring_buf_ptrdiff_t base, head, tail;
};

/*!
 * \}
 */

/*!
 * \defgroup ring_buf Ring Buffer
 * \ingroup ring_buf
 * \{
 */

/*!
 * \brief Ring buffer instance.
 */
struct ring_buf {
  void *space;
  ring_buf_size_t size;
  struct ring_buf_zone put, get;
};

static inline ring_buf_size_t ring_buf_used_space(const struct ring_buf *buf) {
  return buf->put.tail - buf->get.head;
}

static inline bool ring_buf_is_empty(const struct ring_buf *buf) {
  return ring_buf_used_space(buf) == 0U;
}

static inline ring_buf_size_t ring_buf_free_space(const struct ring_buf *buf) {
  return buf->size - (buf->put.head - buf->get.tail);
}

static inline bool ring_buf_is_full(const struct ring_buf *buf) {
  return ring_buf_free_space(buf) == 0U;
}

void ring_buf_reset(struct ring_buf *buf, ring_buf_ptrdiff_t base);

/*!
 * \defgroup ring_buf_contiguous Contiguous Ring Buffer Access
 * \ingroup ring_buf_contiguous
 * \{
 */

/*!
 * \brief Claims space for putting data into a ring buffer.
 * \details Claims contiguous space. Advances the "put" head.
 * One put operation starts with a claim. A successful claim expands the "put
 * zone" by the requested number of bytes.
 * \note The claim cannot span across the end of the buffer space. Buffer size
 * less the put zone's head \e clamps the claim size. It \e cannot exceed the
 * remaining contiguous space.
 * \param buf Ring buffer address.
 * \param space Address of pointer to claimed space, or \c NULL to ignore.
 * \param size Number of bytes to claim.
 * \returns Number of bytes claimed.
 */
ring_buf_size_t ring_buf_put_claim(struct ring_buf *buf, void **space,
                                   ring_buf_size_t size);

/*!
 * \brief Acknowledges space claimed for putting data into a ring buffer.
 * \details Acknowledging the same number of bytes advances the put zone. Notice
 * that the claim cannot span across the end of the buffer space. Buffer size
 * less the put zone's head \e clamps the claim size. It \e cannot exceed the
 * remaining contiguous space.
 * \param buf Ring buffer address.
 * \param size Number of bytes to acknowledge.
 * \retval 0 on successful put.
 * \retval -EINVAL if \c size exceeds previously claimed aggregate space.
 */
int ring_buf_put_ack(struct ring_buf *buf, ring_buf_size_t size);

/*!
 * \brief Claims contiguous space for getting.
 * \details Advances the "get" head.
 */
ring_buf_size_t ring_buf_get_claim(struct ring_buf *buf, void **space,
                                   ring_buf_size_t size);

int ring_buf_get_ack(struct ring_buf *buf, ring_buf_size_t size);

/*!
 * \}
 */

/*!
 * \defgroup ring_buf_discontiguous Discontiguous Ring Buffer Access
 * \ingroup ring_buf_discontiguous
 * \{
 */

/*!
 * \brief Puts non-contiguous bytes into the ring buffer.
 * \details The return value may be less than the given size if the buffer runs
 * out of free space.
 * \note Does \e not automatically acknowledge the space.
 * \param buf Ring buffer.
 * \param data Address of bytes to put.
 * \param size Number of bytes to put.
 * \returns Buffer space to acknowledge in bytes.
 */
ring_buf_size_t ring_buf_put(struct ring_buf *buf, const void *data,
                             ring_buf_size_t size);

/*!
 * \brief Gets data from a ring buffer.
 * \details Copies discontinuous data.
 * \param buf Ring buffer.
 * \param data Address of copied data, or \c NULL to ignore.
 * \param size Number of bytes to get.
 * \returns Number of bytes to acknowledge.
 */
ring_buf_size_t ring_buf_get(struct ring_buf *buf, void *data,
                             ring_buf_size_t size);

/*!
 * \brief Puts all or none.
 * \details Puts all the given data into the ring buffer or puts nothing.
 * Returns an error if there is insufficient space.
 * \param buf Ring buffer.
 * \param data Address of bytes to put.
 * \param size Number of bytes to put.
 * \returns 0 on success, \c -EMSGSIZE if the data will not fit.
 */
int ring_buf_put_all(struct ring_buf *buf, const void *data,
                     ring_buf_size_t size);

/*!
 * \brief Gets all or none.
 */
int ring_buf_get_all(struct ring_buf *buf, void *data, ring_buf_size_t size);

/*!
 * \}
 */

#include <stdint.h>

/*!
 * \brief Defines a static ring buffer.
 * \details This macro creates a ring buffer with the specified name and size.
 * It statically allocates the ring buffer's storage space as an array of
 * bytes.
 * \param _name_ Name of the ring buffer.
 * \param _size_ Size of the ring buffer.
 */
#define RING_BUF_DEFINE(_name_, _size_)                                        \
  static uint8_t _ring_buf_space_##_name_[_size_];                             \
  static struct ring_buf _name_ = {.space = _ring_buf_space_##_name_,          \
                                   .size = _size_}

/*!
 * \}
 */

#ifdef __cplusplus
}
#endif
