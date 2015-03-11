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
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Convert a tick count into a time in nanoseconds using the tick rate.
 * \param ticks a integer tick count
 * \param rate the ticks per second
 * \return the time in nanoseconds
 */
static inline long long SoapySDR_ticksToTimeNs(const long long ticks, const double rate);

/*!
 * Convert a time in nanoseconds into a tick count using the tick rate.
 * \param timeNs time in nanoseconds
 * \param rate the ticks per second
 * \return the integer tick count
 */
static inline long long SoapySDR_timeNsToTicks(const long long timeNs, const double rate);

static inline long long __SoapySDR_scaleTicks(const long long in, const double rate);

static inline long long SoapySDR_ticksToTimeNs(const long long ticks, const double rate)
{
    return __SoapySDR_scaleTicks(ticks, 1e9/rate);
}

static inline long long SoapySDR_timeNsToTicks(const long long timeNs, const double rate)
{
    return __SoapySDR_scaleTicks(timeNs, rate/1e9);
}

static inline long long __SoapySDR_scaleTicks(const long long in, const double rate)
{
    static const int split = 32;

    //split the input into numbers that easily fit into the mantissa of float64
    const double inLow = (unsigned long long)(in & ((1llu << split)-1));
    const double inHigh = (in >> split);

    //perform scaling into the new rate on high and low numbers
    const double outLow = inLow*rate;
    const double outHigh = inHigh*rate;

    //convert to 64 bit integers (low, high (frac part), high (int part))
    const long long outLowLL = (long long)(outLow);
    const long long outHighLL = (long long)(outHigh);
    const double outHighFrac = (outHigh - outHighLL)*(1llu << split);
    const long long outHighFracLL = (long long)(outHighFrac);
    const double error = (outLow-outLowLL) + (outHighFrac-outHighFracLL);
    const int errorInt = (int)(error + ((error < 0.0)?-0.5:0.5));

    //result is the addition of all parts
    printf("__SoapySDR_scaleTicks in=%lld, rate=%f, outLowLL=%lld, outHighLL=%lld error(%f), outHighFracLL=%lld,\n", in, rate, outLowLL, outHighLL, error, outHighFracLL);
    return outLowLL + (outHighLL << split) + outHighFracLL + errorInt;
}

#ifdef __cplusplus
}
#endif
