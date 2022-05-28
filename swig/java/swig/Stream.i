// Copyright (c) 2021-2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

%typemap(javaclassmodifiers) SoapySDR::Java::StreamHandle "class";
%nodefaultctor SoapySDR::Java::StreamHandle;
%ignore SoapySDR::Java::StreamHandle::stream;
%ignore SoapySDR::Java::StreamHandle::channels;
%ignore SoapySDR::Java::StreamHandle::format;

// TODO: ignore NativeStreamFormat setters
%nodefaultctor SoapySDR::Java::NativeStreamFormat;
%nodefaultctor SoapySDR::Java::StreamFormat;

// Java doesn't support bitwise operations on enums, so the
// conventional API for this is to take in a set. We'll deal
// with the ugliness to get the conversions ourselves.

%typemap(jstype) const int flags "java.util.EnumSet<StreamFlags>"
%typemap(javain,
    pre="
        StreamFlags[] flagArray;
        $javainput.ToArray(flagArray);

        int jniInput = 0;
        for(StreamFlags flag: flagArray) {
            jniInput = jniInput | flag.swigValue();
        }

        Kwargs temp$javainput = Utility.ToStringList($javainput);
    ") const int flags "jniInput"

%{
#include "Stream.hpp"
%}

%include "Stream.hpp"
