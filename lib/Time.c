// Copyright (c) 2015-2015 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Time.h>
#include <math.h>
#include "softfloat.h"

//soft convert a double to floatx80 using a reinterpret cast
static floatx80 double_to_floatx80(const double in STATUS_PARAM)
{
    const float64 *in64 = (const float64*)(&in);
    return float64_to_floatx80(*in64 STATUS_VAR);
}

//soft ldexpl floatx80 using simple increment on exponent
static floatx80 floatx80_ldexp(const floatx80 x, const int exp)
{
    #ifdef CONFIG_SOFTFLOAT
    floatx80 tmp = x;
    tmp.high = (tmp.high & (1 << 15)) | ((tmp.high & 0x7fff) + exp);
    return tmp;
    #else
    return ldexpl(x, exp);
    #endif
}

long long SoapySDR_ticksToTimeNs(const long long ticks, const double rate)
{
    float_status s; float_status *status = &s;
    const floatx80 inx80 = int64_to_floatx80(ticks STATUS_VAR);
    int rateExp = 0; double rateSig = frexp(rate, &rateExp);
    const floatx80 inexpx80 = floatx80_ldexp(inx80, 9-rateExp);
    const floatx80 scalarx80 = double_to_floatx80(rateSig/1953125 STATUS_VAR);
    const floatx80 outx80 = floatx80_div(inexpx80, scalarx80 STATUS_VAR);
    return floatx80_to_int64(outx80 STATUS_VAR);
}

long long SoapySDR_timeNsToTicks(const long long timeNs, const double rate)
{
    float_status s; float_status *status = &s;
    const floatx80 inx80 = int64_to_floatx80(timeNs STATUS_VAR);
    int rateExp = 0; double rateSig = frexp(rate, &rateExp);
    const floatx80 inexpx80 = floatx80_ldexp(inx80, rateExp-9);
    const floatx80 scalarx80 = double_to_floatx80(rateSig/1953125 STATUS_VAR);
    const floatx80 outx80 = floatx80_mul(inexpx80, scalarx80 STATUS_VAR);
    return floatx80_to_int64(outx80 STATUS_VAR);
}
