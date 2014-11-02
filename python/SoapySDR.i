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
%template(SoapySDRSizeList) std::vector<size_t>;

%inline %{
    struct StreamResult
    {
        StreamResult(void):
            ret(0), flags(0), timeNs(0){}
        int ret;
        int flags;
        long long timeNs;
    };
%}

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

//global factory lock support
%pythoncode %{
import threading
device_factory_lock = threading.Lock()
%}

//make device a constructable class
%insert("python")
%{
class Device(Device):
    def __new__(cls, *args, **kwargs):
        with device_factory_lock:
            return cls.make(*args, **kwargs)
%}

%extend SoapySDR::Device
{
    StreamResult readStream__(SoapySDR::Stream *stream, const std::vector<size_t> buffs, const size_t numElems, const int flags, const long timeoutUs)
    {
        StreamResult sr;
        sr.flags = flags;
        std::vector<void *> ptrs(buffs.size());
        for (size_t i = 0; i < buffs.size(); i++) ptrs[i] = (void *)buffs[i];
        sr.ret = self->readStream(stream, (&ptrs[0]), numElems, sr.flags, sr.timeNs, timeoutUs);
        return sr;
    }

    StreamResult writeStream__(SoapySDR::Stream *stream, const std::vector<size_t> buffs, const size_t numElems, const int flags, const long long timeNs, const long timeoutUs)
    {
        StreamResult sr;
        sr.flags = flags;
        std::vector<const void *> ptrs(buffs.size());
        for (size_t i = 0; i < buffs.size(); i++) ptrs[i] = (const void *)buffs[i];
        sr.ret = self->writeStream(stream, (&ptrs[0]), numElems, sr.flags, timeNs, timeoutUs);
        return sr;
    }

    %insert("python")
    %{
        #call unmake from custom deleter
        def __del__(self):
            with device_factory_lock:
                Device.unmake(self)

        def readStream(self, stream, buffs, numElems, flags = 0, timeoutUs = 100000):
            buffs = map(lambda x: x.__array_interface__['data'][0])
            return self.readStream__(stream, buffs, numElems, flags, timeoutUs)

        def writeStream(self, stream, buffs, numElems, flags = 0, timeNs = 0, timeoutUs = 100000):
            buffs = map(lambda x: x.__array_interface__['data'][0])
            return self.writeStream__(stream, buffs, numElems, flags, timeNs, timeoutUs)
    %}
};
