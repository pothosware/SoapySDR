// Copyright (c) 2021 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

%include <std_pair.i>
%include <stdint.i>
%include <typemaps.i>

//
// Wrapper class
//

%apply double& OUTPUT { double& fullScaleOut };

%ignore SoapySDR::Java::DeviceDeleter;
%nodefaultctor SoapySDR::Java::Device;

%typemap(javaclassmodifiers) std::pair<SoapySDR::Java::ErrorCode, SoapySDR::Java::StreamResult> "class";
%template(StreamResultPair) std::pair<SoapySDR::Java::ErrorCode, SoapySDR::Java::StreamResult>;

%{
#include "DeviceWrapper.hpp"
%}

%rename(DeviceInternal) SoapySDR::Java::Device;
%typemap(javaclassmodifiers) SoapySDR::Java::Device "class"
%include "DeviceWrapper.hpp"

%typemap(javaclassmodifiers) std::vector<SoapySDR::Java::Device> "class"
%template(DeviceInternalList) std::vector<SoapySDR::Java::Device>;
