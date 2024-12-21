// Copyright (c) 2014-2020 Josh Blum
//                    2021 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

#pragma once
#include <SoapySDR/Config.hpp>
#include <SoapySDR/Types.hpp>
#include <SoapySDR/Types.h>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <algorithm>

/*******************************************************************
 * Helpful converters
 ******************************************************************/
template <typename T>
T *callocArrayType(const size_t length)
{
    auto out = (T *)std::calloc(length, sizeof(T));
    if (out == nullptr) throw std::bad_alloc();
    return out;
}

static inline char *toCString(const std::string &s)
{
    auto out = callocArrayType<char>(s.size()+1);
    std::copy(s.begin(), s.end(), out);
    return out;
}

static inline char **toStrArray(const std::vector<std::string> &strs, size_t *length, size_t minLength = 0)
{
    auto out = callocArrayType<char *>(std::max(minLength, strs.size()));
    for (size_t i = 0; i < strs.size(); i++)
    {
        try
        {
            out[i] = toCString(strs[i]);
        }
        catch (const std::bad_alloc &)
        {
            SoapySDRStrings_clear(&out, i);
            throw;
        }
    }
    *length = strs.size();
    return out;
}

static inline std::vector<std::string> toStringVector(const char * const *strs, size_t length)
{
    std::vector<std::string> out;
    for(size_t i = 0; i < length; ++i) out.emplace_back(strs[i]);

    return out;
}

static inline SoapySDRRange toRange(const SoapySDR::Range &range)
{
    SoapySDRRange out;
    out.minimum = range.minimum();
    out.maximum = range.maximum();
    out.step = range.step();
    return out;
}

static inline SoapySDRRange *toRangeList(const SoapySDR::RangeList &ranges, size_t *length)
{
    auto out = callocArrayType<SoapySDRRange>(ranges.size());
    for (size_t i = 0; i < ranges.size(); i++) out[i] = toRange(ranges[i]);
    *length = ranges.size();
    return out;
}

static inline double *toNumericList(const std::vector<double> &values, size_t *length)
{
    auto out = callocArrayType<double>(values.size());
    std::copy(values.begin(), values.end(), out);
    *length = values.size();
    return out;
}

static inline SoapySDR::Kwargs toKwargs(const SoapySDRKwargs *args)
{
    SoapySDR::Kwargs out;
    if (args == nullptr) return out;
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
        if (SoapySDRKwargs_set(&out, it.first.c_str(), it.second.c_str()) != 0)
        {
            throw std::bad_alloc();
        }
    }
    return out;
}

static inline SoapySDRKwargs *toKwargsList(const SoapySDR::KwargsList &args, size_t *length)
{
    auto outArgs = callocArrayType<SoapySDRKwargs>(args.size());
    for (size_t i = 0; i < args.size(); i++) outArgs[i] = toKwargs(args[i]);
    *length = args.size();
    return outArgs;
}

static inline SoapySDR::KwargsList toKwargsList(const SoapySDRKwargs *args, const size_t length)
{
    SoapySDR::KwargsList outArgs(length);
    for (size_t i = 0; i < length; i++) outArgs[i] = toKwargs(args+i);
    return outArgs;
}

static inline SoapySDRArgInfo toArgInfo(const SoapySDR::ArgInfo &info)
{
    SoapySDRArgInfo out;
    std::memset(&out, 0, sizeof(out));
    try
    {
        out.key = toCString(info.key);
        out.value = toCString(info.value);
        out.name = toCString(info.name);
        out.description = toCString(info.description);
        out.units = toCString(info.units);
        out.type = SoapySDRArgInfoType(info.type);
        out.range = toRange(info.range);
        out.options = toStrArray(info.options, &out.numOptions);
        size_t namesLength = 0;
        // will be calloc-ed to be at least as long as options to prevent clients from reading garbage
        out.optionNames = toStrArray(info.optionNames, &namesLength, out.numOptions);
    }
    catch (const std::bad_alloc &)
    {
        SoapySDRArgInfo_clear(&out);
        throw;
    }

    return out;
}

static inline SoapySDRArgInfo *toArgInfoList(const SoapySDR::ArgInfoList &infos, size_t *length)
{
    auto out = callocArrayType<SoapySDRArgInfo>(infos.size());
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
    std::copy(values, values+length, out.data());
    return out;
}

static inline unsigned *toNumericList(const std::vector<unsigned> &values, size_t *length)
{
    auto out = callocArrayType<unsigned>(values.size());
    std::copy(values.begin(), values.end(), out);
    *length = values.size();
    return out;
}

