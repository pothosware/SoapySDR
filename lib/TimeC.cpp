// Copyright (c) 2015-2015 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Time.h>
#include <cmath>

extern "C" {

#if defined(_MSC_VER) && (_MSC_VER < 1800)

static inline long long int llround(const double x)
{
    return (long long int)((x < (0.0)) ? (x - (0.5)) : (x + (0.5)));
}

#endif

long long SoapySDR_ticksToTimeNs(const long long ticks, const double rate)
{
    const long long ratell = (long long)(rate);
    const long long full = (long long)(ticks/ratell);
    const long long err = ticks - (full*ratell);
    const double part = full*(rate - ratell);
    const double frac = ((err - part)*1000000000)/rate;
    return (full*1000000000) + llround(frac);
}

long long SoapySDR_timeNsToTicks(const long long timeNs, const double rate)
{
    const long long ratell = (long long)(rate);
    const long long full = (long long)(timeNs/1000000000);
    const long long err = timeNs - (full*1000000000);
    const double part = full*(rate - ratell);
    const double frac = part + ((err*rate)/1000000000);
    return (full*ratell) + llround(frac);
}

}
