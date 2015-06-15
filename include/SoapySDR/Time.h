///
/// \file SoapySDR/Time.h
///
/// Utility functions to convert time and ticks.
///
/// \copyright
/// Copyright (c) 2015-2015 Josh Blum
/// SPDX-License-Identifier: BSL-1.0
///

#pragma once
#include <SoapySDR/Config.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Convert a tick count into a time in nanoseconds using the tick rate.
 * \param ticks a integer tick count
 * \param rate the ticks per second
 * \return the time in nanoseconds
 */
SOAPY_SDR_API long long SoapySDR_ticksToTimeNs(const long long ticks, const double rate);

/*!
 * Convert a time in nanoseconds into a tick count using the tick rate.
 * \param timeNs time in nanoseconds
 * \param rate the ticks per second
 * \return the integer tick count
 */
SOAPY_SDR_API long long SoapySDR_timeNsToTicks(const long long timeNs, const double rate);

#ifdef __cplusplus
}
#endif
