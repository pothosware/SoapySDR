///
/// \file SoapySDR/Version.hpp
///
/// Utility functions to query version information.
///
/// \copyright
/// Copyright (c) 2014-2014 Josh Blum
/// SPDX-License-Identifier: BSL-1.0
///

#pragma once
#include <SoapySDR/Config.hpp>
#include <SoapySDR/Version.h>
#include <string>

namespace SoapySDR
{

//! Query the API version string
SOAPY_SDR_API std::string getAPIVersion(void);

/*!
 * Get the ABI version string.
 * This is the SOAPY_SDR_ABI_VERSION that the library was built against.
 */
SOAPY_SDR_API std::string getABIVersion(void);

}
