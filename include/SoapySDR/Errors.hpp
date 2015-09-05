///
/// \file SoapySDR/Errors.hpp
///
/// Error codes used in the device API.
///
/// \copyright
/// Copyright (c) 2015-2015 Josh Blum
/// SPDX-License-Identifier: BSL-1.0
///

#pragma once
#include <SoapySDR/Config.h>
#include <SoapySDR/Errors.h>

namespace SoapySDR
{

/*!
 * Convert a error code to a string for printing purposes.
 * If the error code is unrecognized, errToStr returns "UNKNOWN".
 * \param errorCode a negative integer return code
 * \return a pointer to a string representing the error
 */
SOAPY_SDR_API const char *errToStr(const int errorCode);

}
