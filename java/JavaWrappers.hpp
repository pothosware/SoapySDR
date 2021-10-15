/// Copyright (c) 2021 Nicholas Corgan
/// SPDX-License-Identifier: BSL-1.0
///

#pragma once

#include <SoapySDR/Formats.h>
#include <SoapySDR/Time.hpp>

#include <string>

namespace SoapySDR { namespace Java {

    enum class Direction
    {
        TX,
        RX
    };

    enum class ErrorCode
    {
        TIMEOUT = -1,
        STREAM_ERROR = -2,
        CORRUPTION = -3,
        OVERFLOW = -4,
        NOT_SUPPORTED = -5,
        TIME_ERROR = -6,
        UNDERFLOW = -7
    };

    struct Time
    {
        static inline long long ticksToTimeNs(const long long ticks, const double rate)
        {
            return SoapySDR::ticksToTimeNs(ticks, rate);
        }

        static inline long long timeNsToTicks(const long long timeNs, const double rate)
        {
            return SoapySDR::timeNsToTicks(timeNs, rate);
        }
    };

}}
