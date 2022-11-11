This directory contains the C# bindings for SoapySDR. It largely comprises of a SWIG
wrapper, with some hand-written C# on top to make the API more idiomatic for traditional
C#.

As of the writing of this readme, CMake only supports the .NET implementation for C#, and
we bring in some .NET dependencies that are stubbed out in Mono, so we only support the C#
bindings with MSVC.