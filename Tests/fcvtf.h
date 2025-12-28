/* SPDX-License-Identifier: MIT */
/*!
 * \file fcvtf.h
 */

#pragma once

/*!
 * \brief Print a float using fcvtf.
 * \details Prints the float number \p d with \p ndigit digits after the
 * decimal point using the legacy function fcvtf().
 * \param d Float number to print.
 * \param ndigit Number of digits to print after the decimal point.
 * \return Number of characters printed.
 */
int fcvtfprintf(float d, int ndigit);

/*!
 * \brief Convert a float to string using fcvtf.
 * \details Converts the float number \p d with \p ndigit digits after the
 * decimal point to a string using the legacy function fcvtf().
 * \param d Float number to convert.
 * \param ndigit Number of digits to convert after the decimal point.
 * \return Pointer to the converted string.
 * \note The returned pointer points to a static buffer that is overwritten on
 * each call. Therefore, the string should be used or copied before calling this
 * function again. The buffer size is 80 bytes. The function is not thread-safe.
 * \note Take care when using this function in the same expression multiple
 * times, as it returns a pointer to a static buffer that is overwritten on each
 * call.
 */
const char *_fcvtf(float d, int ndigit);

/*!
 * \brief Convert a float to string using \c fcvtf() into a user-provided buffer.
 * \details Converts the float number \p f with \p ndigit digits after the
 * decimal point to a string using the legacy function fcvtf(). The result is
 * stored in the user-provided buffer \p buf. If \p buf is \c NULL, a new buffer
 * is allocated using the standard library's \c realloc() function.
 * \param f Float number to convert.
 * \param ndigit Number of digits to convert after the decimal point.
 * \param buf User-provided buffer to store the result. If \c NULL, a new buffer
 * is allocated.
 * \return Pointer to the converted string in the provided or newly-allocated
 * buffer. Returns \c NULL if memory allocation fails.
 * \note The caller is responsible for freeing the returned buffer if it was
 * allocated by the function (i.e., when \p buf is \c NULL).
 */
char *cvtfbuf(float f, int ndigit, char *buf);
