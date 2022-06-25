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

    class Stream
    {
    public:
        Stream(void) = delete;

        // Hidden from SWIG
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

    // All hidden from SWIG, called from manual C++
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

    // All hidden from SWIG (minus readStatus)
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

ENUM_CHECK(SoapySDR::Java::StreamFlags::END_BURST, SOAPY_SDR_END_BURST);
ENUM_CHECK(SoapySDR::Java::StreamFlags::HAS_TIME, SOAPY_SDR_HAS_TIME);
ENUM_CHECK(SoapySDR::Java::StreamFlags::END_ABRUPT, SOAPY_SDR_END_ABRUPT);
ENUM_CHECK(SoapySDR::Java::StreamFlags::ONE_PACKET, SOAPY_SDR_ONE_PACKET);
ENUM_CHECK(SoapySDR::Java::StreamFlags::MORE_FRAGMENTS, SOAPY_SDR_MORE_FRAGMENTS);
ENUM_CHECK(SoapySDR::Java::StreamFlags::WAIT_TRIGGER, SOAPY_SDR_WAIT_TRIGGER);

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
