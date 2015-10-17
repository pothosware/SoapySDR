///
/// \file SoapySDR/Registry.hpp
///
/// Device factory registration API.
///
/// \copyright
/// Copyright (c) 2014-2015 Josh Blum
/// SPDX-License-Identifier: BSL-1.0
///

#pragma once
#include <SoapySDR/Config.hpp>
#include <SoapySDR/Version.hpp>
#include <SoapySDR/Types.hpp>
#include <vector>
#include <string>
#include <map>

namespace SoapySDR
{

//! forward declaration of device
class Device;

//! typedef for a device information function
typedef ArgInfoList (*InfoFunction)(void);

//! typedef for a device enumeration function
typedef KwargsList (*FindFunction)(const Kwargs &);

//! typedef for a device factory function
typedef Device* (*MakeFunction)(const Kwargs &);

//! typedef for a dictionary of info functions
typedef std::map<std::string, InfoFunction> InfoFunctions;

//! typedef for a dictionary of find functions
typedef std::map<std::string, FindFunction> FindFunctions;

//! typedef for a dictionary of make functions
typedef std::map<std::string, MakeFunction> MakeFunctions;

/*!
 * A registry object loads device functions into the global registry.
 */
class SOAPY_SDR_API Registry
{
public:

    /*!
     * Register a SDR device info, find, and make function.
     * \param name a unique name to identify the module
     * \param info the info function returns an arg info list
     * \param find the find function returns an arg list
     * \param make the make function returns a device sptr
     * \param abi this value must be SOAPY_SDR_ABI_VERSION
     */
    Registry(const std::string &name, const InfoFunction &info, const FindFunction &find, const MakeFunction &make, const std::string &abi);

    /*!
     * Register a SDR device find and make function.
     * However, providing an info function is recommended.
     * \param name a unique name to identify the module
     * \param find the find function returns an arg list
     * \param make the make function returns a device sptr
     * \param abi this value must be SOAPY_SDR_ABI_VERSION
     */
    Registry(const std::string &name, const FindFunction &find, const MakeFunction &make, const std::string &abi);

    /*!
     * List all loaded info functions.
     * \return a dictionary of module name to info function
     */
    static InfoFunctions listInfoFunctions(void);

    /*!
     * List all loaded find functions.
     * \return a dictionary of module name to find function
     */
    static FindFunctions listFindFunctions(void);

    /*!
     * List all loaded make functions.
     * \return a dictionary of module name to make function
     */
    static MakeFunctions listMakeFunctions(void);
};

}
