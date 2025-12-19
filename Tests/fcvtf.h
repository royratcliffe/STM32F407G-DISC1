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
