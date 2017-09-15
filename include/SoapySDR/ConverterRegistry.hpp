///
/// \file SoapySDR/RegisterConverters.hpp
///
/// Convert buffers between stream formats.
///
/// \copyright
/// Copyright (c) 2015-2015 Josh Blum
/// Copyright (c) 2017-2017 Coburn Wightman
/// SPDX-License-Identifier: BSL-1.0
///

#pragma once
#include <SoapySDR/Logger.hpp>
#include <SoapySDR/Formats.hpp>
#include <vector>
#include <map>
#include <string>
#include <stdexcept>

namespace SoapySDR
{
  class SOAPY_SDR_API ConverterRegistry
  {
  public:
    /*!
     * A typedef for a conversion function.
     * A conversion function converts an input buffer into and output buffer.
     * The parameters are (input pointer, output pointer, number of elements, optional scalar)
     */
    typedef void (*ConverterFunction)(const void *, void *, const size_t, const double);
  
    /*!
     * FormatConverterPriority: allow selection of a converter function with a given source and target format
     */
    enum FunctionPriority{
      GENERIC = 0,          // usual C for loops and shifts and multiplies
      VECTORIZED = 3,       // using SIMD vectorized operations probably
      CUSTOM = 5            // custom user re-implementation, max prio
    };

    /*!
     * TargetFormatConverterPriority: a map of possible conversion functions for a given Priority.
     */
    typedef std::map<FunctionPriority, ConverterFunction> TargetFormatConverterPriority;

    /*!
     * TargetFormatConverters: a map of possible conversion functions for a given Target/Priority Format.
     */
    typedef std::map<std::string, TargetFormatConverterPriority> TargetFormatConverters;
    
    /*!
     * FormatConverters: a map of possible conversion functions for a given Source/Target Format.
     */
    typedef std::map<std::string, TargetFormatConverters> FormatConverters;

    /*!
     * Class constructor for managing the Converter Registry.
     * refuses to register converter and logs error if a source/target/priority entry already exists
     * \param sourceFormat the source format markup string
     * \param targetFormat the target format markup string
     * \param priority the FunctionPriority of the converter to register
     * \param converter function to register
     */
    ConverterRegistry(const std::string &sourceFormat, const std::string &targetFormat, FunctionPriority &, ConverterFunction);
    
    /*!
     * Class constructor for using the Converter Registry.
     * \param none no parameters
     */
    ConverterRegistry(void);
    
    ~ConverterRegistry(void);

    /*!
     * did the constructor for this instance register a function.
     * \param none
     * \return true if a function has been registered
     */    
    bool isRegistered(void);

    /*!
     * Remove the converter registered by this object otherwise do nothing.
     * \param none
     * \return nothing
     */
    void remove(void);
    
    /*!
     * Get a list of formats to which we can convert the source format into.
     * There is a registered conversion function from the specified source
     * format to every target format returned in the result vector.
     * \param sourceFormat the source format markup string
     * \return a vector of target formats or an empty vector if none found
     */
    std::vector<std::string> listTargetFormats(const std::string &sourceFormat);
    
    /*!
     * Get a list of formats to which we can convert the target format from.
     * There is a registered conversion function from every source format
     * returned in the result vector to the specified target format.
     * \param targetFormat the target format markup string
     * \return a vector of source formats or an empty vector if none found
     */
    std::vector<std::string> listSourceFormats(const std::string &targetFormat);
    
    /*!
     * Get a list of available converter priorities for a given source and target format.
     * \throws invalid_argument when the conversion does not exist and logs error
     * \param sourceFormat the source format markup string
     * \param targetFormat the target format markup string
     * \return a vector of priorities
     */
    std::vector<FunctionPriority> listPriorities(const std::string &sourceFormat, const std::string &targetFormat);
    
    /*!
     * Get a converter between a source and target format.
     * \throws invalid_argument when the conversion does not exist and logs error
     * \param sourceFormat the source format markup string
     * \param targetFormat the target format markup string
     * \return a conversion function pointer
     */
    ConverterFunction getFunction(const std::string &sourceFormat, const std::string &targetFormat);
    ConverterFunction getFunction(const std::string &sourceFormat, const std::string &targetFormat, const FunctionPriority &priority);
    
  private:
    static FormatConverters formatConverters;
    
    std::string _sourceFormat;
    std::string _targetFormat;
    FunctionPriority _priority;
    
    bool _isRegistered;
    
  };
  
}
