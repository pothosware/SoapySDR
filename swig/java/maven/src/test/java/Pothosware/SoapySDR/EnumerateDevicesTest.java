// Copyright (c) 2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

package Pothosware.SoapySDR;

import static org.junit.jupiter.api.Assertions.*;
import org.junit.jupiter.api.*;

import java.util.*;

public class EnumerateDevicesTest
{
    // Make sure native libraries are loaded.
    static
    {
        try { JARUtils.initialize(); }
        catch(Exception e) {}
    }

    private static boolean hasNullDevice(KwargsList devices)
    {
        for(int i = 0; i < devices.size(); ++i)
        {
            var dev = devices.get(i);
            if(dev.containsKey("driver") && dev.get("driver").equals("null")) return true;
        }

        return false;
    }

    @Test
    public void testEnumerateNoParams()
    {
        // We can't guarantee the number of devices connected to the machine,
        // so just make sure this doesn't error out.
        var devices = Device.enumerate();
    }

    @Test
    public void testEnumerateStringParam()
    {
        var args = "driver=null,type=null";
        assertTrue(hasNullDevice(Device.enumerate(args)));
    }

    @Test
    public void testEnumerateMapParam()
    {
        var args = new HashMap<String, String>();
        args.put("driver", "null");
        args.put("type", "null");
        assertTrue(hasNullDevice(Device.enumerate(args)));
    }
}
