///
/// \file SoapySDR/Flags.hpp
///
/// Flags used with the streaming API.
///
/// \copyright
/// Copyright (c) 2014-2014 Josh Blum
/// SPDX-License-Identifier: BSL-1.0
///

#pragma once
#include <SoapySDR/Config.hpp>

namespace SoapySDR
{

/*!
 * Indicate end of burst for transmit or receive.
 * For write, end of burst if set by the caller.
 * For read, end of burst is set by the driver.
 */
static const int STREAM_FLAG_END_BURST = (1 << 1);

/*!
 * Indicates that the time stamp is valid.
 * For write, the caller must set has time when timeNs is provided.
 * For read, the driver sets has time when timeNs is provided.
 */
static const int STREAM_FLAG_HAS_TIME = (1 << 2);

/*!
 * Indicates transmit or receive only a single packet.
 * Applicable when the driver fragments samples into packets.
 * For write, the user sets this flag to only send a single packet.
 * For read, the user sets this flag to only receive a single packet.
 */
static const int STREAM_FLAG_ONE_PACKET = (1 << 4);

/*!
 * Returned when read has a timeout.
 */
static const int ERROR_CODE_TIMEOUT = -1;

/*!
 * Returned for non-specific stream errors.
 */
static const int ERROR_CODE_STREAM_ERROR = -2;

/*!
 * Returned when read has data corruption.
 * For example, the driver saw a malformed packet.
 */
static const int ERROR_CODE_CORRUPTION = -3;

/*!
 * Returned when read has an overflow condition.
 * For example, and internal buffer has filled.
 */
static const int ERROR_CODE_OVERFLOW = -4;

}
