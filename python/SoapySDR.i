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
%include <std_complex.i>
%include <std_string.i>
%include <std_vector.i>
%include <std_map.i>
%include <SoapySDR/Types.hpp>

%template(SoapySDRKwargs) std::map<std::string, std::string>;
%template(SoapySDRKwargsList) std::vector<SoapySDR::Kwargs>;
%template(SoapySDRStringList) std::vector<std::string>;
%template(SoapySDRRangeList) std::vector<SoapySDR::Range>;
%template(SoapySDRSizeList) std::vector<size_t>;

%extend std::map<std::string, std::string>
{
    %insert("python")
    %{
        def __str__(self):
            out = list()
            for k, v in self.iteritems():
                out.append("%s=%s"%(k, v))
            return '{'+(', '.join(out))+'}'
    %}
};

%extend SoapySDR::Range
{
    %insert("python")
    %{
        def __str__(self):
            return "%s, %s"%(self.minimum(), self.maximum())
    %}
};

////////////////////////////////////////////////////////////////////////
// Stream result class
// Helps us deal with stream calls that return by reference
////////////////////////////////////////////////////////////////////////
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

%extend StreamResult
{
    %insert("python")
    %{
        def __str__(self):
            return "ret=%s, flags=%s, timeNs=%s"%(self.ret, self.flags, self.timeNs)
    %}
};

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

__all__ = list()
for key in globals().keys():
    if key.startswith('SOAPY_SDR_'):
        __all__.append(key)
%}

//make device a constructable class
%insert("python")
%{
class Device(Device):
    def __new__(cls, *args, **kwargs):
        with device_factory_lock:
            return cls.make(*args, **kwargs)

def extractBuffPointer(buff):
    if hasattr(buff, '__array_interface__'): return buff.__array_interface__['data'][0]
    if hasattr(buff, '__long__'): return long(buff)
    raise Exception("Unrecognized data format: " + str(type(buff)))
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

        def __str__(self):
            return "%s:%s"%(self.getDriverKey(), self.getHardwareKey())

        def readStream(self, stream, buffs, numElems, flags = 0, timeoutUs = 100000):
            ptrs = map(extractBuffPointer, buffs)
            return self.readStream__(stream, ptrs, numElems, flags, timeoutUs)

        def writeStream(self, stream, buffs, numElems, flags = 0, timeNs = 0, timeoutUs = 100000):
            ptrs = map(extractBuffPointer, buffs)
            return self.writeStream__(stream, ptrs, numElems, flags, timeNs, timeoutUs)
    %}
};
