// Copyright (c) 2017-2017 Coburn Wightman
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/ConverterRegistry.hpp>

SoapySDR::ConverterRegistry::FormatConverters SoapySDR::ConverterRegistry::formatConverters;

SoapySDR::ConverterRegistry::ConverterRegistry(const std::string &sourceFormat, const std::string &targetFormat, const FunctionPriority &priority, ConverterFunction converterFunction)
{
  _isRegistered = false;
  
  if (formatConverters.count(sourceFormat) == 0)
    ;
  else if (formatConverters[sourceFormat].count(targetFormat) == 0)
    ;
  else if (formatConverters[sourceFormat][targetFormat].count(priority) != 0)
    {
      SoapySDR::logf(SOAPY_SDR_ERROR, "SoapySDR::ConverterRegistry(%s, %s, %s) duplicate registration", sourceFormat.c_str(), targetFormat.c_str(), std::to_string(priority).c_str());
      return;
    }
  
  formatConverters[sourceFormat][targetFormat][priority] = converterFunction;

  _isRegistered = true;
  _sourceFormat = sourceFormat;
  _targetFormat = targetFormat;
  _priority = priority;

  return;
}

SoapySDR::ConverterRegistry::~ConverterRegistry(void)
{
  if (_isRegistered)
    formatConverters[_sourceFormat][_targetFormat].erase(_priority);

  return;
}

std::vector<std::string> SoapySDR::ConverterRegistry::listTargetFormats(const std::string &sourceFormat)
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

std::vector<std::string> SoapySDR::ConverterRegistry::listSourceFormats(const std::string &targetFormat)
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

std::vector<SoapySDR::ConverterRegistry::FunctionPriority> SoapySDR::ConverterRegistry::listPriorities(const std::string &sourceFormat, const std::string &targetFormat)
{
  std::vector<FunctionPriority> priorities;
  
  if (formatConverters.count(sourceFormat) == 0)
    ;
  else if (formatConverters[sourceFormat].count(targetFormat) == 0)
    ;
  else if (formatConverters[sourceFormat][targetFormat].size() == 0)
    ;
  else
    {
      for(const auto &it:formatConverters[sourceFormat][targetFormat])
	{
	  FunctionPriority priority = it.first;
	  priorities.push_back(priority);
	}
    }
  
  return priorities;
  
}

SoapySDR::ConverterRegistry::ConverterFunction SoapySDR::ConverterRegistry::getFunction(const std::string &sourceFormat, const std::string &targetFormat)
{
  if (formatConverters.count(sourceFormat) == 0)
    {
      throw std::invalid_argument("no registered converters for source format: " + sourceFormat); 
    }
  
  if (formatConverters[sourceFormat].count(targetFormat) == 0)
    {
      throw std::invalid_argument("no registered converters for target format: " + targetFormat);
    }

  if (formatConverters[sourceFormat][targetFormat].size() == 0)
    {
      throw std::invalid_argument("no registered converters of any priority: " + targetFormat);
    }

  return formatConverters[sourceFormat][targetFormat].rbegin()->second;
}

SoapySDR::ConverterRegistry::ConverterFunction SoapySDR::ConverterRegistry::getFunction(const std::string &sourceFormat, const std::string &targetFormat, const FunctionPriority &priority)
{
  if (formatConverters.count(sourceFormat) == 0)
    {
      throw std::invalid_argument("no registered converters for source format: " + sourceFormat);
    }
  
  if (formatConverters[sourceFormat].count(targetFormat) == 0)
    {
      throw std::invalid_argument("no registered converters for target format: " + targetFormat); 
    }
  
  if (formatConverters[sourceFormat][targetFormat].count(priority) == 0)
    {
      throw std::invalid_argument("no registered converters for target priority: " + std::to_string(priority)); 
    }
  
  return formatConverters[sourceFormat][targetFormat][priority];
}
