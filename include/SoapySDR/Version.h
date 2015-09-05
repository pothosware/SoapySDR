///
/// \file SoapySDR/Version.h
///
/// Utility functions to query version information.
///
/// \copyright
/// Copyright (c) 2014-2015 Josh Blum
/// SPDX-License-Identifier: BSL-1.0
///

#pragma once
#include <SoapySDR/Config.h>

/*!
 * ABI Version Information - incremented when the ABI is changed.
 */
#define SOAPY_SDR_ABI_VERSION "0.3-0"

/*!
 * Compatibility define for GPIO access API with masks
 */
#define SOAPY_SDR_API_HAS_MASKED_GPIO

/*!
 * Compatibility define for channel sensors access API
 */
#define SOAPY_SDR_API_HAS_CHANNEL_SENSORS

/*!
 * Compatibility define for error to string function
 */
#define SOAPY_SDR_API_HAS_ERR_TO_STR

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Query the API version string.
 */
SOAPY_SDR_API const char *SoapySDR_getAPIVersion(void);

/*!
 * Get the ABI version string.
 * This is the SOAPY_SDR_ABI_VERSION that the library was built against.
 */
SOAPY_SDR_API const char *SoapySDR_getABIVersion(void);

#ifdef __cplusplus
}
#endif
