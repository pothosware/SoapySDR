///
/// \file SoapySDR/Formats.h
///
/// Format strings used in the stream API.
///
/// \copyright
/// Copyright (c) 2015-2015 Josh Blum
/// SPDX-License-Identifier: BSL-1.0
///

#pragma once
#include <SoapySDR/Config.h>
#include <stddef.h> //size_t

//! Complex 64-bit floats (complex double)
#define SOAPY_SDR_CF64 "CF64"

//! Complex 32-bit floats (complex float)
#define SOAPY_SDR_CF32 "CF32"

//! Complex signed 32-bit integers (complex int32)
#define SOAPY_SDR_CS32 "CS32"

//! Complex unsigned 32-bit integers (complex uint32)
#define SOAPY_SDR_CU32 "CU32"

//! Complex signed 16-bit integers (complex int16)
#define SOAPY_SDR_CS16 "CS16"

//! Complex unsigned 16-bit integers (complex uint16)
#define SOAPY_SDR_CU16 "CU16"

//! Complex signed 12-bit integers (3 bytes)
#define SOAPY_SDR_CS12 "CS12"

//! Complex unsigned 12-bit integers (3 bytes)
#define SOAPY_SDR_CU12 "CU12"

//! Complex signed 8-bit integers (complex int8)
#define SOAPY_SDR_CS8 "CS8"

//! Complex unsigned 8-bit integers (complex uint8)
#define SOAPY_SDR_CU8 "CU8"

//! Complex signed 4-bit integers (1 byte)
#define SOAPY_SDR_CS4 "CS4"

//! Complex unsigned 4-bit integers (1 byte)
#define SOAPY_SDR_CU4 "CU4"

//! Real 64-bit floats (double)
#define SOAPY_SDR_F64 "F64"

//! Real 32-bit floats (float)
#define SOAPY_SDR_F32 "F32"

//! Real signed 32-bit integers (int32)
#define SOAPY_SDR_S32 "S32"

//! Real unsigned 32-bit integers (uint32)
#define SOAPY_SDR_U32 "U32"

//! Real signed 16-bit integers (int16)
#define SOAPY_SDR_S16 "S16"

//! Real unsigned 16-bit integers (uint16)
#define SOAPY_SDR_U16 "U16"

//! Real signed 8-bit integers (int8)
#define SOAPY_SDR_S8 "S8"

//! Real unsigned 8-bit integers (uint8)
#define SOAPY_SDR_U8 "U8"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Get the size of a single element in the specified format.
 * \param format a supported format string
 * \return the size of an element in bytes
 */
SOAPY_SDR_API size_t SoapySDR_formatToSize(const char *format);

#ifdef __cplusplus
}
#endif
