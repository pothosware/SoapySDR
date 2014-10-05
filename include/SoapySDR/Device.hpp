///
/// \file SoapySDR/Device.hpp
///
/// Interface definition for Soapy SDR devices.
///
/// \copyright
/// Copyright (c) 2014-2014 Josh Blum
/// SPDX-License-Identifier: BSL-1.0
///

#pragma once
#include <SoapySDR/Config.hpp>
#include <SoapySDR/Types.hpp>
#include <vector>
#include <string>
#include <complex>
#include <cstddef> //size_t

namespace SoapySDR
{

//! Indicates that read or write had a timeout
static const int STREAM_FLAG_TIMEOUT = (1 << 0);

//! Indicates a end of burst for transmit or receive
static const int STREAM_FLAG_END_BURST = (1 << 1);

//! Indicates that the time stamp is valid
static const int STREAM_FLAG_HAS_TIME = (1 << 2);

//! Indicates an overflow error condition
static const int STREAM_FLAG_OVERFLOW = (1 << 3);

/*!
 * Abstraction for an SDR tranceiver device - configuration and streaming.
 */
class SOAPY_SDR_API Device
{
public:

    //! virtual destructor for inheritance
    virtual ~Device(void);

    /*!
     * Enumerate a list of available devices on the system.
     * \param args device construction key/value argument filters
     * \return a list of arguments strings, each unique to a device
     */
    static std::vector<Kwargs> enumerate(const Kwargs &args);

    /*!
     * Make a new Device object given device construction args.
     * The device pointer will be stored in a table so subsequent calls
     * with the same arguments will produce the same device.
     * For every call to make, there should be a matched call to unmake.
     * \param args device construction key/value argument markup
     * \return a pointer to a new Device object
     */
    static Device *make(const Kwargs &args);

    /*!
     * Unmake or release a device object handle.
     * \param device a pointer to a device object
     */
    static void unmake(Device *device);

    /*******************************************************************
     * Channels API
     ******************************************************************/

    /*!
     * Get a number of channels given the streaming direction
     */
    virtual size_t getNumChannels(const Direction dir) const;

    /*******************************************************************
     * Stream API
     ******************************************************************/

    /*!
     * Initialize a stream given a list of channels and stream arguments.
     * \param dir the channel direction RX or TX
     * \param channels a list of channels for empty for automatic
     * \param args stream args or empty for defaults.
     * \return an opaque pointer to a stream handle
     */
    virtual void *setupStream(const Direction dir, const std::vector<size_t> &channels = std::vector<size_t>(), const Kwargs &args = Kwargs());

    /*!
     * Close an open stream created by setupStream
     * \param handle the opaque pointer to a stream handle
     */
    virtual void closeStream(void *handle);

    /*!
     * Read elements from a stream for reception.
     * This is a multi-channel call, and buffs should be an array of void *,
     * where each pointer will be filled with data from a different channel.
     *
     * \param handle the opaque pointer to a stream handle
     * \param buffs an array of void* buffers num chans in size
     * \param numElems the number of elements in each buffer
     * \param flags optional flag indicators about the result
     * \param timeNs the buffer's timestamp in nanoseconds
     * \param timeoutUs the timeout in microseconds
     * \return the number of elements read per buffer or error
     */
    virtual int readStream(void *handle, const void **buffs, const size_t numElems, int &flags, long long &timeNs, const long timeoutUs = 100000);

    /*!
     * Write elements to a stream for transmission.
     * This is a multi-channel call, and buffs should be an array of void *,
     * where each pointer will be filled with data for a different channel.
     *
     * \param handle the opaque pointer to a stream handle
     * \param buffs an array of void* buffers num chans in size
     * \param numElems the number of elements in each buffer
     * \param flags optional input flags and output flags
     * \param timeNs the buffer's timestamp in nanoseconds
     * \param timeoutUs the timeout in microseconds
     * \return the number of elements written per buffer or error
     */
    virtual int writeStream(void *handle, const void **buffs, const size_t numElems, int &flags, const long long timeNs, const long timeoutUs = 100000);

    /*******************************************************************
     * Antenna API
     ******************************************************************/

    /*!
     * Get a list of available antennas to select on a given chain.
     * \param dir the channel direction RX or TX
     * \param channel an available channel on the device
     * \return a list of available antenna names
     */
    virtual std::vector<std::string> listAntennas(const Direction dir, const size_t channel) const;

    /*!
     * Set the selected antenna on a chain.
     * \param dir the channel direction RX or TX
     * \param channel an available channel on the device
     * \param name the name of an available antenna
     */
    virtual void setAntenna(const Direction dir, const size_t channel, const std::string &name);

    /*!
     * Get the selected antenna on a chain.
     * \param dir the channel direction RX or TX
     * \param channel an available channel on the device
     * \return the name of an available antenna
     */
    virtual std::string getAntenna(const Direction dir, const size_t channel) const;

    /*******************************************************************
     * Frontend corrections API
     ******************************************************************/

    /*!
     * Set the frontend DC offset correction.
     * \param dir the channel direction RX or TX
     * \param channel an available channel on the device
     * \param offset the relative correction (1.0 max)
     */
    virtual void setDCOffset(const Direction dir, const size_t channel, const std::complex<double> &offset);

    /*!
     * Get the frontend DC offset correction.
     * \param dir the channel direction RX or TX
     * \param channel an available channel on the device
     * \return the relative correction (1.0 max)
     */
    virtual std::complex<double> getDCOffset(const Direction dir, const size_t channel) const;

    /*!
     * Set the frontend IQ balance correction.
     * \param dir the channel direction RX or TX
     * \param channel an available channel on the device
     * \param balance the relative correction (1.0 max)
     */
    virtual void setIQBalance(const Direction dir, const size_t channel, const std::complex<double> &balance);

    /*!
     * Get the frontend IQ balance correction.
     * \param dir the channel direction RX or TX
     * \param channel an available channel on the device
     * \return the relative correction (1.0 max)
     */
    virtual std::complex<double> getIQBalance(const Direction dir, const size_t channel) const;

    /*******************************************************************
     * Gain API
     ******************************************************************/

    /*!
     * List available amplification elements.
     * \param dir the channel direction RX or TX
     * \param channel an available channel
     */
    virtual std::vector<std::string> listGains(const Direction dir, const size_t channel) const;

    /*!
     * Set the automatic gain mode on the chain.
     * \param dir the channel direction RX or TX
     * \param channel an available channel on the device
     * \param mode true for automatic gain setting
     */
    virtual void setGainMode(const Direction dir, const size_t channel, const bool mode);

     /*!
     * Get the automatic gain mode on the chain.
     * \param dir the channel direction RX or TX
     * \param channel an available channel on the device
     * \return true for automatic gain setting
     */
    virtual bool getGainMode(const Direction dir, const size_t channel) const;

    /*!
     * Set the value of a amplification element in a chain.
     * \param dir the channel direction RX or TX
     * \param channel an available channel on the device
     * \param name the name of an amplification element
     * \param value the new amplification value in dB
     */
    virtual void setGain(const Direction dir, const size_t channel, const std::string &name, const double value);

    /*!
     * Set the value of multiple amplification elements in a chain.
     * \param dir the channel direction RX or TX
     * \param channel an available channel on the device
     * \param values a dictionary of element names to values
     */
    virtual void setGains(const Direction dir, const size_t channel, const NumericDict &values);

    /*!
     * Get the value of an individual amplification element in a chain.
     * \param dir the channel direction RX or TX
     * \param channel an available channel on the device
     * \param name the name of an amplification element
     * \return the value of the gain in dB
     */
    virtual double getGainValue(const Direction dir, const size_t channel, const std::string &name) const;

    /*!
     * Get the valus of all amplification elements in a chain.
     * \param dir the channel direction RX or TX
     * \param channel an available channel on the device
     * \return a dictionary of element names to values
     */
    virtual NumericDict getGainValues(const Direction dir, const size_t channel) const;

    /*!
     * Get the range of possible gain values.
     * \param dir the channel direction RX or TX
     * \param channel an available channel on the device
     * \param name the name of an amplification element
     * \return a list of gain ranges in dB
     */
    virtual RangeList getGainRange(const Direction dir, const size_t channel, const std::string &name) const;

    /*******************************************************************
     * Frequency API
     ******************************************************************/

    /*!
     * Set the center frequency of the chain.
     * \param dir the channel direction RX or TX
     * \param channel an available channel on the device
     * \param frequency the center frequency in Hz
     * \param args optional tuner arguments
     */
    virtual void setFrequency(const Direction dir, const size_t channel, const double frequency, const Kwargs &args = Kwargs());

    /*!
     * Set the center frequency of the chain with control over individual components.
     * \param dir the channel direction RX or TX
     * \param channel an available channel on the device
     * \param values a dictionary of component names to frequency values in Hz
     * \param args optional tuner arguments
     */
    virtual void setFrequency(const Direction dir, const size_t channel, const NumericDict &values, const Kwargs &args = Kwargs());

    /*!
     * Get the center frequency of the chain.
     * \param dir the channel direction RX or TX
     * \param channel an available channel on the device
     * \return the center frequency in Hz
     */
    virtual double getFrequency(const Direction dir, const size_t channel) const;

    /*!
     * Get the frequencies of various elements in the chain.
     * \param dir the channel direction RX or TX
     * \param channel an available channel on the device
     * \return a dictionary of tunable elements to frequencies in Hz
     */
    virtual NumericDict getFrequencyComponents(const Direction dir, const size_t channel) const;

    /*!
     * Get the range of possible frequency values.
     * \param dir the channel direction RX or TX
     * \param channel an available channel on the device
     * \return a list of frequency ranges in Hz
     */
    virtual RangeList getFrequencyRange(const Direction dir, const size_t channel) const;

    /*******************************************************************
     * Sample Rate API
     ******************************************************************/

    /*!
     * Set the baseband sample rate of the chain.
     * \param dir the channel direction RX or TX
     * \param channel an available channel on the device
     * \param rate the sample rate in samples per second
     */
    virtual void setSampleRate(const Direction dir, const size_t channel, const double rate);

    /*!
     * Get the baseband sample rate of the chain.
     * \param dir the channel direction RX or TX
     * \param channel an available channel on the device
     * \return the sample rate in samples per second
     */
    virtual double getSampleRate(const Direction dir, const size_t channel) const;

    /*!
     * Get the range of possible baseband sample rates.
     * \param dir the channel direction RX or TX
     * \param channel an available channel on the device
     * \return a list of possible rates in samples per second
     */
    virtual RangeList getSampleRateRange(const Direction dir, const size_t channel) const;

    /*!
     * Set the baseband filter width of the chain.
     * \param dir the channel direction RX or TX
     * \param channel an available channel on the device
     * \param bw the baseband filter width in Hz
     */
    virtual void setBandwidth(const Direction dir, const size_t channel, const double bw);

    /*!
     * Get the baseband filter width of the chain.
     * \param dir the channel direction RX or TX
     * \param channel an available channel on the device
     * \return the baseband filter width in Hz
     */
    virtual double getBandwidth(const Direction dir, const size_t channel) const;

    /*!
     * Get the range of possible baseband filter widths.
     * \param dir the channel direction RX or TX
     * \param channel an available channel on the device
     * \return a list of possible bandwidths in Hz
     */
    virtual RangeList getBandwidthRange(const Direction dir, const size_t channel) const;

    /*******************************************************************
     * Clocking API
     ******************************************************************/

    /*!
     * Set the master clock rate of the device.
     * \param rate the clock rate in Hz
     */
    virtual void setMasterClockRate(const double rate);

    /*!
     * Get the master clock rate of the device.
     * \return the clock rate in Hz
     */
    virtual double setMasterClockRate(void) const;

    /*!
     * Get the list of available clock sources.
     * \return a list of clock source names
     */
    virtual std::vector<std::string> listClockSources(void) const;

    /*!
     * Set the clock source on the device
     * \param source the name of a clock source
     */
    virtual void setClockSource(const std::string &source);

    /*!
     * Get the clock source of the device
     * \return the name of a clock source
     */
    virtual std::string getClockSource(void) const;

    /*!
     * Get the list of available time sources.
     * \return a list of time source names
     */
    virtual std::vector<std::string> listTimeSources(void) const;

    /*!
     * Set the time source on the device
     * \param source the name of a time source
     */
    virtual void setTimeSource(const std::string &source);

    /*!
     * Get the time source of the device
     * \return the name of a time source
     */
    virtual std::string getTimeSource(void) const;
};

};
