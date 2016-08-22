// Copyright (c) 2016-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#pragma once
#include <SoapySDR/Config.hpp>
#include <stdexcept>

/*******************************************************************
 * Helper macros for dealing with error messages
 ******************************************************************/

//! Start a section with an open try brace
#define __SOAPY_SDR_C_TRY try {

//! Close a section with a catch, with specified return code
#define __SOAPY_SDR_C_CATCH_RET(ret) } \
    catch (const std::exception &ex) { SoapySDRDevice_reportError(ex.what()); return (ret); } \
    catch (...) { SoapySDRDevice_reportError("unknown"); return (ret); } \
    return 0;

//! Close a section with a catch, -1 return on error
#define __SOAPY_SDR_C_CATCH __SOAPY_SDR_C_CATCH_RET(-1)

//! Report error called by catch macro
void SoapySDRDevice_reportError(const char *msg);
