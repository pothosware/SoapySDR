///
/// \file converter/DefaultConverters.hpp
///
/// Convert buffers between stream formats.
///
/// \copyright
/// Copyright (c) 2017-2017 Coburn Wightman
/// SPDX-License-Identifier: BSL-1.0
///

#pragma once
//#include <SoapySDR/Convert.hpp>
#include "ConvertPrimatives.hpp"
#include <SoapySDR/ConverterRegistry.hpp>
#include <SoapySDR/Logger.hpp>
#include <SoapySDR/Formats.hpp>
#include <iostream>
#include <cstdlib>
#include <cstring> //memcpy
#include <vector>
#include <map>
#include <string>
#include <stdexcept>

namespace SoapySDR
{
  class SOAPY_SDR_API DefaultConverters
  {
  public:
    DefaultConverters(void);
    ~DefaultConverters(void);

  private:

    static SoapySDR::ConverterRegistry::ConverterFunction genericCF32toCF32(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler);
    static SoapySDR::ConverterRegistry::ConverterFunction genericCS16toCF32(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler);
    static SoapySDR::ConverterRegistry::ConverterFunction genericCU16toCF32(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler);

    static std::vector<SoapySDR::ConverterRegistry> registry;
    
  };
  
}
