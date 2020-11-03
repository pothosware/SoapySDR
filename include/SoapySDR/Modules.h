///
/// \file SoapySDR/Modules.h
///
/// Utility functions to deal with modules.
/// These utility functions are made available for advanced usage.
/// For most use cases, the API will automatically load modules.
///
/// \copyright
/// Copyright (c) 2014-2020 Josh Blum
/// SPDX-License-Identifier: BSL-1.0
///

#pragma once
#include <SoapySDR/Config.h>
#include <SoapySDR/Types.h>
#include <stddef.h> //size_t

#ifdef __cplusplus
extern "C" {
#endif

//! Query the root installation path
SOAPY_SDR_API const char *SoapySDR_getRootPath(void);

/*!
 * The list of paths automatically searched by loadModules().
 * \param [out] length the number of elements in the result.
 * \return a list of automatically searched file paths
 */
SOAPY_SDR_API char **SoapySDR_listSearchPaths(size_t *length);

/*!
 * List all modules found in default path.
 * The result is an array of strings owned by the caller.
 * \param [out] length the number of elements in the result.
 * \return a list of file paths to loadable modules
 */
SOAPY_SDR_API char **SoapySDR_listModules(size_t *length);

/*!
 * List all modules found in the given path.
 * The result is an array of strings owned by the caller.
 * \param path a directory on the system
 * \param [out] length the number of elements in the result.
 * \return a list of file paths to loadable modules
 */
SOAPY_SDR_API char **SoapySDR_listModulesPath(const char *path, size_t *length);

/*!
 * Load a single module given its file system path.
 * The caller must free the result error string.
 * \param path the path to a specific module file
 * \return an error message, empty on success
 */
SOAPY_SDR_API char *SoapySDR_loadModule(const char *path);

/*!
 * List all registration loader errors for a given module path.
 * The resulting dictionary contains all registry entry names
 * provided by the specified module. The value of each entry
 * is an error message string or empty on successful load.
 * \param path the path to a specific module file
 * \return a dictionary of registry names to error messages
 */
SOAPY_SDR_API SoapySDRKwargs SoapySDR_getLoaderResult(const char *path);

/*!
 * Get a version string for the specified module.
 * Modules may optionally provide version strings.
 * \param path the path to a specific module file
 * \return a version string or empty if no version provided
 */
SOAPY_SDR_API char *SoapySDR_getModuleVersion(const char *path);

/*!
 * Unload a module that was loaded with loadModule().
 * The caller must free the result error string.
 * \param path the path to a specific module file
 * \return an error message, empty on success
 */
SOAPY_SDR_API char *SoapySDR_unloadModule(const char *path);

/*!
 * Load the support modules installed on this system.
 * This call will only actually perform the load once.
 * Subsequent calls are a NOP.
 */
SOAPY_SDR_API void SoapySDR_loadModules(void);

/*!
 * Unload all currently loaded support modules.
 */
SOAPY_SDR_API void SoapySDR_unloadModules(void);

#ifdef __cplusplus
}
#endif
