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

%{
#include "Stream.hpp"
%}

// Allows bitwise operations
%typemap(javaimports) SoapySDR::Java::StreamFlags "
using System;"
%typemap(javaclassmodifiers) SoapySDR::Java::StreamFlags "[Flags]
public enum";

%typemap(javaimports) SoapySDR::Java::StreamHandle "
using System;
using System.Linq;"

%include "Stream.hpp"
