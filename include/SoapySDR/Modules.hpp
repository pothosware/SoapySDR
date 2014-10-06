///
/// \file SoapySDR/Modules.hpp
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
#include <SoapySDR/Config.hpp>
#include <vector>
#include <string>

namespace SoapySDR
{

//! Query the root installation path
SOAPY_SDR_API std::string getRootPath(void);

/*!
 * List the paths to the modules available on this system.
 */
SOAPY_SDR_API std::vector<std::string> listModules(void);

/*!
 * Load a single module given its file system path.
 */
SOAPY_SDR_API void loadModule(const std::string &path);

/*!
 * Load the support modules installed on this system.
 * This call will only actually perform the load once.
 * Subsequent calls are a NOP.
 */
SOAPY_SDR_API void loadModules(void);

}
