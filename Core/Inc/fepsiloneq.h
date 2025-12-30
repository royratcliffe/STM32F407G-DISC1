/*!
 * \file fepsiloneq.h
 * \copyright (c) 2024, Roy Ratcliffe, Northumberland, United Kingdom
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

/*
 * float.h for DBL_EPSILON and friends
 * math.h for fabs(3) and friends
 */
#include <float.h>
#include <math.h>

/*!
 * \brief Epsilon equality for double-precision floating-point numbers.
 * \details Succeeds only when the absolute difference between the two given
 * numbers X and Y is less than or equal to epsilon, or some factor (epsilons)
 * of epsilon according to rounding limitations.
 * \param n Number of epsilons to use for difference threshold.
 * \param x First floating-point number.
 * \param y Second floating-point number.
 * \retval True if \c x and \c y compare equal.
 */
// [[Rcpp::export]]
static inline bool fepsiloneq(unsigned n, double x, double y) {
  return n * DBL_EPSILON >= fabs(x - y);
}

/*!
 * \brief Epsilon equality for single-precision floating-point numbers.
 * \param n Number of epsilons to use for difference threshold.
 * \param x First floating-point number.
 * \param y Second floating-point number.
 * \retval True if \c x and \c y compare equal.
 */
// [[Rcpp::export]]
static inline bool fepsiloneqf(unsigned n, float x, float y) {
  return n * FLT_EPSILON >= fabsf(x - y);
}
