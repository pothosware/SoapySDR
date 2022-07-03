// Copyright (c) 2021-2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

%module(directors="1") SoapySDR
%feature("compactdefaultargs");

%include "soapy_common.i"
%include "enums.swg"

%include <typemaps.i>
%include <std_map.i>
%include <std_vector.i>
%include "std_complex.i"

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
#include "Env.hpp"
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

%typemap(in) const size_t, const unsigned, const unsigned long, const unsigned long long %{
    if($input < 0) {
        SWIG_JavaException(jenv, SWIG_OverflowError, "Number must be non-negative");
        return $null;
    } else {
        $1 = ($type)$input;
    }
%}

// TODO: make SizeList private after figuring out the Stream getter
%typemap(jstype) const std::vector<size_t> &channels "int[]"
%typemap(javain,
    pre="
        var temp$javainput = new SizeList();
        for(int chan: $javainput)
            temp$javainput.add((long)chan);
    ",
    pgcppname="temp$javainput") const std::vector<size_t> &channels "$javaclassname.getCPtr(temp$javainput)"

%typemap(javaclassmodifiers) std::complex<double> "class";

%typemap(jstype) const std::complex<double> & "org.apache.commons.math3.complex.Complex"
%typemap(javain,
    pre="
        ComplexDouble temp$javainput = new ComplexDouble($javainput.getReal(), $javainput.getImaginary());
    ",
    pgcppname="temp$javainput") const std::complex<double> & "$javaclassname.getCPtr(temp$javainput)"

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

%typemap(javaclassmodifiers) TypeConversionInternal "class";
%nodefaultctor TypeConversionInternal;

%{
struct TypeConversionInternal
{
    template <typename T>
    static inline std::string settingToString(const T &setting)
    {
        return SoapySDR::SettingToString<T>(setting);
    }

    template <typename T>
    static inline T stringToSetting(const std::string &setting)
    {
        return SoapySDR::StringToSetting<T>(setting);
    }

    static inline SoapySDR::Kwargs stringToKwargs(const std::string &args)
    {
        return SoapySDR::KwargsFromString(args);
    }

    static inline std::string kwargsToString(const SoapySDR::Kwargs &kwargs)
    {
        return SoapySDR::KwargsToString(kwargs);
    }
};
%}

struct TypeConversionInternal
{
    template <typename T>
    static std::string settingToString(const T &setting);

    template <typename T>
    static T stringToSetting(const std::string &setting);

    static SoapySDR::Kwargs stringToKwargs(const std::string &args);

    static std::string kwargsToString(const SoapySDR::Kwargs &kwargs);
};

%template(byteToString) TypeConversionInternal::settingToString<int8_t>;
%template(shortToString) TypeConversionInternal::settingToString<int16_t>;
%template(intToString) TypeConversionInternal::settingToString<int32_t>;
%template(longToString) TypeConversionInternal::settingToString<int64_t>;
%template(booleanToString) TypeConversionInternal::settingToString<bool>;
%template(floatToString) TypeConversionInternal::settingToString<float>;
%template(doubleToString) TypeConversionInternal::settingToString<double>;

%template(stringToByte) TypeConversionInternal::stringToSetting<int8_t>;
%template(stringToShort) TypeConversionInternal::stringToSetting<int16_t>;
%template(stringToInt) TypeConversionInternal::stringToSetting<int32_t>;
%template(stringToLong) TypeConversionInternal::stringToSetting<int64_t>;
%template(stringToBoolean) TypeConversionInternal::stringToSetting<bool>;
%template(stringToFloat) TypeConversionInternal::stringToSetting<float>;
%template(stringToDouble) TypeConversionInternal::stringToSetting<double>;

////////////////////////////////////////////////////////////////////////
// We need all STL declarations before the rename call
////////////////////////////////////////////////////////////////////////

%template(StringList) std::vector<std::string>;
%template(UnsignedList) std::vector<unsigned>;
%template(SizeList) std::vector<size_t>;
%template(Kwargs) std::map<std::string, std::string>;
%template(ArgInfoList) std::vector<SoapySDR::ArgInfo>;
%template(KwargsList) std::vector<std::map<std::string, std::string>>;
%template(RangeList) std::vector<SoapySDR::Range>;
%template(ComplexDouble) std::complex<double>;

////////////////////////////////////////////////////////////////////////
// Environment variable hackery
////////////////////////////////////////////////////////////////////////
%nodefaultctor SoapySDR::Java::Env;
%typemap(javaclassmodifiers) SoapySDR::Java::Env "class";
%include "Env.hpp"

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
