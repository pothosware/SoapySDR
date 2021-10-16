// Copyright (c) 2021 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

// TODO: Format needs to be manually done to not be ugly

// Ignore these
#define SOAPY_SDR_API

%module(directors="1") SoapySDRJava
%include "enums.swg"
%javaconst(1);

%include "java_init.i"

JAVA_INIT("SoapySDR")
JAVA_INIT("SoapySDRJava")

%include <exception.i>
%exception
{
    try{$action}
    catch (const Swig::DirectorException &e) {
        SWIG_JavaException(jenv, SWIG_RuntimeError, e.what());
    }
    SWIG_CATCH_STDEXCEPT
    catch (...)
    {SWIG_exception(SWIG_RuntimeError, "unknown");}
}

%include <std_map.i>
%include <std_string.i>
%include <std_vector.i>

// TODO: hide these
%template(StringVector) std::vector<std::string>;

%{
#include "JavaWrappers.hpp"

#include <SoapySDR/Types.hpp>
%}

%nodefaultctor SoapySDR::Java::Time;
%nodefaultdtor SoapySDR::Java::Time;
%include "JavaWrappers.hpp"

%ignore SettingToString;
%ignore StringToSetting;
%ignore KwargsFromString;
%ignore KwargsToString;
%include <SoapySDR/Types.hpp>

%include "Logger.i"
