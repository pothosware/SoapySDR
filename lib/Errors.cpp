// Copyright (c) 2015-2015 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Errors.hpp>

const char *SoapySDR::errToStr(const int errorCode)
{
    return SoapySDR_errToStr(errorCode);
}
