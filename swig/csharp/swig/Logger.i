// Copyright (c) 2021 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

%typemap(csclassmodifiers) LogHandlerBase "internal class"

%ignore SoapySDR_logf;
%ignore SoapySDR_vlogf;
%ignore SoapySDR_registerLogHandler;
%ignore SoapySDR::logf;
%ignore SoapySDR::vlogf;
%ignore SoapySDR::registerLogHandler;

%feature("director:except") {
    if ($error != NULL) {
        throw Swig::DirectorMethodException();
    }
}

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
        virtual void Handle(const SoapySDR::CSharp::LogLevel, const char *) = 0;

        static void Log(const SoapySDR::CSharp::LogLevel logLevel, const std::string& message)
        {
            SoapySDR::log((SoapySDR::LogLevel)logLevel, message);
        }

        static SoapySDR::CSharp::LogLevel GetLogLevel()
        {
            return SoapySDR::CSharp::LogLevel(SoapySDR::getLogLevel());
        }

        static void SetLogLevel(const SoapySDR::CSharp::LogLevel logLevel)
        {
            SoapySDR::setLogLevel((SoapySDR::LogLevel)logLevel);
        }

    private:
        static void GlobalHandler(const SoapySDR::LogLevel logLevel, const char *message)
        {
            if (GlobalHandle != nullptr) GlobalHandle->Handle((SoapySDR::CSharp::LogLevel)logLevel, message);
        }

        static LogHandlerBase *GlobalHandle;
    };

    LogHandlerBase *LogHandlerBase::GlobalHandle = nullptr;
%}