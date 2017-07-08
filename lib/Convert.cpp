// Copyright (c) 2017-2017 Coburn Wightman
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Convert.hpp>

SoapySDR::SourceFormatConverters Converters;

bool SoapySDR::registerConverter(const std::string &sourceFormat, const std::string &targetFormat, SoapySDR::ConvertFunction converterFunction)
{
  if (Converters.count(sourceFormat) == 0)
    {
      SoapySDR::FormatConverters converters;
      converters[targetFormat] = converterFunction;
      Converters[sourceFormat] = converters;
    }
  else
    { 
      SoapySDR::FormatConverters converters = Converters[sourceFormat];
      if (converters.count(targetFormat) != 0){
	throw std::invalid_argument("conversion already registered: " + sourceFormat + " to " + targetFormat + ".");
      }
      else{
	converters[targetFormat] = converterFunction;
      }
    }
  
  return true;
}

std::vector<std::string> SoapySDR::convertTargetFormats(const std::string &sourceFormat)
{
  std::vector<std::string> targets;

  if (Converters.count(sourceFormat) == 0)
    return targets;

  SoapySDR::FormatConverters converters = Converters[sourceFormat];

  for(SoapySDR::FormatConverters::iterator it = converters.begin() ; it != converters.end(); ++it)
    {
      targets.push_back(it->first);
    }
  
  return targets;
}

std::vector<std::string> SoapySDR::convertSourceFormats(const std::string &targetFormat)
{
  std::vector<std::string> sources;

  for(SoapySDR::SourceFormatConverters::iterator it = Converters.begin() ; it != Converters.end(); ++it)
    {
      SoapySDR::FormatConverters converters = Converters[it->first];
      if (converters.count(targetFormat) > 0)
	sources.push_back(it->first);
    }
  
  return sources;
}

SoapySDR::ConvertFunction SoapySDR::getConverter(const std::string &sourceFormat, const std::string &targetFormat)
{
  if (Converters.count(sourceFormat) == 0)
    throw std::invalid_argument("unsupported source format: " + sourceFormat + "."); 
  
  SoapySDR::FormatConverters converters = Converters[sourceFormat];
  if (converters.count(targetFormat) == 0)
    throw std::invalid_argument("unsupported target format: " + targetFormat + "."); 

  return converters[targetFormat];
}
