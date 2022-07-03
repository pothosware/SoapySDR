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

    // Note: the 1D functions are technically identical, but the parameter
    // names trigger different typemaps.
    #define RX_STREAM_READ_FCNS(ctype, arrtype, format) \
        inline SoapySDR::Java::StreamResult readArray( \
            ctype *outputBuffer, \
            const size_t length, \
            const long timeoutUs) \
        { \
            return this->_read1D(outputBuffer, length, timeoutUs, format); \
        } \
        inline SoapySDR::Java::StreamResult writeArray( \
            const SoapySDR::Java::Imported2DArray<ctype, arrtype> &array, \
            const long timeoutUs) \
        { \
            return this->_read2D(array, timeoutUs, format); \
        } \
        inline SoapySDR::Java::StreamResult readBuffer( \
            ctype *outputNIOBuffer, \
            const size_t length, \
            const long timeoutUs) \
        { \
            return this->_read1D(outputNIOBuffer, length, timeoutUs, format); \
        }

    RX_STREAM_READ_FCNS(jbyte, jbyteArray, SOAPY_SDR_CS8)
    RX_STREAM_READ_FCNS(jshort, jshortArray, SOAPY_SDR_CS16)
    RX_STREAM_READ_FCNS(jint, jintArray, SOAPY_SDR_CS32)
    RX_STREAM_READ_FCNS(jfloat, jfloatArray, SOAPY_SDR_CF32)
    RX_STREAM_READ_FCNS(jdouble, jdoubleArray, SOAPY_SDR_CF64)

private:
    template <typename T>
    SoapySDR::Java::StreamResult _read1D(
        T *buffer,
        const size_t length,
        const long timeoutUs,
        const std::string &requiredFormat)
    {
        assert(buffer);

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

    template <typename ElemType, typename Array>
    SoapySDR::Java::StreamResult _read2D(
        const Imported2DArray<ElemType, Array> &array,
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
        auto readRet = _device->readStream(
            _stream,
            pointers.data(),
            array.innerDimension(),
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
