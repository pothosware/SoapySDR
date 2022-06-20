// Copyright (c) 2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

package Pothosware.SoapySDR;

import static org.junit.jupiter.api.Assertions.*;
import org.junit.jupiter.api.Test;

public class BuildInfoTest
{
    // Make sure native libraries are loaded.
    static
    {
        try { JARUtils.initialize(); }
        catch(Exception e) {}
    }

    @Test
    public void testBuildInfo()
    {
        assertFalse(BuildInfo.CompileTime.getABIVersion().isEmpty());
        assertFalse(BuildInfo.CompileTime.getAPIVersion().isEmpty());
        assertFalse(BuildInfo.CompileTime.getLibVersion().isEmpty());

        assertFalse(BuildInfo.Runtime.getABIVersion().isEmpty());
        assertFalse(BuildInfo.Runtime.getAPIVersion().isEmpty());
        assertFalse(BuildInfo.Runtime.getLibVersion().isEmpty());
    }
}
