// Copyright (c) 2021-2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "Imported2DArray.hpp"
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

    // Note: the 1D functions are technically identical, but the parameter
    // names trigger different typemaps.
    #define TX_STREAM_WRITE_FCNS(ctype, arrtype, format) \
        inline SoapySDR::Java::StreamResult writeArray( \
            const ctype *buffer, \
            const size_t length, \
            const long long timeNs, \
            const long timeoutUs) \
        { \
            return this->_write1D(buffer, length, timeNs, timeoutUs, format); \
        } \
        inline SoapySDR::Java::StreamResult writeArray( \
            const SoapySDR::Java::Imported2DArray<ctype, arrtype> &array, \
            const long long timeNs, \
            const long timeoutUs) \
        { \
            return this->_write2D(array, timeNs, timeoutUs, format); \
        } \
        inline SoapySDR::Java::StreamResult writeBuffer( \
            const ctype *nioBuffer, \
            const size_t length, \
            const long long timeNs, \
            const long timeoutUs) \
        { \
            return this->_write1D(nioBuffer, length, timeNs, timeoutUs, format); \
        }

    TX_STREAM_WRITE_FCNS(jbyte, jbyteArray, SOAPY_SDR_CS8)
    TX_STREAM_WRITE_FCNS(jshort, jshortArray, SOAPY_SDR_CS16)
    TX_STREAM_WRITE_FCNS(jint, jintArray, SOAPY_SDR_CS32)
    TX_STREAM_WRITE_FCNS(jfloat, jfloatArray, SOAPY_SDR_CF32)
    TX_STREAM_WRITE_FCNS(jdouble, jdoubleArray, SOAPY_SDR_CF64)

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

    template <typename ElemType, typename Array>
    SoapySDR::Java::StreamResult _write2D(
        const Imported2DArray<ElemType, Array> &array,
        const long long timeNs,
        const long timeoutUs,
        const std::string &requiredFormat)
    {
        assert(not array.anyNull());

        if(_format != requiredFormat)
            throw std::invalid_argument(std::string("Invalid stream format. Expected ")+requiredFormat);
        if(array.size() != _channels.size())
            throw std::invalid_argument(std::string("Outer array dimension must match channel count. Expected ")+std::to_string(_channels.size()));

        const auto pointers = array.pointers();
        int intFlags = 0;

        StreamResult result;
        auto writeRet = _device->writeStream(
            _stream,
            pointers.data(),
            array.innerDimension(),
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
