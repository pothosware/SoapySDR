// Copyright (c) 2021 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

%module(directors="1") SoapySDR

%include <typemaps.i>
%include <std_vector.i>

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

#include "JavaWrappers.hpp"
%}

////////////////////////////////////////////////////////////////////////
// http://www.swig.org/Doc2.0/Library.html#Library_stl_exceptions
////////////////////////////////////////////////////////////////////////
%include <exception.i>

// We only expect to throw DirectorExceptions from within the Java
// logger class's calls.  Catching them permits us to
// propagate exceptions thrown in the C# log handler callback back to
// C#.
%exception
{
    try{$action}
    catch(const Swig::DirectorException &e) {
        SWIG_JavaException(jenv, SWIG_RuntimeError, e.what());
        return $null;
    }
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
%include <std_complex.i>
%include <stdint.i>
%include <std_string.i>
%include <std_vector.i>
%include <std_map.i>

%template(Complex) std::complex<double>;

%typemap(javaclassmodifiers) SoapySDR::ArgInfo "class"
%rename(ArgInfoInternal) SoapySDR::ArgInfo;

%ignore SoapySDR::Detail::StringToSetting;
%include <SoapySDR/Types.hpp>

#ifdef SIZE_T_IS_UNSIGNED_INT
%typemap(javaclassmodifiers) std::vector<uint32_t> "class"
%template(SizeList) std::vector<uint32_t>;
#else
%typemap(javaclassmodifiers) std::vector<unsigned long> "class"
%template(SizeList) std::vector<unsigned long>;
#endif

// Hide SWIG-generated STL types, they're ugly and half-done

%typemap(javaclassmodifiers) SoapySDR::Kwargs "class"
%template(Kwargs) std::map<std::string, std::string>;

%typemap(javaclassmodifiers) std::vector<SoapySDR::Kwargs> "class"
%template(KwargsList) std::vector<SoapySDR::Kwargs>;

%typemap(javaclassmodifiers) std::vector<SoapySDR::ArgInfo> "class"
%template(ArgInfoInternalList) std::vector<SoapySDR::ArgInfo>;

%typemap(javaclassmodifiers) std::vector<std::string> "class"
%template(StringList) std::vector<std::string>;

%typemap(javaclassmodifiers) std::vector<SoapySDR::Range> "class"
%template(RangeList) std::vector<SoapySDR::Range>;

%typemap(javaclassmodifiers) std::vector<double> "class"
%template(DoubleList) std::vector<double>;

////////////////////////////////////////////////////////////////////////
// Include extensions before types that will use them
////////////////////////////////////////////////////////////////////////
%nodefaultctor SoapySDR::Java::BuildInfo;
%nodefaultctor SoapySDR::Java::StreamHandle;

%include "Stream.i"

%include "JavaWrappers.hpp"

%typemap(javaclassmodifiers) SoapySDR::Java::SettingConversion "class"

%template(LongToString) SoapySDR::Java::SettingConversion::SettingToString<int64_t>;
%template(ULongToString) SoapySDR::Java::SettingConversion::SettingToString<unsigned long>;
%template(BoolToString) SoapySDR::Java::SettingConversion::SettingToString<bool>;
%template(DoubleToString) SoapySDR::Java::SettingConversion::SettingToString<double>;

%template(StringToLong) SoapySDR::Java::SettingConversion::StringToSetting<int64_t>;
%template(StringToULong) SoapySDR::Java::SettingConversion::StringToSetting<unsigned long>;
%template(StringToBool) SoapySDR::Java::SettingConversion::StringToSetting<bool>;
%template(StringToDouble) SoapySDR::Java::SettingConversion::StringToSetting<double>;

%include "Device.i"
%include "Logger.i"
