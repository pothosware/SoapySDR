// Copyright (c) 2021 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

//
// TODO: automate this with SWIG doc generator when brought in
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
/// <returns>A list of allowed format strings. See SoapySDR.StreamFormat.</returns>
public";

%csmethodmodifiers SoapySDR::Device::getNativeStreamFormat "
/// <summary>
/// Get the hardware's native stream format for this channel.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <param name=\"fullScale\">The maximum possible value (output)</param>
/// <returns>The native stream format string. See SoapySDR.StreamFormat.</returns>
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
/// Does the device support frontend frequency correction correction?
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <returns>True if correction is supported</returns>
public";

%csmethodmodifiers SoapySDR::Device::setFrequencyCorrection "
/// <summary>
/// Set the frontend frequency correction correction.
/// </summary>
/// <param name=\"direction\">The channel direction (RX or TX)</param>
/// <param name=\"channel\">An available channel on the device</param>
/// <param name=\"value\">The correction in PPM</param>
public";

%csmethodmodifiers SoapySDR::Device::getFrequencyCorrection "
/// <summary>
/// Get the frontend frequency correction correction.
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

%csmethodmodifiers SoapySDR::Device::setHardwareTime "
/// <summary>
/// Write the time to the hardware clock on the device.
/// </summary>
/// <param name=\"timeNs\">The time in nanoseconds</param>
/// <param name=\"what\">An optional name of a specific time counter</param>
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
/// <returns>The current value of the sensor</param>
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

%csmethodmodifiers SoapySDR::Device::GPIOBanks "
/// <summary>
/// A list of the device's available GPIO banks by name.
/// </summary>
public";

%csmethodmodifiers SoapySDR::Device::UARTs "
/// <summary>
/// A list of the device's available UART devices by name.
/// </summary>
public";