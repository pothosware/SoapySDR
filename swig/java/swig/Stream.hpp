// Copyright (c) 2021-2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "Constants.hpp"

#include <SoapySDR/Constants.h>
#include <SoapySDR/Device.hpp>

#include <jni.h>

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
        None          = 0,
        EndBurst      = (1 << 1),
        HasTime       = (1 << 2),
        EndAbrupt     = (1 << 3),
        OnePacket     = (1 << 4),
        MoreFragments = (1 << 5),
        WaitTrigger   = (1 << 6)
    };

    // TODO: ComplexFloat64, etc
    struct StreamFormat
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

    const std::string StreamFormat::CF64 = SOAPY_SDR_CF64;
    const std::string StreamFormat::CF32 = SOAPY_SDR_CF32;
    const std::string StreamFormat::CS32 = SOAPY_SDR_CS32;
    const std::string StreamFormat::CU32 = SOAPY_SDR_CU32;
    const std::string StreamFormat::CS16 = SOAPY_SDR_CS16;
    const std::string StreamFormat::CU16 = SOAPY_SDR_CU16;
    const std::string StreamFormat::CS12 = SOAPY_SDR_CS12;
    const std::string StreamFormat::CU12 = SOAPY_SDR_CU12;
    const std::string StreamFormat::CS8  = SOAPY_SDR_CS8;
    const std::string StreamFormat::CU8  = SOAPY_SDR_CU8;
    const std::string StreamFormat::CS4  = SOAPY_SDR_CS4;
    const std::string StreamFormat::CU4  = SOAPY_SDR_CU4;
    const std::string StreamFormat::F64  = SOAPY_SDR_F64;
    const std::string StreamFormat::F32  = SOAPY_SDR_F32;
    const std::string StreamFormat::S32  = SOAPY_SDR_S32;
    const std::string StreamFormat::U32  = SOAPY_SDR_U32;
    const std::string StreamFormat::S16  = SOAPY_SDR_S16;
    const std::string StreamFormat::U16  = SOAPY_SDR_U16;
    const std::string StreamFormat::S8   = SOAPY_SDR_S8;
    const std::string StreamFormat::U8   = SOAPY_SDR_U8;

    struct StreamResult
    {
        SoapySDR::Java::ErrorCode errorCode;
        size_t numSamples{0};
        StreamFlags flags{ StreamFlags::None }; // TODO: needs to be exposed as an EnumSet
        long long timeNs{0};
        long timeoutUs{0};
        size_t chanMask{0U};
    };

    class Stream
    {
    public:
        Stream(void) = delete;

        // Hidden from SWIG (TODO: not working)
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

    // All hidden from SWIG, called from manual C++ (TODO: not working)
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
    };

    // All hidden from SWIG (minus readStatus), called from manual C++ (TODO: not working)
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

        StreamResult write(
            const void *const *buffs,
            const size_t numElems,
            const long long timeNs,
            const long timeoutUs)
        {
            int intFlags = 0;

            StreamResult result;
            auto writeRet = _device->writeStream(
                _stream,
                buffs,
                numElems,
                intFlags,
                timeNs,
                timeoutUs);

            if(writeRet >= 0) result.numSamples = size_t(writeRet);
            else result.errorCode = SoapySDR::Java::ErrorCode(writeRet);

            result.flags = SoapySDR::Java::StreamFlags(intFlags);

            return result;
        }

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
    };
}}

ENUM_CHECK(SoapySDR::Java::StreamFlags::EndBurst, SOAPY_SDR_END_BURST);
ENUM_CHECK(SoapySDR::Java::StreamFlags::HasTime, SOAPY_SDR_HAS_TIME);
ENUM_CHECK(SoapySDR::Java::StreamFlags::EndAbrupt, SOAPY_SDR_END_ABRUPT);
ENUM_CHECK(SoapySDR::Java::StreamFlags::OnePacket, SOAPY_SDR_ONE_PACKET);
ENUM_CHECK(SoapySDR::Java::StreamFlags::MoreFragments, SOAPY_SDR_MORE_FRAGMENTS);
ENUM_CHECK(SoapySDR::Java::StreamFlags::WaitTrigger, SOAPY_SDR_WAIT_TRIGGER);

// Copied from SWIG-generated output
#define SWIG_JAVA_CATCH_JLONG \
    catch (const Swig::DirectorException &e) \
    { \
      { \
        SWIG_JavaException(jenv, SWIG_RuntimeError, e.what()); return 0;  \
      }; \
    } \
    catch (std::invalid_argument& e) { \
      { \
        SWIG_JavaException(jenv, SWIG_ValueError, e.what()); return 0;  \
      }; \
    } catch (std::domain_error& e) { \
      { \
        SWIG_JavaException(jenv, SWIG_ValueError, e.what()); return 0;  \
      }; \
    } catch (std::overflow_error& e) { \
      { \
        SWIG_JavaException(jenv, SWIG_OverflowError, e.what()); return 0;  \
      }; \
    } catch (std::out_of_range& e) { \
      { \
        SWIG_JavaException(jenv, SWIG_IndexError, e.what()); return 0;  \
      }; \
    } catch (std::length_error& e) { \
      { \
        SWIG_JavaException(jenv, SWIG_IndexError, e.what()); return 0;  \
      }; \
    } catch (std::runtime_error& e) { \
      { \
        SWIG_JavaException(jenv, SWIG_RuntimeError, e.what()); return 0;  \
      }; \
    } catch (std::bad_cast& e) { \
      { \
        SWIG_JavaException(jenv, SWIG_TypeError, e.what()); return 0;  \
      }; \
    } catch (std::exception& e) { \
      { \
        SWIG_JavaException(jenv, SWIG_SystemError, e.what()); return 0;  \
      }; \
    } \
    catch (...) \
    { \
      { \
        SWIG_JavaException(jenv, SWIG_RuntimeError, "unknown"); return 0;  \
      }; \
    }
