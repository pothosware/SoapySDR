///
/// \file SoapySDR/Types.h
///
/// Misc data type definitions used in the API.
///
/// \copyright
/// Copyright (c) 2014-2017 Josh Blum
/// SPDX-License-Identifier: BSL-1.0
///

#pragma once
#include <SoapySDR/Config.h>
#include <stddef.h> //size_t

#ifdef __cplusplus
extern "C" {
#endif

//! Definition for a min/max numeric range
typedef struct
{
    double minimum;
    double maximum;
    double step;
} SoapySDRRange;

//! Definition for a key/value string map
typedef struct
{
    size_t size;
    char **keys;
    char **vals;
} SoapySDRKwargs;

/*!
 * Convert a markup string to a key-value map.
 * The markup format is: "key0=value0, key1=value1"
 */
SOAPY_SDR_API SoapySDRKwargs SoapySDRKwargs_fromString(const char *markup);

/*!
 * Convert a key-value map to a markup string.
 * The markup format is: "key0=value0, key1=value1"
 */
SOAPY_SDR_API char *SoapySDRKwargs_toString(const SoapySDRKwargs *args);

//! Possible data types for argument info
typedef enum
{
    SOAPY_SDR_ARG_INFO_BOOL,
    SOAPY_SDR_ARG_INFO_INT,
    SOAPY_SDR_ARG_INFO_FLOAT,
    SOAPY_SDR_ARG_INFO_STRING
} SoapySDRArgInfoType;

//! Definition for argument info
typedef struct
{
    //! The key used to identify the argument (required)
    char *key;

    /*!
     * The default value of the argument when not specified (required)
     * Numbers should use standard floating point and integer formats.
     * Boolean values should be represented as "true" and  "false".
     */
    char *value;

    //! The displayable name of the argument (optional, use key if empty)
    char *name;

    //! A brief description about the argument (optional)
    char *description;

    //! The units of the argument: dB, Hz, etc (optional)
    char *units;

    //! The data type of the argument (required)
    SoapySDRArgInfoType type;

    /*!
     * The range of possible numeric values (optional)
     * When specified, the argument should be restricted to this range.
     * The range is only applicable to numeric argument types.
     */
    SoapySDRRange range;

    //! The size of the options set, or 0 when not used.
    size_t numOptions;

    /*!
     * A discrete list of possible values (optional)
     * When specified, the argument should be restricted to this options set.
     */
    char **options;

    /*!
     * A discrete list of displayable names for the enumerated options (optional)
     * When not specified, the option value itself can be used as a display name.
     */
    char **optionNames;

} SoapySDRArgInfo;

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

/*!
 * Clear a list of kwargs structures.
 * This frees all the underlying memory and clears the members.
 */
SOAPY_SDR_API void SoapySDRKwargsList_clear(SoapySDRKwargs *args, const size_t length);

/*!
 * Clear the contents of a argument info structure.
 * This frees all the underlying memory and clears the members.
 */
SOAPY_SDR_API void SoapySDRArgInfo_clear(SoapySDRArgInfo *info);

/*!
 * Clear a list of argument info structures.
 * This frees all the underlying memory and clears the members.
 */
SOAPY_SDR_API void SoapySDRArgInfoList_clear(SoapySDRArgInfo *info, const size_t length);

#ifdef __cplusplus
}
#endif
