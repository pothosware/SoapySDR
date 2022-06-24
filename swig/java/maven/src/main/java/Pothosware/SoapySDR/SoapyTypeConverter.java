// Copyright (c) 2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

package Pothosware.SoapySDR;

import java.lang.*;

// Soapy has some non-standard type conversion, so we need this
// to maintain consistency between APIs. This is technically very
// unoptimal, but it's enough of a corner case to hopefully not be
// a problem.
class SoapyTypeConverter
{
    public static Object convert(Object object, Class toType)
    {
        if(toType.equals(String.class))
        {
            // Prioritize most likely types
            if(object instanceof String) return object;
            if(object instanceof Integer) return TypeConversionInternal.intToString((int)object);
            if(object instanceof Double) return TypeConversionInternal.doubleToString((double)object);
            if(object instanceof Boolean) return TypeConversionInternal.booleanToString((boolean)object);
            if(object instanceof Byte) return TypeConversionInternal.byteToString((byte)object);
            if(object instanceof Short) return TypeConversionInternal.shortToString((short)object);
            if(object instanceof Long) return TypeConversionInternal.longToString((long)object);
            if(object instanceof Float) return TypeConversionInternal.floatToString((float)object);
            else
            {
                // Good luck.
                return object.toString();
            }
        }
        else if(object instanceof String)
        {
            // Prioritize most likely types
            if(toType.getClass().equals(String.class)) return object;
            else if(toType.getClass().equals(int.class)) return TypeConversionInternal.stringToInt((String)object);
            else if(toType.getClass().equals(double.class)) return TypeConversionInternal.stringToDouble((String)object);
            else if(toType.getClass().equals(boolean.class)) return TypeConversionInternal.stringToBoolean((String)object);
            else if(toType.getClass().equals(byte.class)) return TypeConversionInternal.stringToByte((String)object);
            else if(toType.getClass().equals(short.class)) return TypeConversionInternal.stringToShort((String)object);
            else if(toType.getClass().equals(long.class)) return TypeConversionInternal.stringToLong((String)object);
            else if(toType.getClass().equals(float.class)) return TypeConversionInternal.stringToFloat((String)object);
            else throw new RuntimeException("Cannot convert "+object.getClass()+" to "+toType);
        }
        else throw new RuntimeException("Cannot convert "+object.getClass()+" to "+toType);
    }
}
