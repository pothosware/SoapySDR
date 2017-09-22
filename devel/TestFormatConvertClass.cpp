// Copyright (c) 2017-2017 Coburn Wightman
// SPDX-License-Identifier: BSL-1.0

#include "../convert/ConvertPrimatives.hpp"
#include <SoapySDR/Version.hpp>
#include <SoapySDR/Modules.hpp>
#include <SoapySDR/Registry.hpp>
#include <SoapySDR/ConverterRegistry.hpp>
#include <iostream>
#include <cstdlib>
#include <cstring> //memcpy

bool fillBuffer(uint8_t*, int, std::string);
bool dumpBuffer(uint8_t*, int, std::string);

SoapySDR::ConverterRegistry::ConverterFunction customCS16toCF32hs(const void *srcBuff, void *dstBuff, const size_t numElems,
						     const double scaler)
{
  size_t elemDepth = 2;

  std::cout << "converting CS16 to CF32 (hot shot)" << std::endl;

  float sf = float(1.0/scaler);
  float *dst = (float*)dstBuff;
  int16_t *src = (int16_t*)srcBuff;
  for (size_t i = 0; i < numElems*elemDepth; i+=elemDepth)
    {
      CS16toCF32(&src[i], &dst[i], sf);
    }
  std::cout << " sample copy with scaler" << std::endl;

  return 0;
}

void dumpTargets(std::string sourceFormat)
{
  auto myConverter = new SoapySDR::ConverterRegistry();
  std::cout << "get supported target formats for " << sourceFormat << std::endl;
  std::vector<std::string> targets = myConverter->listTargetFormats(sourceFormat);
  for(const auto target:targets)
    {
      std::cout << " " << sourceFormat << " > " << target;
      std::vector<SoapySDR::ConverterRegistry::FunctionPriority> priorities = myConverter->listPriorities(sourceFormat, target);
      for(const auto priority:priorities)
  	{
  	  std::cout << " " << priority;
  	}
      std::cout << std::endl;
    }
}

void dumpSources(std::string targetFormat)
{
  auto myConverter = new SoapySDR::ConverterRegistry();
  std::cout << "get supported source formats for " << targetFormat << std::endl;
  std::vector<std::string> sources = myConverter->listSourceFormats(targetFormat);
  for(const auto source:sources)
    {
      std::cout << " " << source << " > " << targetFormat;
      std::vector<SoapySDR::ConverterRegistry::FunctionPriority> priorities = myConverter->listPriorities(source, targetFormat);
      for(const auto priority:priorities)
  	{
  	  std::cout << " " << priority;
  	}
      std::cout << std::endl;
    }
  
}

static int printInfo(void)
{
  std::cout << "API Version: v" << SoapySDR::getAPIVersion() << std::endl;
  std::cout << "ABI Version: v" << SoapySDR::getABIVersion() << std::endl;
  std::cout << "Install root: " << SoapySDR::getRootPath() << std::endl;

  std::vector<std::string> modules = SoapySDR::listModules();
  for (size_t i = 0; i < modules.size(); i++)
    {
      std::cout << "Module found: " << modules[i] << std::endl;
    }
  if (modules.empty()) std::cout << "No modules found!" << std::endl;

  std::cout << "Loading modules... " << std::flush;
  SoapySDR::loadModules();

  // std::cout << "Unloading modules... " << std::flush;
  // for (size_t i = 0; i < modules.size(); i++)
  //   {
  //     SoapySDR::unloadModule(modules[i]);
  //     std::cout << "Module " << modules[i] << " unloaded."  << std::endl;
  //   }
  
  std::cout << "Available factories...";
  const SoapySDR::FindFunctions factories = SoapySDR::Registry::listFindFunctions();
  for (SoapySDR::FindFunctions::const_iterator it = factories.begin(); it != factories.end(); ++it)
    {
      std::cout << it->first << ", ";
    }
  if (factories.empty()) std::cout << "No factories found!" << std::endl;
  else std::cout << std::endl;
  
  std::cout << std::endl;
  return EXIT_SUCCESS;
}

int main(void)
{
  const size_t numElems = 128;
  const size_t elemDepth = 2;
  int32_t devBuffer[numElems * elemDepth];
  int32_t soapyBuffer[numElems * elemDepth];

  int stat = printInfo();
  
  auto myConverter = new SoapySDR::ConverterRegistry();

  std::string sourceFormat = SOAPY_SDR_CS16;
  std::string targetFormat = SOAPY_SDR_CF32;
  SoapySDR::ConverterRegistry::FunctionPriority priority;
    
  dumpTargets(sourceFormat);
  dumpSources(targetFormat);

  std::cout << std::endl << "creating bogus " << sourceFormat << " buffer." << std::endl;
  fillBuffer((uint8_t*)devBuffer, numElems, sourceFormat);
  dumpBuffer((uint8_t*)devBuffer, numElems, sourceFormat);

  std::cout << std::endl << "try a registered GENERIC priority conversion..." << std::endl;
  priority = SoapySDR::ConverterRegistry::GENERIC;
  SoapySDR::ConverterRegistry::ConverterFunction converterFunction = myConverter->getFunction(sourceFormat, targetFormat, priority);
  std::cout << "converting " << std::endl;
  converterFunction(devBuffer, soapyBuffer, numElems, 2);

  dumpBuffer((uint8_t*)soapyBuffer, numElems, targetFormat);
  
  std::cout << std::endl << "try a registered default priority conversion..." << std::endl;
  converterFunction = myConverter->getFunction(sourceFormat, targetFormat);
  converterFunction(devBuffer, soapyBuffer, 8, 2);

  dumpBuffer((uint8_t*)soapyBuffer, 8, targetFormat);
  
  sourceFormat = SOAPY_SDR_CS16;
  targetFormat = SOAPY_SDR_CF32;
  priority = SoapySDR::ConverterRegistry::CUSTOM;

  std::cout << std::endl << "try an unregistered CUSTOM conversion..." << std::endl;
  try{
    converterFunction = myConverter->getFunction(sourceFormat, targetFormat, priority);
    std::cout << " no exception. (not good)" << std::endl;
  }
  catch (const std::exception &ex){
    std::cout << " got exception '" << ex.what() << "' (thats good)" << std::endl;
  }

  std::cout << std::endl << "register a CUSTOM conversion..." << std::endl;  
  auto myCustomConverter = new SoapySDR::ConverterRegistry(sourceFormat, targetFormat, priority, (SoapySDR::ConverterRegistry::ConverterFunction) customCS16toCF32hs);

  dumpTargets(sourceFormat);
  dumpSources(targetFormat);
  
  std::cout << std::endl << "try a registered CUSTOM conversion..." << std::endl;
  converterFunction = myConverter->getFunction(sourceFormat, targetFormat, priority);
  converterFunction(devBuffer, soapyBuffer, 8, 2);
  dumpBuffer((uint8_t*)soapyBuffer, 8, targetFormat);

  std::cout << std::endl << "removing CUSTOM conversion..." << std::endl;
  //  myCustomConverter->remove();
  delete(myCustomConverter);
  std::cout << std::endl << "try a deleted CUSTOM conversion..." << std::endl;
  try{
    converterFunction = myConverter->getFunction(sourceFormat, targetFormat, priority);
    std::cout << " no exception. (not good)" << std::endl;
  }
  catch (const std::exception &ex){
    std::cout << " got exception '" << ex.what() << "'. (thats good)" << std::endl;
  }

  std::cout << std::endl << "Unloading modules... " << std::endl << std::flush;
  std::vector<std::string> modules = SoapySDR::listModules();
  for (size_t i = 0; i < modules.size(); i++)
    {
      SoapySDR::unloadModule(modules[i]);
      std::cout << "Module " << modules[i] << " unloaded."  << std::endl;
    }
    
  std::cout << "DONE!" << std::endl;
  return EXIT_SUCCESS;
}

bool fillBuffer(uint8_t* buffer, int numElems, std::string elemFormat){
  size_t elemDepth = 2;

  if (elemFormat == SOAPY_SDR_CF32){
    float* buff = (float*)buffer;
    for (size_t i = 0; i < numElems*elemDepth; i+=elemDepth)
      {
	buff[i] = (float)i;
	buff[i+1] = (float)i+1;
      }
  }
  else if (elemFormat == SOAPY_SDR_CS32){
    int32_t* buff = (int32_t*)buffer;
    for (size_t i = 0; i < numElems*elemDepth; i+=elemDepth)
      {
	buff[i] = i;
	buff[i+1] = i+1;
      }
  }
  else if (elemFormat == SOAPY_SDR_CS16){
    int16_t* buff = (int16_t*)buffer;
    for (size_t i = 0; i < numElems*elemDepth; i+=elemDepth)
      {
	buff[i] = i;
	buff[i+1] = i+1;
      }
  }
  else
    std::cout << "unrecognized elem format" << std::endl;

  return true;
}

bool dumpBuffer(uint8_t* buffer, int numElems, std::string elemFormat){
  size_t elemDepth = 2;

  if (elemFormat == SOAPY_SDR_CF32){
    float* buff = (float*)buffer;
    for (size_t i = 0; i < numElems*elemDepth; i+=elemDepth)
      {
	if (i%16 == 0 and i!=0){
	  std::cout << std::endl << i << " > ";
	} 
	std::cout << ' ' << buff[i] << ":" << buff[i+1] << ", ";   
      }
  }
  else if (elemFormat == SOAPY_SDR_CS32){
    int32_t* buff = (int32_t*)buffer;
    for (size_t i = 0; i < numElems*elemDepth; i+=elemDepth)
      {
	if (i%16 == 0 and i!=0){
	  std::cout << std::endl << i << " > ";
	} 
	std::cout << ' ' << buff[i] << ":" << buff[i+1] << ", ";   
      }
  }
  else if (elemFormat == SOAPY_SDR_CS16){
    int16_t* buff = (int16_t*)buffer;
    for (size_t i = 0; i < numElems*elemDepth; i+=elemDepth)
      {
	if (i%16 == 0 and i!=0){
	  std::cout << std::endl << i << " > ";
	} 
	std::cout << ' ' << buff[i] << ":" << buff[i+1] << ", ";   
      }
  }
  else{
    std::cout << "unrecognized elem format" << std::endl;
    return false;
  }

  std::cout << std::endl;
  return true;
}
