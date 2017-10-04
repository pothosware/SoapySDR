// Copyright (c) 2017-2017 Coburn Wightman
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Convert.hpp>

SoapySDR::FormatConverters formatConverters;

bool SoapySDR::registerConverter(const std::string &sourceFormat, const std::string &targetFormat, SoapySDR::ConvertFunction converterFunction, FormatConverterPriority &priority)
{
  if (formatConverters[sourceFormat][targetFormat].count(priority) != 0)
    {
      SoapySDR::log(SOAPY_SDR_ERROR, "converter already registered: " + sourceFormat + " to " + targetFormat + " priority " + std::to_string(priority));
      return false;
    }

  formatConverters[sourceFormat][targetFormat][priority] = converterFunction;
  
  return true;
}

std::vector<std::string> SoapySDR::convertTargetFormats(const std::string &sourceFormat)
{
  std::vector<std::string> targets;

  if (formatConverters.count(sourceFormat) == 0)
    return targets;

  for(const auto &it:formatConverters[sourceFormat])
    {
      std::string targetFormat = it.first;
      targets.push_back(targetFormat);
    }
  
  return targets;
}

std::vector<std::string> SoapySDR::convertSourceFormats(const std::string &targetFormat)
{
  std::vector<std::string> sources;

  for(const auto &it:formatConverters)
    {
      std::string sourceFormat = it.first;
      if (formatConverters[sourceFormat].count(targetFormat) > 0)
	sources.push_back(sourceFormat);
    }
  
  return sources;
}

std::vector<SoapySDR::FormatConverterPriority> SoapySDR::getConverterPriorities(const std::string &sourceFormat, const std::string &targetFormat)
{
  std::vector<FormatConverterPriority> priorities;
  
  if (formatConverters.count(sourceFormat) == 0)
    ;
  else if (formatConverters[sourceFormat].count(targetFormat) == 0)
    ;
  else
    {
      for(const auto &it:formatConverters[sourceFormat][targetFormat])
	{
	  FormatConverterPriority priority = it.first;
	  priorities.push_back(priority);
	}
    }
  
  return priorities;
  
}

SoapySDR::ConvertFunction SoapySDR::getConverter(const std::string &sourceFormat, const std::string &targetFormat)
{
  if (formatConverters.count(sourceFormat) == 0)
    throw std::invalid_argument("unsupported source format: " + sourceFormat + "."); 
  
  if (formatConverters[sourceFormat].count(targetFormat) == 0)
    throw std::invalid_argument("unsupported target format: " + targetFormat + "."); 

  return formatConverters[sourceFormat][targetFormat].rbegin()->second;
}

SoapySDR::ConvertFunction SoapySDR::getConverter(const std::string &sourceFormat, const std::string &targetFormat, FormatConverterPriority &priority)
{
  if (formatConverters.count(sourceFormat) == 0)
    throw std::invalid_argument("unsupported source format: " + sourceFormat + "."); 
  
  if (formatConverters[sourceFormat].count(targetFormat) == 0)
    throw std::invalid_argument("unsupported target format: " + targetFormat + "."); 

  if (formatConverters[sourceFormat][targetFormat].count(priority) == 0)
    throw std::invalid_argument("unsupported target priority: " + std::to_string(priority) + "."); 

  return formatConverters[sourceFormat][targetFormat][priority];
}
