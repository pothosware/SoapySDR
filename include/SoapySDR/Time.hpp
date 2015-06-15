///
/// \file SoapySDR/Time.hpp
///
/// Utility functions to convert time and ticks.
///
/// \copyright
/// Copyright (c) 2015-2015 Josh Blum
/// SPDX-License-Identifier: BSL-1.0
///

#pragma once
#include <SoapySDR/Config.hpp>
#include <SoapySDR/Time.h>

namespace SoapySDR
{

/*!
 * Convert a tick count into a time in nanoseconds using the tick rate.
 * \param ticks a integer tick count
 * \param rate the ticks per second
 * \return the time in nanoseconds
 */
static inline long long ticksToTimeNs(const long long ticks, const double rate);

/*!
 * Convert a time in nanoseconds into a tick count using the tick rate.
 * \param timeNs time in nanoseconds
 * \param rate the ticks per second
 * \return the integer tick count
 */
static inline long long timeNsToTicks(const long long timeNs, const double rate);

}

static inline long long SoapySDR::ticksToTimeNs(const long long ticks, const double rate)
{
    return SoapySDR_ticksToTimeNs(ticks, rate);
}

static inline long long SoapySDR::timeNsToTicks(const long long timeNs, const double rate)
{
    return SoapySDR_timeNsToTicks(timeNs, rate);
}
