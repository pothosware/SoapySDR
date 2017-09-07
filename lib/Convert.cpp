// Copyright (c) 2017-2017 Coburn Wightman
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Convert.hpp>

SoapySDR::SourceFormatConverters Converters;

bool SoapySDR::registerConverter(const std::string &sourceFormat, const std::string &targetFormat, SoapySDR::ConvertFunction converterFunction)
{
  if (Converters.count(sourceFormat) == 0)
    {
      Converters[sourceFormat][targetFormat] = converterFunction;
    }
  else
    { 
      if (Converters[sourceFormat].count(targetFormat) != 0)
	{
	  throw std::invalid_argument("conversion already registered: " + sourceFormat + " to " + targetFormat + ".");
	}
      else
	{
	  Converters[sourceFormat][targetFormat] = converterFunction;
	}
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

SoapySDR::ConvertFunction SoapySDR::getConverter(const std::string &sourceFormat, const std::string &targetFormat)
{
  if (Converters.count(sourceFormat) == 0)
    throw std::invalid_argument("unsupported source format: " + sourceFormat + "."); 
  
  if (Converters[sourceFormat].count(targetFormat) == 0)
    throw std::invalid_argument("unsupported target format: " + targetFormat + "."); 

  return Converters[sourceFormat][targetFormat];
}
