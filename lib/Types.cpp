// Copyright (c) 2014-2017 Josh Blum
//                    2021 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Types.hpp>
#include <cctype>

static std::string trim(const std::string &s)
{
    std::string out = s;
    while (not out.empty() and std::isspace(out[0])) out = out.substr(1);
    while (not out.empty() and std::isspace(out[out.size()-1])) out = out.substr(0, out.size()-1);
    return out;
}

SoapySDR::Kwargs SoapySDR::KwargsFromString(const std::string &markup)
{
    SoapySDR::Kwargs kwargs;

    bool inKey = true;
    std::string key, val;
    for (size_t i = 0; i < markup.size(); i++)
    {
        const char ch = markup[i];
        if (inKey)
        {
            if (ch == '=') inKey = false;
            else if (ch == ',') inKey = true;
            else key += ch;
        }
        else
        {
            if (ch == ',') inKey = true;
            else val += ch;
        }
        if ((inKey and (not val.empty() or (ch == ','))) or ((i+1) == markup.size()))
        {
            key = trim(key);
            val = trim(val);
            if (not key.empty()) kwargs[key] = val;
            key = "";
            val = "";
        }
    }

    return kwargs;
}

std::string SoapySDR::KwargsToString(const SoapySDR::Kwargs &args)
{
    std::string markup;

    for (const auto &pair : args)
    {
        if (not markup.empty()) markup += ", ";
        markup += pair.first + "=" + pair.second;
    }

    return markup;
}

SoapySDR::Range::Range(void):
    _min(0.0),
    _max(0.0),
    _step(0.0)
{
    return;
}

SoapySDR::Range::Range(const double minimum, const double maximum, const double step):
    _min(minimum),
    _max(maximum),
    _step(step)
{
    return;
}

SoapySDR::ArgInfo::ArgInfo(void):
    type(SoapySDR::ArgInfo::Type::STRING)
{
    return;
}
