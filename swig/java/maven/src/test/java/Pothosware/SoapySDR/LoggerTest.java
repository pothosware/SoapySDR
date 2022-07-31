// Copyright (c) 2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

package Pothosware.SoapySDR;

import static org.junit.jupiter.api.Assertions.*;
import org.junit.jupiter.api.*;

import java.io.*;
import java.time.LocalTime;

public class LoggerTest
{
    // Make sure native libraries are loaded.
    static
    {
        try { JARUtils.initialize(); }
        catch(Exception e) {}
    }

    private class FileLogger implements Logger.SoapyLogger
    {
        public FileLogger(String path)
        {
            super();
            filepath = path;

            try
            {
                printWriter = new PrintWriter(new BufferedWriter(new FileWriter(filepath, true)), true);
            }
            catch(IOException e)
            {
                System.err.println(e);
            }
        }

        @Override
        public void log(LogLevel logLevel, String message)
        {
            printWriter.println(logLevel.toString() + ": " + message);
        }

        private String filepath = null;
        private PrintWriter printWriter = null;
    }

    private class ExceptionLogger implements Logger.SoapyLogger
    {
        public ExceptionLogger()
        {
            super();
        }

        @Override
        public void log(LogLevel logLevel, String message)
        {
            throw new RuntimeException(logLevel.toString()+": "+message);
        }
    }

    private void callLogger()
    {
        LogLevel logLevel = LogLevel.CRITICAL;
        int intArg = 1351;
        float floatArg = 41.8F;
        String stringArg = "foobar";

        Logger.log(logLevel, "message");
        Logger.logf(logLevel, "message: %d", intArg);
        Logger.logf(logLevel, "message: %d %f %s", intArg, floatArg, stringArg);
    }

    @Test
    public void testLogger() throws IOException
    {
        // TODO: test value after getter implemented
        Logger.setLogLevel(LogLevel.NOTICE);

        // Before doing anything, the standard stdio logger should be used. Unfortunately,
        // we can't intercept and programmatically check the output.
        callLogger();

        Logger.registerLogHandler((logLevel, message) ->
        {
            System.out.println("Java lambda: " + logLevel.toString()+": "+message);
        });
        callLogger();
        Logger.registerLogHandler(null);

        File loggerFile = File.createTempFile("abc", "def");
        loggerFile.deleteOnExit();

        Logger.registerLogHandler(new FileLogger(loggerFile.getAbsolutePath()));
        callLogger();
        Logger.registerLogHandler(null);

        // Now the standard stdio handler should be used.
        callLogger();

        // Check out the log file and make sure the custom logger was invoked as expected.
        assertTrue(loggerFile.exists());

        // Make sure the expected contents were written once.
        var buffer = new char[512];
        var fileReader = new FileReader(loggerFile);
        var numRead = fileReader.read(buffer);
        assertTrue(numRead > 0);

        var expectedString = String.format("CRITICAL: message\nCRITICAL: message: %d\nCRITICAL: message: %d %f %s\n", 1351, 1351, 41.8F, "foobar");
        var logString = new String(buffer, 0, numRead);
        assertEquals(expectedString, logString);
    }

    @Test
    public void testExceptionLogger()
    {
        // Make sure exceptions from the Java callback are handled cleanly.
        Logger.registerLogHandler(new ExceptionLogger());

        var ex = assertThrows(RuntimeException.class, () -> { Logger.log(LogLevel.ERROR, "This should have thrown an exception"); });
        assertEquals("ERROR: This should have thrown an exception", ex.getMessage());
    }
}
