/* SPDX-License-Identifier: MIT */
/*!
 * \file monitor_handles.h
 * \brief Header file for monitor handles initialisation.
 * \details This file declares the function to initialise monitor handles
 * for semihosting support in the testing environment.
 */

#pragma once

/*!
 * \brief Initialise the monitor handles for semihosting support.
 * \details File handles will not be available until this function is called.
 */
void initialise_monitor_handles(void);
