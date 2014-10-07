// Copyright (c) 2014-2014 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Device.hpp>
#include <SoapySDR/Registry.hpp>
#include <uhd/device.hpp>
#include <uhd/usrp/multi_usrp.hpp>

/***********************************************************************
 * Helpful type conversions
 **********************************************************************/
static SoapySDR::Kwargs dictToKwargs(const uhd::device_addr_t &addr)
{
    SoapySDR::Kwargs kwargs;
    const std::vector<std::string> keys = addr.keys();
    for (size_t i = 0; i < keys.size(); i++)
    {
        kwargs[keys[i]] = addr[keys[i]];
    }
    return kwargs;
}

static uhd::device_addr_t kwargsToDict(const SoapySDR::Kwargs &kwargs)
{
    uhd::device_addr_t addr;
    for (SoapySDR::Kwargs::const_iterator it = kwargs.begin(); it != kwargs.end(); ++it)
    {
        addr[it->first] = it->second;
    }
    return addr;
}

static SoapySDR::RangeList metaRangeToRangeList(const uhd::meta_range_t &metaRange)
{
    SoapySDR::RangeList out;
    for (size_t i = 0; i < metaRange.size(); i++)
    {
        out.push_back(SoapySDR::Range(metaRange[i].start(), metaRange[i].stop()));
    }
    return out;
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
        if (dir == SoapySDR::TX) return metaRangeToRangeList(_dev->get_tx_gain_range(name, channel));
        if (dir == SoapySDR::RX) return metaRangeToRangeList(_dev->get_rx_gain_range(name, channel));
        return SoapySDR::Device::getGainRange(dir, channel, name);
    }

    SoapySDR::RangeList getGainRange(const SoapySDR::Direction dir, const size_t channel) const
    {
        if (dir == SoapySDR::TX) return metaRangeToRangeList(_dev->get_tx_gain_range(channel));
        if (dir == SoapySDR::RX) return metaRangeToRangeList(_dev->get_rx_gain_range(channel));
        return SoapySDR::Device::getGainRange(dir, channel);
    }

    /*******************************************************************
     * Frequency support
     ******************************************************************/

    void setFrequency(const SoapySDR::Direction dir, const size_t channel, const double frequency, const SoapySDR::Kwargs &args)
    {
        uhd::tune_request_t tr(frequency);
        tr.args = kwargsToDict(args);
        if (dir == SoapySDR::TX) _trCache[dir][channel] = _dev->set_tx_freq(tr, channel);
        if (dir == SoapySDR::RX) _trCache[dir][channel] = _dev->set_rx_freq(tr, channel);
    }

    void setFrequency(const SoapySDR::Direction dir, const size_t channel, const SoapySDR::NumericDict &values, const SoapySDR::Kwargs &args)
    {
        uhd::tune_request_t tr(0.0);
        tr.target_freq = values.at("target");

        if (values.count("rf") != 0)
        {
            tr.rf_freq = values.at("rf");
            tr.rf_freq_policy = uhd::tune_request_t::POLICY_MANUAL;
        }
        if (values.count("bb") != 0)
        {
            tr.dsp_freq = values.at("bb");
            tr.dsp_freq_policy = uhd::tune_request_t::POLICY_MANUAL;
        }

        tr.args = kwargsToDict(args);
        if (dir == SoapySDR::TX) _trCache[dir][channel] = _dev->set_tx_freq(tr, channel);
        if (dir == SoapySDR::RX) _trCache[dir][channel] = _dev->set_rx_freq(tr, channel);
    }

    std::map<SoapySDR::Direction, std::map<size_t, uhd::tune_result_t> > _trCache;

    double getFrequency(const SoapySDR::Direction dir, const size_t channel) const
    {
        if (dir == SoapySDR::TX) return _dev->get_tx_freq(channel);
        if (dir == SoapySDR::RX) return _dev->get_rx_freq(channel);
        return SoapySDR::Device::getFrequency(dir, channel);
    }

    SoapySDR::NumericDict getFrequencyComponents(const SoapySDR::Direction dir, const size_t channel) const
    {
        const uhd::tune_result_t tr = _trCache.at(dir).at(channel);
        SoapySDR::NumericDict result;
        result["request_rf"] = tr.target_rf_freq;
        result["result_rf"] = tr.actual_rf_freq;
        result["request_bb"] = tr.target_dsp_freq;
        result["result_bb"] = tr.actual_dsp_freq;
        return result;
    }

    SoapySDR::RangeList getFrequencyRange(const SoapySDR::Direction dir, const size_t channel) const
    {
        if (dir == SoapySDR::TX) return metaRangeToRangeList(_dev->get_tx_freq_range(channel));
        if (dir == SoapySDR::RX) return metaRangeToRangeList(_dev->get_rx_freq_range(channel));
        return SoapySDR::Device::getFrequencyRange(dir, channel);
    }

    /*******************************************************************
     * Sample Rate support
     ******************************************************************/

    void setSampleRate(const SoapySDR::Direction dir, const size_t channel, const double rate)
    {
        if (dir == SoapySDR::TX) return _dev->set_tx_rate(rate, channel);
        if (dir == SoapySDR::RX) return _dev->set_rx_rate(rate, channel);
    }

    double getSampleRate(const SoapySDR::Direction dir, const size_t channel) const
    {
        if (dir == SoapySDR::TX) return _dev->get_tx_rate(channel);
        if (dir == SoapySDR::RX) return _dev->get_rx_rate(channel);
        return SoapySDR::Device::getSampleRate(dir, channel);
    }

    SoapySDR::RangeList getSampleRateRange(const SoapySDR::Direction dir, const size_t channel) const
    {
        if (dir == SoapySDR::TX) return metaRangeToRangeList(_dev->get_tx_rates(channel));
        if (dir == SoapySDR::RX) return metaRangeToRangeList(_dev->get_rx_rates(channel));
        return SoapySDR::Device::getSampleRateRange(dir, channel);
    }

    void setBandwidth(const SoapySDR::Direction dir, const size_t channel, const double bw)
    {
        if (dir == SoapySDR::TX) return _dev->set_tx_bandwidth(bw, channel);
        if (dir == SoapySDR::RX) return _dev->set_rx_bandwidth(bw, channel);
    }

    double getBandwidth(const SoapySDR::Direction dir, const size_t channel) const
    {
        if (dir == SoapySDR::TX) return _dev->get_tx_bandwidth(channel);
        if (dir == SoapySDR::RX) return _dev->get_rx_bandwidth(channel);
        return SoapySDR::Device::getBandwidth(dir, channel);
    }

    SoapySDR::RangeList getBandwidthRange(const SoapySDR::Direction dir, const size_t channel) const
    {
        if (dir == SoapySDR::TX) return metaRangeToRangeList(_dev->get_tx_bandwidth_range(channel));
        if (dir == SoapySDR::RX) return metaRangeToRangeList(_dev->get_rx_bandwidth_range(channel));
        return SoapySDR::Device::getBandwidthRange(dir, channel);
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
