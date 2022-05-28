// Copyright (c) 2020-2021 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <cstdint>
#include <vector>

// SWIG seems to struggle with size_t/uintptr_t, even with custom typemap stuff.
#if defined(SIZE_T_IS_UNSIGNED_INT)
using UIntPtrT = uint32_t;
#else
using UIntPtrT = unsigned long;
#endif

using SizeVector = std::vector<UIntPtrT>;

static_assert(sizeof(UIntPtrT) == sizeof(void*), "Can't reinterpret_cast size type to void*");

#include <SoapySDR/Constants.h>
#include <SoapySDR/Device.hpp>
#include <SoapySDR/Formats.hpp>
#include <SoapySDR/Time.hpp>
#include <SoapySDR/Types.hpp>
#include <SoapySDR/Version.hpp>

#include <string>

namespace SoapySDR { namespace Java {

    struct BuildInfo
    {
        static const std::string APIVersion;
        static const std::string ABIVersion;
        static const std::string LibVersion;
    };

    const std::string BuildInfo::APIVersion = SoapySDR::getAPIVersion();
    const std::string BuildInfo::ABIVersion = SoapySDR::getABIVersion();
    const std::string BuildInfo::LibVersion = SoapySDR::getLibVersion();

    enum class Direction
    {
        Tx = 0,
        Rx = 1
    };

    enum class StreamFlags
    {
        None          = 0,
        EndBurst      = (1 << 1),
        HasTime       = (1 << 2),
        EndAbrupt     = (1 << 3),
        OnePacket     = (1 << 4),
        MoreFragments = (1 << 5),
        WaitTrigger   = (1 << 6)
    };

    struct StreamFormats
    {
        static const std::string CF64;
        static const std::string CF32;
        static const std::string CS32;
        static const std::string CU32;
        static const std::string CS16;
        static const std::string CU16;
        static const std::string CS12;
        static const std::string CU12;
        static const std::string CS8;
        static const std::string CU8;
        static const std::string CS4;
        static const std::string CU4;
        static const std::string F64;
        static const std::string F32;
        static const std::string S32;
        static const std::string U32;
        static const std::string S16;
        static const std::string U16;
        static const std::string S8;
        static const std::string U8;

        static inline size_t FormatToSize(const std::string& format)
        {
            return SoapySDR::formatToSize(format);
        }
    };

    const std::string StreamFormats::CF64 = SOAPY_SDR_CF64;
    const std::string StreamFormats::CF32 = SOAPY_SDR_CF32;
    const std::string StreamFormats::CS32 = SOAPY_SDR_CS32;
    const std::string StreamFormats::CU32 = SOAPY_SDR_CU32;
    const std::string StreamFormats::CS16 = SOAPY_SDR_CS16;
    const std::string StreamFormats::CU16 = SOAPY_SDR_CU16;
    const std::string StreamFormats::CS12 = SOAPY_SDR_CS12;
    const std::string StreamFormats::CU12 = SOAPY_SDR_CU12;
    const std::string StreamFormats::CS8  = SOAPY_SDR_CS8;
    const std::string StreamFormats::CU8  = SOAPY_SDR_CU8;
    const std::string StreamFormats::CS4  = SOAPY_SDR_CS4;
    const std::string StreamFormats::CU4  = SOAPY_SDR_CU4;
    const std::string StreamFormats::F64  = SOAPY_SDR_F64;
    const std::string StreamFormats::F32  = SOAPY_SDR_F32;
    const std::string StreamFormats::S32  = SOAPY_SDR_S32;
    const std::string StreamFormats::U32  = SOAPY_SDR_U32;
    const std::string StreamFormats::S16  = SOAPY_SDR_S16;
    const std::string StreamFormats::U16  = SOAPY_SDR_U16;
    const std::string StreamFormats::S8   = SOAPY_SDR_S8;
    const std::string StreamFormats::U8   = SOAPY_SDR_U8;

    struct StreamHandle
    {
        SoapySDR::Stream* stream{ nullptr };

        // Ignored
        SizeVector channels;
        inline SizeVector GetChannels() const {return channels;}

        // Ignored
        std::string format;
        inline std::string GetFormat() const {return format;}

        // Ignored
        inline UIntPtrT GetPointer() const {return reinterpret_cast<UIntPtrT>(stream);}
    };

    struct StreamResult
    {
        size_t NumSamples{0};
        StreamFlags Flags{ StreamFlags::None };
        long long TimeNs{0};
        long TimeoutUs{0};
        size_t ChanMask{0U};
    };

    struct Time
    {
        static inline long long TicksToTimeNs(const long long ticks, const double rate)
        {
            return SoapySDR::ticksToTimeNs(ticks, rate);
        }

        static inline long long TimeNsToTicks(const long long timeNs, const double rate)
        {
            return SoapySDR::timeNsToTicks(timeNs, rate);
        }
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

    struct SettingConversion
    {
        template <typename T>
        static inline std::string SettingToString(const T& setting)
        {
            return SoapySDR::SettingToString<T>(setting);
        }

        template <typename T>
        static inline T StringToSetting(const std::string& setting)
        {
            return SoapySDR::StringToSetting<T>(setting);
        }
    };
}}

// Note: we can't set these enums to the equivalent #define
// because SWIG will copy the #define directly, so we'll
// enforce equality with these static_asserts.
#define ENUM_CHECK(_enum,_define) \
    static_assert(int(_enum) == _define, #_define)

ENUM_CHECK(SoapySDR::Java::Direction::Tx, SOAPY_SDR_TX);
ENUM_CHECK(SoapySDR::Java::Direction::Rx, SOAPY_SDR_RX);
ENUM_CHECK(SoapySDR::Java::StreamFlags::EndBurst, SOAPY_SDR_END_BURST);
ENUM_CHECK(SoapySDR::Java::StreamFlags::HasTime, SOAPY_SDR_HAS_TIME);
ENUM_CHECK(SoapySDR::Java::StreamFlags::EndAbrupt, SOAPY_SDR_END_ABRUPT);
ENUM_CHECK(SoapySDR::Java::StreamFlags::OnePacket, SOAPY_SDR_ONE_PACKET);
ENUM_CHECK(SoapySDR::Java::StreamFlags::MoreFragments, SOAPY_SDR_MORE_FRAGMENTS);
ENUM_CHECK(SoapySDR::Java::StreamFlags::WaitTrigger, SOAPY_SDR_WAIT_TRIGGER);
