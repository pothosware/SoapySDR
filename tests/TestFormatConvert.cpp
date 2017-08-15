// Copyright (c) 2015-2015 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Convert.hpp>
#include <iostream>
#include <cstdlib>


SoapySDR::ConvertFunction CF32toCF32(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
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
      float *dst = (float*)dstBuff;
      float *src = (float*)srcBuff;
      for (size_t i = 0; i < numElems*elemDepth; i++)
	{
	  dst[i] = src[i] * scaler;
	}
      std::cout << " sample copy with scaler" << std::endl;
    }
  return 0;
}

SoapySDR::ConvertFunction CS16toCF32(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  size_t elemDepth = 2;

  std::cout << "converting CS16 to CF32" << std::endl;

  float *dst = (float*)dstBuff;
  int16_t *src = (int16_t*)srcBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = src[i] * scaler;
    }
  std::cout << " sample copy with scaler" << std::endl;
  
  return 0;
}

SoapySDR::ConvertFunction CU16toCF32(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  size_t elemDepth = 2;

  std::cout << "converting CU16 to CF32" << std::endl;

  float *dst = (float*)dstBuff;
  uint16_t *src = (uint16_t*)srcBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = (src[i] - 0x7fff) * scaler;
    }
  std::cout << " sample copy with scaler" << std::endl;
  
  return 0;
}

// ******************************

int main(void)
{
  size_t numElems = 128;
  size_t elemDepth = 2;
  int32_t inElems[numElems * elemDepth];
  int32_t outElems[numElems * elemDepth];

  // todo
  // fill outbuff
  // clear inbuff
  
  std::cout << "registering converters..." << std::endl;
  SoapySDR::registerConverter(SOAPY_SDR_CF32, SOAPY_SDR_CF32, (SoapySDR::ConvertFunction) CF32toCF32);
  SoapySDR::registerConverter(SOAPY_SDR_CS16, SOAPY_SDR_CF32, (SoapySDR::ConvertFunction) CS16toCF32);
  SoapySDR::registerConverter(SOAPY_SDR_CU16, SOAPY_SDR_CF32, (SoapySDR::ConvertFunction) CU16toCF32);

  try{
    SoapySDR::registerConverter(SOAPY_SDR_CU16, SOAPY_SDR_CF32, (SoapySDR::ConvertFunction) CU16toCF32);
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

  SoapySDR::ConvertFunction convert;

  std::cout << std::endl << "try a registered conversion..." << std::endl;
  convert = SoapySDR::getConverter(SOAPY_SDR_CF32, formats[0]);
  convert(outElems, inElems, numElems, 0.1);

  std::cout << std::endl << "try an unregistered conversion..." << std::endl;
  convert = SoapySDR::getConverter(SOAPY_SDR_CU16, SOAPY_SDR_CS16);
  convert(outElems, inElems, numElems, 0.1);

  // todo
  // dump input elems
  // dump output elems
  
  std::cout << "DONE!" << std::endl;
  return EXIT_SUCCESS;
}
