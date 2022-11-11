// Copyright (c) 2021-2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

using System;
using System.Buffers;
using System.Linq;
using System.Runtime.InteropServices;

namespace Pothosware.SoapySDR
{
    /// <summary>
    /// A class representing a receive stream. With this class, you can receive
    /// samples from your device into provided buffers.
    /// </summary>
    public class RxStream: Stream
    {
        internal RxStream(
            Device device,
            string format,
            uint[] channels,
            Kwargs kwargs
        ):
            base(device, format, channels, kwargs)
        {
            _streamHandle = device.SetupStreamInternal(Direction.Rx, format, new UnsignedListInternal(channels), kwargs);
        }

        /// <summary>
        /// Receive data from a single channel into an arbitrary memory location.
        /// </summary>
        /// <typeparam name="T">The type of the given receive buffer. This type must match the stream's format type.</typeparam>
        /// <param name="memory">The buffer to receive into. For complex types, this buffer is interleaved.</param>
        /// <param name="timeoutUs">The operation timeout in microseconds.</param>
        /// <param name="result">An output to store stream metadata.</param>
        /// <returns>An error code for the stream operation.</returns>
        public unsafe ErrorCode Read<T>(
            Memory<T> memory,
            int timeoutUs,
            out StreamResult result) where T : unmanaged
        {
            return Read(
                new Memory<T>[] { memory },
                timeoutUs,
                out result);
        }

        /// <summary>
        /// Receive data from multiple channels into arbitrary memory locations.
        /// </summary>
        /// <typeparam name="T">The type of the given receive buffers. This type must match the stream's format type.</typeparam>
        /// <param name="memory">The buffers to receive into. For complex types, this buffer is interleaved. The array length must match the number of channels, and all Memory<> instances must be of the same length.</param>
        /// <param name="timeoutUs">The operation timeout in microseconds.</param>
        /// <param name="result">An output to store stream metadata.</param>
        /// <returns>An error code for the stream operation.</returns>
        public unsafe ErrorCode Read<T>(
            Memory<T>[] memory,
            int timeoutUs,
            out StreamResult result) where T : unmanaged
        {
            ErrorCode ret;

            if (_streamHandle != null)
            {
                ValidateMemory(memory);

                var memsAsSizes = Utility.ToPointerListInternal(
                    memory,
                    out MemoryHandle[] _);

                var deviceOutput = _device.ReadStreamInternal(
                    _streamHandle,
                    memsAsSizes,
                    (uint)memory[0].Length,
                    timeoutUs);

                result = deviceOutput.Second;
                ret = deviceOutput.First;
            }
            else throw new InvalidOperationException("Stream is closed");

            return ret;
        }

        /// <summary>
        /// Receive data from a single channel into an arbitrary memory location.
        /// </summary>
        /// <typeparam name="T">The type of the given receive buffer. This type must match the stream's format type.</typeparam>
        /// <param name="memory">The buffer to receive into. For complex types, this buffer is interleaved.</param>
        /// <param name="flags">Optional input flags.</param>
        /// <param name="timeNs">The buffer's timestamp in nanoseconds.</param>
        /// <param name="timeoutUs">The operation timeout in microseconds.</param>
        /// <param name="result">An output to store stream metadata.</param>
        /// <returns>An error code for the stream operation.</returns>
        public unsafe ErrorCode Read<T>(
            Span<T> span,
            int timeoutUs,
            out StreamResult result) where T : unmanaged
        {
            ValidateSpan(span);

            fixed(T* data = &MemoryMarshal.GetReference(span))
            {
                return Read((IntPtr)data, (uint)span.Length, timeoutUs, out result);
            }
        }

        /// <summary>
        /// Receive data from a single channel into a managed C# array.
        /// </summary>
        /// <typeparam name="T">The type of the underlying destination array. This type must match the stream's format type.</typeparam>
        /// <param name="buff">The managed array to receive into. For complex types, this buffer is interleaved.</param>
        /// <param name="timeoutUs">The operation timeout in microseconds.</param>
        /// <param name="result">An output to store stream metadata.</param>
        /// <returns>An error code for the stream operation.</returns>
        public unsafe ErrorCode Read<T>(
            ref T[] buff,
            int timeoutUs,
            out StreamResult result) where T: unmanaged
        {
            return Read(new Memory<T>(buff), timeoutUs, out result);
        }

        /// <summary>
        /// Receive data from multiple channels into a managed C# array.
        /// </summary>
        /// <typeparam name="T">The type of the underlying destination array. This type must match the stream's format type.</typeparam>
        /// <param name="buffs">The managed arrays to receive into. For complex types, this buffer is interleaved.</param>
        /// <param name="timeoutUs">The operation timeout in microseconds.</param>
        /// <param name="result">An output to store stream metadata.</param>
        /// <returns>An error code for the stream operation.</returns>
        public unsafe ErrorCode Read<T>(
            ref T[][] buffs,
            int timeoutUs,
            out StreamResult result) where T: unmanaged
        {
            return Read(buffs.Select(buff => new Memory<T>(buff)).ToArray(), timeoutUs, out result);
        }

        /// <summary>
        /// Receive data into an arbitrary memory location referenced by an unmanaged pointer. Minimal validation is performed on input parameters.
        /// </summary>
        /// <param name="ptr">A pointer to the buffer to receive into.</param>
        /// <param name="numElems">The number of elements (of the stream format's size) in the destination buffer.</param>
        /// <param name="timeoutUs">The operation timeout in microseconds.</param>
        /// <param name="result">An output to store stream metadata.</param>
        /// <returns>An error code for the stream operation.</returns>
        public unsafe ErrorCode Read(
            IntPtr ptr,
            uint numElems,
            int timeoutUs,
            out StreamResult result)
        {
            return Read(
                new IntPtr[] { ptr },
                numElems,
                timeoutUs,
                out result);
        }

        /// <summary>
        /// Receive data into arbitrary memory locations referenced by unmanaged pointers. Minimal validation is performed on input parameters except channel count.
        /// </summary>
        /// <param name="ptrs">Pointers to the receive buffers.</param>
        /// <param name="numElems">The number of elements (of the stream format's size) in each destination buffer.</param>
        /// <param name="timeoutUs">The operation timeout in microseconds.</param>
        /// <param name="result">An output to store stream metadata.</param>
        /// <returns>An error code for the stream operation.</returns>
        public unsafe ErrorCode Read(
            IntPtr[] ptrs,
            uint numElems,
            int timeoutUs,
            out StreamResult result)
        {
            ErrorCode ret;

            if (_streamHandle != null)
            {
                ValidateIntPtrArray(ptrs);

                var deviceOutput = _device.ReadStreamInternal(
                    _streamHandle,
                    Utility.ToPointerListInternal(ptrs),
                    numElems,
                    timeoutUs);

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

        public override string ToString() => string.Format("{0}:{1} {2} RX stream (format: {3}, channels: {4})",
            _device.DriverKey,
            _device.HardwareKey,
            (_active ? "active" : "inactive"),
            Format,
            Channels);
    }
}
