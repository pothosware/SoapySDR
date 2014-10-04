///
/// \file SoapySDR/Interface.hpp
///
/// Interface definition for Soapy SDR devices.
///
/// \copyright
/// Copyright (c) 2014-2014 Josh Blum
/// SPDX-License-Identifier: BSL-1.0
///

#pragma once
#include <SoapySDR/Config.hpp>
#include <vector>
#include <map>
#include <string>
#include <complex>
#ifdef SOAPY_SDR_BOOST
#include <boost/shared_ptr.hpp>
#else
#include <memory> //shared ptr
#endif

namespace SoapySDR
{

//! Typedef for a dictionary of key-value string arguments
typedef std::map<std::string, std::string> Kwargs;

//! Typedef for a dictionary of string to numeric values
typedef std::map<std::string, double> NumericDict;

//! Typedef for a list of min/max range pairs
typedef std::vector<std::pair<double, double> > RangeList;

class SOAPY_SDR_API SDRDevice
{
public:

    #ifdef SOAPY_SDR_BOOST
    typedef boost::shared_ptr<SDRDevice> Sptr;
    #else
    typedef std::shared_ptr<SDRDevice> Sptr;
    #endif

    /*!
     * Make a new SDRDevice object given device construction args.
     * \param args device construction key/value argument markup
     * \return a pointer to a new SDRDevice object
     */
    static Sptr make(const std::string &args);

    /*******************************************************************
     * Channels API
     ******************************************************************/

    /*!
     * Get a list of names for all of the receive channels.
     */
    virtual std::vector<std::string> getRxChannels(void) const;

    /*!
     * Get a list of names for all of the transmit channels.
     */
    virtual std::vector<std::string> getTxChannels(void) const;

    /*******************************************************************
     * Antenna API
     ******************************************************************/

    /*!
     * Get a list of available antennas to select on a given chain.
     * \param channel an available channel on the device
     * \return a list of available antenna names
     */
    virtual std::vector<std::string> listAntennas(const std::string &channel) const;

    /*!
     * Set the selected antenna on a chain.
     * \param channel an available channel on the device
     * \param name the name of an available antenna
     */
    virtual void setAntenna(const std::string &channel, const std::string &name);

    /*!
     * Get the selected antenna on a chain.
     * \param channel an available channel on the device
     * \return the name of an available antenna
     */
    virtual std::string getAntenna(const std::string &channel) const;

    /*******************************************************************
     * Frontend corrections API
     ******************************************************************/

    /*!
     * Set the frontend DC offset correction.
     * \param channel an available channel on the device
     * \param offset the relative correction (1.0 max)
     */
    virtual void setDCOffset(const std::string &channel, const std::complex<double> &offset);

    /*!
     * Get the frontend DC offset correction.
     * \param channel an available channel on the device
     * \return the relative correction (1.0 max)
     */
    virtual std::complex<double> getDCOffset(const std::string &channel) const;

    /*!
     * Set the frontend IQ balance correction.
     * \param channel an available channel on the device
     * \param balance the relative correction (1.0 max)
     */
    virtual void setIQBalance(const std::string &channel, const std::complex<double> &balance);

    /*!
     * Get the frontend IQ balance correction.
     * \param channel an available channel on the device
     * \return the relative correction (1.0 max)
     */
    virtual std::complex<double> getIQBalance(const std::string &channel) const;

    /*******************************************************************
     * Gain API
     ******************************************************************/

    /*!
     * List available amplification elements.
     * \param channel an available channel
     */
    virtual std::vector<std::string> listGains(const std::string &channel) const;

    /*!
     * Set the automatic gain mode on the chain.
     * \param channel an available channel on the device
     * \param mode true for automatic gain setting
     */
    virtual void setGainMode(const std::string &channel, const bool mode);

     /*!
     * Get the automatic gain mode on the chain.
     * \param channel an available channel on the device
     * \return true for automatic gain setting
     */
    virtual bool getGainMode(const std::string &channel) const;

    /*!
     * Set the value of a amplification element in a chain.
     * \param channel an available channel on the device
     * \param name the name of an amplification element
     * \param value the new amplification value in dB
     */
    virtual void setGain(const std::string &channel, const std::string &name, const double value);

    /*!
     * Set the value of multiple amplification elements in a chain.
     * \param channel an available channel on the device
     * \param values a dictionary of element names to values
     */
    virtual void setGains(const std::string &channel, const NumericDict &values);

    /*!
     * Get the value of an individual amplification element in a chain.
     * \param channel an available channel on the device
     * \param name the name of an amplification element
     * \return the value of the gain in dB
     */
    virtual double getGainValue(const std::string &channel, const std::string &name) const;

    /*!
     * Get the valus of all amplification elements in a chain.
     * \param channel an available channel on the device
     * \return a dictionary of element names to values
     */
    virtual NumericDict getGainValues(const std::string &channel) const;

    /*!
     * Get the range of possible gain values.
     * \param channel an available channel on the device
     * \param name the name of an amplification element
     * \return a list of gain ranges in dB
     */
    virtual RangeList getGainRange(const std::string &channel, const std::string &name) const;

    /*******************************************************************
     * Frequency API
     ******************************************************************/

    /*!
     * Set the center frequency of the chain.
     * \param channel an available channel on the device
     * \param frequency the center frequency in Hz
     * \param args optional tuner arguments
     */
    virtual void setFrequency(const std::string &channel, const double frequency, const Kwargs &args = Kwargs());

    /*!
     * Set the center frequency of the chain with control over individual components.
     * \param channel an available channel on the device
     * \param values a dictionary of component names to frequency values in Hz
     * \param args optional tuner arguments
     */
    virtual void setFrequency(const std::string &channel, const NumericDict &values, const Kwargs &args = Kwargs());

    /*!
     * Get the center frequency of the chain.
     * \param channel an available channel on the device
     * \return the center frequency in Hz
     */
    virtual double getFrequency(const std::string &channel) const;

    /*!
     * Get the frequencies of various elements in the chain.
     * \param channel an available channel on the device
     * \return a dictionary of tunable elements to frequencies in Hz
     */
    virtual NumericDict getFrequencyComponents(const std::string &channel) const;

    /*!
     * Get the range of possible frequency values.
     * \param channel an available channel on the device
     * \return a list of frequency ranges in Hz
     */
    virtual RangeList getFrequencyRange(const std::string &channel) const;

    /*******************************************************************
     * Sample Rate API
     ******************************************************************/

    /*!
     * Set the baseband sample rate of the chain.
     * \param channel an available channel on the device
     * \param rate the sample rate in samples per second
     */
    virtual void setSampleRate(const std::string &channel, const double rate);

    /*!
     * Get the baseband sample rate of the chain.
     * \param channel an available channel on the device
     * \return the sample rate in samples per second
     */
    virtual double getSampleRate(const std::string &channel) const;

    /*!
     * Get the range of possible baseband sample rates.
     * \param channel an available channel on the device
     * \return a list of possible rates in samples per second
     */
    virtual RangeList getSampleRateRange(const std::string &channel) const;

    /*!
     * Set the baseband filter width of the chain.
     * \param channel an available channel on the device
     * \param bw the baseband filter width in Hz
     */
    virtual void setBandwidth(const std::string &channel, const double bw);

    /*!
     * Get the baseband filter width of the chain.
     * \param channel an available channel on the device
     * \return the baseband filter width in Hz
     */
    virtual double getBandwidth(const std::string &channel) const;

    /*!
     * Get the range of possible baseband filter widths.
     * \param channel an available channel on the device
     * \return a list of possible bandwidths in Hz
     */
    virtual RangeList getBandwidthRange(const std::string &channel) const;

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
