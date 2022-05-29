// Copyright (c) 2021-2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "Constants.hpp"

#include <SoapySDR/Constants.h>
#include <SoapySDR/Device.hpp>

#include <jni.h>

#include <utility>

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

    struct StreamHandle
    {
        SoapySDR::Stream* stream{ nullptr };
        std::string format;
        std::vector<size_t> channels;
    };

    struct StreamResult
    {
        SoapySDR::Java::ErrorCode errorCode;
        size_t numSamples{0};
        StreamFlags flags{ StreamFlags::None }; // TODO: needs to be exposed as an EnumSet
        long long timeNs{0};
        long timeoutUs{0};
        size_t chanMask{0U};
    };
}}

ENUM_CHECK(SoapySDR::Java::StreamFlags::EndBurst, SOAPY_SDR_END_BURST);
ENUM_CHECK(SoapySDR::Java::StreamFlags::HasTime, SOAPY_SDR_HAS_TIME);
ENUM_CHECK(SoapySDR::Java::StreamFlags::EndAbrupt, SOAPY_SDR_END_ABRUPT);
ENUM_CHECK(SoapySDR::Java::StreamFlags::OnePacket, SOAPY_SDR_ONE_PACKET);
ENUM_CHECK(SoapySDR::Java::StreamFlags::MoreFragments, SOAPY_SDR_MORE_FRAGMENTS);
ENUM_CHECK(SoapySDR::Java::StreamFlags::WaitTrigger, SOAPY_SDR_WAIT_TRIGGER);

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
