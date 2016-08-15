// Copyright (c) 2014-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#pragma once
#include <SoapySDR/Config.hpp>
#include <SoapySDR/Types.hpp>
#include <SoapySDR/Types.h>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstring>

/*******************************************************************
 * Helpful converters
 ******************************************************************/
static inline char **toStrArray(const std::vector<std::string> &strs, size_t *length)
{
    char **out = (char **)calloc(strs.size(), sizeof(char *));
    for (size_t i = 0; i < strs.size(); i++)
    {
        out[i] = strdup(strs[i].c_str());
    }
    *length = strs.size();
    return out;
}

static inline SoapySDRRange toRange(const SoapySDR::Range &range)
{
    SoapySDRRange out;
    out.minimum = range.minimum();
    out.maximum = range.maximum();
    return out;
}

static inline SoapySDRRange *toRangeList(const SoapySDR::RangeList &ranges, size_t *length)
{
    SoapySDRRange *out = (SoapySDRRange *)calloc(ranges.size(), sizeof(SoapySDRRange));
    for (size_t i = 0; i < ranges.size(); i++) out[i] = toRange(ranges[i]);
    *length = ranges.size();
    return out;
}

static inline double *toNumericList(const std::vector<double> &values, size_t *length)
{
    double *out = (double *)calloc(values.size(), sizeof(double));
    for (size_t i = 0; i < values.size(); i++) out[i] = values[i];
    *length = values.size();
    return out;
}

static inline SoapySDR::Kwargs toKwargs(const SoapySDRKwargs *args)
{
    SoapySDR::Kwargs out;
    if (args == NULL) return out;
    for (size_t i = 0; i < args->size; i++)
    {
        out[args->keys[i]] = args->vals[i];
    }
    return out;
}

static inline SoapySDRKwargs toKwargs(const SoapySDR::Kwargs &args)
{
    SoapySDRKwargs out;
    std::memset(&out, 0, sizeof(out));
    for (const auto &it : args)
    {
        SoapySDRKwargs_set(&out, it.first.c_str(), it.second.c_str());
    }
    return out;
}

static inline SoapySDRKwargs *toKwargsList(const SoapySDR::KwargsList &args, size_t *length)
{
    SoapySDRKwargs *outArgs = (SoapySDRKwargs *)calloc(args.size(), sizeof(SoapySDRKwargs));
    for (size_t i = 0; i < args.size(); i++) outArgs[i] = toKwargs(args[i]);
    *length = args.size();
    return outArgs;
}

static inline SoapySDRArgInfo toArgInfo(const SoapySDR::ArgInfo &info)
{
    SoapySDRArgInfo out;
    out.key = strdup(info.key.c_str());
    out.name = strdup(info.name.c_str());
    out.description = strdup(info.description.c_str());
    out.units = strdup(info.units.c_str());
    out.type = SoapySDRArgInfoType(info.type);
    out.range = toRange(info.range);
    out.options = toStrArray(info.options, &out.numOptions);
    out.optionNames = toStrArray(info.optionNames, &out.numOptions);

    return out;
}

static inline SoapySDRArgInfo *toArgInfoList(const SoapySDR::ArgInfoList &infos, size_t *length)
{
    SoapySDRArgInfo *out = (SoapySDRArgInfo *)calloc(infos.size(), sizeof(SoapySDRArgInfo));
    for (size_t i = 0; i < infos.size(); i++)
    {
        out[i] = toArgInfo(infos[i]);
    }
    *length = infos.size();
    return out;
}

static inline std::vector<unsigned> toNumericVector(const unsigned *values, size_t length)
{
	std::vector<unsigned> out (length, 0);
    for (size_t i = 0; i < length; i++) out[i] = values[i];
    return out;
}

static inline unsigned *toNumericList(const std::vector<unsigned> &values)
{
	unsigned *out = (unsigned *)calloc(values.size(), sizeof(unsigned));
    for (size_t i = 0; i < values.size(); i++) out[i] = values[i];
    return out;
}

