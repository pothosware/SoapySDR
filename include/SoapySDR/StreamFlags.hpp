///
/// \file SoapySDR/StreamFlags.hpp
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
 * Indicate that read or write had a timeout.
 * This flag is set by the driver when read/write is
 * blocked for more than the specified timeout.
 */
static const int STREAM_FLAG_TIMEOUT = (1 << 0);

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
 * Indicates an overflow error condition.
 * This is set by read when an overflow occurs.
 * For example, and internal buffer has filled.
 */
static const int STREAM_FLAG_OVERFLOW = (1 << 3);

/*!
 * Indicates transmit or receive only a single packet.
 * Applicable when the driver fragments samples into packets.
 * For write, the user sets this flag to only send a single packet.
 * For read, the user sets this flag to only receive a single packet.
 */
static const int STREAM_FLAG_ONE_PACKET = (1 << 4);

}
