// Copyright (c) 2016-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#pragma once
#include <SoapySDR/Config.hpp>
#include <stdexcept>

/*******************************************************************
 * Helper macros for dealing with error messages
 ******************************************************************/
#define __SOAPY_SDR_C_TRY try {
#define __SOAPY_SDR_C_CATCH } \
    catch (const std::exception &ex) { return SoapySDRDevice_reportError(ex.what()); } \
    catch (...) { return SoapySDRDevice_reportError("unknown"); } \
    return 0;

int SoapySDRDevice_reportError(const char *msg);
