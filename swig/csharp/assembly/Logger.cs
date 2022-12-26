// Copyright (c) 2020-2021 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

using System;

namespace Pothosware.SoapySDR
{
    /// <summary>
    /// SoapySDR's logging interface.
    ///
    /// Through this, messages can be logged to console or file by registering C# delegates.
    /// The logging API is designed to match System.String.Format().
    /// </summary>
    public class Logger
    {
        /// <summary>
        /// The signature of a function to use for logging.
        /// </summary>
        /// <param name="logLevel">The message's level of importance.</param>
        /// <param name="message">The message text.</param>
        public delegate void LoggerDelegate(LogLevel logLevel, string message);

        private static LoggerDelegate Delegate = null;

        private class CSharpLogHandler : LogHandlerBase
        {
            public CSharpLogHandler() : base()
            {
            }

            public override void Handle(LogLevel logLevel, string message) => Delegate?.Invoke(logLevel, message);
        }

        /// <summary>
        /// The log level threshold. Messages with lower priorities are dropped.
        /// </summary>
        public static LogLevel LogLevel
        {
            get => LogHandlerBase.GetLogLevel();
            set => LogHandlerBase.SetLogLevel(value);
        }

        private static CSharpLogHandler LogHandler = null;

        /// <summary>
        /// Register a custom logging function to be used for all SoapySDR logging calls.
        /// </summary>
        /// <param name="del">A logging function, or null for the default logger (prints to stderr).</param>
        public static void RegisterLogHandler(LoggerDelegate del)
        {
            if(del != null)
            {
                LogHandler = new CSharpLogHandler();
                Delegate = del;
            }
            else
            {
                LogHandler = null;
                Delegate = null;
            }
        }

        /// <summary>
        /// Log a message with a given level and string.
        /// </summary>
        /// <param name="logLevel">The message's priority</param>
        /// <param name="message">The message string</param>
        public static void Log(LogLevel logLevel, string message) => LogHandlerBase.Log(logLevel, message);

        /// <summary>
        /// Log a message with a given level and string, formatted with System.String.Format().
        /// </summary>
        /// <param name="logLevel">The message's priority</param>
        /// <param name="format">The message format</param>
        public static void LogF(LogLevel logLevel, string format, object arg) =>
            Log(logLevel, string.Format(format, arg));

        /// <summary>
        /// Log a message with a given level and string, formatted with System.String.Format().
        /// </summary>
        /// <param name="logLevel">The message's priority</param>
        /// <param name="format">The message format</param>
        public static void LogF(LogLevel logLevel, string format, object[] args) =>
            Log(logLevel, string.Format(format, args));

        /// <summary>
        /// Log a message with a given level and string, formatted with System.String.Format().
        /// </summary>
        /// <param name="logLevel">The message's priority</param>
        /// <param name="format">The message format</param>
        public static void LogF(LogLevel logLevel, IFormatProvider formatProvider, string format, object arg) =>
            Log(logLevel, string.Format(formatProvider, format, arg));

        /// <summary>
        /// Log a message with a given level and string, formatted with System.String.Format().
        /// </summary>
        /// <param name="logLevel">The message's priority</param>
        /// <param name="format">The message format</param>
        public static void LogF(LogLevel logLevel, IFormatProvider formatProvider, string format, object[] args) =>
            Log(logLevel, string.Format(formatProvider, format, args));

        /// <summary>
        /// Log a message with a given level and string, formatted with System.String.Format().
        /// </summary>
        /// <param name="logLevel">The message's priority</param>
        /// <param name="format">The message format</param>
        public static void LogF(LogLevel logLevel, string format, object arg0, object arg1) =>
            Log(logLevel, string.Format(format, arg0, arg1));

        /// <summary>
        /// Log a message with a given level and string, formatted with System.String.Format().
        /// </summary>
        /// <param name="logLevel">The message's priority</param>
        /// <param name="format">The message format</param>
        public static void LogF(LogLevel logLevel, IFormatProvider formatProvider, string format, object arg0, object arg1) =>
            Log(logLevel, string.Format(formatProvider, format, arg0, arg1));

        /// <summary>
        /// Log a message with a given level and string, formatted with System.String.Format().
        /// </summary>
        /// <param name="logLevel">The message's priority</param>
        /// <param name="format">The message format</param>
        public static void LogF(LogLevel logLevel, string format, object arg0, object arg1, object arg2) =>
            Log(logLevel, string.Format(format, arg0, arg1, arg2));

        /// <summary>
        /// Log a message with a given level and string, formatted with System.String.Format().
        /// </summary>
        /// <param name="logLevel">The message's priority</param>
        /// <param name="format">The message format</param>
        public static void LogF(LogLevel logLevel, IFormatProvider formatProvider, string format, object arg0, object arg1, object arg2) =>
            Log(logLevel, string.Format(formatProvider, format, arg0, arg1, arg2));
    }
}
