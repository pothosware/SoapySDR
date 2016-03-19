///
/// \file SoapySDR/Device.h
///
/// Interface definition for Soapy SDR devices.
/// This is the C version of the Device interface.
///
/// General design rules about the API:
/// The caller must free non-const array results.
///
/// \copyright
/// Copyright (c) 2014-2016 Josh Blum
/// SPDX-License-Identifier: BSL-1.0
///

#pragma once
#include <SoapySDR/Config.h>
#include <SoapySDR/Types.h>
#include <SoapySDR/Constants.h>
#include <SoapySDR/Errors.h>
#include <SoapySDR/Device.h>

#ifdef __cplusplus
extern "C" {
#endif

//! Forward declaration of device handle
typedef struct SoapySDRDevice SoapySDRDevice;

//! Forward declaration of stream handle
typedef struct SoapySDRStream SoapySDRStream;

/*!
 * Enumerate a list of available devices on the system.
 * \param args device construction key/value argument filters
 * \param [out] length the number of elements in the result.
 * \return a list of arguments strings, each unique to a device
 */
SOAPY_SDR_API SoapySDRKwargs *SoapySDRDevice_enumerate(const SoapySDRKwargs *args, size_t *length);

/*!
 * Enumerate a list of available devices on the system.
 * Markup format for args: "keyA=valA, keyB=valB".
 * \param args a markup string of key/value argument filters
 * \param [out] length the number of elements in the result.
 * \return a list of arguments strings, each unique to a device
 */
SOAPY_SDR_API SoapySDRKwargs *SoapySDRDevice_enumerateStrArgs(const char *args, size_t *length);

/*!
 * Make a new Device object given device construction args.
 * The device pointer will be stored in a table so subsequent calls
 * with the same arguments will produce the same device.
 * For every call to make, there should be a matched call to unmake.
 *
 * \note This call is not thread safe. Implementations calling into make
 * from multiple threads should protect this call with a mutex.
 *
 * \param args device construction key/value argument map
 * \return a pointer to a new Device object
 */
SOAPY_SDR_API SoapySDRDevice *SoapySDRDevice_make(const SoapySDRKwargs *args);

/*!
 * Make a new Device object given device construction args.
 * The device pointer will be stored in a table so subsequent calls
 * with the same arguments will produce the same device.
 * For every call to make, there should be a matched call to unmake.
 *
 * \note This call is not thread safe. Implementations calling into make
 * from multiple threads should protect this call with a mutex.
 *
 * \param args a markup string of key/value arguments
 * \return a pointer to a new Device object
 */
SOAPY_SDR_API SoapySDRDevice *SoapySDRDevice_makeStrArgs(const char *args);

/*!
 * Unmake or release a device object handle.
 *
 * \note This call is not thread safe. Implementations calling into unmake
 * from multiple threads should protect this call with a mutex.
 *
 * \param device a pointer to a device object
 */
SOAPY_SDR_API void SoapySDRDevice_unmake(SoapySDRDevice *device);

/*******************************************************************
 * Identification API
 ******************************************************************/

/*!
 * A key that uniquely identifies the device driver.
 * This key identifies the underlying implementation.
 * Serveral variants of a product may share a driver.
 * \param device a pointer to a device instance
 */
SOAPY_SDR_API char *SoapySDRDevice_getDriverKey(const SoapySDRDevice *device);

/*!
 * A key that uniquely identifies the hardware.
 * This key should be meaningful to the user
 * to optimize for the underlying hardware.
 * \param device a pointer to a device instance
 */
SOAPY_SDR_API char *SoapySDRDevice_getHardwareKey(const SoapySDRDevice *device);

/*!
 * Query a dictionary of available device information.
 * This dictionary can any number of values like
 * vendor name, product name, revisions, serials...
 * This information can be displayed to the user
 * to help identify the instantiated device.
 * \param device a pointer to a device instance
 */
SOAPY_SDR_API SoapySDRKwargs SoapySDRDevice_getHardwareInfo(const SoapySDRDevice *device);

/*******************************************************************
 * Channels API
 ******************************************************************/

/*!
 * Set the frontend mapping of available DSP units to RF frontends.
 * This mapping controls channel mapping and channel availability.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param mapping a vendor-specific mapping string
 * \return an error message or NULL for success
 */
SOAPY_SDR_API char *SoapySDRDevice_setFrontendMapping(SoapySDRDevice *device, const int direction, const char *mapping);

/*!
 * Get the mapping configuration string.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param the vendor-specific mapping string
 */
SOAPY_SDR_API char *SoapySDRDevice_getFrontendMapping(const SoapySDRDevice *device, const int direction);

/*!
 * Get a number of channels given the streaming direction
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \return the number of channels
 */
SOAPY_SDR_API size_t SoapySDRDevice_getNumChannels(const SoapySDRDevice *device, const int direction);

/*!
 * Find out if the specified channel is full or half duplex.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \return true for full duplex, false for half duplex
 */
SOAPY_SDR_API bool SoapySDRDevice_getFullDuplex(const SoapySDRDevice *device, const int direction, const size_t channel);

/*******************************************************************
 * Stream API
 ******************************************************************/

/*!
 * Query a list of the available stream formats.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \param [out] length the number of format strings
 * \return a list of allowed format strings
 */
SOAPY_SDR_API char **SoapySDRDevice_getStreamFormats(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length);

/*!
 * Get the hardware's native stream format for this channel.
 * This is the format used by the underlying transport layer,
 * and the direct buffer access API calls (when available).
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \param [out] fullScale the maximum possible value
 * \return the native stream buffer format string
 */
SOAPY_SDR_API char *SoapySDRDevice_getNativeStreamFormat(const SoapySDRDevice *device, const int direction, const size_t channel, double *fullScale);

/*!
 * Query the argument info description for stream args.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \param [out] length the number of argument infos
 * \return a list of argument info structures
 */
SOAPY_SDR_API SoapySDRArgInfo *SoapySDRDevice_getStreamArgsInfo(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length);

/*!
 * Initialize a stream given a list of channels and stream arguments.
 * All stream API calls should be usable with the new stream object
 * after setupStream() is complete, regardless of the activity state.
 *
 * Format string markup guidelines:
 *  - C means complex
 *  - F means floating point
 *  - S means signed integer
 *  - U means unsigned integer
 *  - number float/int size in bytes (complex is 2x this size)
 *
 * Example format strings:
 *  - CF32 complex float32 (8 bytes per element)
 *  - CS16 complex int16 (4 bytes per element)
 *  - CS12 complex int12 (3 bytes per element)
 *  - CS4 complex int4 (1 byte per element)
 *  - S32 int32 (4 bytes per element)
 *  - U8 uint8 (1 byte per element)
 *
 * Recommended keys to use in the args dictionary:
 *  - "WIRE" - format of the samples between device and host
 *
 * On failure setup will return NULL and set the error.
 * The error message string must be freed by the caller.
 *
 * \param device a pointer to a device instance
 * \param [out] stream the opaque pointer to a stream handle
 * \param direction the channel direction RX or TX
 * \param format the desired buffer format in read/writeStream()
 * \param channels a list of channels for empty for automatic
 * \param numChans the number of elements in the channels array
 * \param args stream args or empty for defaults
 * \param errorMsg an error string set on failure
 * \return an error message or NULL for success
 */
SOAPY_SDR_API char *SoapySDRDevice_setupStream(SoapySDRDevice *device,
    SoapySDRStream **stream,
    const int direction,
    const char *format,
    const size_t *channels,
    const size_t numChans,
    const SoapySDRKwargs *args);

/*!
 * Close an open stream created by setupStream
 * \param device a pointer to a device instance
 * \param stream the opaque pointer to a stream handle
 */
SOAPY_SDR_API void SoapySDRDevice_closeStream(SoapySDRDevice *device, SoapySDRStream *stream);

/*!
 * Get the stream's maximum transmission unit (MTU) in number of elements.
 * The MTU specifies the maximum payload transfer in a stream operation.
 * This value can be used as a stream buffer allocation size that can
 * best optimize throughput given the underlying stream implementation.
 *
 * \param device a pointer to a device instance
 * \param stream the opaque pointer to a stream handle
 * \return the MTU in number of stream elements (never zero)
 */
SOAPY_SDR_API size_t SoapySDRDevice_getStreamMTU(const SoapySDRDevice *device, SoapySDRStream *stream);

/*!
 * Activate a stream.
 * Call activate to prepare a stream before using read/write().
 * The implementation control switches or stimulate data flow.
 *
 * The timeNs is only valid when the flags have SOAPY_SDR_HAS_TIME.
 * The numElems count can be used to request a finite burst size.
 * The SOAPY_SDR_END_BURST flag can signal end on the finite burst.
 * Not all implementations will support the full range of options.
 * In this case, the implementation returns SOAPY_SDR_NOT_SUPPORTED.
 *
 * \param device a pointer to a device instance
 * \param stream the opaque pointer to a stream handle
 * \param flags optional flag indicators about the stream
 * \param timeNs optional activation time in nanoseconds
 * \param numElems optional element count for burst control
 * \return 0 for success or error code on failure
 */
SOAPY_SDR_API int SoapySDRDevice_activateStream(SoapySDRDevice *device,
    SoapySDRStream *stream,
    const int flags,
    const long long timeNs,
    const size_t numElems);

/*!
 * Deactivate a stream.
 * Call deactivate when not using using read/write().
 * The implementation control switches or halt data flow.
 *
 * The timeNs is only valid when the flags have SOAPY_SDR_HAS_TIME.
 * Not all implementations will support the full range of options.
 * In this case, the implementation returns SOAPY_SDR_NOT_SUPPORTED.
 *
 * \param device a pointer to a device instance
 * \param stream the opaque pointer to a stream handle
 * \param flags optional flag indicators about the stream
 * \param timeNs optional deactivation time in nanoseconds
 * \return 0 for success or error code on failure
 */
SOAPY_SDR_API int SoapySDRDevice_deactivateStream(SoapySDRDevice *device,
    SoapySDRStream *stream,
    const int flags,
    const long long timeNs);

/*!
 * Read elements from a stream for reception.
 * This is a multi-channel call, and buffs should be an array of void *,
 * where each pointer will be filled with data from a different channel.
 *
 * **Client code compatibility:**
 * The readStream() call should be well defined at all times,
 * including prior to activation and after deactivation.
 * When inactive, readStream() should implement the timeout
 * specified by the caller and return SOAPY_SDR_TIMEOUT.
 *
 * \param device a pointer to a device instance
 * \param stream the opaque pointer to a stream handle
 * \param buffs an array of void* buffers num chans in size
 * \param numElems the number of elements in each buffer
 * \param [out] flags optional flag indicators about the result
 * \param [out] timeNs the buffer's timestamp in nanoseconds
 * \param timeoutUs the timeout in microseconds
 * \return the number of elements read per buffer or error code
 */
SOAPY_SDR_API int SoapySDRDevice_readStream(SoapySDRDevice *device,
    SoapySDRStream *stream,
    void * const *buffs,
    const size_t numElems,
    int *flags,
    long long *timeNs,
    const long timeoutUs);

/*!
 * Write elements to a stream for transmission.
 * This is a multi-channel call, and buffs should be an array of void *,
 * where each pointer will be filled with data for a different channel.
 *
 * **Client code compatibility:**
 * Client code relies on writeStream() for proper back-pressure.
 * The writeStream() implementation must enforce the timeout
 * such that the call blocks until space becomes available
 * or timeout expiration.
 *
 * \param device a pointer to a device instance
 * \param stream the opaque pointer to a stream handle
 * \param buffs an array of void* buffers num chans in size
 * \param numElems the number of elements in each buffer
 * \param [in,out] flags optional input flags and output flags
 * \param timeNs the buffer's timestamp in nanoseconds
 * \param timeoutUs the timeout in microseconds
 * \return the number of elements written per buffer or error
 */
SOAPY_SDR_API int SoapySDRDevice_writeStream(SoapySDRDevice *device,
    SoapySDRStream *stream,
    const void * const *buffs,
    const size_t numElems,
    int *flags,
    const long long timeNs,
    const long timeoutUs);

/*!
 * Readback status information about a stream.
 * This call is typically used on a transmit stream
 * to report time errors, underflows, and burst completion.
 *
 * **Client code compatibility:**
 * Client code may continually poll readStreamStatus() in a loop.
 * Implementations of readStreamStatus() should wait in the call
 * for a status change event or until the timeout expiration.
 * When stream status is not implemented on a particular stream,
 * readStreamStatus() should return SOAPY_SDR_NOT_SUPPORTED.
 * Client code may use this indication to disable a polling loop.
 *
 * \param device a pointer to a device instance
 * \param stream the opaque pointer to a stream handle
 * \param chanMask to which channels this status applies
 * \param flags optional input flags and output flags
 * \param timeNs the buffer's timestamp in nanoseconds
 * \param timeoutUs the timeout in microseconds
 * \return 0 for success or error code like timeout
 */
SOAPY_SDR_API int SoapySDRDevice_readStreamStatus(SoapySDRDevice *device,
    SoapySDRStream *stream,
    size_t *chanMask,
    int *flags,
    long long *timeNs,
    const long timeoutUs);

/*******************************************************************
 * Direct buffer access API
 ******************************************************************/

/*!
 * How many direct access buffers can the stream provide?
 * This is the number of times the user can call acquire()
 * on a stream without making subsequent calls to release().
 * A return value of 0 means that direct access is not supported.
 *
 * \param device a pointer to a device instance
 * \param stream the opaque pointer to a stream handle
 * \return the number of direct access buffers or 0
 */
SOAPY_SDR_API size_t SoapySDRDevice_getNumDirectAccessBuffers(SoapySDRDevice *device, SoapySDRStream *stream);

/*!
 * Get the buffer addresses for a scatter/gather table entry.
 * When the underlying DMA implementation uses scatter/gather
 * then this call provides the user addresses for that table.
 *
 * Example: The caller may query the DMA memory addresses once
 * after stream creation to pre-allocate a re-usable ring-buffer.
 *
 * \param device a pointer to a device instance
 * \param stream the opaque pointer to a stream handle
 * \param handle an index value between 0 and num direct buffers - 1
 * \param buffs an array of void* buffers num chans in size
 * \return 0 for success or error code when not supported
 */
SOAPY_SDR_API int SoapySDRDevice_getDirectAccessBufferAddrs(SoapySDRDevice *device, SoapySDRStream *stream, const size_t handle, void **buffs);

/*!
 * Acquire direct buffers from a receive stream.
 * This call is part of the direct buffer access API.
 *
 * The buffs array will be filled with a stream pointer for each channel.
 * Each pointer can be read up to the number of return value elements.
 *
 * The handle will be set by the implementation so that the caller
 * may later release access to the buffers with releaseReadBuffer().
 * Handle represents an index into the internal scatter/gather table
 * such that handle is between 0 and num direct buffers - 1.
 *
 * \param device a pointer to a device instance
 * \param stream the opaque pointer to a stream handle
 * \param handle an index value used in the release() call
 * \param buffs an array of void* buffers num chans in size
 * \param flags optional flag indicators about the result
 * \param timeNs the buffer's timestamp in nanoseconds
 * \param timeoutUs the timeout in microseconds
 * \return the number of elements read per buffer or error code
 */
SOAPY_SDR_API int SoapySDRDevice_acquireReadBuffer(SoapySDRDevice *device,
    SoapySDRStream *stream,
    size_t *handle,
    const void **buffs,
    int *flags,
    long long *timeNs,
    const long timeoutUs);

/*!
 * Release an acquired buffer back to the receive stream.
 * This call is part of the direct buffer access API.
 *
 * \param device a pointer to a device instance
 * \param stream the opaque pointer to a stream handle
 * \param handle the opaque handle from the acquire() call
 */
SOAPY_SDR_API void SoapySDRDevice_releaseReadBuffer(SoapySDRDevice *device,
    SoapySDRStream *stream,
    const size_t handle);

/*!
 * Acquire direct buffers from a transmit stream.
 * This call is part of the direct buffer access API.
 *
 * The buffs array will be filled with a stream pointer for each channel.
 * Each pointer can be written up to the number of return value elements.
 *
 * The handle will be set by the implementation so that the caller
 * may later release access to the buffers with releaseWriteBuffer().
 * Handle represents an index into the internal scatter/gather table
 * such that handle is between 0 and num direct buffers - 1.
 *
 * \param device a pointer to a device instance
 * \param stream the opaque pointer to a stream handle
 * \param handle an index value used in the release() call
 * \param buffs an array of void* buffers num chans in size
 * \param flags optional input flags and output flags
 * \param timeNs the buffer's timestamp in nanoseconds
 * \param timeoutUs the timeout in microseconds
 * \return the number of available elements per buffer or error
 */
SOAPY_SDR_API int SoapySDRDevice_acquireWriteBuffer(SoapySDRDevice *device,
    SoapySDRStream *stream,
    size_t *handle,
    void **buffs,
    const long timeoutUs);

/*!
 * Release an acquired buffer back to the transmit stream.
 * This call is part of the direct buffer access API.
 *
 * Stream meta-data is provided as part of the release call,
 * and not the acquire call so that the caller may acquire
 * buffers without committing to the contents of the meta-data,
 * which can be determined by the user as the buffers are filled.
 *
 * \param device a pointer to a device instance
 * \param stream the opaque pointer to a stream handle
 * \param handle the opaque handle from the acquire() call
 * \param numElems the number of elements written to each buffer
 * \param flags optional input flags and output flags
 * \param timeNs the buffer's timestamp in nanoseconds
 */
SOAPY_SDR_API void SoapySDRDevice_releaseWriteBuffer(SoapySDRDevice *device,
    SoapySDRStream *stream,
    const size_t handle,
    const size_t numElems,
    int *flags,
    const long long timeNs);

/*******************************************************************
 * Antenna API
 ******************************************************************/

/*!
 * Get a list of available antennas to select on a given chain.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \param [out] length the number of antenna names
 * \return a list of available antenna names
 */
SOAPY_SDR_API char **SoapySDRDevice_listAntennas(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length);

/*!
 * Set the selected antenna on a chain.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \param name the name of an available antenna
 * \return an error message or NULL for success
 */
SOAPY_SDR_API char *SoapySDRDevice_setAntenna(SoapySDRDevice *device, const int direction, const size_t channel, const char *name);

/*!
 * Get the selected antenna on a chain.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \return the name of an available antenna
 */
SOAPY_SDR_API char *SoapySDRDevice_getAntenna(const SoapySDRDevice *device, const int direction, const size_t channel);

/*******************************************************************
 * Frontend corrections API
 ******************************************************************/

/*!
 * Does the device support automatic DC offset corrections?
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \return true if automatic corrections are supported
 */
SOAPY_SDR_API bool SoapySDRDevice_hasDCOffsetMode(const SoapySDRDevice *device, const int direction, const size_t channel);

/*!
 * Set the automatic DC offset corrections mode.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \param automatic true for automatic offset correction
 * \return an error message or NULL for success
 */
SOAPY_SDR_API char *SoapySDRDevice_setDCOffsetMode(SoapySDRDevice *device, const int direction, const size_t channel, const bool automatic);

/*!
 * Get the automatic DC offset corrections mode.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \return true for automatic offset correction
 */
SOAPY_SDR_API bool SoapySDRDevice_getDCOffsetMode(const SoapySDRDevice *device, const int direction, const size_t channel);

/*!
 * Does the device support frontend DC offset correction?
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \return true if DC offset corrections are supported
 */
SOAPY_SDR_API bool SoapySDRDevice_hasDCOffset(const SoapySDRDevice *device, const int direction, const size_t channel);

/*!
 * Set the frontend DC offset correction.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \param offsetI the relative correction (1.0 max)
 * \param offsetQ the relative correction (1.0 max)
 * \return an error message or NULL for success
 */
SOAPY_SDR_API char *SoapySDRDevice_setDCOffset(SoapySDRDevice *device, const int direction, const size_t channel, const double offsetI, const double offsetQ);

/*!
 * Get the frontend DC offset correction.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \param [out] offsetI the relative correction (1.0 max)
 * \param [out] offsetQ the relative correction (1.0 max)
 */
SOAPY_SDR_API void SoapySDRDevice_getDCOffset(const SoapySDRDevice *device, const int direction, const size_t channel, double *offsetI, double *offsetQ);

/*!
 * Does the device support frontend IQ balance correction?
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \return true if IQ balance corrections are supported
 */
SOAPY_SDR_API bool SoapySDRDevice_hasIQBalance(const SoapySDRDevice *device, const int direction, const size_t channel);

/*!
 * Set the frontend IQ balance correction.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \param balanceI the relative correction (1.0 max)
 * \param balanceQ the relative correction (1.0 max)
 * \return an error message or NULL for success
 */
SOAPY_SDR_API char *SoapySDRDevice_setIQBalance(SoapySDRDevice *device, const int direction, const size_t channel, const double balanceI, const double balanceQ);

/*!
 * Get the frontend IQ balance correction.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \param [out] balanceI the relative correction (1.0 max)
 * \param [out] balanceQ the relative correction (1.0 max)
 */
SOAPY_SDR_API void SoapySDRDevice_getIQBalance(const SoapySDRDevice *device, const int direction, const size_t channel, double *balanceI, double *balanceQ);

/*******************************************************************
 * Gain API
 ******************************************************************/

/*!
 * List available amplification elements.
 * Elements should be in order RF to baseband.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel
 * \param [out] length the number of gain names
 * \return a list of gain string names
 */
SOAPY_SDR_API char **SoapySDRDevice_listGains(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length);

/*!
 * Does the device support automatic gain control?
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \return true for automatic gain control
 */
SOAPY_SDR_API bool SoapySDRDevice_hasGainMode(const SoapySDRDevice *device, const int direction, const size_t channel);

/*!
 * Set the automatic gain mode on the chain.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \param automatic true for automatic gain setting
 * \return an error message or NULL for success
 */
SOAPY_SDR_API char *SoapySDRDevice_setGainMode(SoapySDRDevice *device, const int direction, const size_t channel, const bool automatic);

/*!
 * Get the automatic gain mode on the chain.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \return true for automatic gain setting
 */
SOAPY_SDR_API bool SoapySDRDevice_getGainMode(const SoapySDRDevice *device, const int direction, const size_t channel);

/*!
 * Set the overall amplification in a chain.
 * The gain will be distributed automatically across available element.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \param name the name of an amplification element
 * \param value the new amplification value in dB
 * \return an error message or NULL for success
 */
SOAPY_SDR_API char *SoapySDRDevice_setGain(SoapySDRDevice *device, const int direction, const size_t channel, const double value);

/*!
 * Set the value of a amplification element in a chain.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \param name the name of an amplification element
 * \param value the new amplification value in dB
 * \return an error message or NULL for success
 */
SOAPY_SDR_API char *SoapySDRDevice_setGainElement(SoapySDRDevice *device, const int direction, const size_t channel, const char *name, const double value);

/*!
 * Get the overall value of the gain elements in a chain.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \return the value of the gain in dB
 */
SOAPY_SDR_API double SoapySDRDevice_getGain(const SoapySDRDevice *device, const int direction, const size_t channel);

/*!
 * Get the value of an individual amplification element in a chain.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \param name the name of an amplification element
 * \return the value of the gain in dB
 */
SOAPY_SDR_API double SoapySDRDevice_getGainElement(const SoapySDRDevice *device, const int direction, const size_t channel, const char *name);

/*!
 * Get the overall range of possible gain values.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \return a list of gain ranges in dB
 */
SOAPY_SDR_API SoapySDRRange SoapySDRDevice_getGainRange(const SoapySDRDevice *device, const int direction, const size_t channel);

/*!
 * Get the range of possible gain values for a specific element.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \param name the name of an amplification element
 * \return a list of gain ranges in dB
 */
SOAPY_SDR_API SoapySDRRange SoapySDRDevice_getGainElementRange(const SoapySDRDevice *device, const int direction, const size_t channel, const char *name);

/*******************************************************************
 * Frequency API
 ******************************************************************/

/*!
 * Set the center frequency of the chain.
 *  - For RX, this specifies the down-conversion frequency.
 *  - For TX, this specifies the up-conversion frequency.
 *
 * The default implementation of setFrequency() will tune the "RF"
 * component as close as possible to the requested center frequency.
 * Tuning inaccuracies will be compensated for with the "BB" component.
 *
 * The args can be used to augment the tuning algorithm.
 *  - Use "OFFSET" to specify an "RF" tuning offset,
 *    usually with the intention of moving the LO out of the passband.
 *    The offset will be compensated for using the "BB" component.
 *  - Use the name of a component for the key and a frequency in Hz
 *    as the value (any format) to enforce a specific frequency.
 *    The other components will be tuned with compensation
 *    to achieve the specified overall frequency.
 *  - Use the name of a component for the key and the value "IGNORE"
 *    so that the tuning algorithm will avoid altering the component.
 *  - Vendor specific implementations can also use the same args to augment
 *    tuning in other ways such as specifying fractional vs integer N tuning.
 *
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \param frequency the center frequency in Hz
 * \param args optional tuner arguments
 * \return an error message or NULL for success
 */
SOAPY_SDR_API char *SoapySDRDevice_setFrequency(SoapySDRDevice *device, const int direction, const size_t channel, const double frequency, const SoapySDRKwargs *args);

/*!
 * Tune the center frequency of the specified element.
 *  - For RX, this specifies the down-conversion frequency.
 *  - For TX, this specifies the up-conversion frequency.
 *
 * Recommended names used to represent tunable components:
 *  - "CORR" - freq error correction in PPM
 *  - "RF" - frequency of the RF frontend
 *  - "BB" - frequency of the baseband DSP
 *
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \param name the name of a tunable element
 * \param frequency the center frequency in Hz
 * \param args optional tuner arguments
 * \return an error message or NULL for success
 */
SOAPY_SDR_API char *SoapySDRDevice_setFrequencyComponent(SoapySDRDevice *device, const int direction, const size_t channel, const char *name, const double frequency, const SoapySDRKwargs *args);

/*!
 * Get the overall center frequency of the chain.
 *  - For RX, this specifies the down-conversion frequency.
 *  - For TX, this specifies the up-conversion frequency.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \return the center frequency in Hz
 */
SOAPY_SDR_API double SoapySDRDevice_getFrequency(const SoapySDRDevice *device, const int direction, const size_t channel);

/*!
 * Get the frequency of a tunable element in the chain.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \param name the name of a tunable element
 * \return the tunable element's frequency in Hz
 */
SOAPY_SDR_API double SoapySDRDevice_getFrequencyComponent(const SoapySDRDevice *device, const int direction, const size_t channel, const char *name);

/*!
 * List available tunable elements in the chain.
 * Elements should be in order RF to baseband.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel
 * \param [out] length the number names
 * \return a list of tunable elements by name
 */
SOAPY_SDR_API char **SoapySDRDevice_listFrequencies(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length);

/*!
 * Get the range of overall frequency values.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \param [out] length the number of ranges
 * \return a list of frequency ranges in Hz
 */
SOAPY_SDR_API SoapySDRRange *SoapySDRDevice_getFrequencyRange(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length);

/*!
 * Get the range of tunable values for the specified element.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \param name the name of a tunable element
 * \param [out] length the number of ranges
 * \return a list of frequency ranges in Hz
 */
SOAPY_SDR_API SoapySDRRange *SoapySDRDevice_getFrequencyRangeComponent(const SoapySDRDevice *device, const int direction, const size_t channel, const char *name, size_t *length);

/*!
 * Query the argument info description for tune args.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \param [out] length the number of argument infos
 * \return a list of argument info structures
 */
SOAPY_SDR_API SoapySDRArgInfo *SoapySDRDevice_getFrequencyArgsInfo(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length);

/*******************************************************************
 * Sample Rate API
 ******************************************************************/

/*!
 * Set the baseband sample rate of the chain.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \param rate the sample rate in samples per second
 * \return an error message or NULL for success
 */
SOAPY_SDR_API char *SoapySDRDevice_setSampleRate(SoapySDRDevice *device, const int direction, const size_t channel, const double rate);

/*!
 * Get the baseband sample rate of the chain.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \return the sample rate in samples per second
 */
SOAPY_SDR_API double SoapySDRDevice_getSampleRate(const SoapySDRDevice *device, const int direction, const size_t channel);

/*!
 * Get the range of possible baseband sample rates.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \param [out] length the number of sample rates
 * \return a list of possible rates in samples per second
 */
SOAPY_SDR_API double *SoapySDRDevice_listSampleRates(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length);

/*******************************************************************
 * Bandwidth API
 ******************************************************************/

/*!
 * Set the baseband filter width of the chain.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \param bw the baseband filter width in Hz
 * \return an error message or NULL for success
 */
SOAPY_SDR_API char *SoapySDRDevice_setBandwidth(SoapySDRDevice *device, const int direction, const size_t channel, const double bw);

/*!
 * Get the baseband filter width of the chain.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \return the baseband filter width in Hz
 */
SOAPY_SDR_API double SoapySDRDevice_getBandwidth(const SoapySDRDevice *device, const int direction, const size_t channel);

/*!
 * Get the range of possible baseband filter widths.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \param [out] length the number of bandwidths
 * \return a list of possible bandwidths in Hz
 */
SOAPY_SDR_API double *SoapySDRDevice_listBandwidths(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length);

/*******************************************************************
 * Clocking API
 ******************************************************************/

/*!
 * Set the master clock rate of the device.
 * \param device a pointer to a device instance
 * \param rate the clock rate in Hz
 * \return an error message or NULL for success
 */
SOAPY_SDR_API char *SoapySDRDevice_setMasterClockRate(SoapySDRDevice *device, const double rate);

/*!
 * Get the master clock rate of the device.
 * \param device a pointer to a device instance
 * \return the clock rate in Hz
 */
SOAPY_SDR_API double SoapySDRDevice_getMasterClockRate(const SoapySDRDevice *device);

/*!
 * Get the range of available master clock rates.
 * \param device a pointer to a device instance
 * \param [out] length the number of ranges
 * \return a list of clock rate ranges in Hz
 */
SOAPY_SDR_API SoapySDRRange *SoapySDRDevice_getMasterClockRates(const SoapySDRDevice *device, size_t *length);

/*!
 * Get the list of available clock sources.
 * \param device a pointer to a device instance
 * \param [out] length the number of sources
 * \return a list of clock source names
 */
SOAPY_SDR_API char **SoapySDRDevice_listClockSources(const SoapySDRDevice *device, size_t *length);

/*!
 * Set the clock source on the device
 * \param device a pointer to a device instance
 * \param source the name of a clock source
 * \return an error message or NULL for success
 */
SOAPY_SDR_API char *SoapySDRDevice_setClockSource(SoapySDRDevice *device, const char *source);

/*!
 * Get the clock source of the device
 * \param device a pointer to a device instance
 * \return the name of a clock source
 */
SOAPY_SDR_API char *SoapySDRDevice_getClockSource(const SoapySDRDevice *device);

/*!
 * Get the list of available time sources.
 * \param device a pointer to a device instance
 * \param [out] length the number of sources
 * \return a list of time source names
 */
SOAPY_SDR_API char **SoapySDRDevice_listTimeSources(const SoapySDRDevice *device, size_t *length);

/*!
 * Set the time source on the device
 * \param device a pointer to a device instance
 * \param source the name of a time source
 * \return an error message or NULL for success
 */
SOAPY_SDR_API char *SoapySDRDevice_setTimeSource(SoapySDRDevice *device, const char *source);

/*!
 * Get the time source of the device
 * \param device a pointer to a device instance
 * \return the name of a time source
 */
SOAPY_SDR_API char *SoapySDRDevice_getTimeSource(const SoapySDRDevice *device);

/*******************************************************************
 * Time API
 ******************************************************************/

/*!
 * Does this device have a hardware clock?
 * \param device a pointer to a device instance
 * \param what optional argument
 * \return true if the hardware clock exists
 */
SOAPY_SDR_API bool SoapySDRDevice_hasHardwareTime(const SoapySDRDevice *device, const char *what);

/*!
 * Read the time from the hardware clock on the device.
 * The what argument can refer to a specific time counter.
 * \param device a pointer to a device instance
 * \param what optional argument
 * \return the time in nanoseconds
 */
SOAPY_SDR_API long long SoapySDRDevice_getHardwareTime(const SoapySDRDevice *device, const char *what);

/*!
 * Write the time to the hardware clock on the device.
 * The what argument can refer to a specific time counter.
 * \param device a pointer to a device instance
 * \param timeNs time in nanoseconds
 * \param what optional argument
 */
SOAPY_SDR_API void SoapySDRDevice_setHardwareTime(SoapySDRDevice *device, const long long timeNs, const char *what);

/*!
 * Set the time of subsequent configuration calls.
 * The what argument can refer to a specific command queue.
 * Implementations may use a time of 0 to clear.
 * \param device a pointer to a device instance
 * \param timeNs time in nanoseconds
 * \param what optional argument
 */
SOAPY_SDR_API void SoapySDRDevice_setCommandTime(SoapySDRDevice *device, const long long timeNs, const char *what);

/*******************************************************************
 * Sensor API
 ******************************************************************/

/*!
 * List the available global readback sensors.
 * A sensor can represent a reference lock, RSSI, temperature.
 * \param device a pointer to a device instance
 * \param [out] length the number of sensor names
 * \return a list of available sensor string names
 */
SOAPY_SDR_API char **SoapySDRDevice_listSensors(const SoapySDRDevice *device, size_t *length);

/*!
 * Get meta-information about a sensor.
 * Example: displayable name, type, range.
 * \param device a pointer to a device instance
 * \param name the name of an available sensor
 * \return meta-information about a sensor
 */
SOAPY_SDR_API SoapySDRArgInfo SoapySDRDevice_getSensorInfo(const SoapySDRDevice *device, const char *name);

/*!
 * Readback a global sensor given the name.
 * The value returned is a string which can represent
 * a boolean ("true"/"false"), an integer, or float.
 * \param device a pointer to a device instance
 * \param name the name of an available sensor
 * \return the current value of the sensor
 */
SOAPY_SDR_API char *SoapySDRDevice_readSensor(const SoapySDRDevice *device, const char *name);

/*!
 * List the available channel readback sensors.
 * A sensor can represent a reference lock, RSSI, temperature.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \param [out] length the number of sensor names
 * \return a list of available sensor string names
 */
SOAPY_SDR_API char **SoapySDRDevice_listChannelSensors(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length);

/*!
 * Get meta-information about a channel sensor.
 * Example: displayable name, type, range.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \param name the name of an available sensor
 * \return meta-information about a sensor
 */
SOAPY_SDR_API SoapySDRArgInfo SoapySDRDevice_getChannelSensorInfo(const SoapySDRDevice *device, const int direction, const size_t channel, const char *name);

/*!
 * Readback a channel sensor given the name.
 * The value returned is a string which can represent
 * a boolean ("true"/"false"), an integer, or float.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \param name the name of an available sensor
 * \return the current value of the sensor
 */
SOAPY_SDR_API char *SoapySDRDevice_readChannelSensor(const SoapySDRDevice *device, const int direction, const size_t channel, const char *name);

/*******************************************************************
 * Register API
 ******************************************************************/

/*!
 * Write a register on the device.
 * This can represent a register on a soft CPU, FPGA, IC;
 * the interpretation is up the implementation to decide.
 * \param device a pointer to a device instance
 * \param addr the register address
 * \param value the register value
 */
SOAPY_SDR_API void SoapySDRDevice_writeRegister(SoapySDRDevice *device, const unsigned addr, const unsigned value);

/*!
 * Read a register on the device.
 * \param device a pointer to a device instance
 * \param addr the register address
 * \return the register value
 */
SOAPY_SDR_API unsigned SoapySDRDevice_readRegister(const SoapySDRDevice *device, const unsigned addr);

/*******************************************************************
 * Settings API
 ******************************************************************/

/*!
 * Describe the allowed keys and values used for settings.
 * \param device a pointer to a device instance
 * \param [out] length the number of sensor names
 * \return a list of argument info structures
 */
SOAPY_SDR_API SoapySDRArgInfo *SoapySDRDevice_getSettingInfo(const SoapySDRDevice *device, size_t *length);

/*!
 * Write an arbitrary setting on the device.
 * The interpretation is up the implementation.
 * \param device a pointer to a device instance
 * \param key the setting identifier
 * \param value the setting value
 */
SOAPY_SDR_API void SoapySDRDevice_writeSetting(SoapySDRDevice *device, const char *key, const char *value);

/*!
 * Read an arbitrary setting on the device.
 * \param device a pointer to a device instance
 * \param key the setting identifier
 * \return the setting value
 */
SOAPY_SDR_API char *SoapySDRDevice_readSetting(const SoapySDRDevice *device, const char *key);

/*!
 * Describe the allowed keys and values used for channel settings.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \param [out] length the number of sensor names
 * \return a list of argument info structures
 */
SOAPY_SDR_API SoapySDRArgInfo *SoapySDRDevice_getChannelSettingInfo(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length);

/*!
 * Write an arbitrary channel setting on the device.
 * The interpretation is up the implementation.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \param key the setting identifier
 * \param value the setting value
 */
SOAPY_SDR_API void SoapySDRDevice_writeChannelSetting(SoapySDRDevice *device, const int direction, const size_t channel, const char *key, const char *value);

/*!
 * Read an arbitrary channel setting on the device.
 * \param device a pointer to a device instance
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \param key the setting identifier
 * \return the setting value
 */
SOAPY_SDR_API char *SoapySDRDevice_readChannelSetting(const SoapySDRDevice *device, const int direction, const size_t channel, const char *key);

/*******************************************************************
 * GPIO API
 ******************************************************************/

/*!
 * Get a list of available GPIO banks by name.
 * \param [out] length the number of GPIO banks
 * \param device a pointer to a device instance
 */
SOAPY_SDR_API char **SoapySDRDevice_listGPIOBanks(const SoapySDRDevice *device, size_t *length);

/*!
 * Write the value of a GPIO bank.
 * \param device a pointer to a device instance
 * \param bank the name of an available bank
 * \param value an integer representing GPIO bits
 */
SOAPY_SDR_API void SoapySDRDevice_writeGPIO(SoapySDRDevice *device, const char *bank, const unsigned value);

/*!
 * Write the value of a GPIO bank with modification mask.
 * \param device a pointer to a device instance
 * \param bank the name of an available bank
 * \param value an integer representing GPIO bits
 * \param mask a modification mask where 1 = modify
 */
SOAPY_SDR_API void SoapySDRDevice_writeGPIOMasked(SoapySDRDevice *device, const char *bank, const unsigned value, const unsigned mask);

/*!
 * Readback the value of a GPIO bank.
 * \param device a pointer to a device instance
 * \param bank the name of an available bank
 * \return an integer representing GPIO bits
 */
SOAPY_SDR_API unsigned SoapySDRDevice_readGPIO(const SoapySDRDevice *device, const char *bank);

/*!
 * Write the data direction of a GPIO bank.
 * 1 bits represent outputs, 0 bits represent inputs.
 * \param device a pointer to a device instance
 * \param bank the name of an available bank
 * \param dir an integer representing data direction bits
 */
SOAPY_SDR_API void SoapySDRDevice_writeGPIODir(SoapySDRDevice *device, const char *bank, const unsigned dir);

/*!
 * Write the data direction of a GPIO bank with modification mask.
 * 1 bits represent outputs, 0 bits represent inputs.
 * \param device a pointer to a device instance
 * \param bank the name of an available bank
 * \param dir an integer representing data direction bits
 * \param mask a modification mask where 1 = modify
 */
SOAPY_SDR_API void SoapySDRDevice_writeGPIODirMasked(SoapySDRDevice *device, const char *bank, const unsigned dir, const unsigned mask);

/*!
 * Read the data direction of a GPIO bank.
 * \param device a pointer to a device instance
 * 1 bits represent outputs, 0 bits represent inputs.
 * \param bank the name of an available bank
 * \return an integer representing data direction bits
 */
SOAPY_SDR_API unsigned SoapySDRDevice_readGPIODir(const SoapySDRDevice *device, const char *bank);

/*******************************************************************
 * I2C API
 ******************************************************************/

/*!
 * Write to an available I2C slave.
 * If the device contains multiple I2C masters,
 * the address bits can encode which master.
 * \param device a pointer to a device instance
 * \param addr the address of the slave
 * \param data an array of bytes write out
 * \param numBytes the number of bytes to write
 */
SOAPY_SDR_API void SoapySDRDevice_writeI2C(SoapySDRDevice *device, const int addr, const char *data, const size_t numBytes);

/*!
 * Read from an available I2C slave.
 * If the device contains multiple I2C masters,
 * the address bits can encode which master.
 * \param device a pointer to a device instance
 * \param addr the address of the slave
 * \param numBytes the number of bytes to read
 * \return an array of bytes read from the slave
 */
SOAPY_SDR_API char *SoapySDRDevice_readI2C(SoapySDRDevice *device, const int addr, const size_t numBytes);

/*******************************************************************
 * SPI API
 ******************************************************************/

/*!
 * Perform a SPI transaction and return the result.
 * Its up to the implementation to set the clock rate,
 * and read edge, and the write edge of the SPI core.
 * SPI slaves without a readback pin will return 0.
 *
 * If the device contains multiple SPI masters,
 * the address bits can encode which master.
 *
 * \param device a pointer to a device instance
 * \param addr an address of an available SPI slave
 * \param data the SPI data, numBits-1 is first out
 * \param numBits the number of bits to clock out
 * \return the readback data, numBits-1 is first in
 */
SOAPY_SDR_API unsigned SoapySDRDevice_transactSPI(SoapySDRDevice *device, const int addr, const unsigned data, const size_t numBits);

/*******************************************************************
 * UART API
 ******************************************************************/

/*!
 * Enumerate the available UART devices.
 * \param device a pointer to a device instance
 * \param [out] length the number of UART names
 * \return a list of names of available UARTs
 */
SOAPY_SDR_API char **SoapySDRDevice_listUARTs(const SoapySDRDevice *device, size_t *length);

/*!
 * Write data to a UART device.
 * Its up to the implementation to set the baud rate,
 * carriage return settings, flushing on newline.
 * \param device a pointer to a device instance
 * \param which the name of an available UART
 * \param data a null terminated array of bytes
 */
SOAPY_SDR_API void SoapySDRDevice_writeUART(SoapySDRDevice *device, const char *which, const char *data);

/*!
 * Read bytes from a UART until timeout or newline.
 * Its up to the implementation to set the baud rate,
 * carriage return settings, flushing on newline.
 * \param device a pointer to a device instance
 * \param which the name of an available UART
 * \param timeoutUs a timeout in microseconds
 * \return a null terminated array of bytes
 */
SOAPY_SDR_API char *SoapySDRDevice_readUART(const SoapySDRDevice *device, const char *which, const long timeoutUs);

#ifdef __cplusplus
}
#endif
