// Copyright (c) 2021-2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

using NUnit.Framework;

[TestFixture]
public class TestBuildInfo
{
    [Test]
    public void Test_BuildInfoStrings()
    {
        Assert.IsNotEmpty(Pothosware.SoapySDR.BuildInfo.Assembly.ABIVersion);
        Assert.IsNotEmpty(Pothosware.SoapySDR.BuildInfo.Assembly.APIVersion);
        Assert.IsNotEmpty(Pothosware.SoapySDR.BuildInfo.Assembly.LibVersion);
        System.Console.WriteLine("Assembly: ABI={0}, API={1}, Lib={2}", Pothosware.SoapySDR.BuildInfo.Assembly.ABIVersion, Pothosware.SoapySDR.BuildInfo.Assembly.APIVersion, Pothosware.SoapySDR.BuildInfo.Assembly.LibVersion);

        Assert.IsNotEmpty(Pothosware.SoapySDR.BuildInfo.SWIGModule.ABIVersion);
        Assert.IsNotEmpty(Pothosware.SoapySDR.BuildInfo.SWIGModule.APIVersion);
        Assert.IsNotEmpty(Pothosware.SoapySDR.BuildInfo.SWIGModule.LibVersion);
        System.Console.WriteLine("SWIGModule: ABI={0}, API={1}, Lib={2}", Pothosware.SoapySDR.BuildInfo.SWIGModule.ABIVersion, Pothosware.SoapySDR.BuildInfo.SWIGModule.APIVersion, Pothosware.SoapySDR.BuildInfo.SWIGModule.LibVersion);

        Assert.IsNotEmpty(Pothosware.SoapySDR.BuildInfo.Runtime.ABIVersion);
        Assert.IsNotEmpty(Pothosware.SoapySDR.BuildInfo.Runtime.APIVersion);
        Assert.IsNotEmpty(Pothosware.SoapySDR.BuildInfo.Runtime.LibVersion);
        System.Console.WriteLine("Runtime: ABI={0}, API={1}, Lib={2}", Pothosware.SoapySDR.BuildInfo.Runtime.ABIVersion, Pothosware.SoapySDR.BuildInfo.Runtime.APIVersion, Pothosware.SoapySDR.BuildInfo.Runtime.LibVersion);
    }

    public static int Main(string[] args) => TestRunner.RunNUnitTest("TestBuildInfo");
}