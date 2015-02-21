// Copyright (c) 2015-2015 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include "TypeHelpers.hpp"
#include <uhd/utils/static.hpp>
#include <uhd/property_tree.hpp>
#include <uhd/device.hpp>

#include <SoapySDR/Device.hpp>

#include <boost/foreach.hpp>
#include <boost/thread/mutex.hpp>

/***********************************************************************
 * Soapy Logger handle forward to UHD
 **********************************************************************/
 
 //TODO
 
 //ALSO TODO the UHDLogHandler shouldnt autoload

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

private:
    SoapySDR::Device *_device;
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
}

UHDSoapyDevice::~UHDSoapyDevice(void)
{
    boost::mutex::scoped_lock l(suMutexMaker());
    SoapySDR::Device::unmake(_device);
}

/***********************************************************************
 * Streaming
 **********************************************************************/
uhd::rx_streamer::sptr UHDSoapyDevice::get_rx_stream(const uhd::stream_args_t &args)
{
    
}

uhd::tx_streamer::sptr UHDSoapyDevice::get_tx_stream(const uhd::stream_args_t &args)
{
    
}

bool UHDSoapyDevice::recv_async_msg(uhd::async_metadata_t &, double)
{
    
}

/***********************************************************************
 * Registration
 **********************************************************************/
static uhd::device::sptr makeUHDSoapyDevice(const uhd::device_addr_t &device_addr)
{
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
