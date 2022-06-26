// Copyright (c) 2021-2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "Constants.hpp"

#include <SoapySDR/Constants.h>
#include <SoapySDR/Device.hpp>

#include <cassert>
#include <vector>

// TODO: docs

namespace SoapySDR { namespace Java {

struct NativeStreamFormat
{
    std::string format;
    double fullScale;
};

enum class StreamFlags
{
    NONE           = 0,
    END_BURST      = (1 << 1),
    HAS_TIME       = (1 << 2),
    END_ABRUPT     = (1 << 3),
    ONE_PACKET     = (1 << 4),
    MORE_FRAGMENTS = (1 << 5),
    WAIT_TRIGGER   = (1 << 6)
};

struct StreamResult
{
    SoapySDR::Java::ErrorCode errorCode;
    size_t numSamples{0};
    StreamFlags flags{ StreamFlags::NONE }; // TODO: needs to be exposed as an EnumSet
    long long timeNs{0};
    long timeoutUs{0};
    size_t chanMask{0U};
};

// Construction hidden from SWIG
class Stream
{
public:
    Stream(void) = delete;

    Stream(
        SoapySDR::Device *device,
        const int direction,
        const std::string &format,
        const std::vector<size_t> &channels,
        const SoapySDR::Kwargs &args);

    virtual ~Stream(void);

    inline std::string getFormat(void) const
    {
        return _format;
    }

    inline std::vector<size_t> getChannels(void) const
    {
        return _channels;
    }

    inline SoapySDR::Kwargs getArgs(void) const
    {
        return _args;
    }

    inline void close(void)
    {
        assert(_device);

        _device->closeStream(_stream);
    }

    inline size_t getMTU(void) const
    {
        assert(_device);

        return _device->getStreamMTU(_stream);
    }

    inline bool active(void) const
    {
        assert(_device);

        return _active;
    }

    ErrorCode activate(
        const int flags = 0,
        const long long timeNs = 0,
        const size_t numElems = 0);

    ErrorCode deactivate(
        const int flags = 0,
        const long long timeNs = 0);

protected:
    SoapySDR::Device *_device{nullptr};
    SoapySDR::Stream *_stream{nullptr};

    const std::string _format;
    const std::vector<size_t> _channels;
    const SoapySDR::Kwargs _args;

    bool _active{false};
};
}}

ENUM_CHECK(SoapySDR::Java::StreamFlags::END_BURST, SOAPY_SDR_END_BURST);
ENUM_CHECK(SoapySDR::Java::StreamFlags::HAS_TIME, SOAPY_SDR_HAS_TIME);
ENUM_CHECK(SoapySDR::Java::StreamFlags::END_ABRUPT, SOAPY_SDR_END_ABRUPT);
ENUM_CHECK(SoapySDR::Java::StreamFlags::ONE_PACKET, SOAPY_SDR_ONE_PACKET);
ENUM_CHECK(SoapySDR::Java::StreamFlags::MORE_FRAGMENTS, SOAPY_SDR_MORE_FRAGMENTS);
ENUM_CHECK(SoapySDR::Java::StreamFlags::WAIT_TRIGGER, SOAPY_SDR_WAIT_TRIGGER);
