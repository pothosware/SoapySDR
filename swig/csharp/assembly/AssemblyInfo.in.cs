// Copyright (c) 2021 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

using System.Reflection;

// Information about this assembly is defined by the following
// attributes.
//
// change them to the information which is associated with the assembly
// you compile.

#if (DEBUG)
[assembly: AssemblyConfiguration("Debug")]
#else
[assembly: AssemblyConfiguration("Release")]
#endif

[assembly: AssemblyTitle("SoapySDR @SOAPY_SDR_VERSION@ - C# Bindings")]
[assembly: AssemblyDescription("SoapySDR @SOAPY_SDR_VERSION@ - C# Bindings")]
[assembly: AssemblyCompany("Pothosware")]
[assembly: AssemblyProduct("SoapySDR")]
[assembly: AssemblyDefaultAlias("Generated Assembly")]
[assembly: AssemblyCopyright("Copyright © 2021- Nicholas Corgan")]
[assembly: AssemblyTrademark("SoapySDR - https://github.com/pothosware/SoapySDR")]

// The assembly version has following format :
//
// Major.Minor.Build.Revision
//
// You can specify all values by your own or you can build default build and revision
// numbers with the '*' character (the default):

[assembly: AssemblyVersion("@SOAPY_SDR_LIBVER@")]
[assembly: AssemblyInformationalVersion("@SOAPY_SDR_VERSION@")]

// The following attributes specify the key for the sign of your assembly. See the
// .NET Framework documentation for more information about signing.
// This is not required, if you don't want signing let these attributes like they're.
// http://msdn.microsoft.com/en-us/library/6f05ezxy(VS.80).aspx
[assembly: AssemblyKeyFile("")]
//[assembly: AssemblyKeyName("GDCM Key")]
[assembly: AssemblyKeyName("")]