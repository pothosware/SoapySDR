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
        }

        @Override
        public void log(LogLevel logLevel, String message)
        {
            try
            {
                new PrintWriter(new BufferedWriter(new FileWriter(filepath, true))).println(logLevel.toString() + ": " + message);
            }
            catch(IOException e){}
        }

        private String filepath;
    }

    private void callLogger()
    {
        LogLevel logLevel = LogLevel.CRITICAL;
        int intArg = 1351;
        float floatArg = 41.8F;
        LocalTime localDateArg = LocalTime.now();
        String stringArg = "foobar";

        Logger.log(logLevel, "message");
        Logger.logf(logLevel, "message: %d", intArg);
        Logger.logf(logLevel, "message: %d %f %s %s", intArg, floatArg, localDateArg, stringArg);
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
            System.out.println(logLevel.toString()+": "+message);
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
        // TODO
    }

    @Test
    public void testExceptionLogger()
    {
        // TODO
    }
}
