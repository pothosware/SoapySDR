// Copyright (c) 2021-2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

%typemap(javaclassmodifiers) LogHandlerBase "class"

%ignore SoapySDR_logf;
%ignore SoapySDR_vlogf;
%ignore SoapySDR_registerLogHandler;
%ignore SoapySDR::logf;
%ignore SoapySDR::vlogf;
%ignore SoapySDR::registerLogHandler;

%feature("director") LogHandlerBase;

%inline %{
    #include "Constants.hpp"

    class LogHandlerBase
    {
    public:
        LogHandlerBase()
        {
            GlobalHandle = this;
            SoapySDR::registerLogHandler(&GlobalHandler);
        }
        virtual ~LogHandlerBase()
        {
            GlobalHandle = nullptr;
            // Restore the default, C coded, log handler.
            SoapySDR::registerLogHandler(nullptr);
        }
        virtual void handle(const SoapySDR::Java::LogLevel, const char *) = 0;

        static void log(const SoapySDR::Java::LogLevel logLevel, const std::string& message)
        {
            SoapySDR::log((SoapySDR::LogLevel)logLevel, message);
        }

        static void setLogLevel(const SoapySDR::Java::LogLevel logLevel)
        {
            SoapySDR::setLogLevel((SoapySDR::LogLevel)logLevel);
        }

    private:
        static void GlobalHandler(const SoapySDR::LogLevel logLevel, const char *message)
        {
            if (GlobalHandle != nullptr) GlobalHandle->handle((SoapySDR::Java::LogLevel)logLevel, message);
        }

        static LogHandlerBase *GlobalHandle;
    };

    LogHandlerBase *LogHandlerBase::GlobalHandle = nullptr;
%}
