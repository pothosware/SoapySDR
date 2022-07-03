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
        public int streamElems = 1024;

        public int[] oneChannel = new int[]{0};
        public int[] twoChannels = new int[]{0,1};
        public String streamArgsString = "bufflen=8192,buffers=15";
        public Kwargs streamArgsMap = new Kwargs();
        public HashSet<StreamFlags> streamFlags = new HashSet<StreamFlags>();
        public long timeNs = 1000;
        public int timeoutUs = 1000;
        public long numElems = 1024;

        public byte[] byteArray = null;
        public ByteBuffer byteBuffer = null;

        public short[] shortArray = null;
        public ShortBuffer shortBuffer = null;

        public int[] intArray = null;
        public IntBuffer intBuffer = null;

        public float[] floatArray = null;
        public FloatBuffer floatBuffer = null;

        public double[] doubleArray = null;
        public DoubleBuffer doubleBuffer = null;

        public TestParams()
        {
            streamArgsMap.put("bufflen", "8192");
            streamArgsMap.put("buffers", "15");

            streamFlags.add(StreamFlags.HAS_TIME);
            streamFlags.add(StreamFlags.END_BURST);

            byteArray = new byte[streamElems];
            byteBuffer = ByteBuffer.allocateDirect(streamElems);

            //
            // For some reason, only ByteBuffer has allocateDirect
            //

            shortArray = new short[streamElems*2];
            shortBuffer = ByteBuffer.allocateDirect(streamElems*2*2).asShortBuffer();

            intArray = new int[streamElems*2];
            intBuffer = ByteBuffer.allocateDirect(streamElems*2*4).asIntBuffer();

            floatArray = new float[streamElems*2];
            floatBuffer = ByteBuffer.allocateDirect(streamElems*2*4).asFloatBuffer();

            doubleArray = new double[streamElems*2];
            doubleBuffer = ByteBuffer.allocateDirect(streamElems*2*8).asDoubleBuffer();
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
    // CS8
    //

    private void testTxStreamWriteCS8(
        TxStream txStream,
        TestParams params,
        boolean streamFormatMatches)
    {
        var mtu = (int)txStream.getMTU();
        assertEquals(1024, mtu);

        if(streamFormatMatches)
        {
            // Pause the JNI and get the direct heap location to the buffer.
            txStream.setExecutionPolicy(StreamExecutionPolicy.EFFICIENT);
            assertEquals(StreamExecutionPolicy.EFFICIENT, txStream.getExecutionPolicy());
            var streamResult = txStream.writeArray(params.byteArray, params.timeNs, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            // Copy the array and operate on the copy.
            txStream.setExecutionPolicy(StreamExecutionPolicy.THREAD_SAFE);
            assertEquals(StreamExecutionPolicy.THREAD_SAFE, txStream.getExecutionPolicy());
            streamResult = txStream.writeArray(params.byteArray, params.timeNs, params.timeoutUs);
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
                    txStream.writeArray(params.byteArray, params.timeNs, params.timeoutUs);
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

    private void testRxStreamReadCS8(
        RxStream rxStream,
        TestParams params,
        boolean streamFormatMatches)
    {
        var mtu = (int)rxStream.getMTU();
        assertEquals(1024, mtu);

        if(streamFormatMatches)
        {
            // Pause the JNI and get the direct heap location to the buffer.
            rxStream.setExecutionPolicy(StreamExecutionPolicy.EFFICIENT);
            assertEquals(StreamExecutionPolicy.EFFICIENT, rxStream.getExecutionPolicy());
            var streamResult = rxStream.readArray(params.byteArray, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            // Copy the array and operate on the copy.
            rxStream.setExecutionPolicy(StreamExecutionPolicy.THREAD_SAFE);
            assertEquals(StreamExecutionPolicy.THREAD_SAFE, rxStream.getExecutionPolicy());
            streamResult = rxStream.readArray(params.byteArray, params.timeoutUs);
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
                    rxStream.readArray(params.byteArray, params.timeoutUs);
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

    private void testTxStreamWriteCS16(
        TxStream txStream,
        TestParams params,
        boolean streamFormatMatches)
    {
        var mtu = (int)txStream.getMTU();
        assertEquals(1024, mtu);

        if(streamFormatMatches)
        {
            // Pause the JNI and get the direct heap location to the buffer.
            txStream.setExecutionPolicy(StreamExecutionPolicy.EFFICIENT);
            assertEquals(StreamExecutionPolicy.EFFICIENT, txStream.getExecutionPolicy());
            var streamResult = txStream.writeArray(params.shortArray, params.timeNs, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            // Copy the array and operate on the copy.
            txStream.setExecutionPolicy(StreamExecutionPolicy.THREAD_SAFE);
            assertEquals(StreamExecutionPolicy.THREAD_SAFE, txStream.getExecutionPolicy());
            streamResult = txStream.writeArray(params.shortArray, params.timeNs, params.timeoutUs);
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
                    txStream.writeArray(params.shortArray, params.timeNs, params.timeoutUs);
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

    private void testRxStreamReadCS16(
        RxStream rxStream,
        TestParams params,
        boolean streamFormatMatches)
    {
        var mtu = (int)rxStream.getMTU();
        assertEquals(1024, mtu);

        if(streamFormatMatches)
        {
            // Pause the JNI and get the direct heap location to the buffer.
            rxStream.setExecutionPolicy(StreamExecutionPolicy.EFFICIENT);
            assertEquals(StreamExecutionPolicy.EFFICIENT, rxStream.getExecutionPolicy());
            var streamResult = rxStream.readArray(params.shortArray, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            // Copy the array and operate on the copy.
            rxStream.setExecutionPolicy(StreamExecutionPolicy.THREAD_SAFE);
            assertEquals(StreamExecutionPolicy.THREAD_SAFE, rxStream.getExecutionPolicy());
            streamResult = rxStream.readArray(params.shortArray, params.timeoutUs);
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
                    rxStream.readArray(params.shortArray, params.timeoutUs);
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

    private void testTxStreamWriteCS32(
        TxStream txStream,
        TestParams params,
        boolean streamFormatMatches)
    {
        var mtu = (int)txStream.getMTU();
        assertEquals(1024, mtu);

        if(streamFormatMatches)
        {
            // Pause the JNI and get the direct heap location to the buffer.
            txStream.setExecutionPolicy(StreamExecutionPolicy.EFFICIENT);
            assertEquals(StreamExecutionPolicy.EFFICIENT, txStream.getExecutionPolicy());
            var streamResult = txStream.writeArray(params.intArray, params.timeNs, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            // Copy the array and operate on the copy.
            txStream.setExecutionPolicy(StreamExecutionPolicy.THREAD_SAFE);
            assertEquals(StreamExecutionPolicy.THREAD_SAFE, txStream.getExecutionPolicy());
            streamResult = txStream.writeArray(params.intArray, params.timeNs, params.timeoutUs);
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
                    txStream.writeArray(params.intArray, params.timeNs, params.timeoutUs);
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

    private void testRxStreamReadCS32(
        RxStream rxStream,
        TestParams params,
        boolean streamFormatMatches)
    {
        var mtu = (int)rxStream.getMTU();
        assertEquals(1024, mtu);

        if(streamFormatMatches)
        {
            // Pause the JNI and get the direct heap location to the buffer.
            rxStream.setExecutionPolicy(StreamExecutionPolicy.EFFICIENT);
            assertEquals(StreamExecutionPolicy.EFFICIENT, rxStream.getExecutionPolicy());
            var streamResult = rxStream.readArray(params.intArray, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            // Copy the array and operate on the copy.
            rxStream.setExecutionPolicy(StreamExecutionPolicy.THREAD_SAFE);
            assertEquals(StreamExecutionPolicy.THREAD_SAFE, rxStream.getExecutionPolicy());
            streamResult = rxStream.readArray(params.intArray, params.timeoutUs);
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
                    rxStream.readArray(params.intArray, params.timeoutUs);
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

    private void testTxStreamWriteCF32(
        TxStream txStream,
        TestParams params,
        boolean streamFormatMatches)
    {
        var mtu = (int)txStream.getMTU();
        assertEquals(1024, mtu);

        if(streamFormatMatches)
        {
            // Pause the JNI and get the direct heap location to the buffer.
            txStream.setExecutionPolicy(StreamExecutionPolicy.EFFICIENT);
            assertEquals(StreamExecutionPolicy.EFFICIENT, txStream.getExecutionPolicy());
            var streamResult = txStream.writeArray(params.floatArray, params.timeNs, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            // Copy the array and operate on the copy.
            txStream.setExecutionPolicy(StreamExecutionPolicy.THREAD_SAFE);
            assertEquals(StreamExecutionPolicy.THREAD_SAFE, txStream.getExecutionPolicy());
            streamResult = txStream.writeArray(params.floatArray, params.timeNs, params.timeoutUs);
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
                    txStream.writeArray(params.floatArray, params.timeNs, params.timeoutUs);
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

    private void testRxStreamReadCF32(
        RxStream rxStream,
        TestParams params,
        boolean streamFormatMatches)
    {
        var mtu = (int)rxStream.getMTU();
        assertEquals(1024, mtu);

        if(streamFormatMatches)
        {
            // Pause the JNI and get the direct heap location to the buffer.
            rxStream.setExecutionPolicy(StreamExecutionPolicy.EFFICIENT);
            assertEquals(StreamExecutionPolicy.EFFICIENT, rxStream.getExecutionPolicy());
            var streamResult = rxStream.readArray(params.floatArray, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            // Copy the array and operate on the copy.
            rxStream.setExecutionPolicy(StreamExecutionPolicy.THREAD_SAFE);
            assertEquals(StreamExecutionPolicy.THREAD_SAFE, rxStream.getExecutionPolicy());
            streamResult = rxStream.readArray(params.floatArray, params.timeoutUs);
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

            // Make sure we can't read floato a read-only buffer.
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
                    rxStream.readArray(params.floatArray, params.timeoutUs);
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

    private void testTxStreamWriteCF64(
        TxStream txStream,
        TestParams params,
        boolean streamFormatMatches)
    {
        var mtu = (int)txStream.getMTU();
        assertEquals(1024, mtu);

        if(streamFormatMatches)
        {
            // Pause the JNI and get the direct heap location to the buffer.
            txStream.setExecutionPolicy(StreamExecutionPolicy.EFFICIENT);
            assertEquals(StreamExecutionPolicy.EFFICIENT, txStream.getExecutionPolicy());
            var streamResult = txStream.writeArray(params.doubleArray, params.timeNs, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            // Copy the array and operate on the copy.
            txStream.setExecutionPolicy(StreamExecutionPolicy.THREAD_SAFE);
            assertEquals(StreamExecutionPolicy.THREAD_SAFE, txStream.getExecutionPolicy());
            streamResult = txStream.writeArray(params.doubleArray, params.timeNs, params.timeoutUs);
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
                    txStream.writeArray(params.doubleArray, params.timeNs, params.timeoutUs);
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

    private void testRxStreamReadCF64(
        RxStream rxStream,
        TestParams params,
        boolean streamFormatMatches)
    {
        var mtu = (int)rxStream.getMTU();
        assertEquals(1024, mtu);

        if(streamFormatMatches)
        {
            // Pause the JNI and get the direct heap location to the buffer.
            rxStream.setExecutionPolicy(StreamExecutionPolicy.EFFICIENT);
            assertEquals(StreamExecutionPolicy.EFFICIENT, rxStream.getExecutionPolicy());
            var streamResult = rxStream.readArray(params.doubleArray, params.timeoutUs);
            assertEquals(ErrorCode.NOT_SUPPORTED, streamResult.getErrorCode());
            assertEquals(0, streamResult.getNumSamples());

            // Copy the array and operate on the copy.
            rxStream.setExecutionPolicy(StreamExecutionPolicy.THREAD_SAFE);
            assertEquals(StreamExecutionPolicy.THREAD_SAFE, rxStream.getExecutionPolicy());
            streamResult = rxStream.readArray(params.doubleArray, params.timeoutUs);
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

            // Make sure we can't read doubleo a read-only buffer.
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
                    rxStream.readArray(params.doubleArray, params.timeoutUs);
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
        var params = new TestParams();

        //
        // Test with single channel
        //

        var txStream = device.setupTxStream(format, params.oneChannel, params.streamArgsMap);
        assertEquals(format, txStream.getFormat());
        // TODO: compare channels when stream returns int[]
        assertEquals(params.streamArgsMap, txStream.getArgs());
        assertFalse(txStream.active());

        assertEquals(ErrorCode.NOT_SUPPORTED, txStream.activate(params.streamFlags, params.timeNs, params.timeoutUs));

        testTxStreamWriteCS8(txStream, params, format.equals(StreamFormat.CS8));
        testTxStreamWriteCS16(txStream, params, format.equals(StreamFormat.CS16));
        testTxStreamWriteCS32(txStream, params, format.equals(StreamFormat.CS32));
        testTxStreamWriteCF32(txStream, params, format.equals(StreamFormat.CF32));
        testTxStreamWriteCF64(txStream, params, format.equals(StreamFormat.CF64));
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
}
