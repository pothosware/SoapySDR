// Copyright (c) 2021-2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

%{
#include "Utility.hpp"

#include <SoapySDR/Types.hpp>
%}

%include <attribute.i>
%include <std_map.i>
%include <std_string.i>

//
// ArgInfo
//

%typemap(csclassmodifiers) SoapySDR::ArgInfo "
/// <summary>Representation of a key/value argument.</summary>
public class";

%typemap(csclassmodifiers) SoapySDR::ArgInfo::Type "
/// <summary>The data type of the argument.</summary>
public enum";

// Needed because the "type" variable would be renamed to Type, which
// conflicts with the enum type name
%rename(ArgType) SoapySDR::ArgInfo::type;

%rename(Bool) SoapySDR::ArgInfo::BOOL;
%rename(Int) SoapySDR::ArgInfo::INT;
%rename(Float) SoapySDR::ArgInfo::FLOAT;
%rename(String) SoapySDR::ArgInfo::STRING;

%csattributes SoapySDR::ArgInfo::BOOL "/// <summary>Boolean argument</summary>";
%csattributes SoapySDR::ArgInfo::INT "/// <summary>Integral argument</summary>";
%csattributes SoapySDR::ArgInfo::FLOAT "/// <summary>Floating-point argument</summary>";
%csattributes SoapySDR::ArgInfo::STRING "/// <summary>String argument</summary>";

%csattributes SoapySDR::ArgInfo::key "/// <summary>The key used to identify the argument.</summary>";
%csattributes SoapySDR::ArgInfo::value "/// <summary>The default value of the argument when not specified.</summary>";
%csattributes SoapySDR::ArgInfo::name "/// <summary>The displayable name of the argument.</summary>";
%csattributes SoapySDR::ArgInfo::description "/// <summary>A brief description about the argument.</summary>";
%csattributes SoapySDR::ArgInfo::units "/// <summary>The units of the argument: dB, Hz, etc.</summary>";
%csattributes SoapySDR::ArgInfo::type "/// <summary>The data type of the argument.</summary>";
%csattributes SoapySDR::ArgInfo::range "
/// <summary>
/// The range of possible numeric values.
///
/// When specified, the argument should be restricted to this range.
/// The range is only applicable to numeric argument types.
/// </summary>
";
%csattributes SoapySDR::ArgInfo::options "
/// <summary>
/// A discrete list of possible values.
///
/// When specified, the argument should be restricted to this options set.
/// </summary>
";
%csattributes SoapySDR::ArgInfo::optionNames "
/// <summary>
/// A discrete list of displayable names for the enumerated options.
///
/// When not specified, the option value itself can be used as a display name.
/// </summary>
";

%typemap(cscode) SoapySDR::ArgInfo
%{
    //
    // Object overrides
    //

    public override string ToString()
    {
        return string.Format("{0} ({1})", Name, ArgType);
    }

    public override bool Equals(object obj)
    {
        var objAsArgInfo = obj as ArgInfo;
        if(objAsArgInfo != null) return Key.Equals(objAsArgInfo.Key) && ArgType.Equals(objAsArgInfo.ArgType);
        else                     return false;
    }

    public override int GetHashCode() => HashCodeBuilder.Create()
        .AddValue(GetType())
        .AddValue(Key)
        .AddValue(ArgType);
%}

//
// Kwargs
//

// Accept any Dictionary and convert it internally
%typemap(cstype) const std::map<std::string, std::string> & "System.Collections.Generic.IDictionary<string, string>"
%typemap(csin,
    pre="
        Kwargs temp$csinput = Utility.ToKwargs($csinput);
    ") const std::map<std::string, std::string> & "$csclassname.getCPtr(temp$csinput)"

//
// Range
//

%attribute(SoapySDR::Range, double, Minimum, minimum);
%attribute(SoapySDR::Range, double, Maximum, maximum);
%attribute(SoapySDR::Range, double, Step, step);

%typemap(cscode) SoapySDR::Range
%{
    //
    // Object overrides
    //

    public override string ToString()
    {
        return string.Format("Range: min={0}, max={1}, step={2}", Minimum, Maximum, Step);
    }

    public override bool Equals(object obj)
    {
        var objAsRange = obj as Range;
        if(objAsRange != null) return Minimum.Equals(objAsRange.Minimum) && Maximum.Equals(objAsRange.Maximum) && Step.Equals(objAsRange.Step);
        else                   return false;
    }

    public override int GetHashCode() => HashCodeBuilder.Create()
        .AddValue(GetType())
        .AddValue(Minimum)
        .AddValue(Maximum)
        .AddValue(Step);
%}

//
// Finally, include the header
//

%feature("compactdefaultargs", "0") Range;

%ignore SoapySDR::KwargsFromString;
%ignore SoapySDR::KwargsToString;
%ignore SoapySDR::SettingToString;
%ignore SoapySDR::StringToSetting;
%ignore SoapySDR::Detail::SettingToString;
%ignore SoapySDR::Detail::StringToSetting;
%include <SoapySDR/Types.hpp>