// Copyright (c) 2021-2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "Stream.hpp"

#include <SoapySDR/Constants.h>
#include <SoapySDR/Device.hpp>

#include <cassert>
#include <stdexcept>
#include <utility>
#include <vector>

// TODO: docs

namespace SoapySDR { namespace Java {

// Construction hidden from SWIG
class RxStream: public Stream
{
public:
    RxStream(void) = delete;
    virtual ~RxStream(void) = default;

    RxStream(
        SoapySDR::Device *device,
        const std::string &format,
        const std::vector<size_t> &channels,
        const SoapySDR::Kwargs &args
    ):
        Stream(device, SOAPY_SDR_RX, format, channels, args)
    {}

    // Note: these functions are technically identical, but the parameter
    // names trigger different typemaps.
    #define RX_STREAM_READ_FCNS(ctype, format) \
        inline SoapySDR::Java::StreamResult readArray( \
            ctype *outputBuffer, \
            const size_t length, \
            const long timeoutUs) \
        { \
            return this->_read1D(outputBuffer, length, timeoutUs, format); \
        } \
        inline SoapySDR::Java::StreamResult readBuffer( \
            ctype *outputNIOBuffer, \
            const size_t length, \
            const long timeoutUs) \
        { \
            return this->_read1D(outputNIOBuffer, length, timeoutUs, format); \
        }

    RX_STREAM_READ_FCNS(int8_t, SOAPY_SDR_CS8)
    RX_STREAM_READ_FCNS(short, SOAPY_SDR_CS16)
    RX_STREAM_READ_FCNS(int, SOAPY_SDR_CS32)
    RX_STREAM_READ_FCNS(float, SOAPY_SDR_CF32)
    RX_STREAM_READ_FCNS(double, SOAPY_SDR_CF64)

private:
    template <typename T>
    SoapySDR::Java::StreamResult _read1D(
        T *buffer,
        const size_t length,
        const long timeoutUs,
        const std::string &requiredFormat)
    {
        assert(buffer);
        assert((length % 2) == 0);

        if(_format != requiredFormat)
            throw std::invalid_argument(std::string("Invalid stream format. Expected ")+requiredFormat);

        int intFlags = 0;

        StreamResult result;
        auto readRet = _device->readStream(
            _stream,
            (void**)&buffer,
            length,
            intFlags,
            result.timeNs,
            timeoutUs);

        if(readRet >= 0) result.numSamples = size_t(readRet);
        else result.errorCode = SoapySDR::Java::ErrorCode(readRet);

        result.flags = SoapySDR::Java::StreamFlags(intFlags);

        return result;
    }
};
}}
