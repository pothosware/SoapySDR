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

/*!
 * Clear the contents of a list of string
 * Convenience call to deal with results that return a string list.
 */
SOAPY_SDR_API void SoapySDRStrings_clear(char ***elems, const size_t length);

/*!
 * Set a key/value pair in a kwargs structure.
 */
SOAPY_SDR_API void SoapySDRKwargs_set(SoapySDRKwargs *args, const char *key, const char *val);

/*!
 * Get a value given a key in a kwargs structure.
 * \return the string or NULL if not found
 */
SOAPY_SDR_API const char *SoapySDRKwargs_get(SoapySDRKwargs *args, const char *key);

/*!
 * Clear the contents of a kwargs structure.
 * This frees all the underlying memory and clears the members.
 */
SOAPY_SDR_API void SoapySDRKwargs_clear(SoapySDRKwargs *args);

#ifdef __cplusplus
}
#endif
