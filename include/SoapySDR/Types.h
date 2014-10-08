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
typedef enum {SOAPY_SDR_TX, SOAPY_SDR_RX} SoapySDRDirection;

//! Definition for a list of strings
typedef struct
{
    size_t size;
    char **elems;
} SoapySDRStringList;

//! Append to a list of strings
SOAPY_SDR_API void SoapySDRStringList_append(SoapySDRStringList *list, const char *s);

//! Destroy a list of strings
SOAPY_SDR_API void SoapySDRStringList_destroy(SoapySDRStringList *list);

//! Definition for a list of numbers
typedef struct
{
    size_t size;
    double *elems;
} SoapySDRNumericList;

//! Definition for a min/max numeric range
typedef struct
{
    double minimum;
    double maximum;
} SoapySDRRange;

//! Definition for a list of ranges
typedef struct
{
    size_t size;
    SoapySDRRange *elems;
} SoapySDRRangeList;

//! Definition for a map of strings
typedef struct
{
    size_t size;
    char **keys;
    char **vals;
} SoapySDRKwargs;

//! Definition for a list of kwargs
typedef struct
{
    size_t size;
    SoapySDRKwargs *elems;
} SoapySDRKwargsList;

#ifdef __cplusplus
}
#endif
