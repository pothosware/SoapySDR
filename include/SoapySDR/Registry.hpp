///
/// \file SoapySDR/Registry.hpp
///
/// Device factory registration API.
///
/// \copyright
/// Copyright (c) 2014-2014 Josh Blum
/// SPDX-License-Identifier: BSL-1.0
///

#pragma once
#include <SoapySDR/Config.hpp>
#include <SoapySDR/Types.hpp>
#include <vector>

namespace SoapySDR
{

//! forward declaration of sdr device
class Device;

/*!
 * A registry object loads device functions into the global registry.
 */
class SOAPY_SDR_API Registry
{
public:

    //! typedef for a device enumeration function
    typedef std::vector<Kwargs> (*FindFunction)(const Kwargs &);

    //! typedef for a device factory function
    typedef Device* (*MakeFunction)(const Kwargs &);

    /*!
     * Register an SDR device find and make function.
     * \param name a unique name to identify the module
     * \param find the find function returns arg list
     * \param make the make function returns device sptr
     */
    Registry(const std::string &name, const FindFunction &find, const MakeFunction &make);
};

}
