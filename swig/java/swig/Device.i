// Copyright (c) 2021-2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

%{
#include "Stream.hpp"
%}

%include <attribute.i>
%include <std_pair.i>
%include <stdint.i>
%include <typemaps.i>

// Ignore normal factory stuff
%nodefaultctor SoapySDR::Device;
%ignore SoapySDR::Device::~Device; // Class will call unmake on destruction
%ignore SoapySDR::Device::make;
%ignore SoapySDR::Device::unmake;

%apply double& OUTPUT { double& fullScale };

%typemap(javaclassmodifiers) std::pair<SoapySDR::Java::ErrorCode, SoapySDR::Java::StreamResult> "internal class";
%template(StreamResultPairInternal) std::pair<SoapySDR::Java::ErrorCode, SoapySDR::Java::StreamResult>;

// 
// Use the Java enum for direction
//

%typemap(javatype) const int direction "Direction"
%typemap(javain,
         pre="int temp$javainput = (int)$javainput;")
         const int direction
         "temp$javainput"

// Don't wrap deprecated functions
%ignore SoapySDR::Device::listSampleRates;
%ignore SoapySDR::Device::listBandwidths;
%ignore SoapySDR::Device::setCommandTime;
%ignore SoapySDR::Device::writeRegister(const unsigned, const unsigned);
%ignore SoapySDR::Device::readRegister(const unsigned) const;

// Ignore stream-related functions, we're rewriting some
%ignore SoapySDR::Device::setupStream;
%ignore SoapySDR::Device::closeStream;
%ignore SoapySDR::Device::getStreamMTU;
%ignore SoapySDR::Device::activateStream;
%ignore SoapySDR::Device::deactivateStream;
%ignore SoapySDR::Device::readStream;
%ignore SoapySDR::Device::writeStream;
%ignore SoapySDR::Device::readStreamStatus;
%ignore SoapySDR::Device::getNumDirectAccessBuffers;
%ignore SoapySDR::Device::getDirectAccessBufferAddrs;
%ignore SoapySDR::Device::acquireReadBuffer;
%ignore SoapySDR::Device::releaseReadBuffer;
%ignore SoapySDR::Device::acquireWriteBuffer;
%ignore SoapySDR::Device::releaseWriteBuffer;

// Ignore functions explicitly using std::vector<unsigned> due to size_t workaround
%ignore SoapySDR::Device::writeRegisters;
%ignore SoapySDR::Device::readRegisters;

// Don't wrap development-layer functions
%ignore SoapySDR::Device::getNativeDeviceHandle;

%typemap(javacode) SoapySDR::Device
%{
    public override string ToString() => string.Format("{0}:{1}", DriverKey, HardwareKey);

    public override bool Equals(object obj) => (obj as Device)?.swigCPtr.Equals(swigCPtr) ?? false;

    public override int GetHashCode() => GetType().GetHashCode() ^ (swigCPtr.GetHashCode() << 1);
%}

%feature("compactdefaultargs", "0") setHardwareTime;
%feature("compactdefaultargs", "0") readUART;

%include <SoapySDR/Device.hpp>

%javamethodmodifiers SoapySDR::Device::SetupStreamInternal "internal";
%javamethodmodifiers SoapySDR::Device::CloseStreamInternal "internal";
%javamethodmodifiers SoapySDR::Device::GetStreamMTUInternal "internal";
%javamethodmodifiers SoapySDR::Device::ActivateStreamInternal "internal";
%javamethodmodifiers SoapySDR::Device::DeactivateStreamInternal "internal";
%javamethodmodifiers SoapySDR::Device::ReadStreamInternal "internal";
%javamethodmodifiers SoapySDR::Device::WriteStreamInternal "internal";
%javamethodmodifiers SoapySDR::Device::ReadStreamStatusInternal "internal";

// Internal bridge functions to make the Java part easier
%extend SoapySDR::Device
{
    Device()
    {
        return SoapySDR::Device::make("");
    }

    Device(const SoapySDR::Kwargs &kwargs)
    {
        return SoapySDR::Device::make(kwargs);
    }

    Device(const std::string &args)
    {
        return SoapySDR::Device::make(args);
    }

    ~Device()
    {
        SoapySDR::Device::unmake(self);
    }
};
