#include <SoapySDR/Device.hpp>
#include <SoapySDR/Registry.hpp>

/***********************************************************************
 * Device interface
 **********************************************************************/
class MyDevice : public SoapySDR::Device
{
    //Implement constructor with device specific arguments...

    //Implement all applicable virtual methods from SoapySDR::Device
};

/***********************************************************************
 * Find available devices
 **********************************************************************/
SoapySDR::KwargsList findMyDevice(const SoapySDR::Kwargs &args)
{
    (void)args;
    //locate the device on the system...
    //return a list of 0, 1, or more argument maps that each identify a device

    return SoapySDR::KwargsList();
}

/***********************************************************************
 * Make device instance
 **********************************************************************/
SoapySDR::Device *makeMyDevice(const SoapySDR::Kwargs &args)
{
    (void)args;
    //create an instance of the device object given the args
    //here we will translate args into something used in the constructor
    return new MyDevice();
}

/***********************************************************************
 * Registration
 **********************************************************************/
static SoapySDR::Registry registerMyDevice("my_device", &findMyDevice, &makeMyDevice, SOAPY_SDR_ABI_VERSION);
