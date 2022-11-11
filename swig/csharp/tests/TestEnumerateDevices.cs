// Copyright (c) 2021 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

using System.Collections.Generic;
using System.Linq;

using NUnit.Framework;

[TestFixture]
public class TestEnumerateDevices
{
    static private bool HasNullDevice(Pothosware.SoapySDR.KwargsList devices)
        => devices.Select(args => args.ContainsKey("driver") && (args["driver"] == "null")).Any();

    [Test]
    public void Test_EnumerateNoParam()
    {
        // We can't guarantee the number of devices connected to the machine, so just
        // make sure this doesn't error out.
        _ = Pothosware.SoapySDR.Device.Enumerate();
    }

    [Test]
    public void Test_EnumerateStringParam()
    {
        var args = "driver=null,type=null";
        Assert.IsTrue(HasNullDevice(Pothosware.SoapySDR.Device.Enumerate(args)));
    }

    [Test]
    public void Test_EnumerateDictParam()
    {
        // Arbitrarily use some non-standard IDictionary subclass to test the
        // interface parameter.
        var args = new SortedList<string, string>();
        args.Add("driver", "null");
        args.Add("type", "null");

        Assert.IsTrue(HasNullDevice(Pothosware.SoapySDR.Device.Enumerate(args)));
    }

    public static int Main(string[] args) => TestRunner.RunNUnitTest("TestEnumerateDevices");
}