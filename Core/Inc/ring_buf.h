/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024, Roy Ratcliffe, Northumberland, United Kingdom
 */
/*!
 * \file ring_buf.h
 * \brief Ring buffer function prototypes.
 * \details Declares ring buffer functions for putting and getting data,
 * including contiguous and discontiguous access methods. Also includes
 * functions for acknowledging put and get operations, as well as resetting the
 * buffer.
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

#ifndef __RING_BUF_H__
#define __RING_BUF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>

#ifndef EMSGSIZE
/*!
 * \brief Message size error code.
 * \details Error code indicating that a message is too large to fit in
 * a ring buffer.
 */
#define EMSGSIZE 115
#endif

/*!
 * \brief Ring buffer pointer difference type.
 * \details Signed type used for pointer arithmetic within the ring buffer.
 */
typedef ptrdiff_t ring_buf_ptrdiff_t;

/*!
 * \brief Ring buffer size type.
 * \details Unsigned type used for sizes within the ring buffer.
 */
typedef size_t ring_buf_size_t;

/*!
 * \brief Maximum ring buffer size.
 * \details Defines the maximum size of the ring buffer based on the minimum
 * value of the pointer difference type.
 */
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
 * \details Represents a ring buffer with its associated data and zones for
 * putting and getting data.
 * \note This structure needs to exist within the header.
 */
struct ring_buf {
  /*!
   * \brief Pointer to the buffer's data space.
   * \details Points to the memory area where the ring buffer's data is stored.
   */
  void *space;
  /*!
   * \brief Size of the ring buffer.
   * \details Indicates the total size of the buffer in bytes.
   */
  ring_buf_size_t size;
  /*!
   * \brief Put and get zones.
   * \details Contains the zones for putting and getting data in the ring
   * buffer.
   */
  struct ring_buf_zone put, get;
};

/*!
 * \brief Calculates used space in the ring buffer.
 * \details Computes the number of bytes currently used in the ring buffer.
 * \param buf Ring buffer.
 * \returns Number of used bytes.
 */
static inline ring_buf_size_t ring_buf_used_space(const struct ring_buf *buf) {
  return buf->put.tail - buf->get.head;
}

/*!
 * \brief Checks if the ring buffer is empty.
 * \details Determines whether the ring buffer has no used space. A ring buffer
 * is considered empty when the used space is zero.
 * \param buf Ring buffer.
 * \retval true if the buffer is empty.
 * \retval false otherwise.
 */
static inline bool ring_buf_is_empty(const struct ring_buf *buf) {
  return ring_buf_used_space(buf) == 0U;
}

/*!
 * \brief Calculates free space in the ring buffer.
 * \details Computes the number of bytes currently available for use in the
 * ring buffer.
 * \param buf Ring buffer.
 * \returns Number of free bytes.
 */
static inline ring_buf_size_t ring_buf_free_space(const struct ring_buf *buf) {
  return buf->size - (buf->put.head - buf->get.tail);
}

/*!
 * \brief Checks if the ring buffer is full.
 * \details Determines whether the ring buffer has no free space. A ring buffer
 * is considered full when the free space is zero.
 * \param buf Ring buffer.
 * \retval true if the buffer is full.
 * \retval false if not full.
 */
static inline bool ring_buf_is_full(const struct ring_buf *buf) {
  return ring_buf_free_space(buf) == 0U;
}

/*!
 * \brief Resets a ring buffer.
 * \details Resets both put and get zones to the specified base index.
 * The base index typically starts at zero but can be set to any value.
 * \param buf Ring buffer.
 * \param base Base index for both put and get zones.
 */
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
 * \details Acknowledging a number of bytes advances the put zone. Notice
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
 * One get operation starts with a claim. A successful claim expands the "get
 * zone" by the requested number of bytes.
 * \note The claim cannot span across the end of the buffer space. Buffer size
 * less the get zone's head \e clamps the claim size. It \e cannot exceed the
 * remaining contiguous space.
 * \param buf Ring buffer address.
 * \param space Address of pointer to claimed space, or \c NULL to ignore.
 * \param size Number of bytes to claim.
 * \returns Number of bytes claimed.
 */
ring_buf_size_t ring_buf_get_claim(struct ring_buf *buf, void **space,
                                   ring_buf_size_t size);

/*!
 * \brief Acknowledges space claimed for getting data from a ring buffer.
 * \details Acknowledging a number of bytes advances the get zone.
 * \param buf Ring buffer address.
 * \param size Number of bytes to acknowledge.
 * \retval 0 on successful get.
 * \retval -EINVAL if \c size exceeds previously claimed aggregate space.
 */
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
 * \details Gets all the requested data from the ring buffer or gets nothing.
 * Returns an error if there is insufficient data.
 * \param buf Ring buffer.
 * \param data Address of copied data.
 * \param size Number of bytes to get.
 * \retval 0 on success
 * \retval -EMSGSIZE if insufficient data is available.
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
 *
 * It correctly initialises the ring buffer structure with a pointer to the
 * allocated space and sets the size. The put and get zones are initialised to
 * zero by default. This assumes that the compiler will zero-initialise static
 * storage, i.e. allocated storage in the Blank Static Storage section. There is
 * not need to explicitly reset the ring buffer before use.
 * \param _name_ Name of the ring buffer.
 * \param _size_ Size of the ring buffer.
 */
#define RING_BUF_DEFINE_STATIC(_name_, _size_)                                 \
  static uint8_t _ring_buf_space_##_name_[_size_];                             \
  static struct ring_buf _name_ = {.space = _ring_buf_space_##_name_,          \
                                   .size = _size_}

/*!
 * \}
 */

#ifdef __cplusplus
}
#endif

#endif /* __RING_BUF_H__ */
