// Copyright (c) 2020-2022 Nicholas Corgan
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
ENUM_CHECK(SoapySDR::CSharp::ErrorCode::Timeout, SOAPY_SDR_TIMEOUT);
ENUM_CHECK(SoapySDR::CSharp::ErrorCode::StreamError, SOAPY_SDR_STREAM_ERROR);
ENUM_CHECK(SoapySDR::CSharp::ErrorCode::Corruption, SOAPY_SDR_CORRUPTION);
ENUM_CHECK(SoapySDR::CSharp::ErrorCode::Overflow, SOAPY_SDR_OVERFLOW);
ENUM_CHECK(SoapySDR::CSharp::ErrorCode::NotSupported, SOAPY_SDR_NOT_SUPPORTED);
ENUM_CHECK(SoapySDR::CSharp::ErrorCode::TimeError, SOAPY_SDR_TIME_ERROR);
ENUM_CHECK(SoapySDR::CSharp::ErrorCode::Underflow, SOAPY_SDR_UNDERFLOW);
ENUM_CHECK(SoapySDR::CSharp::LogLevel::Fatal, SOAPY_SDR_FATAL);
ENUM_CHECK(SoapySDR::CSharp::LogLevel::Critical, SOAPY_SDR_CRITICAL);
ENUM_CHECK(SoapySDR::CSharp::LogLevel::Error, SOAPY_SDR_ERROR);
ENUM_CHECK(SoapySDR::CSharp::LogLevel::Warning, SOAPY_SDR_WARNING);
ENUM_CHECK(SoapySDR::CSharp::LogLevel::Notice, SOAPY_SDR_NOTICE);
ENUM_CHECK(SoapySDR::CSharp::LogLevel::Info, SOAPY_SDR_INFO);
ENUM_CHECK(SoapySDR::CSharp::LogLevel::Debug, SOAPY_SDR_DEBUG);
ENUM_CHECK(SoapySDR::CSharp::LogLevel::Trace, SOAPY_SDR_TRACE);
ENUM_CHECK(SoapySDR::CSharp::LogLevel::SSI, SOAPY_SDR_SSI);