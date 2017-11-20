///
/// \file SoapySDR/Version.hpp
///
/// Utility functions to query version information.
///
/// \copyright
/// Copyright (c) 2014-2016 Josh Blum
/// SPDX-License-Identifier: BSL-1.0
///

#pragma once
#include <SoapySDR/Config.hpp>
#include <SoapySDR/Version.h>
#include <string>

namespace SoapySDR
{

/*!
 * Get the SoapySDR library API version as a string.
 * The format of the version string is <b>major.minor.increment</b>,
 * where the digits are taken directly from <b>SOAPY_SDR_API_VERSION</b>.
 */
SOAPY_SDR_API std::string getAPIVersion(void);

/*!
 * Get the ABI version string that the library was built against.
 * A client can compare <b>SOAPY_SDR_ABI_VERSION</b> to getABIVersion()
 * to check for ABI incompatibility before using the library.
 * If the values are not equal then the client code was
 * compiled against a different ABI than the library.
 */
SOAPY_SDR_API std::string getABIVersion(void);

/*!
 * Get the library version and build information string.
 * The format of the version string is <b>major.minor.patch-buildInfo</b>.
 * This function is commonly used to identify the software back-end
 * to the user for command-line utilities and graphical applications.
 */
SOAPY_SDR_API std::string getLibVersion(void);

}
