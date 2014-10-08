///
/// \file SoapySDR/Modules.h
///
/// Utility functions to deal with modules.
/// These utility functions are made available for advanced usage.
/// For most use cases, the API will automatically load modules.
///
/// \copyright
/// Copyright (c) 2014-2014 Josh Blum
/// SPDX-License-Identifier: BSL-1.0
///

#pragma once
#include <SoapySDR/Config.h>
#include <stddef.h> //size_t

#ifdef __cplusplus
extern "C" {
#endif

//! Query the root installation path
SOAPY_SDR_API const char *SoapySDR_getRootPath(void);

/*!
 * List the paths to the modules available on this system.
 * The result is an array of strings owned by the caller.
 * \param [out] length the number of elements in the result.
 */
SOAPY_SDR_API char **SoapySDR_listModules(size_t *length);

/*!
 * Load a single module given its file system path.
 */
SOAPY_SDR_API void SoapySDR_loadModule(const char *path);

/*!
 * Load the support modules installed on this system.
 * This call will only actually perform the load once.
 * Subsequent calls are a NOP.
 */
SOAPY_SDR_API void SoapySDR_loadModules(void);

#ifdef __cplusplus
}
#endif
