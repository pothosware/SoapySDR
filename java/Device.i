// Copyright (c) 2021 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

%include <std_pair.i>
%include <stdint.i>
%include <typemaps.i>

// Java has no unsigned types, so we need these extra checks to
// catch values outside the end C++ type before they overflow or
// underflow.

%typemap(check) const size_t channel {
    if($input < 0)
    {
        SWIG_JavaException(jenv, SWIG_JavaIllegalArgumentException, "Channel must be >= 0");
        return $null;
    }
}

%typemap(check) const unsigned addr {
    if(($input < 0) || ($input > 0xFFFFFFFF))
    {
        SWIG_JavaException(jenv, SWIG_JavaIllegalArgumentException, "Address must be in the range [0, 0xFFFFFFFF]");
        return $null;
    }
}

%typemap(check) const unsigned value {
    if(($input < 0) || ($input > 0xFFFFFFFF))
    {
        SWIG_JavaException(jenv, SWIG_JavaIllegalArgumentException, "Value must be in the range [0, 0xFFFFFFFF]");
        return $null;
    }
}

%typemap(check) const unsigned mask {
    if(($input < 0) || ($input > 0xFFFFFFFF))
    {
        SWIG_JavaException(jenv, SWIG_JavaIllegalArgumentException, "Mask must be in the range [0, 0xFFFFFFFF]");
        return $null;
    }
}

%typemap(check) const unsigned dir {
    if(($input < 0) || ($input > 0xFFFFFFFF))
    {
        SWIG_JavaException(jenv, SWIG_JavaIllegalArgumentException, "Direction bits must be in the range [0, 0xFFFFFFFF]");
        return $null;
    }
}

%typemap(check) const size_t length {
    if($input < 0)
    {
        SWIG_JavaException(jenv, SWIG_JavaIllegalArgumentException, "Length must be >= 0");
        return $null;
    }
}

%typemap(check) const size_t numBytes {
    if($input < 0)
    {
        SWIG_JavaException(jenv, SWIG_JavaIllegalArgumentException, "Byte count must be >= 0");
        return $null;
    }
}

%typemap(check) const unsigned data {
    if(($input < 0) || ($input > 0xFFFFFFFF))
    {
        SWIG_JavaException(jenv, SWIG_JavaIllegalArgumentException, "Data must be in the range [0, 0xFFFFFFFF]");
        return $null;
    }
}

%typemap(check) const size_t numBits {
    if($input < 0)
    {
        SWIG_JavaException(jenv, SWIG_JavaIllegalArgumentException, "Bit count must be >= 0");
        return $null;
    }
}

%apply double& OUTPUT { double& fullScaleOut };

//
// Wrapper class
//

%ignore SoapySDR::Java::DeviceDeleter;
%nodefaultctor SoapySDR::Java::Device;

%{
#include "DeviceWrapper.hpp"
%}

%rename(DeviceInternal) SoapySDR::Java::Device;
%typemap(javaclassmodifiers) SoapySDR::Java::Device "class"
%include "DeviceWrapper.hpp"

%typemap(javaclassmodifiers) std::vector<SoapySDR::Java::Device> "class"
%template(DeviceInternalList) std::vector<SoapySDR::Java::Device>;
