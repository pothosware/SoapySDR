/*
 * Copyright 2014-2015 Free Software Foundation, Inc.
 *
 * This file is part of GrOsmoSDR support modules
 *
 * GrOsmoSDR is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GrOsmoSDR is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GrOsmoSDR; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#pragma once
#include <SoapySDR/Device.hpp>
#include <boost/shared_ptr.hpp>
#include "sink_iface.h"
#include "source_iface.h"
#include "osmosdr/source.h"
#include <gnuradio/sync_block.h>
#include <boost/lexical_cast.hpp>
#include <stdexcept>

class GrOsmoSDRStreamer
{
public:
    GrOsmoSDRStreamer(boost::shared_ptr<gr::sync_block> block, const size_t numChans):
        _block(block)
    {
        //only one of these buffers gets used -- depends read vs write
        _input_items.resize(numChans);
        _output_items.resize(numChans);
    }

    void start(void)
    {
        _block->start();
    }

    void stop(void)
    {
        _block->stop();
    }

    int read(void * const *buffs, const size_t numElems)
    {
        _output_items.assign(buffs, buffs+_output_items.size());
        int ret = _block->work(numElems, _input_items, _output_items);
        if (ret == 0) return SOAPY_SDR_TIMEOUT;
        return ret;
    }

    int write(const void * const *buffs, const size_t numElems)
    {
        _block->_consumed_total = 0; //clear consumed
        _input_items.assign(buffs, buffs+_output_items.size());
        int ret = _block->work(numElems, _input_items, _output_items);
        if (ret == 0) return SOAPY_SDR_TIMEOUT;
        return (ret < 0)? ret : _block->_consumed_total;
    }

private:
    boost::shared_ptr<gr::sync_block> _block;
    gr_vector_const_void_star _input_items;
    gr_vector_void_star _output_items;
};

class GrOsmoSDRInterface : public SoapySDR::Device
{
public:
    /*******************************************************************
     * Initialization
     ******************************************************************/
    GrOsmoSDRInterface(const std::string &target):
        _target(target)
    {
        return;
    }

    template <typename SourceType>
    void installSource(boost::shared_ptr<SourceType> source)
    {
        _source = source;
        _sourceBlock = source;
    }

    template <typename SinkType>
    void installSink(boost::shared_ptr<SinkType> sink)
    {
        _sink = sink;
        _sinkBlock = sink;
    }

    /*******************************************************************
     * Identification API
     ******************************************************************/
    std::string getDriverKey(void) const
    {
        return _target;
    }

    std::string getHardwareKey(void) const
    {
        return _target;
    }

    /*******************************************************************
     * Channels support
     ******************************************************************/
    size_t getNumChannels(const int dir) const
    {
        if (dir == SOAPY_SDR_TX and _sink) return _sink->get_num_channels();
        if (dir == SOAPY_SDR_RX and _source) return _source->get_num_channels();
        return SoapySDR::Device::getNumChannels(dir);
    }

    /*******************************************************************
     * Stream support
     ******************************************************************/
    SoapySDR::Stream *setupStream(const int dir, const std::string &format, const std::vector<size_t> &, const SoapySDR::Kwargs &)
    {
        if (format != "CF32") throw std::runtime_error("GrOsmoSDRStreamer only supports format CF32");
        GrOsmoSDRStreamer *stream = new GrOsmoSDRStreamer((dir == SOAPY_SDR_TX)?_sinkBlock:_sourceBlock, this->getNumChannels(dir));
        return reinterpret_cast<SoapySDR::Stream *>(stream);
    }

    void closeStream(SoapySDR::Stream *handle)
    {
        delete reinterpret_cast<GrOsmoSDRStreamer *>(handle);
    }

    int activateStream(SoapySDR::Stream *stream, const int flags, const long long, const size_t)
    {
        if (flags != 0) return SOAPY_SDR_NOT_SUPPORTED;
        reinterpret_cast<GrOsmoSDRStreamer *>(stream)->start();
        return 0;
    }

    int deactivateStream(SoapySDR::Stream *stream, const int flags, const long long)
    {
        if (flags != 0) return SOAPY_SDR_NOT_SUPPORTED;
        reinterpret_cast<GrOsmoSDRStreamer *>(stream)->stop();
        return 0;
    }

    int readStream(SoapySDR::Stream *handle, void * const *buffs, const size_t numElems, int &flags, long long &, const long)
    {
        GrOsmoSDRStreamer *streamer = reinterpret_cast<GrOsmoSDRStreamer *>(handle);
        flags = 0;
        return streamer->read(buffs, numElems);
    }

    int writeStream(SoapySDR::Stream *handle, const void * const *buffs, const size_t numElems, int &flags, const long long, const long)
    {
        GrOsmoSDRStreamer *streamer = reinterpret_cast<GrOsmoSDRStreamer *>(handle);
        flags = 0;
        return streamer->write(buffs, numElems);
    }

    /*******************************************************************
     * Antenna support
     ******************************************************************/

    std::vector<std::string> listAntennas(const int dir, const size_t channel) const
    {
        if (dir == SOAPY_SDR_TX and _sink) return _sink->get_antennas(channel);
        if (dir == SOAPY_SDR_RX and _source) return _source->get_antennas(channel);
        return SoapySDR::Device::listAntennas(dir, channel);
    }

    void setAntenna(const int dir, const size_t channel, const std::string &name)
    {
        if (dir == SOAPY_SDR_TX and _sink) _sink->set_antenna(name, channel);
        if (dir == SOAPY_SDR_RX and _source) _source->set_antenna(name, channel);
    }

    std::string getAntenna(const int dir, const size_t channel) const
    {
        if (dir == SOAPY_SDR_TX and _sink) return _sink->get_antenna(channel);
        if (dir == SOAPY_SDR_RX and _source) return _source->get_antenna(channel);
        return SoapySDR::Device::getAntenna(dir, channel);
    }

    /*******************************************************************
     * Frontend corrections support
     ******************************************************************/

    void setDCOffsetMode(const int dir, const size_t channel, const bool automatic)
    {
        if (dir == SOAPY_SDR_RX and _source) return _source->set_dc_offset_mode(
            automatic?osmosdr::source::DCOffsetAutomatic:osmosdr::source::DCOffsetManual, channel);
        return SoapySDR::Device::setDCOffsetMode(dir, channel, automatic);
    }

    void setDCOffset(const int dir, const size_t channel, const std::complex<double> &offset)
    {
        if (dir == SOAPY_SDR_TX and _sink) _sink->set_dc_offset(offset, channel);
        if (dir == SOAPY_SDR_RX and _source) _source->set_dc_offset(offset, channel);
    }

    void setIQBalance(const int dir, const size_t channel, const std::complex<double> &balance)
    {
        if (dir == SOAPY_SDR_TX and _sink) _sink->set_iq_balance(balance, channel);
        if (dir == SOAPY_SDR_RX and _source) _source->set_iq_balance(balance, channel);
    }

    /*******************************************************************
     * Gain support
     ******************************************************************/

    std::vector<std::string> listGains(const int dir, const size_t channel) const
    {
        if (dir == SOAPY_SDR_TX and _sink) return _sink->get_gain_names(channel);
        if (dir == SOAPY_SDR_RX and _source) return _source->get_gain_names(channel);
        return SoapySDR::Device::listGains(dir, channel);
    }

    void setGainMode(const int dir, const size_t channel, const bool mode)
    {
        if (dir == SOAPY_SDR_TX and _sink) _sink->set_gain_mode(mode, channel);
        if (dir == SOAPY_SDR_RX and _source) _source->set_gain_mode(mode, channel);
    }

    bool getGainMode(const int dir, const size_t channel) const
    {
        if (dir == SOAPY_SDR_TX and _sink) return _sink->get_gain_mode(channel);
        if (dir == SOAPY_SDR_RX and _source) return _source->get_gain_mode(channel);
        return SoapySDR::Device::getGainMode(dir, channel);
    }

    void setGain(const int dir, const size_t channel, const double value)
    {
        if (dir == SOAPY_SDR_TX and _sink) _sink->set_gain(value, channel);
        if (dir == SOAPY_SDR_RX and _source) _source->set_gain(value, channel);
    }

    void setGain(const int dir, const size_t channel, const std::string &name, const double value)
    {
        if (dir == SOAPY_SDR_TX and _sink) _sink->set_gain(value, name, channel);
        if (dir == SOAPY_SDR_RX and _source) _source->set_gain(value, name, channel);
    }

    double getGain(const int dir, const size_t channel) const
    {
        if (dir == SOAPY_SDR_TX and _sink) return _sink->get_gain(channel);
        if (dir == SOAPY_SDR_RX and _source) return _source->get_gain(channel);
        return SoapySDR::Device::getGain(dir, channel);
    }

    double getGain(const int dir, const size_t channel, const std::string &name) const
    {
        if (dir == SOAPY_SDR_TX and _sink) return _sink->get_gain(name, channel);
        if (dir == SOAPY_SDR_RX and _source) return _source->get_gain(name, channel);
        return SoapySDR::Device::getGain(dir, channel, name);
    }

    SoapySDR::Range getGainRange(const int dir, const size_t channel, const std::string &name) const
    {
        if (dir == SOAPY_SDR_TX and _sink) return this->toRange(_sink->get_gain_range(name, channel));
        if (dir == SOAPY_SDR_RX and _source) return this->toRange(_source->get_gain_range(name, channel));
        return SoapySDR::Device::getGainRange(dir, channel, name);
    }

    SoapySDR::Range getGainRange(const int dir, const size_t channel) const
    {
        if (dir == SOAPY_SDR_TX and _sink) return this->toRange(_sink->get_gain_range(channel));
        if (dir == SOAPY_SDR_RX and _source) return this->toRange(_source->get_gain_range(channel));
        return SoapySDR::Device::getGainRange(dir, channel);
    }

    /*******************************************************************
     * Frequency support
     ******************************************************************/

    void setFrequency(const int dir, const size_t channel, const double frequency, const SoapySDR::Kwargs &args)
    {
        this->setFrequency(dir, channel, "RF", frequency, args);
        if (args.count("CORR") != 0)
        {
            const double ppm = boost::lexical_cast<double>(args.at("CORR"));
            this->setFrequency(dir, channel, "CORR", ppm, args);
        }
    }

    void setFrequency(const int dir, const size_t channel, const std::string &name, const double frequency, const SoapySDR::Kwargs &)
    {
        if (name == "RF")
        {
            if (dir == SOAPY_SDR_TX and _sink) _sink->set_center_freq(frequency, channel);
            if (dir == SOAPY_SDR_RX and _source) _source->set_center_freq(frequency, channel);
        }
        if (name == "CORR")
        {
            if (dir == SOAPY_SDR_TX and _sink) _sink->set_freq_corr(frequency, channel);
            if (dir == SOAPY_SDR_RX and _source) _source->set_freq_corr(frequency, channel);
        }
    }

    double getFrequency(const int dir, const size_t channel) const
    {
        return this->getFrequency(dir, channel, "RF");
    }

    double getFrequency(const int dir, const size_t channel, const std::string &name) const
    {
        if (name == "RF")
        {
            if (dir == SOAPY_SDR_TX and _sink) return _sink->get_center_freq(channel);
            if (dir == SOAPY_SDR_RX and _source) return _source->get_center_freq(channel);
        }
        if (name == "CORR")
        {
            if (dir == SOAPY_SDR_TX and _sink) return _sink->get_freq_corr(channel);
            if (dir == SOAPY_SDR_RX and _source) return _source->get_freq_corr(channel);
        }
        return SoapySDR::Device::getFrequency(dir, channel, name);
    }

    SoapySDR::RangeList getFrequencyRange(const int dir, const size_t channel) const
    {
        return this->getFrequencyRange(dir, channel, "RF");
    }

    SoapySDR::RangeList getFrequencyRange(const int dir, const size_t channel, const std::string &name) const
    {
        if (name == "RF")
        {
            if (dir == SOAPY_SDR_TX and _sink) return this->toRangeList(_sink->get_freq_range(channel));
            if (dir == SOAPY_SDR_RX and _source) return this->toRangeList(_source->get_freq_range(channel));
        }
        return SoapySDR::Device::getFrequencyRange(dir, channel, name);
    }

    std::vector<std::string> listFrequencies(const int, const size_t) const
    {
        std::vector<std::string> elems;
        elems.push_back("RF");
        elems.push_back("CORR");
        return elems;
    }

    /*******************************************************************
     * Sample Rate support
     ******************************************************************/

    void setSampleRate(const int dir, const size_t, const double rate)
    {
        if (dir == SOAPY_SDR_TX and _sink) _sink->set_sample_rate(rate);
        if (dir == SOAPY_SDR_RX and _source) _source->set_sample_rate(rate);
    }

    double getSampleRate(const int dir, const size_t channel) const
    {
        if (dir == SOAPY_SDR_TX and _sink) return _sink->get_sample_rate();
        if (dir == SOAPY_SDR_RX and _source) return _source->get_sample_rate();
        return SoapySDR::Device::getSampleRate(dir, channel);
    }

    std::vector<double> listSampleRates(const int dir, const size_t channel) const
    {
        if (dir == SOAPY_SDR_TX and _sink) return this->toNumericList(_sink->get_sample_rates());
        if (dir == SOAPY_SDR_RX and _source) return this->toNumericList(_source->get_sample_rates());
        return SoapySDR::Device::listSampleRates(dir, channel);
    }

    void setBandwidth(const int dir, const size_t channel, const double bw)
    {
        if (dir == SOAPY_SDR_TX and _sink) _sink->set_bandwidth(bw, channel);
        if (dir == SOAPY_SDR_RX and _source) _source->set_bandwidth(bw, channel);
    }

    double getBandwidth(const int dir, const size_t channel) const
    {
        if (dir == SOAPY_SDR_TX and _sink) return _sink->get_bandwidth(channel);
        if (dir == SOAPY_SDR_RX and _source) return _source->get_bandwidth(channel);
        return SoapySDR::Device::getBandwidth(dir, channel);
    }

    std::vector<double> listBandwidths(const int dir, const size_t channel) const
    {
        if (dir == SOAPY_SDR_TX and _sink) return this->toNumericList(_sink->get_bandwidth_range(channel));
        if (dir == SOAPY_SDR_RX and _source) return this->toNumericList(_source->get_bandwidth_range(channel));
        return SoapySDR::Device::listBandwidths(dir, channel);
    }

    /*******************************************************************
     * Clocking support
     ******************************************************************/

    void setMasterClockRate(const double rate)
    {
        _source->set_clock_rate(rate);
    }

    double getMasterClockRate(void) const
    {
        return _source->get_clock_rate();
    }

    std::vector<std::string> listClockSources(void) const
    {
        return _source->get_clock_sources(0);
    }

    void setClockSource(const std::string &source)
    {
        _source->set_clock_source(source, 0);
    }

    std::string getClockSource(void) const
    {
        return _source->get_clock_source(0);
    }

    std::vector<std::string> listTimeSources(void) const
    {
        return _source->get_time_sources(0);
    }

    void setTimeSource(const std::string &source)
    {
        _source->set_time_source(source, 0);
    }

    std::string getTimeSource(void) const
    {
        return _source->get_time_source(0);
    }

    /*******************************************************************
     * Time support
     ******************************************************************/

    bool hasHardwareTime(const std::string &what) const
    {
        return false;//(what == "PPS" or what.empty());
    }

    long long getHardwareTime(const std::string &what) const
    {
        if (what == "PPS") return _source->get_time_last_pps().to_ticks(1e9);
        return _source->get_time_now().to_ticks(1e9);
    }

    void setHardwareTime(const long long timeNs, const std::string &what)
    {
        osmosdr::time_spec_t time = osmosdr::time_spec_t::from_ticks(timeNs, 1e9);
        if (what == "PPS") return _source->set_time_next_pps(time);
        if (what == "UNKNOWN_PPS") return _source->set_time_unknown_pps(time);
        return _source->set_time_now(time);
    }

private:

    template <typename RangeType>
    SoapySDR::RangeList toRangeList(const RangeType &ranges) const
    {
        SoapySDR::RangeList out;
        for (size_t i = 0; i < ranges.size(); i++)
        {
            out.push_back(SoapySDR::Range(ranges[i].start(), ranges[i].stop()));
        }
        return out;
    }

    template <typename RangeType>
    SoapySDR::Range toRange(const RangeType &ranges) const
    {
        return SoapySDR::Range(ranges.start(), ranges.stop());
    }

    template <typename RangeType>
    std::vector<double> toNumericList(const RangeType &ranges) const
    {
        std::vector<double> out;
        for (size_t i = 0; i < ranges.size(); i++)
        {
            //in these cases start == stop
            out.push_back(ranges[i].start());
        }
        return out;
    }

    const std::string _target;

    boost::shared_ptr<source_iface> _source;
    boost::shared_ptr<sink_iface> _sink;

    boost::shared_ptr<gr::sync_block> _sourceBlock;
    boost::shared_ptr<gr::sync_block> _sinkBlock;
};
