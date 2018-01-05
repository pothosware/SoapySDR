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

//! typedef for a device enumeration function
typedef KwargsList (*FindFunction)(const Kwargs &);

//! typedef for a device factory function
typedef Device* (*MakeFunction)(const Kwargs &);

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
     * Register a SDR device find and make function.
     * \param name a unique name to identify the entry
     * \param find the find function returns an arg list
     * \param make the make function returns a device sptr
     * \param abi this value must be SOAPY_SDR_ABI_VERSION
     */
    Registry(const std::string &name, const FindFunction &find, const MakeFunction &make, const std::string &abi);

    //! Cleanup this registry entry
    ~Registry(void);

    /*!
     * List all loaded find functions.
     * \return a dictionary of registry entry names to find functions
     */
    static FindFunctions listFindFunctions(void);

    /*!
     * List all loaded make functions.
     * \return a dictionary of registry entry names to make functions
     */
    static MakeFunctions listMakeFunctions(void);

private:
    std::string _name;
};

}
