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

    private static class TestParams
    {
        public int streamElems = 1024;

        public int[] oneChannel = new int[]{0};
        public int[] twoChannels = new int[]{0,1};
        public String streamArgsString = "bufflen=8192,buffers=15";
        public Kwargs streamArgsMap = new Kwargs();
        public HashSet<StreamFlags> streamFlags = new HashSet<StreamFlags>();
        public long timeNs = 1000;
        public int timeoutUs = 1000;
        public long numElems = 1024;

        public byte[] byte1DArray = null;
        public byte[][] byte2DArray = null;
        public ByteBuffer byteBuffer = null;

        public short[] short1DArray = null;
        public short[][] short2DArray = null;
        public ShortBuffer shortBuffer = null;

        public int[] int1DArray = null;
        public int[][] int2DArray = null;
        public IntBuffer intBuffer = null;

        public float[] float1DArray = null;
        public float[][] float2DArray = null;
        public FloatBuffer floatBuffer = null;

        public double[] double1DArray = null;
        public double[][] double2DArray = null;
        public DoubleBuffer doubleBuffer = null;

        public TestParams()
        {
            streamArgsMap.put("bufflen", "8192");
            streamArgsMap.put("buffers", "15");

            streamFlags.add(StreamFlags.HAS_TIME);
            streamFlags.add(StreamFlags.END_BURST);

            byte1DArray = new byte[streamElems];
            byte2DArray = new byte[2][];
            byte2DArray[0] = new byte[streamElems];
            byte2DArray[1] = new byte[streamElems];
            byteBuffer = ByteBuffer.allocateDirect(streamElems);

            //
            // For some reason, only ByteBuffer has allocateDirect
            //

            short1DArray = new short[streamElems*2];
            short2DArray = new short[2][];
            short2DArray[0] = new short[streamElems];
            short2DArray[1] = new short[streamElems];
            shortBuffer = ByteBuffer.allocateDirect(streamElems*2*2).asShortBuffer();

            int1DArray = new int[streamElems*2];
            int2DArray = new int[2][];
            int2DArray[0] = new int[streamElems];
            int2DArray[1] = new int[streamElems];
            intBuffer = ByteBuffer.allocateDirect(streamElems*2*4).asIntBuffer();

            float1DArray = new float[streamElems*2];
            float2DArray = new float[2][];
            float2DArray[0] = new float[streamElems];
            float2DArray[1] = new float[streamElems];
            floatBuffer = ByteBuffer.allocateDirect(streamElems*2*4).asFloatBuffer();

            double1DArray = new double[streamElems*2];
            double2DArray = new double[2][];
            double2DArray[0] = new double[streamElems];
            double2DArray[1] = new double[streamElems];
            doubleBuffer = ByteBuffer.allocateDirect(streamElems*2*8).asDoubleBuffer();
        }
    }

    private static TestParams params = new TestParams();

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
    // CS8
    //

    private void test1DTxStreamWriteCS8(
        TxStream txStream,
        boolean streamFormatMatches)
    {
        assertEquals(1, txStream.getChannels().size());

        var mtu = (int)txStream.getMTU();
        assertEquals(1024, mtu);

        if(streamFormatMatches)
        {
            // Pause the JNI and get the direct heap location to the buffer.
            txStream.setExecutionPolicy(StreamExecutionPolicy.EFFICIENT);
            assertEquals(StreamExecutionPolicy.EFFICIENT, txStream.getExecutionPolicy());
            var streamResult = txStream.writeArray(params.byte1DArray, params.timeNs, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            // Copy the array and operate on the copy.
            txStream.setExecutionPolicy(StreamExecutionPolicy.THREAD_SAFE);
            assertEquals(StreamExecutionPolicy.THREAD_SAFE, txStream.getExecutionPolicy());
            streamResult = txStream.writeArray(params.byte1DArray, params.timeNs, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            streamResult = txStream.writeBuffer(params.byteBuffer, params.timeNs, params.timeoutUs);
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
                    txStream.writeArray(new byte[mtu-1], params.timeNs, params.timeoutUs);
                });
            assertTrue(arrWriteEx.getMessage().contains("interleaved"));

            var buffWriteEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    txStream.writeBuffer(ByteBuffer.allocateDirect(mtu-1), params.timeNs, params.timeoutUs);
                });
            assertTrue(buffWriteEx.getMessage().contains("divisible"));

            // We need the buffer to be direct so we can access the underlying memory through the JNI.
            var indirectBuffWriteEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    txStream.writeBuffer(ByteBuffer.allocate(mtu), params.timeNs, params.timeoutUs);
                });
            assertTrue(indirectBuffWriteEx.getMessage().contains("direct"));
        }
        else
        {
            var arrWriteEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    txStream.writeArray(params.byte1DArray, params.timeNs, params.timeoutUs);
                });
            assertTrue(arrWriteEx.getMessage().contains(StreamFormat.CS8));

            var buffWriteEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    txStream.writeBuffer(params.byteBuffer, params.timeNs, params.timeoutUs);
                });
            assertTrue(buffWriteEx.getMessage().contains(StreamFormat.CS8));
        }
    }

    private void test2DTxStreamWriteCS8(
        TxStream txStream,
        boolean streamFormatMatches)
    {
        assertEquals(2, txStream.getChannels().size());

        var mtu = (int)txStream.getMTU();
        assertEquals(1024, mtu);

        if(streamFormatMatches)
        {
            // Pause the JNI and get the direct heap location to the buffer.
            txStream.setExecutionPolicy(StreamExecutionPolicy.EFFICIENT);
            assertEquals(StreamExecutionPolicy.EFFICIENT, txStream.getExecutionPolicy());
            var streamResult = txStream.writeArray(params.byte2DArray, params.timeNs, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            // Copy the arrays and operate on the copies.
            txStream.setExecutionPolicy(StreamExecutionPolicy.THREAD_SAFE);
            assertEquals(StreamExecutionPolicy.THREAD_SAFE, txStream.getExecutionPolicy());
            streamResult = txStream.writeArray(params.byte2DArray, params.timeNs, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            streamResult = txStream.readStatus(params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());

            // TODO: bad 2D arrays
        }
        else
        {
            var arrWriteEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    txStream.writeArray(params.byte2DArray, params.timeNs, params.timeoutUs);
                });
            assertTrue(arrWriteEx.getMessage().contains(StreamFormat.CS8));
        }
    }

    private void test1DRxStreamReadCS8(
        RxStream rxStream,
        boolean streamFormatMatches)
    {
        assertEquals(1, rxStream.getChannels().size());

        var mtu = (int)rxStream.getMTU();
        assertEquals(1024, mtu);

        if(streamFormatMatches)
        {
            // Pause the JNI and get the direct heap location to the buffer.
            rxStream.setExecutionPolicy(StreamExecutionPolicy.EFFICIENT);
            assertEquals(StreamExecutionPolicy.EFFICIENT, rxStream.getExecutionPolicy());
            var streamResult = rxStream.readArray(params.byte1DArray, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            // Copy the array and operate on the copy.
            rxStream.setExecutionPolicy(StreamExecutionPolicy.THREAD_SAFE);
            assertEquals(StreamExecutionPolicy.THREAD_SAFE, rxStream.getExecutionPolicy());
            streamResult = rxStream.readArray(params.byte1DArray, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            streamResult = rxStream.readBuffer(params.byteBuffer, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            // We should only be able to accept even-length buffers since we're writing
            // samples.
            var arrReadEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    rxStream.readArray(new byte[mtu-1], params.timeoutUs);
                });
            assertTrue(arrReadEx.getMessage().contains("interleaved"));

            var buffReadEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    rxStream.readBuffer(ByteBuffer.allocateDirect(mtu-1), params.timeoutUs);
                });
            assertTrue(buffReadEx.getMessage().contains("divisible"));

            // We need the buffer to be direct so we can access the underlying memory through the JNI.
            var indirectBuffReadEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    rxStream.readBuffer(ByteBuffer.allocate(mtu), params.timeoutUs);
                });
            assertTrue(indirectBuffReadEx.getMessage().contains("direct"));

            // Make sure we can't read into a read-only buffer.
            var readonlyBuffReadEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    rxStream.readBuffer(params.byteBuffer.asReadOnlyBuffer(), params.timeoutUs);
                });
            assertTrue(readonlyBuffReadEx.getMessage().contains("read-only"));
        }
        else
        {
            var arrReadEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    rxStream.readArray(params.byte1DArray, params.timeoutUs);
                });
            assertTrue(arrReadEx.getMessage().contains(StreamFormat.CS8));

            var buffReadEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    rxStream.readBuffer(params.byteBuffer, params.timeoutUs);
                });
            assertTrue(buffReadEx.getMessage().contains(StreamFormat.CS8));
        }
    }

    //
    // CS16
    //

    private void test1DTxStreamWriteCS16(
        TxStream txStream,
        boolean streamFormatMatches)
    {
        assertEquals(1, txStream.getChannels().size());

        var mtu = (int)txStream.getMTU();
        assertEquals(1024, mtu);

        if(streamFormatMatches)
        {
            // Pause the JNI and get the direct heap location to the buffer.
            txStream.setExecutionPolicy(StreamExecutionPolicy.EFFICIENT);
            assertEquals(StreamExecutionPolicy.EFFICIENT, txStream.getExecutionPolicy());
            var streamResult = txStream.writeArray(params.short1DArray, params.timeNs, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            // Copy the array and operate on the copy.
            txStream.setExecutionPolicy(StreamExecutionPolicy.THREAD_SAFE);
            assertEquals(StreamExecutionPolicy.THREAD_SAFE, txStream.getExecutionPolicy());
            streamResult = txStream.writeArray(params.short1DArray, params.timeNs, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            streamResult = txStream.writeBuffer(params.shortBuffer, params.timeNs, params.timeoutUs);
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
                    txStream.writeArray(new short[mtu-1], params.timeNs, params.timeoutUs);
                });
            assertTrue(arrWriteEx.getMessage().contains("interleaved"));

            var buffWriteEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    txStream.writeBuffer(ByteBuffer.allocateDirect(mtu-1).asShortBuffer(), params.timeNs, params.timeoutUs);
                });
            assertTrue(buffWriteEx.getMessage().contains("divisible"));

            // We need the buffer to be direct so we can access the underlying memory through the JNI.
            var indirectBuffWriteEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    txStream.writeBuffer(ByteBuffer.allocate(mtu).asShortBuffer(), params.timeNs, params.timeoutUs);
                });
            assertTrue(indirectBuffWriteEx.getMessage().contains("direct"));
        }
        else
        {
            var arrWriteEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    txStream.writeArray(params.short1DArray, params.timeNs, params.timeoutUs);
                });
            assertTrue(arrWriteEx.getMessage().contains(StreamFormat.CS16));

            var buffWriteEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    txStream.writeBuffer(params.shortBuffer, params.timeNs, params.timeoutUs);
                });
            assertTrue(buffWriteEx.getMessage().contains(StreamFormat.CS16));
        }
    }

    private void test1DRxStreamReadCS16(
        RxStream rxStream,
        boolean streamFormatMatches)
    {
        assertEquals(1, rxStream.getChannels().size());

        var mtu = (int)rxStream.getMTU();
        assertEquals(1024, mtu);

        if(streamFormatMatches)
        {
            // Pause the JNI and get the direct heap location to the buffer.
            rxStream.setExecutionPolicy(StreamExecutionPolicy.EFFICIENT);
            assertEquals(StreamExecutionPolicy.EFFICIENT, rxStream.getExecutionPolicy());
            var streamResult = rxStream.readArray(params.short1DArray, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            // Copy the array and operate on the copy.
            rxStream.setExecutionPolicy(StreamExecutionPolicy.THREAD_SAFE);
            assertEquals(StreamExecutionPolicy.THREAD_SAFE, rxStream.getExecutionPolicy());
            streamResult = rxStream.readArray(params.short1DArray, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            streamResult = rxStream.readBuffer(params.shortBuffer, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            // We should only be able to accept even-length buffers since we're writing
            // samples.
            var arrReadEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    rxStream.readArray(new short[mtu-1], params.timeoutUs);
                });
            assertTrue(arrReadEx.getMessage().contains("interleaved"));

            var buffReadEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    rxStream.readBuffer(ByteBuffer.allocateDirect(mtu-1).asShortBuffer(), params.timeoutUs);
                });
            assertTrue(buffReadEx.getMessage().contains("divisible"));

            // We need the buffer to be direct so we can access the underlying memory through the JNI.
            var indirectBuffReadEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    rxStream.readBuffer(ByteBuffer.allocate(mtu).asShortBuffer(), params.timeoutUs);
                });
            assertTrue(indirectBuffReadEx.getMessage().contains("direct"));

            // Make sure we can't read into a read-only buffer.
            var readonlyBuffReadEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    rxStream.readBuffer(params.shortBuffer.asReadOnlyBuffer(), params.timeoutUs);
                });
            assertTrue(readonlyBuffReadEx.getMessage().contains("read-only"));
        }
        else
        {
            var arrReadEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    rxStream.readArray(params.short1DArray, params.timeoutUs);
                });
            assertTrue(arrReadEx.getMessage().contains(StreamFormat.CS16));

            var buffReadEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    rxStream.readBuffer(params.shortBuffer, params.timeoutUs);
                });
            assertTrue(buffReadEx.getMessage().contains(StreamFormat.CS16));
        }
    }

    //
    // CS32
    //

    private void test1DTxStreamWriteCS32(
        TxStream txStream,
        boolean streamFormatMatches)
    {
        assertEquals(1, txStream.getChannels().size());

        var mtu = (int)txStream.getMTU();
        assertEquals(1024, mtu);

        if(streamFormatMatches)
        {
            // Pause the JNI and get the direct heap location to the buffer.
            txStream.setExecutionPolicy(StreamExecutionPolicy.EFFICIENT);
            assertEquals(StreamExecutionPolicy.EFFICIENT, txStream.getExecutionPolicy());
            var streamResult = txStream.writeArray(params.int1DArray, params.timeNs, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            // Copy the array and operate on the copy.
            txStream.setExecutionPolicy(StreamExecutionPolicy.THREAD_SAFE);
            assertEquals(StreamExecutionPolicy.THREAD_SAFE, txStream.getExecutionPolicy());
            streamResult = txStream.writeArray(params.int1DArray, params.timeNs, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            streamResult = txStream.writeBuffer(params.intBuffer, params.timeNs, params.timeoutUs);
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
                    txStream.writeArray(new int[mtu-1], params.timeNs, params.timeoutUs);
                });
            assertTrue(arrWriteEx.getMessage().contains("interleaved"));

            var buffWriteEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    txStream.writeBuffer(ByteBuffer.allocateDirect(mtu-1).asIntBuffer(), params.timeNs, params.timeoutUs);
                });
            assertTrue(buffWriteEx.getMessage().contains("divisible"));

            // We need the buffer to be direct so we can access the underlying memory through the JNI.
            var indirectBuffWriteEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    txStream.writeBuffer(ByteBuffer.allocate(mtu).asIntBuffer(), params.timeNs, params.timeoutUs);
                });
            assertTrue(indirectBuffWriteEx.getMessage().contains("direct"));
        }
        else
        {
            var arrWriteEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    txStream.writeArray(params.int1DArray, params.timeNs, params.timeoutUs);
                });
            assertTrue(arrWriteEx.getMessage().contains(StreamFormat.CS32));

            var buffWriteEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    txStream.writeBuffer(params.intBuffer, params.timeNs, params.timeoutUs);
                });
            assertTrue(buffWriteEx.getMessage().contains(StreamFormat.CS32));
        }
    }

    private void test1DRxStreamReadCS32(
        RxStream rxStream,
        boolean streamFormatMatches)
    {
        assertEquals(1, rxStream.getChannels().size());

        var mtu = (int)rxStream.getMTU();
        assertEquals(1024, mtu);

        if(streamFormatMatches)
        {
            // Pause the JNI and get the direct heap location to the buffer.
            rxStream.setExecutionPolicy(StreamExecutionPolicy.EFFICIENT);
            assertEquals(StreamExecutionPolicy.EFFICIENT, rxStream.getExecutionPolicy());
            var streamResult = rxStream.readArray(params.int1DArray, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            // Copy the array and operate on the copy.
            rxStream.setExecutionPolicy(StreamExecutionPolicy.THREAD_SAFE);
            assertEquals(StreamExecutionPolicy.THREAD_SAFE, rxStream.getExecutionPolicy());
            streamResult = rxStream.readArray(params.int1DArray, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            streamResult = rxStream.readBuffer(params.intBuffer, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            // We should only be able to accept even-length buffers since we're writing
            // samples.
            var arrReadEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    rxStream.readArray(new int[mtu-1], params.timeoutUs);
                });
            assertTrue(arrReadEx.getMessage().contains("interleaved"));

            var buffReadEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    rxStream.readBuffer(ByteBuffer.allocateDirect(mtu-1).asIntBuffer(), params.timeoutUs);
                });
            assertTrue(buffReadEx.getMessage().contains("divisible"));

            // We need the buffer to be direct so we can access the underlying memory through the JNI.
            var indirectBuffReadEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    rxStream.readBuffer(ByteBuffer.allocate(mtu).asIntBuffer(), params.timeoutUs);
                });
            assertTrue(indirectBuffReadEx.getMessage().contains("direct"));

            // Make sure we can't read into a read-only buffer.
            var readonlyBuffReadEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    rxStream.readBuffer(params.intBuffer.asReadOnlyBuffer(), params.timeoutUs);
                });
            assertTrue(readonlyBuffReadEx.getMessage().contains("read-only"));
        }
        else
        {
            var arrReadEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    rxStream.readArray(params.int1DArray, params.timeoutUs);
                });
            assertTrue(arrReadEx.getMessage().contains(StreamFormat.CS32));

            var buffReadEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    rxStream.readBuffer(params.intBuffer, params.timeoutUs);
                });
            assertTrue(buffReadEx.getMessage().contains(StreamFormat.CS32));
        }
    }

    //
    // CF32
    //

    private void test1DTxStreamWriteCF32(
        TxStream txStream,
        boolean streamFormatMatches)
    {
        assertEquals(1, txStream.getChannels().size());

        var mtu = (int)txStream.getMTU();
        assertEquals(1024, mtu);

        if(streamFormatMatches)
        {
            // Pause the JNI and get the direct heap location to the buffer.
            txStream.setExecutionPolicy(StreamExecutionPolicy.EFFICIENT);
            assertEquals(StreamExecutionPolicy.EFFICIENT, txStream.getExecutionPolicy());
            var streamResult = txStream.writeArray(params.float1DArray, params.timeNs, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            // Copy the array and operate on the copy.
            txStream.setExecutionPolicy(StreamExecutionPolicy.THREAD_SAFE);
            assertEquals(StreamExecutionPolicy.THREAD_SAFE, txStream.getExecutionPolicy());
            streamResult = txStream.writeArray(params.float1DArray, params.timeNs, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            streamResult = txStream.writeBuffer(params.floatBuffer, params.timeNs, params.timeoutUs);
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
                    txStream.writeArray(new float[mtu-1], params.timeNs, params.timeoutUs);
                });
            assertTrue(arrWriteEx.getMessage().contains("interleaved"));

            var buffWriteEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    txStream.writeBuffer(ByteBuffer.allocateDirect(mtu-1).asFloatBuffer(), params.timeNs, params.timeoutUs);
                });
            assertTrue(buffWriteEx.getMessage().contains("divisible"));

            // We need the buffer to be direct so we can access the underlying memory through the JNI.
            var indirectBuffWriteEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    txStream.writeBuffer(ByteBuffer.allocate(mtu).asFloatBuffer(), params.timeNs, params.timeoutUs);
                });
            assertTrue(indirectBuffWriteEx.getMessage().contains("direct"));
        }
        else
        {
            var arrWriteEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    txStream.writeArray(params.float1DArray, params.timeNs, params.timeoutUs);
                });
            assertTrue(arrWriteEx.getMessage().contains(StreamFormat.CF32));

            var buffWriteEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    txStream.writeBuffer(params.floatBuffer, params.timeNs, params.timeoutUs);
                });
            assertTrue(buffWriteEx.getMessage().contains(StreamFormat.CF32));
        }
    }

    private void test1DRxStreamReadCF32(
        RxStream rxStream,
        boolean streamFormatMatches)
    {
        assertEquals(1, rxStream.getChannels().size());

        var mtu = (int)rxStream.getMTU();
        assertEquals(1024, mtu);

        if(streamFormatMatches)
        {
            // Pause the JNI and get the direct heap location to the buffer.
            rxStream.setExecutionPolicy(StreamExecutionPolicy.EFFICIENT);
            assertEquals(StreamExecutionPolicy.EFFICIENT, rxStream.getExecutionPolicy());
            var streamResult = rxStream.readArray(params.float1DArray, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            // Copy the array and operate on the copy.
            rxStream.setExecutionPolicy(StreamExecutionPolicy.THREAD_SAFE);
            assertEquals(StreamExecutionPolicy.THREAD_SAFE, rxStream.getExecutionPolicy());
            streamResult = rxStream.readArray(params.float1DArray, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            streamResult = rxStream.readBuffer(params.floatBuffer, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            // We should only be able to accept even-length buffers since we're writing
            // samples.
            var arrReadEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    rxStream.readArray(new float[mtu-1], params.timeoutUs);
                });
            assertTrue(arrReadEx.getMessage().contains("interleaved"));

            var buffReadEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    rxStream.readBuffer(ByteBuffer.allocateDirect(mtu-1).asFloatBuffer(), params.timeoutUs);
                });
            assertTrue(buffReadEx.getMessage().contains("divisible"));

            // We need the buffer to be direct so we can access the underlying memory through the JNI.
            var indirectBuffReadEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    rxStream.readBuffer(ByteBuffer.allocate(mtu).asFloatBuffer(), params.timeoutUs);
                });
            assertTrue(indirectBuffReadEx.getMessage().contains("direct"));

            // Make sure we can't read into a read-only buffer.
            var readonlyBuffReadEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    rxStream.readBuffer(params.floatBuffer.asReadOnlyBuffer(), params.timeoutUs);
                });
            assertTrue(readonlyBuffReadEx.getMessage().contains("read-only"));
        }
        else
        {
            var arrReadEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    rxStream.readArray(params.float1DArray, params.timeoutUs);
                });
            assertTrue(arrReadEx.getMessage().contains(StreamFormat.CF32));

            var buffReadEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    rxStream.readBuffer(params.floatBuffer, params.timeoutUs);
                });
            assertTrue(buffReadEx.getMessage().contains(StreamFormat.CF32));
        }
    }

    //
    // CF64
    //

    private void test1DTxStreamWriteCF64(
        TxStream txStream,
        boolean streamFormatMatches)
    {
        assertEquals(1, txStream.getChannels().size());

        var mtu = (int)txStream.getMTU();
        assertEquals(1024, mtu);

        if(streamFormatMatches)
        {
            // Pause the JNI and get the direct heap location to the buffer.
            txStream.setExecutionPolicy(StreamExecutionPolicy.EFFICIENT);
            assertEquals(StreamExecutionPolicy.EFFICIENT, txStream.getExecutionPolicy());
            var streamResult = txStream.writeArray(params.double1DArray, params.timeNs, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            // Copy the array and operate on the copy.
            txStream.setExecutionPolicy(StreamExecutionPolicy.THREAD_SAFE);
            assertEquals(StreamExecutionPolicy.THREAD_SAFE, txStream.getExecutionPolicy());
            streamResult = txStream.writeArray(params.double1DArray, params.timeNs, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            streamResult = txStream.writeBuffer(params.doubleBuffer, params.timeNs, params.timeoutUs);
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
                    txStream.writeArray(new double[mtu-1], params.timeNs, params.timeoutUs);
                });
            assertTrue(arrWriteEx.getMessage().contains("interleaved"));

            var buffWriteEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    txStream.writeBuffer(ByteBuffer.allocateDirect(mtu-1).asDoubleBuffer(), params.timeNs, params.timeoutUs);
                });
            assertTrue(buffWriteEx.getMessage().contains("divisible"));

            // We need the buffer to be direct so we can access the underlying memory through the JNI.
            var indirectBuffWriteEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    txStream.writeBuffer(ByteBuffer.allocate(mtu).asDoubleBuffer(), params.timeNs, params.timeoutUs);
                });
            assertTrue(indirectBuffWriteEx.getMessage().contains("direct"));
        }
        else
        {
            var arrWriteEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    txStream.writeArray(params.double1DArray, params.timeNs, params.timeoutUs);
                });
            assertTrue(arrWriteEx.getMessage().contains(StreamFormat.CF64));

            var buffWriteEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    txStream.writeBuffer(params.doubleBuffer, params.timeNs, params.timeoutUs);
                });
            assertTrue(buffWriteEx.getMessage().contains(StreamFormat.CF64));
        }
    }

    private void test1DRxStreamReadCF64(
        RxStream rxStream,
        boolean streamFormatMatches)
    {
        assertEquals(1, rxStream.getChannels().size());

        var mtu = (int)rxStream.getMTU();
        assertEquals(1024, mtu);

        if(streamFormatMatches)
        {
            // Pause the JNI and get the direct heap location to the buffer.
            rxStream.setExecutionPolicy(StreamExecutionPolicy.EFFICIENT);
            assertEquals(StreamExecutionPolicy.EFFICIENT, rxStream.getExecutionPolicy());
            var streamResult = rxStream.readArray(params.double1DArray, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            // Copy the array and operate on the copy.
            rxStream.setExecutionPolicy(StreamExecutionPolicy.THREAD_SAFE);
            assertEquals(StreamExecutionPolicy.THREAD_SAFE, rxStream.getExecutionPolicy());
            streamResult = rxStream.readArray(params.double1DArray, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            streamResult = rxStream.readBuffer(params.doubleBuffer, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            // We should only be able to accept even-length buffers since we're writing
            // samples.
            var arrReadEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    rxStream.readArray(new double[mtu-1], params.timeoutUs);
                });
            assertTrue(arrReadEx.getMessage().contains("interleaved"));

            var buffReadEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    rxStream.readBuffer(ByteBuffer.allocateDirect(mtu-1).asDoubleBuffer(), params.timeoutUs);
                });
            assertTrue(buffReadEx.getMessage().contains("divisible"));

            // We need the buffer to be direct so we can access the underlying memory through the JNI.
            var indirectBuffReadEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    rxStream.readBuffer(ByteBuffer.allocate(mtu).asDoubleBuffer(), params.timeoutUs);
                });
            assertTrue(indirectBuffReadEx.getMessage().contains("direct"));

            // Make sure we can't read into a read-only buffer.
            var readonlyBuffReadEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    rxStream.readBuffer(params.doubleBuffer.asReadOnlyBuffer(), params.timeoutUs);
                });
            assertTrue(readonlyBuffReadEx.getMessage().contains("read-only"));
        }
        else
        {
            var arrReadEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                    rxStream.readArray(params.double1DArray, params.timeoutUs);
                });
            assertTrue(arrReadEx.getMessage().contains(StreamFormat.CF64));

            var buffReadEx = assertThrows(
                IllegalArgumentException.class,
                () ->
                {
                        rxStream.readBuffer(params.doubleBuffer, params.timeoutUs);
                });
            assertTrue(buffReadEx.getMessage().contains(StreamFormat.CF64));
        }
    }

    //
    // Tests
    //

    private void testTxStreaming(String format)
    {
        var device = getTestDevice();

        //
        // Test with single channel
        //

        var txStream = device.setupTxStream(format, params.oneChannel, params.streamArgsMap);
        assertEquals(format, txStream.getFormat());
        // TODO: compare channels when stream returns int[]
        assertEquals(params.streamArgsMap, txStream.getArgs());
        assertFalse(txStream.active());

        assertEquals(ErrorCode.NOT_SUPPORTED, txStream.activate(params.streamFlags, params.timeNs, params.timeoutUs));

        test1DTxStreamWriteCS8(txStream, format.equals(StreamFormat.CS8));
        test1DTxStreamWriteCS16(txStream, format.equals(StreamFormat.CS16));
        test1DTxStreamWriteCS32(txStream, format.equals(StreamFormat.CS32));
        test1DTxStreamWriteCF32(txStream, format.equals(StreamFormat.CF32));
        test1DTxStreamWriteCF64(txStream, format.equals(StreamFormat.CF64));

        //
        // Test with multiple channels
        //

        txStream = device.setupTxStream(format, params.twoChannels, params.streamArgsMap);
        assertEquals(format, txStream.getFormat());
        // TODO: compare channels when stream returns int[]
        assertEquals(params.streamArgsMap, txStream.getArgs());
        assertFalse(txStream.active());

        assertEquals(ErrorCode.NOT_SUPPORTED, txStream.activate(params.streamFlags, params.timeNs, params.timeoutUs));

        test2DTxStreamWriteCS8(txStream, format.equals(StreamFormat.CS8));
    }

    private void testRxStreaming(String format)
    {
        var device = getTestDevice();

        //
        // Test with single channel
        //

        var rxStream = device.setupRxStream(format, params.oneChannel, params.streamArgsMap);
        assertEquals(format, rxStream.getFormat());
        // TODO: compare channels when stream returns int[]
        assertEquals(params.streamArgsMap, rxStream.getArgs());
        assertFalse(rxStream.active());

        assertEquals(ErrorCode.NOT_SUPPORTED, rxStream.activate(params.streamFlags, params.timeNs, params.timeoutUs));

        test1DRxStreamReadCS8(rxStream, format.equals(StreamFormat.CS8));
        test1DRxStreamReadCS16(rxStream, format.equals(StreamFormat.CS16));
        test1DRxStreamReadCS32(rxStream, format.equals(StreamFormat.CS32));
        test1DRxStreamReadCF32(rxStream, format.equals(StreamFormat.CF32));
        test1DRxStreamReadCF64(rxStream, format.equals(StreamFormat.CF64));
    }

    // For some reason, parameterized tests weren't working.

    @Test
    public void testTxStreamingCS8()
    {
        testTxStreaming(StreamFormat.CS8);
    }

    @Test
    public void testTxStreamingCS16()
    {
        testTxStreaming(StreamFormat.CS16);
    }

    @Test
    public void testTxStreamingCS32()
    {
        testTxStreaming(StreamFormat.CS32);
    }

    @Test
    public void testTxStreamingCF32()
    {
        testTxStreaming(StreamFormat.CF32);
    }

    @Test
    public void testTxStreamingCF64()
    {
        testTxStreaming(StreamFormat.CF64);
    }

    @Test
    public void testRxStreamingCS8()
    {
        testRxStreaming(StreamFormat.CS8);
    }

    @Test
    public void testRxStreamingCS16()
    {
        testRxStreaming(StreamFormat.CS16);
    }

    @Test
    public void testRxStreamingCS32()
    {
        testRxStreaming(StreamFormat.CS32);
    }

    @Test
    public void testRxStreamingCF32()
    {
        testRxStreaming(StreamFormat.CF32);
    }

    @Test
    public void testRxStreamingCF64()
    {
        testRxStreaming(StreamFormat.CF64);
    }
}
