// Copyright (c) 2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

package Pothosware.SoapySDR;

import static org.junit.jupiter.api.Assertions.*;
import org.junit.jupiter.api.*;
import org.junit.jupiter.params.*;
import org.junit.jupiter.params.provider.*;

import org.apache.commons.math3.complex.*;

import java.util.*;

// For the null device, functions generally don't do anything or return some
// hardcoded value, but we can still make sure functions compile and run as
// expected.

public class StreamingAPITest
{
    // Make sure native libraries are loaded.
    static
    {
        try { JARUtils.initialize(); }
        catch(Exception e) {}
    }

    //
    // Utility
    //

    private class TestParams
    {
        public int[] oneChannel = new int[]{0};
        public int[] twoChannels = new int[]{0,1};
        public String streamArgsString = "bufflen=8192,buffers=15";
        public Kwargs streamArgsMap = new Kwargs();
        //public EnumSet<StreamFlags> streamFlags = new EnumSet<StreamFlags>();
        public long timeNs = 1000;
        public long timeoutUs = 1000;
        public long numElems = 1024;

        public TestParams()
        {
            streamArgsMap.put("bufflen", "8192");
            streamArgsMap.put("buffers", "15");

            /*
            streamFlags.add(StreamFlags.HAS_TIME);
            streamFlags.add(StreamFlags.END_BURST);
            */
        }
    }

    private void testDeviceKeys(Device device)
    {
        assertEquals("null", device.getDriverKey());
        assertEquals("null", device.getHardwareKey());
        assertEquals("null:null", device.toString());
    }

    private Device getTestDevice()
    {
        // Make sure either method works.
        testDeviceKeys(new Device("driver=null,type=null"));

        var args = new HashMap<String, String>();
        args.put("driver", "null");
        args.put("type", "null");

        var device = new Device(args);
        testDeviceKeys(device);

        return device;
    }

    //
    // Tests (TODO: generic if I figure it out)
    //

    @Test
    public void testTxStreamingCS8()
    {
        var device = getTestDevice();
        var params = new TestParams();

        //
        // Test with single channel
        //

        var txStream = device.setupTxStream(StreamFormat.CS8, params.oneChannel, params.streamArgsMap);
        assertEquals(StreamFormat.CS8, txStream.getFormat());
        // TODO: compare channels when stream returns int[]
        assertEquals(params.streamArgsMap, txStream.getArgs());
    }
}
