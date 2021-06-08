// Copyright (c) 2019-2019 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Types.hpp>
#include <cstdlib>
#include <cstdio>
#include <iostream>

#define check_equal(x, y) \
    printf("  Check %s == %s ... ", #x, #y); \
    if ((x) != (y)) \
    { \
        printf("FAIL\n"); \
        std::cout << "  -> " << x << " != " << y << std::endl; \
        return EXIT_FAILURE; \
    } \
    else printf("PASS\n")

int main(void)
{
    printf("Check boolean types:\n");
    check_equal(SoapySDR::SettingToString(true), SOAPY_SDR_TRUE);
    check_equal(SoapySDR::SettingToString(false), SOAPY_SDR_FALSE);
    check_equal(SoapySDR::StringToSetting<bool>(SOAPY_SDR_TRUE), true);
    check_equal(SoapySDR::StringToSetting<bool>(SOAPY_SDR_FALSE), false);
    check_equal(SoapySDR::StringToSetting<bool>("one-headed donkey"), true);
    check_equal(SoapySDR::StringToSetting<bool>("+"), true);
    check_equal(SoapySDR::StringToSetting<bool>(""), false);
    check_equal(SoapySDR::StringToSetting<bool>("0"), false);
    check_equal(SoapySDR::StringToSetting<bool>("0.2"), true);
    check_equal(SoapySDR::StringToSetting<bool>("1"), true);
    check_equal(SoapySDR::StringToSetting<bool>("-42"), true);
    check_equal(SoapySDR::StringToSetting<bool>("0.0"), false);
    check_equal(SoapySDR::StringToSetting<bool>("1.0"), true);
    check_equal(SoapySDR::StringToSetting<bool>("0e12"), false);
    check_equal(SoapySDR::StringToSetting<bool>("0.2e12"), true);
    check_equal(SoapySDR::StringToSetting<bool>("000"), false);
    check_equal(SoapySDR::StringToSetting<bool>("001"), true);

    printf("Check integer types:\n");
    check_equal(SoapySDR::SettingToString(0), "0");
    check_equal(SoapySDR::SettingToString(-1), "-1");
    check_equal(SoapySDR::SettingToString(1), "1");
    check_equal(SoapySDR::SettingToString(100000000000000ull), "100000000000000");
    check_equal(SoapySDR::SettingToString(-100000000000000ll), "-100000000000000");
    check_equal(SoapySDR::StringToSetting<int>("0"), 0);
    check_equal(SoapySDR::StringToSetting<int>("-1"), -1);
    check_equal(SoapySDR::StringToSetting<int>("1"), 1);
    check_equal(SoapySDR::StringToSetting<unsigned long long>("100000000000000"), 100000000000000ull);
    check_equal(SoapySDR::StringToSetting<long long>("-100000000000000"), -100000000000000ll);

    printf("Check string types:\n");
    check_equal(SoapySDR::SettingToString(""), "");
    check_equal(SoapySDR::SettingToString(std::string()), "");
    check_equal(SoapySDR::SettingToString("hello"), "hello");
    check_equal(SoapySDR::StringToSetting<std::string>(""), "");
    check_equal(SoapySDR::StringToSetting<std::string>(std::string()), "");
    check_equal(SoapySDR::StringToSetting<std::string>("hello"), "hello");

    printf("Check float types:\n");
    //floating point formatting is harder, cheat with std::to_string
    check_equal(SoapySDR::SettingToString(0.0), std::to_string(0.0));
    check_equal(SoapySDR::SettingToString(1.1), std::to_string(1.1));
    check_equal(SoapySDR::SettingToString(-1.1), std::to_string(-1.1));
    check_equal(SoapySDR::StringToSetting<double>("0.0"), 0.0);
    check_equal(SoapySDR::StringToSetting<double>("1.1"), +1.1);
    check_equal(SoapySDR::StringToSetting<double>("-1.1"), -1.1);

    printf("DONE!\n");
    return EXIT_SUCCESS;
}
