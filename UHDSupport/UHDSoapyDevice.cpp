// Copyright (c) 2015-2015 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include "TypeHelpers.hpp"
#include <uhd/utils/static.hpp>
#include <uhd/property_tree.hpp>
#include <uhd/device.hpp>
#include <uhd/convert.hpp>
#include <uhd/utils/msg.hpp>
#include <uhd/types/sensors.hpp>
#include <uhd/types/ranges.hpp>
#include <uhd/usrp/mboard_eeprom.hpp>
#include <uhd/usrp/dboard_eeprom.hpp>
#include <uhd/usrp/subdev_spec.hpp>

#include <SoapySDR/Device.hpp>
#include <SoapySDR/Logger.hpp>

#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/algorithm/string.hpp>

/***********************************************************************
 * Custom UHD Device to support Soapy
 **********************************************************************/
class UHDSoapyDevice : public uhd::device
{
public:
    UHDSoapyDevice(const uhd::device_addr_t &args);

    ~UHDSoapyDevice(void);

    uhd::rx_streamer::sptr get_rx_stream(const uhd::stream_args_t &args);
    uhd::tx_streamer::sptr get_tx_stream(const uhd::stream_args_t &args);
    bool recv_async_msg(uhd::async_metadata_t &, double);

    void set_cmd_time(const uhd::time_spec_t &time)
    {
        _device->setCommandTime(time.to_ticks(1e9));
    }

    uhd::time_spec_t get_hardware_time(const std::string &what)
    {
        return uhd::time_spec_t::from_ticks(_device->getHardwareTime(what), 1e9);
    }

    void set_hardware_time(const std::string &what, const uhd::time_spec_t &time)
    {
        _device->setHardwareTime(time.to_ticks(1e9), what);
    }

    uhd::usrp::subdev_spec_t get_frontend_mapping(const int dir)
    {
        //return uhd::usrp::subdev_spec_t(_device->getFrontendMapping(dir));
        uhd::usrp::subdev_spec_t spec;
        for (size_t ch = 0; ch < _device->getNumChannels(dir); ch++)
        {
            const std::string chName(boost::lexical_cast<std::string>(ch));
            spec.push_back(uhd::usrp::subdev_spec_pair_t(chName, chName));
        }
        return spec;
    }

    void set_frontend_mapping(const int, const uhd::usrp::subdev_spec_t &)
    {
        //there is no translation from spec to frontend map
        //however, frontend map can be set by device args
        //_device->setFrontendMapping(dir, spec.to_string());
    }

    uhd::meta_range_t get_freq_range(const int dir, const size_t chan, const std::string &name)
    {
        return rangeListToMetaRange(_device->getFrequencyRange(dir, chan, name));
    }

    void stash_tune_args(const int dir, const size_t chan, const uhd::device_addr_t &args)
    {
        _tuneArgsStash[dir][chan] = dictToKwargs(args);
    }

    std::map<int, std::map<size_t, SoapySDR::Kwargs> > _tuneArgsStash;

    void set_frequency(const int dir, const size_t chan, const std::string &name, const double freq)
    {
        _device->setFrequency(dir, chan, name, freq, _tuneArgsStash[dir][chan]);
    }

    uhd::meta_range_t get_bw_range(const int dir, const size_t chan)
    {
        return numberListToMetaRange(_device->listBandwidths(dir, chan));
    }

    uhd::meta_range_t get_rate_range(const int dir, const size_t chan)
    {
        return numberListToMetaRange(_device->listSampleRates(dir, chan));
    }

    uhd::meta_range_t get_gain_range(const int dir, const size_t chan, const std::string &name)
    {
        return rangeToMetaRange(_device->getGainRange(dir, chan, name));
    }

    uhd::sensor_value_t get_mboard_sensor(const std::string &name)
    {
        const std::string &value = _device->readSensor(name);
        if (value == "true") return uhd::sensor_value_t(name, true, "true", "false");
        if (value == "false") return uhd::sensor_value_t(name, true, "true", "false");
        try
        {
            const double valueNum = boost::lexical_cast<double>(value);
            return uhd::sensor_value_t(name, valueNum, "");
        }
        catch (const boost::bad_lexical_cast &){}
        return uhd::sensor_value_t(name, value, "");
    }

    void old_issue_stream_cmd(const size_t chan, const uhd::stream_cmd_t &cmd)
    {
        uhd::rx_streamer::sptr stream = _rx_streamers[chan].lock();
        if (stream) stream->issue_stream_cmd(cmd);
    }

    void setupChannelHooks(const int dir);
    void setupChannelHooks(const int dir, const size_t chan, const std::string &dirName, const std::string &chName);

private:
    SoapySDR::Device *_device;

    //stash streamers to implement old-style issue stream cmd and async message
    std::map<size_t, boost::weak_ptr<uhd::rx_streamer> > _rx_streamers;
    std::map<size_t, boost::weak_ptr<uhd::tx_streamer> > _tx_streamers;
};

/***********************************************************************
 * Factory and initialization
 **********************************************************************/
static boost::mutex &suMutexMaker(void)
{
    static boost::mutex m;
    return m;
}

UHDSoapyDevice::UHDSoapyDevice(const uhd::device_addr_t &args)
{
    {
        boost::mutex::scoped_lock l(suMutexMaker());
        _device = SoapySDR::Device::make(dictToKwargs(args));
    }

    //optional frontend map args
    if (args.has_key("rx_map")) _device->setFrontendMapping(SOAPY_SDR_RX, args.get("rx_map"));
    if (args.has_key("tx_map")) _device->setFrontendMapping(SOAPY_SDR_TX, args.get("tx_map"));

    //setup property tree
    _tree = uhd::property_tree::make();
    const uhd::fs_path mb_path = "/mboards/0";
    _tree->create<std::string>("/name").set(_device->getDriverKey());
    _tree->create<std::string>(mb_path / "name").set(_device->getHardwareKey());

    //mb eeprom filled with hardware info
    uhd::usrp::mboard_eeprom_t mb_eeprom;
    const uhd::device_addr_t hardware_info(kwargsToDict(_device->getHardwareInfo()));
    BOOST_FOREACH(const std::string &key, hardware_info.keys()) mb_eeprom[key] = hardware_info[key];
    _tree->create<uhd::usrp::mboard_eeprom_t>(mb_path / "eeprom").set(mb_eeprom);

    //the frontend mapping
    _tree->create<uhd::usrp::subdev_spec_t>(mb_path / "rx_subdev_spec")
        .publish(boost::bind(&UHDSoapyDevice::get_frontend_mapping, this, SOAPY_SDR_RX))
        .subscribe(boost::bind(&UHDSoapyDevice::set_frontend_mapping, this, SOAPY_SDR_RX, _1));
    _tree->create<uhd::usrp::subdev_spec_t>(mb_path / "tx_subdev_spec")
        .publish(boost::bind(&UHDSoapyDevice::get_frontend_mapping, this, SOAPY_SDR_TX))
        .subscribe(boost::bind(&UHDSoapyDevice::set_frontend_mapping, this, SOAPY_SDR_TX, _1));

    //timed command support
    _tree->create<uhd::time_spec_t>(mb_path / "time" / "cmd")
        .subscribe(boost::bind(&UHDSoapyDevice::set_cmd_time, this, _1));
    _tree->create<double>(mb_path / "tick_rate")
        .publish(boost::bind(&SoapySDR::Device::getMasterClockRate, _device))
        .subscribe(boost::bind(&SoapySDR::Device::setMasterClockRate, _device, _1));

    //hardware time support
    _tree->create<uhd::time_spec_t>(mb_path / "time" / "now")
        .publish(boost::bind(&UHDSoapyDevice::get_hardware_time, this, ""))
        .subscribe(boost::bind(&UHDSoapyDevice::set_hardware_time, this, "", _1));
    _tree->create<uhd::time_spec_t>(mb_path / "time" / "pps")
        .publish(boost::bind(&UHDSoapyDevice::get_hardware_time, this, "PPS"))
        .subscribe(boost::bind(&UHDSoapyDevice::set_hardware_time, this, "PPS", _1));

    //clock and time sources
    _tree->create<std::vector<std::string> >(mb_path / "clock_source"/ "options")
        .publish(boost::bind(&SoapySDR::Device::listClockSources, _device));
    _tree->create<std::string>(mb_path / "clock_source" / "value")
        .publish(boost::bind(&SoapySDR::Device::getClockSource, _device))
        .subscribe(boost::bind(&SoapySDR::Device::setClockSource, _device, _1));
    _tree->create<std::vector<std::string> >(mb_path / "time_source"/ "options")
        .publish(boost::bind(&SoapySDR::Device::listTimeSources, _device));
    _tree->create<std::string>(mb_path / "time_source" / "value")
        .publish(boost::bind(&SoapySDR::Device::getTimeSource, _device))
        .subscribe(boost::bind(&SoapySDR::Device::setTimeSource, _device, _1));

    //mboard sensors
    _tree->create<int>(mb_path / "sensors"); //ensure this path exists
    BOOST_FOREACH(const std::string &name, _device->listSensors())
    {
        std::string nameLower(name); boost::algorithm::to_lower(nameLower);
        if (nameLower.substr(0, 2) == "rx") continue; //install in frontend sensors
        if (nameLower.substr(0, 2) == "tx") continue; //install in frontend sensors
        _tree->create<uhd::sensor_value_t>(mb_path / "sensors" / name)
            .publish(boost::bind(&UHDSoapyDevice::get_mboard_sensor, this, name));
    }

    //setup channel and frontend hooks
    this->setupChannelHooks(SOAPY_SDR_RX);
    this->setupChannelHooks(SOAPY_SDR_TX);
}

UHDSoapyDevice::~UHDSoapyDevice(void)
{
    boost::mutex::scoped_lock l(suMutexMaker());
    SoapySDR::Device::unmake(_device);
}

void UHDSoapyDevice::setupChannelHooks(const int dir)
{
    for (size_t ch = 0; ch < _device->getNumChannels(dir); ch++)
    {
        const std::string dirName((dir==SOAPY_SDR_RX)?"rx":"tx");
        const std::string chName(boost::lexical_cast<std::string>(ch));
        this->setupChannelHooks(dir, ch, dirName, chName);
    }
}

void UHDSoapyDevice::setupChannelHooks(const int dir, const size_t chan, const std::string &dirName, const std::string &chName)
{
    const uhd::fs_path mb_path = "/mboards/0";
    const uhd::fs_path rf_fe_path = mb_path / "dboards" / chName / (dirName+"_frontends") / chName;
    const uhd::fs_path dsp_path = mb_path / (dirName+"_dsps") / chName;
    const uhd::fs_path codec_path = mb_path / (dirName+"_codecs") / chName;

    _tree->create<std::string>(codec_path / "name").set("Soapy"+std::string((dir==SOAPY_SDR_RX)?"ADC":"DAC"));
    _tree->create<int>(codec_path / "gains"); //empty, gains in frontend
    _tree->create<std::string>(rf_fe_path / "name").set("SoapyRF");
    _tree->create<std::string>(rf_fe_path / "connection").set("IQ");

    //names of the tunable components
    const std::vector<std::string> comps = _device->listFrequencies(dir, chan);
    const std::string rfCompName = (comps.size()>0)?comps.at(0):"RF";
    const std::string bbCompName = (comps.size()>1)?comps.at(1):"BB";

    //samp rate
    _tree->create<uhd::meta_range_t>(dsp_path / "rate" / "range")
        .publish(boost::bind(&UHDSoapyDevice::get_rate_range, this, dir, chan));
    _tree->create<double>(dsp_path / "rate" / "value")
        .publish(boost::bind(&SoapySDR::Device::getSampleRate, _device, dir, chan))
        .subscribe(boost::bind(&SoapySDR::Device::setSampleRate, _device, dir, chan, _1));

    //dsp freq
    _tree->create<double>(dsp_path / "freq" / "value")
        .publish(boost::bind(&SoapySDR::Device::getFrequency, _device, dir, chan, bbCompName))
        .subscribe(boost::bind(&UHDSoapyDevice::set_frequency, this, dir, chan, bbCompName, _1));
    _tree->create<uhd::meta_range_t>(dsp_path / "freq" / "range")
        .publish(boost::bind(&UHDSoapyDevice::get_freq_range, this, dir, chan, bbCompName));

    //old style stream cmd
    if (dir == SOAPY_SDR_RX)
    {
        _tree->create<uhd::stream_cmd_t>(dsp_path / "stream_cmd")
            .subscribe(boost::bind(&UHDSoapyDevice::old_issue_stream_cmd, this, chan, _1));
    }

    //frontend sensors
    _tree->create<int>(rf_fe_path / "sensors"); //ensure this path exists
    BOOST_FOREACH(const std::string &name, _device->listSensors())
    {
        //parse the name for tx/rx formatting
        int sensorChan = 0;
        std::string prefix;
        std::string suffix;
        bool after_underscore = false;
        BOOST_FOREACH(const char ch, name)
        {
            if (after_underscore) suffix += ch;
            else if (ch == '_') after_underscore = true;
            else if (std::isdigit(ch)) sensorChan = (sensorChan*10) + (sensorChan-'0');
            else prefix += ch;
        }

        //check for channel and direction match
        boost::algorithm::to_lower(prefix);
        if (prefix != dirName) continue;
        if (sensorChan != int(chan)) continue;

        //install the sensor
        _tree->create<uhd::sensor_value_t>(mb_path / "sensors" / suffix)
            .publish(boost::bind(&UHDSoapyDevice::get_mboard_sensor, this, name));
    }

    //dummy eeprom values
    if (dir == SOAPY_SDR_RX)
    {
        _tree->create<uhd::usrp::dboard_eeprom_t>(mb_path / "dboards" / chName / "rx_eeprom")
            .set(uhd::usrp::dboard_eeprom_t());
    }
    else
    {
        _tree->create<uhd::usrp::dboard_eeprom_t>(mb_path / "dboards" / chName / "tx_eeprom")
            .set(uhd::usrp::dboard_eeprom_t());
        _tree->create<uhd::usrp::dboard_eeprom_t>(mb_path / "dboards" / chName / "gdb_eeprom")
            .set(uhd::usrp::dboard_eeprom_t());
    }

    //gains
    BOOST_FOREACH(const std::string &name, _device->listGains(dir, chan))
    {
        _tree->create<uhd::meta_range_t>(rf_fe_path / "gains" / name / "range")
            .publish(boost::bind(&UHDSoapyDevice::get_gain_range, this, dir, chan, name));
        _tree->create<double>(rf_fe_path / "gains" / name / "value")
            .publish(boost::bind(&SoapySDR::Device::getGain, _device, dir, chan, name))
            .subscribe(boost::bind(&SoapySDR::Device::setGain, _device, dir, chan, name, _1));
    }

    //agc
    _tree->create<bool>(rf_fe_path / "gain" / "agc" / "enable")
        .publish(boost::bind(&SoapySDR::Device::getGainMode, _device, dir, chan))
        .subscribe(boost::bind(&SoapySDR::Device::setGainMode, _device, dir, chan, _1));

    //freq
    _tree->create<double>(rf_fe_path / "freq" / "value")
        .publish(boost::bind(&SoapySDR::Device::getFrequency, _device, dir, chan, rfCompName))
        .subscribe(boost::bind(&UHDSoapyDevice::set_frequency, this, dir, chan, rfCompName, _1));
    _tree->create<uhd::meta_range_t>(rf_fe_path / "freq" / "range")
        .publish(boost::bind(&UHDSoapyDevice::get_freq_range, this, dir, chan, rfCompName));
    _tree->create<bool>(rf_fe_path / "use_lo_offset").set(false);
    _tree->create<uhd::device_addr_t>(rf_fe_path / "tune_args")
        .subscribe(boost::bind(&UHDSoapyDevice::stash_tune_args, this, dir, chan, _1));

    //ant
    _tree->create<std::vector<std::string> >(rf_fe_path / "antenna" / "options")
        .publish(boost::bind(&SoapySDR::Device::listAntennas, _device, dir, chan));
    _tree->create<std::string>(rf_fe_path / "antenna" / "value")
        .publish(boost::bind(&SoapySDR::Device::getAntenna, _device, dir, chan))
        .subscribe(boost::bind(&SoapySDR::Device::setAntenna, _device, dir, chan, _1));

    //bw
    _tree->create<double>(rf_fe_path / "bandwidth" / "value")
        .publish(boost::bind(&SoapySDR::Device::getBandwidth, _device, dir, chan))
        .subscribe(boost::bind(&SoapySDR::Device::setBandwidth, _device, dir, chan, _1));
    _tree->create<uhd::meta_range_t>(rf_fe_path / "bandwidth" / "range")
        .publish(boost::bind(&UHDSoapyDevice::get_bw_range, this, dir, chan));
}

/***********************************************************************
 * RX Streaming
 **********************************************************************/
static SoapySDR::Stream *make_stream(SoapySDR::Device *d, const int direction, const uhd::stream_args_t &args)
{
    //ensure at least one channel selected
    std::vector<size_t> chans = args.channels;
    if (chans.empty()) chans.push_back(0);

    //load keyword args from stream args
    SoapySDR::Kwargs kwargs(dictToKwargs(args.args));

    //fill in WIRE keyword when specified
    if (not args.otw_format.empty() and kwargs.count("WIRE") == 0)
    {
        kwargs["WIRE"] = args.otw_format;
    }

    //the format string
    std::string hostFormat;
    BOOST_FOREACH(const char ch, args.cpu_format)
    {
        if (ch == 'c') hostFormat = "C" + hostFormat;
        else if (ch == 'f') hostFormat += "F";
        else if (ch == 's') hostFormat += "S";
        else if (std::isdigit(ch)) hostFormat += ch;
        else throw std::runtime_error("UHDSoapyDevice::setupStream("+args.cpu_format+") unknown format");
    }

    return d->setupStream(direction, hostFormat, chans, kwargs);
}

class UHDSoapyRxStream : public uhd::rx_streamer
{
public:
    UHDSoapyRxStream(SoapySDR::Device *d, const uhd::stream_args_t &args):
        _device(d),
        _stream(make_stream(d, SOAPY_SDR_RX, args)),
        _nchan(std::max<size_t>(1, args.channels.size())),
        _elemSize(uhd::convert::get_bytes_per_item(args.cpu_format))
    {
        _offsetBuffs.resize(_nchan);
    }

    ~UHDSoapyRxStream(void)
    {
        _device->deactivateStream(_stream);
        _device->closeStream(_stream);
    }

    size_t get_num_channels(void) const
    {
        return _nchan;
    }

    size_t get_max_num_samps(void) const
    {
        return _device->getStreamMTU(_stream);
    }

    size_t recv(
        const buffs_type &buffs,
        const size_t nsamps_per_buff,
        uhd::rx_metadata_t &md,
        const double timeout = 0.1,
        const bool one_packet = false
    )
    {
        size_t total = 0;
        md.reset();

        while (total < nsamps_per_buff)
        {
            int flags = 0;
            if (one_packet) flags |= SOAPY_SDR_ONE_PACKET;
            long long timeNs = 0;
            size_t numElems = (nsamps_per_buff-total);
            for (size_t i = 0; i < _nchan; i++) _offsetBuffs[i] = ((char *)buffs[i]) + total*_elemSize;
            int ret = _device->readStream(_stream, &(_offsetBuffs[0]), numElems, flags, timeNs, long(timeout*1e6));

            //deal with return error codes
            switch (ret)
            {
            case SOAPY_SDR_TIMEOUT:
                md.error_code = uhd::rx_metadata_t::ERROR_CODE_TIMEOUT;
                break;

            case SOAPY_SDR_STREAM_ERROR:
                md.error_code = uhd::rx_metadata_t::ERROR_CODE_BROKEN_CHAIN;
                break;

            case SOAPY_SDR_CORRUPTION:
                md.error_code = uhd::rx_metadata_t::ERROR_CODE_BAD_PACKET;
                break;

            case SOAPY_SDR_OVERFLOW:
                md.error_code = uhd::rx_metadata_t::ERROR_CODE_OVERFLOW;
                break;

            case SOAPY_SDR_TIME_ERROR:
                md.error_code = uhd::rx_metadata_t::ERROR_CODE_LATE_COMMAND;
                break;
            }
            if (ret < 0) break;
            total += ret;

            //more fragments always over written by last recv
            md.more_fragments = (flags & SOAPY_SDR_MORE_FRAGMENTS) != 0;

            //apply time if this is the first recv
            if (total == size_t(ret))
            {
                md.has_time_spec = (flags & SOAPY_SDR_HAS_TIME) != 0;
                md.time_spec = uhd::time_spec_t::from_ticks(timeNs, 1e9);
            }

            //mark end of burst and exit call
            if ((flags & SOAPY_SDR_END_BURST) != 0)
            {
                md.end_of_burst = true;
                break;
            }

            //inline overflow indication
            if ((flags & SOAPY_SDR_END_ABRUPT) != 0)
            {
                md.error_code = uhd::rx_metadata_t::ERROR_CODE_OVERFLOW;
                break;
            }

            //one pkt mode, end loop
            if (one_packet) break;
        }

        return total;
    }

    void issue_stream_cmd(const uhd::stream_cmd_t &stream_cmd)
    {
        int flags = 0;
        if (not stream_cmd.stream_now) flags |= SOAPY_SDR_HAS_TIME;
        long long timeNs = stream_cmd.time_spec.to_ticks(1e9);
        size_t numElems = 0;
        bool activate = true;

        switch(stream_cmd.stream_mode)
        {
        case uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS:
            break;

        case uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS:
            activate = false;
            break;

        case uhd::stream_cmd_t::STREAM_MODE_NUM_SAMPS_AND_DONE:
            flags |= SOAPY_SDR_END_BURST;
            numElems = stream_cmd.num_samps;
            break;

        case uhd::stream_cmd_t::STREAM_MODE_NUM_SAMPS_AND_MORE:
            numElems = stream_cmd.num_samps;
            break;
        }

        int ret = 0;
        if (activate) ret = _device->activateStream(_stream, flags, timeNs, numElems);
        else ret = _device->deactivateStream(_stream, flags, timeNs);
        if (ret != 0) throw std::runtime_error(str(boost::format("UHDSoapyRxStream::issue_stream_cmd() = %d") % ret));
    }

private:
    SoapySDR::Device *_device;
    SoapySDR::Stream *_stream;
    const size_t _nchan;
    const size_t _elemSize;
    std::vector<void *> _offsetBuffs;
    bool _doErrorOnNextRecv;
};

uhd::rx_streamer::sptr UHDSoapyDevice::get_rx_stream(const uhd::stream_args_t &args)
{
    uhd::rx_streamer::sptr stream(new UHDSoapyRxStream(_device, args));
    BOOST_FOREACH(const size_t ch, args.channels) _rx_streamers[ch] = stream;
    if (args.channels.empty()) _rx_streamers[0] = stream;
    return stream;
}

/***********************************************************************
 * TX Streaming
 **********************************************************************/
class UHDSoapyTxStream : public uhd::tx_streamer
{
public:
    UHDSoapyTxStream(SoapySDR::Device *d, const uhd::stream_args_t &args):
        _device(d),
        _stream(make_stream(d, SOAPY_SDR_TX, args)),
        _nchan(std::max<size_t>(1, args.channels.size())),
        _elemSize(uhd::convert::get_bytes_per_item(args.cpu_format))
    {
        _offsetBuffs.resize(_nchan);
        _device->activateStream(_stream);
    }

    ~UHDSoapyTxStream(void)
    {
        _device->deactivateStream(_stream);
        _device->closeStream(_stream);
    }

    size_t get_num_channels(void) const
    {
        return _nchan;
    }

    size_t get_max_num_samps(void) const
    {
        return _device->getStreamMTU(_stream);
    }

    size_t send(
        const buffs_type &buffs,
        size_t nsamps_per_buff,
        const uhd::tx_metadata_t &md,
        const double timeout = 0.1
    )
    {
        size_t total = 0;
        if (nsamps_per_buff == 0) nsamps_per_buff = 1;
        const long long timeNs(md.time_spec.to_ticks(1e9));

        while (total < nsamps_per_buff)
        {
            int flags = 0;
            size_t numElems = (nsamps_per_buff-total);
            if (md.has_time_spec and total == 0) flags |= SOAPY_SDR_HAS_TIME;
            if (md.end_of_burst) flags |= SOAPY_SDR_END_BURST;
            for (size_t i = 0; i < _nchan; i++) _offsetBuffs[i] = ((char *)buffs[i]) + total*_elemSize;
            int ret = _device->writeStream(_stream, &(_offsetBuffs[0]), numElems, flags, timeNs, long(timeout*1e6));
            if (ret == SOAPY_SDR_TIMEOUT) break;
            if (ret < 0) throw std::runtime_error(str(boost::format("UHDSoapyTxStream::send() = %d") % ret));
            total += ret;
        }

        return total;
    }

    bool recv_async_msg(uhd::async_metadata_t &md, double timeout = 0.1)
    {
        size_t chanMask = 0;
        int flags = 0;
        long long timeNs = 0;
        int ret = _device->readStreamStatus(_stream, chanMask, flags, timeNs, long(timeout*1e6));

        //save the first channel found in the mask
        md.channel = 0;
        for (size_t i = 0; i < _nchan; i++)
        {
            if ((chanMask & (1 << i)) == 0) continue;
            md.channel = i;
            break;
        }

        //convert the time
        md.has_time_spec = (flags & SOAPY_SDR_HAS_TIME) != 0;
        md.time_spec = uhd::time_spec_t::from_ticks(timeNs, 1e9);

        //check flags
        if ((flags & SOAPY_SDR_END_BURST) != 0)
        {
            md.event_code = uhd::async_metadata_t::EVENT_CODE_BURST_ACK;
        }

        //set event code based on ret
        switch (ret)
        {
        case SOAPY_SDR_TIMEOUT: return false;

        case SOAPY_SDR_STREAM_ERROR:
            md.event_code = uhd::async_metadata_t::EVENT_CODE_SEQ_ERROR;
            break;

        case SOAPY_SDR_NOT_SUPPORTED:
            md.event_code = uhd::async_metadata_t::EVENT_CODE_USER_PAYLOAD;
            break;

        case SOAPY_SDR_TIME_ERROR:
            md.event_code = uhd::async_metadata_t::EVENT_CODE_TIME_ERROR;
            break;

        case SOAPY_SDR_UNDERFLOW:
            md.event_code = uhd::async_metadata_t::EVENT_CODE_UNDERFLOW;
            break;
        }

        return true;
    }

private:
    SoapySDR::Device *_device;
    SoapySDR::Stream *_stream;
    const size_t _nchan;
    const size_t _elemSize;
    std::vector<const void *> _offsetBuffs;
};

uhd::tx_streamer::sptr UHDSoapyDevice::get_tx_stream(const uhd::stream_args_t &args)
{
    uhd::tx_streamer::sptr stream(new UHDSoapyTxStream(_device, args));
    BOOST_FOREACH(const size_t ch, args.channels) _tx_streamers[ch] = stream;
    if (args.channels.empty()) _tx_streamers[0] = stream;
    return stream;
}

bool UHDSoapyDevice::recv_async_msg(uhd::async_metadata_t &md, double timeout)
{
    uhd::tx_streamer::sptr stream = _tx_streamers[0].lock();
    if (not stream) return false;
    return stream->recv_async_msg(md, timeout);
}

/***********************************************************************
 * Soapy Logger handle forward to UHD
 **********************************************************************/
static void UHDSoapyLogger(const SoapySDR::LogLevel logLevel, const char *message)
{
    switch(logLevel)
    {
    case SOAPY_SDR_FATAL:
    case SOAPY_SDR_CRITICAL:
    case SOAPY_SDR_ERROR: UHD_MSG(error) << message << std::endl; break;
    case SOAPY_SDR_WARNING: UHD_MSG(warning) << message << std::endl; break;
    case SOAPY_SDR_NOTICE:
    case SOAPY_SDR_INFO:
    case SOAPY_SDR_DEBUG:
    case SOAPY_SDR_TRACE: UHD_MSG(status) << message << std::endl; break;
    case SOAPY_SDR_SSI: UHD_MSG(fastpath) << message << std::flush; break;
    }
}

/***********************************************************************
 * Registration
 **********************************************************************/
static uhd::device::sptr makeUHDSoapyDevice(const uhd::device_addr_t &device_addr)
{
    SoapySDR::registerLogHandler(&UHDSoapyLogger);
    return uhd::device::sptr(new UHDSoapyDevice(device_addr));
}

static uhd::device_addrs_t findUHDSoapyDevice(const uhd::device_addr_t &args_)
{
    //prevent going into the the SoapyUHDDevice
    SoapySDR::Kwargs args(dictToKwargs(args_));
    if (args.count(SOAPY_UHD_NO_DEEPER) != 0) return uhd::device_addrs_t();
    args[SOAPY_UHD_NO_DEEPER] = "";

    //type filter for soapy devices
    if (args.count("type") != 0 and args.at("type") != "soapy") return uhd::device_addrs_t();

    uhd::device_addrs_t result;
    BOOST_FOREACH(SoapySDR::Kwargs found, SoapySDR::Device::enumerate(args))
    {
        found.erase(SOAPY_UHD_NO_DEEPER);
        result.push_back(kwargsToDict(found));
    }
    return result;
}

UHD_STATIC_BLOCK(registerUHDSoapyDevice)
{
    #ifdef UHD_HAS_DEVICE_FILTER
    uhd::device::register_device(&findUHDSoapyDevice, &makeUHDSoapyDevice, uhd::device::USRP);
    #else
    uhd::device::register_device(&findUHDSoapyDevice, &makeUHDSoapyDevice);
    #endif
}
