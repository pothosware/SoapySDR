// Copyright (c) 2021 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

using NUnit.Framework;

[TestFixture]
public class TestSoapyTypes
{
    //
    // ArgInfo
    //

    [Test]
    public void Test_ArgInfo_TypeAgnosticFields()
    {
        var key = "TestKey";
        var name = "TestName";
        var description = "TestDescription";
        var units = "TestUnits";
        var type = Pothosware.SoapySDR.ArgInfo.Type.Bool;
        var range = new Pothosware.SoapySDR.Range(0.0, 1.0, 0.1);
        string[] options = { "opt1", "opt2", "opt3" };
        string[] optionNames = { "Option1", "Option2", "Option3" };

        var argInfo = new Pothosware.SoapySDR.ArgInfo
        {
            Key = key,
            Name = name,
            Description = description,
            Units = units,
            ArgType = type,
            Range = range,
            Options = new Pothosware.SoapySDR.StringList(options),
            OptionNames = new Pothosware.SoapySDR.StringList(optionNames)
        };

        Assert.AreEqual(key, argInfo.Key);
        Assert.AreEqual(name, argInfo.Name);
        Assert.AreEqual(description, argInfo.Description);
        Assert.AreEqual(units, argInfo.Units);
        Assert.AreEqual(type, argInfo.ArgType);
        Assert.AreEqual(range.Minimum, argInfo.Range.Minimum);
        Assert.AreEqual(range.Maximum, argInfo.Range.Maximum);
        Assert.AreEqual(range.Step, argInfo.Range.Step);
        Assert.AreEqual(options, argInfo.Options);
        Assert.AreEqual(optionNames, argInfo.OptionNames);
    }

    //
    // Range
    //

    [Test]
    public void Test_Range()
    {
        var minimum = 1.0;
        var maximum = 2.0;
        var step = 0.05;

        // No parameters
        var range1 = new Pothosware.SoapySDR.Range();
        Assert.AreEqual(0.0, range1.Minimum);
        Assert.AreEqual(0.0, range1.Maximum);
        Assert.AreEqual(0.0, range1.Step);

        // No step
        var range2 = new Pothosware.SoapySDR.Range(minimum, maximum);
        Assert.AreEqual(minimum, range2.Minimum);
        Assert.AreEqual(maximum, range2.Maximum);
        Assert.AreEqual(0.0, range2.Step);

        // All parameters
        var range3 = new Pothosware.SoapySDR.Range(minimum, maximum, step);
        Assert.AreEqual(minimum, range3.Minimum);
        Assert.AreEqual(maximum, range3.Maximum);
        Assert.AreEqual(step, range3.Step);
    }

    public static int Main(string[] args) => TestRunner.RunNUnitTest("TestSoapyTypes");
}