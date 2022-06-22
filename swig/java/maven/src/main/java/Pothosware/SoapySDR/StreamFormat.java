// Copyright (c) 2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

package Pothosware.SoapySDR;

// Unfortunately, this is necessary, or SWIG will generate
// getCF64(), etc.

public final class StreamFormat
{

public static final String CF64 = "CF64";
public static final String CF32 = "CF32";

public static final String CS32 = "CS32";
public static final String CS16 = "CS16";
public static final String CS8 = "CS8";

public static final String CU32 = "CU32";
public static final String CU16 = "CU16";
public static final String CU8 = "CU8";

}
