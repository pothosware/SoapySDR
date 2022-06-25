// Copyright (c) 2021-2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "Constants.hpp"

#include <SoapySDR/Constants.h>
#include <SoapySDR/Device.hpp>

#include <cassert>
#include <stdexcept>
#include <utility>
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
            const SoapySDR::Kwargs &args
        ):
            _device(device),
            _stream(_device->setupStream(direction, format, channels, args)),
            _format(format),
            _channels(channels),
            _args(args),
            _active(false)
        {}

        ~Stream(void)
        {
            if(_active) this->deactivate();
        }

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
            assert(device);

            _device->closeStream(_stream);
        }

        inline size_t getMTU(void) const
        {
            assert(device);

            return _device->getStreamMTU(_stream);
        }

        inline bool active(void) const
        {
            assert(device);

            return _active;
        }

        ErrorCode activate(
            const int flags = 0,
            const long long timeNs = 0,
            const size_t numElems = 0)
        {
            assert(device);

            if(_active) throw std::runtime_error("Device already active");

            const auto ret = _device->activateStream(
                _stream,
                flags,
                timeNs,
                numElems);

            if(ret == 0) _active = true;

            return ErrorCode(ret);
        }

        ErrorCode deactivate(
            const int flags = 0,
            const long long timeNs = 0)
        {
            assert(device);

            if(not _active) throw std::runtime_error("Device already inactive");

            const auto ret = _device->deactivateStream(
                _stream,
                flags,
                timeNs);

            if(ret == 0) _active = false;

            return ErrorCode(ret);
        }

    protected:
        SoapySDR::Device *_device{nullptr};
        SoapySDR::Stream *_stream{nullptr};

        const std::string _format;
        const std::vector<size_t> _channels;
        const SoapySDR::Kwargs _args;

        bool _active{false};
    };

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

        StreamResult read(
            void *const *buffs,
            const size_t numElems,
            const long timeoutUs)
        {
            int intFlags = 0;

            StreamResult result;
            auto readRet = _device->readStream(
                _stream,
                buffs,
                numElems,
                intFlags,
                result.timeNs,
                timeoutUs);

            if(readRet >= 0) result.numSamples = size_t(readRet);
            else result.errorCode = SoapySDR::Java::ErrorCode(readRet);

            result.flags = SoapySDR::Java::StreamFlags(intFlags);

            return result;
        }

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

        TX_STREAM_WRITE_FCNS(int8_t, SOAPY_SDR_CS8)
        TX_STREAM_WRITE_FCNS(short, SOAPY_SDR_CS16)
        TX_STREAM_WRITE_FCNS(int, SOAPY_SDR_CS32)
        TX_STREAM_WRITE_FCNS(float, SOAPY_SDR_CF32)
        TX_STREAM_WRITE_FCNS(double, SOAPY_SDR_CF64)

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

ENUM_CHECK(SoapySDR::Java::StreamFlags::END_BURST, SOAPY_SDR_END_BURST);
ENUM_CHECK(SoapySDR::Java::StreamFlags::HAS_TIME, SOAPY_SDR_HAS_TIME);
ENUM_CHECK(SoapySDR::Java::StreamFlags::END_ABRUPT, SOAPY_SDR_END_ABRUPT);
ENUM_CHECK(SoapySDR::Java::StreamFlags::ONE_PACKET, SOAPY_SDR_ONE_PACKET);
ENUM_CHECK(SoapySDR::Java::StreamFlags::MORE_FRAGMENTS, SOAPY_SDR_MORE_FRAGMENTS);
ENUM_CHECK(SoapySDR::Java::StreamFlags::WAIT_TRIGGER, SOAPY_SDR_WAIT_TRIGGER);
