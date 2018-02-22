// Copyright (c) 2014-2018 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Version.hpp>
#include <SoapySDR/Modules.hpp>
#include <SoapySDR/Registry.hpp>
#include <SoapySDR/Device.hpp>
#include <SoapySDR/ConverterRegistry.hpp>
#include <algorithm> //sort, min, max
#include <cstdlib>
#include <cstddef>
#include <iostream>
#include <iomanip>
#include <getopt.h>

std::string SoapySDRDeviceProbe(SoapySDR::Device *);
int SoapySDRRateTest(
    const std::string &argStr,
    const double sampleRate,
    const std::string &channelStr,
    const std::string &directionStr);

/***********************************************************************
 * Print the banner
 **********************************************************************/
static void printBanner(void)
{
    std::cout << "######################################################" << std::endl;
    std::cout << "##     Soapy SDR -- the SDR abstraction library     ##" << std::endl;
    std::cout << "######################################################" << std::endl;
    std::cout << std::endl;
}

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
    std::cout << std::endl;

    std::cout << "  Advanced options:" << std::endl;
    std::cout << "    --check[=driverName] \t\t Check if driver is present" << std::endl;
    std::cout << "    --sparse             \t\t Simplified output for --find" << std::endl;
    std::cout << std::endl;

    std::cout << "  Rate testing options:" << std::endl;
    std::cout << "    --args[=\"driver=foo\"] \t\t Arguments for testing" << std::endl;
    std::cout << "    --rate[=stream rate Sps] \t\t Rate in samples per second" << std::endl;
    std::cout << "    --channels[=\"0, 1, 2\"] \t\t List of channels, default 0" << std::endl;
    std::cout << "    --direction[=RX or TX] \t\t Specify the channel direction" << std::endl;
    std::cout << std::endl;
    return EXIT_SUCCESS;
}

/***********************************************************************
 * Print version and module info
 **********************************************************************/
static int printInfo(void)
{
    std::cout << "Lib Version: v" << SoapySDR::getLibVersion() << std::endl;
    std::cout << "API Version: v" << SoapySDR::getAPIVersion() << std::endl;
    std::cout << "ABI Version: v" << SoapySDR::getABIVersion() << std::endl;
    std::cout << "Install root: " << SoapySDR::getRootPath() << std::endl;

    for (const auto &path : SoapySDR::listSearchPaths())
        std::cout << "Search path: " << path << std::endl;

    //get a list of module and calculate the max path length
    const auto modules = SoapySDR::listModules();
    size_t maxModulePathLen(0);
    for (const auto &mod : modules) maxModulePathLen = std::max(maxModulePathLen, mod.size());

    //load each module and print information
    for (const auto &mod : modules)
    {
        std::cout << "Module found: " << mod;
        const auto &errMsg = SoapySDR::loadModule(mod);
        if (not errMsg.empty()) std::cout << "\n  " << errMsg;
        const auto version = SoapySDR::getModuleVersion(mod);
        if (not version.empty()) std::cout << std::string(maxModulePathLen-mod.size(), ' ') << " (" << version << ")";
        std::cout << std::endl;
    }
    if (modules.empty()) std::cout << "No modules found!" << std::endl;

    std::cout << "Available factories... ";
    std::string factories;
    for (const auto &it : SoapySDR::Registry::listFindFunctions())
    {
        if (not factories.empty()) factories += ", ";
        factories += it.first;
    }
    if (factories.empty()) factories = "No factories found!";
    std::cout << factories << std::endl;

    std::cout << "Available converters..." << std::endl;
    for (const auto &source : SoapySDR::ConverterRegistry::listAvailableSourceFormats())
    {
        std::string targets;
        for (const auto &target : SoapySDR::ConverterRegistry::listTargetFormats(source))
        {
            if (not targets.empty()) targets += ", ";
            targets += target;
        }
        std::cout << " - " << std::setw(5) << source << " -> [" << targets << "]" << std::endl;
    }

    return EXIT_SUCCESS;
}

/***********************************************************************
 * Find devices and print args
 **********************************************************************/
static int findDevices(const std::string &argStr, const bool sparse)
{
    const auto results = SoapySDR::Device::enumerate(argStr);
    if (sparse)
    {
        std::vector<std::string> sparseResults;
        for (size_t i = 0; i < results.size(); i++)
        {
            const auto it = results[i].find("label");
            if (it != results[i].end()) sparseResults.push_back(it->second);
            else sparseResults.push_back(SoapySDR::KwargsToString(results[i]));
        }
        std::sort(sparseResults.begin(), sparseResults.end());
        for (size_t i = 0; i < sparseResults.size(); i++)
        {
            std::cout << i << ": " << sparseResults[i] << std::endl;
        }
    }
    else
    {
        for (size_t i = 0; i < results.size(); i++)
        {
            std::cout << "Found device " << i << std::endl;
            for (const auto &it : results[i])
            {
                std::cout << "  " << it.first << " = " << it.second << std::endl;
            }
            std::cout << std::endl;
        }
        if (results.empty()) std::cerr << "No devices found!" << std::endl;
        else std::cout << std::endl;
    }
    return results.empty()?EXIT_FAILURE:EXIT_SUCCESS;
}

/***********************************************************************
 * Make device and print hardware info
 **********************************************************************/
static int makeDevice(const std::string &argStr)
{
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
static int probeDevice(const std::string &argStr)
{
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
static int checkDriver(const std::string &driverName)
{
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
    std::string argStr;
    std::string chanStr;
    std::string dirStr;
    double sampleRate(0.0);
    std::string driverName;
    bool findDevicesFlag(false);
    bool sparsePrintFlag(false);
    bool makeDeviceFlag(false);
    bool probeDeviceFlag(false);

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
        {"sparse", no_argument, 0, 's'},

        {"args", optional_argument, 0, 'a'},
        {"rate", optional_argument, 0, 'r'},
        {"channels", optional_argument, 0, 'n'},
        {"direction", optional_argument, 0, 'd'},
        {0, 0, 0,  0}
    };
    int long_index = 0;
    int option = 0;
    while ((option = getopt_long_only(argc, argv, "", long_options, &long_index)) != -1)
    {
        switch (option)
        {
        case 'h':
            printBanner();
            return printHelp();
        case 'i':
            printBanner();
            return printInfo();
        case 'f':
            findDevicesFlag = true;
            if (optarg != nullptr) argStr = optarg;
            break;
        case 'm':
            makeDeviceFlag = true;
            if (optarg != nullptr) argStr = optarg;
            break;
        case 'p':
            probeDeviceFlag = true;
            if (optarg != nullptr) argStr = optarg;
            break;
        case 'c':
            if (optarg != nullptr) driverName = optarg;
            break;
        case 's':
            sparsePrintFlag = true;
            break;
        case 'a':
            if (optarg != nullptr) argStr = optarg;
            break;
        case 'r':
            if (optarg != nullptr) sampleRate = std::stod(optarg);
            break;
        case 'n':
            if (optarg != nullptr) chanStr = optarg;
            break;
        case 'd':
            if (optarg != nullptr) dirStr = optarg;
            break;
        }
    }

    if (not sparsePrintFlag) printBanner();
    if (not driverName.empty()) return checkDriver(driverName);
    if (findDevicesFlag) return findDevices(argStr, sparsePrintFlag);
    if (makeDeviceFlag)  return makeDevice(argStr);
    if (probeDeviceFlag) return probeDevice(argStr);

    //invoke utilities that rely on multiple arguments
    if (sampleRate != 0.0)
    {
        return SoapySDRRateTest(argStr, sampleRate, chanStr, dirStr);
    }

    //unknown or unspecified options, do help...
    return printHelp();
}
