// Copyright (c) 2021-2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

// Unfortunately, SWIG doesn't support automatic docstring generation for
// C#, so we have to do this.

//
// SoapySDR::Device
//

%typemap(csclassmodifiers) Time "
/// <summary>
/// Utility functions to convert time and ticks.
/// </summary>
public class";

%csmethodmodifiers Time::TicksToTimeNs "
/// <summary>Convert a tick count into a time in nanoseconds using the tick rate.</summary>
/// <param name=\"ticks\">An integer tick count</param>
/// <param name=\"rate\">The ticks per second</param>
/// <returns>The time in nanoseconds</returns>
public";

%csmethodmodifiers Time::TimeNsToTicks "
/// <summary>Convert a time in nanoseconds into a tick count using the tick rate.</summary>
/// <param name=\"timeNs\">The time in nanoseconds</param>
/// <param name=\"rate\">The ticks per second</param>
/// <returns>The integer tick count</returns>
public";

%typemap(csclassmodifiers) SoapySDR::Device "
/// <summary>
/// Abstraction for an SDR transceiver device - configuration and streaming.
/// </summary>
public class";

%csmethodmodifiers SoapySDR::Device::enumerate(const Kwargs&) "
/// <summary>
/// Enumerate a list of devices on the system, given a set of filters.
/// </summary>
/// <param name=\"args\">A map of construction key/value argument filters</param>
/// <returns>A list of argument maps, each unique to a device</returns>
public";

%csmethodmodifiers SoapySDR::Device::enumerate(const std::string&) "
/// <summary>
/// Enumerate a list of devices on the system, given a set of filters.
///
/// Markup format for args: \"keyA=valA, keyB=valB\".
/// </summary>
/// <param name=\"args\">A markup string of device construction key/value argument filters</param>
/// <returns>A list of argument maps, each unique to a device</returns>
public";

%csmethodmodifiers SoapySDR::Device::Device() "
/// <summary>
/// Create a device instance from the first device SoapySDR finds.
/// </summary>
public";

%csmethodmodifiers SoapySDR::Device::Device(const SoapySDR::Kwargs &) "
/// <summary>
/// Create a device instance based on the given construction args.
/// </summary>
/// <param name=\"args\">Device construction args</param>
public";

%csmethodmodifiers SoapySDR::Device::Device(const std::string &) "
/// <summary>
/// Create a device instance based on the given construction args.
/// </summary>
/// <param name=\"args\">Device construction args</param>
public";

%csmethodmodifiers SoapySDR::Device::Device() "
/// <summary>
/// Create a device instance from the first device SoapySDR finds.
/// </summary>
public";

%csmethodmodifiers SoapySDR::Device::DriverKey "
/// <summary>
/// A key that uniquely identifies the device driver.
///
/// This key identifies the underlying implementation.
/// Several variants of a product may share a driver.
/// </summary>
public";

%csmethodmodifiers SoapySDR::Device::HardwareKey "
/// <summary>
/// A key that uniquely identifies the hardware.
///
/// This key should be meaningful to the user
/// to optimize for the underlying hardware.
/// </summary>
public";

%csmethodmodifiers SoapySDR::Device::HardwareInfo "
/// <summary>
/// A dictionary of available device information.
///
/// This dictionary can any number of values like
/// vendor name, product name, revisions, serials...
/// This information can be displayed to the user
/// to help identify the instantiated device.
/// </summary>
public";

%csmethodmodifiers SoapySDR::Device::setFrontendMapping "
/// <summary>
/// Set the frontend mapping of available DSP units to RF frontends.
///
/// This mapping controls channel mapping and channel availability.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"mapping\">A vendor-specific mapping string</param>
public";

%csmethodmodifiers SoapySDR::Device::getFrontendMapping "
/// <summary>
/// Get the mapping configuration string.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <returns>A vendor-specific mapping string</returns>
public";

%csmethodmodifiers SoapySDR::Device::getNumChannels "
/// <summary>
/// Get the number of channels given the streaming direction.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
public";

%csmethodmodifiers SoapySDR::Device::getChannelInfo "
/// <summary>
/// Query a dictionary of available channel information.
///
/// This dictionary can any number of values like
/// decoder type, version, available functions...
/// This information can be displayed to the user
/// to help identify the instantiated channel.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
public";

%csmethodmodifiers SoapySDR::Device::getFullDuplex "
/// <summary>
/// Find out if the specified channel is half or full duplex.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <returns>True for full duplex, false for half duplex</returns>
public";

%csmethodmodifiers SoapySDR::Device::getStreamFormats "
/// <summary>
/// Query a list of the available stream formats.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <returns>A list of allowed format strings. See Pothosware.SoapySDR.StreamFormat.</returns>
public";

%csmethodmodifiers SoapySDR::Device::getNativeStreamFormat "
/// <summary>
/// Get the hardware's native stream format for this channel.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <param name=\"fullScale\">The maximum possible value (output)</param>
/// <returns>The native stream format string. See Pothosware.SoapySDR.StreamFormat.</returns>
public";

%csmethodmodifiers SoapySDR::Device::getStreamArgsInfo "
/// <summary>
/// Query the argument info description for stream args.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
public";

%csmethodmodifiers SoapySDR::Device::listAntennas "
/// <summary>
/// Get a list of available antennas to select on a given chain.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <returns>A list of available antenna names</returns>
public";

%csmethodmodifiers SoapySDR::Device::setAntenna "
/// <summary>
/// Set the selected antenna on a chain.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <param name=\"name\">The name of an available antenna</param>
public";

%csmethodmodifiers SoapySDR::Device::getAntenna "
/// <summary>
/// Set the selected antenna on a chain.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <returns>The name of an available antenna</returns>
public";

%csmethodmodifiers SoapySDR::Device::hasDCOffsetMode "
/// <summary>
/// Does the device support automatic frontend DC offset correction?
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <returns>True if automatic correction is supported</returns>
public";

%csmethodmodifiers SoapySDR::Device::setDCOffsetMode "
/// <summary>
/// Enable or disable automatic frontend DC offset correction.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <param name=\"automatic\">True to enable correction, false to disable</param>
public";

%csmethodmodifiers SoapySDR::Device::getDCOffsetMode "
/// <summary>
/// Is automatic frontend DC offset correction enabled?
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <returns>True if correction is enabled, false if disabled</returns>
public";

%csmethodmodifiers SoapySDR::Device::hasDCOffset "
/// <summary>
/// Does the device support frontend DC offset correction?
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <returns>True if correction is supported</returns>
public";

%csmethodmodifiers SoapySDR::Device::setDCOffset "
/// <summary>
/// Set the frontend DC offset correction.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <param name=\"offset\">The relative correction (1.0 max)</param>
public";

%csmethodmodifiers SoapySDR::Device::getDCOffset "
/// <summary>
/// Get the frontend DC offset correction.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <returns>The relative correction (1.0 max)</returns>
public";

%csmethodmodifiers SoapySDR::Device::hasIQBalanceMode "
/// <summary>
/// Does the device support automatic frontend IQ balance correction?
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <returns>True if automatic correction is supported</returns>
public";

%csmethodmodifiers SoapySDR::Device::setIQBalanceMode "
/// <summary>
/// Enable or disable automatic frontend IQ balance correction.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <param name=\"automatic\">True to enable correction, false to disable</param>
public";

%csmethodmodifiers SoapySDR::Device::getIQBalanceMode "
/// <summary>
/// Is automatic frontend IQ balance correction enabled?
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <returns>True if correction is enabled, false if disabled</returns>
public";

%csmethodmodifiers SoapySDR::Device::hasIQBalance "
/// <summary>
/// Does the device support frontend IQ balance correction?
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <returns>True if correction is supported</returns>
public";

%csmethodmodifiers SoapySDR::Device::setIQBalance "
/// <summary>
/// Set the frontend IQ balance correction.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <param name=\"balance\">The relative correction (1.0 max)</param>
public";

%csmethodmodifiers SoapySDR::Device::getIQBalance "
/// <summary>
/// Get the frontend IQ balance correction.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <returns>The relative correction (1.0 max)</returns>
public";

%csmethodmodifiers SoapySDR::Device::hasFrequencyCorrection "
/// <summary>
/// Does the device support frontend frequency correction?
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <returns>True if correction is supported</returns>
public";

%csmethodmodifiers SoapySDR::Device::setFrequencyCorrection "
/// <summary>
/// Set the frontend frequency correction.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <param name=\"value\">The correction in PPM</param>
public";

%csmethodmodifiers SoapySDR::Device::getFrequencyCorrection "
/// <summary>
/// Get the frontend frequency correction.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <returns>The correction in PPM</returns>
public";

%csmethodmodifiers SoapySDR::Device::listGains "
/// <summary>
/// List available amplification elements, ordered from RF to baseband.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <returns>A list of gain string names</returns>
public";

%csmethodmodifiers SoapySDR::Device::hasGainMode "
/// <summary>
/// Does the device support automatic gain control?
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <returns>True if automatic correction is supported</returns>
public";

%csmethodmodifiers SoapySDR::Device::setGainMode "
/// <summary>
/// Enable or disable automatic gain control on the chain.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <param name=\"automatic\">True to enable automatic gain control, false to disable</param>
public";

%csmethodmodifiers SoapySDR::Device::getGainMode "
/// <summary>
/// Is automatic gain control enabled on the chain?
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <returns>True if automatic gain control is enabled, false if disabled</returns>
public";

%csmethodmodifiers SoapySDR::Device::setGain(const int, const size_t, const double) "
/// <summary>
/// Set the overall amplification in a chain.
///
/// The gain will be distributed automatically across available elements.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <param name=\"value\">The new amplification value in dB</param>
public";

%csmethodmodifiers SoapySDR::Device::setGain(const int, const size_t, const std::string &, const double) "
/// <summary>
/// Set the value of an amplification element in a chain.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <param name=\"name\">The name of an amplification element</param>
/// <param name=\"value\">The new amplification value in dB</param>
public";

%csmethodmodifiers SoapySDR::Device::getGain(const int, const size_t) const "
/// <summary>
/// Get the overall value of the gain elements in a chain.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <returns>The value of the gain in dB</returns>
public";

%csmethodmodifiers SoapySDR::Device::getGain(const int, const size_t, const std::string &) const "
/// <summary>
/// Get the value of an individual amplification element in a chain.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <param name=\"name\">The name of an amplification element</param>
/// <returns>The value of the gain in dB</returns>
public";

%csmethodmodifiers SoapySDR::Device::getGainRange(const int, const size_t) const "
/// <summary>
/// Get the overall range of possible gain values.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <returns>A list of gain ranges in dB</returns>
public";

%csmethodmodifiers SoapySDR::Device::getGainRange(const int, const size_t, const std::string &) const "
/// <summary>
/// Get the range of possible gain values for a specific element.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <param name=\"name\">The name of an amplification element</param>
/// <returns>A list of gain ranges in dB</returns>
public";

%csmethodmodifiers SoapySDR::Device::setFrequency(const int, const size_t, const double, const Kwargs &) "
/// <summary>
/// Set the center frequency of the chain.
///  - For RX, this specifies the down-conversion frequency.
///  - For TX, this specifies the up-conversion frequency.
///
/// When no args are provided, setFrequency() will tune the \"RF\"
/// component as close as possible to the requested center frequency.
/// Tuning inaccuracies will be compensated for with the \"BB\" component.
///
/// The args can be used to augment the tuning algorithm.
///  - Use \"OFFSET\" to specify an \"RF\" tuning offset,
///    usually with the intention of moving the LO out of the passband.
///    The offset will be compensated for using the \"BB\" component.
///  - Use the name of a component for the key and a frequency in Hz
///    as the value (any format) to enforce a specific frequency.
///    The other components will be tuned with compensation
///    to achieve the specified overall frequency.
///  - Use the name of a component for the key and the value \"IGNORE\"
///    so that the tuning algorithm will avoid altering the component.
///  - Vendor specific implementations can also use the same args to augment
///    tuning in other ways such as specifying fractional vs integer N tuning.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <param name=\"frequency\">The center frequency in Hz</param>
/// <param name=\"args\">Optional tuner arguments</param>
public";

%csmethodmodifiers SoapySDR::Device::setFrequency(const int, const size_t, const std::string &, const double, const Kwargs &) "
/// <summary>
/// Tune the center frequency of the specified element.
///  - For RX, this specifies the down-conversion frequency.
///  - For TX, this specifies the up-conversion frequency.
///
/// When no args are provided, setFrequency() will tune the \"RF\"
/// component as close as possible to the requested center frequency.
/// Tuning inaccuracies will be compensated for with the \"BB\" component.
///
/// The args can be used to augment the tuning algorithm.
///  - Use \"OFFSET\" to specify an \"RF\" tuning offset,
///    usually with the intention of moving the LO out of the passband.
///    The offset will be compensated for using the \"BB\" component.
///  - Use the name of a component for the key and a frequency in Hz
///    as the value (any format) to enforce a specific frequency.
///    The other components will be tuned with compensation
///    to achieve the specified overall frequency.
///  - Use the name of a component for the key and the value \"IGNORE\"
///    so that the tuning algorithm will avoid altering the component.
///  - Vendor specific implementations can also use the same args to augment
///    tuning in other ways such as specifying fractional vs integer N tuning.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <param name=\"name\">The name of a tunable element</param>
/// <param name=\"frequency\">The center frequency in Hz</param>
/// <param name=\"args\">Optional tuner arguments</param>
public";

%csmethodmodifiers SoapySDR::Device::getFrequency(const int, const size_t) const "
/// <summary>
/// Get the overall center frequency of the chain.
///  - For RX, this specifies the down-conversion frequency.
///  - For TX, this specifies the up-conversion frequency.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <returns>The center frequency in Hz</returns>
public";

%csmethodmodifiers SoapySDR::Device::getFrequency(const int, const size_t, const std::string &) const "
/// <summary>
/// Get the center frequency in a tunable element of the chain.
///  - For RX, this specifies the down-conversion frequency.
///  - For TX, this specifies the up-conversion frequency.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <param name=\"name\">The name of a tunable element</param>
/// <returns>The center frequency in Hz</returns>
public";

%csmethodmodifiers SoapySDR::Device::listFrequencies "
/// <summary>
/// List available tunable elements in the chain, ordered from RF to baseband.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <returns>A list of tunable elements by name</returns>
public";

%csmethodmodifiers SoapySDR::Device::getFrequencyRange(const int, const size_t) const "
/// <summary>
/// Get the range of overall frequency values.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <returns>A list of frequency ranges in Hz</returns>
public";

%csmethodmodifiers SoapySDR::Device::getFrequencyRange(const int, const size_t, const std::string &) const "
/// <summary>
/// Get the range of tunable values for the specified element.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <param name=\"name\">The name of a tunable elements</param>
/// <returns>A list of frequency ranges in Hz</returns>
public";

%csmethodmodifiers SoapySDR::Device::getFrequencyArgsInfo "
/// <summary>
/// Query the argument info description for tune args.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <returns>A list of argument info structs</returns>
public";

%csmethodmodifiers SoapySDR::Device::setSampleRate "
/// <summary>
/// Set the baseband sample rate of the chain.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <param name=\"rate\">The rate in samples per second</param>
public";

%csmethodmodifiers SoapySDR::Device::getSampleRate "
/// <summary>
/// Get the baseband sample rate of the chain.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <returns>The rate in samples per second</returns>
public";

%csmethodmodifiers SoapySDR::Device::getSampleRateRange "
/// <summary>
/// Get the range of possible baseband sample rates.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <returns>A list of sample rates in samples per second</returns>
public";

%csmethodmodifiers SoapySDR::Device::setBandwidth "
/// <summary>
/// Set the baseband filter width of the chain.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <param name=\"bw\">The baseband filter width in Hz</param>
public";

%csmethodmodifiers SoapySDR::Device::getBandwidth "
/// <summary>
/// Get the baseband filter width of the chain.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <returns>The baseband filter width in Hz</returns>
public";

%csmethodmodifiers SoapySDR::Device::getBandwidthRange "
/// <summary>
/// Get the range of possible baseband filter widths.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <returns>A list of bandwidth ranges in Hz</returns>
public";

%csmethodmodifiers SoapySDR::Device::MasterClockRate "
/// <summary>
/// The device's master clock rate in Hz.
/// </summary>
public";

%csmethodmodifiers SoapySDR::Device::MasterClockRates "
/// <summary>
/// A list of available master clock rates in Hz.
/// </summary>
public";

%csmethodmodifiers SoapySDR::Device::ReferenceClockRate "
/// <summary>
/// The device's reference clock rate in Hz.
/// </summary>
public";

%csmethodmodifiers SoapySDR::Device::ReferenceClockRates "
/// <summary>
/// A list of available reference clock rates in Hz.
/// </summary>
public";

%csmethodmodifiers SoapySDR::Device::ClockSource "
/// <summary>
/// The name of the device's clock source.
/// </summary>
public";

%csmethodmodifiers SoapySDR::Device::ClockSources "
/// <summary>
/// A list of the device's available clock sources.
/// </summary>
public";

%csmethodmodifiers SoapySDR::Device::TimeSource "
/// <summary>
/// The name of the device's time source.
/// </summary>
public";

%csmethodmodifiers SoapySDR::Device::TimeSources "
/// <summary>
/// A list of the device's available time sources.
/// </summary>
public";

%csmethodmodifiers SoapySDR::Device::hasHardwareTime "
/// <summary>
/// Does this device have a hardware clock?
/// </summary>
/// <param name=\"what\">An optional clock name</param>
/// <returns>True if the hardware clock exists</returns>
public";

%csmethodmodifiers SoapySDR::Device::setHardwareTime(const long long) "
/// <summary>
/// Write the time to the hardware clock on the device.
/// </summary>
/// <param name=\"timeNs\">The time in nanoseconds</param>
public";

%csmethodmodifiers SoapySDR::Device::setHardwareTime(const long long, const std::string &) "
/// <summary>
/// Write the time to the hardware clock on the device.
/// </summary>
/// <param name=\"timeNs\">The time in nanoseconds</param>
/// <param name=\"what\">Name of a specific time counter</param>
public";

%csmethodmodifiers SoapySDR::Device::getHardwareTime "
/// <summary>
/// Read the time from the hardware clock on the device.
/// </summary>
/// <param name=\"what\">An optional name of a specific time counter</param>
/// <returns>The time in nanoseconds</returns>
public";

%csmethodmodifiers SoapySDR::Device::listSensors() const "
/// <summary>
/// List the available global readback sensors.
///
/// A sensor can represent a reference lock, RSSI, temperature.
/// </summary>
/// <returns>A list of available sensor string names</returns>
public";

%csmethodmodifiers SoapySDR::Device::getSensorInfo(const std::string &) const "
/// <summary>
/// Get meta-information about a global sensor.
///
/// Example: displayable name, type, range.
/// </summary>
/// <returns>Meta-information about a sensor</returns>
public";

%csmethodmodifiers SoapySDR::Device::readSensor(const std::string &) const "
/// <summary>
/// Readback a global sensor given the name.
///
/// The value returned is a string which can represent
/// a boolean (\"true\"/\"false\"), an integer, or float.
/// <param name=\"key\">The ID name of an available sensor</param>
/// <returns>The current value of the sensor</returns>
public";

%csmethodmodifiers SoapySDR::Device::listSensors(const int, const size_t) const "
/// <summary>
/// List the available channel readback sensors.
///
/// A sensor can represent a reference lock, RSSI, temperature.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <returns>A list of available sensor string names</returns>
public";

%csmethodmodifiers SoapySDR::Device::getSensorInfo(const int, const size_t, const std::string &) const "
/// <summary>
/// Get meta-information about a channel sensor.
///
/// Example: displayable name, type, range.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <returns>Meta-information about a sensor</returns>
public";

%csmethodmodifiers SoapySDR::Device::readSensor(const int, const size_t, const std::string &) const "
/// <summary>
/// Readback a channel sensor given the name.
///
/// The value returned is a string which can represent
/// a boolean (\"true\"/\"false\"), an integer, or float.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <param name=\"key\">The ID name of an available sensor</param>
/// <returns>The current value of the sensor</param>
public";

%csmethodmodifiers SoapySDR::Device::RegisterInterfaces "
/// <summary>
/// A list of the device's available register interfaces by name.
/// </summary>
public";

%csmethodmodifiers SoapySDR::Device::writeRegister "
/// <summary>
/// Write a register on the device given the interface name.
///
/// This can represent a register on a soft CPU, FPGA, IC;
/// the interpretation is up the implementation to decide.
/// </summary>
/// <param name=\"name\">The name of a available register interface</param>
/// <param name=\"addr\"The register address</param>
/// <param name=\"value\">The register value</param>
public";

%csmethodmodifiers SoapySDR::Device::readRegister "
/// <summary>
/// Read a register on the device given the interface name.
/// </summary>
/// <param name=\"name\">The name of a available register interface</param>
/// <param name=\"addr\">The register address</param>
/// <returns>The register value</returns>
public";

%csmethodmodifiers SoapySDR::Device::WriteRegisters "
/// <summary>
/// Write a memory block on the device given the interface name.
///
/// This can represent a memory block on a soft CPU, FPGA, IC;
/// the interpretation is up the implementation to decide.
/// </summary>
/// <param name=\"name\">The name of a available memory block interface</param>
/// <param name=\"addr\">The memory block start address</param>
/// <param name=\"value\">The memory block content</param>
public";

%csmethodmodifiers SoapySDR::Device::ReadRegisters "
/// <summary>
/// Read a memory block on the device given the interface name.
/// </summary>
/// <param name=\"name\">The name of a available memory block interface</param>
/// <param name=\"addr\">The memory block start address</param>
/// <param name=\"length\">Number of words to be read from memory block</param>
/// <returns>The memory block content</returns>
public";

%csmethodmodifiers SoapySDR::Device::getSettingInfo() const "
/// <summary>Describe the allowed keys and values used for settings.</summary>
/// <returns>A list of argument info structures</returns>
public";

%csmethodmodifiers SoapySDR::Device::getSettingInfo(const std::string &) const "
/// <summary>Get information on a specific setting.</summary>
/// <param name=\"key\">The setting identifier</param>
/// <returns>All information for a specific setting</returns>
public";

%csmethodmodifiers SoapySDR::Device::writeSetting(const std::string &, const std::string &) "
/// <summary>
/// Write an arbitrary setting on the device.
///
/// The interpretation is up the implementation.
/// </summary>
/// <param name=\"key\">The setting identifier</param>
/// <param name=\"value\">The setting value</param>
public";

%csmethodmodifiers SoapySDR::Device::readSetting(const std::string &) const "
/// <summary>Read an arbitrary setting on the device.</summary>
/// <param name=\"key\">The setting identifier</param>
/// <returns>The setting value</returns>
public";

%csmethodmodifiers SoapySDR::Device::getSettingInfo(const int, const size_t) const "
/// <summary>Describe the allowed keys and values used for channel settings.</summary>
/// <param name=\"direction\">The channel direction (RX or TX)</summary></param>
/// <param name=\"channel\">An available channel on the device</param>
/// <returns>A list of argument info structures</returns>
public";

%csmethodmodifiers SoapySDR::Device::getSettingInfo(const int, const size_t, const std::string &) const "
/// <summary>Get information on a specific channel setting.</summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <param name=\"key\">The setting identifier</param>
/// <returns>All information for a specific channel setting</returns>
public";

%csmethodmodifiers SoapySDR::Device::writeSetting(const int, const size_t, const std::string &, const std::string &) "
/// <summary>
/// Write an arbitrary channel setting on the device.
///
/// The interpretation is up the implementation.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <param name=\"key\">The setting identifier</param>
/// <param name=\"value\">The setting value</param>
public";

%csmethodmodifiers SoapySDR::Device::readSetting(const int, const size_t, const std::string &) const "
/// <summary>Read an arbitrary channel setting on the device.</summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <param name=\"key\">The setting identifier</param>
/// <returns>The setting value</returns>
public";

%csmethodmodifiers SoapySDR::Device::GPIOBanks "
/// <summary>
/// A list of the device's available GPIO banks by name.
/// </summary>
public";

%csmethodmodifiers SoapySDR::Device::writeGPIO(const std::string &, const unsigned) "
/// <summary>Write the value of a GPIO bank.</summary>
/// <param name=\"bank\">The name of an available bank</param>
/// <param name=\"value\">An integer representing GPIO bits</param>
public";

%csmethodmodifiers SoapySDR::Device::writeGPIO(const std::string &, const unsigned, const unsigned) "
/// <summary>Write the value of a GPIO bank with a modification mask.</summary>
/// <param name=\"bank\">The name of an available bank</param>
/// <param name=\"value\">An integer representing GPIO bits</param>
/// <param name=\"mask\">A modification mask where 1 = modify</param>
public";

%csmethodmodifiers SoapySDR::Device::readGPIO "
/// <summary>Readback the value of a GPIO bank.</summary>
/// <param name=\"bank\">The name of an available bank</param>
/// <returns>An integer representing GPIO bits</returns>
public";

%csmethodmodifiers SoapySDR::Device::writeGPIODir(const std::string &, const unsigned) "
/// <summary>
/// Write the data direction of a GPIO bank.
///
/// 1 bits represent outputs, 0 bits represent inputs.
/// </summary>
/// <param name=\"bank\">The name of an available bank</param>
/// <param name=\"dir\">An integer representing data direction bits</param>
public";

%csmethodmodifiers SoapySDR::Device::writeGPIODir(const std::string &, const unsigned, const unsigned) "
/// <summary>
/// Write the data direction of a GPIO bank with a modification mask.
///
/// 1 bits represent outputs, 0 bits represent inputs.
/// </summary>
/// <param name=\"bank\">The name of an available bank</param>
/// <param name=\"dir\">An integer representing data direction bits</param>
/// <param name=\"mask\">A modification mask where 1 = modify</param>
public";

%csmethodmodifiers SoapySDR::Device::readGPIODir "
/// <summary>
/// Read the data direction of a GPIO bank.
///
/// 1 bits represent outputs, 0 bits represent inputs.
/// </summary>
/// <param name=\"bank\">The name of an available bank</param>
/// <returns>An integer representing data direction bits</returns>
public";

%csmethodmodifiers SoapySDR::Device::writeI2C "
/// <summary>
/// Write to an available I2C slave.
///
/// If the device contains multiple I2C masters, the address bits can encode which master.
/// </summary>
/// <param name=\"addr\">The address of the slave</param>
/// <param name=\"data\">An array of bytes to write out</param>
public";

%csmethodmodifiers SoapySDR::Device::readI2C "
/// <summary>
/// Read from an available I2C slave.
///
/// If the device contains multiple I2C masters, the address bits can encode which master.
/// </summary>
/// <param name=\"addr\">The address of the slave</param>
/// <param name=\"numBytes\">The number of bytes to read</param>
/// <returns>An array of bytes read from the slave</returns>
public";

%csmethodmodifiers SoapySDR::Device::transactSPI "
/// <summary>
/// Perform a SPI transaction and return the result.
///
/// It's up to the implementation to set the clock rate,
/// and read edge, and the write edge of the SPI core.
/// SPI slaves without a readback pin will return 0.
///
/// If the device contains multiple SPI masters,
/// the address bits can encode which master.
/// </summary>
/// <param name=\"addr\">An address of an available SPI slave</param>
/// <param name=\"data\">The SPI data, numBits-1 is first out</param>
/// <param name=\"numBits\">The number of bits to clock out</param>
/// <returns>The readback data, numBits-1 is first in</returns>
public";

%csmethodmodifiers SoapySDR::Device::UARTs "
/// <summary>
/// A list of the device's available UART devices by name.
/// </summary>
public";

%csmethodmodifiers SoapySDR::Device::writeUART "
/// <summary>
/// Write data to a UART device.
///
/// It's up to the implementation to set the baud rate, carriage return settings, flushing on newline.
/// </summary>
/// <param name=\"which\">The name of an available UART</param>
/// <param name=\"data\">An array of bytes to write out</param>
public";

%csmethodmodifiers SoapySDR::Device::readUART "
/// <summary>
/// Read bytes from a UART until timeout or newline.
///
/// It's up to the implementation to set the baud rate, carriage return settings, flushing on newline.
/// </summary>
/// <param name=\"which\">The name of an available UART</param>
/// <param name=\"timeoutUs\">Timeout in microseconds</param>
/// <returns>An array of bytes read from the UART</returns>
public";

//
// SoapySDR::Range
//

%typemap(csclassmodifiers) SoapySDR::Range "
/// <summary>A simple min/max numeric range.</summary>
public class";

//
// BuildInfo::SWIGModule
//

%typemap(csclassmodifiers) BuildInfo::SWIGModule "
/// <summary>
/// Version strings for the SoapySDR version used to build the underlying SWIG module.
/// </summary>
public class";

%csattributes BuildInfo::SWIGModule::ABIVersion "
/// <summary>
/// The SoapySDR ABI version the underlying SWIG module was built against.
///
/// On instantiation of a device, this version will be validated against
/// the installed library.
/// </summary>";

%csattributes BuildInfo::SWIGModule::APIVersion "
/// <summary>
/// The SoapySDR API version the underlying SWIG module was built against.
///
/// The format of the version string is <b>major.minor.increment</b>.
/// </summary>";

%csattributes BuildInfo::SWIGModule::LibVersion "
/// <summary>
/// The underlying SoapySDR library version the underlying SWIG module was built against.
///
/// The format of the version string is <b>major.minor.patch-buildInfo</b>.
/// </summary>";

//
// BuildInfo::Runtime
//

%typemap(csclassmodifiers) BuildInfo::Runtime "
/// <summary>
/// Version strings for the SoapySDR build currently installed.
/// </summary>
public class";

%csattributes BuildInfo::Runtime::ABIVersion "
/// <summary>
/// The ABI version of the currently installed SoapySDR library.
///
/// On instantiation of a device, this version will be validated against
/// the assembly.
/// </summary>";

%csattributes BuildInfo::Runtime::APIVersion "
/// <summary>
/// The API version of the currently installed SoapySDR library.
///
/// The format of the version string is <b>major.minor.increment</b>.
/// </summary>";

%csattributes BuildInfo::Runtime::LibVersion "
/// <summary>
/// The library version of the currently installed SoapySDR library.
///
/// The format of the version string is <b>major.minor.patch-buildInfo</b>.
/// </summary>";

//
// SoapySDR::CSharp::Direction
//

%typemap(csclassmodifiers) SoapySDR::CSharp::Direction "
/// <summary>The RF direction to use for a given function.</summary>
public enum";

%csattributes SoapySDR::CSharp::Direction::Tx "/// <summary>Transmit</summary>";
%csattributes SoapySDR::CSharp::Direction::Rx "/// <summary>Receive</summary>";

//
// SoapySDR::CSharp::ErrorCode
//

%typemap(csclassmodifiers) SoapySDR::CSharp::ErrorCode "
/// <summary>Error codes returned from streaming functions.</summary>
public enum";

%csattributes SoapySDR::CSharp::ErrorCode::None "/// <summary>No error.</summary>";
%csattributes SoapySDR::CSharp::ErrorCode::Timeout "/// <summary>Returned when read has a timeout.</summary>";
%csattributes SoapySDR::CSharp::ErrorCode::StreamError "/// <summary>Returned for non-specific stream errors.</summary>";
%csattributes SoapySDR::CSharp::ErrorCode::Corruption "/// <summary>Returned when read has data corruption. For example, the driver saw a malformed packet.</summary>";
%csattributes SoapySDR::CSharp::ErrorCode::Overflow "/// <summary>Returned when read has an overflow condition. For example, and internal buffer has filled.</summary>";
%csattributes SoapySDR::CSharp::ErrorCode::NotSupported "/// <summary>Returned when a requested operation or flag setting is not supported by the underlying implementation.</summary>";
%csattributes SoapySDR::CSharp::ErrorCode::TimeError "/// <summary>Returned when a the device encountered a stream time which was expired (late) or too early to process.</summary>";
%csattributes SoapySDR::CSharp::ErrorCode::Underflow "/// <summary>Returned when write caused an underflow condition. For example, a continuous stream was interrupted.</summary>";

//
// SoapySDR::CSharp::LogLevel
//

%typemap(csclassmodifiers) SoapySDR::CSharp::LogLevel "
/// <summary>The available priority levels for log messages.
///
/// The default log level threshold is Info. Log messages with lower priorities are dropped.</summary>
public enum";