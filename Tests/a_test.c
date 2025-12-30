#include "arm_math.h"
#include "fcvtf.h"
#include "monitor_handles.h"

#include "arm_math.h"
#include "ring_buf.h"
#include "ring_buf_circ.h"
#include "fepsiloneq.h"

struct correlate_f32 {
  float32_t *const correlated, *const expected, *const actual;
  struct ring_buf *const buf_expected, *const buf_actual;
  size_t correlated_len, expected_len, actual_len;
};

#define CORRELATE_F32_DEFINE_STATIC(_name_, _size_)                                                \
  static float32_t _name_##_correlated[_size_ + _size_ - 1];                                       \
  static float32_t _name_##_expected[_size_];                                                      \
  static float32_t _name_##_actual[_size_];                                                        \
  RING_BUF_DEFINE_STATIC(_name_##_buf_expected, sizeof(float[_size_]));                            \
  RING_BUF_DEFINE_STATIC(_name_##_buf_actual, sizeof(float[_size_]));                              \
  struct correlate_f32 _name_ = {                                                                  \
      .correlated = _name_##_correlated,                                                           \
      .expected = _name_##_expected,                                                               \
      .actual = _name_##_actual,                                                                   \
      .buf_expected = &_name_##_buf_expected,                                                      \
      .buf_actual = &_name_##_buf_actual,                                                          \
  }

int correlate_add_expected_f32(struct correlate_f32 *correlate, float32_t expected) {
  return ring_buf_put_circ(correlate->buf_expected, &expected, sizeof(expected));
}

int correlate_add_actual_f32(struct correlate_f32 *correlate, float32_t actual) {
  return ring_buf_put_circ(correlate->buf_actual, &actual, sizeof(actual));
}

/*!
 * \brief Get used float32_t data from ring buffer.
 * \details Retrieves all used data from the ring buffer as float32_t elements.
 * \param buf Ring buffer.
 * \param data Destination array for retrieved data.
 * \returns Number of float32_t elements retrieved.
 */
static size_t ring_buf_get_used_f32(struct ring_buf *buf, float32_t *data) {
  size_t len = ring_buf_get(buf, data, ring_buf_used_space(buf)) / sizeof(float32_t);
  (void)ring_buf_get_ack(buf, 0U);
  return len;
}

void correlate_f32(struct correlate_f32 *correlate) {
  const size_t expected_len = ring_buf_get_used_f32(correlate->buf_expected, correlate->expected);
  const size_t actual_len = ring_buf_get_used_f32(correlate->buf_actual, correlate->actual);
  correlate->expected_len = expected_len;
  correlate->actual_len = actual_len;
  if (actual_len == 0U || expected_len == 0U) {
    correlate->correlated_len = 0U;
    return;
  }
  arm_correlate_f32(correlate->expected, expected_len, correlate->actual, actual_len,
                    correlate->correlated);
  correlate->correlated_len = expected_len + actual_len - 1U;
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

#include <assert.h>
#include <stdio.h>
#include <unistd.h>

/*!
 * \brief Cross-correlate two float sequences.
 * \param x First input float sequence.
 * \param len_x Length of the first input sequence.
 * \param y Second input float sequence.
 * \param len_y Length of the second input sequence.
 * \param result Output array to store the cross-correlation result.
 * \details This function computes the cross-correlation of two float
 * sequences x and y, storing the result in the provided output array. The length
 * of the result array should be at least len_x + len_y - 1.
 */
static void conv_f32(const float *x, int len_x, const float *y, int len_y, float *result) {
  int len_result = len_x + len_y - 1;
  for (int i = 0; i < len_result; i++) {
    result[i] = 0;
    for (int j = 0; j < len_x; j++) {
      /*
       * reversed convolution index relation:
       * k = i - j
       * y[k] aligns with x[j] for the current output result[i]
       * k must be within the bounds of y
       */
      int k = i - j;
      if (k >= 0 && k < len_y) {
        result[i] += x[j] * y[k];
      }
    }
  }
}

/*
 * Example: Using arm_conv_f32 and arm_correlate_f32 from CMSIS-DSP
 *
 * Build (generic GCC for desktop test of API layout):
 *   gcc -std=c11 -O2 example.c -o example \
 *       -I/path/to/CMSIS/DSP/Include \
 *       -L/path/to/lib -larm_cortexM_math
 *
 * On embedded targets, use your usual ARM toolchain and linker setup.
 */

#include "arm_math.h" // or "dsp/arm_math.h" depending on CMSIS layout
#include <math.h>
#include <stdio.h>

int a_test_1(void) {
  /*
   * Buffer for float-to-string conversion. Used by cvtfbuf() to avoid repeated
   * allocations.
   */
  char buf[80];

  // --- Input signals ---
  // x[n]: a short sequence; for convolution, think of it as the "input".
  // h[n]: impulse response / filter taps for convolution.
  const float32_t x[] = {0.0f, 1.0f, 2.0f, 3.0f, 2.0f, 1.0f};
  const float32_t h[] = {0.5f, 0.25f, -0.25f};

  const uint32_t Nx = sizeof(x) / sizeof(x[0]); // 6
  const uint32_t Nh = sizeof(h) / sizeof(h[0]); // 3

  // --- Output lengths ---
  // For both convolution and correlation with lengths Nx and Nh:
  // output length = Nx + Nh - 1
  const uint32_t Nout = Nx + Nh - 1;

  // --- Buffers ---
  float32_t y_conv[Nout]; // Convolution output
  float32_t r_corr[Nout]; // Correlation output

  // --- Convolution: y[n] = x[n] * h[n] (with h time-reversed internally) ---
  arm_conv_f32(x, Nx, h, Nh, y_conv);

  // --- Correlation: r_xy[lag] = sum_k x[k] * y[k+lag] (no time reversal) ---
  // Here we correlate x with h for illustration.
  arm_correlate_f32(x, Nx, h, Nh, r_corr);

  // --- Print results ---
  printf("Convolution y_conv (length %u):\n", (unsigned)Nout);
  for (uint32_t n = 0; n < Nout; ++n) {
    printf("  y_conv[%2u] = %s\n", (unsigned)n, cvtfbuf(y_conv[n], 5, buf));
  }

  printf("\nCorrelation r_corr (length %u):\n", (unsigned)Nout);
  for (uint32_t n = 0; n < Nout; ++n) {
    printf("  r_corr[%2u] = %s\n", (unsigned)n, cvtfbuf(r_corr[n], 5, buf));
  }

  // --- Optional: find peak correlation and its lag ---
  float32_t maxVal;
  uint32_t maxIdx;
  arm_max_f32(r_corr, Nout, &maxVal, &maxIdx);

  // For correlation between sequences of lengths Nx and Nh, the "zero lag" index
  // is (Nh - 1). Positive lag corresponds to shifting h forward relative to x.
  int32_t zeroLagIndex = (int32_t)Nh - 1;
  int32_t peakLag = (int32_t)maxIdx - zeroLagIndex;

  printf("\nPeak correlation:\n");
  printf("  max value = %s at index %u (lag = %ld samples)\n", cvtfbuf(maxVal, 5, buf),
         (unsigned)maxIdx, peakLag);

  // --- Optional normalisation (if desired) ---
  // Normalised correlation often divides by sqrt(E_x * E_h)
  // where E_x = sum x^2, E_h = sum h^2. This yields values in [-1, 1].
  float32_t Ex, Eh;
  arm_dot_prod_f32(x, x, Nx, &Ex); // sum of squares of x
  arm_dot_prod_f32(h, h, Nh, &Eh); // sum of squares of h
  float32_t denom = sqrtf(Ex * Eh);
  if (denom > 0.0f) {
    printf("Normalised correlation r_corr_norm:\n");
    for (uint32_t n = 0; n < Nout; ++n) {
      float32_t r_norm = r_corr[n] / denom;
      printf("  r_corr_norm[%2u] = %s\n", (unsigned)n, cvtfbuf(r_norm, 5, buf));
    }
  }

  CORRELATE_F32_DEFINE_STATIC(test_corr, 16);
  for (size_t i = 0; i < sizeof(x) / sizeof(x[0]); i++) {
    assert(correlate_add_expected_f32(&test_corr, x[i]) == 0);
  }
  for (size_t i = 0; i < sizeof(h) / sizeof(h[0]); i++) {
    assert(correlate_add_actual_f32(&test_corr, h[i]) == 0);
  }
  correlate_f32(&test_corr);
  float_t *correlated;
  size_t correlated_len = correlate_get_correlated_f32(&test_corr, &correlated);
  assert(correlated_len == Nout);
  for (size_t i = 0; i < correlated_len; i++) {
    assert(fepsiloneqf(1, correlated[i], r_corr[i]));
  }
  float_t max;
  assert(correlated_max_f32(&test_corr, &max) == maxIdx);
  assert(FLT_EPSILON >= fabsf(max - maxVal));

  correlate_normalise_f32(&test_corr);
  printf("Normalised correlation:\n");
  for (size_t i = 0; i < correlated_len; i++) {
    printf("  correlated[%5u] = %s\n", (unsigned)i, cvtfbuf(correlated[i], 5, buf));
  }

  /*
   * Check normalised maximum value. Use epsilon of 100 to allow for accumulated
   * numerical error in the normalisation process and rounding errors when
   * comparing the maximum with the expected value expressed as a floating-point
   * number with only five decimal places.
   */
  {
    float_t max;
    assert(correlated_max_f32(&test_corr, &max) == maxIdx);
    assert(fepsiloneqf(100, 0.46829F, max));
  }

  return 0;
}

int main(void) {
  initialise_monitor_handles();
  (void)printf("Hello, World from %s!!!\n", "a_test");

  assert(a_test_1() == 0);

  const float x[] = {1.0f, 2.0f, 3.0f, 4.0f};
  const float y[] = {5.0f, 6.0f, 7.0f};
  float result[sizeof(x) / sizeof(x[0]) + sizeof(y) / sizeof(y[0]) - 1] = {0.0f};
  conv_f32(x, sizeof(x) / sizeof(x[0]), y, sizeof(y) / sizeof(y[0]), result);
  (void)printf("Cross-correlation result:\n");
  for (size_t i = 0; i < sizeof(result) / sizeof(result[0]); i++) {
    (void)printf("%s ", cvtfbuf(result[i], 6, NULL));
  }
  (void)printf("\n");
  assert(5.0f == result[0]);
  assert(16.0f == result[1]);
  assert(34.0f == result[2]);
  assert(52.0f == result[3]);
  assert(45.0f == result[4]);
  assert(28.0f == result[5]);

  /*
   * Now use the ARM CMSIS DSP library to perform the same cross-correlation.
   * This serves as a validation of the direct implementation.
   */
  (void)memset(result, 0, sizeof(result));
  arm_conv_f32(x, sizeof(x) / sizeof(x[0]), y, sizeof(y) / sizeof(y[0]), result);
  (void)printf("ARM CMSIS cross-correlation result:\n");
  for (size_t i = 0; i < sizeof(result) / sizeof(result[0]); i++) {
    (void)printf("%s ", cvtfbuf(result[i], 6, NULL));
  }
  (void)printf("\n");
  assert(5.0f == result[0]);
  assert(16.0f == result[1]);
  assert(34.0f == result[2]);
  assert(52.0f == result[3]);
  assert(45.0f == result[4]);
  assert(28.0f == result[5]);

  RING_BUF_DEFINE_STATIC(buf, sizeof(float[4U]));

  float number = 1.0F;
  ring_buf_size_t ack;
  while ((ack = ring_buf_put(&buf, &number, sizeof(number)))) {
    int err = ring_buf_put_ack(&buf, ack);
    assert(err == 0);
    number += 1.0F;
  }

  float sum = 0.0F;
  void *space;
  while ((ring_buf_get_claim(&buf, &space, sizeof(float))))
    sum += *(float *)space;
  assert(sum == 1.0F + 2.0F + 3.0F + 4.0F);

  assert(ring_buf_free_space(&buf) == 0U);
  ring_buf_get_ack(&buf, 0U);
  assert(ring_buf_used_space(&buf) == sizeof(float[4U]));

  while ((ack = ring_buf_get(&buf, &number, sizeof(number)))) {
    int err = ring_buf_get_ack(&buf, ack);
    assert(err == 0);

    (void)printf("number=");
    (void)fcvtfprintf(number, 3);
    (void)printf("\n");
  }

  assert(ring_buf_free_space(&buf) == sizeof(float[4U]));
  assert(ring_buf_used_space(&buf) == 0U);

  _exit(0);
  return 0;
}
