// Copyright (c) 2021-2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

using System;
using System.CommandLine;
using System.CommandLine.Invocation;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;

public class RxSamplesToFileExample
{
    //
    // Utility code
    //

    private static void AppendData(FileInfo file, byte[] buffer, int index, int count)
    {
        using (var fileStream = new FileStream(file.FullName, FileMode.Append, FileAccess.Write, FileShare.None))
        using (var bw = new BinaryWriter(fileStream))
        {
            bw.Write(buffer, index, count);
        }
    }

    private static void LogFunction(Pothosware.SoapySDR.LogLevel logLevel, string message)
    {
        System.Console.WriteLine("{0}: {1}: {2}", System.DateTime.Now, logLevel, message);
    }

    //
    // Actual app logic
    //

    private static int RxSamplesToFileApp(
        string args,
        FileInfo file,
        uint numSamps,
        double rate,
        double freq,
        double bw,
        uint chan,
        double gain,
        string antenna,
        double clockRate,
        bool debug)
    {
        try
        {
            Pothosware.SoapySDR.Logger.RegisterLogHandler(LogFunction);
            Pothosware.SoapySDR.Logger.LogLevel = debug ? Pothosware.SoapySDR.LogLevel.Debug : Pothosware.SoapySDR.LogLevel.Warning;

            System.Console.WriteLine("Creating SDR with args: \"{0}\"", args);
            var sdr = new Pothosware.SoapySDR.Device(args);
            System.Console.WriteLine("Found {0}", sdr);

            if(clockRate > 0)
            {
                System.Console.WriteLine("Setting master clock rate to {0} MHz", clockRate / 1e6);
                sdr.MasterClockRate = clockRate;
            }
            System.Console.WriteLine("Actual master clock rate: {0} MHz", sdr.MasterClockRate / 1e6);

            if(rate > 0)
            {
                System.Console.WriteLine("Setting Rx rate to {0} Msps", rate / 1e6);
                sdr.SetSampleRate(Pothosware.SoapySDR.Direction.Rx, chan, rate);
            }
            System.Console.WriteLine("Actual Rx Rate {0} Msps", sdr.GetSampleRate(Pothosware.SoapySDR.Direction.Rx, chan) / 1e6);

            if(bw > 0)
            {
                System.Console.WriteLine("Setting Rx bandwidth to {0} MHz", bw / 1e6);
                sdr.SetBandwidth(Pothosware.SoapySDR.Direction.Rx, chan, bw);
            }
            System.Console.WriteLine("Actual Rx bandwidth: {0} MHz", sdr.GetBandwidth(Pothosware.SoapySDR.Direction.Rx, chan) / 1e6);

            if(antenna.Length > 0)
            {
                System.Console.WriteLine("Setting Rx antenna to {0}", antenna);
                sdr.SetAntenna(Pothosware.SoapySDR.Direction.Rx, chan, antenna);
            }
            System.Console.WriteLine("Actual Rx antenna: {0}", sdr.GetAntenna(Pothosware.SoapySDR.Direction.Rx, chan));

            if(gain > 0)
            {
                System.Console.WriteLine("Setting Rx gain to {0} dB", gain);
                sdr.SetGain(Pothosware.SoapySDR.Direction.Rx, chan, gain);
            }
            System.Console.WriteLine("Actual Rx gain: {0} dB", sdr.GetGain(Pothosware.SoapySDR.Direction.Rx, chan));

            if(freq > 0.0)
            {
                System.Console.WriteLine("Tuning the frontend to {0} MHz", freq / 1e6);
                sdr.SetFrequency(Pothosware.SoapySDR.Direction.Rx, chan, freq);
                System.Console.WriteLine("Actual Rx frequency: {0} MHz", sdr.GetFrequency(Pothosware.SoapySDR.Direction.Rx, chan) / 1e6);
            }

            var format = Pothosware.SoapySDR.StreamFormat.ComplexFloat32;
            var formatSize = Pothosware.SoapySDR.StreamFormat.FormatToSize(format);

            System.Console.WriteLine(string.Format("Create Rx stream (format: {0}, chan: {1})", format, chan));
            var rxStream = sdr.SetupRxStream(format, new uint[] { chan }, "");
            System.Console.WriteLine("Activate Rx stream");
            rxStream.Activate(Pothosware.SoapySDR.StreamFlags.None);

            var mtu = rxStream.MTU;
            System.Console.WriteLine("Stream MTU: {0}", mtu);

            // Create as buffer for ease of writing, but we can use System.Span to use this
            // memory with the appropriate type.
            byte[] buffer = new byte[mtu * formatSize];
            var floatSpan = MemoryMarshal.Cast<byte, float>(new Span<byte>(buffer));

            uint totalSamps = 0;

            while(totalSamps < numSamps)
            {
                var expectedSamps = Math.Min(mtu, (numSamps - totalSamps));

                var bufferSlice = floatSpan.Slice((int)totalSamps, (int)expectedSamps);

                var streamFlags = Pothosware.SoapySDR.StreamFlags.None;
                if ((totalSamps + expectedSamps) == numSamps) streamFlags |= Pothosware.SoapySDR.StreamFlags.EndBurst;

                var streamResult = new Pothosware.SoapySDR.StreamResult();
                var status = rxStream.Read(bufferSlice, 100000, out streamResult);
                if(status != Pothosware.SoapySDR.ErrorCode.None)
                {
                    throw new ApplicationException(string.Format("Read returned: {0}", status));
                }
                else if (streamResult.NumSamples != expectedSamps)
                {
                    throw new ApplicationException(string.Format("Read returned {0} elements, expected {1}", streamResult.NumSamples, expectedSamps));
                }

                AppendData(file, buffer, (int)(totalSamps * formatSize), (int)(expectedSamps * formatSize));

                totalSamps += streamResult.NumSamples;
            }

            // Executed after Ctrl+C
            System.Console.WriteLine("Clean up stream");
            rxStream.Deactivate();
            rxStream.Close();
            System.Console.WriteLine("Done!");

            return 0;
        }
        catch(Exception ex)
        {
            System.Console.Error.WriteLine(ex);
            return -1;
        }
    }

    public static int Main(string[] args)
    {
        /*
         * Unintuitively, "IsRequired = true" refers to whether a value is required when the option is passed in,
         * *not* if the option itself is required. Options themselves become required if there is no default value.
         * Since these types can't be nullable in the C# standard we're targeting, we'll just need to use a default
         * value and check for it in the function itself.
         */
        var rootCmd = new RootCommand() { TreatUnmatchedTokensAsErrors = true }.WithHandler(nameof(RxSamplesToFileApp));
        rootCmd.Description = "A simple example that reads samples and writes to file to demonstrate the SoapySDR C# API.\n\nNote: " +
                              "(REQUIRED) in the option descriptions below refers to a value being necessary " +
                              "for the option, not the option itself being required.";

        rootCmd.AddOption(new Option<string>("--args", description: "Device args") { IsRequired = true });
        rootCmd.AddOption(new Option<FileInfo>("--output", description: "Output filepath") { IsRequired = true });
        rootCmd.AddOption(new Option<uint>("--num-samps", description: "Number of samples to write to file") { IsRequired = true });
        rootCmd.AddOption(new Option<double>("--rate", description: "Rx sample rate (samples/sec)", getDefaultValue: () => 1e6) { IsRequired = true });
        rootCmd.AddOption(new Option<double>("--freq", description: "Rx frequency (Hz)") { IsRequired = true });
        rootCmd.AddOption(new Option<double>("--bw", description: "Optional Rx filter bandwidth (Hz)", getDefaultValue: () => 5e6) { IsRequired = true });
        rootCmd.AddOption(new Option<uint>("--chan", description: "Optional Rx channel", getDefaultValue: () => 0U) { IsRequired = true });
        rootCmd.AddOption(new Option<double>("--gain", description: "Optional Rx gain", getDefaultValue: () => 0.0) { IsRequired = true });
        rootCmd.AddOption(new Option<string>("--ant", description: "Optional Rx antenna", getDefaultValue: () => "") { IsRequired = true });
        rootCmd.AddOption(new Option<double>("--clock-rate", description: "Optional Rx clock rate (Hz)", getDefaultValue: () => 0.0) { IsRequired = true });
        rootCmd.AddOption(new Option<bool>("--debug", description: "Log debug info", getDefaultValue: () => false) { IsRequired = true });

        // Note: exceptions must be caught in the function itself, or System.CommandLine will intercept it and print
        // the entire stacktrace, which includes a bunch of dynamic invoking stuff we don't care about.
        return rootCmd.Invoke(args);
    }
}

//
// Convenient System.CommandLine extensions
//

static class CommandLineHelpers
{
    public static RootCommand WithHandler(this RootCommand command, string methodName)
    {
        var method = typeof(RxSamplesToFileExample).GetMethod(methodName, BindingFlags.NonPublic | BindingFlags.Static);
        var handler = CommandHandler.Create(method);
        command.Handler = handler;
        return command;
    }
}