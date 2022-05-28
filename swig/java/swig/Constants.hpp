// Copyright (c) 2020-2021 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <SoapySDR/Constants.h>
#include <SoapySDR/Formats.hpp>
#include <SoapySDR/Time.hpp>
#include <SoapySDR/Types.hpp>
#include <SoapySDR/Version.hpp>

#include <string>

namespace SoapySDR { namespace CSharp {

    enum class Direction
    {
        Tx = 0,
        Rx = 1
    };

    // Note: we need to repeat the literal enum values or
    //       SWIG will copy SOAPY_SDR* into the C# file.
    enum class ErrorCode
    {
        None         = 0,
        Timeout      = -1,
        StreamError  = -2,
        Corruption   = -3,
        Overflow     = -4,
        NotSupported = -5,
        TimeError    = -6,
        Underflow    = -7
    };

    // Note: we need to repeat the literal enum values or
    //       SWIG will copy SOAPY_SDR* into the C# file.
    enum class LogLevel
    {
        Fatal    = 1,
        Critical = 2,
        Error    = 3,
        Warning  = 4,
        Notice   = 5,
        Info     = 6,
        Debug    = 7,
        Trace    = 8,
        SSI      = 9
    };
}}

// Note: we can't set these enums to the equivalent #define
// because SWIG will copy the #define directly, so we'll
// enforce equality with these static_asserts.
#define ENUM_CHECK(_enum,_define) \
    static_assert(int(_enum) == _define, #_define)

ENUM_CHECK(SoapySDR::CSharp::Direction::Tx, SOAPY_SDR_TX);
ENUM_CHECK(SoapySDR::CSharp::Direction::Rx, SOAPY_SDR_RX);