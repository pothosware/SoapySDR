///
/// \file SoapySDR/Config.h
///
/// Common macro definitions for Soapy SDR library API export.
///
/// \copyright
/// Copyright (c) 2014-2014 Josh Blum
/// SPDX-License-Identifier: BSL-1.0
///

#pragma once
// http://gcc.gnu.org/wiki/Visibility
// Generic helper definitions for shared library support
#if defined _WIN32 || defined __CYGWIN__
  #define SOAPY_SDR_HELPER_DLL_IMPORT __declspec(dllimport)
  #define SOAPY_SDR_HELPER_DLL_EXPORT __declspec(dllexport)
  #define SOAPY_SDR_HELPER_DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define SOAPY_SDR_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
    #define SOAPY_SDR_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
    #define SOAPY_SDR_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define SOAPY_SDR_HELPER_DLL_IMPORT
    #define SOAPY_SDR_HELPER_DLL_EXPORT
    #define SOAPY_SDR_HELPER_DLL_LOCAL
  #endif
#endif

// Now we use the generic helper definitions above to define SOAPY_SDR_API and SOAPY_SDR_LOCAL.
// SOAPY_SDR_API is used for the public API symbols. It either DLL imports or DLL exports (or does nothing for static build)
// SOAPY_SDR_LOCAL is used for non-api symbols.

#define SOAPY_SDR_DLL //always building a DLL

#ifdef SOAPY_SDR_DLL // defined if SOAPY is compiled as a DLL
  #ifdef SOAPY_SDR_DLL_EXPORTS // defined if we are building the SOAPY DLL (instead of using it)
    #define SOAPY_SDR_API SOAPY_SDR_HELPER_DLL_EXPORT
    #define SOAPY_SDR_EXTERN
  #else
    #define SOAPY_SDR_API SOAPY_SDR_HELPER_DLL_IMPORT
    #define SOAPY_SDR_EXTERN extern
  #endif // SOAPY_SDR_DLL_EXPORTS
  #define SOAPY_SDR_LOCAL SOAPY_SDR_HELPER_DLL_LOCAL
#else // SOAPY_SDR_DLL is not defined: this means SOAPY is a static lib.
  #define SOAPY_SDR_API
  #define SOAPY_SDR_LOCAL
  #define SOAPY_SDR_EXTERN
#endif // SOAPY_SDR_DLL

#include <iso646.h>

#ifndef _MSC_VER
#include <stdbool.h>
#endif
