// Copyright (c) 2021-2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

////////////////////////////////////////////////////////////////////////
// Include all major headers to compile against
////////////////////////////////////////////////////////////////////////
%{
#include <SoapySDR/Version.hpp>
#include <SoapySDR/Modules.hpp>
#include <SoapySDR/Device.hpp>
#include <SoapySDR/Errors.hpp>
#include <SoapySDR/Formats.hpp>
#include <SoapySDR/Time.hpp>
#include <SoapySDR/Logger.hpp>
%}

////////////////////////////////////////////////////////////////////////
// Common ignores
////////////////////////////////////////////////////////////////////////

// SoapySDR/Errors.h
%ignore SoapySDR_errToStr;

// SoapySDR/Formats.h
%ignore SoapySDR_formatToSize;

// SoapySDR/Logger.h
%ignore SoapySDR_log;
%ignore SoapySDR_vlogf;
%ignore SoapySDR_logf;
%ignore SoapySDRLogHandler;
%ignore SoapySDR_registerLogHandler;
%ignore SoapySDR_getLogLevel;
%ignore SoapySDR_setLogLevel;

// SoapySDR/Logger.hpp
%ignore SoapySDR::logf;
%ignore SoapySDR::vlogf;
%ignore SoapySDR::LogHandler;
%ignore SoapySDR::registerLogHandler;

// SoapySDR/Modules.h
%ignore SoapySDR_getRootPath;
%ignore SoapySDR_listSearchPaths;
%ignore SoapySDR_listModules;
%ignore SoapySDR_listModulesPath;
%ignore SoapySDR_loadModule;
%ignore SoapySDR_getLoaderResult;
%ignore SoapySDR_getModuleVersion;
%ignore SoapySDR_unloadModule;
%ignore SoapySDR_loadModules;
%ignore SoapySDR_unloadModules;

// SoapySDR/Types.h
%ignore SoapySDRRange;
%ignore SoapySDRKwargs;
%ignore SoapySDRKwargs_fromString;
%ignore SoapySDRKwargs_toString;
%ignore SoapySDRArgInfoType;
%ignore SoapySDRArgInfo;
%ignore SoapySDR_free;
%ignore SoapySDRStrings_clear;
%ignore SoapySDRKwargs_get;
%ignore SoapySDRKwargs_set;
%ignore SoapySDRKwargs_clear;
%ignore SoapySDRKwargsList_clear;
%ignore SoapySDRArgInfo_clear;
%ignore SoapySDRArgInfoList_clear;

// SoapySDR/Types.hpp
%ignore SoapySDR::Detail::StringToSetting;
%ignore SoapySDR::Detail::SettingToString;

// SoapySDR/Version.h
%ignore SoapySDR_getAPIVersion;
%ignore SoapySDR_getABIVersion;
%ignore SoapySDR_getLibVersion;
