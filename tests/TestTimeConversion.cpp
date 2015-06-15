// Copyright (c) 2015-2015 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/Time.hpp>
#include <cstdlib>
#include <cstdio>
#include <cmath>

static bool loopbackTimeToTicks(const long long timeNs, const double rate)
{
    const long long ticks = SoapySDR::timeNsToTicks(timeNs, rate);
    const long long outTime = SoapySDR::ticksToTimeNs(ticks, rate);
    //we expect error because timeNs specifies a sub-tick
    if (std::abs(timeNs - outTime)/1e9 < rate) return true;
    printf("FAIL: loopbackTimeToTicks(%lld, %f)\n", timeNs, rate);
    printf("    ticks = %lld, outTime = %lld\n", ticks, outTime);
    printf("    error = %f secs\n", std::abs(timeNs - outTime)/1e9);
    return false;
}

static bool loopbackTicksToTime(const long long ticks, const double rate)
{
    const long long timeNs = SoapySDR::ticksToTimeNs(ticks, rate);
    const long long outTicks = SoapySDR::timeNsToTicks(timeNs, rate);
    if (std::abs(ticks - outTicks) == 0) return true;
    printf("FAIL: loopbackTicksToTime(%lld, %f)\n", ticks, rate);
    printf("    timeNs = %lld, outTicks = %lld\n", timeNs, outTicks);
    printf("    error = %d ticks\n", int(std::abs(ticks - outTicks)));
    return false;
}

//http://stackoverflow.com/questions/8120062/generate-random-64-bit-integer
static unsigned rand256(void)
{
    static unsigned const limit = RAND_MAX - RAND_MAX % 256;
    unsigned result = std::rand();
    while ( result >= limit )
    {
        result = std::rand();
    }
    return result % 256;
}

static unsigned long long rand64bits(void)
{
    unsigned long long results = 0ULL;
    for ( int count = 8; count > 0; -- count)
    {
        results = 256U * results + rand256();
    }
    return results;
}

int main(void)
{
    //test that random times can make it through the conversion
    printf("Test random times...\n");
    for (size_t i = 0; i < 100; i++)
    {
        const long long timeNs = rand64bits();
        if (not loopbackTimeToTicks(timeNs, 1e9)) return EXIT_FAILURE;
        if (not loopbackTimeToTicks(-timeNs, 1e9)) return EXIT_FAILURE;
        if (not loopbackTimeToTicks(timeNs, 52e6)) return EXIT_FAILURE;
        if (not loopbackTimeToTicks(-timeNs, 52e6)) return EXIT_FAILURE;
        if (not loopbackTimeToTicks(timeNs, 61.44e6)) return EXIT_FAILURE;
        if (not loopbackTimeToTicks(-timeNs, 61.44e6)) return EXIT_FAILURE;
        if (not loopbackTimeToTicks(timeNs, 100e6/3)) return EXIT_FAILURE;
        if (not loopbackTimeToTicks(-timeNs, 100e6/3)) return EXIT_FAILURE;
    }
    printf("OK\n");

    //test random ticks for several different rates
    printf("Test random ticks...\n");
    for (size_t i = 0; i < 100; i++)
    {
        const long long ticks = rand64bits() >> 8; //room for max rate
        if (not loopbackTicksToTime(ticks, 1e9)) return EXIT_FAILURE;
        if (not loopbackTicksToTime(-ticks, 1e9)) return EXIT_FAILURE;
        if (not loopbackTicksToTime(ticks, 52e6)) return EXIT_FAILURE;
        if (not loopbackTicksToTime(-ticks, 52e6)) return EXIT_FAILURE;
        if (not loopbackTicksToTime(ticks, 61.44e6)) return EXIT_FAILURE;
        if (not loopbackTicksToTime(-ticks, 61.44e6)) return EXIT_FAILURE;
        if (not loopbackTicksToTime(ticks, 100e6/3)) return EXIT_FAILURE;
        if (not loopbackTicksToTime(-ticks, 100e6/3)) return EXIT_FAILURE;
    }
    printf("OK\n");

    printf("DONE!\n");
    return EXIT_SUCCESS;
}
