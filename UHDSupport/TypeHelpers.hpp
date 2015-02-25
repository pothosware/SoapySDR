// Copyright (c) 2014-2015 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#pragma once
#include <SoapySDR/Types.hpp>
#include <uhd/types/device_addr.hpp>
#include <uhd/types/ranges.hpp>
#include <vector>

#define SOAPY_UHD_NO_DEEPER "soapy_uhd_no_deeper"

/***********************************************************************
 * Helpful type conversions
 **********************************************************************/
static inline SoapySDR::Kwargs dictToKwargs(const uhd::device_addr_t &addr)
{
    SoapySDR::Kwargs kwargs;
    const std::vector<std::string> keys = addr.keys();
    for (size_t i = 0; i < keys.size(); i++)
    {
        kwargs[keys[i]] = addr[keys[i]];
    }
    return kwargs;
}

static inline uhd::device_addr_t kwargsToDict(const SoapySDR::Kwargs &kwargs)
{
    uhd::device_addr_t addr;
    for (SoapySDR::Kwargs::const_iterator it = kwargs.begin(); it != kwargs.end(); ++it)
    {
        addr[it->first] = it->second;
    }
    return addr;
}

static inline SoapySDR::RangeList metaRangeToRangeList(const uhd::meta_range_t &metaRange)
{
    SoapySDR::RangeList out;
    for (size_t i = 0; i < metaRange.size(); i++)
    {
        out.push_back(SoapySDR::Range(metaRange[i].start(), metaRange[i].stop()));
    }
    return out;
}

static inline uhd::meta_range_t rangeListToMetaRange(const SoapySDR::RangeList &ranges)
{
    uhd::meta_range_t out;
    for (size_t i = 0; i < ranges.size(); i++)
    {
        out.push_back(uhd::range_t(ranges[i].minimum(), ranges[i].maximum()));
    }
    return out;
}

static inline SoapySDR::Range metaRangeToRange(const uhd::meta_range_t &metaRange)
{
    return SoapySDR::Range(metaRange.start(), metaRange.stop());
}

uhd::meta_range_t numberListToMetaRange(const std::vector<double> &nums)
{
    uhd::meta_range_t out;
    for (size_t i = 0; i < nums.size(); i++)
    {
        out.push_back(uhd::range_t(nums[i]));
    }
    return out;
}

static inline std::vector<double> metaRangeToNumericList(const uhd::meta_range_t &metaRange)
{
    std::vector<double> out;
    for (size_t i = 0; i < metaRange.size(); i++)
    {
        //in these cases start == stop
        out.push_back(metaRange[i].start());
    }
    return out;
}

static inline uhd::meta_range_t rangeToMetaRange(const SoapySDR::Range &range)
{
    return uhd::meta_range_t(range.minimum(), range.maximum());
}
