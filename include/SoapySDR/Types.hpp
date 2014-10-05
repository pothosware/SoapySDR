///
/// \file SoapySDR/Types.hpp
///
/// Misc data type definitions used in the API.
///
/// \copyright
/// Copyright (c) 2014-2014 Josh Blum
/// SPDX-License-Identifier: BSL-1.0
///

#pragma once
#include <SoapySDR/Config.hpp>
#include <vector>
#include <string>
#include <utility> //pair
#include <map>

namespace SoapySDR
{

//! Direction type: transmit or receive
enum Direction {TX, RX};

//! Typedef for a dictionary of key-value string arguments
typedef std::map<std::string, std::string> Kwargs;

//! Typedef for a dictionary of string to numeric values
typedef std::map<std::string, double> NumericDict;

//! Typedef for a min/max numeric range
typedef std::pair<double, double> Range;

//! Typedef for a list of min/max range pairs
typedef std::vector<Range> RangeList;

}
