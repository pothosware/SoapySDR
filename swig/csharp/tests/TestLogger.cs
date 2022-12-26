// Copyright (c) 2021-2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

using System;
using System.Globalization;
using System.IO;
using NUnit.Framework;

[TestFixture]
public class TestLogger
{
    private static readonly string TempFileName = Path.Combine(Path.GetTempPath(), Guid.NewGuid().ToString());

    private static void TestLoggerFcn(Pothosware.SoapySDR.LogLevel logLevel, string message)
    {
        using (FileStream fs = File.Open(TempFileName, FileMode.Append))
        {
            using (StreamWriter sw = new StreamWriter(fs))
            {
                sw.WriteLine(string.Format("{0}: {1}", logLevel, message));
            }
        }
    }

    private static void CallLogger()
    {
        Pothosware.SoapySDR.LogLevel logLevel = Pothosware.SoapySDR.LogLevel.Critical;
        int intArg = 1351;
        float floatArg = 41.8F;
        string stringArg = "foobar";
        var cultureInfo = new CultureInfo("es-ES", false);

        Pothosware.SoapySDR.Logger.Log(logLevel, "message");
        Pothosware.SoapySDR.Logger.LogF(logLevel, "message: {0}", intArg);
        Pothosware.SoapySDR.Logger.LogF(logLevel, "message: {0} {1} {2}", new object[] { intArg, floatArg, stringArg });
        Pothosware.SoapySDR.Logger.LogF(logLevel, cultureInfo, "message: {0}", intArg);
        Pothosware.SoapySDR.Logger.LogF(logLevel, cultureInfo, "message: {0} {1} {2}", new object[] { intArg, floatArg, stringArg });
        Pothosware.SoapySDR.Logger.LogF(logLevel, "message: {0} {1}", intArg, floatArg);
        Pothosware.SoapySDR.Logger.LogF(logLevel, cultureInfo, "message: {0} {1}", intArg, floatArg);
    }

    private static string GetExpectedLoggerOutput()
    {
        Pothosware.SoapySDR.LogLevel logLevel = Pothosware.SoapySDR.LogLevel.Critical;
        int intArg = 1351;
        float floatArg = 41.8F;
        string stringArg = "foobar";
        var cultureInfo = new CultureInfo("es-ES", false);

        string[] expectedOutputs = {
            string.Format("{0}: {1}", logLevel, string.Format("message")),
            string.Format("{0}: {1}", logLevel, string.Format("message: {0}", intArg)),
            string.Format("{0}: {1}", logLevel, string.Format("message: {0} {1} {2}", new object[] { intArg, floatArg, stringArg })),
            string.Format("{0}: {1}", logLevel, string.Format(cultureInfo, "message: {0}", intArg)),
            string.Format("{0}: {1}", logLevel, string.Format(cultureInfo, "message: {0} {1} {2}", new object[] { intArg, floatArg, stringArg })),
            string.Format("{0}: {1}", logLevel, string.Format("message: {0} {1}", intArg, floatArg)),
            string.Format("{0}: {1}", logLevel, string.Format(cultureInfo, "message: {0} {1}", intArg, floatArg)),
        };

        return string.Join("\r\n", expectedOutputs) + "\r\n";
    }

    [Test]
    public void Test_Logger()
    {
        Pothosware.SoapySDR.Logger.LogLevel = Pothosware.SoapySDR.LogLevel.Notice;
        Assert.AreEqual(Pothosware.SoapySDR.LogLevel.Notice, Pothosware.SoapySDR.Logger.LogLevel);

        // Before doing anything, the standard stdio logger should be used. Unfortunately,
        // we can't intercept and programmatically check the output.
        CallLogger();

        Pothosware.SoapySDR.Logger.RegisterLogHandler(TestLoggerFcn);
        CallLogger();
        Pothosware.SoapySDR.Logger.RegisterLogHandler(null);

        // Now the standard stdio handler should be used.
        CallLogger();

        // Check out log file and make sure the custom logger was invoked as expected.
        Assert.IsTrue(File.Exists(TempFileName));

        // Make sure the expected contents were written once.
        using (FileStream fs = File.Open(TempFileName, FileMode.Open))
        {
            using (StreamReader sr = new StreamReader(fs))
            {
                var fileContents = sr.ReadToEnd();
                Assert.AreEqual(GetExpectedLoggerOutput(), fileContents);
            }
        }
    }

    private static void ExceptionLogger(Pothosware.SoapySDR.LogLevel logLevel, string message) => throw new InvalidOperationException(string.Format("{0}: {1}", logLevel, message));

    [Test]
    public void Test_LoggerException()
    {
        Pothosware.SoapySDR.Logger.LogLevel = Pothosware.SoapySDR.LogLevel.Notice;
        Assert.AreEqual(Pothosware.SoapySDR.LogLevel.Notice, Pothosware.SoapySDR.Logger.LogLevel);

        // Make sure exceptions from the C# callback given to C++ propagate up
        // to C# properly.
        Pothosware.SoapySDR.Logger.RegisterLogHandler(ExceptionLogger);

        var ex = Assert.Throws<InvalidOperationException>(delegate { Pothosware.SoapySDR.Logger.Log(Pothosware.SoapySDR.LogLevel.Error, "This should throw"); });
        Assert.AreEqual("Error: This should throw", ex.Message);

        Pothosware.SoapySDR.Logger.RegisterLogHandler(null);
    }

    [OneTimeTearDown]
    public void TearDown()
    {
        if (File.Exists(TempFileName)) File.Delete(TempFileName);
    }

    public static int Main(string[] args) => TestRunner.RunNUnitTest("TestLogger");
}