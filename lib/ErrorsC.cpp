// Copyright (c) 2015-2015 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Errors.h>

extern "C" {

const char *SoapySDR_errToStr(const int errorCode)
{
    switch(errorCode)
    {
    case SOAPY_SDR_TIMEOUT:         return "TIMEOUT";
    case SOAPY_SDR_STREAM_ERROR:    return "STREAM_ERROR";
    case SOAPY_SDR_CORRUPTION:      return "CORRUPTION";
    case SOAPY_SDR_OVERFLOW:        return "OVERFLOW";
    case SOAPY_SDR_NOT_SUPPORTED:   return "NOT_SUPPORTED";
    case SOAPY_SDR_TIME_ERROR:      return "TIME_ERROR";
    case SOAPY_SDR_UNDERFLOW:       return "UNDERFLOW";
    default: break;
    }
    return "UNKNOWN";
}

} //extern "C"
