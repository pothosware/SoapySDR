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
#include <stdbool.h>

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

/*******************************************************************
 * Channels API
 ******************************************************************/

/*!
 * Set the frontend mapping of available DSP units to RF frontends.
 * This mapping controls channel mapping and channel availability.
 * \param device a pointer to a device instance
 * \param dir the channel direction RX or TX
 * \param mapping a vendor-specific mapping string
 */
SOAPY_SDR_API void SoapySDRDevice_setFrontendMapping(SoapySDRDevice *device, const SoapySDRDirection dir, const char *mapping);

/*!
 * Get the mapping configuration string.
 * \param device a pointer to a device instance
 * \param dir the channel direction RX or TX
 * \param the vendor-specific mapping string
 */
SOAPY_SDR_API char *SoapySDRDevice_getFrontendMapping(const SoapySDRDevice *device, const SoapySDRDirection dir);

/*!
 * Get a number of channels given the streaming direction
 * \param device a pointer to a device instance
 * \param dir the channel direction RX or TX
 * \return the number of channels
 */
SOAPY_SDR_API size_t SoapySDRDevice_getNumChannels(const SoapySDRDevice *device, const SoapySDRDirection dir);

/*******************************************************************
 * Stream API
 ******************************************************************/

/*!
 * Initialize a stream given a list of channels and stream arguments.
 * Recommended keys to use in the args dictionary:
 *  - "host" - format of samples passed into read/writStream()
 *  - "wire" - format of the samples between device and host
 * \param device a pointer to a device instance
 * \param dir the channel direction RX or TX
 * \param channels a list of channels for empty for automatic
 * \param numChans the number of elements in the channels array
 * \param args stream args or empty for defaults.
 * \return an opaque pointer to a stream handle
 */
SOAPY_SDR_API void *SoapySDRDevice_setupStream(SoapySDRDevice *device, const SoapySDRDirection dir, const size_t *channels, const size_t numChans, const SoapySDRKwargs *args);

/*!
 * Close an open stream created by setupStream
 * \param device a pointer to a device instance
 * \param handle the opaque pointer to a stream handle
 */
SOAPY_SDR_API void SoapySDRDevice_closeStream(SoapySDRDevice *device, void *handle);

/*!
 * Read elements from a stream for reception.
 * This is a multi-channel call, and buffs should be an array of void *,
 * where each pointer will be filled with data from a different channel.
 *
 * \param device a pointer to a device instance
 * \param handle the opaque pointer to a stream handle
 * \param buffs an array of void* buffers num chans in size
 * \param numElems the number of elements in each buffer
 * \param [out] flags optional flag indicators about the result
 * \param [out] timeNs the buffer's timestamp in nanoseconds
 * \param timeoutUs the timeout in microseconds
 * \return the number of elements read per buffer or error code
 */
SOAPY_SDR_API int SoapySDRDevice_readStream(SoapySDRDevice *device, void *handle, void * const *buffs, const size_t numElems, int *flags, long long *timeNs, const long timeoutUs);

/*!
 * Write elements to a stream for transmission.
 * This is a multi-channel call, and buffs should be an array of void *,
 * where each pointer will be filled with data for a different channel.
 *
 * \param device a pointer to a device instance
 * \param handle the opaque pointer to a stream handle
 * \param buffs an array of void* buffers num chans in size
 * \param numElems the number of elements in each buffer
 * \param [in,out] flags optional input flags and output flags
 * \param timeNs the buffer's timestamp in nanoseconds
 * \param timeoutUs the timeout in microseconds
 * \return the number of elements written per buffer or error
 */
SOAPY_SDR_API int SoapySDRDevice_writeStream(SoapySDRDevice *device, void *handle, const void * const *buffs, const size_t numElems, int *flags, const long long timeNs, const long timeoutUs);

/*******************************************************************
 * Antenna API
 ******************************************************************/

/*!
 * Get a list of available antennas to select on a given chain.
 * \param device a pointer to a device instance
 * \param dir the channel direction RX or TX
 * \param channel an available channel on the device
 * \param [out] length the number of antenna names
 * \return a list of available antenna names
 */
SOAPY_SDR_API char **SoapySDRDevice_listAntennas(const SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel, size_t *length);

/*!
 * Set the selected antenna on a chain.
 * \param device a pointer to a device instance
 * \param dir the channel direction RX or TX
 * \param channel an available channel on the device
 * \param name the name of an available antenna
 */
SOAPY_SDR_API void SoapySDRDevice_setAntenna(SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel, const char *name);

/*!
 * Get the selected antenna on a chain.
 * \param device a pointer to a device instance
 * \param dir the channel direction RX or TX
 * \param channel an available channel on the device
 * \return the name of an available antenna
 */
SOAPY_SDR_API char *SoapySDRDevice_getAntenna(const SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel);

/*******************************************************************
 * Frontend corrections API
 ******************************************************************/

/*!
 * Set the frontend DC offset correction.
 * \param device a pointer to a device instance
 * \param dir the channel direction RX or TX
 * \param channel an available channel on the device
 * \param offsetI the relative correction (1.0 max)
 * \param offsetQ the relative correction (1.0 max)
 */
SOAPY_SDR_API void SoapySDRDevice_setDCOffset(SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel, const double offsetI, const double offsetQ);

/*!
 * Get the frontend DC offset correction.
 * \param device a pointer to a device instance
 * \param dir the channel direction RX or TX
 * \param channel an available channel on the device
 * \param [out] offsetI the relative correction (1.0 max)
 * \param [out] offsetQ the relative correction (1.0 max)
 */
SOAPY_SDR_API void SoapySDRDevice_getDCOffset(const SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel, double *offsetI, double *offsetQ);

/*!
 * Set the frontend IQ balance correction.
 * \param device a pointer to a device instance
 * \param dir the channel direction RX or TX
 * \param channel an available channel on the device
 * \param balanceI the relative correction (1.0 max)
 * \param balanceQ the relative correction (1.0 max)
 */
SOAPY_SDR_API void SoapySDRDevice_setIQBalance(SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel, const double balanceI, const double balanceQ);

/*!
 * Get the frontend IQ balance correction.
 * \param device a pointer to a device instance
 * \param dir the channel direction RX or TX
 * \param channel an available channel on the device
 * \param [out] balanceI the relative correction (1.0 max)
 * \param [out] balanceQ the relative correction (1.0 max)
 */
SOAPY_SDR_API void SoapySDRDevice_getIQBalance(const SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel, double *balanceI, double *balanceQ);

/*******************************************************************
 * Gain API
 ******************************************************************/

/*!
 * List available amplification elements.
 * \param device a pointer to a device instance
 * \param dir the channel direction RX or TX
 * \param channel an available channel
 * \param [out] length the number of gain names
 * \return a list of gain string names
 */
SOAPY_SDR_API char **SoapySDRDevice_listGains(const SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel, size_t *length);

/*!
 * Set the automatic gain mode on the chain.
 * \param device a pointer to a device instance
 * \param dir the channel direction RX or TX
 * \param channel an available channel on the device
 * \param mode true for automatic gain setting
 */
SOAPY_SDR_API void SoapySDRDevice_setGainMode(SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel, const bool mode);

 /*!
 * Get the automatic gain mode on the chain.
 * \param device a pointer to a device instance
 * \param dir the channel direction RX or TX
 * \param channel an available channel on the device
 * \return true for automatic gain setting
 */
SOAPY_SDR_API bool SoapySDRDevice_getGainMode(const SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel);

/*!
 * Set the overall amplification in a chain.
 * The gain will be distributed automatically across available element.
 * \param device a pointer to a device instance
 * \param dir the channel direction RX or TX
 * \param channel an available channel on the device
 * \param name the name of an amplification element
 * \param value the new amplification value in dB
 */
SOAPY_SDR_API void SoapySDRDevice_setGain(SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel, const double value);

/*!
 * Set the value of a amplification element in a chain.
 * \param device a pointer to a device instance
 * \param dir the channel direction RX or TX
 * \param channel an available channel on the device
 * \param name the name of an amplification element
 * \param value the new amplification value in dB
 */
SOAPY_SDR_API void SoapySDRDevice_setGainElement(SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel, const char *name, const double value);

/*!
 * Get the overall value of the gain elements in a chain.
 * \param device a pointer to a device instance
 * \param dir the channel direction RX or TX
 * \param channel an available channel on the device
 * \return the value of the gain in dB
 */
SOAPY_SDR_API double SoapySDRDevice_getGain(const SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel);

/*!
 * Get the value of an individual amplification element in a chain.
 * \param device a pointer to a device instance
 * \param dir the channel direction RX or TX
 * \param channel an available channel on the device
 * \param name the name of an amplification element
 * \return the value of the gain in dB
 */
SOAPY_SDR_API double SoapySDRDevice_getGainElement(const SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel, const char *name);

/*!
 * Get the overall range of possible gain values.
 * \param device a pointer to a device instance
 * \param dir the channel direction RX or TX
 * \param channel an available channel on the device
 * \return a list of gain ranges in dB
 */
SOAPY_SDR_API SoapySDRRange SoapySDRDevice_getGainRange(const SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel);

/*!
 * Get the range of possible gain values for a specific element.
 * \param device a pointer to a device instance
 * \param dir the channel direction RX or TX
 * \param channel an available channel on the device
 * \param name the name of an amplification element
 * \return a list of gain ranges in dB
 */
SOAPY_SDR_API SoapySDRRange SoapySDRDevice_getGainElementRange(const SoapySDRDevice *device, const SoapySDRDirection dir, const size_t channel, const char *name);

#ifdef __cplusplus
}
#endif
