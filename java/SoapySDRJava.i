// Copyright (c) 2021 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

%module(directors="1") SoapySDRJava

%include "java_init.i"

JAVA_INIT("SoapySDR")
JAVA_INIT("SoapySDRJava")

%include <exception.i>
%exception
{
    try{$action}
    catch (const Swig::DirectorException &e) {
        SWIG_JavaException(jenv, SWIG_RuntimeError, e.what());
        return 0;
    }
    SWIG_CATCH_STDEXCEPT
    catch (...)
    {SWIG_exception(SWIG_RuntimeError, "unknown");}
}

%{
#include <SoapySDR/Time.hpp>
%}

%include <SoapySDR/Time.hpp>
