///
/// \file SoapySDR/Converters.h
///
/// Convert buffers between stream formats.
///
/// \copyright
/// Copyright (c) 2020 Nicholas Corgan
/// SPDX-License-Identifier: BSL-1.0
///

#pragma once
#include <SoapySDR/Config.h>
#include <SoapySDR/Errors.h>
#include <SoapySDR/Types.h>
#include <SoapySDR/Constants.h>

/*!
 * A typedef for declaring a ConverterFunction to be maintained in the ConverterRegistry.
 * A converter function copies and optionally converts an input buffer of one format into an
 * output buffer of another format.
 * The parameters are (input pointer, output pointer, number of elements, optional scalar)
 */
typedef void (*SoapySDRConverterFunction)(const void *, void *, const size_t, const double);

/*!
 * Allow selection of a converter function with a given source and target format.
 */
typedef enum
{
    //! Usual C for-loops, shifts, multiplies, etc. Min priority.
    SOAPY_SDR_CONVERTER_GENERIC = 0,

    //! Vectorized configurations such as SIMD.
    SOAPY_SDR_CONVERTER_VECTORIZED = 3,

    //! Custom user re-implementation. Max priority.
    SOAPY_SDR_CONVERTER_CUSTOM = 5
} SoapySDRConverterFunctionPriority;

#ifdef __cplusplus
extern "C"
{
#endif

/*!
 * Get a list of existing target formats to which we can convert the specified source from.
 * \param sourceFormat the source format markup string
 * \param [out] length the number of valid target formats
 * \return a list of valid target formats
 */
SOAPY_SDR_API char **SoapySDRConverter_listTargetFormats(const char *sourceFormat, size_t *length);

/*!
 * Get a list of existing source formats to which we can convert the specified target from.
 * \param targetFormat the target format markup string
 * \param [out] length the number of valid source formats
 * \return a list of valid source formats
 */
SOAPY_SDR_API char **SoapySDRConverter_listSourceFormats(const char *targetFormat, size_t *length);

/*!
 * Get a list of available converter priorities for a given source and target format.
 * \param sourceFormat the source format markup string
 * \param targetFormat the target format markup string
 * \param [out] length the number of priorities
 * \return a list of priorities or nullptr if none are found
 */
SOAPY_SDR_API SoapySDRConverterFunctionPriority *SoapySDRConverter_listPriorities(const char *sourceFormat, const char *targetFormat, size_t *length);

/*!
 * Get a converter between a source and target format with the highest available priority.
 * \param sourceFormat the source format markup string
 * \param targetFormat the target format markup string
 * \return a conversion function pointer or nullptr if none are found
 */
SOAPY_SDR_API SoapySDRConverterFunction SoapySDRConverter_getFunction(const char *sourceFormat, const char *targetFormat);

/*!
 * Get a converter between a source and target format with a given priority.
 * \param sourceFormat the source format markup string
 * \param targetFormat the target format markup string
 * \return a conversion function pointer or nullptr if none are found
 */
SOAPY_SDR_API SoapySDRConverterFunction SoapySDRConverter_getFunctionWithPriority(const char *sourceFormat, const char *targetFormat, const SoapySDRConverterFunctionPriority priority);

/*!
 * Get a list of known source formats in the registry.
 * \param [out] length the number of known source formats
 * \return a list of known source formats
 */
SOAPY_SDR_API char **SoapySDRConverter_listAvailableSourceFormats(size_t *length);

#ifdef __cplusplus
}
#endif
