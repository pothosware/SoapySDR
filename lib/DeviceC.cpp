// Copyright (c) 2014-2016 Josh Blum
// Copyright (c) 2016-2016 Bastille Networks
// SPDX-License-Identifier: BSL-1.0

#include "TypeHelpers.hpp"
#include <SoapySDR/Device.h>
#include <SoapySDR/Device.hpp>
#include <algorithm>
#include <cstdlib>
#include <cstring>

/*******************************************************************
 * Helper macros for dealing with error messages
 ******************************************************************/
#define __SOAPY_SDR_C_TRY try {
#define __SOAPY_SDR_C_CATCH } \
    catch (const std::exception &ex) { return SoapySDRDevice_reportError(ex.what()); } \
    catch (...) { return SoapySDRDevice_reportError("unknown"); } \
    return 0;

#ifdef _MSC_VER
#define __thread __declspec(thread)
#endif

static __thread char lastErrorMsg[1024];

static int SoapySDRDevice_reportError(const char *msg)
{
    strncpy(lastErrorMsg, msg, sizeof(lastErrorMsg));
    lastErrorMsg[sizeof(lastErrorMsg)-1] = '\0';
    return -1;
}

const char *SoapySDRDevice_lastError(void)
{
    return lastErrorMsg;
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
    return strdup(device->getDriverKey().c_str());
}

char *SoapySDRDevice_getHardwareKey(const SoapySDRDevice *device)
{
    return strdup(device->getHardwareKey().c_str());
}

SoapySDRKwargs SoapySDRDevice_getHardwareInfo(const SoapySDRDevice *device)
{
    return toKwargs(device->getHardwareInfo());
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
    return strdup(device->getFrontendMapping(direction).c_str());
}

size_t SoapySDRDevice_getNumChannels(const SoapySDRDevice *device, const int direction)
{
    return device->getNumChannels(direction);
}

SoapySDRKwargs SoapySDRDevice_getChannelInfo(const SoapySDRDevice *device, const int direction, const size_t channel)
{
    return toKwargs(device->getChannelInfo(direction, channel));
}

bool SoapySDRDevice_getFullDuplex(const SoapySDRDevice *device, const int direction, const size_t channel)
{
    return device->getFullDuplex(direction, channel);
}

/*******************************************************************
 * Stream API
 ******************************************************************/
char **SoapySDRDevice_getStreamFormats(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length)
{
    return toStrArray(device->getStreamFormats(direction, channel), length);
}

char *SoapySDRDevice_getNativeStreamFormat(const SoapySDRDevice *device, const int direction, const size_t channel, double *fullScale)
{
    return strdup(device->getNativeStreamFormat(direction, channel, *fullScale).c_str());
}

SoapySDRArgInfo *SoapySDRDevice_getStreamArgsInfo(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length)
{
    return toArgInfoList(device->getStreamArgsInfo(direction, channel), length);
}

int SoapySDRDevice_setupStream(SoapySDRDevice *device, SoapySDRStream **stream, const int direction, const char *format, const size_t *channels, const size_t numChans, const SoapySDRKwargs *args)
{
    __SOAPY_SDR_C_TRY
    *stream = reinterpret_cast<SoapySDRStream *>(device->setupStream(direction, format, std::vector<size_t>(channels, channels+numChans), toKwargs(args)));
    __SOAPY_SDR_C_CATCH
}

void SoapySDRDevice_closeStream(SoapySDRDevice *device, SoapySDRStream *stream)
{
    return device->closeStream(reinterpret_cast<SoapySDR::Stream *>(stream));
}

size_t SoapySDRDevice_getStreamMTU(const SoapySDRDevice *device, SoapySDRStream *stream)
{
    return device->getStreamMTU(reinterpret_cast<SoapySDR::Stream *>(stream));
}

int SoapySDRDevice_activateStream(SoapySDRDevice *device,
    SoapySDRStream *stream,
    const int flags,
    const long long timeNs,
    const size_t numElems)
{
    return device->activateStream(reinterpret_cast<SoapySDR::Stream *>(stream), flags, timeNs, numElems);
}

int SoapySDRDevice_deactivateStream(SoapySDRDevice *device,
    SoapySDRStream *stream,
    const int flags,
    const long long timeNs)
{
    return device->deactivateStream(reinterpret_cast<SoapySDR::Stream *>(stream), flags, timeNs);
}

int SoapySDRDevice_readStream(SoapySDRDevice *device, SoapySDRStream *stream, void * const *buffs, const size_t numElems, int *flags, long long *timeNs, const long timeoutUs)
{
    return device->readStream(reinterpret_cast<SoapySDR::Stream *>(stream), buffs, numElems, *flags, *timeNs, timeoutUs);
}

int SoapySDRDevice_writeStream(SoapySDRDevice *device, SoapySDRStream *stream, const void * const *buffs, const size_t numElems, int *flags, const long long timeNs, const long timeoutUs)
{
    return device->writeStream(reinterpret_cast<SoapySDR::Stream *>(stream), buffs, numElems, *flags, timeNs, timeoutUs);
}

int SoapySDRDevice_readStreamStatus(SoapySDRDevice *device, SoapySDRStream *stream, size_t *chanMask, int *flags, long long *timeNs, const long timeoutUs)
{
    return device->readStreamStatus(reinterpret_cast<SoapySDR::Stream *>(stream), *chanMask, *flags, *timeNs, timeoutUs);
}


/*******************************************************************
 * Direct buffer access API
 ******************************************************************/
size_t SoapySDRDevice_getNumDirectAccessBuffers(SoapySDRDevice *device, SoapySDRStream *stream)
{
    return device->getNumDirectAccessBuffers(reinterpret_cast<SoapySDR::Stream *>(stream));
}

int SoapySDRDevice_getDirectAccessBufferAddrs(SoapySDRDevice *device, SoapySDRStream *stream, const size_t handle, void **buffs)
{
    return device->getDirectAccessBufferAddrs(reinterpret_cast<SoapySDR::Stream *>(stream), handle, buffs);
}

int SoapySDRDevice_acquireReadBuffer(SoapySDRDevice *device,
    SoapySDRStream *stream,
    size_t *handle,
    const void **buffs,
    int *flags,
    long long *timeNs,
    const long timeoutUs)
{
    return device->acquireReadBuffer(reinterpret_cast<SoapySDR::Stream *>(stream), *handle, buffs, *flags, *timeNs, timeoutUs);
}

void SoapySDRDevice_releaseReadBuffer(SoapySDRDevice *device,
    SoapySDRStream *stream,
    const size_t handle)
{
    return device->releaseReadBuffer(reinterpret_cast<SoapySDR::Stream *>(stream), handle);
}

int SoapySDRDevice_acquireWriteBuffer(SoapySDRDevice *device,
    SoapySDRStream *stream,
    size_t *handle,
    void **buffs,
    const long timeoutUs)
{
    return device->acquireWriteBuffer(reinterpret_cast<SoapySDR::Stream *>(stream), *handle, buffs, timeoutUs);
}

void SoapySDRDevice_releaseWriteBuffer(SoapySDRDevice *device,
    SoapySDRStream *stream,
    const size_t handle,
    const size_t numElems,
    int *flags,
    const long long timeNs)
{
    return device->releaseWriteBuffer(reinterpret_cast<SoapySDR::Stream *>(stream), handle, numElems, *flags, timeNs);
}

/*******************************************************************
 * Antenna API
 ******************************************************************/
char **SoapySDRDevice_listAntennas(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length)
{
    return toStrArray(device->listAntennas(direction, channel), length);
}

int SoapySDRDevice_setAntenna(SoapySDRDevice *device, const int direction, const size_t channel, const char *name)
{
    __SOAPY_SDR_C_TRY
    device->setAntenna(direction, channel, name);
    __SOAPY_SDR_C_CATCH
}

char *SoapySDRDevice_getAntenna(const SoapySDRDevice *device, const int direction, const size_t channel)
{
    return strdup(device->getAntenna(direction, channel).c_str());
}

/*******************************************************************
 * Frontend corrections API
 ******************************************************************/
bool SoapySDRDevice_hasDCOffsetMode(const SoapySDRDevice *device, const int direction, const size_t channel)
{
    return device->hasDCOffsetMode(direction, channel);
}

int SoapySDRDevice_setDCOffsetMode(SoapySDRDevice *device, const int direction, const size_t channel, const bool automatic)
{
    __SOAPY_SDR_C_TRY
    device->setDCOffsetMode(direction, channel, automatic);
    __SOAPY_SDR_C_CATCH
}

bool SoapySDRDevice_getDCOffsetMode(const SoapySDRDevice *device, const int direction, const size_t channel)
{
    return device->getDCOffsetMode(direction, channel);
}

bool SoapySDRDevice_hasDCOffset(const SoapySDRDevice *device, const int direction, const size_t channel)
{
    return device->hasDCOffset(direction, channel);
}

int SoapySDRDevice_setDCOffset(SoapySDRDevice *device, const int direction, const size_t channel, const double offsetI, const double offsetQ)
{
    __SOAPY_SDR_C_TRY
    device->setDCOffset(direction, channel, std::complex<double>(offsetI, offsetQ));
    __SOAPY_SDR_C_CATCH
}

void SoapySDRDevice_getDCOffset(const SoapySDRDevice *device, const int direction, const size_t channel, double *offsetI, double *offsetQ)
{
    std::complex<double> ret = device->getDCOffset(direction, channel);
    *offsetI = ret.real();
    *offsetQ = ret.imag();
}

bool SoapySDRDevice_hasIQBalance(const SoapySDRDevice *device, const int direction, const size_t channel)
{
    return device->hasIQBalance(direction, channel);
}

int SoapySDRDevice_setIQBalance(SoapySDRDevice *device, const int direction, const size_t channel, const double balanceI, const double balanceQ)
{
    __SOAPY_SDR_C_TRY
    device->setDCOffset(direction, channel, std::complex<double>(balanceI, balanceQ));
    __SOAPY_SDR_C_CATCH
}

void SoapySDRDevice_getIQBalance(const SoapySDRDevice *device, const int direction, const size_t channel, double *balanceI, double *balanceQ)
{
    std::complex<double> ret = device->getIQBalance(direction, channel);
    *balanceI = ret.real();
    *balanceQ = ret.imag();
}

/*******************************************************************
 * Gain API
 ******************************************************************/
char **SoapySDRDevice_listGains(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length)
{
    return toStrArray(device->listGains(direction, channel), length);
}

bool SoapySDRDevice_hasGainMode(const SoapySDRDevice *device, const int direction, const size_t channel)
{
    return device->hasGainMode(direction, channel);
}

int SoapySDRDevice_setGainMode(SoapySDRDevice *device, const int direction, const size_t channel, const bool automatic)
{
    __SOAPY_SDR_C_TRY
    device->setGainMode(direction, channel, automatic);
    __SOAPY_SDR_C_CATCH
}

bool SoapySDRDevice_getGainMode(const SoapySDRDevice *device, const int direction, const size_t channel)
{
    return device->getGainMode(direction, channel);
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
    return device->getGain(direction, channel);
}

double SoapySDRDevice_getGainElement(const SoapySDRDevice *device, const int direction, const size_t channel, const char *name)
{
    return device->getGain(direction, channel, name);
}

SoapySDRRange SoapySDRDevice_getGainRange(const SoapySDRDevice *device, const int direction, const size_t channel)
{
    return toRange(device->getGainRange(direction, channel));
}

SoapySDRRange SoapySDRDevice_getGainElementRange(const SoapySDRDevice *device, const int direction, const size_t channel, const char *name)
{
    return toRange(device->getGainRange(direction, channel, name));
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
    return device->getFrequency(direction, channel);
}

double SoapySDRDevice_getFrequencyComponent(const SoapySDRDevice *device, const int direction, const size_t channel, const char *name)
{
    return device->getFrequency(direction, channel, name);
}

char **SoapySDRDevice_listFrequencies(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length)
{
    return toStrArray(device->listFrequencies(direction, channel), length);
}

SoapySDRRange *SoapySDRDevice_getFrequencyRange(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length)
{
    return toRangeList(device->getFrequencyRange(direction, channel), length);
}

SoapySDRRange *SoapySDRDevice_getFrequencyRangeComponent(const SoapySDRDevice *device, const int direction, const size_t channel, const char *name, size_t *length)
{
    return toRangeList(device->getFrequencyRange(direction, channel, name), length);
}

SoapySDRArgInfo *SoapySDRDevice_getFrequencyArgsInfo(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length)
{
    return toArgInfoList(device->getFrequencyArgsInfo(direction, channel), length);
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
    return device->getSampleRate(direction, channel);
}

double *SoapySDRDevice_listSampleRates(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length)
{
    return toNumericList(device->listSampleRates(direction, channel), length);
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
    return device->getBandwidth(direction, channel);
}

double *SoapySDRDevice_listBandwidths(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length)
{
    return toNumericList(device->listBandwidths(direction, channel), length);
}

SoapySDRRange *SoapySDRDevice_getBandwidthRange(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length)
{
    return toRangeList(device->getBandwidthRange(direction, channel), length);
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
    return device->getMasterClockRate();
}

SoapySDRRange *SoapySDRDevice_getMasterClockRates(const SoapySDRDevice *device, size_t *length)
{
    return toRangeList(device->getMasterClockRates(), length);
}

char **SoapySDRDevice_listClockSources(const SoapySDRDevice *device, size_t *length)
{
    return toStrArray(device->listClockSources(), length);
}

int SoapySDRDevice_setClockSource(SoapySDRDevice *device, const char *source)
{
    __SOAPY_SDR_C_TRY
    device->setClockSource(source);
    __SOAPY_SDR_C_CATCH
}

char *SoapySDRDevice_getClockSource(const SoapySDRDevice *device)
{
    return strdup(device->getClockSource().c_str());
}

/*******************************************************************
 * Time API
 ******************************************************************/

char **SoapySDRDevice_listTimeSources(const SoapySDRDevice *device, size_t *length)
{
    return toStrArray(device->listTimeSources(), length);
}

int SoapySDRDevice_setTimeSource(SoapySDRDevice *device, const char *source)
{
    __SOAPY_SDR_C_TRY
    device->setTimeSource(source);
    __SOAPY_SDR_C_CATCH
}

char *SoapySDRDevice_getTimeSource(const SoapySDRDevice *device)
{
    return strdup(device->getTimeSource().c_str());
}

bool SoapySDRDevice_hasHardwareTime(const SoapySDRDevice *device, const char *what)
{
    return device->hasHardwareTime(what);
}

long long SoapySDRDevice_getHardwareTime(const SoapySDRDevice *device, const char *what)
{
    return device->getHardwareTime(what);
}

void SoapySDRDevice_setHardwareTime(SoapySDRDevice *device, const long long timeNs, const char *what)
{
    device->setHardwareTime(timeNs, what);
}

void SoapySDRDevice_setCommandTime(SoapySDRDevice *device, const long long timeNs, const char *what)
{
    device->setCommandTime(timeNs, what);
}

/*******************************************************************
 * Sensor API
 ******************************************************************/
char **SoapySDRDevice_listSensors(const SoapySDRDevice *device, size_t *length)
{
    return toStrArray(device->listSensors(), length);
}

SoapySDRArgInfo SoapySDRDevice_getSensorInfo(const SoapySDRDevice *device, const char *name)
{
    return toArgInfo(device->getSensorInfo(name));
}

char *SoapySDRDevice_readSensor(const SoapySDRDevice *device, const char *name)
{
    return strdup(device->readSensor(name).c_str());
}

char **SoapySDRDevice_listChannelSensors(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length)
{
    return toStrArray(device->listSensors(direction, channel), length);
}

SoapySDRArgInfo SoapySDRDevice_getChannelSensorInfo(const SoapySDRDevice *device, const int direction, const size_t channel, const char *name)
{
    return toArgInfo(device->getSensorInfo(direction, channel, name));
}

char *SoapySDRDevice_readChannelSensor(const SoapySDRDevice *device, const int direction, const size_t channel, const char *name)
{
    return strdup(device->readSensor(direction, channel, name).c_str());
}

/*******************************************************************
 * Register API
 ******************************************************************/
char **SoapySDRDevice_listRegisterInterfaces(const SoapySDRDevice *device, size_t *length)
{
    return toStrArray(device->listRegisterInterfaces(), length);
}

void SoapySDRDevice_writeNamedRegister(SoapySDRDevice *device, const char *name, const unsigned addr, const unsigned value)
{
    return device->writeRegister(name, addr, value);
}

unsigned SoapySDRDevice_readNamedRegister(const SoapySDRDevice *device, const char *name, const unsigned addr)
{
    return device->readRegister(name, addr);
}

void SoapySDRDevice_writeRegister(SoapySDRDevice *device, const unsigned addr, const unsigned value)
{
    return device->writeRegister(addr, value);
}

unsigned SoapySDRDevice_readRegister(const SoapySDRDevice *device, const unsigned addr)
{
    return device->readRegister(addr);
}

/*******************************************************************
 * Settings API
 ******************************************************************/
SoapySDRArgInfo *SoapySDRDevice_getSettingInfo(const SoapySDRDevice *device, size_t *length)
{
    return toArgInfoList(device->getSettingInfo(), length);
}

void SoapySDRDevice_writeSetting(SoapySDRDevice *device, const char *key, const char *value)
{
    return device->writeSetting(key, value);
}

char *SoapySDRDevice_readSetting(const SoapySDRDevice *device, const char *key)
{
    return strdup(device->readSetting(key).c_str());
}

SoapySDRArgInfo *SoapySDRDevice_getChannelSettingInfo(const SoapySDRDevice *device, const int direction, const size_t channel, size_t *length)
{
    return toArgInfoList(device->getSettingInfo(direction, channel), length);
}

void SoapySDRDevice_writeChannelSetting(SoapySDRDevice *device, const int direction, const size_t channel, const char *key, const char *value)
{
    return device->writeSetting(direction, channel, key, value);
}

char *SoapySDRDevice_readChannelSetting(const SoapySDRDevice *device, const int direction, const size_t channel, const char *key)
{
    return strdup(device->readSetting(direction, channel, key).c_str());
}

/*******************************************************************
 * GPIO API
 ******************************************************************/
char **SoapySDRDevice_listGPIOBanks(const SoapySDRDevice *device, size_t *length)
{
    return toStrArray(device->listGPIOBanks(), length);
}

void SoapySDRDevice_writeGPIO(SoapySDRDevice *device, const char *bank, const unsigned value)
{
    return device->writeGPIO(bank, value);
}

void SoapySDRDevice_writeGPIOMasked(SoapySDRDevice *device, const char *bank, const unsigned value, const unsigned mask)
{
    return device->writeGPIO(bank, value, mask);
}

unsigned SoapySDRDevice_readGPIO(const SoapySDRDevice *device, const char *bank)
{
    return device->readGPIO(bank);
}

void SoapySDRDevice_writeGPIODir(SoapySDRDevice *device, const char *bank, const unsigned dir)
{
    return device->writeGPIODir(bank, dir);
}

void SoapySDRDevice_writeGPIODirMasked(SoapySDRDevice *device, const char *bank, const unsigned dir, const unsigned mask)
{
    return device->writeGPIODir(bank, dir, mask);
}

unsigned SoapySDRDevice_readGPIODir(const SoapySDRDevice *device, const char *bank)
{
    return device->readGPIODir(bank);
}

/*******************************************************************
 * I2C API
 ******************************************************************/
void SoapySDRDevice_writeI2C(SoapySDRDevice *device, const int addr, const char *data, const size_t numBytes)
{
    return device->writeI2C(addr, std::string(data, numBytes));
}

char *SoapySDRDevice_readI2C(SoapySDRDevice *device, const int addr, const size_t numBytes)
{
    const std::string bytes = device->readI2C(addr, numBytes).c_str();
    char *buff = (char *)std::malloc(bytes.size());
    std::copy(bytes.begin(), bytes.end(), buff);
    return buff;
}

/*******************************************************************
 * SPI API
 ******************************************************************/
unsigned SoapySDRDevice_transactSPI(SoapySDRDevice *device, const int addr, const unsigned data, const size_t numBits)
{
    return device->transactSPI(addr, data, numBits);
}

/*******************************************************************
 * UART API
 ******************************************************************/
char **SoapySDRDevice_listUARTs(const SoapySDRDevice *device, size_t *length)
{
    return toStrArray(device->listUARTs(), length);
}

void SoapySDRDevice_writeUART(SoapySDRDevice *device, const char *which, const char *data)
{
    return device->writeUART(which, data);
}

char *SoapySDRDevice_readUART(const SoapySDRDevice *device, const char *which, const long timeoutUs)
{
    return strdup(device->readUART(which, timeoutUs).c_str());
}

/*******************************************************************
 * Memory Block API
 ******************************************************************/

void SoapySDRDevice_writeNamedRegisters(SoapySDRDevice *device, const char *name, const unsigned addr, const unsigned *value, size_t length)
{
    return device->writeRegisters(name, addr, toNumericVector(value, length));
}

unsigned *SoapySDRDevice_readNamedRegisters(const SoapySDRDevice *device, const char *name, const unsigned addr, size_t length)
{
    return toNumericList(device->readRegisters(name, addr, length));
}


} //extern "C"
