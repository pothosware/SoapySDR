// Copyright (c) 2014-2014 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Registry.hpp>
#include <cstdlib>
#include <cstddef>
#include <iostream>

int main(int argc, char *argv[])
{
    std::cout << "######################################################" << std::endl;
    std::cout << "## Soapy SDR v" << SoapySDR::Registry::getAPIVersion() << std::endl;
    std::cout << "######################################################" << std::endl;
    std::cout << std::endl;

    std::cout << "Install root: " << SoapySDR::Registry::getRootPath() << std::endl;

    std::vector<std::string> modules = SoapySDR::Registry::listModules();
    for (size_t i = 0; i < modules.size(); i++)
    {
        std::cout << "Module found: " << modules[i] << std::endl;
    }
    if (modules.empty()) std::cout << "No modules found!" << std::endl;

    std::cout << std::endl;
    return EXIT_SUCCESS;
}
