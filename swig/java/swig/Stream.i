// Copyright (c) 2021 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

%typemap(csclassmodifiers) SoapySDR::Java::StreamHandle "internal class";
%nodefaultctor SoapySDR::Java::StreamHandle;
%ignore SoapySDR::Java::StreamHandle::stream;
%ignore SoapySDR::Java::StreamHandle::channels;
%ignore SoapySDR::Java::StreamHandle::format;

%nodefaultctor SoapySDR::Java::StreamFormat;

%{
#include "Stream.hpp"
%}

// Allows bitwise operations
%typemap(csimports) SoapySDR::Java::StreamFlags "
using System;"
%typemap(csclassmodifiers) SoapySDR::Java::StreamFlags "[Flags]
public enum";

%typemap(csimports) SoapySDR::Java::StreamHandle "
using System;
using System.Linq;"

%include "Stream.hpp"