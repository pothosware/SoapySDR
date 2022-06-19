// Copyright (c) 2021-2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

%{
#include <SoapySDR/Types.hpp>
%}

%include <attribute.i>
%include <std_map.i>
%include <std_string.i>

//
// ArgInfo
//

// TODO: ignore setters

%typemap(javacode) SoapySDR::ArgInfo
%{
    @Override
    public String toString() {
        return String.format("%s (%s)", getName(), getType());
    }

    @Override
    public boolean equals(Object o) {
        return (hashCode() == o.hashCode());
    }

    @Override
    public int hashCode() {
        return new org.apache.commons.lang3.builder.HashCodeBuilder(1351, 4063)
            .append(getClass().getName())
            .append(getName())
            .append(getType())
            .toHashCode();
    }
%}

//
// Kwargs
//

// TODO: implement toKwargs
// Accept any Map and convert it internally
%typemap(jstype) const std::map<std::string, std::string> & "java.util.AbstractMap<String, String>"
%typemap(javain,
    pre="
        Kwargs temp$javainput = Utility.ToKwargs($javainput);
    ") const std::map<std::string, std::string> & "$javaclassname.getCPtr(temp$javainput)"

//
// Range
//

// TODO: docs
%attribute(SoapySDR::Range, double, Minimum, minimum);
%attribute(SoapySDR::Range, double, Maximum, maximum);
%attribute(SoapySDR::Range, double, Step, step);

%typemap(javacode) SoapySDR::Range
%{
    @Override
    public String toString() {
        return String.format("Range: min=%f, max=%f, step=%f", getMinimum(), getMaximum(), getStep());
    }

    @Override
    public boolean equals(Object o) {
        return (hashCode() == o.hashCode());
    }

    @Override
    public int hashCode() {
        return new org.apache.commons.lang3.builder.HashCodeBuilder(1351, 4063)
            .append(getClass().getName())
            .append(getMinimum())
            .append(getMaximum())
            .append(getStep())
            .toHashCode();
    }
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
