// Copyright (c) 2020-2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

using System;
using System.Linq;

namespace Pothosware.SoapySDR
{
    /// <summary>
    /// The base class for representing transmit or receive streams. This
    /// class will never be used itself. See TxStream and RxStream.
    /// </summary>
    public class Stream
    {
        internal Device _device = null;
        internal StreamHandle _streamHandle = null;
        protected bool _active = false;

        /// <summary>
        /// The underlying stream format. See Pothosware.SoapySDR.StreamFormat.
        /// </summary>
        public string Format { get; }

        /// <summary>
        /// The device channels used in this stream.
        /// </summary>
        public uint[] Channels { get; }

        /// <summary>
        /// The arguments used in creating this stream.
        /// </summary>
        public Kwargs StreamArgs { get; }

        /// <summary>
        /// Whether or not the stream is active.
        /// </summary>
        public bool Active => _active;

        // We already used these parameters to create the stream,
        // this is just for the sake of getters.
        internal Stream(
            Device device,
            string format,
            uint[] channels,
            Kwargs kwargs)
        {
            _device = device;

            Format = format;
            Channels = (uint[])channels.Clone();
            StreamArgs = kwargs;
        }

        ~Stream()
        {
            if(_active)               Deactivate();
            if(_streamHandle != null) Close();
        }

        /// <summary>
        /// The number of elements per channel this stream can handle in a single read/write call.
        /// </summary>
        public ulong MTU => _device.GetStreamMTUInternal(_streamHandle);

        /// <summary>
        /// Activate the stream to prepare it for read/write operations.
        /// </summary>
        /// <param name="flags">Optional stream flags.</param>
        /// <param name="timeNs">Optional activation time in nanoseconds. Only valid when flags includes Pothosware.SoapySDR.StreamFlags.HasTime.</param>
        /// <param name="numElems">Optional element count for burst control.</param>
        /// <returns>An error code for the stream activation.</returns>
        public ErrorCode Activate(
            StreamFlags flags = StreamFlags.None,
            long timeNs = 0,
            uint numElems = 0)
        {
            ErrorCode ret;
            if(_streamHandle != null)
            {
                if(!_active)
                {
                    ret = _device.ActivateStreamInternal(
                        _streamHandle,
                        flags,
                        timeNs,
                        numElems);

                    if (ret == ErrorCode.None) _active = true;
                }
                else throw new InvalidOperationException("Stream is already active");
            }
            else throw new InvalidOperationException("Stream is closed");

            return ret;
        }

        /// <summary>
        /// Deactivate the stream to end read/write operations.
        /// </summary>
        /// <param name="flags">Optional stream flags.</param>
        /// <param name="timeNs">Optional activation time in nanoseconds. Only valid when flags includes Pothosware.SoapySDR.StreamFlags.HasTime.</param>
        /// <param name="numElems">Optional element count for burst control.</param>
        /// <returns>An error code for the stream deactivation.</returns>
        public ErrorCode Deactivate(
            StreamFlags flags = StreamFlags.None,
            long timeNs = 0)
        {
            ErrorCode ret;
            if(_streamHandle != null)
            {
                if(_active)
                {
                    ret = _device.DeactivateStreamInternal(
                        _streamHandle,
                        flags,
                        timeNs);

                    if(ret == ErrorCode.None) _active = false;
                }
                else throw new InvalidOperationException("Stream is already inactive");
            }
            else throw new InvalidOperationException("Stream is closed");

            return ret;
        }

        /// <summary>
        /// Close the underlying stream.
        /// </summary>
        public void Close()
        {
            if (_streamHandle != null)
            {
                _device.CloseStreamInternal(_streamHandle);
                _streamHandle = null;
            }
            else throw new InvalidOperationException("Stream is already closed");
        }

        //
        // Utility
        //

        protected void ValidateSpan<T>(ReadOnlySpan<T> span) where T : unmanaged
        {
            var numChannels = _streamHandle.GetChannels().Length;
            var format = _streamHandle.GetFormat();
            var complexFormatString = Utility.GetFormatString<T>();

            if(numChannels != 1)
                throw new ArgumentException("Stream is configured for a single channel. Cannot accept multiple buffers.");
            else if (!format.Equals(complexFormatString))
                throw new ArgumentException(string.Format("Stream format \"{0}\" is incompatible with buffer type {1}.", format, typeof(T)));
            else if ((span.Length % 2) != 0)
                throw new ArgumentException("For complex interleaved streams, input buffers must be of an even size.");
        }

        protected void ValidateSpan<T>(Span<T> span) where T : unmanaged => ValidateSpan((ReadOnlySpan<T>)span);

        protected void ValidateMemory<T>(ReadOnlyMemory<T>[] mems) where T : unmanaged
        {
            var numChannels = _streamHandle.GetChannels().Length;
            var format = _streamHandle.GetFormat();

            var complexFormatString = Utility.GetFormatString<T>();

            if (mems == null)
                throw new ArgumentNullException("mems");
            else if (numChannels != mems.Length)
                throw new ArgumentException(string.Format("Stream is configured for {0} channel(s). Cannot accept {1} buffer(s).", numChannels, mems.Length));
            else if (!format.Equals(complexFormatString))
                throw new ArgumentException(string.Format("Stream format \"{0}\" is incompatible with buffer type {1}.", format, typeof(T)));
            else if (mems.Select(buff => buff.Length).Distinct().Count() > 1)
                throw new ArgumentException("All buffers must be of the same length");
            else if ((mems[0].Length % 2) != 0)
                throw new ArgumentException("For complex interleaved streams, input buffers must be of an even size.");
        }

        protected void ValidateMemory<T>(Memory<T>[] mems) where T : unmanaged => ValidateMemory(mems.Select(mem => (ReadOnlyMemory<T>)mem).ToArray());

        protected void ValidateIntPtrArray(IntPtr[] intPtrs)
        {
            var numChannels = _streamHandle.GetChannels().Length;
            if (intPtrs == null)
                throw new ArgumentNullException("mems");
            else if (intPtrs.Length != numChannels)
                throw new ArgumentException(string.Format("Stream is configured for {0} channel(s). Cannot accept {1} buffer(s).", numChannels, intPtrs.Length));
            else if (intPtrs.Any(x => x == null))
                throw new ArgumentNullException("intPtrs");
        }

        //
        // Object overrides
        //

        // For completeness, but a stream is only ever equal to itself
        public override bool Equals(object other) => ReferenceEquals(this, other);

        public override int GetHashCode() => HashCodeBuilder.Create()
            .AddValue(GetType())
            .AddValue(_device)
            .AddValue(Format)
            .AddValue(Channels)
            .AddValue(StreamArgs);
    }
}
