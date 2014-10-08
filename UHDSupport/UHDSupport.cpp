// Copyright (c) 2014-2014 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Device.hpp>
#include <SoapySDR/Registry.hpp>
#include <uhd/device.hpp>
#include <uhd/usrp/multi_usrp.hpp>
#include <set>

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

static SoapySDR::Range metaRangeToRange(const uhd::meta_range_t &metaRange)
{
    return SoapySDR::Range(metaRange.start(), metaRange.stop());
}

static std::vector<double> metaRangeToNumericList(const uhd::meta_range_t &metaRange)
{
    std::vector<double> out;
    for (size_t i = 0; i < metaRange.size(); i++)
    {
        //in these cases start == stop
        out.push_back(metaRange[i].start());
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

    std::string getFrontendMapping(const SoapySDR::Direction dir) const
    {
        if (dir == SoapySDR::TX) return _dev->get_tx_subdev_spec().to_string();
        if (dir == SoapySDR::RX) return _dev->get_rx_subdev_spec().to_string();
        return SoapySDR::Device::getFrontendMapping(dir);
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
    void *setupStream(const SoapySDR::Direction dir, const std::vector<size_t> &channels, const SoapySDR::Kwargs &args)
    {
        //convert input to stream args
        uhd::stream_args_t stream_args;
        stream_args.channels = channels;
        stream_args.args = kwargsToDict(args);
        if (args.count("host") != 0) stream_args.cpu_format = args.at("host");
        if (args.count("wire") != 0) stream_args.otw_format = args.at("wire");

        //create streamers
        if (dir == SoapySDR::TX)
        {
            uhd::tx_streamer::sptr stream = _dev->get_tx_stream(stream_args);
            _activeStreams.insert(stream);
            return stream.get();
        }

        if (dir == SoapySDR::RX)
        {
            uhd::rx_streamer::sptr stream = _dev->get_rx_stream(stream_args);
            _activeStreams.insert(stream);
            return stream.get();
        }

        return NULL;
    }

    void closeStream(void *handle)
    {
        for (std::set<boost::shared_ptr<void> >::iterator it = _activeStreams.begin(); it != _activeStreams.end(); ++it)
        {
            if (handle == it->get())
            {
                _activeStreams.erase(it);
                return;
            }
        }
    }

    //maintain sptrs to active streams, removed by close
    std::set<boost::shared_ptr<void> > _activeStreams;

    int readStream(void *handle, void * const *buffs, const size_t numElems, int &flags, long long &timeNs, const long timeoutUs)
    {
        uhd::rx_streamer *stream = reinterpret_cast<uhd::rx_streamer *>(handle);

        //receive into buffers and metadata
        uhd::rx_metadata_t md;
        uhd::rx_streamer::buffs_type stream_buffs(buffs, stream->get_num_channels());
        int ret = stream->recv(stream_buffs, numElems, md, timeoutUs/1e6, (flags & SOAPY_SDR_ONE_PACKET) != 0);

        //parse the metadata
        flags = 0;
        if (md.has_time_spec) flags |= SOAPY_SDR_HAS_TIME;
        if (md.end_of_burst) flags |= SOAPY_SDR_END_BURST;
        timeNs = md.time_spec.to_ticks(1e9);
        switch (md.error_code)
        {
        case uhd::rx_metadata_t::ERROR_CODE_NONE: return ret;
        case uhd::rx_metadata_t::ERROR_CODE_OVERFLOW: return SOAPY_SDR_OVERFLOW;
        case uhd::rx_metadata_t::ERROR_CODE_TIMEOUT: return SOAPY_SDR_TIMEOUT;
        case uhd::rx_metadata_t::ERROR_CODE_BAD_PACKET: return SOAPY_SDR_CORRUPTION;
        case uhd::rx_metadata_t::ERROR_CODE_ALIGNMENT: return SOAPY_SDR_CORRUPTION;
        case uhd::rx_metadata_t::ERROR_CODE_LATE_COMMAND: return SOAPY_SDR_STREAM_ERROR;
        case uhd::rx_metadata_t::ERROR_CODE_BROKEN_CHAIN: return SOAPY_SDR_STREAM_ERROR;
        }
        return ret;
    }

    int writeStream(void *handle, const void * const *buffs, const size_t numElems, int &flags, const long long timeNs, const long timeoutUs)
    {
        uhd::tx_streamer *stream = reinterpret_cast<uhd::tx_streamer *>(handle);

        //load metadata
        uhd::tx_metadata_t md;
        md.has_time_spec = (flags & SOAPY_SDR_HAS_TIME) != 0;
        md.end_of_burst = (flags & SOAPY_SDR_END_BURST) != 0;
        md.time_spec = uhd::time_spec_t::from_ticks(timeNs, 1e9);

        //send buffers and metadata
        uhd::tx_streamer::buffs_type stream_buffs(buffs, stream->get_num_channels());
        int ret = stream->send(stream_buffs, numElems, md, timeoutUs/1e6);

        flags = 0;
        return ret;
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

    double getGain(const SoapySDR::Direction dir, const size_t channel) const
    {
        if (dir == SoapySDR::TX) return _dev->get_tx_gain(channel);
        if (dir == SoapySDR::RX) return _dev->get_rx_gain(channel);
        return SoapySDR::Device::getGain(dir, channel);
    }

    double getGain(const SoapySDR::Direction dir, const size_t channel, const std::string &name) const
    {
        if (dir == SoapySDR::TX) return _dev->get_tx_gain(name, channel);
        if (dir == SoapySDR::RX) return _dev->get_rx_gain(name, channel);
        return SoapySDR::Device::getGain(dir, channel, name);
    }

    SoapySDR::Range getGainRange(const SoapySDR::Direction dir, const size_t channel, const std::string &name) const
    {
        if (dir == SoapySDR::TX) return metaRangeToRange(_dev->get_tx_gain_range(name, channel));
        if (dir == SoapySDR::RX) return metaRangeToRange(_dev->get_rx_gain_range(name, channel));
        return SoapySDR::Device::getGainRange(dir, channel, name);
    }

    SoapySDR::Range getGainRange(const SoapySDR::Direction dir, const size_t channel) const
    {
        if (dir == SoapySDR::TX) return metaRangeToRange(_dev->get_tx_gain_range(channel));
        if (dir == SoapySDR::RX) return metaRangeToRange(_dev->get_rx_gain_range(channel));
        return SoapySDR::Device::getGainRange(dir, channel);
    }

    /*******************************************************************
     * Frequency support
     ******************************************************************/

    void setFrequency(const SoapySDR::Direction dir, const size_t channel, const double frequency, const SoapySDR::Kwargs &args)
    {
        uhd::tune_request_t tr(frequency);

        if (args.count("OFFSET") != 0)
        {
            tr = uhd::tune_request_t(frequency, boost::lexical_cast<double>(args.at("OFFSET")));
        }
        if (args.count("RF") != 0)
        {
            tr.rf_freq = boost::lexical_cast<double>(args.at("RF"));
            tr.rf_freq_policy = uhd::tune_request_t::POLICY_MANUAL;
        }
        if (args.count("BB") != 0)
        {
            tr.dsp_freq = boost::lexical_cast<double>(args.at("BB"));
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

    double getFrequency(const SoapySDR::Direction dir, const size_t channel, const std::string &name) const
    {
        const uhd::tune_result_t tr = _trCache.at(dir).at(channel);
        if (name == "RF") return tr.actual_rf_freq;
        if (name == "BB") return tr.actual_dsp_freq;
        return SoapySDR::Device::getFrequency(dir, channel, name);
    }

    std::vector<std::string> listFrequencies(const SoapySDR::Direction, const size_t) const
    {
        std::vector<std::string> comps;
        comps.push_back("RF");
        comps.push_back("BB");
        return comps;
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

    std::vector<double> listSampleRates(const SoapySDR::Direction dir, const size_t channel) const
    {
        if (dir == SoapySDR::TX) return metaRangeToNumericList(_dev->get_tx_rates(channel));
        if (dir == SoapySDR::RX) return metaRangeToNumericList(_dev->get_rx_rates(channel));
        return SoapySDR::Device::listSampleRates(dir, channel);
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

    std::vector<double> listBandwidths(const SoapySDR::Direction dir, const size_t channel) const
    {
        if (dir == SoapySDR::TX) return metaRangeToNumericList(_dev->get_tx_bandwidth_range(channel));
        if (dir == SoapySDR::RX) return metaRangeToNumericList(_dev->get_rx_bandwidth_range(channel));
        return SoapySDR::Device::listBandwidths(dir, channel);
    }

    /*******************************************************************
     * Clocking support
     ******************************************************************/

    void setMasterClockRate(const double rate)
    {
        _dev->set_master_clock_rate(rate);
    }

    double getMasterClockRate(void) const
    {
        return _dev->get_master_clock_rate();
    }

    std::vector<std::string> listClockSources(void) const
    {
        return _dev->get_clock_sources(0);
    }

    void setClockSource(const std::string &source)
    {
        _dev->set_clock_source(source, 0);
    }

    std::string getClockSource(void) const
    {
        return _dev->get_clock_source(0);
    }

    std::vector<std::string> listTimeSources(void) const
    {
        return _dev->get_time_sources(0);
    }

    void setTimeSource(const std::string &source)
    {
        _dev->set_time_source(source, 0);
    }

    std::string getTimeSource(void) const
    {
        return _dev->get_time_source(0);
    }

    /*******************************************************************
     * Time support
     ******************************************************************/

    long long getHardwareTime(const std::string &what) const
    {
        if (what == "PPS") return _dev->get_time_last_pps().to_ticks(1e9);
        return _dev->get_time_now().to_ticks(1e9);
    }

    void setHardwareTime(const long long timeNs, const std::string &what)
    {
        uhd::time_spec_t time = uhd::time_spec_t::from_ticks(timeNs, 1e9);
        if (what == "PPS") return _dev->set_time_next_pps(time);
        return _dev->set_time_now(time);
    }

    void setCommandTime(const long long timeNs, const std::string &)
    {
        if (timeNs == 0) _dev->clear_command_time();
        else _dev->set_command_time(uhd::time_spec_t::from_ticks(timeNs, 1e9));
    }

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
