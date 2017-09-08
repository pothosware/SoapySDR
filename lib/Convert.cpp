// Copyright (c) 2017-2017 Coburn Wightman
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Convert.hpp>

SoapySDR::FormatConverters Converters;

bool SoapySDR::registerConverter(const std::string &sourceFormat, const std::string &targetFormat, SoapySDR::ConvertFunction converterFunction, FormatConverterPriority &priority)
{
  if (Converters[sourceFormat][targetFormat].count(priority) == 0)
    {
      Converters[sourceFormat][targetFormat][priority] = converterFunction;
    }
  else
    {
      throw std::invalid_argument("conversion already registered: " + sourceFormat + " to " + targetFormat + " priority " + std::to_string(priority) + ".");
    }
  
  return true;
}

std::vector<std::string> SoapySDR::convertTargetFormats(const std::string &sourceFormat)
{
  std::vector<std::string> targets;

  if (Converters.count(sourceFormat) == 0)
    return targets;

  for(const auto &it:Converters[sourceFormat])
    {
      std::string targetFormat = it.first;
      targets.push_back(targetFormat);
    }
  
  return targets;
}

std::vector<std::string> SoapySDR::convertSourceFormats(const std::string &targetFormat)
{
  std::vector<std::string> sources;

  for(const auto &it:Converters)
    {
      std::string sourceFormat = it.first;
      if (Converters[sourceFormat].count(targetFormat) > 0)
	sources.push_back(sourceFormat);
    }
  
  return sources;
}

std::vector<SoapySDR::FormatConverterPriority> SoapySDR::getConverterPriorities(const std::string &sourceFormat, const std::string &targetFormat)
{
  std::vector<FormatConverterPriority> priorities;
  
  if (Converters.count(sourceFormat) == 0)
    throw std::invalid_argument("unsupported source format: " + sourceFormat + "."); 
  
  if (Converters[sourceFormat].count(targetFormat) == 0)
    throw std::invalid_argument("unsupported target format: " + targetFormat + ".");

  // if (Converters[sourceFormat][targetFormat].size() == 0)
  //   throw std::invalid_argument("no registered functions for: " sourceFormat + " to "+ targetFormat + ".");
  
  for(const auto &it:Converters[sourceFormat][targetFormat])
    {
      FormatConverterPriority priority = it.first;
      priorities.push_back(priority);
    }
  
  return priorities;
  
}

SoapySDR::ConvertFunction SoapySDR::getConverter(const std::string &sourceFormat, const std::string &targetFormat)
{
  if (Converters.count(sourceFormat) == 0)
    throw std::invalid_argument("unsupported source format: " + sourceFormat + "."); 
  
  if (Converters[sourceFormat].count(targetFormat) == 0)
    throw std::invalid_argument("unsupported target format: " + targetFormat + "."); 

  return Converters[sourceFormat][targetFormat].rbegin()->second;
}

SoapySDR::ConvertFunction SoapySDR::getConverter(const std::string &sourceFormat, const std::string &targetFormat, FormatConverterPriority &priority)
{
  if (Converters.count(sourceFormat) == 0)
    throw std::invalid_argument("unsupported source format: " + sourceFormat + "."); 
  
  if (Converters[sourceFormat].count(targetFormat) == 0)
    throw std::invalid_argument("unsupported target format: " + targetFormat + "."); 

  if (Converters[sourceFormat][targetFormat].count(priority) == 0)
    throw std::invalid_argument("unsupported target priority: " + std::to_string(priority) + "."); 

  return Converters[sourceFormat][targetFormat][priority];
}
