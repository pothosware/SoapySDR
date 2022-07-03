// Copyright (c) 2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

package Pothosware.SoapySDR;

import static org.junit.jupiter.api.Assertions.*;
import org.junit.jupiter.api.*;
import org.junit.jupiter.params.*;
import org.junit.jupiter.params.provider.*;

import org.apache.commons.math3.complex.*;

import java.nio.*;
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
        public HashSet<StreamFlags> streamFlags = new HashSet<StreamFlags>();
        public long timeNs = 1000;
        public int timeoutUs = 1000;
        public long numElems = 1024;

        public TestParams()
        {
            streamArgsMap.put("bufflen", "8192");
            streamArgsMap.put("buffers", "15");

            streamFlags.add(StreamFlags.HAS_TIME);
            streamFlags.add(StreamFlags.END_BURST);
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

    // TODO: generic if I figure it out
    private void testTxStreamWriteCS8(
        TxStream txStream,
        TestParams params,
        boolean streamFormatMatches)
    {
        var mtu = (int)txStream.getMTU();
        assertEquals(1024, mtu);

        // Test array and NIO buffer overloads.
        var arr = new byte[mtu*2];
        var buff = ByteBuffer.allocateDirect(mtu*2);

        if(streamFormatMatches)
        {
            // Pause the JNI and get the direct heap location to the buffer.
            txStream.setExecutionPolicy(StreamExecutionPolicy.EFFICIENT);
            assertEquals(StreamExecutionPolicy.EFFICIENT, txStream.getExecutionPolicy());
            var streamResult = txStream.writeArray(arr, params.timeNs, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            // Copy the array and operate on the copy.
            txStream.setExecutionPolicy(StreamExecutionPolicy.THREAD_SAFE);
            assertEquals(StreamExecutionPolicy.THREAD_SAFE, txStream.getExecutionPolicy());
            streamResult = txStream.writeArray(arr, params.timeNs, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            streamResult = txStream.writeBuffer(buff, params.timeNs, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            streamResult = txStream.readStatus(params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());

            // We should only be able to accept even-length buffers since we're writing
            // samples.
            var arrWriteEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    var badStreamResult = txStream.writeArray(new byte[mtu-1], params.timeNs, params.timeoutUs);
                });
            assertTrue(arrWriteEx.getMessage().contains("interleaved"));

            var buffWriteEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    var badStreamResult = txStream.writeBuffer(ByteBuffer.allocateDirect(mtu-1), params.timeNs, params.timeoutUs);
                });
            assertTrue(buffWriteEx.getMessage().contains("divisible"));

            // We need the buffer to be direct so we can access the underlying memory through the JNI.
            var indirectBuffWriteEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    var badStreamResult = txStream.writeBuffer(ByteBuffer.allocate(mtu), params.timeNs, params.timeoutUs);
                });
            assertTrue(indirectBuffWriteEx.getMessage().contains("direct"));
        }
        else
        {
            var arrWriteEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    var badStreamResult = txStream.writeArray(arr, params.timeNs, params.timeoutUs);
                });
            assertTrue(arrWriteEx.getMessage().contains(StreamFormat.CS8));

            var buffWriteEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    var badStreamResult = txStream.writeBuffer(buff, params.timeNs, params.timeoutUs);
                });
            assertTrue(buffWriteEx.getMessage().contains(StreamFormat.CS8));
        }
    }

    // TODO: generic if I figure it out
    private void testRxStreamReadCS8(
        RxStream rxStream,
        TestParams params,
        boolean streamFormatMatches)
    {
        var mtu = (int)rxStream.getMTU();
        assertEquals(1024, mtu);

        // Test array and NIO buffer overloads.
        var arr = new byte[mtu*2];
        var buff = ByteBuffer.allocateDirect(mtu*2);

        if(streamFormatMatches)
        {
            // Pause the JNI and get the direct heap location to the buffer.
            rxStream.setExecutionPolicy(StreamExecutionPolicy.EFFICIENT);
            assertEquals(StreamExecutionPolicy.EFFICIENT, rxStream.getExecutionPolicy());
            var streamResult = rxStream.readArray(arr, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            // Copy the array and operate on the copy.
            rxStream.setExecutionPolicy(StreamExecutionPolicy.THREAD_SAFE);
            assertEquals(StreamExecutionPolicy.THREAD_SAFE, rxStream.getExecutionPolicy());
            streamResult = rxStream.readArray(arr, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            streamResult = rxStream.readBuffer(buff, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            // We should only be able to accept even-length buffers since we're writing
            // samples.
            var arrReadEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    var badStreamResult = rxStream.readArray(new byte[mtu-1], params.timeoutUs);
                });
            assertTrue(arrReadEx.getMessage().contains("interleaved"));

            var buffReadEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    var badStreamResult = rxStream.readBuffer(ByteBuffer.allocateDirect(mtu-1), params.timeoutUs);
                });
            assertTrue(buffReadEx.getMessage().contains("divisible"));

            // We need the buffer to be direct so we can access the underlying memory through the JNI.
            var indirectBuffReadEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    var badStreamResult = rxStream.readBuffer(ByteBuffer.allocate(mtu), params.timeoutUs);
                });
            assertTrue(indirectBuffReadEx.getMessage().contains("direct"));

            // Make sure we can't read into a read-only buffer.
            var readonlyBuffReadEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    var badStreamResult = rxStream.readBuffer(ByteBuffer.allocateDirect(mtu).asReadOnlyBuffer(), params.timeoutUs);
                });
            assertTrue(readonlyBuffReadEx.getMessage().contains("read-only"));
        }
        else
        {
            var arrReadEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    var badStreamResult = rxStream.readArray(arr, params.timeoutUs);
                });
            assertTrue(arrReadEx.getMessage().contains(StreamFormat.CS8));

            var buffReadEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    var badStreamResult = rxStream.readBuffer(buff, params.timeoutUs);
                });
            assertTrue(buffReadEx.getMessage().contains(StreamFormat.CS8));
        }
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
        assertFalse(txStream.active());

        assertEquals(ErrorCode.NOT_SUPPORTED, txStream.activate(params.streamFlags, params.timeNs, params.timeoutUs));

        testTxStreamWriteCS8(txStream, params, true);
    }

    @Test
    public void testRxStreamingCS8()
    {
        var device = getTestDevice();
        var params = new TestParams();

        //
        // Test with single channel
        //

        var rxStream = device.setupRxStream(StreamFormat.CS8, params.oneChannel, params.streamArgsMap);
        assertEquals(StreamFormat.CS8, rxStream.getFormat());
        // TODO: compare channels when stream returns int[]
        assertEquals(params.streamArgsMap, rxStream.getArgs());
        assertFalse(rxStream.active());

        assertEquals(ErrorCode.NOT_SUPPORTED, rxStream.activate(params.streamFlags, params.timeNs, params.timeoutUs));

        testRxStreamReadCS8(rxStream, params, true);
    }
}
