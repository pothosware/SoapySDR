// Copyright (c) 2021-2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "Stream.hpp"

#include <SoapySDR/Constants.h>
#include <SoapySDR/Device.hpp>

#include <jni.h>

#include <cassert>
#include <stdexcept>
#include <utility>
#include <vector>

// TODO: docs

namespace SoapySDR { namespace Java {

// Construction hidden from SWIG
class TxStream: public Stream
{
public:
    TxStream(void) = delete;
    virtual ~TxStream(void) = default;

    TxStream(
        SoapySDR::Device *device,
        const std::string &format,
        const std::vector<size_t> &channels,
        const SoapySDR::Kwargs &args
    ):
        Stream(device, SOAPY_SDR_TX, format, channels, args)
    {}

    // Note: these functions are technically identical, but the parameter
    // names trigger different typemaps.
    #define TX_STREAM_WRITE_FCNS(ctype, format) \
        inline SoapySDR::Java::StreamResult writeArray( \
            const ctype *buffer, \
            const size_t length, \
            const long long timeNs, \
            const long timeoutUs) \
        { \
            return this->_write1D(buffer, length, timeNs, timeoutUs, format); \
        } \
        inline SoapySDR::Java::StreamResult writeBuffer( \
            const ctype *nioBuffer, \
            const size_t length, \
            const long long timeNs, \
            const long timeoutUs) \
        { \
            return this->_write1D(nioBuffer, length, timeNs, timeoutUs, format); \
        }

    TX_STREAM_WRITE_FCNS(jbyte, SOAPY_SDR_CS8)
    TX_STREAM_WRITE_FCNS(jshort, SOAPY_SDR_CS16)
    TX_STREAM_WRITE_FCNS(jint, SOAPY_SDR_CS32)
    TX_STREAM_WRITE_FCNS(jfloat, SOAPY_SDR_CF32)
    TX_STREAM_WRITE_FCNS(jdouble, SOAPY_SDR_CF64)

    StreamResult readStatus(const long timeoutUs)
    {
        int intFlags = 0;

        StreamResult result;
        auto readRet = _device->readStreamStatus(
            _stream,
            result.chanMask,
            intFlags,
            result.timeNs,
            timeoutUs);

        if(readRet < 0) result.errorCode = SoapySDR::Java::ErrorCode(readRet);

        result.flags = SoapySDR::Java::StreamFlags(intFlags);

        return result;
    }
private:
    template <typename T>
    SoapySDR::Java::StreamResult _write1D(
        const T *buffer,
        const size_t length,
        const long long timeNs,
        const long timeoutUs,
        const std::string &requiredFormat)
    {
        assert(buffer);
        assert((length % 2) == 0);

        if(_format != requiredFormat)
            throw std::invalid_argument(std::string("Invalid stream format. Expected ")+requiredFormat);

        int intFlags = 0;

        StreamResult result;
        auto writeRet = _device->writeStream(
            _stream,
            (const void**)&buffer,
            length,
            intFlags,
            timeNs,
            timeoutUs);

        if(writeRet >= 0) result.numSamples = size_t(writeRet);
        else result.errorCode = SoapySDR::Java::ErrorCode(writeRet);

        result.flags = SoapySDR::Java::StreamFlags(intFlags);

        return result;
    }
};
}}
