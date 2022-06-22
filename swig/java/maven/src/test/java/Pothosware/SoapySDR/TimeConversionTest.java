// Copyright (c) 2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

package Pothosware.SoapySDR;

import static org.junit.jupiter.api.Assertions.*;
import org.junit.jupiter.api.*;

public class TimeConversionTest
{
    // Make sure native libraries are loaded.
    static
    {
        try { JARUtils.initialize(); }
        catch(Exception e) {}
    }

    private static java.util.Random rng = new java.util.Random();

    private static final double[] rates = {1e9, 52e6, 61.44e6, 100e6 / 3};

    @Test
    public void testTimeNsToTicks()
    {
        // For some reason, @RepeatedTest wasn't working
        for(int i = 0; i < 100; ++i)
        {
            for(double rate: rates)
            {
                long timeNs = java.lang.Math.abs(rng.nextLong());
                long ticks = Time.timeNsToTicks(timeNs, rate);
                long timeNsOut = Time.ticksToTimeNs(ticks, rate);

                // We expect an error because timeNs specifies a subtick.
                assertTrue(((double)java.lang.Math.abs(timeNs - timeNsOut) / 1e9) < rate);
            }
        }
    }

    @Test
    public void testTicksToTimeNs()
    {
        // For some reason, @RepeatedTest wasn't working
        for(int i = 0; i < 100; ++i)
        {
            for(double rate: rates)
            {
                long ticks = java.lang.Math.abs(rng.nextLong());
                long timeNs = Time.ticksToTimeNs(ticks, rate);
                long ticksOut = Time.timeNsToTicks(timeNs, rate);
                assertEquals(ticks, ticksOut);
            }
        }
    }
}
