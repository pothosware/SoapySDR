// Copyright (c) 2014-2014 Josh Blum
// SPDX-License-Identifier: BSL-1.0

%module SoapySDR

////////////////////////////////////////////////////////////////////////
// Include all major headers to compile against
////////////////////////////////////////////////////////////////////////
%{
#include <SoapySDR/Version.hpp>
#include <SoapySDR/Modules.hpp>
#include <SoapySDR/Device.hpp>
%}

////////////////////////////////////////////////////////////////////////
// http://www.swig.org/Doc2.0/Library.html#Library_stl_exceptions
////////////////////////////////////////////////////////////////////////
%include <exception.i>

%exception
{
    try{$action}
    catch (const std::exception& e)
    {SWIG_exception(SWIG_RuntimeError, e.what());}
}

////////////////////////////////////////////////////////////////////////
// Config header defines API export
////////////////////////////////////////////////////////////////////////
%include <SoapySDR/Config.h>

////////////////////////////////////////////////////////////////////////
// Commonly used data types
////////////////////////////////////////////////////////////////////////
%include <std_string.i>
%include <std_vector.i>
%include <std_map.i>
%include <SoapySDR/Types.hpp>

%template(SoapySDRKwargs) std::map<std::string, std::string>;
%template(SoapySDRKwargsList) std::vector<SoapySDR::Kwargs>;
%template(SoapySDRStringList) std::vector<std::string>;
%template(SoapySDRRangeList) std::vector<SoapySDR::Range>;

////////////////////////////////////////////////////////////////////////
// Utility functions
////////////////////////////////////////////////////////////////////////
%include <SoapySDR/Constants.h>
%include <SoapySDR/Version.hpp>
%include <SoapySDR/Modules.hpp>

////////////////////////////////////////////////////////////////////////
// Device object
////////////////////////////////////////////////////////////////////////
%nodefaultctor SoapySDR::Device;
%include <SoapySDR/Device.hpp>

//make device a constructable class
%insert("python")
%{
class Device(Device):
    def __new__(cls, *args, **kwargs):
        return cls.make(*args, **kwargs)
%}

//call unmake from custom deleter
%extend SoapySDR::Device
{
    void __del__(void)
    {
        SoapySDR::Device::unmake(self);
    }
};
