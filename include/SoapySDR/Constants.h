///
/// \file SoapySDR/Constants.h
///
/// Constants used in the device API.
///
/// \copyright
/// Copyright (c) 2014-2016 Josh Blum
/// SPDX-License-Identifier: BSL-1.0
///

#pragma once
#include <SoapySDR/Config.h>

/*!
 * Constant to represent the transmit direction
 */
#define SOAPY_SDR_TX 0

/*!
 * Constant to represent the receive direction
 */
#define SOAPY_SDR_RX 1

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
 * Indicates that stream terminated prematurely.
 * This is the flag version of an overflow error
 * that indicates an overflow with the end samples.
 */
#define SOAPY_SDR_END_ABRUPT (1 << 3)

/*!
 * Indicates transmit or receive only a single packet.
 * Applicable when the driver fragments samples into packets.
 * For write, the user sets this flag to only send a single packet.
 * For read, the user sets this flag to only receive a single packet.
 */
#define SOAPY_SDR_ONE_PACKET (1 << 4)

/*!
 * Indicate that this read call and the next results in a fragment.
 * Used when the implementation has an underlying packet interface.
 * The caller can use this indicator and the SOAPY_SDR_ONE_PACKET flag
 * on subsequent read stream calls to re-align with packet boundaries.
 */
#define SOAPY_SDR_MORE_FRAGMENTS (1 << 5)

/*!
 * Indicate that the stream should wait for an external trigger event.
 * This flag might be used with the flags argument in any of the
 * stream API calls. The trigger implementation is hardware-specific.
 */
#define SOAPY_SDR_WAIT_TRIGGER (1 << 6)
