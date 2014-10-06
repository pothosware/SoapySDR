/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#pragma once
#include <SoapySDR/Device.hpp>
#include <boost/shared_ptr.hpp>
#include "sink_iface.h"
#include "source_iface.h"
#include <gnuradio/sync_block.h>

class GrOsmoSDRStreamer
{
public:
    GrOsmoSDRStreamer(boost::shared_ptr<gr::sync_block> block, const SoapySDR::Direction dir, const std::vector<size_t> &channels):
        _block(block)
    {
        if (dir == SoapySDR::TX) _input_items.resize(channels.size());
        if (dir == SoapySDR::RX) _output_items.resize(channels.size());
        _block->start();
    }

    ~GrOsmoSDRStreamer(void)
    {
        _block->stop();
    }

    int read(void * const *buffs, const size_t numElems)
    {
        _output_items.assign(buffs, buffs+_output_items.size());
        return _block->work(numElems, _input_items, _output_items);
    }

    int write(const void * const *buffs, const size_t numElems)
    {
        _block->_consumed_total = 0; //clear consumed
        _input_items.assign(buffs, buffs+_output_items.size());
        int ret = _block->work(numElems, _input_items, _output_items);
        return (ret <= 0)? ret : _block->_consumed_total;
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
    GrOsmoSDRInterface(void)
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
     * Channels support
     ******************************************************************/
    size_t getNumChannels(const SoapySDR::Direction dir) const
    {
        if (dir == SoapySDR::TX and _sink) return _sink->get_num_channels();
        if (dir == SoapySDR::RX and _source) return _source->get_num_channels();
        return SoapySDR::Device::getNumChannels(dir);
    }

    /*******************************************************************
     * Stream support
     ******************************************************************/
    void *setupStream(const SoapySDR::Direction dir, const std::vector<size_t> &channels, const SoapySDR::Kwargs &)
    {
        return new GrOsmoSDRStreamer((dir == SoapySDR::TX)?_sinkBlock:_sourceBlock, dir, channels);
    }

    void closeStream(void *handle)
    {
        delete reinterpret_cast<GrOsmoSDRStreamer *>(handle);
    }

    int readStream(void *handle, void * const *buffs, const size_t numElems, int &flags, long long &, const long)
    {
        GrOsmoSDRStreamer *streamer = reinterpret_cast<GrOsmoSDRStreamer *>(handle);
        flags = 0;
        return streamer->read(buffs, numElems);
    }

    int writeStream(void *handle, const void * const *buffs, const size_t numElems, int &flags, const long long, const long)
    {
        GrOsmoSDRStreamer *streamer = reinterpret_cast<GrOsmoSDRStreamer *>(handle);
        flags = 0;
        return streamer->write(buffs, numElems);
    }

    /*******************************************************************
     * Antenna support
     ******************************************************************/

    std::vector<std::string> listAntennas(const SoapySDR::Direction dir, const size_t channel) const
    {
        if (dir == SoapySDR::TX and _sink) return _sink->get_antennas(channel);
        if (dir == SoapySDR::RX and _source) return _source->get_antennas(channel);
        return SoapySDR::Device::listAntennas(dir, channel);
    }

    void setAntenna(const SoapySDR::Direction dir, const size_t channel, const std::string &name)
    {
        if (dir == SoapySDR::TX and _sink) _sink->set_antenna(name, channel);
        if (dir == SoapySDR::RX and _source) _source->set_antenna(name, channel);
    }

    std::string getAntenna(const SoapySDR::Direction dir, const size_t channel) const
    {
        if (dir == SoapySDR::TX and _sink) return _sink->get_antenna(channel);
        if (dir == SoapySDR::RX and _source) return _source->get_antenna(channel);
        return SoapySDR::Device::getAntenna(dir, channel);
    }

    /*******************************************************************
     * Frontend corrections support
     ******************************************************************/

    void setDCOffset(const SoapySDR::Direction dir, const size_t channel, const std::complex<double> &offset)
    {
        if (dir == SoapySDR::TX and _sink) _sink->set_dc_offset(offset, channel);
        if (dir == SoapySDR::RX and _source) _source->set_dc_offset(offset, channel);
    }

    void setIQBalance(const SoapySDR::Direction dir, const size_t channel, const std::complex<double> &balance)
    {
        if (dir == SoapySDR::TX and _sink) _sink->set_iq_balance(balance, channel);
        if (dir == SoapySDR::RX and _source) _source->set_iq_balance(balance, channel);
    }

    /*******************************************************************
     * Gain support
     ******************************************************************/

    std::vector<std::string> listGains(const SoapySDR::Direction dir, const size_t channel) const
    {
        if (dir == SoapySDR::TX and _sink) return _sink->get_gain_names(channel);
        if (dir == SoapySDR::RX and _source) return _source->get_gain_names(channel);
        return SoapySDR::Device::listGains(dir, channel);
    }

    void setGainMode(const SoapySDR::Direction dir, const size_t channel, const bool mode)
    {
        if (dir == SoapySDR::TX and _sink) _sink->set_gain_mode(mode, channel);
        if (dir == SoapySDR::RX and _source) _source->set_gain_mode(mode, channel);
    }

    bool getGainMode(const SoapySDR::Direction dir, const size_t channel) const
    {
        if (dir == SoapySDR::TX and _sink) return _sink->get_gain_mode(channel);
        if (dir == SoapySDR::RX and _source) return _source->get_gain_mode(channel);
        return SoapySDR::Device::getGainMode(dir, channel);
    }

    void setGain(const SoapySDR::Direction dir, const size_t channel, const double value)
    {
        if (dir == SoapySDR::TX and _sink) _sink->set_gain(value, channel);
        if (dir == SoapySDR::RX and _source) _source->set_gain(value, channel);
    }

    void setGain(const SoapySDR::Direction dir, const size_t channel, const std::string &name, const double value)
    {
        if (dir == SoapySDR::TX and _sink) _sink->set_gain(value, name, channel);
        if (dir == SoapySDR::RX and _source) _source->set_gain(value, name, channel);
    }

    double getGainValue(const SoapySDR::Direction dir, const size_t channel, const std::string &name) const
    {
        if (dir == SoapySDR::TX and _sink) return _sink->get_gain(name, channel);
        if (dir == SoapySDR::RX and _source) return _source->get_gain(name, channel);
        return SoapySDR::Device::getGainValue(dir, channel, name);
    }

    SoapySDR::RangeList getGainRange(const SoapySDR::Direction dir, const size_t channel, const std::string &name) const
    {
        if (dir == SoapySDR::TX and _sink) return this->toRangeList(_sink->get_gain_range(name, channel));
        if (dir == SoapySDR::RX and _source) return this->toRangeList(_source->get_gain_range(name, channel));
        return SoapySDR::Device::getGainRange(dir, channel, name);
    }

    SoapySDR::RangeList getGainRange(const SoapySDR::Direction dir, const size_t channel) const
    {
        if (dir == SoapySDR::TX and _sink) return this->toRangeList(_sink->get_gain_range(channel));
        if (dir == SoapySDR::RX and _source) return this->toRangeList(_source->get_gain_range(channel));
        return SoapySDR::Device::getGainRange(dir, channel);
    }

    /*******************************************************************
     * Frequency support
     ******************************************************************/

    void setFrequency(const SoapySDR::Direction dir, const size_t channel, const double frequency, const SoapySDR::Kwargs &)
    {
        if (dir == SoapySDR::TX and _sink) _sink->set_center_freq(frequency, channel);
        if (dir == SoapySDR::RX and _source) _source->set_center_freq(frequency, channel);
    }

    double getFrequency(const SoapySDR::Direction dir, const size_t channel) const
    {
        if (dir == SoapySDR::TX and _sink) return _sink->get_center_freq(channel);
        if (dir == SoapySDR::RX and _source) return _source->get_center_freq(channel);
        return SoapySDR::Device::getFrequency(dir, channel);
    }

    SoapySDR::RangeList getFrequencyRange(const SoapySDR::Direction dir, const size_t channel) const
    {
        if (dir == SoapySDR::TX and _sink) return this->toRangeList(_sink->get_freq_range(channel));
        if (dir == SoapySDR::RX and _source) return this->toRangeList(_source->get_freq_range(channel));
        return SoapySDR::Device::getFrequencyRange(dir, channel);
    }

    /*******************************************************************
     * Sample Rate support
     ******************************************************************/

    void setSampleRate(const SoapySDR::Direction dir, const size_t, const double rate)
    {
        if (dir == SoapySDR::TX and _sink) _sink->set_sample_rate(rate);
        if (dir == SoapySDR::RX and _source) _source->set_sample_rate(rate);
    }

    double getSampleRate(const SoapySDR::Direction dir, const size_t channel) const
    {
        if (dir == SoapySDR::TX and _sink) return _sink->get_sample_rate();
        if (dir == SoapySDR::RX and _source) return _source->get_sample_rate();
        return SoapySDR::Device::getSampleRate(dir, channel);
    }

    SoapySDR::RangeList getSampleRateRange(const SoapySDR::Direction dir, const size_t channel) const
    {
        if (dir == SoapySDR::TX and _sink) return this->toRangeList(_sink->get_sample_rates());
        if (dir == SoapySDR::RX and _source) return this->toRangeList(_source->get_sample_rates());
        return SoapySDR::Device::getSampleRateRange(dir, channel);
    }

    void setBandwidth(const SoapySDR::Direction dir, const size_t channel, const double bw)
    {
        if (dir == SoapySDR::TX and _sink) _sink->set_bandwidth(bw, channel);
        if (dir == SoapySDR::RX and _source) _source->set_bandwidth(bw, channel);
    }

    double getBandwidth(const SoapySDR::Direction dir, const size_t channel) const
    {
        if (dir == SoapySDR::TX and _sink) return _sink->get_bandwidth(channel);
        if (dir == SoapySDR::RX and _source) return _source->get_bandwidth(channel);
        return SoapySDR::Device::getBandwidth(dir, channel);
    }

    SoapySDR::RangeList getBandwidthRange(const SoapySDR::Direction dir, const size_t channel) const
    {
        if (dir == SoapySDR::TX and _sink) return this->toRangeList(_sink->get_bandwidth_range(channel));
        if (dir == SoapySDR::RX and _source) return this->toRangeList(_source->get_bandwidth_range(channel));
        return SoapySDR::Device::getBandwidthRange(dir, channel);
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

    boost::shared_ptr<source_iface> _source;
    boost::shared_ptr<sink_iface> _sink;

    boost::shared_ptr<gr::sync_block> _sourceBlock;
    boost::shared_ptr<gr::sync_block> _sinkBlock;
};
