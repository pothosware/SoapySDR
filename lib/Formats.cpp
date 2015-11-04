// Copyright (c) 2015-2015 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Formats.hpp>

size_t SoapySDR::formatToSize(const std::string &format)
{
    return SoapySDR_formatToSize(format.c_str());
}
