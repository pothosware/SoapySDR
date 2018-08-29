///
/// \file SoapySDR/ConverterRegistry.hpp
///
/// Convert buffers between stream formats.
///
/// \copyright
/// Copyright (c) 2015-2018 Josh Blum
/// Copyright (c) 2017-2018 Coburn Wightman
/// SPDX-License-Identifier: BSL-1.0
///

#pragma once
#include <SoapySDR/Config.hpp>
#include <SoapySDR/Logger.hpp>
#include <SoapySDR/Formats.hpp>
#include <utility>
#include <vector>
#include <map>
#include <string>

namespace SoapySDR
{
  /*!
   * ConverterRegistry class. The ConverterRegistry maintains a list of
   * ConverterFunctions that can be queried and retrieved by markup strings for
   * converting buffers between formats.
   *
   * While Soapy standard format markup strings are declared in the formats include file,
   * custom formats can be created and ConverterFunctions registered to be used as needed.
   * Additionally, different functions can be registered for the same source/target pair
   * with FunctionPriority serving as a selector to allow specialization.
   */
  class SOAPY_SDR_API ConverterRegistry
  {
  public:
    /*!
     * A typedef for declaring a ConverterFunction to be maintained in the ConverterRegistry.
     * A converter function copies and optionally converts an input buffer of one format into an
     * output buffer of another format.
     * The parameters are (input pointer, output pointer, number of elements, optional scalar)
     */
    typedef void (*ConverterFunction)(const void *, void *, const size_t, const double);

    /*!
     * FunctionPriority: allow selection of a converter function with a given source and target format.
     */
    enum FunctionPriority{
      GENERIC = 0,          //!< Usual C for-loops, shifts, multiplies, etc. Min priority.
      VECTORIZED = 3,       //!< Vectorized operations such as SIMD.
      CUSTOM = 5            //!< Custom user re-implementation. Max priority.
    };

    /*!
     * TargetFormatConverterPriority: a map of possible conversion functions for a given Priority.
     * Maintained by the registry.
     */
    typedef std::map<FunctionPriority, ConverterFunction> TargetFormatConverterPriority;

    /*!
     * TargetFormatConverters: a map of possible conversion functions for a given Target/Priority Format.
     * Maintained by the registry.
     */
    typedef std::map<std::string, TargetFormatConverterPriority> TargetFormatConverters;
    
    /*!
     * FormatConverters: a map of possible conversion functions for a given Source/Target Format.
     * Maintained by the registry.
     */
    typedef std::map<std::string, TargetFormatConverters> FormatConverters;

    /*!
     * Class constructor. Registers a ConverterFunction with a
     * given source format, target format, and priority.
     *
     * refuses to register converter and logs error if a source/target/priority entry already exists
     * \param sourceFormat the source format markup string
     * \param targetFormat the target format markup string
     * \param priority the FunctionPriority of the converter to register
     * \param converter function to register
     */
    ConverterRegistry(const std::string &sourceFormat, const std::string &targetFormat, const FunctionPriority &priority, ConverterFunction converter);
    
    /*!
     * Get a list of existing target formats to which we can convert the specified source from.
     * There is a source format converter function registered for each target format
     * returned in the result vector.
     * \param sourceFormat the source format markup string
     * \return a vector of target formats or an empty vector if none found
     */
    static std::vector<std::string> listTargetFormats(const std::string &sourceFormat);
    
    /*!
     * Get a list of existing source formats from which we can convert to the specified target.
     * There is a target format converter function registered for each source format
     * returned in the result vector.
     * \param targetFormat the target format markup string
     * \return a vector of source formats or an empty vector if none found
     */
    static std::vector<std::string> listSourceFormats(const std::string &targetFormat);
    
    /*!
     * Get a list of available converter priorities for a given source and target format.
     * \param sourceFormat the source format markup string
     * \param targetFormat the target format markup string
     * \return a vector of priorities or an empty vector if none found
     */
    static std::vector<FunctionPriority> listPriorities(const std::string &sourceFormat, const std::string &targetFormat);
    
    /*!
     * Get a converter between a source and target format with the highest available priority.
     * \throws invalid_argument when the conversion does not exist and logs error
     * \param sourceFormat the source format markup string
     * \param targetFormat the target format markup string
     * \return a conversion function pointer
     */
    static ConverterFunction getFunction(const std::string &sourceFormat, const std::string &targetFormat);

    /*!
     * Get a converter between a source and target format with a given priority.
     */

    static ConverterFunction getFunction(const std::string &sourceFormat, const std::string &targetFormat, const FunctionPriority &priority);

    /*!
     * Get a list of known source formats in the registry.
     */
    static std::vector<std::string> listAvailableSourceFormats(void);

  };
  
}
