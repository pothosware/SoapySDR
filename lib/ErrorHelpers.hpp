// Copyright (c) 2016-2020 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#pragma once
#include <SoapySDR/Config.hpp>
#include <stdexcept>

/*******************************************************************
 * Helper macros for dealing with error messages
 ******************************************************************/

//! Start a section with an open try brace
#define __SOAPY_SDR_C_TRY \
    SoapySDRDevice_clearError(); try {

//! Close a section with a catch, with specified return code
#define __SOAPY_SDR_C_CATCH_RET(ret) } \
    catch (const std::exception &ex) { SoapySDRDevice_reportError(ex.what()); return ret; } \
    catch (...) { SoapySDRDevice_reportError("unknown"); return ret; }

//! Close a section with a catch, -1 return on error
#define __SOAPY_SDR_C_CATCH \
    __SOAPY_SDR_C_CATCH_RET(-1) return 0;

//! Pass into __SOAPY_SDR_C_CATCH_RET for void return calls
#define SoapySDRVoidRet

//! Clear the error on try macro entry
void SoapySDRDevice_clearError(void);

//! Report error called by catch macro
void SoapySDRDevice_reportError(const char *msg);
