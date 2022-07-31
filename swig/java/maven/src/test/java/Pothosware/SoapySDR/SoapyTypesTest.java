// Copyright (c) 2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

package Pothosware.SoapySDR;

import static org.junit.jupiter.api.Assertions.*;
import org.junit.jupiter.api.Test;

public class SoapyTypesTest
{
    // Make sure native libraries are loaded.
    static
    {
        try { JARUtils.initialize(); }
        catch(Exception e) {}
    }

    private ArgInfo getArgInfo()
    {
        ArgInfo argInfo = new ArgInfo();

        argInfo.setKey("Key");
        argInfo.setValue("Value");
        argInfo.setName("Name");
        argInfo.setDescription("Description");
        argInfo.setUnits("Unit");
        argInfo.setType(ArgInfo.Type.FLOAT);
        argInfo.setRange(new Range(0.0, 1.0, 0.1));

        return argInfo;
    }

    @Test
    public void testArgInfo()
    {
        ArgInfo argInfo1 = getArgInfo();
        assertEquals("Key", argInfo1.getKey());
        assertEquals("Value", argInfo1.getValue());
        assertEquals("Name", argInfo1.getName());
        assertEquals("Description", argInfo1.getDescription());
        assertEquals("Unit", argInfo1.getUnits());
        assertEquals(ArgInfo.Type.FLOAT, argInfo1.getType());
        assertEquals(new Range(0.0, 1.0, 0.1), argInfo1.getRange());

        ArgInfo argInfo2 = getArgInfo();
        assertEquals(argInfo1, argInfo2);
        assertEquals(argInfo1.hashCode(), argInfo2.hashCode());

        argInfo2.setKey("Key2");
        assertNotEquals(argInfo1, argInfo2);
        assertNotEquals(argInfo1.hashCode(), argInfo2.hashCode());
    }

    private void testKwargsKeyValuePair(
        Kwargs kwargs,
        String key,
        String value)
    {
        assertTrue(kwargs.containsKey(key));
        assertEquals(value, kwargs.get(key));
    }

    @Test
    public void testKwargs()
    {
        // Make sure we can pass any Java map in.
        java.util.HashMap<String, String> hashMap = new java.util.HashMap<String, String>();
        hashMap.put("key0", "val0");
        hashMap.put("key1", "val1");
        hashMap.put("key2", "val2");

        Kwargs kwargs1 = Utility.toKwargs(hashMap);
        testKwargsKeyValuePair(kwargs1, "key0", "val0");
        testKwargsKeyValuePair(kwargs1, "key1", "val1");
        testKwargsKeyValuePair(kwargs1, "key2", "val2");

        Kwargs kwargs2 = Utility.toKwargs(kwargs1);
        testKwargsKeyValuePair(kwargs2, "key0", "val0");
        testKwargsKeyValuePair(kwargs2, "key1", "val1");
        testKwargsKeyValuePair(kwargs2, "key2", "val2");
    }

    @Test
    public void testRange()
    {
        Range range0Params = new Range();
        assertEquals(0.0, range0Params.minimum());
        assertEquals(0.0, range0Params.maximum());
        assertEquals(0.0, range0Params.step());

        Range range2Params = new Range(1.0, 2.0);
        assertEquals(1.0, range2Params.minimum());
        assertEquals(2.0, range2Params.maximum());
        assertEquals(0.0, range2Params.step());

        Range range3Params = new Range(3.0, 4.0, 0.1);
        assertEquals(3.0, range3Params.minimum());
        assertEquals(4.0, range3Params.maximum());
        assertEquals(0.1, range3Params.step());

        Range range3Params2 = new Range(3.0, 4.0, 0.1);
        assertEquals(range3Params, range3Params2);
        assertEquals(range3Params.hashCode(), range3Params2.hashCode());
        assertNotEquals(range3Params, range2Params);
        assertNotEquals(range3Params.hashCode(), range2Params.hashCode());
    }
}
