// Copyright (c) 2021-2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

%module(directors="1") SoapySDR
%feature("compactdefaultargs");

%include "java_init.i"
%include "soapy_common.i"
%include "enums.swg"

%include <typemaps.i>
%include <std_map.i>
%include <std_vector.i>
%include "std_complex.i" // TODO: typemap to Apache complex

////////////////////////////////////////////////////////////////////////
// Include all major headers to compile against
////////////////////////////////////////////////////////////////////////
%{
#include <SoapySDR/Version.hpp>
#include <SoapySDR/Modules.hpp>
#include <SoapySDR/Device.hpp>
#include <SoapySDR/Errors.hpp>
#include <SoapySDR/Formats.hpp>
#include <SoapySDR/Time.hpp>
#include <SoapySDR/Logger.hpp>

#include "Constants.hpp"
#include "Versions.hpp"
%}

////////////////////////////////////////////////////////////////////////
// http://www.swig.org/Doc2.0/Library.html#Library_stl_exceptions
////////////////////////////////////////////////////////////////////////
%include <exception.i>

// We only expect to throw DirectorExceptions from within
// the Java logger class. This will allow us to propagate the
// Java error back to Java.
%exception
{
    try{$action}
    catch (const Swig::DirectorException &e)
    {SWIG_exception(SWIG_RuntimeError, e.what());}
    SWIG_CATCH_STDEXCEPT
    catch (...)
    {SWIG_exception(SWIG_RuntimeError, "unknown");}
}

////////////////////////////////////////////////////////////////////////
// Config header defines API export
////////////////////////////////////////////////////////////////////////
%include <SoapySDR/Config.h>

////////////////////////////////////////////////////////////////////////
// Commonly used data types
////////////////////////////////////////////////////////////////////////

%typemap(jstype) size_t "int"
%typemap(jstype) const size_t "int"

%typemap(javaclassmodifiers) std::complex<double> "class";

%typemap(jstype) const std::complex<double> & "org.apache.commons.math3.complex.Complex"
%typemap(javain,
    pre="
        ComplexDouble temp$javainput = new ComplexDouble($javainput.getReal(), $javainput.getImaginary());
    ") const std::complex<double> & "$javaclassname.getCPtr(temp$javainput)"

%typemap(jstype) std::complex<double> "org.apache.commons.math3.complex.Complex"
%typemap(javaout) std::complex<double> {
    ComplexDouble tempComplex = new ComplexDouble($jnicall, true);

    return new org.apache.commons.math3.complex.Complex(tempComplex.real(), tempComplex.imag());
}

%include <stdint.i>
%include <std_complex.i>
%include <std_string.i>

////////////////////////////////////////////////////////////////////////
// Build info
////////////////////////////////////////////////////////////////////////
%ignore __getCompileTimeAPIVersion;
%nodefaultctor BuildInfo;
%nodefaultctor BuildInfo::CompileTime;
%nodefaultctor BuildInfo::Runtime;
%include "Versions.hpp"

%include "Constants.hpp"

////////////////////////////////////////////////////////////////////////
// Java needs everything in classes, so add some simple structs for
// global functions
////////////////////////////////////////////////////////////////////////
%nodefaultctor Time;
%{
#include <SoapySDR/Time.hpp>

struct Time
{
    static inline long long ticksToTimeNs(const long long ticks, const double rate)
    {
        return SoapySDR::ticksToTimeNs(ticks, rate);
    }

    static inline long long timeNsToTicks(const long long timeNs, const double rate)
    {
        return SoapySDR::timeNsToTicks(timeNs, rate);
    }
};
%}

struct Time
{
    static long long ticksToTimeNs(const long long ticks, const double rate);
    static long long timeNsToTicks(const long long timeNs, const double rate);
};

/*

%typemap(javaclassmodifiers) TypeConversionInternal "class";
%nodefaultctor TypeConversionInternal;

%{
struct TypeConversionInternal
{
    template <typename T>
    static inline std::string SettingToString(const T &setting)
    {
        return SoapySDR::SettingToString<T>(setting);
    }

    template <typename T>
    static inline T StringToSetting(const std::string &setting)
    {
        return SoapySDR::StringToSetting<T>(setting);
    }

    static inline SoapySDR::Kwargs StringToKwargs(const std::string &args)
    {
        return SoapySDR::KwargsFromString(args);
    }

    static inline std::string KwargsToString(const SoapySDR::Kwargs &kwargs)
    {
        return SoapySDR::KwargsToString(kwargs);
    }
};
%}

struct TypeConversionInternal
{
    template <typename T>
    static std::string SettingToString(const T &setting);

    template <typename T>
    static T StringToSetting(const std::string &setting);

    static SoapySDR::Kwargs StringToKwargs(const std::string &args);

    static std::string KwargsToString(const SoapySDR::Kwargs &kwargs);
};

%template(SByteToString) TypeConversionInternal::SettingToString<int8_t>;
%template(ShortToString) TypeConversionInternal::SettingToString<int16_t>;
%template(IntToString) TypeConversionInternal::SettingToString<int32_t>;
%template(LongToString) TypeConversionInternal::SettingToString<int64_t>;
%template(ByteToString) TypeConversionInternal::SettingToString<uint8_t>;
%template(UShortToString) TypeConversionInternal::SettingToString<uint16_t>;
%template(UIntToString) TypeConversionInternal::SettingToString<uint32_t>;
%template(ULongToString) TypeConversionInternal::SettingToString<uint64_t>;
%template(BoolToString) TypeConversionInternal::SettingToString<bool>;
%template(FloatToString) TypeConversionInternal::SettingToString<float>;
%template(DoubleToString) TypeConversionInternal::SettingToString<double>;

%template(StringToSByte) TypeConversionInternal::StringToSetting<int8_t>;
%template(StringToShort) TypeConversionInternal::StringToSetting<int16_t>;
%template(StringToInt) TypeConversionInternal::StringToSetting<int32_t>;
%template(StringToLong) TypeConversionInternal::StringToSetting<int64_t>;
%template(StringToByte) TypeConversionInternal::StringToSetting<uint8_t>;
%template(StringToUShort) TypeConversionInternal::StringToSetting<uint16_t>;
%template(StringToUInt) TypeConversionInternal::StringToSetting<uint32_t>;
%template(StringToULong) TypeConversionInternal::StringToSetting<uint64_t>;
%template(StringToBool) TypeConversionInternal::StringToSetting<bool>;
%template(StringToFloat) TypeConversionInternal::StringToSetting<float>;
%template(StringToDouble) TypeConversionInternal::StringToSetting<double>;

*/

////////////////////////////////////////////////////////////////////////
// We need all STL declarations before the rename call
////////////////////////////////////////////////////////////////////////

%template(StringList) std::vector<std::string>;
%template(SizeList) std::vector<size_t>;
%template(Kwargs) std::map<std::string, std::string>;
%template(ArgInfoList) std::vector<SoapySDR::ArgInfo>;
%template(KwargsList) std::vector<std::map<std::string, std::string>>;
%template(RangeList) std::vector<SoapySDR::Range>;
%template(ComplexDouble) std::complex<double>;

////////////////////////////////////////////////////////////////////////
// Enforce Java naming conventions
////////////////////////////////////////////////////////////////////////

%rename("%(camelCase)s", %$ispublic) "";

////////////////////////////////////////////////////////////////////////
// With types established, this is the bulk of it
////////////////////////////////////////////////////////////////////////

%include "Types.i"
%include "Stream.i"
%include "Device.i"
%include "Logger.i"
