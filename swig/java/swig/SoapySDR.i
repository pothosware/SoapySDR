// Copyright (c) 2021 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

%module(directors="1") SoapySDR
%feature("compactdefaultargs");

%include "soapy_common.i"
%include "Docs.i"

%include <typemaps.i>
%include <std_map.i>
%include "soapy_std_vector.i"

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
#include "Utility.hpp"
#include "Versions.hpp"
%}

%ignore copyVector;
%ignore toSizeTVector;
%ignore reinterpretCastVector;
%ignore detail::copyVector;
%include "Utility.hpp"

////////////////////////////////////////////////////////////////////////
// http://www.swig.org/Doc2.0/Library.html#Library_stl_exceptions
////////////////////////////////////////////////////////////////////////
%include <exception.i>

// We only expect to throw DirectorExceptions from within
// the C# logger class. This will allow us to propagate the
// C# error back to C#.
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
%include <stdint.i>
%include <std_complex.i>
%include <std_string.i>

//
// size_t
//

// Without this, size_t always typedefs to unsigned int, but we need it to
// match the size of void* for our buffer functions.
#ifdef SIZE_T_IS_UNSIGNED_INT
typedef unsigned int size_t;
#else
typedef unsigned long long size_t;
#endif

// Keep size_t stuff internal due to workaround
%typemap(csclassmodifiers) std::vector<size_t> "internal class";
%template(SizeListInternal) std::vector<size_t>;

//
// Do different things for different std::vector<size_t> parameters
//

// Buffer functions do their own thing
%typemap(cstype) const std::vector<size_t> &channels "uint[]" // SetupStream
%typemap(cstype) const std::vector<size_t> &value "uint[]"    // WriteRegisters
%typemap(csin,
    pre="
        var temp$csinput = new SizeListInternal();
        foreach(var x in $csinput) temp$csinput.Add(x);
    ") const std::vector<size_t> & "$csclassname.getCPtr(temp$csinput)"

%typemap(cstype) std::vector<size_t> "uint[]"
%typemap(csout, excode=SWIGEXCODE) std::vector<size_t> {
    var sizeListPtr = $imcall;$excode

    return new SizeListInternal(sizeListPtr, false).Select(x => (uint)x).ToArray();
}

////////////////////////////////////////////////////////////////////////
// Build info
////////////////////////////////////////////////////////////////////////
%ignore __getCompileTimeAPIVersion;
%nodefaultctor BuildInfo;
%nodefaultctor BuildInfo::CompileTime;
%nodefaultctor BuildInfo::Runtime;
%typemap(csclassmodifiers) BuildInfo "public partial class";
%include "Versions.hpp"

%include "Constants.hpp"

////////////////////////////////////////////////////////////////////////
// C# needs everything in classes, so add some simple structs for
// global functions
////////////////////////////////////////////////////////////////////////
%nodefaultctor Time;
%{
#include <SoapySDR/Time.hpp>

struct Time
{
    static __forceinline long long TicksToTimeNs(const long long ticks, const double rate)
    {
        return SoapySDR::ticksToTimeNs(ticks, rate);
    }

    static __forceinline long long TimeNsToTicks(const long long timeNs, const double rate)
    {
        return SoapySDR::timeNsToTicks(timeNs, rate);
    }
};
%}

struct Time
{
    static long long TicksToTimeNs(const long long ticks, const double rate);
    static long long TimeNsToTicks(const long long timeNs, const double rate);
};

%typemap(csclassmodifiers) TypeConversionInternal "internal class";
%nodefaultctor TypeConversionInternal;

%{
struct TypeConversionInternal
{
    template <typename T>
    static __forceinline std::string SettingToString(const T &setting)
    {
        return SoapySDR::SettingToString<T>(setting);
    }

    template <typename T>
    static __forceinline T StringToSetting(const std::string &setting)
    {
        return SoapySDR::StringToSetting<T>(setting);
    }

    static __forceinline SoapySDR::Kwargs StringToKwargs(const std::string &args)
    {
        return SoapySDR::KwargsFromString(args);
    }

    static __forceinline std::string KwargsToString(const SoapySDR::Kwargs &kwargs)
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

////////////////////////////////////////////////////////////////////////
// We need all STL declarations before the rename call
////////////////////////////////////////////////////////////////////////

%template(StringList) std::vector<std::string>;
%template(Kwargs) std::map<std::string, std::string>;
%template(ArgInfoList) std::vector<SoapySDR::ArgInfo>;
%template(KwargsList) std::vector<std::map<std::string, std::string>>;
%template(RangeList) std::vector<SoapySDR::Range>;

////////////////////////////////////////////////////////////////////////
// Enforce C# naming conventions
////////////////////////////////////////////////////////////////////////

%rename("%(camelcase)s", %$ispublic) "";

////////////////////////////////////////////////////////////////////////
// With types established, this is the bulk of it
////////////////////////////////////////////////////////////////////////

%include "Types.i"
%include "Stream.i"
%include "Device.i"
%include "Logger.i"