///
/// \file SoapySDR/Formats.hpp
///
/// Format strings used in the stream API.
///
/// \copyright
/// Copyright (c) 2015-2015 Josh Blum
/// SPDX-License-Identifier: BSL-1.0
///

#pragma once
#include <SoapySDR/Config.h>
#include <SoapySDR/Formats.h>
#include <string>
#include <cstddef>

namespace SoapySDR
{

/*!
 * Get the size of a single element in the specified format.
 * \param format a supported format string
 * \return the size of an element in bytes
 */
SOAPY_SDR_API size_t formatToSize(const std::string &format);

}
