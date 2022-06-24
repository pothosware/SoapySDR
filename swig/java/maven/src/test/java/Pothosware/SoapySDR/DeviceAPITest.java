// Copyright (c) 2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

package Pothosware.SoapySDR;

import static org.junit.jupiter.api.Assertions.*;
import org.junit.jupiter.api.*;
import org.junit.jupiter.params.*;
import org.junit.jupiter.params.provider.*;

import java.util.*;

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

        var value = device.readSensor("");

        //
        // Register API
        //

        var registerInterfaces = device.listRegisterInterfaces();

        device.writeRegister("", 0, 0);
        assertEquals(0, device.readRegister("", 0));

        // TODO: writeRegisters, readRegisters

        //
        // Settings API
        //

        var allSettingInfo = device.getSettingInfo();
        var settingInfo = device.getSettingInfo("");

        device.writeSetting("", 0);
        device.writeSetting("", 0.0);
        device.writeSetting("", false);
        device.writeSetting("", "");

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

    @ParameterizedTest
    @EnumSource(Direction.class)
    void testDeviceNonStreamingDirectionFunctions(Direction direction)
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

        /*
         *
        var device = GetTestDevice();

        //
        // Channels API
        //

        device.SetFrontendMapping(direction, "");
        _ = device.GetFrontendMapping(direction);
        _ = device.GetNumChannels(direction);
        _ = device.GetChannelInfo(direction, 0);
        _ = device.GetFullDuplex(direction, 0);

        //
        // Stream API
        //

        _ = device.GetStreamFormats(direction, 0);

        double fullScale;
        Assert.AreEqual(Pothosware.SoapySDR.StreamFormat.ComplexInt16, device.GetNativeStreamFormat(direction, 0, out fullScale));
        Assert.AreEqual(1 << 15, fullScale);

        _ = device.GetStreamArgsInfo(direction, 0);

        //
        // Antenna API
        //

        _ = device.ListAntennas(direction, 0);
        _ = device.GetAntenna(direction, 0);
        device.SetAntenna(direction, 0, "ANT");

        //
        // Frontend corrections API
        //

        _ = device.HasDCOffsetMode(direction, 0);
        _ = device.GetDCOffsetMode(direction, 0);
        device.SetDCOffsetMode(direction, 0, true);

        _ = device.HasDCOffset(direction, 0);
        _ = device.GetDCOffset(direction, 0);
        device.SetDCOffset(direction, 0, new System.Numerics.Complex(1.0, 0.0));

        _ = device.HasIQBalanceMode(direction, 0);
        _ = device.GetIQBalanceMode(direction, 0);
        device.SetIQBalanceMode(direction, 0, true);

        _ = device.HasIQBalance(direction, 0);
        _ = device.GetIQBalance(direction, 0);
        device.SetIQBalance(direction, 0, new System.Numerics.Complex(1.0, 0.0));

        _ = device.HasFrequencyCorrection(direction, 0);
        _ = device.GetFrequencyCorrection(direction, 0);
        device.SetFrequencyCorrection(direction, 0, 0.0);

        //
        // Gain API
        //

        _ = device.ListGains(direction, 0);

        _ = device.HasGainMode(direction, 0);
        _ = device.GetGainMode(direction, 0);
        device.SetGainMode(direction, 0, true);

        _ = device.GetGain(direction, 0);
        _ = device.GetGain(direction, 0, "");
        device.SetGain(direction, 0, 0.0);
        device.SetGain(direction, 0, "", 0.0);

        _ = device.GetGainRange(direction, 0);
        _ = device.GetGainRange(direction, 0, "");

        //
        // Frequency API
        //

        var frequencyArgsString = "key0=val0,key1=val1";

        var frequencyArgsMap = new Dictionary<string, string>();
        frequencyArgsMap["key0"] = "val0";
        frequencyArgsMap["key1"] = "val1";

        device.SetFrequency(direction, 0, 0.0);
        device.SetFrequency(direction, 0, 0.0, frequencyArgsString);
        device.SetFrequency(direction, 0, 0.0, frequencyArgsMap);
        _ = device.GetFrequency(direction, 0);

        device.SetFrequency(direction, 0, "", 0.0);
        device.SetFrequency(direction, 0, "", 0.0, frequencyArgsString);
        device.SetFrequency(direction, 0, "", 0.0, frequencyArgsMap);
        _ = device.GetFrequency(direction, 0, "");

        _ = device.ListFrequencies(direction, 0);

        _ = device.GetFrequencyRange(direction, 0);
        _ = device.GetFrequencyRange(direction, 0, "");
        _ = device.GetFrequencyArgsInfo(direction, 0);

        //
        // Sample rate API
        //

        device.SetSampleRate(direction, 0, 0.0);
        _ = device.GetSampleRate(direction, 0);
        _ = device.GetSampleRateRange(direction, 0);

        //
        // Bandwidth API
        //

        device.SetBandwidth(direction, 0, 0.0);
        _ = device.GetBandwidth(direction, 0);
        _ = device.GetBandwidthRange(direction, 0);

        //
        // Sensor API
        //

        _ = device.ListSensors(direction, 0);

        _ = device.ReadSensor(direction, 0, "");
        _ = device.ReadSensor<bool>(direction, 0, "");
        _ = device.ReadSensor<string>(direction, 0, "");

        //
        // Settings API
        //

        _ = device.GetSettingInfo(direction, 0);
        _ = device.GetSettingInfo(direction, 0, "");

        device.WriteSetting("", 0);
        device.WriteSetting("", 0.0);
        device.WriteSetting("", false);
        device.WriteSetting("", "");

        /*
        _ = device.ReadSetting(direction, 0, "");
        _ = device.ReadSetting<bool>(direction, 0, "");

        _ = device.ReadSetting<short>(direction, 0, "");
        _ = device.ReadSetting<int>(direction, 0, "");
        _ = device.ReadSetting<long>(direction, 0, "");
        _ = device.ReadSetting<ushort>(direction, 0, "");
        _ = device.ReadSetting<uint>(direction, 0, "");
        _ = device.ReadSetting<ulong>(direction, 0, "");
        _ = device.ReadSetting<float>(direction, 0, "");
        _ = device.ReadSetting<double>(direction, 0, "");

        _ = device.ReadSetting<string>(direction, 0, "");
         */
    }
}
