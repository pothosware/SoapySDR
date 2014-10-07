// Copyright (c) 2014-2014 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Device.hpp>
#include <SoapySDR/Registry.hpp>
#include <uhd/device.hpp>
#include <uhd/usrp/multi_usrp.hpp>

/***********************************************************************
 * Helpful type conversions
 **********************************************************************/
SoapySDR::Kwargs dictToKwargs(const uhd::device_addr_t &addr)
{
    SoapySDR::Kwargs kwargs;
    const std::vector<std::string> keys = addr.keys();
    for (size_t i = 0; i < keys.size(); i++)
    {
        kwargs[keys[i]] = addr[keys[i]];
    }
    return kwargs;
}

uhd::device_addr_t kwargsToDict(const SoapySDR::Kwargs &kwargs)
{
    uhd::device_addr_t addr;
    for (SoapySDR::Kwargs::const_iterator it = kwargs.begin(); it != kwargs.end(); ++it)
    {
        addr[it->first] = it->second;
    }
    return addr;
}

/***********************************************************************
 * Device interface
 **********************************************************************/
class SoapyUHDDevice : public SoapySDR::Device
{
public:
    SoapyUHDDevice(uhd::usrp::multi_usrp::sptr dev):
        _dev(dev)
    {
        return;
    }

    /*******************************************************************
     * Channels support
     ******************************************************************/
    void setFrontendMapping(const SoapySDR::Direction dir, const std::string &mapping)
    {
        if (dir == SoapySDR::TX) return _dev->set_tx_subdev_spec(mapping);
        if (dir == SoapySDR::RX) return _dev->set_rx_subdev_spec(mapping);
    }

    size_t getNumChannels(const SoapySDR::Direction dir) const
    {
        if (dir == SoapySDR::TX) return _dev->get_tx_num_channels();
        if (dir == SoapySDR::RX) return _dev->get_rx_num_channels();
        return SoapySDR::Device::getNumChannels(dir);
    }

    /*******************************************************************
     * Stream support
     ******************************************************************/
    void *setupStream(const SoapySDR::Direction dir, const std::vector<size_t> &, const SoapySDR::Kwargs &)
    {
    }

    void closeStream(void *handle)
    {
    }

    int readStream(void *handle, void * const *buffs, const size_t numElems, int &flags, long long &, const long)
    {
    }

    int writeStream(void *handle, const void * const *buffs, const size_t numElems, int &flags, const long long, const long)
    {
    }

    /*******************************************************************
     * Antenna support
     ******************************************************************/

    std::vector<std::string> listAntennas(const SoapySDR::Direction dir, const size_t channel) const
    {
        if (dir == SoapySDR::TX) return _dev->get_tx_antennas(channel);
        if (dir == SoapySDR::RX) return _dev->get_rx_antennas(channel);
        return SoapySDR::Device::listAntennas(dir, channel);
    }

    void setAntenna(const SoapySDR::Direction dir, const size_t channel, const std::string &name)
    {
        if (dir == SoapySDR::TX) _dev->set_tx_antenna(name, channel);
        if (dir == SoapySDR::RX) _dev->set_rx_antenna(name, channel);
    }

    std::string getAntenna(const SoapySDR::Direction dir, const size_t channel) const
    {
        if (dir == SoapySDR::TX) return _dev->get_tx_antenna(channel);
        if (dir == SoapySDR::RX) return _dev->get_rx_antenna(channel);
        return SoapySDR::Device::getAntenna(dir, channel);
    }

    /*******************************************************************
     * Frontend corrections support
     ******************************************************************/

    void setDCOffset(const SoapySDR::Direction dir, const size_t channel, const std::complex<double> &offset)
    {
        if (dir == SoapySDR::TX) _dev->set_tx_dc_offset(offset, channel);
        if (dir == SoapySDR::RX) _dev->set_rx_dc_offset(offset, channel);
    }

    void setIQBalance(const SoapySDR::Direction dir, const size_t channel, const std::complex<double> &balance)
    {
        if (dir == SoapySDR::TX) _dev->set_tx_iq_balance(balance, channel);
        if (dir == SoapySDR::RX) _dev->set_rx_iq_balance(balance, channel);
    }

    /*******************************************************************
     * Gain support
     ******************************************************************/

    std::vector<std::string> listGains(const SoapySDR::Direction dir, const size_t channel) const
    {
        if (dir == SoapySDR::TX) return _dev->get_tx_gain_names(channel);
        if (dir == SoapySDR::RX) return _dev->get_rx_gain_names(channel);
        return SoapySDR::Device::listGains(dir, channel);
    }

    void setGain(const SoapySDR::Direction dir, const size_t channel, const double value)
    {
        if (dir == SoapySDR::TX) _dev->set_tx_gain(value, channel);
        if (dir == SoapySDR::RX) _dev->set_rx_gain(value, channel);
    }

    void setGain(const SoapySDR::Direction dir, const size_t channel, const std::string &name, const double value)
    {
        if (dir == SoapySDR::TX) _dev->set_tx_gain(value, name, channel);
        if (dir == SoapySDR::RX) _dev->set_rx_gain(value, name, channel);
    }

    double getGainValue(const SoapySDR::Direction dir, const size_t channel) const
    {
        if (dir == SoapySDR::TX) return _dev->get_tx_gain(channel);
        if (dir == SoapySDR::RX) return _dev->get_rx_gain(channel);
        return SoapySDR::Device::getGainValue(dir, channel);
    }

    double getGainValue(const SoapySDR::Direction dir, const size_t channel, const std::string &name) const
    {
        if (dir == SoapySDR::TX) return _dev->get_tx_gain(name, channel);
        if (dir == SoapySDR::RX) return _dev->get_rx_gain(name, channel);
        return SoapySDR::Device::getGainValue(dir, channel, name);
    }

    SoapySDR::RangeList getGainRange(const SoapySDR::Direction dir, const size_t channel, const std::string &name) const
    {
    }

    SoapySDR::RangeList getGainRange(const SoapySDR::Direction dir, const size_t channel) const
    {
    }

    /*******************************************************************
     * Frequency support
     ******************************************************************/

    void setFrequency(const SoapySDR::Direction dir, const size_t channel, const double frequency, const SoapySDR::Kwargs &)
    {
    }

    double getFrequency(const SoapySDR::Direction dir, const size_t channel) const
    {
    }

    SoapySDR::RangeList getFrequencyRange(const SoapySDR::Direction dir, const size_t channel) const
    {
    }

    /*******************************************************************
     * Sample Rate support
     ******************************************************************/

    void setSampleRate(const SoapySDR::Direction dir, const size_t, const double rate)
    {
    }

    double getSampleRate(const SoapySDR::Direction dir, const size_t channel) const
    {
    }

    SoapySDR::RangeList getSampleRateRange(const SoapySDR::Direction dir, const size_t channel) const
    {
    }

    void setBandwidth(const SoapySDR::Direction dir, const size_t channel, const double bw)
    {
    }

    double getBandwidth(const SoapySDR::Direction dir, const size_t channel) const
    {
    }

    SoapySDR::RangeList getBandwidthRange(const SoapySDR::Direction dir, const size_t channel) const
    {
    }

    /*******************************************************************
     * Clocking support
     ******************************************************************/
private:
    uhd::usrp::multi_usrp::sptr _dev;
};

/***********************************************************************
 * Registration
 **********************************************************************/
std::vector<SoapySDR::Kwargs> find_uhd(const SoapySDR::Kwargs &args)
{
    //perform the discovery
    const uhd::device_addrs_t addrs = uhd::device::find(kwargsToDict(args), uhd::device::USRP);

    //convert addrs to results
    std::vector<SoapySDR::Kwargs> results;
    for (size_t i = 0; i < addrs.size(); i++)
    {
        results.push_back(dictToKwargs(addrs[i]));
    }
    return results;
}

SoapySDR::Device *make_uhd(const SoapySDR::Kwargs &args)
{
    return new SoapyUHDDevice(uhd::usrp::multi_usrp::make(kwargsToDict(args)));
}

static SoapySDR::Registry register__uhd("uhd", &find_uhd, &make_uhd, SOAPY_SDR_ABI_VERSION);
