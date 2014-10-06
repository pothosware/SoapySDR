// Copyright (c) 2014-2014 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Version.hpp>
#include <SoapySDR/Modules.hpp>
#include <SoapySDR/Registry.hpp>
#include <cstdlib>
#include <cstddef>
#include <iostream>

int main(int argc, char *argv[])
{
    std::cout << "######################################################" << std::endl;
    std::cout << "## Soapy SDR -- the SDR abstraction library" << std::endl;
    std::cout << "######################################################" << std::endl;
    std::cout << std::endl;

    std::cout << "API Version: v" << SoapySDR::getAPIVersion() << std::endl;
    std::cout << "ABI Version: v" << SoapySDR::getABIVersion() << std::endl;
    std::cout << "Install root: " << SoapySDR::getRootPath() << std::endl;

    std::vector<std::string> modules = SoapySDR::listModules();
    for (size_t i = 0; i < modules.size(); i++)
    {
        std::cout << "Module found: " << modules[i] << std::endl;
    }
    if (modules.empty()) std::cout << "No modules found!" << std::endl;

    std::cout << "Loading modules... " << std::flush;
    SoapySDR::loadModules();
    std::cout << "done" << std::endl;

    std::cout << "Available factories...";
    std::map<std::string, SoapySDR::Registry::FindFunction> factories = SoapySDR::Registry::listFindFunctions();
    for (std::map<std::string, SoapySDR::Registry::FindFunction>::iterator it = factories.begin(); it != factories.end(); ++it)
    {
        std::cout << it->first << ", ";
    }
    std::cout << std::endl;

    std::cout << std::endl;
    return EXIT_SUCCESS;
}
