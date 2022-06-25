// Copyright (c) 2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

package Pothosware.SoapySDR;

import static org.junit.jupiter.api.Assertions.*;
import org.junit.jupiter.api.*;
import org.junit.jupiter.params.*;
import org.junit.jupiter.params.provider.*;

import org.apache.commons.math3.complex.*;

import java.util.*;

// For the null device, functions generally don't do anything or return some
// hardcoded value, but we can still make sure functions compile and run as
// expected.

public class DeviceAPITest
{
    // Make sure native libraries are loaded.
    static
    {
        try { JARUtils.initialize(); }
        catch(Exception e) {}
    }

    private void testDeviceKeys(Device device)
    {
        assertEquals("null", device.getDriverKey());
        assertEquals("null", device.getHardwareKey());
        assertEquals("null:null", device.toString());
    }

    private Device getTestDevice()
    {
        // Make sure either method works.
        testDeviceKeys(new Device("driver=null,type=null"));

        var args = new HashMap<String, String>();
        args.put("driver", "null");
        args.put("type", "null");

        var device = new Device(args);
        testDeviceKeys(device);

        return device;
    }

    @Test
    public void testDeviceGeneralAPI()
    {
        var device = getTestDevice();

        //
        // Clocking API
        //

        device.setMasterClockRate(0.0);
        assertEquals(0.0, device.getMasterClockRate());
        var masterClockRates = device.getMasterClockRates();

        device.setReferenceClockRate(0.0);
        assertEquals(0.0, device.getReferenceClockRate());
        var referenceClockRates = device.getReferenceClockRates();

        device.setClockSource("");
        assertEquals("", device.getClockSource());
        var clockSources = device.listClockSources();

        //
        // Time API
        //

        device.setTimeSource("");
        assertEquals("", device.getTimeSource());
        var timeSources = device.listTimeSources();

        var hasTime = device.hasHardwareTime("");
        assertEquals(0, device.getHardwareTime(""));
        device.setHardwareTime(0);
        device.setHardwareTime(0, "");

        //
        // Sensor API (TODO: non-string returns when implemented)
        //

        var sensors = device.listSensors();
        var sensorInfo = device.getSensorInfo("");

        var sensorValue = device.readSensor("");

        //
        // Register API
        //

        var registerInterfaces = device.listRegisterInterfaces();

        device.writeRegister("", 0, 0);
        assertEquals(0, device.readRegister("", 0));

        // TODO: writeRegisters, readRegisters

        //
        // Settings API (TODO: non-string returns when implemented)
        //

        var allSettingInfo = device.getSettingInfo();
        var settingInfo = device.getSettingInfo("");

        device.writeSetting("", 0);
        device.writeSetting("", 0.0);
        device.writeSetting("", false);
        device.writeSetting("", "");

        var settingValue = device.readSetting("");

        //
        // GPIO API
        //

        var gpioBanks = device.listGPIOBanks();

        device.writeGPIO("", 0);
        device.writeGPIO("", 0, 0);
        assertEquals(0, device.readGPIO(""));

        device.writeGPIODir("", 0);
        device.writeGPIODir("", 0, 0);
        assertEquals(0, device.readGPIODir(""));

        //
        // I2C API
        //

        device.writeI2C(0, "");
        assertEquals("", device.readI2C(0, 0));

        //
        // SPI API
        //

        assertEquals(0, device.transactSPI(0, 0, 0));

        //
        // UART API
        //

        var uarts = device.listUARTs();
        device.writeUART("", "");
        assertEquals("", device.readUART("", 1000));
    }

    private void testDeviceNonStreamingDirectionFunctions(Direction direction)
    {
        var device = getTestDevice();

        //
        // Channels API
        //

        device.setFrontendMapping(direction, "");
        var frontendMapping = device.getFrontendMapping(direction);
        var numChannels = device.getNumChannels(direction);
        var channelInfo = device.getChannelInfo(direction, 0);
        var fullDuplex = device.getFullDuplex(direction, 0);

        //
        // Stream API
        //

        var streamFormats = device.getStreamFormats(direction, 0);

        var nativeStreamFormat = device.getNativeStreamFormat(direction, 0);
        assertEquals(StreamFormat.CS16, nativeStreamFormat.getFormat());
        assertEquals(1 << 15, nativeStreamFormat.getFullScale());

        var streamArgsInfo = device.getStreamArgsInfo(direction, 0);

        //
        // Antenna API
        //

        var antennas = device.listAntennas(direction, 0);
        var antenna = device.getAntenna(direction, 0);
        device.setAntenna(direction, 0, "ANT");

        //
        // Frontend corrections API
        //

        var hasDCOffsetMode = device.hasDCOffsetMode(direction, 0);
        var dcOffsetMode = device.getDCOffsetMode(direction, 0);
        device.setDCOffsetMode(direction, 0, true);

        var hasDCOffset = device.hasDCOffset(direction, 0);
        var dcOffset = device.getDCOffset(direction, 0);
        device.setDCOffset(direction, 0, new Complex(1.0, 0.0));

        var hasIQBalanceMode = device.hasIQBalanceMode(direction, 0);
        var iqBalanceMode = device.getIQBalanceMode(direction, 0);
        device.setIQBalanceMode(direction, 0, true);

        var hasIQBalance = device.hasIQBalance(direction, 0);
        var iqBalance = device.getIQBalance(direction, 0);
        device.setIQBalance(direction, 0, new Complex(1.0, 0.0));

        var hasFrequencyCorrection = device.hasFrequencyCorrection(direction, 0);
        var getFrequencyCorrection = device.getFrequencyCorrection(direction, 0);
        device.setFrequencyCorrection(direction, 0, 0.0);

        //
        // Gain API
        //

        var gains = device.listGains(direction, 0);

        var hasGainMode = device.hasGainMode(direction, 0);
        var gainMode = device.getGainMode(direction, 0);
        device.setGainMode(direction, 0, true);

        var gain = device.getGain(direction, 0);
        var gainElement = device.getGain(direction, 0, "");
        device.setGain(direction, 0, 0.0);
        device.setGain(direction, 0, "", 0.0);

        var gainRange = device.getGainRange(direction, 0);
        var channelGainRange = device.getGainRange(direction, 0, "");

        //
        // Frequency API
        //

        var frequencyArgsString = "key=val0,key1=val1";

        var frequencyArgsMap = new HashMap<String, String>();
        frequencyArgsMap.put("key0", "val0");
        frequencyArgsMap.put("key1", "val1");

        device.setFrequency(direction, 0, 0.0);
        device.setFrequency(direction, 0, 0.0, frequencyArgsString);
        device.setFrequency(direction, 0, 0.0, frequencyArgsMap);
        var frequency = device.getFrequency(direction, 0);

        device.setFrequency(direction, 0, "", 0.0);
        device.setFrequency(direction, 0, "", 0.0, frequencyArgsString);
        device.setFrequency(direction, 0, "", 0.0, frequencyArgsMap);
        var frequencyComponent = device.getFrequency(direction, 0, "");

        var frequencies = device.listFrequencies(direction, 0);

        var frequencyRange = device.getFrequencyRange(direction, 0);
        var frequencyComponentRange = device.getFrequencyRange(direction, 0, "");
        var frequencyArgsInfo = device.getFrequencyArgsInfo(direction, 0);

        //
        // Sample rate API
        //

        device.setSampleRate(direction, 0, 0.0);
        var sampleRate = device.getSampleRate(direction, 0);
        var sampleRateRange = device.getSampleRateRange(direction, 0);

        //
        // Bandwidth API
        //

        device.setBandwidth(direction, 0, 0.0);
        var bandwidth = device.getBandwidth(direction, 0);
        var bandwidthRange = device.getBandwidthRange(direction, 0);

        //
        // Sensor API (TODO: non-string returns when implemented)
        //

        var sensors = device.listSensors(direction, 0);

        var sensorValue = device.readSensor(direction, 0, "");

        //
        // Settings API (TODO: non-string returns when implemented)
        //

        var allSettingInfo = device.getSettingInfo(direction, 0);
        var settingInfo = device.getSettingInfo(direction, 0, "");

        device.writeSetting(direction, 0, "", 0);
        device.writeSetting(direction, 0, "", 0.0);
        device.writeSetting(direction, 0, "", false);
        device.writeSetting(direction, 0, "", "");

        var settingValue = device.readSetting(direction, 0, "");
    }

    // Workaround for parameterized tests not working for some reason
    @Test
    public void testDeviceTxNonStreamingDirectionFunctions()
    {
        testDeviceNonStreamingDirectionFunctions(Direction.TX);
    }

    // Workaround for parameterized tests not working for some reason
    @Test
    public void testDeviceRxNonStreamingDirectionFunctions()
    {
        testDeviceNonStreamingDirectionFunctions(Direction.RX);
    }
}
