///
/// \file SoapySDR/Flags.h
///
/// Flags used with the streaming API.
///
/// \copyright
/// Copyright (c) 2014-2014 Josh Blum
/// SPDX-License-Identifier: BSL-1.0
///

#pragma once
#include <SoapySDR/Config.h>

/*!
 * Indicate end of burst for transmit or receive.
 * For write, end of burst if set by the caller.
 * For read, end of burst is set by the driver.
 */
#define SOAPY_SDR_END_BURST (1 << 1)

/*!
 * Indicates that the time stamp is valid.
 * For write, the caller must set has time when timeNs is provided.
 * For read, the driver sets has time when timeNs is provided.
 */
#define SOAPY_SDR_HAS_TIME (1 << 2)

/*!
 * Indicates transmit or receive only a single packet.
 * Applicable when the driver fragments samples into packets.
 * For write, the user sets this flag to only send a single packet.
 * For read, the user sets this flag to only receive a single packet.
 */
#define SOAPY_SDR_ONE_PACKET (1 << 4)

/*!
 * Returned when read has a timeout.
 */
#define SOAPY_SDR_TIMEOUT (-1)

/*!
 * Returned for non-specific stream errors.
 */
#define SOAPY_SDR_STREAM_ERROR (-2)

/*!
 * Returned when read has data corruption.
 * For example, the driver saw a malformed packet.
 */
#define SOAPY_SDR_CORRUPTION (-3)

/*!
 * Returned when read has an overflow condition.
 * For example, and internal buffer has filled.
 */
#define SOAPY_SDR_OVERFLOW (-4)

