// Copyright (c) 2021 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

%{
#include "JavaWrappers.hpp"

#include <SoapySDR/Logger.hpp>
%}

%feature("director") LogHandlerBase;

%inline %{
    class LogHandlerBase
    {
    public:
        LogHandlerBase(void)
        {
            globalHandle = this;
            SoapySDR::registerLogHandler(&globalHandler);
        }
        virtual ~LogHandlerBase(void)
        {
            globalHandle = nullptr;
            // Restore the default, C coded, log handler.
            SoapySDR::registerLogHandler(nullptr);
        }
        virtual void handle(const SoapySDR::Java::LogLevel, const char *) = 0;

    private:
        static void globalHandler(const SoapySDR::LogLevel logLevel, const char *message)
        {
            if (globalHandle != nullptr) globalHandle->handle(static_cast<SoapySDR::Java::LogLevel>(logLevel), message);
        }

        static LogHandlerBase *globalHandle;
    };
%}

%{
    LogHandlerBase *LogHandlerBase::globalHandle = nullptr;
%}
