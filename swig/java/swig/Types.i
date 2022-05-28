// Copyright (c) 2021 Nicholas Corgan
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

// Needed because the "type" variable would be renamed to Type, which
// conflicts with the enum type name
%rename(ArgType) SoapySDR::ArgInfo::type;

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

    public override int GetHashCode() => GetType().GetHashCode() ^ (Key.GetHashCode() << 1) ^ (ArgType.GetHashCode() << 2);
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

// TODO: docs
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

    public override int GetHashCode() => GetType().GetHashCode() ^ (Minimum.GetHashCode() << 1) ^ (Maximum.GetHashCode() << 2) ^ (Step.GetHashCode() << 3);
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