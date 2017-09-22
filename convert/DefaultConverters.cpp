// Copyright (c) 2017-2017 Coburn Wightman
// SPDX-License-Identifier: BSL-1.0

#include "DefaultConverters.hpp"


SoapySDR::ConverterRegistry::ConverterFunction SoapySDR::DefaultConverters::genericCF32toCF32(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
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

SoapySDR::ConverterRegistry::ConverterFunction SoapySDR::DefaultConverters::genericCS16toCF32(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
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

SoapySDR::ConverterRegistry::ConverterFunction SoapySDR::DefaultConverters::genericCU16toCF32(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
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

std::vector<SoapySDR::ConverterRegistry> SoapySDR::DefaultConverters::registry;

SoapySDR::DefaultConverters::DefaultConverters(void)
{
  SoapySDR::ConverterRegistry::FunctionPriority priority;

  std::cout << "registering default converters"  << std::endl;
  
  priority = SoapySDR::ConverterRegistry::GENERIC;
  SoapySDR::DefaultConverters::registry.emplace_back(SOAPY_SDR_CF32, SOAPY_SDR_CF32, priority, (SoapySDR::ConverterRegistry::ConverterFunction) SoapySDR::DefaultConverters::genericCF32toCF32);
  SoapySDR::DefaultConverters::registry.emplace_back(SOAPY_SDR_CS16, SOAPY_SDR_CF32, priority, (SoapySDR::ConverterRegistry::ConverterFunction) SoapySDR::DefaultConverters::genericCS16toCF32);
  SoapySDR::DefaultConverters::registry.emplace_back(SOAPY_SDR_CU16, SOAPY_SDR_CF32, priority, (SoapySDR::ConverterRegistry::ConverterFunction) SoapySDR::DefaultConverters::genericCU16toCF32);
}

SoapySDR::DefaultConverters::~DefaultConverters()
{
  std::cout << "unregistering " << SoapySDR::DefaultConverters::registry.size() << " default converters" << std::endl;
  while (!SoapySDR::DefaultConverters::registry.empty())
    {
      SoapySDR::DefaultConverters::registry.pop_back();
    }
}

static SoapySDR::DefaultConverters* converterRegistry = new SoapySDR::DefaultConverters();

