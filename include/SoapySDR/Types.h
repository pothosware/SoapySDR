///
/// \file SoapySDR/Types.h
///
/// Misc data type definitions used in the API.
///
/// \copyright
/// Copyright (c) 2014-2014 Josh Blum
/// SPDX-License-Identifier: BSL-1.0
///

#pragma once
#include <SoapySDR/Config.h>
#include <stddef.h> //size_t

#ifdef __cplusplus
extern "C" {
#endif

//! Direction type: transmit or receive
typedef enum
{
    SOAPY_SDR_TX,
    SOAPY_SDR_RX
} SoapySDRDirection;

//! Definition for a min/max numeric range
typedef struct
{
    double minimum;
    double maximum;
} SoapySDRRange;

//! Definition for a key/value string map
typedef struct
{
    size_t size;
    char **keys;
    char **vals;
} SoapySDRKwargs;

#ifdef __cplusplus
}
#endif
