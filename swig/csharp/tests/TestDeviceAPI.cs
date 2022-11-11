// Copyright (c) 2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

using System.Collections.Generic;

using NUnit.Framework;

// For the null device, functions generally don't do anything or return some
// hardcoded value, but we can still make sure functions compile and run as
// expected, especially for the added C# fanciness.

[TestFixture]
public class TestDeviceAPI
{
    private void TestDeviceKeys(Pothosware.SoapySDR.Device device)
    {
        Assert.AreEqual("null", device.DriverKey);
        Assert.AreEqual("null", device.HardwareKey);
        Assert.AreEqual("null:null", device.ToString());
    }

    private Pothosware.SoapySDR.Device GetTestDevice()
    {
        // Make sure either method works.
        TestDeviceKeys(new Pothosware.SoapySDR.Device("driver=null,type=null"));

        var args = new Dictionary<string, string>();
        args.Add("driver", "null");
        args.Add("type", "null");

        var device = new Pothosware.SoapySDR.Device(args);
        TestDeviceKeys(device);

        return device;
    }

    [Test]
    public void Test_DeviceGeneralFunctions()
    {
        var device = GetTestDevice();

        //
        // Clocking API
        //

        device.MasterClockRate = 0.0;
        Assert.AreEqual(0.0, device.MasterClockRate);
        _ = device.MasterClockRates;

        device.ReferenceClockRate = 0.0;
        Assert.AreEqual(0.0, device.ReferenceClockRate);
        _ = device.ReferenceClockRates;

        device.ClockSource = "";
        Assert.AreEqual("", device.ClockSource);
        _ = device.ClockSources;

        //
        // Time API
        //

        device.TimeSource = "";
        Assert.AreEqual("", device.TimeSource);
        _ = device.TimeSources;

        _ = device.HasHardwareTime("");
        Assert.AreEqual(0, device.GetHardwareTime(""));
        device.SetHardwareTime(0);
        device.SetHardwareTime(0, "");

        //
        // Sensor API
        //

        _ = device.ListSensors();
        _ = device.GetSensorInfo("");

        _ = device.ReadSensor("");
        _ = device.ReadSensor<bool>("");
        /*
        _ = device.ReadSensor<short>("");
        _ = device.ReadSensor<int>("");
        _ = device.ReadSensor<long>("");
        _ = device.ReadSensor<ushort>("");
        _ = device.ReadSensor<uint>("");
        _ = device.ReadSensor<ulong>("");
        _ = device.ReadSensor<float>("");
        _ = device.ReadSensor<double>("");
        */
        _ = device.ReadSensor<string>("");

        //
        // Register API
        //

        _ = device.RegisterInterfaces;

        device.WriteRegister("", 0, 0);
        Assert.AreEqual(0, device.ReadRegister("", 0));

        device.WriteRegisters("", 0, new uint[] { 0, 0, 0 });
        _ = device.ReadRegisters("", 0, 1);

        //
        // Settings API
        //

        _ = device.GetSettingInfo();
        _ = device.GetSettingInfo("");

        device.WriteSetting("", 0);
        device.WriteSetting("", 0.0);
        device.WriteSetting("", false);
        device.WriteSetting("", "");

        /*
        _ = device.ReadSetting("");
        _ = device.ReadSetting<bool>("");
        
        _ = device.ReadSetting<short>("");
        _ = device.ReadSetting<int>("");
        _ = device.ReadSetting<long>("");
        _ = device.ReadSetting<ushort>("");
        _ = device.ReadSetting<uint>("");
        _ = device.ReadSetting<ulong>("");
        _ = device.ReadSetting<float>("");
        _ = device.ReadSetting<double>("");
        
        _ = device.ReadSetting<string>("");
        */

        //
        // GPIO API
        //

        _ = device.GPIOBanks;

        device.WriteGPIO("", 0);
        device.WriteGPIO("", 0, 0);
        Assert.AreEqual(0, device.ReadGPIO(""));

        device.WriteGPIODir("", 0);
        device.WriteGPIODir("", 0, 0);
        Assert.AreEqual(0, device.ReadGPIODir(""));

        //
        // I2C API
        //

        device.WriteI2C(0, "");
        Assert.AreEqual("", device.ReadI2C(0, 0));

        //
        // SPI API
        //

        Assert.AreEqual(0, device.TransactSPI(0, 0, 0));

        //
        // UART API
        //

        _ = device.UARTs;
        device.WriteUART("", "");
        Assert.AreEqual("", device.ReadUART("", 1000));
    }

    [Test]
    [TestCase(Pothosware.SoapySDR.Direction.Rx)]
    [TestCase(Pothosware.SoapySDR.Direction.Tx)]
    public void Test_DeviceNonStreamingDirectionFunctions(Pothosware.SoapySDR.Direction direction)
    {
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
        /*
        _ = device.ReadSensor<short>(direction, 0, "");
        _ = device.ReadSensor<int>(direction, 0, "");
        _ = device.ReadSensor<long>(direction, 0, "");
        _ = device.ReadSensor<ushort>(direction, 0, "");
        _ = device.ReadSensor<uint>(direction, 0, "");
        _ = device.ReadSensor<ulong>(direction, 0, "");
        _ = device.ReadSensor<float>(direction, 0, "");
        _ = device.ReadSensor<double>(direction, 0, "");
        */
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

    public static int Main(string[] args) => TestRunner.RunNUnitTest("TestDeviceAPI");
}