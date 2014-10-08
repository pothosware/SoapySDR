///
/// \file SoapySDR/Device.h
///
/// Interface definition for Soapy SDR devices.
///
/// \copyright
/// Copyright (c) 2014-2014 Josh Blum
/// SPDX-License-Identifier: BSL-1.0
///

#pragma once
#include <SoapySDR/Config.h>
#include <SoapySDR/Types.h>
#include <SoapySDR/Flags.h>

#ifdef __cplusplus
extern "C" {
#endif

//! Forward declaration of device
struct SoapySDRDevice;

/*!
 * Enumerate a list of available devices on the system.
 * \param args device construction key/value argument filters
 * \param [out] length the number of elements in the result.
 * \return a list of arguments strings, each unique to a device
 */
SOAPY_SDR_API SoapySDRKwargs *SoapySDRDevice_enumerate(const SoapySDRKwargs *args, size_t *length);

/*!
 * Make a new Device object given device construction args.
 * The device pointer will be stored in a table so subsequent calls
 * with the same arguments will produce the same device.
 * For every call to make, there should be a matched call to unmake.
 * \param args device construction key/value argument map
 * \return a pointer to a new Device object
 */
SOAPY_SDR_API SoapySDRDevice *SoapySDRDevice_make(const SoapySDRKwargs *args);

/*!
 * Unmake or release a device object handle.
 * \param device a pointer to a device object
 */
SOAPY_SDR_API void SoapySDRDevice_unmake(SoapySDRDevice *device);

#ifdef __cplusplus
}
#endif
