///
/// \file SoapySDR/Types.hpp
///
/// Misc data type definitions used in the API.
///
/// \copyright
/// Copyright (c) 2014-2017 Josh Blum
/// SPDX-License-Identifier: BSL-1.0
///

#pragma once
#include <SoapySDR/Config.hpp>
#include <SoapySDR/Types.h>
#include <type_traits>
#include <vector>
#include <string>
#include <map>

namespace SoapySDR
{

//! Typedef for a dictionary of key-value string arguments
typedef std::map<std::string, std::string> Kwargs;

/*!
 * Convert a markup string to a key-value map.
 * The markup format is: "key0=value0, key1=value1"
 */
SOAPY_SDR_API Kwargs KwargsFromString(const std::string &markup);

/*!
 * Convert a key-value map to a markup string.
 * The markup format is: "key0=value0, key1=value1"
 */
SOAPY_SDR_API std::string KwargsToString(const Kwargs &args);

//! Typedef for a list of key-word dictionaries
typedef std::vector<Kwargs> KwargsList;

/*!
 * Convert a string to the specified type.
 * Supports bools, integers, floats, and strings
 * \tparam Type the specified output type
 * \param s the setting value as a string
 * \return the value converted to Type
 */
template <typename Type>
Type StringToSetting(const std::string &s);

/*!
 * Convert an arbitrary type to a string.
 * Supports bools, integers, floats, and strings
 * \tparam Type the type of the input argument
 * \param s the setting value as the specified type
 * \return the value converted to a string
 */
template <typename Type>
std::string SettingToString(const Type &s);

/*!
 * A simple min/max numeric range pair
 */
class SOAPY_SDR_API Range
{
public:

    //! Create an empty range (0.0, 0.0)
    Range(void);

    //! Create a min/max range
    Range(const double minimum, const double maximum, const double step=0.0);

    //! Get the range minimum
    double minimum(void) const;

    //! Get the range maximum
    double maximum(void) const;

    //! Get the range step size
    double step(void) const;

private:
    double _min, _max, _step;
};

/*!
 * Typedef for a list of min/max range pairs.
 * Overall minimum: rl.front().minimum();
 * Overall maximum: rl.back().maximum();
 */
typedef std::vector<Range> RangeList;

/*!
 * Argument info describes a key/value argument.
 */
class SOAPY_SDR_API ArgInfo
{
public:

    //! Default constructor
    ArgInfo(void);

    //! The key used to identify the argument (required)
    std::string key;

    /*!
     * The default value of the argument when not specified (required)
     * Numbers should use standard floating point and integer formats.
     * Boolean values should be represented as "true" and  "false".
     */
    std::string value;

    //! The displayable name of the argument (optional, use key if empty)
    std::string name;

    //! A brief description about the argument (optional)
    std::string description;

    //! The units of the argument: dB, Hz, etc (optional)
    std::string units;

    //! The data type of the argument (required)
    enum Type {BOOL, INT, FLOAT, STRING} type;

    /*!
     * The range of possible numeric values (optional)
     * When specified, the argument should be restricted to this range.
     * The range is only applicable to numeric argument types.
     */
    Range range;

    /*!
     * A discrete list of possible values (optional)
     * When specified, the argument should be restricted to this options set.
     */
    std::vector<std::string> options;

    /*!
     * A discrete list of displayable names for the enumerated options (optional)
     * When not specified, the option value itself can be used as a display name.
     */
    std::vector<std::string> optionNames;
};

/*!
 * Typedef for a list of Argument infos.
 */
typedef std::vector<ArgInfo> ArgInfoList;

}

inline double SoapySDR::Range::minimum(void) const
{
    return _min;
}

inline double SoapySDR::Range::maximum(void) const
{
    return _max;
}

inline double SoapySDR::Range::step(void) const
{
    return _step;
}

namespace SoapySDR {
namespace Detail {


//private implementation details for settings converters

template <typename Type>
typename std::enable_if<std::is_same<Type, bool>::value, Type>::type StringToSetting(const std::string &s)
{
    if (s == SOAPY_SDR_TRUE) return true;
    if (s == SOAPY_SDR_FALSE) return false;

    //zeros and empty strings are false
    if (s == "0") return false;
    if (s == "0.0") return false;
    if (s == "") return false;

    //other values are true
    return "true";
}

template <typename Type>
typename std::enable_if<not std::is_same<Type, bool>::value and std::is_integral<Type>::value and std::is_signed<Type>::value, Type>::type StringToSetting(const std::string &s)
{
    return Type(std::stoll(s));
}

template <typename Type>
typename std::enable_if<not std::is_same<Type, bool>::value and std::is_integral<Type>::value and std::is_unsigned<Type>::value, Type>::type StringToSetting(const std::string &s)
{
    return Type(std::stoull(s));
}

template <typename Type>
typename std::enable_if<std::is_floating_point<Type>::value, Type>::type StringToSetting(const std::string &s)
{
    return Type(std::stod(s));
}

template <typename Type>
typename std::enable_if<std::is_same<typename std::decay<Type>::type, std::string>::value, Type>::type StringToSetting(const std::string &s)
{
    return s;
}

inline std::string SettingToString(const bool &s)
{
    return s?SOAPY_SDR_TRUE:SOAPY_SDR_FALSE;
}

inline std::string SettingToString(const char *s)
{
    return s;
}

inline std::string SettingToString(const std::string &s)
{
    return s;
}

template <typename Type>
std::string SettingToString(const Type &s)
{
    return std::to_string(s);
}

}}

template <typename Type>
Type SoapySDR::StringToSetting(const std::string &s)
{
    return SoapySDR::Detail::StringToSetting<Type>(s);
}

template <typename Type>
std::string SoapySDR::SettingToString(const Type &s)
{
    return SoapySDR::Detail::SettingToString(s);
}
