// Copyright (c) 2015-2015 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include "convertPrimatives.hpp"
#include <SoapySDR/Convert.hpp>
#include <iostream>
#include <cstdlib>
#include <cstring> //memcpy

bool fillBuffer(uint8_t*, int, std::string);
bool dumpBuffer(uint8_t*, int, std::string);


SoapySDR::ConvertFunction cvtCF32toCF32(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
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

SoapySDR::ConvertFunction cvtCS16toCF32(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
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

SoapySDR::ConvertFunction cvtCU16toCF32(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
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

int main(void)
{
  const size_t numElems = 128;
  const size_t elemDepth = 2;
  int32_t devBuffer[numElems * elemDepth];
  int32_t soapyBuffer[numElems * elemDepth];

  std::cout << "registering converters..." << std::endl;
  SoapySDR::registerConverter(SOAPY_SDR_CF32, SOAPY_SDR_CF32, (SoapySDR::ConvertFunction) cvtCF32toCF32);
  SoapySDR::registerConverter(SOAPY_SDR_CS16, SOAPY_SDR_CF32, (SoapySDR::ConvertFunction) cvtCS16toCF32);
  SoapySDR::registerConverter(SOAPY_SDR_CU16, SOAPY_SDR_CF32, (SoapySDR::ConvertFunction) cvtCU16toCF32);

  try{
    SoapySDR::registerConverter(SOAPY_SDR_CU16, SOAPY_SDR_CF32, (SoapySDR::ConvertFunction) cvtCU16toCF32);
  }
  catch (const std::exception &ex){
    std::cout << " got error '" << ex.what() << "' (thats good)" << std::endl;
  }

  std::cout << std::endl << "get supported target formats for " << SOAPY_SDR_CF32 << std::endl;
  std::vector<std::string> formats;
  formats = SoapySDR::convertTargetFormats(SOAPY_SDR_CF32);
  for(std::vector<std::string>::iterator it = formats.begin() ; it != formats.end(); ++it)
    {
      std::cout << " " << *it << std::endl;
    }

  std::cout << "get supported source formats for " << SOAPY_SDR_CF32 << std::endl;
  formats = SoapySDR::convertSourceFormats(SOAPY_SDR_CF32);
  for(std::vector<std::string>::iterator it = formats.begin() ; it != formats.end(); ++it)
    {
      std::cout << " " << *it << std::endl;
    }

  std::string inElemFormat = SOAPY_SDR_CS16;
  std::string outElemFormat = SOAPY_SDR_CF32;
  
  std::cout << std::endl << "try a registered conversion..." << std::endl;
  SoapySDR::ConvertFunction convert = SoapySDR::getConverter(inElemFormat, outElemFormat);

  fillBuffer((uint8_t*)devBuffer, numElems, inElemFormat);
  dumpBuffer((uint8_t*)devBuffer, numElems, inElemFormat);

  convert(devBuffer, soapyBuffer, numElems, 2);
  dumpBuffer((uint8_t*)soapyBuffer, numElems, outElemFormat);
  
  std::cout << std::endl << "try an unregistered conversion..." << std::endl;
  try{
    convert = SoapySDR::getConverter(SOAPY_SDR_CU16, SOAPY_SDR_CS16);
    convert(devBuffer, soapyBuffer, numElems, 0.1);
  }
  catch (const std::exception &ex){
    std::cout << " got error '" << ex.what() << "' (thats good)" << std::endl;
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
