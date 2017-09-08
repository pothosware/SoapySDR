///
/// \file SoapySDR/Convert.hpp
///
/// Convert buffers between stream formats.
///
/// \copyright
/// Copyright (c) 2015-2015 Josh Blum
/// SPDX-License-Identifier: BSL-1.0
///

#pragma once
#include <SoapySDR/Formats.hpp>
#include <vector>
#include <map>
#include <string>
#include <stdexcept>

namespace SoapySDR
{
/*!
 * A typedef for a conversion function.
 * A conversion function converts an input buffer into and output buffer.
 * The parameters are (input pointer, output pointer, number of elements, optional scalar)
 */
typedef void (*ConvertFunction)(const void *, void *, const size_t, const double);

/*!
 * FormatConverterPriority: allow selection of a converter function with a given source and target format
 */
enum FormatConverterPriority{
  GENERIC = 0,          // usual C for loops and shifts and multiplies
  VECTORIZED = 3,       // using SIMD vectorized operations probably
  CUSTOM = 5            // custom user re-implementation, max prio
};

    
/*!
 * TargetFormatConverterPriority: a map of possible conversion functions for a given priority.
 */
typedef std::map<FormatConverterPriority, ConvertFunction> TargetFormatConverterPriority;

/*!
 * TargetFormatConverters: a map of possible conversion functions for a given Target Format.
 */
typedef std::map<std::string, TargetFormatConverterPriority> TargetFormatConverters;

/*!
 * FormatConverters: a map of possible conversion functions for a given Source Format.
 */
typedef std::map<std::string, TargetFormatConverters> FormatConverters;

/*!
 * Get a list of formats to which we can convert the source format into.
 * There is a registered conversion function from the specified source
 * format to every target format returned in the result vector.
 * \param sourceFormat the source format markup string
 * \return a list of target formats
 */
SOAPY_SDR_API std::vector<std::string> convertTargetFormats(const std::string &sourceFormat);

/*!
 * Get a list of formats to which we can convert the target format from.
 * There is a registered conversion function from every source format
 * returned in the result vector to the specified target format.
 * \param targetFormat the target format markup string
 * \return a list of source formats
 */
SOAPY_SDR_API std::vector<std::string> convertSourceFormats(const std::string &targetFormat);

/*!
 * Register a converter between a source and target format.
 * \throws invalid_argument if the conversion already exists
 * \param sourceFormat the source format markup string
 * \param targetFormat the target format markup string
 * \param converter function to register
 * \return true
 */
SOAPY_SDR_API bool registerConverter(const std::string &sourceFormat, const std::string &targetFormat, ConvertFunction, FormatConverterPriority &);

/*!
 * Get a list of available converter priorities for a given source and target format.
 * \throws invalid_argument when the conversion does not exist
 * \param sourceFormat the source format markup string
 * \param targetFormat the target format markup string
 * \return a vector of priorities
 */
SOAPY_SDR_API std::vector<FormatConverterPriority> getConverterPriorities(const std::string &sourceFormat, const std::string &targetFormat);
  
/*!
 * Get a converter between a source and target format.
 * \throws invalid_argument when the conversion does not exist
 * \param sourceFormat the source format markup string
 * \param targetFormat the target format markup string
 * \param priority converter priority or highest if not specified
 * \return a conversion function pointer
 */
SOAPY_SDR_API ConvertFunction getConverter(const std::string &sourceFormat, const std::string &targetFormat);

SOAPY_SDR_API ConvertFunction getConverter(const std::string &sourceFormat, const std::string &targetFormat, FormatConverterPriority &priority);

}
