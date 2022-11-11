// Copyright (c) 2021-2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

using System;
using System.CommandLine;
using System.CommandLine.Invocation;
using System.Linq;
using System.Reflection;

public class SiggenExample
{
    //
    // Utility code
    //

    // https://stackoverflow.com/a/67131017
    private static float[] LinSpace(float startval, float endval, int steps)
    {
        float interval = (endval / Math.Abs(endval)) * Math.Abs(endval - startval) / (steps - 1);
        return (from val in Enumerable.Range(0, steps)
                select startval + (val * interval)).ToArray();
    }

    private static float[] MakeSamples(float ampl, float[] phases)
    {
        var samps = new float[phases.Length * 2]; // Complex interleaved
        for(var i = 0; i < phases.Length; ++i)
        {
            var complex = new System.Numerics.Complex(0, 1) * phases[i] * ampl;
            samps[i * 2] = (float)complex.Real;
            samps[(i * 2) + 1] = (float)complex.Imaginary;
        }

        return samps;
    }

    private static void LogFunction(Pothosware.SoapySDR.LogLevel logLevel, string message)
    {
        System.Console.WriteLine("{0}: {1}: {2}", System.DateTime.Now, logLevel, message);
    }

    //
    // Actual app logic
    //

    private static int SiggenApp(
        string args,
        double rate,
        float ampl,
        double freq,
        double txBandwidth,
        uint txChan,
        double txGain,
        string txAntenna,
        double clockRate,
        double waveFreq,
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
                System.Console.WriteLine("Setting Tx rate to {0} Msps", rate / 1e6);
                sdr.SetSampleRate(Pothosware.SoapySDR.Direction.Tx, txChan, rate);
            }
            System.Console.WriteLine("Actual Tx Rate {0} Msps", sdr.GetSampleRate(Pothosware.SoapySDR.Direction.Tx, txChan) / 1e6);

            if(txBandwidth > 0)
            {
                System.Console.WriteLine("Setting Tx bandwidth to {0} MHz", txBandwidth / 1e6);
                sdr.SetBandwidth(Pothosware.SoapySDR.Direction.Tx, txChan, txBandwidth);
            }
            System.Console.WriteLine("Actual Tx bandwidth: {0} MHz", sdr.GetBandwidth(Pothosware.SoapySDR.Direction.Tx, txChan) / 1e6);

            if(txAntenna.Length > 0)
            {
                System.Console.WriteLine("Setting Tx antenna to {0}", txAntenna);
                sdr.SetAntenna(Pothosware.SoapySDR.Direction.Tx, txChan, txAntenna);
            }
            System.Console.WriteLine("Actual Tx antenna: {0}", sdr.GetAntenna(Pothosware.SoapySDR.Direction.Tx, txChan));

            if(txGain > 0)
            {
                System.Console.WriteLine("Setting Tx gain to {0} dB", txGain);
                sdr.SetGain(Pothosware.SoapySDR.Direction.Tx, txChan, txGain);
            }
            System.Console.WriteLine("Actual Tx gain: {0} dB", sdr.GetGain(Pothosware.SoapySDR.Direction.Tx, txChan));

            if(freq > 0.0)
            {
                System.Console.WriteLine("Tuning the frontend to {0} MHz", freq / 1e6);
                sdr.SetFrequency(Pothosware.SoapySDR.Direction.Tx, txChan, freq);
                System.Console.WriteLine("Actual Tx frequency: {0} MHz", sdr.GetFrequency(Pothosware.SoapySDR.Direction.Tx, txChan) / 1e6);
            }

            var format = Pothosware.SoapySDR.StreamFormat.ComplexFloat32;
            System.Console.WriteLine(string.Format("Create Tx stream (format: {0}, chan: {1})", format, txChan));
            var txStream = sdr.SetupTxStream(format, new uint[] { txChan }, "");
            System.Console.WriteLine("Activate Tx stream");
            txStream.Activate(Pothosware.SoapySDR.StreamFlags.None);

            var mtu = txStream.MTU;
            System.Console.WriteLine("Stream MTU: {0}", mtu);

            var actualWaveFreq = (waveFreq > 0.0) ? waveFreq : (rate / 10.0);

            var phaseAcc = 0.0F;
            var phaseInc = 2.0F * Math.PI * actualWaveFreq / rate;

            uint totalSamps = 0;
            bool running = true;

            // Handle Ctrl+C
            System.Console.CancelKeyPress += delegate (object sender, ConsoleCancelEventArgs e)
            {
                e.Cancel = true;
                running = false;
            };

            var lastPrintTime = System.DateTime.Now;

            System.Console.WriteLine("\nStarting...");
            while (running)
            {
                float phaseAccNext = (float)(phaseAcc + (mtu * phaseInc));
                var phases = LinSpace(phaseAcc, phaseAccNext, (int)mtu);
                var samples = MakeSamples(ampl, phases);

                phaseAcc = phaseAccNext;
                while (phaseAcc > (Math.PI * 2.0F)) phaseAcc -= (float)(Math.PI * 2.0F);

                var streamResult = new Pothosware.SoapySDR.StreamResult();
                var status = txStream.Write(samples, Pothosware.SoapySDR.StreamFlags.None, 0, 1000000, out streamResult);
                if (status != Pothosware.SoapySDR.ErrorCode.None)
                {
                    throw new ApplicationException(string.Format("Write returned: {0}", status));
                }
                else if (streamResult.NumSamples != mtu)
                {
                    throw new ApplicationException(string.Format("Write consumed {0} elements, expected {1}", streamResult.NumSamples, mtu));
                }

                totalSamps += streamResult.NumSamples;

                // Print rate every 5 seconds
                var now = System.DateTime.Now;
                var durationSecs = (System.DateTime.Now - lastPrintTime).TotalSeconds;
                if (durationSecs > 5)
                {
                    var currentRate = totalSamps / durationSecs / 1e6;
                    System.Console.WriteLine("C# siggen rate: {0} Msps", currentRate);

                    totalSamps = 0;
                    lastPrintTime = System.DateTime.Now;
                }
            }

            // Executed after Ctrl+C
            System.Console.WriteLine("Clean up stream");
            txStream.Deactivate();
            txStream.Close();
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
        var rootCmd = new RootCommand() { TreatUnmatchedTokensAsErrors = true }.WithHandler(nameof(SiggenApp));
        rootCmd.Description = "A simple signal generator example to demonstrate the SoapySDR C# API.\n\nNote: " +
                              "(REQUIRED) in the option descriptions below refers to a value being necessary " +
                              "for the option, not the option itself being required.";

        rootCmd.AddOption(new Option<string>("--args", description: "Device args") { IsRequired = true });
        rootCmd.AddOption(new Option<double>("--rate", description: "Tx sample rate (samples/sec)", getDefaultValue: () => 1e6) { IsRequired = true });
        rootCmd.AddOption(new Option<double>("--ampl", description: "Optional Tx digital amplitude rate (0.0-1.0)", getDefaultValue: () => 0.7) { IsRequired = true });
        rootCmd.AddOption(new Option<double>("--freq", description: "Tx frequency (Hz)") { IsRequired = true });
        rootCmd.AddOption(new Option<double>("--bw", description: "Optional Tx filter bandwidth (Hz)", getDefaultValue: () => 5e6) { IsRequired = true });
        rootCmd.AddOption(new Option<uint>("--chan", description: "Optional Tx channel", getDefaultValue: () => 0U) { IsRequired = true });
        rootCmd.AddOption(new Option<double>("--gain", description: "Optional Tx gain", getDefaultValue: () => 0.0) { IsRequired = true });
        rootCmd.AddOption(new Option<string>("--ant", description: "Optional Tx antenna", getDefaultValue: () => "") { IsRequired = true });
        rootCmd.AddOption(new Option<double>("--clock-rate", description: "Optional Tx clock rate (Hz)", getDefaultValue: () => 0.0) { IsRequired = true });
        rootCmd.AddOption(new Option<double>("--wave-freq", description: "Optional baseband waveform freq (Hz), actually defaults to (rate / 10)", getDefaultValue: () => 0.0));
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
        var method = typeof(SiggenExample).GetMethod(methodName, BindingFlags.NonPublic | BindingFlags.Static);
        var handler = CommandHandler.Create(method);
        command.Handler = handler;
        return command;
    }
}