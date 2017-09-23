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
#include <SoapySDR/Formats.hpp>
#include <cstring> //memcpy
#include <vector>
#include <string>

#include <iostream>
#include <cstdlib>

namespace SoapySDR
{
  class SOAPY_SDR_API DefaultConverters
  {
  public:
    DefaultConverters(void);
    ~DefaultConverters(void);

  private:
    static SoapySDR::ConverterRegistry::ConverterFunctionProto genericCF32toCF32;
    //    static void genericCF32toCF32(const void *, void *, const size_t, const double);
    static void genericCS16toCF32(const void *, void *, const size_t, const double);
    static void genericCU16toCF32(const void *, void *, const size_t, const double);

    static std::vector<SoapySDR::ConverterRegistry> registry;    
  };

}
