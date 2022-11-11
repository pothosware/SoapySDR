// Copyright (c) 2021-2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

%typemap(csclassmodifiers) SoapySDR::CSharp::StreamHandle "internal class";
%nodefaultctor SoapySDR::CSharp::StreamHandle;
%ignore SoapySDR::CSharp::StreamHandle::stream;
%ignore SoapySDR::CSharp::StreamHandle::channels;
%ignore SoapySDR::CSharp::StreamHandle::format;

%nodefaultctor SoapySDR::CSharp::StreamFormat;

%{
#include "Stream.hpp"
%}

//
// SoapySDR::CSharp::StreamFlags
//

%typemap(csimports) SoapySDR::CSharp::StreamFlags "
using System;"
%typemap(csclassmodifiers) SoapySDR::CSharp::StreamFlags "
/// <summary>
/// Bitwise flags to be passed into or returned from streaming functions.
/// </summary>
[Flags]
public enum";

%csattributes SoapySDR::CSharp::StreamFlags::None "/// <summary>No flags.</summary>";
%csattributes SoapySDR::CSharp::StreamFlags::EndBurst "///
/// <summary>
/// Indicate end of burst for transmit or receive. For write, end of burst if set by the caller. For read, end of burst is set by the driver.
/// </summary>";
%csattributes SoapySDR::CSharp::StreamFlags::HasTime "///
/// <summary>
/// Indicates that the time stamp is valid. For write, the caller must set has time when <b>timeNs</b> is provided. For read, the driver sets has time when <b>timeNs</b> is provided.
/// </summary>";
%csattributes SoapySDR::CSharp::StreamFlags::EndAbrupt "///
/// <summary>
/// Indicates that the stream terminated prematurely. This is the flag version of an overflow error that indicates an overflow with the end samples.
/// </summary>";
%csattributes SoapySDR::CSharp::StreamFlags::OnePacket "///
/// <summary>
/// Indicates transmit or receive only a single packet.
/// Applicable when the driver fragments samples into packets.
///
/// For write, the user sets this flag to only send a single packet.
/// For read, the user sets this flag to only receive a single packet.
/// </summary>";
%csattributes SoapySDR::CSharp::StreamFlags::MoreFragments "///
/// <summary>
/// Indicate that this read call and the next results in a fragment.
/// Used when the implementation has an underlying packet interface.
///
/// The caller can use this indicator and the OnePacket flag
/// on subsequent read stream calls to re-align with packet boundaries.
/// </summary>";
%csattributes SoapySDR::CSharp::StreamFlags::WaitTrigger "///
/// <summary>
/// Indicate that the stream should wait for an external trigger event.
/// This flag might be used with the <b>flags</b> argument in any of the
/// stream API calls. The trigger implementation is hardware-specific.
/// </summary>";
%csattributes SoapySDR::CSharp::StreamFlags::UserFlag0 "/// <summary>A flag that can be used for SDR specific data.</summary>";
%csattributes SoapySDR::CSharp::StreamFlags::UserFlag1 "/// <summary>A flag that can be used for SDR specific data.</summary>";
%csattributes SoapySDR::CSharp::StreamFlags::UserFlag2 "/// <summary>A flag that can be used for SDR specific data.</summary>";
%csattributes SoapySDR::CSharp::StreamFlags::UserFlag3 "/// <summary>A flag that can be used for SDR specific data.</summary>";
%csattributes SoapySDR::CSharp::StreamFlags::UserFlag4 "/// <summary>A flag that can be used for SDR specific data.</summary>";

//
// SoapySDR::CSharp::StreamFormat
//

%typemap(csclassmodifiers) SoapySDR::CSharp::StreamFormat "
/// <summary>A set of string constants corresponding to different sample types.</summary>
public class";

%csmethodmodifiers SoapySDR::CSharp::StreamFormat::FormatToSize "
/// <summary>
/// Return the size of the given sample type, coming out to twice the size of the primitive type.
///
/// This value is intended to be used when allocating unmanaged memory to be used for streaming.
/// </summary>
public";

%csattributes SoapySDR::CSharp::StreamFormat::ComplexFloat64 "/// <summary>Complex double-precision floating-point samples.</summary>";
%csattributes SoapySDR::CSharp::StreamFormat::ComplexFloat32 "/// <summary>Complex single-precision floating-point samples.</summary>";
%csattributes SoapySDR::CSharp::StreamFormat::ComplexInt32 "/// <summary>Complex signed 32-bit integer samples.</summary>";
%csattributes SoapySDR::CSharp::StreamFormat::ComplexUInt32 "/// <summary>Complex unsigned 32-bit integer samples.</summary>";
%csattributes SoapySDR::CSharp::StreamFormat::ComplexInt16 "/// <summary>Complex signed 16-bit integer samples.</summary>";
%csattributes SoapySDR::CSharp::StreamFormat::ComplexUInt16 "/// <summary>Complex unsigned 16-bit integer samples.</summary>";
%csattributes SoapySDR::CSharp::StreamFormat::ComplexInt12 "
/// <summary>Packed complex signed 12-bit integer samples.
///
/// This stream format can only be used with the non-generic stream API, as there is no native C# representation for this type.
/// </summary>";
%csattributes SoapySDR::CSharp::StreamFormat::ComplexUInt12 "
/// <summary>Packed complex unsigned 12-bit integer samples.
///
/// This stream format can only be used with the non-generic stream API, as there is no native C# representation for this type.
/// </summary>";
%csattributes SoapySDR::CSharp::StreamFormat::ComplexInt8 "/// <summary>Complex signed 8-bit integer samples.</summary>";
%csattributes SoapySDR::CSharp::StreamFormat::ComplexUInt8 "/// <summary>Complex unsigned 8-bit integer samples.</summary>";
%csattributes SoapySDR::CSharp::StreamFormat::ComplexInt4 "
/// <summary>Packed complex signed 4-bit integer samples.
///
/// This stream format can only be used with the non-generic stream API, as there is no native C# representation for this type.
/// </summary>";
%csattributes SoapySDR::CSharp::StreamFormat::ComplexUInt4 "
/// <summary>Packed complex unsigned 4-bit integer samples.
///
/// This stream format can only be used with the non-generic stream API, as there is no native C# representation for this type.
/// </summary>";

//
// SoapySDR::CSharp::StreamHandle
//

%typemap(csimports) SoapySDR::CSharp::StreamHandle "
using System;
using System.Linq;"

//
// SoapySDR::CSharp::StreamResult
//

%typemap(csclassmodifiers) SoapySDR::CSharp::StreamResult "
/// <summary>Metadata returned from a streaming operation.</summary>
public class";

%csattributes SoapySDR::CSharp::StreamResult::NumSamples "/// <summary>How many samples were written or read by the streaming operation.</summary>";
%csattributes SoapySDR::CSharp::StreamResult::Flags "/// <summary>Bitwise metadata flags returned from the driver.</summary>";
%csattributes SoapySDR::CSharp::StreamResult::TimeNs "/// <summary>If positive, a timestamp for the associated buffer.</summary>";
%csattributes SoapySDR::CSharp::StreamResult::ChanMask "/// <summary>A bitwise mask of which channels the operation applies to.</summary>";

//
// Process the header
//

%include "Stream.hpp"