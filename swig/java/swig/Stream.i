// Copyright (c) 2021 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

%typemap(csclassmodifiers) SoapySDR::CSharp::StreamHandle "internal class";
%nodefaultctor SoapySDR::CSharp::StreamHandle;
%ignore SoapySDR::CSharp::StreamHandle::stream;
%ignore SoapySDR::CSharp::StreamHandle::channels;
%ignore SoapySDR::CSharp::StreamHandle::format;

%nodefaultctor SoapySDR::CSharp::StreamFormat;

%{
#include "Stream.hpp"
%}

// Allows bitwise operations
%typemap(csimports) SoapySDR::CSharp::StreamFlags "
using System;"
%typemap(csclassmodifiers) SoapySDR::CSharp::StreamFlags "[Flags]
public enum";

%typemap(csimports) SoapySDR::CSharp::StreamHandle "
using System;
using System.Linq;"

%include "Stream.hpp"