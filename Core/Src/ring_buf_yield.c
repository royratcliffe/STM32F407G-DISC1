/*!
 * \file ring_buf_yield.c
 * \brief Implementation of ring buffer yielding functions.
 * \details This file contains the definitions for functions that allow
 * yielding of ring buffer space to a callback function.
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
