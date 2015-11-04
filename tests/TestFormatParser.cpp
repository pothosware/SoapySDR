// Copyright (c) 2015-2015 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Formats.hpp>
#include <cstdlib>
#include <cstdio>

int main(void)
{
    #define formatCheck(formatStr, expectedSize) \
    { \
        size_t bytes = SoapySDR::formatToSize(formatStr); \
        printf("%s -> %d bytes/element\t", formatStr, int(bytes)); \
        if (bytes != expectedSize) \
        { \
            printf("FAIL: expected %d bytes!\n", int(expectedSize)); \
            return EXIT_FAILURE; \
        } \
        else printf("OK\n"); \
    }

    formatCheck(SOAPY_SDR_CF64, 16);
    formatCheck(SOAPY_SDR_CF32, 8);
    formatCheck(SOAPY_SDR_CS32, 8);
    formatCheck(SOAPY_SDR_CU32, 8);
    formatCheck(SOAPY_SDR_CS16, 4);
    formatCheck(SOAPY_SDR_CU16, 4);
    formatCheck(SOAPY_SDR_CS12, 3);
    formatCheck(SOAPY_SDR_CU12, 3);
    formatCheck(SOAPY_SDR_CS8, 2);
    formatCheck(SOAPY_SDR_CU8, 2);
    formatCheck(SOAPY_SDR_CS4, 1);
    formatCheck(SOAPY_SDR_CU4, 1);

    formatCheck(SOAPY_SDR_F64, 8);
    formatCheck(SOAPY_SDR_F32, 4);
    formatCheck(SOAPY_SDR_S32, 4);
    formatCheck(SOAPY_SDR_U32, 4);
    formatCheck(SOAPY_SDR_S16, 2);
    formatCheck(SOAPY_SDR_U16, 2);
    formatCheck(SOAPY_SDR_S8, 1);
    formatCheck(SOAPY_SDR_U8, 1);

    printf("DONE!\n");
    return EXIT_SUCCESS;
}
