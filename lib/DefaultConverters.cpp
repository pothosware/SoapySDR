// Copyright (c) 2017-2017 Coburn Wightman
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/ConverterPrimatives.hpp>
#include <SoapySDR/ConverterRegistry.hpp>
#include <SoapySDR/Formats.hpp>
#include <cstring> //memcpy
#include <string>

#include <iostream>
#include <cstdlib>

void genericCF32toCF32(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
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
}

void genericCS16toCF32(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
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
  
}

void genericCU16toCF32(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
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
}

static SoapySDR::ConverterRegistry registerGenericCF32toCF32(SOAPY_SDR_CF32, SOAPY_SDR_CF32, SoapySDR::ConverterRegistry::GENERIC, &genericCF32toCF32);
static SoapySDR::ConverterRegistry registerGenericCS16toCF32(SOAPY_SDR_CS16, SOAPY_SDR_CF32, SoapySDR::ConverterRegistry::GENERIC, &genericCS16toCF32);
static SoapySDR::ConverterRegistry registerGenericCU16toCF32(SOAPY_SDR_CU16, SOAPY_SDR_CF32, SoapySDR::ConverterRegistry::GENERIC, &genericCU16toCF32);

