#include "arm_math.h"
#include "correlate_f32.h"
#include "fcvtf.h"
#include "fepsiloneq.h"
#include "monitor_handles.h"

#include <assert.h>
#include <stdio.h>
#include <unistd.h>

CORRELATE_F32_DEFINE_STATIC(test_corr, 100);

static const float32_t x[] = {0.0f, 1.0f, 2.0f, 3.0f, 2.0f, 1.0f};
static const float32_t h[] = {0.5f, 0.25f, -0.25f};

int correlate_f32_test(void) {
  /*
   * Buffer for float-to-string conversion. Used by cvtfbuf() to avoid repeated
   * allocations.
   */
  char buf[80];

  /*
   * Load the input signals and run a correlation.
   * Fail the test if the correlation fails.
   */
  for (size_t i = 0; i < sizeof(x) / sizeof(x[0]); i++) {
    assert(correlate_add_expected_f32(&test_corr, x[i]) == 0);
  }
  for (size_t i = 0; i < sizeof(h) / sizeof(h[0]); i++) {
    assert(correlate_add_actual_f32(&test_corr, h[i]) == 0);
  }
  assert(correlate_f32(&test_corr) == 0);

  float_t *correlated;
  size_t correlated_len = correlate_get_correlated_f32(&test_corr, &correlated);
  assert(correlated_len == sizeof(x) / sizeof(x[0]) + sizeof(h) / sizeof(h[0]) - 1);

  /*
   * Print the correlation result.
   */
  printf("Correlation before normalisation:\n");
  for (size_t i = 0; i < correlated_len; i++) {
    /*
     * Note that nano newlib does not support %f format specifier. Nor does it
     * support %zu for size_t; it crashes!
     */
    (void)printf("  correlated[%3d] = %15s\n", (int)i, cvtfbuf(correlated[i], 9, buf));
  }

  /*
   * Get and print the peak correlation and its lag.
   */
  float_t peak;
  int32_t peak_lag = correlate_peak_lag_f32(&test_corr, &peak);
  assert(peak_lag != INT32_MIN);
  (void)printf("Peak correlation value %s at lag %ld\n", cvtfbuf(peak, 9, buf), (long)peak_lag);

  /*
   * Normalise the correlation result. Fail the test if normalisation fails.
   */
  assert(correlate_normalise_f32(&test_corr) == 0);
  printf("Correlation after normalisation:\n");
  for (size_t i = 0; i < correlated_len; i++) {
    printf("  correlated[%3d] = %15s\n", (int)i, cvtfbuf(correlated[i], 9, buf));
  }
  peak_lag = correlate_peak_lag_f32(&test_corr, &peak);
  assert(peak_lag != INT32_MIN);
  (void)printf("Normalised peak correlation value %s at lag %ld\n", cvtfbuf(peak, 9, buf),
               (long)peak_lag);

  /*
   * Check normalised maximum value. Use epsilon of one (although it succeeds
   * with one epsilon) to allow for accumulated numerical error in the
   * normalisation process and rounding errors when comparing the maximum with
   * the expected value expressed as a floating-point number with only nine
   * decimal places. Do not presume anything about float representation beyond
   * single-precision IEEE 754's precision limits.
   */
  float_t max, min;
  assert(correlated_max_f32(&test_corr, &max) == 7U);
  assert(fepsiloneqf(3, 0.468292892F, max));
  assert(correlated_min_f32(&test_corr, &min) == 4U);
  assert(fepsiloneqf(3, -0.093658581F, min));

  return 0;
}

int main(void) {
  initialise_monitor_handles();
  (void)printf("Hello, World from %s!!!\n", "correlate_f32_test");

  assert(correlate_f32_test() == 0);

  _exit(0);
  return 0;
}
