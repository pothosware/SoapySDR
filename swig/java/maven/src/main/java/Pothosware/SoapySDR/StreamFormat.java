// Copyright (c) 2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

package Pothosware.SoapySDR;

// Unfortunately, this is necessary, or SWIG will generate
// getCF64(), etc.

class StreamFormat
{

public static String CF64 = "CF64";
public static String CF32 = "CF32";

public static String CS32 = "CS32";
public static String CS16 = "CS16";
public static String CS8 = "CS8";

public static String CU32 = "CU32";
public static String CU16 = "CU16";
public static String CU8 = "CU8";

}
