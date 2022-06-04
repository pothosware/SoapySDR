// Copyright (c) 2014-2020 Josh Blum
// Copyright (c) 2016-2016 Bastille Networks
// SPDX-License-Identifier: BSL-1.0

#include "ErrorHelpers.hpp"
#include "TypeHelpers.hpp"
#include <SoapySDR/Device.h>
#include <SoapySDR/Device.hpp>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <cmath> //NAN

/*******************************************************************
 * Error message implementation
 ******************************************************************/
#ifdef _MSC_VER
#define __thread __declspec(thread)
#endif

static __thread int lastErrorStatus;

static __thread char lastErrorMsg[1024];

void SoapySDRDevice_clearError(void)
{
    lastErrorMsg[0] = '\0';
    lastErrorStatus = 0;
}

int SoapySDRDevice_lastStatus(void)
{
    return lastErrorStatus;
}

void SoapySDRDevice_reportError(const char *msg)
{
    strncpy(lastErrorMsg, msg, sizeof(lastErrorMsg));
    lastErrorMsg[sizeof(lastErrorMsg)-1] = '\0';
    lastErrorStatus = -1;
}

const char *SoapySDRDevice_lastError(void)
{
    return lastErrorMsg;
}

/*******************************************************************
 * Error POD types
 ******************************************************************/

static const bool SoapySDRBoolErr = bool(-1);

static const SoapySDRRange SoapySDRRangeNAN = {NAN, NAN, 0.0};

static SoapySDRArgInfo SoapySDRArgInfoNull(void)
{
    SoapySDRArgInfo info;
    std::memset(&info, 0, sizeof(info));
    return info;
}

/*******************************************************************
 * Simple subclass definition for device
 ******************************************************************/
struct SoapySDRDevice : SoapySDR::Device {};

extern "C" {

/*******************************************************************
 * Identification API
 ******************************************************************/
char *SoapySDRDevice_getDriverKey(const SoapySDRDevice *device)
{
    __SOAPY_SDR_C_TRY
    return toCString(device->getDriverKey());
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

char *SoapySDRDevice_getHardwareKey(const SoapySDRDevice *device)
{
    __SOAPY_SDR_C_TRY
    return toCString(device->getHardwareKey());
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

SoapySDRKwargs SoapySDRDevice_getHardwareInfo(const SoapySDRDevice *device)
{
    __SOAPY_SDR_C_TRY
    return toKwargs(device->getHardwareInfo());
    __SOAPY_SDR_C_CATCH_RET(toKwargs(SoapySDR::Kwargs()));
}

/*******************************************************************
 * Channels API
 ******************************************************************/
int SoapySDRDevice_setFrontendMapping(SoapySDRDevice *device, const int direction, const char *mapping)
{
    __SOAPY_SDR_C_TRY
    device->setFrontendMapping(direction, mapping);
    __SOAPY_SDR_C_CATCH
}

char *SoapySDRDevice_getFrontendMapping(const SoapySDRDevice *device, const int direction)
{
    __SOAPY_SDR_C_TRY
    return toCString(device->getFrontendMapping(direction));
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

size_t SoapySDRDevice_getNumChannels(const SoapySDRDevice *device, const int direction)
{
    __SOAPY_SDR_C_TRY
    return device->getNumChannels(direction);
    __SOAPY_SDR_C_CATCH_RET(std::string::npos);
}

SoapySDRKwargs SoapySDRDevice_getChannelInfo(const SoapySDRDevice *device, const int direction, const size_t channel)
{
    __SOAPY_SDR_C_TRY
    return toKwargs(device->getChannelInfo(direction, channel));
    __SOAPY_SDR_C_CATCH_RET(toKwargs(SoapySDR::Kwargs()));
}

bool SoapySDRDevice_getFullDuplex(const SoapySDRDevice *device, const int direction, const size_t channel)
{
    __SOAPY_SDR_C_TRY
    return device->getFullDuplex(direction, channel);
    __SOAPY_SDR_C_CATCH_RET(SoapySDRBoolErr);
}

/*******************************************************************
 * Stream API
 ******************************************************************/
char **SoapySDRDevice_getStreamFormats(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length)
{
    *length = 0;
    __SOAPY_SDR_C_TRY
    return toStrArray(device->getStreamFormats(direction, channel), length);
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

char *SoapySDRDevice_getNativeStreamFormat(const SoapySDRDevice *device, const int direction, const size_t channel, double *fullScale)
{
    __SOAPY_SDR_C_TRY
    return toCString(device->getNativeStreamFormat(direction, channel, *fullScale));
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

SoapySDRArgInfo *SoapySDRDevice_getStreamArgsInfo(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length)
{
    *length = 0;
    __SOAPY_SDR_C_TRY
    return toArgInfoList(device->getStreamArgsInfo(direction, channel), length);
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

#undef SoapySDRDevice_setupStream

SoapySDRStream *SoapySDRDevice_setupStream(SoapySDRDevice *device, const int direction, const char *format, const size_t *channels, const size_t numChans, const SoapySDRKwargs *args)
{
    __SOAPY_SDR_C_TRY
    return reinterpret_cast<SoapySDRStream *>(device->setupStream(direction, format, std::vector<size_t>(channels, channels+numChans), toKwargs(args)));
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

int SoapySDRDevice_closeStream(SoapySDRDevice *device, SoapySDRStream *stream)
{
    __SOAPY_SDR_C_TRY
    device->closeStream(reinterpret_cast<SoapySDR::Stream *>(stream));
    __SOAPY_SDR_C_CATCH
}

size_t SoapySDRDevice_getStreamMTU(const SoapySDRDevice *device, SoapySDRStream *stream)
{
    __SOAPY_SDR_C_TRY
    return device->getStreamMTU(reinterpret_cast<SoapySDR::Stream *>(stream));
    __SOAPY_SDR_C_CATCH_RET(std::string::npos);
}

int SoapySDRDevice_activateStream(SoapySDRDevice *device,
    SoapySDRStream *stream,
    const int flags,
    const long long timeNs,
    const size_t numElems)
{
    __SOAPY_SDR_C_TRY
    return device->activateStream(reinterpret_cast<SoapySDR::Stream *>(stream), flags, timeNs, numElems);
    __SOAPY_SDR_C_CATCH_RET(SOAPY_SDR_STREAM_ERROR);
}

int SoapySDRDevice_deactivateStream(SoapySDRDevice *device,
    SoapySDRStream *stream,
    const int flags,
    const long long timeNs)
{
    __SOAPY_SDR_C_TRY
    return device->deactivateStream(reinterpret_cast<SoapySDR::Stream *>(stream), flags, timeNs);
    __SOAPY_SDR_C_CATCH_RET(SOAPY_SDR_STREAM_ERROR);
}

int SoapySDRDevice_readStream(SoapySDRDevice *device, SoapySDRStream *stream, void * const *buffs, const size_t numElems, int *flags, long long *timeNs, const long timeoutUs)
{
    __SOAPY_SDR_C_TRY
    return device->readStream(reinterpret_cast<SoapySDR::Stream *>(stream), buffs, numElems, *flags, *timeNs, timeoutUs);
    __SOAPY_SDR_C_CATCH_RET(SOAPY_SDR_STREAM_ERROR);
}

int SoapySDRDevice_writeStream(SoapySDRDevice *device, SoapySDRStream *stream, const void * const *buffs, const size_t numElems, int *flags, const long long timeNs, const long timeoutUs)
{
    __SOAPY_SDR_C_TRY
    return device->writeStream(reinterpret_cast<SoapySDR::Stream *>(stream), buffs, numElems, *flags, timeNs, timeoutUs);
    __SOAPY_SDR_C_CATCH_RET(SOAPY_SDR_STREAM_ERROR);
}

int SoapySDRDevice_readStreamStatus(SoapySDRDevice *device, SoapySDRStream *stream, size_t *chanMask, int *flags, long long *timeNs, const long timeoutUs)
{
    __SOAPY_SDR_C_TRY
    return device->readStreamStatus(reinterpret_cast<SoapySDR::Stream *>(stream), *chanMask, *flags, *timeNs, timeoutUs);
    __SOAPY_SDR_C_CATCH_RET(SOAPY_SDR_STREAM_ERROR);
}

/*******************************************************************
 * Direct buffer access API
 ******************************************************************/
size_t SoapySDRDevice_getNumDirectAccessBuffers(SoapySDRDevice *device, SoapySDRStream *stream)
{
    __SOAPY_SDR_C_TRY
    return device->getNumDirectAccessBuffers(reinterpret_cast<SoapySDR::Stream *>(stream));
    __SOAPY_SDR_C_CATCH_RET(std::string::npos);
}

int SoapySDRDevice_getDirectAccessBufferAddrs(SoapySDRDevice *device, SoapySDRStream *stream, const size_t handle, void **buffs)
{
    __SOAPY_SDR_C_TRY
    return device->getDirectAccessBufferAddrs(reinterpret_cast<SoapySDR::Stream *>(stream), handle, buffs);
    __SOAPY_SDR_C_CATCH
}

int SoapySDRDevice_acquireReadBuffer(SoapySDRDevice *device,
    SoapySDRStream *stream,
    size_t *handle,
    const void **buffs,
    int *flags,
    long long *timeNs,
    const long timeoutUs)
{
    __SOAPY_SDR_C_TRY
    return device->acquireReadBuffer(reinterpret_cast<SoapySDR::Stream *>(stream), *handle, buffs, *flags, *timeNs, timeoutUs);
    __SOAPY_SDR_C_CATCH_RET(SOAPY_SDR_STREAM_ERROR);
}

void SoapySDRDevice_releaseReadBuffer(SoapySDRDevice *device,
    SoapySDRStream *stream,
    const size_t handle)
{
    __SOAPY_SDR_C_TRY
    return device->releaseReadBuffer(reinterpret_cast<SoapySDR::Stream *>(stream), handle);
    __SOAPY_SDR_C_CATCH_RET(SoapySDRVoidRet);
}

int SoapySDRDevice_acquireWriteBuffer(SoapySDRDevice *device,
    SoapySDRStream *stream,
    size_t *handle,
    void **buffs,
    const long timeoutUs)
{
    __SOAPY_SDR_C_TRY
    return device->acquireWriteBuffer(reinterpret_cast<SoapySDR::Stream *>(stream), *handle, buffs, timeoutUs);
    __SOAPY_SDR_C_CATCH_RET(SOAPY_SDR_STREAM_ERROR);
}

void SoapySDRDevice_releaseWriteBuffer(SoapySDRDevice *device,
    SoapySDRStream *stream,
    const size_t handle,
    const size_t numElems,
    int *flags,
    const long long timeNs)
{
    __SOAPY_SDR_C_TRY
    return device->releaseWriteBuffer(reinterpret_cast<SoapySDR::Stream *>(stream), handle, numElems, *flags, timeNs);
    __SOAPY_SDR_C_CATCH_RET(SoapySDRVoidRet);
}

/*******************************************************************
 * Antenna API
 ******************************************************************/
char **SoapySDRDevice_listAntennas(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length)
{
    *length = 0;
    __SOAPY_SDR_C_TRY
    return toStrArray(device->listAntennas(direction, channel), length);
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

int SoapySDRDevice_setAntenna(SoapySDRDevice *device, const int direction, const size_t channel, const char *name)
{
    __SOAPY_SDR_C_TRY
    device->setAntenna(direction, channel, name);
    __SOAPY_SDR_C_CATCH
}

char *SoapySDRDevice_getAntenna(const SoapySDRDevice *device, const int direction, const size_t channel)
{
    __SOAPY_SDR_C_TRY
    return toCString(device->getAntenna(direction, channel));
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

/*******************************************************************
 * Frontend corrections API
 ******************************************************************/
bool SoapySDRDevice_hasDCOffsetMode(const SoapySDRDevice *device, const int direction, const size_t channel)
{
    __SOAPY_SDR_C_TRY
    return device->hasDCOffsetMode(direction, channel);
    __SOAPY_SDR_C_CATCH_RET(SoapySDRBoolErr);
}

int SoapySDRDevice_setDCOffsetMode(SoapySDRDevice *device, const int direction, const size_t channel, const bool automatic)
{
    __SOAPY_SDR_C_TRY
    device->setDCOffsetMode(direction, channel, automatic);
    __SOAPY_SDR_C_CATCH
}

bool SoapySDRDevice_getDCOffsetMode(const SoapySDRDevice *device, const int direction, const size_t channel)
{
    __SOAPY_SDR_C_TRY
    return device->getDCOffsetMode(direction, channel);
    __SOAPY_SDR_C_CATCH_RET(SoapySDRBoolErr);
}

bool SoapySDRDevice_hasDCOffset(const SoapySDRDevice *device, const int direction, const size_t channel)
{
    __SOAPY_SDR_C_TRY
    return device->hasDCOffset(direction, channel);
    __SOAPY_SDR_C_CATCH_RET(SoapySDRBoolErr);
}

int SoapySDRDevice_setDCOffset(SoapySDRDevice *device, const int direction, const size_t channel, const double offsetI, const double offsetQ)
{
    __SOAPY_SDR_C_TRY
    device->setDCOffset(direction, channel, std::complex<double>(offsetI, offsetQ));
    __SOAPY_SDR_C_CATCH
}

int SoapySDRDevice_getDCOffset(const SoapySDRDevice *device, const int direction, const size_t channel, double *offsetI, double *offsetQ)
{
    __SOAPY_SDR_C_TRY
    std::complex<double> ret = device->getDCOffset(direction, channel);
    *offsetI = ret.real();
    *offsetQ = ret.imag();
    __SOAPY_SDR_C_CATCH
}

bool SoapySDRDevice_hasIQBalance(const SoapySDRDevice *device, const int direction, const size_t channel)
{
    __SOAPY_SDR_C_TRY
    return device->hasIQBalance(direction, channel);
    __SOAPY_SDR_C_CATCH_RET(SoapySDRBoolErr);
}

int SoapySDRDevice_setIQBalance(SoapySDRDevice *device, const int direction, const size_t channel, const double balanceI, const double balanceQ)
{
    __SOAPY_SDR_C_TRY
    device->setIQBalance(direction, channel, std::complex<double>(balanceI, balanceQ));
    __SOAPY_SDR_C_CATCH
}

int SoapySDRDevice_getIQBalance(const SoapySDRDevice *device, const int direction, const size_t channel, double *balanceI, double *balanceQ)
{
    __SOAPY_SDR_C_TRY
    std::complex<double> ret = device->getIQBalance(direction, channel);
    *balanceI = ret.real();
    *balanceQ = ret.imag();
    __SOAPY_SDR_C_CATCH
}

bool SoapySDRDevice_hasIQBalanceMode(const SoapySDRDevice *device, const int direction, const size_t channel)
{
    __SOAPY_SDR_C_TRY
    return device->hasIQBalanceMode(direction, channel);
    __SOAPY_SDR_C_CATCH_RET(SoapySDRBoolErr);
}

int SoapySDRDevice_setIQBalanceMode(SoapySDRDevice *device, const int direction, const size_t channel, const bool automatic)
{
    __SOAPY_SDR_C_TRY
    device->setIQBalance(direction, channel, automatic);
    __SOAPY_SDR_C_CATCH
}

bool SoapySDRDevice_getIQBalanceMode(const SoapySDRDevice *device, const int direction, const size_t channel)
{
    __SOAPY_SDR_C_TRY
    return device->getIQBalanceMode(direction, channel);
    __SOAPY_SDR_C_CATCH_RET(SoapySDRBoolErr);
}

bool SoapySDRDevice_hasFrequencyCorrection(const SoapySDRDevice *device, const int direction, const size_t channel)
{
    __SOAPY_SDR_C_TRY
    return device->hasFrequencyCorrection(direction, channel);
    __SOAPY_SDR_C_CATCH_RET(SoapySDRBoolErr);
}

int SoapySDRDevice_setFrequencyCorrection(SoapySDRDevice *device, const int direction, const size_t channel, const double value)
{
    __SOAPY_SDR_C_TRY
    device->setFrequencyCorrection(direction, channel, value);
    __SOAPY_SDR_C_CATCH
}

double SoapySDRDevice_getFrequencyCorrection(const SoapySDRDevice *device, const int direction, const size_t channel)
{
    __SOAPY_SDR_C_TRY
    return device->getFrequencyCorrection(direction, channel);
    __SOAPY_SDR_C_CATCH_RET(NAN);
}

/*******************************************************************
 * Gain API
 ******************************************************************/
char **SoapySDRDevice_listGains(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length)
{
    *length = 0;
    __SOAPY_SDR_C_TRY
    return toStrArray(device->listGains(direction, channel), length);
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

bool SoapySDRDevice_hasGainMode(const SoapySDRDevice *device, const int direction, const size_t channel)
{
    __SOAPY_SDR_C_TRY
    return device->hasGainMode(direction, channel);
    __SOAPY_SDR_C_CATCH_RET(SoapySDRBoolErr);
}

int SoapySDRDevice_setGainMode(SoapySDRDevice *device, const int direction, const size_t channel, const bool automatic)
{
    __SOAPY_SDR_C_TRY
    device->setGainMode(direction, channel, automatic);
    __SOAPY_SDR_C_CATCH
}

bool SoapySDRDevice_getGainMode(const SoapySDRDevice *device, const int direction, const size_t channel)
{
    __SOAPY_SDR_C_TRY
    return device->getGainMode(direction, channel);
    __SOAPY_SDR_C_CATCH_RET(SoapySDRBoolErr);
}

int SoapySDRDevice_setGain(SoapySDRDevice *device, const int direction, const size_t channel, const double value)
{
    __SOAPY_SDR_C_TRY
    device->setGain(direction, channel, value);
    __SOAPY_SDR_C_CATCH
}

int SoapySDRDevice_setGainElement(SoapySDRDevice *device, const int direction, const size_t channel, const char *name, const double value)
{
    __SOAPY_SDR_C_TRY
    device->setGain(direction, channel, name, value);
    __SOAPY_SDR_C_CATCH
}

double SoapySDRDevice_getGain(const SoapySDRDevice *device, const int direction, const size_t channel)
{
    __SOAPY_SDR_C_TRY
    return device->getGain(direction, channel);
    __SOAPY_SDR_C_CATCH_RET(NAN);
}

double SoapySDRDevice_getGainElement(const SoapySDRDevice *device, const int direction, const size_t channel, const char *name)
{
    __SOAPY_SDR_C_TRY
    return device->getGain(direction, channel, name);
    __SOAPY_SDR_C_CATCH_RET(NAN);
}

SoapySDRRange SoapySDRDevice_getGainRange(const SoapySDRDevice *device, const int direction, const size_t channel)
{
    __SOAPY_SDR_C_TRY
    return toRange(device->getGainRange(direction, channel));
    __SOAPY_SDR_C_CATCH_RET(SoapySDRRangeNAN);
}

SoapySDRRange SoapySDRDevice_getGainElementRange(const SoapySDRDevice *device, const int direction, const size_t channel, const char *name)
{
    __SOAPY_SDR_C_TRY
    return toRange(device->getGainRange(direction, channel, name));
    __SOAPY_SDR_C_CATCH_RET(SoapySDRRangeNAN);
}

/*******************************************************************
 * Frequency API
 ******************************************************************/
int SoapySDRDevice_setFrequency(SoapySDRDevice *device, const int direction, const size_t channel, const double frequency, const SoapySDRKwargs *args)
{
    __SOAPY_SDR_C_TRY
    device->setFrequency(direction, channel, frequency, toKwargs(args));
    __SOAPY_SDR_C_CATCH
}

int SoapySDRDevice_setFrequencyComponent(SoapySDRDevice *device, const int direction, const size_t channel, const char *name, const double frequency, const SoapySDRKwargs *args)
{
    __SOAPY_SDR_C_TRY
    device->setFrequency(direction, channel, name, frequency, toKwargs(args));
    __SOAPY_SDR_C_CATCH
}

double SoapySDRDevice_getFrequency(const SoapySDRDevice *device, const int direction, const size_t channel)
{
    __SOAPY_SDR_C_TRY
    return device->getFrequency(direction, channel);
    __SOAPY_SDR_C_CATCH_RET(NAN);
}

double SoapySDRDevice_getFrequencyComponent(const SoapySDRDevice *device, const int direction, const size_t channel, const char *name)
{
    __SOAPY_SDR_C_TRY
    return device->getFrequency(direction, channel, name);
    __SOAPY_SDR_C_CATCH_RET(NAN);
}

char **SoapySDRDevice_listFrequencies(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length)
{
    *length = 0;
    __SOAPY_SDR_C_TRY
    return toStrArray(device->listFrequencies(direction, channel), length);
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

SoapySDRRange *SoapySDRDevice_getFrequencyRange(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length)
{
    *length = 0;
    __SOAPY_SDR_C_TRY
    return toRangeList(device->getFrequencyRange(direction, channel), length);
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

SoapySDRRange *SoapySDRDevice_getFrequencyRangeComponent(const SoapySDRDevice *device, const int direction, const size_t channel, const char *name, size_t *length)
{
    *length = 0;
    __SOAPY_SDR_C_TRY
    return toRangeList(device->getFrequencyRange(direction, channel, name), length);
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

SoapySDRArgInfo *SoapySDRDevice_getFrequencyArgsInfo(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length)
{
    *length = 0;
    __SOAPY_SDR_C_TRY
    return toArgInfoList(device->getFrequencyArgsInfo(direction, channel), length);
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

/*******************************************************************
 * Sample Rate API
 ******************************************************************/
int SoapySDRDevice_setSampleRate(SoapySDRDevice *device, const int direction, const size_t channel, const double rate)
{
    __SOAPY_SDR_C_TRY
    device->setSampleRate(direction, channel, rate);
    __SOAPY_SDR_C_CATCH
}

double SoapySDRDevice_getSampleRate(const SoapySDRDevice *device, const int direction, const size_t channel)
{
    __SOAPY_SDR_C_TRY
    return device->getSampleRate(direction, channel);
    __SOAPY_SDR_C_CATCH_RET(NAN);
}

double *SoapySDRDevice_listSampleRates(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length)
{
    *length = 0;
    __SOAPY_SDR_C_TRY
    return toNumericList(device->listSampleRates(direction, channel), length);
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

SoapySDRRange *SoapySDRDevice_getSampleRateRange(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length)
{
    *length = 0;
    __SOAPY_SDR_C_TRY
    return toRangeList(device->getSampleRateRange(direction, channel), length);
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

/*******************************************************************
 * Bandwidth API
 ******************************************************************/
int SoapySDRDevice_setBandwidth(SoapySDRDevice *device, const int direction, const size_t channel, const double bw)
{
    __SOAPY_SDR_C_TRY
    device->setBandwidth(direction, channel, bw);
    __SOAPY_SDR_C_CATCH
}

double SoapySDRDevice_getBandwidth(const SoapySDRDevice *device, const int direction, const size_t channel)
{
    __SOAPY_SDR_C_TRY
    return device->getBandwidth(direction, channel);
    __SOAPY_SDR_C_CATCH_RET(NAN);
}

double *SoapySDRDevice_listBandwidths(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length)
{
    *length = 0;
    __SOAPY_SDR_C_TRY
    return toNumericList(device->listBandwidths(direction, channel), length);
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

SoapySDRRange *SoapySDRDevice_getBandwidthRange(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length)
{
    *length = 0;
    __SOAPY_SDR_C_TRY
    return toRangeList(device->getBandwidthRange(direction, channel), length);
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

/*******************************************************************
 * Clocking API
 ******************************************************************/
int SoapySDRDevice_setMasterClockRate(SoapySDRDevice *device, const double rate)
{
    __SOAPY_SDR_C_TRY
    device->setMasterClockRate(rate);
    __SOAPY_SDR_C_CATCH
}

double SoapySDRDevice_getMasterClockRate(const SoapySDRDevice *device)
{
    __SOAPY_SDR_C_TRY
    return device->getMasterClockRate();
    __SOAPY_SDR_C_CATCH_RET(NAN);
}

SoapySDRRange *SoapySDRDevice_getMasterClockRates(const SoapySDRDevice *device, size_t *length)
{
    *length = 0;
    __SOAPY_SDR_C_TRY
    return toRangeList(device->getMasterClockRates(), length);
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

int SoapySDRDevice_setReferenceClockRate(SoapySDRDevice *device, const double rate)
{
    __SOAPY_SDR_C_TRY
    device->setReferenceClockRate(rate);
    __SOAPY_SDR_C_CATCH
}

double SoapySDRDevice_getReferenceClockRate(const SoapySDRDevice *device)
{
    __SOAPY_SDR_C_TRY
    return device->getReferenceClockRate();
    __SOAPY_SDR_C_CATCH_RET(NAN);
}

SoapySDRRange *SoapySDRDevice_getReferenceClockRates(const SoapySDRDevice *device, size_t *length)
{
    *length = 0;
    __SOAPY_SDR_C_TRY
    return toRangeList(device->getReferenceClockRates(), length);
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

char **SoapySDRDevice_listClockSources(const SoapySDRDevice *device, size_t *length)
{
    *length = 0;
    __SOAPY_SDR_C_TRY
    return toStrArray(device->listClockSources(), length);
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

int SoapySDRDevice_setClockSource(SoapySDRDevice *device, const char *source)
{
    __SOAPY_SDR_C_TRY
    device->setClockSource(source);
    __SOAPY_SDR_C_CATCH
}

char *SoapySDRDevice_getClockSource(const SoapySDRDevice *device)
{
    __SOAPY_SDR_C_TRY
    return toCString(device->getClockSource());
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

/*******************************************************************
 * Time API
 ******************************************************************/

char **SoapySDRDevice_listTimeSources(const SoapySDRDevice *device, size_t *length)
{
    *length = 0;
    __SOAPY_SDR_C_TRY
    return toStrArray(device->listTimeSources(), length);
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

int SoapySDRDevice_setTimeSource(SoapySDRDevice *device, const char *source)
{
    __SOAPY_SDR_C_TRY
    device->setTimeSource(source);
    __SOAPY_SDR_C_CATCH
}

char *SoapySDRDevice_getTimeSource(const SoapySDRDevice *device)
{
    __SOAPY_SDR_C_TRY
    return toCString(device->getTimeSource());
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

bool SoapySDRDevice_hasHardwareTime(const SoapySDRDevice *device, const char *what)
{
    __SOAPY_SDR_C_TRY
    return device->hasHardwareTime(what);
    __SOAPY_SDR_C_CATCH_RET(SoapySDRBoolErr);
}

long long SoapySDRDevice_getHardwareTime(const SoapySDRDevice *device, const char *what)
{
    __SOAPY_SDR_C_TRY
    return device->getHardwareTime(what);
    __SOAPY_SDR_C_CATCH
}

int SoapySDRDevice_setHardwareTime(SoapySDRDevice *device, const long long timeNs, const char *what)
{
    __SOAPY_SDR_C_TRY
    device->setHardwareTime(timeNs, what);
    __SOAPY_SDR_C_CATCH
}

int SoapySDRDevice_setCommandTime(SoapySDRDevice *device, const long long timeNs, const char *what)
{
    __SOAPY_SDR_C_TRY
    device->setCommandTime(timeNs, what);
    __SOAPY_SDR_C_CATCH
}

/*******************************************************************
 * Sensor API
 ******************************************************************/
char **SoapySDRDevice_listSensors(const SoapySDRDevice *device, size_t *length)
{
    *length = 0;
    __SOAPY_SDR_C_TRY
    return toStrArray(device->listSensors(), length);
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

SoapySDRArgInfo SoapySDRDevice_getSensorInfo(const SoapySDRDevice *device, const char *key)
{
    __SOAPY_SDR_C_TRY
    return toArgInfo(device->getSensorInfo(key));
    __SOAPY_SDR_C_CATCH_RET(SoapySDRArgInfoNull());
}

char *SoapySDRDevice_readSensor(const SoapySDRDevice *device, const char *key)
{
    __SOAPY_SDR_C_TRY
    return toCString(device->readSensor(key));
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

char **SoapySDRDevice_listChannelSensors(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length)
{
    *length = 0;
    __SOAPY_SDR_C_TRY
    return toStrArray(device->listSensors(direction, channel), length);
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

SoapySDRArgInfo SoapySDRDevice_getChannelSensorInfo(const SoapySDRDevice *device, const int direction, const size_t channel, const char *key)
{
    __SOAPY_SDR_C_TRY
    return toArgInfo(device->getSensorInfo(direction, channel, key));
    __SOAPY_SDR_C_CATCH_RET(SoapySDRArgInfoNull());
}

char *SoapySDRDevice_readChannelSensor(const SoapySDRDevice *device, const int direction, const size_t channel, const char *key)
{
    __SOAPY_SDR_C_TRY
    return toCString(device->readSensor(direction, channel, key));
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

/*******************************************************************
 * Register API
 ******************************************************************/
char **SoapySDRDevice_listRegisterInterfaces(const SoapySDRDevice *device, size_t *length)
{
    *length = 0;
    __SOAPY_SDR_C_TRY
    return toStrArray(device->listRegisterInterfaces(), length);
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

int SoapySDRDevice_writeRegister(SoapySDRDevice *device, const char *name, const unsigned addr, const unsigned value)
{
    __SOAPY_SDR_C_TRY
    device->writeRegister(name, addr, value);
    __SOAPY_SDR_C_CATCH
}

unsigned SoapySDRDevice_readRegister(const SoapySDRDevice *device, const char *name, const unsigned addr)
{
    __SOAPY_SDR_C_TRY
    return device->readRegister(name, addr);
    __SOAPY_SDR_C_CATCH
}

int SoapySDRDevice_writeRegisters(SoapySDRDevice *device, const char *name, const unsigned addr, const unsigned *value, const size_t length)
{
    __SOAPY_SDR_C_TRY
    device->writeRegisters(name, addr, toNumericVector(value, length));
    __SOAPY_SDR_C_CATCH
}

unsigned *SoapySDRDevice_readRegisters(const SoapySDRDevice *device, const char *name, const unsigned addr, size_t *length)
{
    const size_t inputLen = *length;
    *length = 0; //clear in case of error

    __SOAPY_SDR_C_TRY
    return toNumericList(device->readRegisters(name, addr, inputLen), length);
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

/*******************************************************************
 * Settings API
 ******************************************************************/
SoapySDRArgInfo *SoapySDRDevice_getSettingInfo(const SoapySDRDevice *device, size_t *length)
{
    *length = 0;
    __SOAPY_SDR_C_TRY
    return toArgInfoList(device->getSettingInfo(), length);
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

SoapySDRArgInfo SoapySDRDevice_getSettingInfoWithKey(const SoapySDRDevice *device, const char *key)
{
    __SOAPY_SDR_C_TRY
    return toArgInfo(device->getSettingInfo(key));
    __SOAPY_SDR_C_CATCH_RET(SoapySDRArgInfo())
}

int SoapySDRDevice_writeSetting(SoapySDRDevice *device, const char *key, const char *value)
{
    __SOAPY_SDR_C_TRY
    device->writeSetting(key, value);
    __SOAPY_SDR_C_CATCH
}

char *SoapySDRDevice_readSetting(const SoapySDRDevice *device, const char *key)
{
    __SOAPY_SDR_C_TRY
    return toCString(device->readSetting(key));
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

SoapySDRArgInfo *SoapySDRDevice_getChannelSettingInfo(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length)
{
    *length = 0;
    __SOAPY_SDR_C_TRY
    return toArgInfoList(device->getSettingInfo(direction, channel), length);
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

SoapySDRArgInfo SoapySDRDevice_getChannelSettingInfoWithKey(const SoapySDRDevice *device, const int direction, const size_t channel, const char *key)
{
    __SOAPY_SDR_C_TRY
    return toArgInfo(device->getSettingInfo(direction, channel, key));
    __SOAPY_SDR_C_CATCH_RET(SoapySDRArgInfo())
}

int SoapySDRDevice_writeChannelSetting(SoapySDRDevice *device, const int direction, const size_t channel, const char *key, const char *value)
{
    __SOAPY_SDR_C_TRY
    device->writeSetting(direction, channel, key, value);
    __SOAPY_SDR_C_CATCH
}

char *SoapySDRDevice_readChannelSetting(const SoapySDRDevice *device, const int direction, const size_t channel, const char *key)
{
    __SOAPY_SDR_C_TRY
    return toCString(device->readSetting(direction, channel, key));
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

/*******************************************************************
 * GPIO API
 ******************************************************************/
char **SoapySDRDevice_listGPIOBanks(const SoapySDRDevice *device, size_t *length)
{
    *length = 0;
    __SOAPY_SDR_C_TRY
    return toStrArray(device->listGPIOBanks(), length);
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

int SoapySDRDevice_writeGPIO(SoapySDRDevice *device, const char *bank, const unsigned value)
{
    __SOAPY_SDR_C_TRY
    device->writeGPIO(bank, value);
    __SOAPY_SDR_C_CATCH
}

int SoapySDRDevice_writeGPIOMasked(SoapySDRDevice *device, const char *bank, const unsigned value, const unsigned mask)
{
    __SOAPY_SDR_C_TRY
    device->writeGPIO(bank, value, mask);
    __SOAPY_SDR_C_CATCH
}

unsigned SoapySDRDevice_readGPIO(const SoapySDRDevice *device, const char *bank)
{
    __SOAPY_SDR_C_TRY
    return device->readGPIO(bank);
    __SOAPY_SDR_C_CATCH
}

int SoapySDRDevice_writeGPIODir(SoapySDRDevice *device, const char *bank, const unsigned dir)
{
    __SOAPY_SDR_C_TRY
    device->writeGPIODir(bank, dir);
    __SOAPY_SDR_C_CATCH
}

int SoapySDRDevice_writeGPIODirMasked(SoapySDRDevice *device, const char *bank, const unsigned dir, const unsigned mask)
{
    __SOAPY_SDR_C_TRY
    device->writeGPIODir(bank, dir, mask);
    __SOAPY_SDR_C_CATCH
}

unsigned SoapySDRDevice_readGPIODir(const SoapySDRDevice *device, const char *bank)
{
    __SOAPY_SDR_C_TRY
    return device->readGPIODir(bank);
    __SOAPY_SDR_C_CATCH
}

/*******************************************************************
 * I2C API
 ******************************************************************/
int SoapySDRDevice_writeI2C(SoapySDRDevice *device, const int addr, const char *data, const size_t numBytes)
{
    __SOAPY_SDR_C_TRY
    device->writeI2C(addr, std::string(data, numBytes));
    __SOAPY_SDR_C_CATCH
}

char *SoapySDRDevice_readI2C(SoapySDRDevice *device, const int addr, size_t *numBytes)
{
    const size_t inputNumBytes = *numBytes;
    *numBytes = 0; //clear in case of error

    __SOAPY_SDR_C_TRY
    const auto bytes = device->readI2C(addr, inputNumBytes);
    char *buff = callocArrayType<char>(bytes.size());
    std::copy(bytes.begin(), bytes.end(), buff);
    *numBytes = bytes.size();
    return buff;
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

/*******************************************************************
 * SPI API
 ******************************************************************/
unsigned SoapySDRDevice_transactSPI(SoapySDRDevice *device, const int addr, const unsigned data, const size_t numBits)
{
    __SOAPY_SDR_C_TRY
    return device->transactSPI(addr, data, numBits);
    __SOAPY_SDR_C_CATCH
}

/*******************************************************************
 * UART API
 ******************************************************************/
char **SoapySDRDevice_listUARTs(const SoapySDRDevice *device, size_t *length)
{
    *length = 0;
    __SOAPY_SDR_C_TRY
    return toStrArray(device->listUARTs(), length);
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

int SoapySDRDevice_writeUART(SoapySDRDevice *device, const char *which, const char *data)
{
    __SOAPY_SDR_C_TRY
    device->writeUART(which, data);
    __SOAPY_SDR_C_CATCH
}

char *SoapySDRDevice_readUART(const SoapySDRDevice *device, const char *which, const long timeoutUs)
{
    __SOAPY_SDR_C_TRY
    return toCString(device->readUART(which, timeoutUs));
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

/*******************************************************************
 * Native Access API
 ******************************************************************/
void* SoapySDRDevice_getNativeDeviceHandle(const SoapySDRDevice *device)
{
    __SOAPY_SDR_C_TRY
    return device->getNativeDeviceHandle();
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

} //extern "C"
