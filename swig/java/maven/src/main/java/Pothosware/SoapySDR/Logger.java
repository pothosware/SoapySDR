// Copyright (c) 2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

package Pothosware.SoapySDR;

public class Logger
{
    public interface SoapyLogger
    {
        void log(LogLevel logLevel, String message);
    }

    private static class LogHandler extends LogHandlerBase
    {
        public LogHandler(SoapyLogger _logger)
        {
            super();
            logger = _logger;
        }

        @Override
        public void handle(LogLevel logLevel, String message)
        {
            logger.log(logLevel, message);
        }

        private void setLogger(SoapyLogger _logger)
        {
            logger = _logger;
        }

        private SoapyLogger logger = null;
    }

    private static LogHandler logHandler = null;

    public static void log(LogLevel logLevel, String message)
    {
        LogHandlerBase.log(logLevel, message);
    }

    public static void logf(LogLevel logLevel, String format, Object... args)
    {
        log(logLevel, String.format(format, args));
    }

    public static void registerLogHandler(SoapyLogger logger)
    {
        logHandler = (logger != null) ? new LogHandler(logger) : null;
    }

    public static void setLogLevel(LogLevel logLevel)
    {
        LogHandlerBase.setLogLevel(logLevel);
    }
}
