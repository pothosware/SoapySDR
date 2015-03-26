// Copyright (c) 2014-2015 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include "TypeHelpers.hpp"
#include <SoapySDR/Device.hpp>
#include <SoapySDR/Registry.hpp>
#include <SoapySDR/Logger.hpp>
#include <uhd/version.hpp>
#include <uhd/device.hpp>
#include <uhd/utils/msg.hpp>
#include <uhd/usrp/multi_usrp.hpp>
#include <uhd/property_tree.hpp>
#include <cctype>
#include <iostream>

/***********************************************************************
 * Stream wrapper
 **********************************************************************/
struct SoapyUHDStream
{
    uhd::rx_streamer::sptr rx;
    uhd::tx_streamer::sptr tx;
};

/***********************************************************************
 * Device interface
 **********************************************************************/
class SoapyUHDDevice : public SoapySDR::Device
{
public:
    SoapyUHDDevice(uhd::usrp::multi_usrp::sptr dev, const std::string &type):
        _dev(dev),
        _type(type)
    {
        return;
    }

    /*******************************************************************
     * Identification API
     ******************************************************************/
    std::string getDriverKey(void) const
    {
        return _type;
    }

    std::string getHardwareKey(void) const
    {
        return _dev->get_mboard_name();
    }

    SoapySDR::Kwargs getHardwareInfo(void) const
    {
        SoapySDR::Kwargs out;
        for (size_t i = 0; i < this->getNumChannels(SOAPY_SDR_TX); i++)
        {
            const uhd::dict<std::string, std::string> info = _dev->get_usrp_tx_info(i);
            BOOST_FOREACH (const std::string &key, info.keys())
            {
                if (key.size() > 3 and key.substr(0, 3) == "tx_")
                    out[str(boost::format("tx%d_%s") % i % key.substr(3))] = info[key];
                else out[key] = info[key];
            }
        }
        for (size_t i = 0; i < this->getNumChannels(SOAPY_SDR_RX); i++)
        {
            const uhd::dict<std::string, std::string> info = _dev->get_usrp_rx_info(i);
            BOOST_FOREACH (const std::string &key, info.keys())
            {
                if (key.size() > 3 and key.substr(0, 3) == "rx_")
                    out[str(boost::format("rx%d_%s") % i % key.substr(3))] = info[key];
                else out[key] = info[key];
            }
        }

        uhd::property_tree::sptr tree = _dev->get_device()->get_tree();
        if (tree->exists("/mboards/0/fw_version")) out["fw_version"] = tree->access<std::string>("/mboards/0/fw_version").get();
        if (tree->exists("/mboards/0/fpga_version")) out["fpga_version"] = tree->access<std::string>("/mboards/0/fpga_version").get();

        return out;
    }

    /*******************************************************************
     * Channels support
     ******************************************************************/
    void setFrontendMapping(const int dir, const std::string &mapping)
    {
        if (dir == SOAPY_SDR_TX) return _dev->set_tx_subdev_spec(mapping);
        if (dir == SOAPY_SDR_RX) return _dev->set_rx_subdev_spec(mapping);
    }

    std::string getFrontendMapping(const int dir) const
    {
        if (dir == SOAPY_SDR_TX) return _dev->get_tx_subdev_spec().to_string();
        if (dir == SOAPY_SDR_RX) return _dev->get_rx_subdev_spec().to_string();
        return SoapySDR::Device::getFrontendMapping(dir);
    }

    size_t getNumChannels(const int dir) const
    {
        if (dir == SOAPY_SDR_TX) return _dev->get_tx_num_channels();
        if (dir == SOAPY_SDR_RX) return _dev->get_rx_num_channels();
        return SoapySDR::Device::getNumChannels(dir);
    }

    /*******************************************************************
     * Stream support
     ******************************************************************/
    SoapySDR::Stream *setupStream(const int dir, const std::string &format, const std::vector<size_t> &channels, const SoapySDR::Kwargs &args)
    {
        std::string hostFormat;
        BOOST_FOREACH(const char ch, format)
        {
            if (ch == 'C') hostFormat += "c";
            else if (ch == 'F') hostFormat = "f" + hostFormat;
            else if (ch == 'S') hostFormat = "s" + hostFormat;
            else if (std::isdigit(ch)) hostFormat += ch;
            else throw std::runtime_error("SoapyUHDDevice::setupStream("+format+") unknown format");
        }

        //convert input to stream args
        uhd::stream_args_t stream_args(hostFormat);
        stream_args.channels = channels;
        stream_args.args = kwargsToDict(args);
        if (args.count("WIRE") != 0) stream_args.otw_format = args.at("WIRE");

        //create streamers
        SoapyUHDStream *stream = new SoapyUHDStream();
        if (dir == SOAPY_SDR_TX) stream->tx = _dev->get_tx_stream(stream_args);
        if (dir == SOAPY_SDR_RX) stream->rx = _dev->get_rx_stream(stream_args);
        return reinterpret_cast<SoapySDR::Stream *>(stream);
    }

    void closeStream(SoapySDR::Stream *handle)
    {
        SoapyUHDStream *stream = reinterpret_cast<SoapyUHDStream *>(handle);
        delete stream;
    }

    size_t getStreamMTU(SoapySDR::Stream *handle) const
    {
        SoapyUHDStream *stream = reinterpret_cast<SoapyUHDStream *>(handle);
        if (stream->rx) return stream->rx->get_max_num_samps();
        if (stream->tx) return stream->tx->get_max_num_samps();
        return SoapySDR::Device::getStreamMTU(handle);
    }

    int activateStream(SoapySDR::Stream *handle, const int flags, const long long timeNs, const size_t numElems)
    {
        SoapyUHDStream *stream = reinterpret_cast<SoapyUHDStream *>(handle);
        if (not stream->rx) return SoapySDR::Device::activateStream(handle, flags, timeNs, numElems);

        //determine stream mode
        uhd::stream_cmd_t::stream_mode_t mode;
        if (numElems == 0) mode = uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS;
        else if ((flags & SOAPY_SDR_END_BURST) != 0) mode = uhd::stream_cmd_t::STREAM_MODE_NUM_SAMPS_AND_DONE;
        else mode = uhd::stream_cmd_t::STREAM_MODE_NUM_SAMPS_AND_MORE;

        //fill in the command
        uhd::stream_cmd_t cmd(mode);
        cmd.stream_now = (flags & SOAPY_SDR_HAS_TIME) == 0;
        cmd.time_spec = uhd::time_spec_t::from_ticks(timeNs, 1e9);
        cmd.num_samps = numElems;

        //issue command
        stream->rx->issue_stream_cmd(cmd);
        return 0;
    }

    int deactivateStream(SoapySDR::Stream *handle, const int flags, const long long timeNs)
    {
        SoapyUHDStream *stream = reinterpret_cast<SoapyUHDStream *>(handle);
        if (not stream->rx) return SoapySDR::Device::deactivateStream(handle, flags, timeNs);
        if (flags != 0) return SOAPY_SDR_NOT_SUPPORTED;

        //stop the stream
        stream->rx->issue_stream_cmd(uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS);
        return 0;
    }

    int readStream(SoapySDR::Stream *handle, void * const *buffs, const size_t numElems, int &flags, long long &timeNs, const long timeoutUs)
    {
        uhd::rx_streamer::sptr &stream = reinterpret_cast<SoapyUHDStream *>(handle)->rx;

        //receive into buffers and metadata
        uhd::rx_metadata_t md;
        uhd::rx_streamer::buffs_type stream_buffs(buffs, stream->get_num_channels());
        int ret = stream->recv(stream_buffs, numElems, md, timeoutUs/1e6, (flags & SOAPY_SDR_ONE_PACKET) != 0);

        //parse the metadata
        flags = 0;
        if (md.has_time_spec) flags |= SOAPY_SDR_HAS_TIME;
        if (md.end_of_burst) flags |= SOAPY_SDR_END_BURST;
        if (md.more_fragments) flags |= SOAPY_SDR_MORE_FRAGMENTS;
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

    int writeStream(SoapySDR::Stream *handle, const void * const *buffs, const size_t numElems, int &flags, const long long timeNs, const long timeoutUs)
    {
        uhd::tx_streamer::sptr &stream = reinterpret_cast<SoapyUHDStream *>(handle)->tx;

        //load metadata
        uhd::tx_metadata_t md;
        md.has_time_spec = (flags & SOAPY_SDR_HAS_TIME) != 0;
        md.end_of_burst = (flags & SOAPY_SDR_END_BURST) != 0;
        md.time_spec = uhd::time_spec_t::from_ticks(timeNs, 1e9);

        //send buffers and metadata
        uhd::tx_streamer::buffs_type stream_buffs(buffs, stream->get_num_channels());
        int ret = stream->send(stream_buffs, numElems, md, timeoutUs/1e6);

        flags = 0;
        //consider a return of 0 to be a complete timeout
        if (ret == 0) return SOAPY_SDR_TIMEOUT;
        return ret;
    }

    int readStreamStatus(SoapySDR::Stream *handle, size_t &chanMask, int &flags, long long &timeNs, const long timeoutUs)
    {
        if (reinterpret_cast<SoapyUHDStream *>(handle)->rx) return SOAPY_SDR_NOT_SUPPORTED;
        uhd::tx_streamer::sptr &stream = reinterpret_cast<SoapyUHDStream *>(handle)->tx;

        uhd::async_metadata_t md;
        if (not stream->recv_async_msg(md, timeoutUs/1e6)) return SOAPY_SDR_TIMEOUT;

        chanMask = (1 << md.channel);
        flags = 0;
        if (md.has_time_spec) flags |= SOAPY_SDR_HAS_TIME;
        timeNs = md.time_spec.to_ticks(1e9);

        switch (md.event_code)
        {
        case uhd::async_metadata_t::EVENT_CODE_BURST_ACK: flags |= SOAPY_SDR_END_BURST; break;
        case uhd::async_metadata_t::EVENT_CODE_UNDERFLOW: return SOAPY_SDR_UNDERFLOW;
        case uhd::async_metadata_t::EVENT_CODE_SEQ_ERROR: return SOAPY_SDR_CORRUPTION;
        case uhd::async_metadata_t::EVENT_CODE_TIME_ERROR: return SOAPY_SDR_TIME_ERROR;
        case uhd::async_metadata_t::EVENT_CODE_UNDERFLOW_IN_PACKET: return SOAPY_SDR_UNDERFLOW;
        case uhd::async_metadata_t::EVENT_CODE_SEQ_ERROR_IN_BURST: return SOAPY_SDR_CORRUPTION;
        case uhd::async_metadata_t::EVENT_CODE_USER_PAYLOAD: break;
        }
        return 0;
    }

    /*******************************************************************
     * Antenna support
     ******************************************************************/

    std::vector<std::string> listAntennas(const int dir, const size_t channel) const
    {
        if (dir == SOAPY_SDR_TX) return _dev->get_tx_antennas(channel);
        if (dir == SOAPY_SDR_RX) return _dev->get_rx_antennas(channel);
        return SoapySDR::Device::listAntennas(dir, channel);
    }

    void setAntenna(const int dir, const size_t channel, const std::string &name)
    {
        if (dir == SOAPY_SDR_TX) _dev->set_tx_antenna(name, channel);
        if (dir == SOAPY_SDR_RX) _dev->set_rx_antenna(name, channel);
    }

    std::string getAntenna(const int dir, const size_t channel) const
    {
        if (dir == SOAPY_SDR_TX) return _dev->get_tx_antenna(channel);
        if (dir == SOAPY_SDR_RX) return _dev->get_rx_antenna(channel);
        return SoapySDR::Device::getAntenna(dir, channel);
    }

    /*******************************************************************
     * Frontend corrections support
     ******************************************************************/
    void setDCOffsetMode(const int dir, const size_t channel, const bool automatic)
    {
        if (dir == SOAPY_SDR_RX) _dev->set_rx_dc_offset(automatic, channel);
    }

    void setDCOffset(const int dir, const size_t channel, const std::complex<double> &offset)
    {
        if (dir == SOAPY_SDR_TX) _dev->set_tx_dc_offset(offset, channel);
        if (dir == SOAPY_SDR_RX) _dev->set_rx_dc_offset(offset, channel);
    }

    void setIQBalance(const int dir, const size_t channel, const std::complex<double> &balance)
    {
        if (dir == SOAPY_SDR_TX) _dev->set_tx_iq_balance(balance, channel);
        if (dir == SOAPY_SDR_RX) _dev->set_rx_iq_balance(balance, channel);
    }

    /*******************************************************************
     * Gain support
     ******************************************************************/

    std::vector<std::string> listGains(const int dir, const size_t channel) const
    {
        if (dir == SOAPY_SDR_TX) return _dev->get_tx_gain_names(channel);
        if (dir == SOAPY_SDR_RX) return _dev->get_rx_gain_names(channel);
        return SoapySDR::Device::listGains(dir, channel);
    }

    void setGainMode(const int dir, const size_t channel, const bool automatic)
    {
        #ifdef UHD_HAS_SET_RX_AGC
        if (dir == SOAPY_SDR_RX) return _dev->set_rx_agc(automatic, channel);
        #endif
        return SoapySDR::Device::setGainMode(dir, channel, automatic);
    }

    void setGain(const int dir, const size_t channel, const double value)
    {
        if (dir == SOAPY_SDR_TX) _dev->set_tx_gain(value, channel);
        if (dir == SOAPY_SDR_RX) _dev->set_rx_gain(value, channel);
    }

    void setGain(const int dir, const size_t channel, const std::string &name, const double value)
    {
        if (dir == SOAPY_SDR_TX) _dev->set_tx_gain(value, name, channel);
        if (dir == SOAPY_SDR_RX) _dev->set_rx_gain(value, name, channel);
    }

    double getGain(const int dir, const size_t channel) const
    {
        if (dir == SOAPY_SDR_TX) return _dev->get_tx_gain(channel);
        if (dir == SOAPY_SDR_RX) return _dev->get_rx_gain(channel);
        return SoapySDR::Device::getGain(dir, channel);
    }

    double getGain(const int dir, const size_t channel, const std::string &name) const
    {
        if (dir == SOAPY_SDR_TX) return _dev->get_tx_gain(name, channel);
        if (dir == SOAPY_SDR_RX) return _dev->get_rx_gain(name, channel);
        return SoapySDR::Device::getGain(dir, channel, name);
    }

    SoapySDR::Range getGainRange(const int dir, const size_t channel, const std::string &name) const
    {
        if (dir == SOAPY_SDR_TX) return metaRangeToRange(_dev->get_tx_gain_range(name, channel));
        if (dir == SOAPY_SDR_RX) return metaRangeToRange(_dev->get_rx_gain_range(name, channel));
        return SoapySDR::Device::getGainRange(dir, channel, name);
    }

    SoapySDR::Range getGainRange(const int dir, const size_t channel) const
    {
        if (dir == SOAPY_SDR_TX) return metaRangeToRange(_dev->get_tx_gain_range(channel));
        if (dir == SOAPY_SDR_RX) return metaRangeToRange(_dev->get_rx_gain_range(channel));
        return SoapySDR::Device::getGainRange(dir, channel);
    }

    /*******************************************************************
     * Frequency support
     ******************************************************************/

    void setFrequency(const int dir, const size_t channel, const double frequency, const SoapySDR::Kwargs &args)
    {
        uhd::tune_request_t tr(frequency);

        if (args.count("OFFSET") != 0)
        {
            tr = uhd::tune_request_t(frequency, boost::lexical_cast<double>(args.at("OFFSET")));
        }
        if (args.count("RF") != 0)
        {
            try
            {
                tr.rf_freq = boost::lexical_cast<double>(args.at("RF"));
                tr.rf_freq_policy = uhd::tune_request_t::POLICY_MANUAL;
            }
            catch (...)
            {
                tr.rf_freq_policy = uhd::tune_request_t::POLICY_NONE;
            }
        }
        if (args.count("BB") != 0)
        {
            try
            {
                tr.dsp_freq = boost::lexical_cast<double>(args.at("BB"));
                tr.dsp_freq_policy = uhd::tune_request_t::POLICY_MANUAL;
            }
            catch (...)
            {
                tr.dsp_freq_policy = uhd::tune_request_t::POLICY_NONE;
            }
        }
        tr.args = kwargsToDict(args);

        if (dir == SOAPY_SDR_TX) _trCache[dir][channel] = _dev->set_tx_freq(tr, channel);
        if (dir == SOAPY_SDR_RX) _trCache[dir][channel] = _dev->set_rx_freq(tr, channel);
    }

    void setFrequency(const int dir, const size_t channel, const std::string &name, const double frequency, const SoapySDR::Kwargs &args)
    {
        //use tune request to get individual elements -- could use property tree here
        uhd::tune_request_t tr(frequency);
        tr.rf_freq_policy = uhd::tune_request_t::POLICY_NONE;
        tr.dsp_freq_policy = uhd::tune_request_t::POLICY_NONE;
        tr.args = kwargsToDict(args);
        if (name == "RF")
        {
            tr.rf_freq = frequency;
            tr.rf_freq_policy = uhd::tune_request_t::POLICY_MANUAL;
        }
        if (name == "BB")
        {
            tr.dsp_freq = frequency;
            tr.dsp_freq_policy = uhd::tune_request_t::POLICY_MANUAL;
        }

        if (dir == SOAPY_SDR_TX) _trCache[dir][channel] = _dev->set_tx_freq(tr, channel);
        if (dir == SOAPY_SDR_RX) _trCache[dir][channel] = _dev->set_rx_freq(tr, channel);
    }

    std::map<int, std::map<size_t, uhd::tune_result_t> > _trCache;

    double getFrequency(const int dir, const size_t channel) const
    {
        if (dir == SOAPY_SDR_TX) return _dev->get_tx_freq(channel);
        if (dir == SOAPY_SDR_RX) return _dev->get_rx_freq(channel);
        return SoapySDR::Device::getFrequency(dir, channel);
    }

    double getFrequency(const int dir, const size_t channel, const std::string &name) const
    {
        const uhd::tune_result_t tr = _trCache.at(dir).at(channel);
        if (name == "RF") return tr.actual_rf_freq;
        if (name == "BB") return tr.actual_dsp_freq;
        return SoapySDR::Device::getFrequency(dir, channel, name);
    }

    std::vector<std::string> listFrequencies(const int, const size_t) const
    {
        std::vector<std::string> comps;
        comps.push_back("RF");
        comps.push_back("BB");
        return comps;
    }

    SoapySDR::RangeList getFrequencyRange(const int dir, const size_t channel) const
    {
        if (dir == SOAPY_SDR_TX) return metaRangeToRangeList(_dev->get_tx_freq_range(channel));
        if (dir == SOAPY_SDR_RX) return metaRangeToRangeList(_dev->get_rx_freq_range(channel));
        return SoapySDR::Device::getFrequencyRange(dir, channel);
    }

    SoapySDR::RangeList getFrequencyRange(const int dir, const size_t channel, const std::string &name) const
    {
        if (name == "RF")
        {
            //use overall range - could use property tree, but close enough
            if (dir == SOAPY_SDR_TX) return metaRangeToRangeList(_dev->get_tx_freq_range(channel));
            if (dir == SOAPY_SDR_RX) return metaRangeToRangeList(_dev->get_rx_freq_range(channel));
        }
        if (name == "BB")
        {
            //read the range from the property tree
            uhd::property_tree::sptr tree = _dev->get_device()->get_tree();
            const std::string path = str(boost::format("/mboards/0/%s_dsps/%u/freq/range") % ((dir == SOAPY_SDR_TX)?"tx":"rx") % channel);
            return metaRangeToRangeList(tree->access<uhd::meta_range_t>(path).get());
        }
        return SoapySDR::Device::getFrequencyRange(dir, channel, name);
    }

    /*******************************************************************
     * Sample Rate support
     ******************************************************************/

    void setSampleRate(const int dir, const size_t channel, const double rate)
    {
        if (dir == SOAPY_SDR_TX) return _dev->set_tx_rate(rate, channel);
        if (dir == SOAPY_SDR_RX) return _dev->set_rx_rate(rate, channel);
    }

    double getSampleRate(const int dir, const size_t channel) const
    {
        if (dir == SOAPY_SDR_TX) return _dev->get_tx_rate(channel);
        if (dir == SOAPY_SDR_RX) return _dev->get_rx_rate(channel);
        return SoapySDR::Device::getSampleRate(dir, channel);
    }

    std::vector<double> listSampleRates(const int dir, const size_t channel) const
    {
        if (dir == SOAPY_SDR_TX) return metaRangeToNumericList(_dev->get_tx_rates(channel));
        if (dir == SOAPY_SDR_RX) return metaRangeToNumericList(_dev->get_rx_rates(channel));
        return SoapySDR::Device::listSampleRates(dir, channel);
    }

    void setBandwidth(const int dir, const size_t channel, const double bw)
    {
        if (dir == SOAPY_SDR_TX) return _dev->set_tx_bandwidth(bw, channel);
        if (dir == SOAPY_SDR_RX) return _dev->set_rx_bandwidth(bw, channel);
    }

    double getBandwidth(const int dir, const size_t channel) const
    {
        if (dir == SOAPY_SDR_TX) return _dev->get_tx_bandwidth(channel);
        if (dir == SOAPY_SDR_RX) return _dev->get_rx_bandwidth(channel);
        return SoapySDR::Device::getBandwidth(dir, channel);
    }

    std::vector<double> listBandwidths(const int dir, const size_t channel) const
    {
        if (dir == SOAPY_SDR_TX) return metaRangeToNumericList(_dev->get_tx_bandwidth_range(channel));
        if (dir == SOAPY_SDR_RX) return metaRangeToNumericList(_dev->get_rx_bandwidth_range(channel));
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

    bool hasHardwareTime(const std::string &what) const
    {
        return (what == "PPS" or what.empty());
    }

    long long getHardwareTime(const std::string &what) const
    {
        if (what == "PPS") return _dev->get_time_last_pps().to_ticks(1e9);
        return _dev->get_time_now().to_ticks(1e9);
    }

    void setHardwareTime(const long long timeNs, const std::string &what)
    {
        uhd::time_spec_t time = uhd::time_spec_t::from_ticks(timeNs, 1e9);
        if (what == "PPS") return _dev->set_time_next_pps(time);
        if (what == "UNKNOWN_PPS") return _dev->set_time_unknown_pps(time);
        return _dev->set_time_now(time);
    }

    void setCommandTime(const long long timeNs, const std::string &)
    {
        if (timeNs == 0) _dev->clear_command_time();
        else _dev->set_command_time(uhd::time_spec_t::from_ticks(timeNs, 1e9));
    }

    /*******************************************************************
     * Sensor support
     ******************************************************************/

    std::vector<std::string> listSensors(void) const
    {
        std::vector<std::string> names = _dev->get_mboard_sensor_names();

        for (size_t i = 0; i < this->getNumChannels(SOAPY_SDR_TX); i++)
        {
            BOOST_FOREACH(const std::string &name, _dev->get_tx_sensor_names(i))
            {
                names.push_back(str(boost::format("tx%d_%s") % i % name));
            }
        }
        for (size_t i = 0; i < this->getNumChannels(SOAPY_SDR_RX); i++)
        {
            BOOST_FOREACH(const std::string &name, _dev->get_rx_sensor_names(i))
            {
                names.push_back(str(boost::format("rx%d_%s") % i % name));
            }
        }

        return names;
    }

    std::string readSensor(const std::string &name) const
    {
        //parse the name for tx/rx formatting
        int chan = 0;
        std::string prefix;
        std::string suffix;
        bool after_underscore = false;
        BOOST_FOREACH(const char ch, name)
        {
            if (after_underscore) suffix += ch;
            else if (ch == '_') after_underscore = true;
            else if (std::isdigit(ch)) chan = (chan*10) + (chan-'0');
            else prefix += ch;
        }

        //read the sensor
        if (prefix == "tx") return _dev->get_tx_sensor(suffix, chan).value;
        if (prefix == "rx") return _dev->get_rx_sensor(suffix, chan).value;
        return _dev->get_mboard_sensor(name).value;
    }

private:
    uhd::usrp::multi_usrp::sptr _dev;
    const std::string _type;
};

/***********************************************************************
 * Register into logger
 **********************************************************************/
static void SoapyUHDLogger(uhd::msg::type_t t, const std::string &s)
{
    if (s.empty()) return;
    if (s[s.size()-1] == '\n') return SoapyUHDLogger(t, s.substr(0, s.size()-1));
    switch (t)
    {
    case uhd::msg::status: SoapySDR::log(SOAPY_SDR_INFO, s); break;
    case uhd::msg::warning: SoapySDR::log(SOAPY_SDR_WARNING, s); break;
    case uhd::msg::error: SoapySDR::log(SOAPY_SDR_ERROR, s); break;
    case uhd::msg::fastpath: SoapySDR::log(SOAPY_SDR_SSI, s); break;
    }
}

/***********************************************************************
 * Registration
 **********************************************************************/
std::vector<SoapySDR::Kwargs> find_uhd(const SoapySDR::Kwargs &args_)
{
    //prevent going into the the UHDSoapyDevice
    SoapySDR::Kwargs args(args_);
    if (args.count(SOAPY_UHD_NO_DEEPER) != 0) return std::vector<SoapySDR::Kwargs>();
    args[SOAPY_UHD_NO_DEEPER] = "";

    //perform the discovery
    #ifdef UHD_HAS_DEVICE_FILTER
    const uhd::device_addrs_t addrs = uhd::device::find(kwargsToDict(args), uhd::device::USRP);
    #else
    const uhd::device_addrs_t addrs = uhd::device::find(kwargsToDict(args));
    #endif

    //convert addrs to results
    std::vector<SoapySDR::Kwargs> results;
    for (size_t i = 0; i < addrs.size(); i++)
    {
        SoapySDR::Kwargs result(dictToKwargs(addrs[i]));
        result.erase(SOAPY_UHD_NO_DEEPER);
        results.push_back(result);
    }
    return results;
}

SoapySDR::Device *make_uhd(const SoapySDR::Kwargs &args)
{
    if(std::string(UHD_VERSION_ABI_STRING) != uhd::get_abi_string()) throw std::runtime_error(str(boost::format(
        "SoapySDR detected ABI compatibility mismatch with UHD library.\n"
        "SoapySDR UHD support was build against ABI: %s,\n"
        "but UHD library reports ABI: %s\n"
        "Suggestion: install an ABI compatible version of UHD,\n"
        "or rebuild SoapySDR UHD support against this ABI version.\n"
    ) % UHD_VERSION_ABI_STRING % uhd::get_abi_string()));
    uhd::msg::register_handler(&SoapyUHDLogger);
    return new SoapyUHDDevice(uhd::usrp::multi_usrp::make(kwargsToDict(args)), args.at("type"));
}

static SoapySDR::Registry register__uhd("uhd", &find_uhd, &make_uhd, SOAPY_SDR_ABI_VERSION);
