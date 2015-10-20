// Copyright (c) 2014-2015 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Types.hpp>

SoapySDR::Range::Range(void):
    _min(0.0),
    _max(0.0)
{
    return;
}

SoapySDR::Range::Range(const double minimum, const double maximum):
    _min(minimum),
    _max(maximum)
{
    return;
}

SoapySDR::ArgInfo::ArgInfo(void)
{
    return;
}
