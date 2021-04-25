// Copyright (c) 2014-2019 Josh Blum
// Copyright (c) 2016-2016 Bastille Networks
// Copyright (c)      2021 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

%define DOCSTRING
"SoapySDR API.

SoapySDR is an open-source generalized API and runtime library for interfacing
with Software Defined Radio devices. With SoapySDR, you can instantiate,
configure, and stream with an SDR device in a variety of environments.
Refer to https://github.com/pothosware/SoapySDR/wiki

This Python interface closely maps to the C/C++ one.
See https://pothosware.github.io/SoapySDR/doxygen/latest/index.html for details.
"
%enddef

%module(directors="1", docstring=DOCSTRING) SoapySDR

////////////////////////////////////////////////////////////////////////
// python3.8 and up need to have the dll search path set
// https://docs.python.org/3/whatsnew/3.8.html#bpo-36085-whatsnew
////////////////////////////////////////////////////////////////////////
%pythonbegin %{
import os

if os.name == 'nt' and hasattr(os, 'add_dll_directory'):
    root_dir = __file__
    for i in range(5): #limit search depth
        root_dir = os.path.dirname(root_dir)
        bin_dir = os.path.join(root_dir, 'bin')
        if os.path.exists(bin_dir):
            try: os.add_dll_directory(bin_dir)
            except Exception as ex:
                print('add_dll_directory(%s): %s'%(bin_dir, ex))
            break
%}

////////////////////////////////////////////////////////////////////////
// Check ABI before attempting to use Python module
////////////////////////////////////////////////////////////////////////
%insert("python")
%{

COMPILE_ABI_VERSION = "@SOAPY_SDR_ABI_VERSION@"
PYTHONLIB_ABI_VERSION = _SoapySDR.SOAPY_SDR_ABI_VERSION
CORELIB_ABI_VERSION = _SoapySDR.getABIVersion()

if not (COMPILE_ABI_VERSION == PYTHONLIB_ABI_VERSION == CORELIB_ABI_VERSION):
    raise Exception("""Failed ABI check.
Import script:    {0}
Python module:    {1}
SoapySDR library: {2}""".format(COMPILE_ABI_VERSION, PYTHONLIB_ABI_VERSION, CORELIB_ABI_VERSION))
%}

////////////////////////////////////////////////////////////////////////
// Include all major headers to compile against
////////////////////////////////////////////////////////////////////////
%{
#include <SoapySDR/Version.hpp>
#include <SoapySDR/Modules.hpp>
#include <SoapySDR/Device.hpp>
#include <SoapySDR/Errors.hpp>
#include <SoapySDR/Formats.hpp>
#include <SoapySDR/Time.hpp>
#include <SoapySDR/Logger.hpp>
%}

////////////////////////////////////////////////////////////////////////
// http://www.swig.org/Doc2.0/Library.html#Library_stl_exceptions
////////////////////////////////////////////////////////////////////////
%include <exception.i>

// We only expect to throw DirectorExceptions from within
// SoapySDR_pythonLogHandlerBase calls.  Catching them permits us to
// propagate exceptions thrown in the Python log handler callback back to
// Python.
%exception
{
    try{$action}
    catch (const Swig::DirectorException &e) { SWIG_fail; }
    SWIG_CATCH_STDEXCEPT
    catch (...)
    {SWIG_exception(SWIG_RuntimeError, "unknown");}
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
%ignore SoapySDR::Detail::StringToSetting; //ignore SFINAE overloads
%include <SoapySDR/Types.hpp>

//handle arm 32-bit case where size_t and unsigned are the same
#ifdef SIZE_T_IS_UNSIGNED_INT
%typedef unsigned int size_t;
#else
%template(SoapySDRUnsignedList) std::vector<unsigned>;
#endif

%template(SoapySDRKwargs) std::map<std::string, std::string>;
%template(SoapySDRKwargsList) std::vector<SoapySDR::Kwargs>;
%template(SoapySDRArgInfoList) std::vector<SoapySDR::ArgInfo>;
%template(SoapySDRStringList) std::vector<std::string>;
%template(SoapySDRRangeList) std::vector<SoapySDR::Range>;
%template(SoapySDRSizeList) std::vector<size_t>;
%template(SoapySDRDoubleList) std::vector<double>;
%template(SoapySDRDeviceList) std::vector<SoapySDR::Device *>;

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
            fields = [self.minimum(), self.maximum()]
            if self.step() != 0.0: fields.append(self.step())
            return ', '.join(['%g'%f for f in fields])
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
            ret(0), flags(0), timeNs(0), chanMask(0){}
        int ret;
        int flags;
        long long timeNs;
        size_t chanMask;
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
// Constants SOAPY_SDR_*
////////////////////////////////////////////////////////////////////////
%include <SoapySDR/Constants.h>
//import types.h for the defines
//these ignores are C++ functions that were taken by %template() above
%ignore SoapySDRKwargs;
%ignore SoapySDRKwargs_clear;
%ignore SoapySDRKwargsList_clear;
%ignore SoapySDRArgInfoList_clear;
%include <SoapySDR/Types.h>
%include <SoapySDR/Errors.h>
%include <SoapySDR/Version.h>
%include <SoapySDR/Formats.h>

////////////////////////////////////////////////////////////////////////
// Logging tie-ins for python
////////////////////////////////////////////////////////////////////////
%ignore SoapySDR_logf;
%ignore SoapySDR_vlogf;
%ignore SoapySDR_registerLogHandler;
%ignore SoapySDR::logf;
%ignore SoapySDR::vlogf;
%ignore SoapySDR::registerLogHandler;
%include <SoapySDR/Logger.h>
%include <SoapySDR/Logger.hpp>

%feature("director:except") {
    if ($error != NULL) {
        throw Swig::DirectorMethodException();
    }
}


%feature("director") _SoapySDR_pythonLogHandlerBase;

%inline %{
    class _SoapySDR_pythonLogHandlerBase
    {
    public:
        _SoapySDR_pythonLogHandlerBase(void)
        {
            globalHandle = this;
            SoapySDR::registerLogHandler(&globalHandler);
        }
        virtual ~_SoapySDR_pythonLogHandlerBase(void)
        {
            globalHandle = nullptr;
            // Restore the default, C coded, log handler.
            SoapySDR::registerLogHandler(nullptr);
        }
        virtual void handle(const SoapySDR::LogLevel, const char *) = 0;

    private:
        static void globalHandler(const SoapySDR::LogLevel logLevel, const char *message)
        {
            if (globalHandle != nullptr) globalHandle->handle(logLevel, message);
        }

        static _SoapySDR_pythonLogHandlerBase *globalHandle;
    };
%}

%{
    _SoapySDR_pythonLogHandlerBase *_SoapySDR_pythonLogHandlerBase::globalHandle = nullptr;
%}

%insert("python")
%{
_SoapySDR_globalLogHandlers = [None]

class _SoapySDR_pythonLogHandler(_SoapySDR_pythonLogHandlerBase):
    def __init__(self, handler):
        self.handler = handler
        getattr(_SoapySDR_pythonLogHandlerBase, '__init__')(self)

    def handle(self, *args): self.handler(*args)

def registerLogHandler(handler):
    """Register a new system log handler.

    Platforms should call this to replace the default stdio handler.

    :param handler: is a callback function that's called each time an event is
    to be logged by the SoapySDR module.  It is passed the log level and the
    the log message.  The callback shouldn't return anything, but may throw
    exceptions which can be handled in turn in the Python client code.
    Alternately, setting handler to None restores the default.

    :type handler: Callable[[int, str], None] or None

    :returns: None
    """
    if handler is None:
        _SoapySDR_globalLogHandlers[0] = None
    else:
        _SoapySDR_globalLogHandlers[0] = _SoapySDR_pythonLogHandler(handler)
%}

////////////////////////////////////////////////////////////////////////
// Utility functions
////////////////////////////////////////////////////////////////////////
%include <SoapySDR/Errors.hpp>
%include <SoapySDR/Version.hpp>
%include <SoapySDR/Modules.hpp>
%include <SoapySDR/Formats.hpp>
%include <SoapySDR/Time.hpp>

%ignore SoapySDR::logf;
%ignore SoapySDR::vlogf;
%ignore SoapySDR::registerLogHandler;
%include <SoapySDR/Logger.hpp>

////////////////////////////////////////////////////////////////////////
// Device object
////////////////////////////////////////////////////////////////////////
%nodefaultctor SoapySDR::Device;
%include <SoapySDR/Device.hpp>

//narrow import * to SOAPY_SDR_ constants
%pythoncode %{

__all__ = list()
for key in sorted(globals().keys()):
    if key.startswith('SOAPY_SDR_'):
        __all__.append(key)
%}

//make device a constructable class
%insert("python")
%{
class Device(Device):
    def __new__(cls, *args, **kwargs):
        return cls.make(*args, **kwargs)

def extractBuffPointer(buff):
    if hasattr(buff, '__array_interface__'): return buff.__array_interface__['data'][0]
    if hasattr(buff, 'buffer_info'): return buff.buffer_info()[0]
    if hasattr(buff, '__long__'): return long(buff)
    if hasattr(buff, '__int__'): return int(buff)
    raise Exception("Unrecognized data format: " + str(type(buff)))
%}

%extend SoapySDR::Device
{
    //additional overloads for writeSetting for basic types
    %template(writeSetting) SoapySDR::Device::writeSetting<bool>;
    %template(writeSetting) SoapySDR::Device::writeSetting<double>;
    %template(writeSetting) SoapySDR::Device::writeSetting<long long>;
    %template(readSensorBool) SoapySDR::Device::readSensor<bool>;
    %template(readSensorInt) SoapySDR::Device::readSensor<long long>;
    %template(readSensorFloat) SoapySDR::Device::readSensor<double>;
    %template(readSettingBool) SoapySDR::Device::readSetting<bool>;
    %template(readSettingInt) SoapySDR::Device::readSetting<long long>;
    %template(readSettingFloat) SoapySDR::Device::readSetting<double>;

    StreamResult readStream__(SoapySDR::Stream *stream, const std::vector<size_t> &buffs, const size_t numElems, const int flags, const long timeoutUs)
    {
        StreamResult sr;
        sr.flags = flags;
        std::vector<void *> ptrs(buffs.size());
        for (size_t i = 0; i < buffs.size(); i++) ptrs[i] = (void *)buffs[i];
        sr.ret = self->readStream(stream, (&ptrs[0]), numElems, sr.flags, sr.timeNs, timeoutUs);
        return sr;
    }

    StreamResult writeStream__(SoapySDR::Stream *stream, const std::vector<size_t> &buffs, const size_t numElems, const int flags, const long long timeNs, const long timeoutUs)
    {
        StreamResult sr;
        sr.flags = flags;
        std::vector<const void *> ptrs(buffs.size());
        for (size_t i = 0; i < buffs.size(); i++) ptrs[i] = (const void *)buffs[i];
        sr.ret = self->writeStream(stream, (&ptrs[0]), numElems, sr.flags, timeNs, timeoutUs);
        return sr;
    }

    StreamResult readStreamStatus__(SoapySDR::Stream *stream, const long timeoutUs)
    {
        StreamResult sr;
        sr.ret = self->readStreamStatus(stream, sr.chanMask, sr.flags, sr.timeNs, timeoutUs);
        return sr;
    }

    %insert("python")
    %{
        #manually unmake and flag for future calls and the deleter
        def close(self):
            try: getattr(self, '__closed__')
            except AttributeError: Device.unmake(self)
            setattr(self, '__closed__', True)

        def __del__(self): self.close()

        def __str__(self):
            return "%s:%s"%(self.getDriverKey(), self.getHardwareKey())

        def readStream(self, stream, buffs, numElems, flags = 0, timeoutUs = 100000):
            ptrs = [extractBuffPointer(b) for b in buffs]
            return self.readStream__(stream, ptrs, numElems, flags, timeoutUs)

        def writeStream(self, stream, buffs, numElems, flags = 0, timeNs = 0, timeoutUs = 100000):
            ptrs = [extractBuffPointer(b) for b in buffs]
            return self.writeStream__(stream, ptrs, numElems, flags, timeNs, timeoutUs)

        def readStreamStatus(self, stream, timeoutUs = 100000):
            return self.readStreamStatus__(stream, timeoutUs)
    %}
};
