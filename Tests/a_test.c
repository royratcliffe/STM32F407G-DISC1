#include "arm_math.h"
#include "fcvtf.h"
#include "monitor_handles.h"
#include "ring_buf.h"

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

int main(void) {
  initialise_monitor_handles();
  (void)printf("Hello, World from %s!!!\n", "a_test");

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

  RING_BUF_DEFINE(buf, sizeof(float[4U]));

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
