// Copyright (c) 2021-2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "Constants.hpp"
#include "Utility.hpp"

#include <SoapySDR/Constants.h>
#include <SoapySDR/Device.hpp>

#include <utility>

namespace SoapySDR { namespace CSharp { 

    enum class StreamFlags
    {
        None          = 0,
        EndBurst      = (1 << 1),
        HasTime       = (1 << 2),
        EndAbrupt     = (1 << 3),
        OnePacket     = (1 << 4),
        MoreFragments = (1 << 5),
        WaitTrigger   = (1 << 6),
        UserFlag0     = (1 << 16),
        UserFlag1     = (1 << 17),
        UserFlag2     = (1 << 18),
        UserFlag3     = (1 << 19),
        UserFlag4     = (1 << 20)
    };

    struct StreamFormat
    {
        static const std::string ComplexFloat64;
        static const std::string ComplexFloat32;
        static const std::string ComplexInt32;
        static const std::string ComplexUInt32;
        static const std::string ComplexInt16;
        static const std::string ComplexUInt16;
        static const std::string ComplexInt12;
        static const std::string ComplexUInt12;
        static const std::string ComplexInt8;
        static const std::string ComplexUInt8;
        static const std::string ComplexInt4;
        static const std::string ComplexUInt4;

        static inline size_t FormatToSize(const std::string& format)
        {
            return SoapySDR::formatToSize(format);
        }
    };

    const std::string StreamFormat::ComplexFloat64 = SOAPY_SDR_CF64;
    const std::string StreamFormat::ComplexFloat32 = SOAPY_SDR_CF32;
    const std::string StreamFormat::ComplexInt32   = SOAPY_SDR_CS32;
    const std::string StreamFormat::ComplexUInt32  = SOAPY_SDR_CU32;
    const std::string StreamFormat::ComplexInt16   = SOAPY_SDR_CS16;
    const std::string StreamFormat::ComplexUInt16  = SOAPY_SDR_CU16;
    const std::string StreamFormat::ComplexInt12   = SOAPY_SDR_CS12;
    const std::string StreamFormat::ComplexUInt12  = SOAPY_SDR_CU12;
    const std::string StreamFormat::ComplexInt8    = SOAPY_SDR_CS8;
    const std::string StreamFormat::ComplexUInt8   = SOAPY_SDR_CU8;
    const std::string StreamFormat::ComplexInt4    = SOAPY_SDR_CS4;
    const std::string StreamFormat::ComplexUInt4   = SOAPY_SDR_CU4;

    struct StreamHandle
    {
        SoapySDR::Stream* stream{ nullptr };

        // Ignored
        std::vector<unsigned> channels;
        inline std::vector<unsigned> GetChannels() const {return channels;}

        // Ignored
        std::string format;
        inline std::string GetFormat() const {return format;}

        // Ignored
        inline size_t GetPointer() const {return reinterpret_cast<size_t>(stream);}
    };

    struct StreamResult
    {
        size_t NumSamples{0};
        StreamFlags Flags{ StreamFlags::None };
        long long TimeNs{0};
        size_t ChanMask{0U};
    };
    
    using StreamResultPairInternal = std::pair<SoapySDR::CSharp::ErrorCode, SoapySDR::CSharp::StreamResult>;
}}

ENUM_CHECK(SoapySDR::CSharp::StreamFlags::EndBurst, SOAPY_SDR_END_BURST);
ENUM_CHECK(SoapySDR::CSharp::StreamFlags::HasTime, SOAPY_SDR_HAS_TIME);
ENUM_CHECK(SoapySDR::CSharp::StreamFlags::EndAbrupt, SOAPY_SDR_END_ABRUPT);
ENUM_CHECK(SoapySDR::CSharp::StreamFlags::OnePacket, SOAPY_SDR_ONE_PACKET);
ENUM_CHECK(SoapySDR::CSharp::StreamFlags::MoreFragments, SOAPY_SDR_MORE_FRAGMENTS);
ENUM_CHECK(SoapySDR::CSharp::StreamFlags::WaitTrigger, SOAPY_SDR_WAIT_TRIGGER);
ENUM_CHECK(SoapySDR::CSharp::StreamFlags::UserFlag0, SOAPY_SDR_USER_FLAG0);
ENUM_CHECK(SoapySDR::CSharp::StreamFlags::UserFlag1, SOAPY_SDR_USER_FLAG1);
ENUM_CHECK(SoapySDR::CSharp::StreamFlags::UserFlag2, SOAPY_SDR_USER_FLAG2);
ENUM_CHECK(SoapySDR::CSharp::StreamFlags::UserFlag3, SOAPY_SDR_USER_FLAG3);
ENUM_CHECK(SoapySDR::CSharp::StreamFlags::UserFlag4, SOAPY_SDR_USER_FLAG4);