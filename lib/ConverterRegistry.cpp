// Copyright (c) 2017-2017 Coburn Wightman
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/ConverterRegistry.hpp>
#include <iostream>

SoapySDR::ConverterRegistry::FormatConverters SoapySDR::ConverterRegistry::formatConverters;

SoapySDR::ConverterRegistry::ConverterRegistry(const std::string &sourceFormat, const std::string &targetFormat, const FunctionPriority &priority, ConverterFunction converterFunction)
{
  if (formatConverters[sourceFormat][targetFormat].count(priority) != 0)
    {
      SoapySDR::log(SOAPY_SDR_ERROR, "converter already registered: " + sourceFormat + " to " + targetFormat + " priority " + std::to_string(priority));
      return;
    }
  
  formatConverters[sourceFormat][targetFormat][priority].usageCount = 1;
  formatConverters[sourceFormat][targetFormat][priority].function = converterFunction;
  //SoapySDR::log(SOAPY_SDR_NOTICE, "registered converter: " + sourceFormat + " to " + targetFormat + " priority " + std::to_string(priority));
  _isRegistered = true;

  _sourceFormat = sourceFormat;
  _targetFormat = targetFormat;
  _priority = priority;

  return;
}

SoapySDR::ConverterRegistry::ConverterRegistry(void)
{
  _isRegistered = false;
  return;
}

SoapySDR::ConverterRegistry::ConverterRegistry(const SoapySDR::ConverterRegistry &old)
{
  _sourceFormat = old._sourceFormat;
  _targetFormat = old._targetFormat;
  _priority = old._priority;
  _isRegistered = old._isRegistered;

  if (_isRegistered)
    ++formatConverters[_sourceFormat][_targetFormat][_priority].usageCount;
  
  //SoapySDR::log(SOAPY_SDR_NOTICE, "copied converter: " + _sourceFormat + " to " + _targetFormat + " priority " + std::to_string(_priority));
  return;
}


SoapySDR::ConverterRegistry::~ConverterRegistry(void)
{
  if (_isRegistered)
    {
      --formatConverters[_sourceFormat][_targetFormat][_priority].usageCount;
      //std::cout << "destructor usageCount = " << std::to_string(formatConverters[_sourceFormat][_targetFormat][_priority].usageCount) << std::endl; 
      if(formatConverters[_sourceFormat][_targetFormat][_priority].usageCount == 0)
	{
	  formatConverters[_sourceFormat][_targetFormat].erase(_priority);
	  //SoapySDR::log(SOAPY_SDR_NOTICE, "deleted converter: " + _sourceFormat + " to " + _targetFormat + " priority " + std::to_string(_priority));
	}
    }
  
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
    SoapySDR::log(SOAPY_SDR_WARNING, "unsupported converter source format: " + sourceFormat + " to " + targetFormat); 
  
  else if (formatConverters[sourceFormat].count(targetFormat) == 0)
    SoapySDR::log(SOAPY_SDR_WARNING, "unsupported converter target format: " + sourceFormat + " to " + targetFormat); 

  else if (formatConverters[sourceFormat][targetFormat].size() == 0)
    SoapySDR::log(SOAPY_SDR_ERROR, "no registered converter functions for: " + sourceFormat + " to " + targetFormat); 

  else{
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
      SoapySDR::log(SOAPY_SDR_ERROR, "unsupported converter source format: " + sourceFormat); 
      throw std::invalid_argument("unsupported converter source format: " + sourceFormat); 
    }
  
  if (formatConverters[sourceFormat].count(targetFormat) == 0)
    {
      SoapySDR::log(SOAPY_SDR_ERROR, "unsupported converter target format: " + targetFormat); 
      throw std::invalid_argument("unsupported converter target format: " + targetFormat);
    }

  return formatConverters[sourceFormat][targetFormat].rbegin()->second.function;
}

SoapySDR::ConverterRegistry::ConverterFunction SoapySDR::ConverterRegistry::getFunction(const std::string &sourceFormat, const std::string &targetFormat, const FunctionPriority &priority)
{
  if (formatConverters.count(sourceFormat) == 0)
    {
      SoapySDR::log(SOAPY_SDR_ERROR, "unsupported converter source format: " + sourceFormat);
      throw std::invalid_argument("unsupported converter source format: " + sourceFormat);
    }
  
  if (formatConverters[sourceFormat].count(targetFormat) == 0)
    {
      SoapySDR::log(SOAPY_SDR_ERROR, "unsupported converter target format: " + targetFormat); 
      throw std::invalid_argument("unsupported converter target format: " + targetFormat); 
    }
  
  if (formatConverters[sourceFormat][targetFormat].count(priority) == 0)
    {
      SoapySDR::log(SOAPY_SDR_ERROR, "unsupported converter target priority: " + std::to_string(priority)); 
      throw std::invalid_argument("unsupported converter target priority: " + std::to_string(priority)); 
    }
  
  return formatConverters[sourceFormat][targetFormat][priority].function;
}
