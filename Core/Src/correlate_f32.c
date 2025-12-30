/*!
 * \file correlate_f32.c
 * \brief Float32 correlation function definitions.
 */

#include "correlate_f32.h"

#include "ring_buf.h"
#include "fepsiloneq.h"

#include <errno.h>

/*!
 * \brief Get used float32_t data from ring buffer.
 * \details Retrieves all used data from the ring buffer as float32_t elements.
 * \param buf Ring buffer.
 * \param data Destination array for retrieved data.
 * \returns Number of float32_t elements retrieved.
 */
static size_t ring_buf_get_used_f32(struct ring_buf *buf, float32_t *data);

int correlate_add_expected_f32(struct correlate_f32 *correlate, float32_t expected) {
  return ring_buf_put_circ(correlate->buf_expected, &expected, sizeof(expected));
}

int correlate_add_actual_f32(struct correlate_f32 *correlate, float32_t actual) {
  return ring_buf_put_circ(correlate->buf_actual, &actual, sizeof(actual));
}

int correlate_f32(struct correlate_f32 *correlate) {
  const size_t expected_len = ring_buf_get_used_f32(correlate->buf_expected, correlate->expected);
  const size_t actual_len = ring_buf_get_used_f32(correlate->buf_actual, correlate->actual);
  correlate->expected_len = expected_len;
  correlate->actual_len = actual_len;
  if (actual_len == 0U || expected_len == 0U) {
    correlate->correlated_len = 0U;
    return -EINVAL;
  }
  arm_correlate_f32(correlate->expected, expected_len, correlate->actual, actual_len,
                    correlate->correlated);
  correlate->correlated_len = expected_len + actual_len - 1U;
  return 0;
}

size_t correlate_get_correlated_f32(const struct correlate_f32 *correlate, float32_t **correlated) {
  if (correlated != NULL) {
    *correlated = correlate->correlated;
  }
  return correlate->correlated_len;
}

size_t correlate_get_expected_f32(const struct correlate_f32 *correlate, float32_t **expected) {
  if (expected != NULL) {
    *expected = correlate->expected;
  }
  return correlate->expected_len;
}

size_t correlate_get_actual_f32(const struct correlate_f32 *correlate, float32_t **actual) {
  if (actual != NULL) {
    *actual = correlate->actual;
  }
  return correlate->actual_len;
}

size_t correlated_max_f32(const struct correlate_f32 *correlate, float32_t *max) {
  float32_t value;
  uint32_t index;
  arm_max_f32(correlate->correlated, correlate->correlated_len, &value, &index);
  if (max != NULL) {
    *max = value;
  }
  return (size_t)index;
}

void correlate_normalise_f32(struct correlate_f32 *correlate) {
  float32_t expected_dot, actual_dot;
  /*
   * Normalise by sqrt(expected_dot * actual_dot) where expected_dot = sum
   * expected^2, actual_dot = sum actual^2 using CMSIS-DSP for dot product
   * calculation, i.e. the sum of the squares of the elements.
   */
  arm_dot_prod_f32(correlate->expected, correlate->expected, correlate->expected_len,
                   &expected_dot);
  arm_dot_prod_f32(correlate->actual, correlate->actual, correlate->actual_len, &actual_dot);
  float32_t denom = sqrtf(expected_dot * actual_dot);
  /*
   * Only normalise if denom is not too small to avoid division by zero.
   * FLT_EPSILON is the smallest such that 1.0 + FLT_EPSILON != 1.0 in
   * single-precision floating point.
   */
  if (FLT_EPSILON <= denom) {
    for (size_t n = 0; n < correlate->correlated_len; ++n) {
      correlate->correlated[n] /= denom;
    }
  }
}

static size_t ring_buf_get_used_f32(struct ring_buf *buf, float32_t *data) {
  size_t len = ring_buf_get(buf, data, ring_buf_used_space(buf)) / sizeof(float32_t);
  (void)ring_buf_get_ack(buf, 0U);
  return len;
}
