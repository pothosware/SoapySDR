// Copyright (c) 2021 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "JavaWrappers.hpp"

#include <SoapySDR/Device.hpp>

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <memory>
#include <utility>

// TODO: consistency in const parameters

// We're separately using a separate thin wrapper because: 
// * To abstract away the make() and unmake() calls, which SWIG won't
//   deal with well.
namespace SoapySDR { namespace Java {

    struct DeviceDeleter
    {
        void operator()(SoapySDR::Device* pDevice)
        {
            // unmake can throw, which is bad in destructors.
            try { SoapySDR::Device::unmake(pDevice); }
            catch(const std::exception& ex) { fputs(ex.what(), stderr); }
            catch(...) { fputs("Unknown error.", stderr); }
        }
    };

    class Device
    {
        public:
            using DeviceVector = std::vector<SoapySDR::Java::Device>;

            Device(const Kwargs& kwargs): _deviceSPtr(SoapySDR::Device::make(kwargs), DeviceDeleter())
            {}

            Device(const std::string& args): _deviceSPtr(SoapySDR::Device::make(args), DeviceDeleter())
            {}

            static inline SoapySDR::KwargsList enumerate()
            {
                return SoapySDR::Device::enumerate();
            }

            static inline SoapySDR::KwargsList enumerate(const std::string& args)
            {
                return SoapySDR::Device::enumerate(args);
            }

            static inline SoapySDR::KwargsList enumerate(const SoapySDR::Kwargs& args)
            {
                return SoapySDR::Device::enumerate(args);
            }

            //
            // Identification API
            //

            inline std::string getDriverKey() const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getDriverKey();
            }

            inline std::string getHardwareKey() const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getHardwareKey();
            }

            inline SoapySDR::Kwargs getHardwareInfo() const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getHardwareInfo();
            }

            //
            // Channels API
            //

            inline void setFrontendMapping(
                const SoapySDR::Java::Direction direction,
                const std::string& mapping)
            {
                assert(_deviceSPtr);

                return _deviceSPtr->setFrontendMapping(int(direction), mapping);
            }

            inline std::string getFrontendMapping(SoapySDR::Java::Direction direction) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getFrontendMapping(int(direction));
            }

            inline size_t getNumChannels(SoapySDR::Java::Direction direction) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getNumChannels(int(direction));
            }

            inline SoapySDR::Kwargs getChannelInfo(
                const SoapySDR::Java::Direction direction,
                const size_t channel) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getChannelInfo(int(direction), channel);
            }

            inline bool getFullDuplex(
                const SoapySDR::Java::Direction direction,
                const size_t channel) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getFullDuplex(int(direction), channel);
            }

            //
            // Stream API
            //

            inline std::vector<std::string> getStreamFormats(
                const SoapySDR::Java::Direction direction,
                const size_t channel) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getStreamFormats(int(direction), channel);
            }

            inline std::string getNativeStreamFormat(
                const SoapySDR::Java::Direction direction,
                const size_t channel,
                double &fullScaleOut) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getNativeStreamFormat(int(direction), channel, fullScaleOut);
            }

            inline SoapySDR::ArgInfoList getStreamArgsInfo(
                const SoapySDR::Java::Direction direction,
                const size_t channel) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getStreamArgsInfo(int(direction), channel);
            }

            // TODO: this needs custom JNI stuff, figure out later
/*
            SoapySDR::Java::StreamHandle setupStream(
                const SoapySDR::Java::Direction direction,
                const std::string& format,
                const SizeVector& channels,
                const SoapySDR::Kwargs& kwargs)
            {
                assert(_deviceSPtr);

                SoapySDR::Java::StreamHandle streamHandle;
                streamHandle.stream = _deviceSPtr->setupStream(int(direction), format, channels, kwargs);
                streamHandle.channels = channels;

                return streamHandle;
            }

            inline void CloseStream(const SoapySDR::Java::StreamHandle& streamHandle)
            {
                assert(_deviceSPtr);

                _deviceSPtr->closeStream(streamHandle.stream);
            }

            inline size_t getStreamMTU(const SoapySDR::Java::StreamHandle& streamHandle)
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getStreamMTU(streamHandle.stream);
            }

            inline SoapySDR::Java::ErrorCode ActivateStream(
                const SoapySDR::Java::StreamHandle& streamHandle,
                const SoapySDR::Java::StreamFlags flags,
                const long long timeNs,
                const size_t numElems)
            {
                assert(_deviceSPtr);

                return SoapySDR::Java::ErrorCode(_deviceSPtr->activateStream(
                    streamHandle.stream,
                    int(flags),
                    timeNs,
                    numElems));
            }

            inline SoapySDR::Java::ErrorCode DeactivateStream(
                const SoapySDR::Java::StreamHandle& streamHandle,
                const SoapySDR::Java::StreamFlags flags,
                const long long timeNs)
            {
                assert(_deviceSPtr);

                return SoapySDR::Java::ErrorCode(_deviceSPtr->deactivateStream(
                    streamHandle.stream,
                    int(flags),
                    timeNs));
            }

            StreamResultPair readStream(
                const SoapySDR::Java::StreamHandle& streamHandle,
                const SizeVector& buffs,
                const size_t numElems,
                const SoapySDR::Java::StreamFlags flags,
                const long long timeNs,
                const long timeoutUs)
            {
                assert(_deviceSPtr);

                StreamResultPair resultPair;
                auto& errorCode = resultPair.first;
                auto& result = resultPair.second;

                std::vector<void*> buffPtrs(buffs.size());
                std::transform(
                    buffs.begin(),
                    buffs.end(),
                    std::back_inserter(buffPtrs),
                    [](const typename SizeVector::value_type buffNum)
                    { return reinterpret_cast<void*>(buffNum); });

                auto intFlags = int(flags);
                auto cppRet = _deviceSPtr->readStream(streamHandle.stream, buffPtrs.data(), numElems, intFlags, result.TimeNs, result.TimeoutUs);
                result.Flags = SoapySDR::Java::StreamFlags(intFlags);

                if(cppRet >= 0) result.NumSamples = static_cast<size_t>(cppRet);
                else            errorCode = static_cast<SoapySDR::Java::ErrorCode>(cppRet);

                return resultPair;
            }

            StreamResultPair writeStream(
                const SoapySDR::Java::StreamHandle& streamHandle,
                const SizeVector& buffs,
                const size_t numElems,
                const long long timeNs,
                const long timeoutUs)
            {
                assert(_deviceSPtr);

                StreamResultPair resultPair;
                auto& errorCode = resultPair.first;
                auto& result = resultPair.second;

                std::vector<const void*> buffPtrs;
                std::transform(
                    buffs.begin(),
                    buffs.end(),
                    std::back_inserter(buffPtrs),
                    [](const typename SizeVector::value_type buffNum)
                    { return reinterpret_cast<const void*>(buffNum); });

                int intFlags = 0;
                auto cppRet = _deviceSPtr->writeStream(streamHandle.stream, buffPtrs.data(), numElems, intFlags, timeNs, timeoutUs);
                result.Flags = SoapySDR::Java::StreamFlags(intFlags);

                if(cppRet >= 0) result.NumSamples = static_cast<size_t>(cppRet);
                else            errorCode = static_cast<SoapySDR::Java::ErrorCode>(cppRet);

                return resultPair;
            }

            StreamResultPair readStreamStatus(
                const SoapySDR::Java::StreamHandle& streamHandle,
                const long timeoutUs)
            {
                assert(_deviceSPtr);

                StreamResultPair resultPair;
                auto& errorCode = resultPair.first;
                auto& result = resultPair.second;

                int intFlags = 0;
                errorCode = SoapySDR::Java::ErrorCode(_deviceSPtr->readStreamStatus(
                    streamHandle.stream,
                    result.ChanMask,
                    intFlags,
                    result.TimeNs,
                    result.TimeoutUs));
                result.Flags = SoapySDR::Java::StreamFlags(intFlags);

                return resultPair;
            }
*/

            //
            // Antenna API
            //

            inline std::vector<std::string> listAntennas(
                const SoapySDR::Java::Direction direction,
                const size_t channel) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->listAntennas(int(direction), channel);
            }

            inline void setAntenna(
                const SoapySDR::Java::Direction direction,
                const size_t channel,
                const std::string& name)
            {
                assert(_deviceSPtr);

                _deviceSPtr->setAntenna(int(direction), channel, name);
            }

            inline std::string getAntenna(
                const SoapySDR::Java::Direction direction,
                const size_t channel) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getAntenna(int(direction), channel);
            }

            //
            // Frontend corrections API
            //

            inline bool hasDCOffsetMode(
                const SoapySDR::Java::Direction direction,
                const size_t channel) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->hasDCOffsetMode(int(direction), channel);
            }

            inline void setDCOffsetMode(
                const SoapySDR::Java::Direction direction,
                const size_t channel,
                const bool automatic)
            {
                assert(_deviceSPtr);

                return _deviceSPtr->setDCOffsetMode(int(direction), channel, automatic);
            }

            inline bool getDCOffsetMode(
                const SoapySDR::Java::Direction direction,
                const size_t channel) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getDCOffsetMode(int(direction), channel);
            }

            inline bool hasDCOffset(
                const SoapySDR::Java::Direction direction,
                const size_t channel) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->hasDCOffset(int(direction), channel);
            }

            /*
            inline void setDCOffset(
                const SoapySDR::Java::Direction direction,
                const size_t channel,
                const std::complex<double>& offset)
            {
                assert(_deviceSPtr);

                _deviceSPtr->setDCOffset(int(direction), channel, offset);
            }

            inline std::complex<double> getDCOffset(
                const SoapySDR::Java::Direction direction,
                const size_t channel) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getDCOffset(int(direction), channel);
            }
            */

            inline bool hasIQBalance(
                const SoapySDR::Java::Direction direction,
                const size_t channel) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->hasIQBalance(int(direction), channel);
            }

            /*
            inline void setIQBalance(
                const SoapySDR::Java::Direction direction,
                const size_t channel,
                const std::complex<double>& balance)
            {
                assert(_deviceSPtr);

                _deviceSPtr->setIQBalance(int(direction), channel, balance);
            }

            inline std::complex<double> getIQBalance(
                const SoapySDR::Java::Direction direction,
                const size_t channel) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getIQBalance(int(direction), channel);
            }
            */

            inline bool hasIQBalanceMode(
                const SoapySDR::Java::Direction direction,
                const size_t channel) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->hasIQBalanceMode(int(direction), channel);
            }

            inline void setIQBalanceMode(
                const SoapySDR::Java::Direction direction,
                const size_t channel,
                const bool automatic)
            {
                assert(_deviceSPtr);

                _deviceSPtr->setIQBalanceMode(int(direction), channel, automatic);
            }

            inline bool getIQBalanceMode(
                const SoapySDR::Java::Direction direction,
                const size_t channel) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getIQBalanceMode(int(direction), channel);
            }

            inline bool hasFrequencyCorrection(
                const SoapySDR::Java::Direction direction,
                const size_t channel) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->hasFrequencyCorrection(int(direction), channel);
            }

            inline void setFrequencyCorrection(
                const SoapySDR::Java::Direction direction,
                const size_t channel,
                const double value)
            {
                assert(_deviceSPtr);

                _deviceSPtr->setFrequencyCorrection(int(direction), channel, value);
            }

            inline double getFrequencyCorrection(
                const SoapySDR::Java::Direction direction,
                const size_t channel) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getFrequencyCorrection(int(direction), channel);
            }

            //
            // Gain API
            //

            inline std::vector<std::string> listGains(
                const SoapySDR::Java::Direction direction,
                const size_t channel) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->listGains(int(direction), channel);
            }

            inline bool hasGainMode(
                const SoapySDR::Java::Direction direction,
                const size_t channel) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->hasGainMode(int(direction), channel);
            }

            inline void setGainMode(
                const SoapySDR::Java::Direction direction,
                const size_t channel,
                const bool automatic)
            {
                assert(_deviceSPtr);

                _deviceSPtr->setGainMode(int(direction), channel, automatic);
            }

            inline bool getGainMode(
                const SoapySDR::Java::Direction direction,
                const size_t channel) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getGainMode(int(direction), channel);
            }

            inline void setGain(
                const SoapySDR::Java::Direction direction,
                const size_t channel,
                const double value)
            {
                assert(_deviceSPtr);

                _deviceSPtr->setGain(int(direction), channel, value);
            }

            inline void setGain(
                const SoapySDR::Java::Direction direction,
                const size_t channel,
                const std::string& name,
                const double value)
            {
                assert(_deviceSPtr);

                _deviceSPtr->setGain(int(direction), channel, name, value);
            }

            inline double getGain(
                const SoapySDR::Java::Direction direction,
                const size_t channel) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getGain(int(direction), channel);
            }

            inline double getGain(
                const SoapySDR::Java::Direction direction,
                const size_t channel,
                const std::string& name) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getGain(int(direction), channel, name);
            }

            inline SoapySDR::Range getGainRange(
                const SoapySDR::Java::Direction direction,
                const size_t channel) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getGainRange(int(direction), channel);
            }

            inline SoapySDR::Range getGainRange(
                const SoapySDR::Java::Direction direction,
                const size_t channel,
                const std::string& name) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getGainRange(int(direction), channel, name);
            }

            //
            // Frequency API
            //

            inline void setFrequency(
                const SoapySDR::Java::Direction direction,
                const size_t channel,
                const double frequency,
                const SoapySDR::Kwargs& args)
            {
                assert(_deviceSPtr);

                _deviceSPtr->setFrequency(int(direction), channel, frequency, args);
            }

            inline void setFrequency(
                const SoapySDR::Java::Direction direction,
                const size_t channel,
                const std::string& name,
                const double frequency,
                const SoapySDR::Kwargs& args)
            {
                assert(_deviceSPtr);

                _deviceSPtr->setFrequency(int(direction), channel, name, frequency, args);
            }

            inline double getFrequency(
                const SoapySDR::Java::Direction direction,
                const size_t channel) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getFrequency(int(direction), channel);
            }

            inline double getFrequency(
                const SoapySDR::Java::Direction direction,
                const size_t channel,
                const std::string& name) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getFrequency(int(direction), channel, name);
            }

            inline std::vector<std::string> listFrequencies(
                const SoapySDR::Java::Direction direction,
                const size_t channel) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->listFrequencies(int(direction), channel);
            }

            inline SoapySDR::RangeList getFrequencyRange(
                const SoapySDR::Java::Direction direction,
                const size_t channel) const 
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getFrequencyRange(int(direction), channel);
            }

            inline SoapySDR::RangeList getFrequencyRange(
                const SoapySDR::Java::Direction direction,
                const size_t channel,
                const std::string& name) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getFrequencyRange(int(direction), channel, name);
            }

            inline SoapySDR::ArgInfoList getFrequencyArgsInfo(
                const SoapySDR::Java::Direction direction,
                const size_t channel) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getFrequencyArgsInfo(int(direction), channel);
            }

            //
            // Sample Rate API
            //

            inline void setSampleRate(
                const SoapySDR::Java::Direction direction,
                const size_t channel,
                const double rate)
            {
                assert(_deviceSPtr);

                _deviceSPtr->setSampleRate(int(direction), channel, rate);
            }

            inline double getSampleRate(
                const SoapySDR::Java::Direction direction,
                const size_t channel) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getSampleRate(int(direction), channel);
            }

            inline SoapySDR::RangeList getSampleRateRange(
                const SoapySDR::Java::Direction direction,
                const size_t channel) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getSampleRateRange(int(direction), channel);
            }

            //
            // Bandwidth API
            //

            inline void setBandwidth(
                const SoapySDR::Java::Direction direction,
                const size_t channel,
                const double bandwidth)
            {
                assert(_deviceSPtr);

                _deviceSPtr->setBandwidth(int(direction), channel, bandwidth);
            }

            inline double getBandwidth(
                const SoapySDR::Java::Direction direction,
                const size_t channel) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getBandwidth(int(direction), channel);
            }

            inline SoapySDR::RangeList getBandwidthRange(
                const SoapySDR::Java::Direction direction,
                const size_t channel) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getBandwidthRange(int(direction), channel);
            }

            //
            // Clocking API
            //

            inline void setMasterClockRate(const double rate)
            {
                assert(_deviceSPtr);

                _deviceSPtr->setMasterClockRate(rate);
            }

            inline double getMasterClockRate() const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getMasterClockRate();
            }

            inline SoapySDR::RangeList getMasterClockRates() const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getMasterClockRates();
            }

            inline void setReferenceClockRate(const double rate)
            {
                assert(_deviceSPtr);

                _deviceSPtr->setReferenceClockRate(rate);
            }

            inline double getReferenceClockRate() const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getReferenceClockRate();
            }

            inline SoapySDR::RangeList getReferenceClockRates() const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getReferenceClockRates();
            }

            inline std::vector<std::string> listClockSources() const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->listClockSources();
            }

            inline void setClockSource(const std::string& source)
            {
                assert(_deviceSPtr);

                _deviceSPtr->setClockSource(source);
            }

            inline std::string getClockSource() const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getClockSource();
            }

            //
            // Time API
            //

            inline std::vector<std::string> listTimeSources() const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->listTimeSources();
            }

            inline void setTimeSource(const std::string& source)
            {
                assert(_deviceSPtr);

                _deviceSPtr->setTimeSource(source);
            }

            inline std::string getTimeSource() const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getTimeSource();
            }

            inline bool hasHardwareTime(const std::string& what) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->hasHardwareTime(what);
            }

            inline long long getHardwareTime(const std::string& what) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getHardwareTime(what);
            }

            inline void setHardwareTime(const long long timeNs, const std::string& what)
            {
                assert(_deviceSPtr);

                _deviceSPtr->setHardwareTime(timeNs, what);
            }

            //
            // Sensor API
            //

            inline std::vector<std::string> listSensors() const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->listSensors();
            }

            inline SoapySDR::ArgInfo getSensorInfo(const std::string& key) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getSensorInfo(key);
            }

            inline std::string readSensor(const std::string& key) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->readSensor(key);
            }

            inline std::vector<std::string> listSensors(
                const SoapySDR::Java::Direction direction,
                const size_t channel) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->listSensors(int(direction), channel);
            }

            inline SoapySDR::ArgInfo getSensorInfo(
                const SoapySDR::Java::Direction direction,
                const size_t channel,
                const std::string& key) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getSensorInfo(int(direction), channel, key);
            }

            inline std::string readSensor(
                const SoapySDR::Java::Direction direction,
                const size_t channel,
                const std::string& key) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->readSensor(int(direction), channel, key);
            }

            //
            // Register API
            //

            inline std::vector<std::string> listRegisterInterfaces() const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->listRegisterInterfaces();
            }

            inline void writeRegister(
                const std::string& name,
                const unsigned addr,
                const unsigned value)
            {
                assert(_deviceSPtr);

                _deviceSPtr->writeRegister(name, addr, value);
            }

            inline unsigned readRegister(
                const std::string& name,
                const unsigned addr) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->readRegister(name, addr);
            }

            // To avoid the uint/ulong issue, these functions will internally
            // use SizeVector. The public-facing function will use uint[] as
            // expected, and these are less commonly used functions, so this
            // is fine.

            inline void writeRegisters(
                const std::string& name,
                const unsigned addr,
                const SizeVector& value)
            {
                assert(_deviceSPtr);

                std::vector<unsigned> valueUnsigned;
                std::transform(
                    value.begin(),
                    value.end(),
                    std::back_inserter(valueUnsigned),
                    [](const UIntPtrT elem) {return static_cast<unsigned>(elem); });

                _deviceSPtr->writeRegisters(name, addr, valueUnsigned);
            }

            inline SizeVector readRegisters(
                const std::string& name,
                const unsigned addr,
                const size_t length)
            {
                assert(_deviceSPtr);

                const auto valueUnsigned = _deviceSPtr->readRegisters(name, addr, length);
                SizeVector value;
                std::transform(
                    valueUnsigned.begin(),
                    valueUnsigned.end(),
                    std::back_inserter(value),
                    [](const unsigned elem) {return static_cast<UIntPtrT>(elem); });

                return value;
            }

            //
            // Settings API
            //

            inline SoapySDR::ArgInfoList getSettingInfo() const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getSettingInfo();
            }

            inline void writeSetting(
                const std::string& key,
                const std::string& value)
            {
                assert(_deviceSPtr);

                _deviceSPtr->writeSetting(key, value);
            }

            inline std::string readSetting(const std::string& key) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->readSetting(key);
            }

            inline SoapySDR::ArgInfoList getSettingInfo(
                const SoapySDR::Java::Direction direction,
                const size_t channel) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->getSettingInfo(int(direction), channel);
            }

            inline void writeSetting(
                const SoapySDR::Java::Direction direction,
                const size_t channel,
                const std::string& key,
                const std::string& value)
            {
                assert(_deviceSPtr);

                _deviceSPtr->writeSetting(int(direction), channel, key, value);
            }

            inline std::string readSetting(
                const SoapySDR::Java::Direction direction,
                const size_t channel,
                const std::string& key) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->readSetting(int(direction), channel, key);
            }

            //
            // GPIO API
            //

            inline std::vector<std::string> listGPIOBanks() const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->listGPIOBanks();
            }

            inline void writeGPIO(
                const std::string& bank,
                const unsigned value)
            {
                assert(_deviceSPtr);

                _deviceSPtr->writeGPIO(bank, value);
            }

            inline void writeGPIO(
                const std::string& bank,
                const unsigned value,
                const unsigned mask)
            {
                assert(_deviceSPtr);

                _deviceSPtr->writeGPIO(bank, value, mask);
            }

            inline unsigned readGPIO(const std::string& bank) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->readGPIO(bank);
            }

            inline void writeGPIODir(
                const std::string& bank,
                const unsigned dir)
            {
                assert(_deviceSPtr);

                _deviceSPtr->writeGPIODir(bank, dir);
            }

            inline void writeGPIODir(
                const std::string& bank,
                const unsigned dir,
                const unsigned mask)
            {
                assert(_deviceSPtr);

                _deviceSPtr->writeGPIODir(bank, dir, mask);
            }

            inline unsigned readGPIODir(const std::string& bank) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->readGPIODir(bank);
            }

            //
            // I2C API
            //

            inline void writeI2C(
                const int addr,
                const std::string& data)
            {
                assert(_deviceSPtr);

                _deviceSPtr->writeI2C(addr, data);
            }

            inline std::string readI2C(
                const int addr,
                const size_t numBytes)
            {
                assert(_deviceSPtr);

                return _deviceSPtr->readI2C(addr, numBytes);
            }

            //
            // SPI API
            //

            inline unsigned transactSPI(
                const int addr,
                const unsigned data,
                const size_t numBits)
            {
                assert(_deviceSPtr);

                return _deviceSPtr->transactSPI(addr, data, numBits);
            }

            //
            // UART API
            //

            inline std::vector<std::string> listUARTs() const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->listUARTs();
            }

            inline void writeUART(
                const std::string& which,
                const std::string& data)
            {
                assert(_deviceSPtr);

                _deviceSPtr->writeUART(which, data);
            }

            inline std::string readUART(
                const std::string& which,
                const long long timeoutUs) const
            {
                assert(_deviceSPtr);

                return _deviceSPtr->readUART(which, static_cast<long>(timeoutUs));
            }

            //
            // Used for Java internals
            //

            inline std::string __toString() const
            {
                assert(_deviceSPtr);

                return (_deviceSPtr->getDriverKey() + ":" + _deviceSPtr->getHardwareKey());
            }

            inline bool equals(const SoapySDR::Java::Device& other) const
            {
                assert(_deviceSPtr);

                return (__toString() == other.__toString());
            }

            inline UIntPtrT getPointer() const
            {
                assert(_deviceSPtr);

                return reinterpret_cast<UIntPtrT>(_deviceSPtr.get());
            }

        private:
            std::shared_ptr<SoapySDR::Device> _deviceSPtr;

            // C# class takes ownership, will unmake
            Device(SoapySDR::Device* pDevice): _deviceSPtr(pDevice, DeviceDeleter())
            {}
    };
}}
