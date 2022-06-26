// Copyright (c) 2016,2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <cstdlib>
#include <string>

/*
 * Java has no reliable and non-hacky way of setting environment variables for
 * underlying native methods, so we must expose setenv,etc, to be able to set
 * the underlying SoapySDR's environment variables.
 */

namespace SoapySDR { namespace Java {

class Env
{
public:
    static inline std::string getEnv(const std::string &key)
    {
        char* val = getenv(key.c_str());
        return (val) ? val : "";
    }

    static inline void setEnv(
        const std::string &key,
        const std::string &val)
    {
        #ifdef _WIN32
            _putenv_s(key.c_str(), val.c_str());
        #else
            setenv(key.c_str(), val.c_str(), 1);
        #endif
    }

    static inline void unsetEnv(const std::string &key)
    {
        #ifdef _WIN32
            _putenv_s(key.c_str(), "");
        #else
            unsetenv(key.c_str());
        #endif
    }
};

}}
