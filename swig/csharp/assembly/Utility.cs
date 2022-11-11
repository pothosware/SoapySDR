// Copyright (c) 2020-2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

using System;
using System.Buffers;
using System.Collections.Generic;
using System.Linq;

namespace Pothosware.SoapySDR
{
    /// <summary>
    /// Utility functions to generate Pothosware.SoapySDR.Device parameters
    /// </summary>
    public class Utility
    {
        //
        // Public
        //

        /// <summary>
        /// Return the SoapySDR complex format string that corresponds to the given type.
        /// This string will be passed into Device's stream setup functions.
        ///
        /// This function will throw if the type is unsupported.
        /// </summary>
        /// <typeparam name="T">The format type</typeparam>
        /// <returns>The type's complex format string (see Pothosware.SoapySDR.StreamFormat)</returns>
        public static string GetFormatString<T>() where T : unmanaged
        {
            var type = typeof(T);

            if (typeof(T).Equals(typeof(sbyte))) return StreamFormat.ComplexInt8;
            else if (typeof(T).Equals(typeof(short))) return StreamFormat.ComplexInt16;
            else if (typeof(T).Equals(typeof(int))) return StreamFormat.ComplexInt32;
            else if (typeof(T).Equals(typeof(byte))) return StreamFormat.ComplexUInt8;
            else if (typeof(T).Equals(typeof(ushort))) return StreamFormat.ComplexUInt16;
            else if (typeof(T).Equals(typeof(uint))) return StreamFormat.ComplexUInt32;
            else if (typeof(T).Equals(typeof(float))) return StreamFormat.ComplexFloat32;
            else if (typeof(T).Equals(typeof(double))) return StreamFormat.ComplexFloat64;
            else throw new Exception(string.Format("Type {0} not covered by GetFormatString", type));
        }

        /// <summary>
        /// Convert a markup string to a key-value map.
        ///
        /// The markup format is: "key0=value0, key1=value1".
        /// </summary>
        /// <param name="args">The markup string</param>
        /// <returns>An equivalent key-value map</returns>
        public static Kwargs StringToKwargs(string args) => TypeConversionInternal.StringToKwargs(args);

        /// <summary>
        /// Convert a key-value map to a markup string.
        ///
        /// The markup format is: "key0=value0, key1=value1".
        /// </summary>
        /// <param name="kwargs">The key-value map</param>
        /// <returns>An equivalent markup string</returns>
        public static string KwargsToString(IDictionary<string, string> kwargs) => TypeConversionInternal.KwargsToString(ToKwargs(kwargs));

        //
        // Internal
        //

        internal static Kwargs ToKwargs(IDictionary<string, string> input)
        {
            if (input is Kwargs) return (Kwargs)input;

            Kwargs kwargs;

            var output = new Kwargs();
            foreach(var pair in input)
            {
                output.Add(pair.Key, pair.Value);
            }

            return output;
        }

        internal unsafe static PointerListInternal ToPointerListInternal<T>(
            Memory<T>[] memory,
            out MemoryHandle[] memoryHandles)
        {
            memoryHandles = memory.Select(mem => mem.Pin()).ToArray();
            return ToPointerListInternal(memoryHandles.Select(handle => (UIntPtr)handle.Pointer).ToArray());
        }

        internal unsafe static PointerListInternal ToPointerListInternal<T>(
            ReadOnlyMemory<T>[] memory,
            out MemoryHandle[] memoryHandles)
        {
            memoryHandles = memory.Select(mem => mem.Pin()).ToArray();
            return ToPointerListInternal(memoryHandles.Select(handle => (UIntPtr)handle.Pointer).ToArray());
        }

        internal static PointerListInternal ToPointerListInternal(UIntPtr[] arr) => new PointerListInternal(arr.Select(x => (ulong)x));

        internal unsafe static PointerListInternal ToPointerListInternal(IntPtr[] arr) => new PointerListInternal(arr.Select(x => (ulong)(UIntPtr)(void*)x));

    }
}
