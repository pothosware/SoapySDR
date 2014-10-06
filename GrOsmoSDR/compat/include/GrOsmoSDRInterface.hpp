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
        return 0;
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

private:
    boost::shared_ptr<source_iface> _source;
    boost::shared_ptr<sink_iface> _sink;

    boost::shared_ptr<gr::sync_block> _sourceBlock;
    boost::shared_ptr<gr::sync_block> _sinkBlock;
};
