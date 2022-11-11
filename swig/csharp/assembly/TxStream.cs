// Copyright (c) 2020-2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

using System;
using System.Buffers;
using System.Linq;
using System.Runtime.InteropServices;

namespace Pothosware.SoapySDR
{
    /// <summary>
    /// A class representing a transmit stream. With this class, you can write
    /// samples to your device to transmit.
    /// </summary>
    public class TxStream: Stream
    {
        internal TxStream(
            Device device,
            string format,
            uint[] channels,
            Kwargs kwargs
        ):
            base(device, format, channels, kwargs)
        {
            _streamHandle = device.SetupStreamInternal(Direction.Tx, format, new UnsignedListInternal(channels), kwargs);
        }

        /// <summary>
        /// Write data from an arbitrary memory source for a single-channel transmission.
        /// </summary>
        /// <typeparam name="T">The type of the underlying buffer to send. This type must match the stream's format type.</typeparam>
        /// <param name="memory">The buffer to transmit. For complex types, this buffer is interleaved.</param>
        /// <param name="flags">Optional input flags.</param>
        /// <param name="timeNs">The buffer's timestamp in nanoseconds.</param>
        /// <param name="timeoutUs">The operation timeout in microseconds.</param>
        /// <param name="result">An output to store stream metadata.</param>
        /// <returns>An error code for the stream operation.</returns>
        public unsafe ErrorCode Write<T>(
            ReadOnlyMemory<T> memory,
            StreamFlags flags,
            long timeNs,
            int timeoutUs,
            out StreamResult result) where T : unmanaged
        {
            return Write(
                memory.Span,
                flags,
                timeNs,
                timeoutUs,
                out result);
        }

        /// <summary>
        /// Write data from an arbitrary memory source for a multi-channel transmission.
        /// </summary>
        /// <typeparam name="T">The type of the underlying buffers to send. This type must match the stream's format type.</typeparam>
        /// <param name="memory">The buffers to transmit. For complex types, this buffer is interleaved. The array length must match the number of channels, and all ReadOnlyMemory<> instances must be of the same length.</param>
        /// <param name="flags">Optional input flags.</param>
        /// <param name="timeNs">The buffer's timestamp in nanoseconds.</param>
        /// <param name="timeoutUs">The operation timeout in microseconds.</param>
        /// <param name="result">An output to store stream metadata.</param>
        /// <returns>An error code for the stream operation.</returns>
        public unsafe ErrorCode Write<T>(
            ReadOnlyMemory<T>[] memory,
            StreamFlags flags,
            long timeNs,
            int timeoutUs,
            out StreamResult result) where T : unmanaged
        {
            ErrorCode ret;

            if (_streamHandle != null)
            {
                ValidateMemory(memory);

                var memsAsSizes = Utility.ToPointerListInternal(
                    memory,
#pragma warning disable IDE0059 // Unnecessary assignment of a value
                    out MemoryHandle[] memoryHandles);
#pragma warning restore IDE0059 // Unnecessary assignment of a value

                var deviceOutput = _device.WriteStreamInternal(
                    _streamHandle,
                    memsAsSizes,
                    (uint)memory[0].Length,
                    flags,
                    timeNs,
                    timeoutUs);

                result = deviceOutput.Second;
                ret = deviceOutput.First;
            }
            else throw new InvalidOperationException("Stream is closed");

            return ret;
        }

        /// <summary>
        /// Write data from an arbitrary memory source for a single-channel transmission.
        /// </summary>
        /// <typeparam name="T">The type of the underlying buffer to send. This type must match the stream's format type.</typeparam>
        /// <param name="span">The buffer to transmit. For complex types, this buffer is interleaved.</param>
        /// <param name="flags">Optional input flags.</param>
        /// <param name="timeNs">The buffer's timestamp in nanoseconds.</param>
        /// <param name="timeoutUs">The operation timeout in microseconds.</param>
        /// <param name="result">An output to store stream metadata.</param>
        /// <returns>An error code for the stream operation.</returns>
        public unsafe ErrorCode Write<T>(
            ReadOnlySpan<T> span,
            StreamFlags flags,
            long timeNs,
            int timeoutUs,
            out StreamResult result) where T : unmanaged
        {
            ValidateSpan(span);

            fixed(T* data = &MemoryMarshal.GetReference(span))
            {
                return Write((IntPtr)data, (uint)span.Length, flags, timeNs, timeoutUs, out result);
            }
        }

        /// <summary>
        /// Write data from a managed C# array for a single-channel transmission.
        /// </summary>
        /// <typeparam name="T">The type of the underlying array to send. This type must match the stream's format type.</typeparam>
        /// <param name="buff">The managed array to transmit. For complex types, this buffer is interleaved.</param>
        /// <param name="flags">Optional input flags.</param>
        /// <param name="timeNs">The buffer's timestamp in nanoseconds.</param>
        /// <param name="timeoutUs">The operation timeout in microseconds.</param>
        /// <param name="result">An output to store stream metadata.</param>
        /// <returns>An error code for the stream operation.</returns>
        public unsafe ErrorCode Write<T>(
            T[] buff,
            StreamFlags flags,
            long timeNs,
            int timeoutUs,
            out StreamResult result) where T : unmanaged
        {
            return Write(new ReadOnlySpan<T>(buff), flags, timeNs, timeoutUs, out result);
        }

        /// <summary>
        /// Write data from a managed C# array for a multi-channel transmission.
        /// </summary>
        /// <typeparam name="T">The type of the underlying arrays to send. This type must match the stream's format type.</typeparam>
        /// <param name="buffs">The managed arrays to transmit. For complex types, this buffer is interleaved. The outer array dimension must match the number of channels, and all inner arrays must be of the same length.</param>
        /// <param name="flags">Optional input flags.</param>
        /// <param name="timeNs">The buffer's timestamp in nanoseconds.</param>
        /// <param name="timeoutUs">The operation timeout in microseconds.</param>
        /// <param name="result">An output to store stream metadata.</param>
        /// <returns>An error code for the stream operation.</returns>
        public unsafe ErrorCode Write<T>(
            T[][] buffs,
            StreamFlags flags,
            long timeNs,
            int timeoutUs,
            out StreamResult result) where T : unmanaged
        {
            return Write(buffs.Select(buff => new ReadOnlyMemory<T>(buff)).ToArray(), flags, timeNs, timeoutUs, out result);
        }

        /// <summary>
        /// Write data from an unmanaged pointer for a single-channel transmission. Minimal validation is performed on input parameters.
        /// </summary>
        /// <param name="ptr">A pointer to the buffer to send.</param>
        /// <param name="numElems">The number of elements (of the stream format's size) in the underlying buffer.</param>
        /// <param name="flags">Optional input flags.</param>
        /// <param name="timeNs">The buffer's timestamp in nanoseconds.</param>
        /// <param name="timeoutUs">The operation timeout in microseconds.</param>
        /// <param name="result">An output to store stream metadata.</param>
        /// <returns>An error code for the stream operation.</returns>
        public unsafe ErrorCode Write(
            IntPtr ptr,
            uint numElems,
            StreamFlags flags,
            long timeNs,
            int timeoutUs,
            out StreamResult result)
        {
            return Write(
                new IntPtr[] { ptr },
                numElems,
                flags,
                timeNs,
                timeoutUs,
                out result);
        }

        /// <summary>
        /// Write data from unmanaged pointers for a multi-channel transmission. Minimal validation is performed on input parameters except channel count.
        /// </summary>
        /// <param name="ptrs">Pointers to the buffers to send.</param>
        /// <param name="numElems">The number of elements (of the stream format's size) in each underlying buffer.</param>
        /// <param name="flags">Optional input flags.</param>
        /// <param name="timeNs">The buffer's timestamp in nanoseconds.</param>
        /// <param name="timeoutUs">The operation timeout in microseconds.</param>
        /// <param name="result">An output to store stream metadata.</param>
        /// <returns>An error code for the stream operation.</returns>
        public unsafe ErrorCode Write(
            IntPtr[] ptrs,
            uint numElems,
            StreamFlags flags,
            long timeNs,
            int timeoutUs,
            out StreamResult result)
        {
            ErrorCode ret;

            if (_streamHandle != null)
            {
                ValidateIntPtrArray(ptrs);

                var deviceOutput = _device.WriteStreamInternal(
                    _streamHandle,
                    Utility.ToPointerListInternal(ptrs),
                    numElems,
                    flags,
                    timeNs,
                    timeoutUs);

                result = deviceOutput.Second;
                ret = deviceOutput.First;
            }
            else throw new InvalidOperationException("Stream is closed");

            return ret;
        }

        /// <summary>
        /// Read status information about the stream.
        /// </summary>
        /// <param name="timeoutUs">The operation timeout in microseconds.</param>
        /// <param name="result">An output to store the requested data.</param>
        /// <returns>An error code for the stream operation.</returns>
        public ErrorCode ReadStatus(int timeoutUs, out StreamResult result)
        {
            ErrorCode ret;
            result = new StreamResult();

            if (_streamHandle != null)
            {
                var deviceOutput = _device.ReadStreamStatusInternal(_streamHandle, timeoutUs);

                result = deviceOutput.Second;
                ret = deviceOutput.First;
            }
            else throw new InvalidOperationException("Stream is closed");

            return ret;
        }

        //
        // Object overrides
        //

        public override bool Equals(object other) => base.Equals(other);

        public override int GetHashCode() => base.GetHashCode();

        public override string ToString() => string.Format("{0}:{1} {2} TX stream (format: {3}, channels: {4})",
            _device.DriverKey,
            _device.HardwareKey,
            (_active ? "active" : "inactive"),
            Format,
            Channels);
    }
}
