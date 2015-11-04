// Copyright (c) 2015-2015 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Formats.h>
#include <cctype>

extern "C" {

size_t SoapySDR_formatToSize(const char *format)
{
    size_t size = 0;
    size_t isComplex = false;
    char ch = 0;
    while ((ch = *format++) != '\0')
    {
        if (ch == 'C') isComplex = true;
        if (std::isdigit(ch)) size = (size*10) + size_t(ch-'0');
    }
    if (isComplex) size *= 2;
    return size / 8; //bits to bytes
}

} //extern "C"
