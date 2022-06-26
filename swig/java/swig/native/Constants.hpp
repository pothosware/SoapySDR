// Copyright (c) 2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <SoapySDR/Constants.h>
#include <SoapySDR/Errors.h>
#include <SoapySDR/Formats.hpp>
#include <SoapySDR/Logger.hpp>
#include <SoapySDR/Time.hpp>
#include <SoapySDR/Types.hpp>
#include <SoapySDR/Version.hpp>

#include <string>

namespace SoapySDR { namespace Java {

    // Note: we need to repeat the literal enum values or
    //       SWIG will copy SOAPY_SDR* into the Java file.
    enum class Direction
    {
        TX = 0,
        RX = 1
    };

    // Note: we need to repeat the literal enum values or
    //       SWIG will copy SOAPY_SDR* into the Java file.
    enum class ErrorCode
    {
        NONE          = 0,
        TIMEOUT       = -1,
        STREAM_ERROR  = -2,
        CORRUPTION    = -3,
        OVERFLOW      = -4,
        NOT_SUPPORTED = -5,
        TIME_ERROR    = -6,
        UNDERFLOW     = -7
    };

    // Note: we need to repeat the literal enum values or
    //       SWIG will copy SOAPY_SDR* into the Java file.
    enum class LogLevel
    {
        FATAL    = 1,
        CRITICAL = 2,
        ERROR    = 3,
        WARNING  = 4,
        NOTICE   = 5,
        INFO     = 6,
        DEBUG    = 7,
        TRACE    = 8,
        SSI      = 9
    };
}}

// Note: we can't set these enums to the equivalent #define
// because SWIG will copy the #define directly, so we'll
// enforce equality with these static_asserts.
#define ENUM_CHECK(_enum,_define) \
    static_assert(int(_enum) == _define, #_define)

ENUM_CHECK(SoapySDR::Java::Direction::TX, SOAPY_SDR_TX);
ENUM_CHECK(SoapySDR::Java::Direction::RX, SOAPY_SDR_RX);
ENUM_CHECK(SoapySDR::Java::ErrorCode::TIMEOUT, SOAPY_SDR_TIMEOUT);
ENUM_CHECK(SoapySDR::Java::ErrorCode::STREAM_ERROR, SOAPY_SDR_STREAM_ERROR);
ENUM_CHECK(SoapySDR::Java::ErrorCode::CORRUPTION, SOAPY_SDR_CORRUPTION);
ENUM_CHECK(SoapySDR::Java::ErrorCode::OVERFLOW, SOAPY_SDR_OVERFLOW);
ENUM_CHECK(SoapySDR::Java::ErrorCode::NOT_SUPPORTED, SOAPY_SDR_NOT_SUPPORTED);
ENUM_CHECK(SoapySDR::Java::ErrorCode::TIME_ERROR, SOAPY_SDR_TIME_ERROR);
ENUM_CHECK(SoapySDR::Java::ErrorCode::UNDERFLOW, SOAPY_SDR_UNDERFLOW);
ENUM_CHECK(SoapySDR::Java::LogLevel::FATAL, SOAPY_SDR_FATAL);
ENUM_CHECK(SoapySDR::Java::LogLevel::CRITICAL, SOAPY_SDR_CRITICAL);
ENUM_CHECK(SoapySDR::Java::LogLevel::ERROR, SOAPY_SDR_ERROR);
ENUM_CHECK(SoapySDR::Java::LogLevel::WARNING, SOAPY_SDR_WARNING);
ENUM_CHECK(SoapySDR::Java::LogLevel::NOTICE, SOAPY_SDR_NOTICE);
ENUM_CHECK(SoapySDR::Java::LogLevel::INFO, SOAPY_SDR_INFO);
ENUM_CHECK(SoapySDR::Java::LogLevel::DEBUG, SOAPY_SDR_DEBUG);
ENUM_CHECK(SoapySDR::Java::LogLevel::TRACE, SOAPY_SDR_TRACE);
ENUM_CHECK(SoapySDR::Java::LogLevel::SSI, SOAPY_SDR_SSI);
