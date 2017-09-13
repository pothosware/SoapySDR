// Copyright (c) 2017-2017 Coburn Wightman
// SPDX-License-Identifier: BSL-1.0

#include "convertPrimatives.hpp"
//#include <SoapySDR/Convert.hpp>
#include <SoapySDR/ConverterRegistry.hpp>
#include <iostream>
#include <cstdlib>
#include <cstring> //memcpy

bool fillBuffer(uint8_t*, int, std::string);
bool dumpBuffer(uint8_t*, int, std::string);


SoapySDR::ConverterRegistry::ConverterFunction cvtCF32toCF32(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  size_t elemDepth = 2;

  std::cout << "converting CF32 to CF32" << std::endl;

  if (scaler == 1.0)
    {
      size_t elemSize = 4;
      std::memcpy(dstBuff, srcBuff, numElems*elemSize*elemDepth);
      std::cout << " straight memcpy" << std::endl;
    }
  else
    {
      float sf = float(1.0/scaler);
      float *dst = (float*)dstBuff;
      float *src = (float*)srcBuff;
      for (size_t i = 0; i < numElems*elemDepth; i+=elemDepth)
	{
	  // dst[i] = src[i] * sf;
	  // dst[i+1] = src[i+1] * sf;
	  CF32toCF32(&src[i], &dst[i], sf);
	}
      std::cout << " sample copy with scaler" << std::endl;
    }
  return 0;
}

SoapySDR::ConverterRegistry::ConverterFunction cvtCS16toCF32(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  size_t elemDepth = 2;

  std::cout << "converting CS16 to CF32" << std::endl;

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

SoapySDR::ConverterRegistry::ConverterFunction cvtCS16toCF32hs(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
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

SoapySDR::ConverterRegistry::ConverterFunction cvtCU16toCF32(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  size_t elemDepth = 2;

  std::cout << "converting CU16 to CF32" << std::endl;

  float sf = float(1.0/scaler);
  float *dst = (float*)dstBuff;
  uint16_t *src = (uint16_t*)srcBuff;
  for (size_t i = 0; i < numElems*elemDepth; i+=elemDepth)
    {
      //dst[i] = src[i] - 0x7fff) * scaler;
      CU16toCF32(src, dst, sf);
    }
  std::cout << " sample copy with scaler" << std::endl;
  
  return 0;
}

// ******************************
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

int main(void)
{
  const size_t numElems = 128;
  const size_t elemDepth = 2;
  int32_t devBuffer[numElems * elemDepth];
  int32_t soapyBuffer[numElems * elemDepth];

  std::vector<SoapySDR::ConverterRegistry> converters;

  SoapySDR::ConverterRegistry::FunctionPriority priority;

  priority = SoapySDR::ConverterRegistry::GENERIC;
  std::cout << "registering priority " << std::to_string(priority) << " converters";
  converters.emplace_back(SOAPY_SDR_CF32, SOAPY_SDR_CF32, priority, (SoapySDR::ConverterRegistry::ConverterFunction) cvtCF32toCF32);
  converters.emplace_back(SOAPY_SDR_CS16, SOAPY_SDR_CF32, priority, (SoapySDR::ConverterRegistry::ConverterFunction) cvtCS16toCF32);
  converters.emplace_back(SOAPY_SDR_CU16, SOAPY_SDR_CF32, priority, (SoapySDR::ConverterRegistry::ConverterFunction) cvtCU16toCF32);
  std::cout  << std::endl;
  
  priority = SoapySDR::ConverterRegistry::VECTORIZED;
  std::cout << "registering priority " << std::to_string(priority) << " converters";
  converters.emplace_back(SOAPY_SDR_CF32, SOAPY_SDR_CF32, priority, (SoapySDR::ConverterRegistry::ConverterFunction) cvtCF32toCF32);
  converters.emplace_back(SOAPY_SDR_CS16, SOAPY_SDR_CF32, priority, (SoapySDR::ConverterRegistry::ConverterFunction) cvtCS16toCF32);
  converters.emplace_back(SOAPY_SDR_CU16, SOAPY_SDR_CF32, priority, (SoapySDR::ConverterRegistry::ConverterFunction) cvtCU16toCF32);
  std::cout  << std::endl;

  priority = SoapySDR::ConverterRegistry::CUSTOM;
  std::cout << "registering priority " << std::to_string(priority) << " converters";
  converters.emplace_back(SOAPY_SDR_CF32, SOAPY_SDR_CF32, priority, (SoapySDR::ConverterRegistry::ConverterFunction) cvtCF32toCF32);
  converters.emplace_back(SOAPY_SDR_CS16, SOAPY_SDR_CF32, priority, (SoapySDR::ConverterRegistry::ConverterFunction) cvtCS16toCF32);
  converters.emplace_back(SOAPY_SDR_CU16, SOAPY_SDR_CF32, priority, (SoapySDR::ConverterRegistry::ConverterFunction) cvtCU16toCF32);
  std::cout  << std::endl;

  std::cout << std::endl << "registering a duplicate converters..." << std::endl;
  try
    {
      converters.emplace_back(SOAPY_SDR_CS16, SOAPY_SDR_CF32, priority, (SoapySDR::ConverterRegistry::ConverterFunction) cvtCU16toCF32);
    }
  catch (const std::exception &ex){
    std::cout << " got exception '" << ex.what() << "' (thats good)" << std::endl;
  }
    
  auto myConverter = new SoapySDR::ConverterRegistry();

  std::string sourceFormat = SOAPY_SDR_CS16;
  std::string targetFormat = SOAPY_SDR_CF32;
    
  dumpTargets(sourceFormat);
  dumpSources(targetFormat);

  std::cout << std::endl << "creating bogus " << sourceFormat << " buffer." << std::endl;
  fillBuffer((uint8_t*)devBuffer, numElems, sourceFormat);
  dumpBuffer((uint8_t*)devBuffer, numElems, sourceFormat);

  std::cout << std::endl << "try a registered generic priority conversion..." << std::endl;
  priority = SoapySDR::ConverterRegistry::GENERIC;
  SoapySDR::ConverterRegistry::ConverterFunction converterFunction = myConverter->getFunction(sourceFormat, targetFormat, priority); 
  converterFunction(devBuffer, soapyBuffer, numElems, 2);

  dumpBuffer((uint8_t*)soapyBuffer, numElems, targetFormat);
  
  std::cout << std::endl << "try a registered unspecified priority conversion..." << std::endl;
  priority = SoapySDR::ConverterRegistry::GENERIC;
  converterFunction = myConverter->getFunction(sourceFormat, targetFormat);
  converterFunction(devBuffer, soapyBuffer, numElems, 2);

  dumpBuffer((uint8_t*)soapyBuffer, numElems, targetFormat);
  
  std::cout << std::endl << "try an unregistered conversion..." << std::endl;
  try{
    converterFunction = myConverter->getFunction(SOAPY_SDR_CU16, SOAPY_SDR_CS16, priority);
    converterFunction(devBuffer, soapyBuffer, numElems, 0.1);
  }
  catch (const std::exception &ex){
    std::cout << " got error '" << ex.what() << "' (thats good)" << std::endl;
  }

  std::cout << "unregistering converters";
  while (!converters.empty())
    {
      converters.rbegin()->remove();
      converters.pop_back();
    }
  std::cout  << std::endl;

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
	if (i%16 == 0){
	  std::cout << std::endl << i << " > ";
	} 
	std::cout << ' ' << buff[i] << ":" << buff[i+1] << ", ";   
      }
  }
  else if (elemFormat == SOAPY_SDR_CS32){
    int32_t* buff = (int32_t*)buffer;
    for (size_t i = 0; i < numElems*elemDepth; i+=elemDepth)
      {
	if (i%16 == 0){
	  std::cout << std::endl << i << " > ";
	} 
	std::cout << ' ' << buff[i] << ":" << buff[i+1] << ", ";   
      }
  }
  else if (elemFormat == SOAPY_SDR_CS16){
    int16_t* buff = (int16_t*)buffer;
    for (size_t i = 0; i < numElems*elemDepth; i+=elemDepth)
      {
	if (i%16 == 0){
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
