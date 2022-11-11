// Copyright (c) 2020-2021 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

using System;

namespace Pothosware.SoapySDR
{
    /// <summary>
    /// SoapySDR has some non-standard conversion logic, so for the sake of consistency, we'll pass
    /// our calls down into the native library for conversion. This adds a minor performance hit, but
    /// it's enough of an edge case to be acceptable.
    /// </summary>
    internal class SoapyConvertible : System.IConvertible
    {
        private string _value;

        public SoapyConvertible(SoapyConvertible other) => _value = other._value;

        public SoapyConvertible(object value)
        {
            switch (value)
            {
                case string _:
                    _value = (string)value;
                    break;
                case bool _:
                    _value = TypeConversionInternal.BoolToString((bool)value);
                    break;
                case float _:
                    _value = TypeConversionInternal.FloatToString((float)value);
                    break;
                case double _:
                    _value = TypeConversionInternal.DoubleToString((double)value);
                    break;
                case decimal _:
                    _value = TypeConversionInternal.DoubleToString(Convert.ToDouble(value));
                    break;
                case sbyte _:
                    _value = TypeConversionInternal.SByteToString((sbyte)value);
                    break;
                case short _:
                    _value = TypeConversionInternal.ShortToString((short)value);
                    break;
                case int _:
                    _value = TypeConversionInternal.IntToString((int)value);
                    break;
                case long _:
                    _value = TypeConversionInternal.LongToString((long)value);
                    break;
                case byte _:
                    _value = TypeConversionInternal.ByteToString((byte)value);
                    break;
                case ushort _:
                    _value = TypeConversionInternal.UShortToString((ushort)value);
                    break;
                case uint _:
                    _value = TypeConversionInternal.UIntToString((uint)value);
                    break;
                case ulong _:
                    _value = TypeConversionInternal.ULongToString((ulong)value);
                    break;
                default:
                    _value = value.ToString(); // Good luck
                    break;
            }
        }

        //
        // IConvertible overrides
        //

        public TypeCode GetTypeCode() => TypeCode.Object;

        public bool ToBoolean(IFormatProvider _) => TypeConversionInternal.StringToBool(_value);

        public byte ToByte(IFormatProvider _) => TypeConversionInternal.StringToByte(_value);

        public char ToChar(IFormatProvider _) => throw new NotImplementedException();

        public DateTime ToDateTime(IFormatProvider _) => throw new NotImplementedException();

        public decimal ToDecimal(IFormatProvider _) => (decimal)TypeConversionInternal.StringToDouble(_value);

        public double ToDouble(IFormatProvider _) => TypeConversionInternal.StringToDouble(_value);

        public short ToInt16(IFormatProvider _) => TypeConversionInternal.StringToShort(_value);

        public int ToInt32(IFormatProvider _) => TypeConversionInternal.StringToInt(_value);

        public long ToInt64(IFormatProvider _) => TypeConversionInternal.StringToLong(_value);

        public sbyte ToSByte(IFormatProvider _) => TypeConversionInternal.StringToSByte(_value);

        public float ToSingle(IFormatProvider _) => TypeConversionInternal.StringToFloat(_value);

        public string ToString(IFormatProvider _) => _value;

        public object ToType(Type conversionType, IFormatProvider _)
        {
            if (conversionType.Equals(typeof(string))) return ToString(_);
            if (conversionType.Equals(typeof(bool))) return ToBoolean(_);
            if (conversionType.Equals(typeof(sbyte))) return ToSByte(_);
            if (conversionType.Equals(typeof(short))) return ToInt16(_);
            if (conversionType.Equals(typeof(int))) return ToInt32(_);
            if (conversionType.Equals(typeof(long))) return ToInt64(_);
            if (conversionType.Equals(typeof(byte))) return ToByte(_);
            if (conversionType.Equals(typeof(ushort))) return ToUInt16(_);
            if (conversionType.Equals(typeof(uint))) return ToUInt32(_);
            if (conversionType.Equals(typeof(ulong))) return ToUInt64(_);
            if (conversionType.Equals(typeof(float))) return ToSingle(_);
            if (conversionType.Equals(typeof(double))) return ToDouble(_);
            if (conversionType.Equals(typeof(decimal))) return ToDecimal(_);

            throw new NotImplementedException(conversionType.FullName);
        }

        public ushort ToUInt16(IFormatProvider _) => TypeConversionInternal.StringToUShort(_value);

        public uint ToUInt32(IFormatProvider _) => TypeConversionInternal.StringToUInt(_value);

        public ulong ToUInt64(IFormatProvider _) => TypeConversionInternal.StringToULong(_value);

        //
        // Object overrides
        //

        public override string ToString() => ToString(null);

        public override int GetHashCode() => GetType().GetHashCode() ^ _value.GetHashCode();

        public override bool Equals(object obj) => (obj as SoapyConvertible)?._value.Equals(_value) ?? false;
    }
}