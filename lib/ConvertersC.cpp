// Copyright (c) 2020 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

#include "ErrorHelpers.hpp"
#include "TypeHelpers.hpp"

#include <SoapySDR/Converters.h>
#include <SoapySDR/ConverterRegistry.hpp>

#include <type_traits>

extern "C" {

static_assert(int(SoapySDR::ConverterRegistry::GENERIC) == int(SOAPY_SDR_CONVERTER_GENERIC), "GENERIC");
static_assert(int(SoapySDR::ConverterRegistry::VECTORIZED) == int(SOAPY_SDR_CONVERTER_VECTORIZED), "VECTORIZED");
static_assert(int(SoapySDR::ConverterRegistry::CUSTOM) == int(SOAPY_SDR_CONVERTER_CUSTOM), "CUSTOM");
static_assert(std::is_same<SoapySDR::ConverterRegistry::ConverterFunction, SoapySDRConverterFunction>::value, "ConverterFunction");

char **SoapySDRConverter_listTargetFormats(const char *sourceFormat, size_t *length)
{
    *length = 0;

    __SOAPY_SDR_C_TRY
    return toStrArray(SoapySDR::ConverterRegistry::listTargetFormats(sourceFormat), length);
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

char **SoapySDRConverter_listSourceFormats(const char *targetFormat, size_t *length)
{
    *length = 0;

    __SOAPY_SDR_C_TRY
    return toStrArray(SoapySDR::ConverterRegistry::listSourceFormats(targetFormat), length);
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

SoapySDRConverterFunctionPriority *SoapySDRConverter_listPriorities(const char* sourceFormat, const char* targetFormat, size_t *length)
{
    *length = 0;

    __SOAPY_SDR_C_TRY
    auto prioritiesCpp = SoapySDR::ConverterRegistry::listPriorities(sourceFormat, targetFormat);
    if(!prioritiesCpp.empty())
    {
        auto* prioritiesC = callocArrayType<SoapySDRConverterFunctionPriority>(prioritiesCpp.size());
        for(size_t i = 0; i < prioritiesCpp.size(); ++i)
        {
            prioritiesC[i] = static_cast<SoapySDRConverterFunctionPriority>(prioritiesCpp[i]);
        }
        *length = prioritiesCpp.size();

        return prioritiesC;
    }
    else return nullptr;
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

SoapySDRConverterFunction SoapySDRConverter_getFunction(const char *sourceFormat, const char *targetFormat)
{
    __SOAPY_SDR_C_TRY
    return static_cast<SoapySDRConverterFunction>(SoapySDR::ConverterRegistry::getFunction(sourceFormat, targetFormat));
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

SoapySDRConverterFunction SoapySDRConverter_getFunctionWithPriority(const char *sourceFormat, const char *targetFormat, const SoapySDRConverterFunctionPriority priority)
{
    __SOAPY_SDR_C_TRY
    return static_cast<SoapySDRConverterFunction>(SoapySDR::ConverterRegistry::getFunction(sourceFormat, targetFormat, static_cast<SoapySDR::ConverterRegistry::FunctionPriority>(priority)));
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

char **SoapySDRConverter_listAvailableSourceFormats(size_t *length)
{
    *length = 0;

    __SOAPY_SDR_C_TRY
    return toStrArray(SoapySDR::ConverterRegistry::listAvailableSourceFormats(), length);
    __SOAPY_SDR_C_CATCH_RET(nullptr);
}

}
