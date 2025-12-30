/*!
 * \file correlate_f32.h
 * \brief Float32 correlation function prototypes.
 */

#pragma once

/*
 * arm_math.h for float32_t type
 */
#include "arm_math.h"
#include "ring_buf.h"

/*!
 * \brief Correlate float32_t structure.
 * \details Holds buffers and state for float32_t correlation.
 */
struct correlate_f32 {
  /*
   * Output correlated data buffer must be at least expected_len + actual_len -
   * 1 in size to hold the full correlation result. Input expected and actual
   * data buffers must be at least correlated_len in size to hold the data prior
   * to correlation. Buffers are managed as ring buffers for dynamic data
   * addition.
   */
  float32_t *const correlated, *const expected, *const actual;
  struct ring_buf *const buf_expected, *const buf_actual;
  size_t correlated_len, expected_len, actual_len;
};

/*!
 * \brief Define a static correlate_f32 instance.
 * \param _name_ Name of the correlate_f32 instance.
 * \param _size_ Size of the expected and actual data buffers.
 */
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

/*!
 * \brief Add expected 32-bit float data to correlate_f32 instance.
 * \param correlate Correlate 32-bit float instance.
 * \param expected Expected 32-bit float data to add.
 * \returns 0 on success, negative error code on failure.
 */
int correlate_add_expected_f32(struct correlate_f32 *correlate, float32_t expected);

/*!
 * \brief Add actual 32-bit float data to correlate_f32 instance.
 * \param correlate Correlate 32-bit float instance.
 * \param actual Actual 32-bit float data to add.
 * \returns 0 on success, negative error code on failure.
 */
int correlate_add_actual_f32(struct correlate_f32 *correlate, float32_t actual);

/*!
 * \brief Perform correlation on the data in the correlate_f32 instance.
 * \param correlate Correlate 32-bit float instance.
 * \returns 0 on success, negative error code on failure.
 * \note Updates the correlated_len, expected_len, and actual_len fields.
 * \note Operates on all data currently in the expected and actual ring buffers.
 */
int correlate_f32(struct correlate_f32 *correlate);

/*!
 * \brief Get correlated 32-bit float data from a correlate_f32 instance.
 * \param correlate Correlate 32-bit float instance.
 * \param correlated Pointer to store address of correlated data array. Can be
 * NULL to ignore. Only the length is returned in this case.
 * \returns Length of the correlated data array in 32-bit floating-point elements.
 */
size_t correlate_get_correlated_f32(const struct correlate_f32 *correlate, float32_t **correlated);

/*!
 * \brief Get expected 32-bit float data from a correlate_f32 instance.
 * \param correlate Correlate 32-bit float instance.
 * \param expected Pointer to store address of expected data array. Can be NULL
 * to ignore, returning just the length.
 * \returns Length of the expected data array.
 */
size_t correlate_get_expected_f32(const struct correlate_f32 *correlate, float32_t **expected);

/*!
 * \brief Get actual 32-bit float data from a correlate_f32 instance.
 * \param correlate Correlate 32-bit float instance.
 * \param actual Pointer to store address of actual data array. Can be NULL to
 * ignore, returning just the length.
 * \returns Length of the actual data array.
 */
size_t correlate_get_actual_f32(const struct correlate_f32 *correlate, float32_t **actual);

/*!
 * \brief Get maximum value from correlated data in a correlate_f32 instance.
 * \param correlate Correlate 32-bit float instance.
 * \param max Pointer to store maximum correlated value. Can be NULL to ignore. In this case,
 * only the index is returned.
 * \returns Index of the maximum correlated value.
 */
size_t correlated_max_f32(const struct correlate_f32 *correlate, float32_t *max);

/*!
 * \brief Normalise correlated data in a correlate_f32 instance.
 * \param correlate Correlate 32-bit float instance.
 * \details Normalises the correlated data by dividing each element by the
 * square root of the product of the dot products of the expected and actual
 * data with themselves.
 * \note Avoids division by zero by checking against FLT_EPSILON.
 */
void correlate_normalise_f32(struct correlate_f32 *correlate);
