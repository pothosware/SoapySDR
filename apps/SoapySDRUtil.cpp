// Copyright (c) 2014-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Version.hpp>
#include <SoapySDR/Modules.hpp>
#include <SoapySDR/Registry.hpp>
#include <SoapySDR/Device.hpp>
#include <cstdlib>
#include <cstddef>
#include <iostream>
#include <getopt.h>

std::string SoapySDRDeviceProbe(SoapySDR::Device *);

/***********************************************************************
 * Print help message
 **********************************************************************/
static int printHelp(void)
{
    std::cout << "Usage SoapySDRUtil [options]" << std::endl;
    std::cout << "  Options summary:" << std::endl;
    std::cout << "    --help \t\t\t\t Print this help message" << std::endl;
    std::cout << "    --info \t\t\t\t Print module information" << std::endl;
    std::cout << "    --find[=\"driver=foo,type=bar\"] \t Discover available devices" << std::endl;
    std::cout << "    --make[=\"driver=foo,type=bar\"] \t Create a device instance" << std::endl;
    std::cout << "    --probe[=\"driver=foo,type=bar\"] \t Print detailed information" << std::endl;
    std::cout << "    --check[=driverName] \t\t Check if driver is present" << std::endl;
    std::cout << std::endl;
    return EXIT_SUCCESS;
}

/***********************************************************************
 * Print version and module info
 **********************************************************************/
static int printInfo(void)
{
    std::cout << "API Version: v" << SoapySDR::getAPIVersion() << std::endl;
    std::cout << "ABI Version: v" << SoapySDR::getABIVersion() << std::endl;
    std::cout << "Install root: " << SoapySDR::getRootPath() << std::endl;

    const auto modules = SoapySDR::listModules();
    for (const auto &mod : modules) std::cout << "Module found: " << mod << std::endl;
    if (modules.empty()) std::cout << "No modules found!" << std::endl;

    std::cout << "Loading modules... " << std::flush;
    SoapySDR::loadModules();
    std::cout << "done" << std::endl;

    std::cout << "Available factories...";
    const auto factories = SoapySDR::Registry::listFindFunctions();
    for (const auto &it : factories) std::cout << it.first << ", ";
    if (factories.empty()) std::cout << "No factories found!" << std::endl;
    std::cout << std::endl;
    return EXIT_SUCCESS;
}

/***********************************************************************
 * Find devices and print args
 **********************************************************************/
static int findDevices(void)
{
    std::string argStr;
    if (optarg != NULL) argStr = optarg;

    const auto results = SoapySDR::Device::enumerate(argStr);
    for (size_t i = 0; i < results.size(); i++)
    {
        std::cout << "Found device " << i << std::endl;
        for (const auto &it : results[i])
        {
            std::cout << "  " << it.first << " = " << it.second << std::endl;
        }
        std::cout << std::endl;
    }
    if (results.empty())
    {
        std::cerr << "No devices found!" << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << std::endl;
    return EXIT_SUCCESS;
}

/***********************************************************************
 * Make device and print hardware info
 **********************************************************************/
static int makeDevice(void)
{
    std::string argStr;
    if (optarg != NULL) argStr = optarg;

    std::cout << "Make device " << argStr << std::endl;
    try
    {
        auto device = SoapySDR::Device::make(argStr);
        std::cout << "  driver=" << device->getDriverKey() << std::endl;
        std::cout << "  hardware=" << device->getHardwareKey() << std::endl;
        for (const auto &it : device->getHardwareInfo())
        {
            std::cout << "  " << it.first << "=" << it.second << std::endl;
        }
        SoapySDR::Device::unmake(device);
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Error making device: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << std::endl;
    return EXIT_SUCCESS;
}

/***********************************************************************
 * Make device and print detailed info
 **********************************************************************/
static int probeDevice(void)
{
    std::string argStr;
    if (optarg != NULL) argStr = optarg;

    std::cout << "Probe device " << argStr << std::endl;
    try
    {
        auto device = SoapySDR::Device::make(argStr);
        std::cout << SoapySDRDeviceProbe(device) << std::endl;
        SoapySDR::Device::unmake(device);
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Error probing device: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << std::endl;
    return EXIT_SUCCESS;
}

/***********************************************************************
 * Check the registry for a specific driver
 **********************************************************************/
static int checkDriver(void)
{
    std::string driverName;
    if (optarg != NULL) driverName = optarg;

    std::cout << "Loading modules... " << std::flush;
    SoapySDR::loadModules();
    std::cout << "done" << std::endl;

    std::cout << "Checking driver '" << driverName << "'... " << std::flush;
    const auto factories = SoapySDR::Registry::listFindFunctions();

    if (factories.find(driverName) == factories.end())
    {
        std::cout << "MISSING!" << std::endl;
        return EXIT_FAILURE;
    }
    else
    {
        std::cout << "PRESENT" << std::endl;
        return EXIT_SUCCESS;
    }
}

/***********************************************************************
 * main utility entry point
 **********************************************************************/
int main(int argc, char *argv[])
{
    std::cout << "######################################################" << std::endl;
    std::cout << "## Soapy SDR -- the SDR abstraction library" << std::endl;
    std::cout << "######################################################" << std::endl;
    std::cout << std::endl;

    /*******************************************************************
     * parse command line options
     ******************************************************************/
    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"find", optional_argument, 0, 'f'},
        {"make", optional_argument, 0, 'm'},
        {"info", optional_argument, 0, 'i'},
        {"probe", optional_argument, 0, 'p'},
        {"check", optional_argument, 0, 'c'},
        {0, 0, 0,  0}
    };
    int long_index = 0;
    int option = 0;
    while ((option = getopt_long_only(argc, argv, "", long_options, &long_index)) != -1)
    {
        switch (option)
        {
        case 'h': return printHelp();
        case 'i': return printInfo();
        case 'f': return findDevices();
        case 'm': return makeDevice();
        case 'p': return probeDevice();
        case 'c': return checkDriver();
        }
    }

    //unknown or unspecified options, do help...
    return printHelp();
}
