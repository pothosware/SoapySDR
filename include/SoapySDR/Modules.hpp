///
/// \file SoapySDR/Modules.hpp
///
/// Utility functions to deal with modules.
/// These utility functions are made available for advanced usage.
/// For most use cases, the API will automatically load modules.
///
/// \copyright
/// Copyright (c) 2014-2015 Josh Blum
/// SPDX-License-Identifier: BSL-1.0
///

#pragma once
#include <SoapySDR/Config.hpp>
#include <vector>
#include <string>

namespace SoapySDR
{

//! Query the root installation path
SOAPY_SDR_API std::string getRootPath(void);

/*!
 * List all modules found in default path.
 * \return a list of file paths to loadable modules
 */
SOAPY_SDR_API std::vector<std::string> listModules(void);

/*!
 * List all modules found in the given path.
 * \param path a directory on the system
 * \return a list of file paths to loadable modules
 */
SOAPY_SDR_API std::vector<std::string> listModules(const std::string &path);

/*!
 * Load a single module given its file system path.
 * \param path the path to a specific module file
 * \return the modules handle to unload it later (null on error)
 */
SOAPY_SDR_API void *loadModule(const std::string &path);

/*!
 * Unload a module that was loaded with loadModule().
 * \param handle the handle to a loaded module
 * \param return true on success, false on failure
 */
SOAPY_SDR_API bool unloadModule(void *handle);

/*!
 * Load the support modules installed on this system.
 * This call will only actually perform the load once.
 * Subsequent calls are a NOP.
 */
SOAPY_SDR_API void loadModules(void);

}
