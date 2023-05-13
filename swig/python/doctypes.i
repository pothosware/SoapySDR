// Copyright (c) 2023 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

//
// C++ types
//

%typemap(doctype) bool "bool" // For some reason translates to "boolean" by default
%typemap(doctype) std::string "str"
%typemap(doctype) std::complex<double> "complex"

%typemap(doctype) std::vector<unsigned> "list[int]"
%typemap(doctype) std::vector<std::string> "list[str]"

//
// SoapySDR types
//

%typemap(doctype) SoapySDR::ArgInfo "SoapySDR.ArgInfo"
%typemap(doctype) SoapySDR::ArgInfoList "list[SoapySDR.ArgInfo]"

%typemap(doctype) SoapySDR::Kwargs "dict"
%typemap(doctype) std::vector<SoapySDR::Kwargs> "list[dict]"

%typemap(doctype) SoapySDR::Range "SoapySDR.Range"
%typemap(doctype) std::vector<SoapySDR::Range> "list[SoapySDR.Range]"

%typemap(doctype) SoapySDR::Device "SoapySDR.Device"
%typemap(doctype) SoapySDR::Device* "SoapySDR.Device"
%typemap(doctype) std::vector<SoapySDR::Device*> "list[SoapySDR.Device]"

%typemap(doctype) SoapySDR::Stream "SoapySDR.Stream"
%typemap(doctype) SoapySDR::Stream* "SoapySDR.Stream"
