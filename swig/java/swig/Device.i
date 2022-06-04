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

%typemap(javaclassmodifiers) std::pair<SoapySDR::Java::ErrorCode, SoapySDR::Java::StreamResult> "class";
%template(StreamResultPairInternal) std::pair<SoapySDR::Java::ErrorCode, SoapySDR::Java::StreamResult>;

// 
// Use the Java enum for direction
//

%typemap(jstype) const int direction "Direction"
%typemap(javain,
         pre="int temp$javainput = $javainput.swigValue();")
         const int direction
         "temp$javainput"

// Don't wrap deprecated functions
%ignore SoapySDR::Device::listSampleRates;
%ignore SoapySDR::Device::listBandwidths;
%ignore SoapySDR::Device::setCommandTime;
%ignore SoapySDR::Device::writeRegister(const unsigned, const unsigned);
%ignore SoapySDR::Device::readRegister(const unsigned) const;

// Ignore stream-related functions, we're rewriting some
%ignore SoapySDR::Device::getNativeStreamFormat(const int, const size_t, double &) const;
%ignore SoapySDR::Device::setupStream(const int, const std::string &, const std::vector<size_t> &, const Kwargs &);
%ignore SoapySDR::Device::closeStream(Stream *);
%ignore SoapySDR::Device::getStreamMTU(Stream *) const;
%ignore SoapySDR::Device::activateStream(Stream *, const int, const long long, const size_t);
%ignore SoapySDR::Device::deactivateStream(Stream *, const int, const long long);
%ignore SoapySDR::Device::readStream;
%ignore SoapySDR::Device::writeStream;
%ignore SoapySDR::Device::readStreamStatus(Stream *, size_t &, int &, long long &, const long);

// Don't wrap development-layer functions
%ignore SoapySDR::Device::getNumDirectAccessBuffers;
%ignore SoapySDR::Device::getDirectAccessBufferAddrs;
%ignore SoapySDR::Device::acquireReadBuffer;
%ignore SoapySDR::Device::releaseReadBuffer;
%ignore SoapySDR::Device::acquireWriteBuffer;
%ignore SoapySDR::Device::releaseWriteBuffer;
%ignore SoapySDR::Device::getNativeDeviceHandle;

%typemap(javacode) SoapySDR::Device
%{
    @override
    public String toString() {
        return getDriverKey()+":"+getHardwareKey();
    }

    @override
    public boolean equals(Object o) {
        return (hashCode() == o.hashCode());
    }

    @override
    public int hashCode() {
        return new HashCodeBuilder(1351, 4063)
            .append(getClass().getName())
            .append(swigCPtr)
            .toHashCode();
    }
%}

%feature("compactdefaultargs", "0") setHardwareTime;
%feature("compactdefaultargs", "0") readUART;

%include <SoapySDR/Device.hpp>

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

    SoapySDR::Java::NativeStreamFormat getNativeStreamFormat(
        const int direction,
        const size_t channel) const
    {
        SoapySDR::Java::NativeStreamFormat ret;
        ret.format = self->getNativeStreamFormat(direction, channel, ret.fullScale);

        return ret;
    }

    // TODO: any way to do a Java generic overload?
    SoapySDR::Java::RxStream *setupRxStream(
        const std::string &format,
        const std::vector<size_t> &channels = std::vector<size_t>(),
        const SoapySDR::Kwargs &args = SoapySDR::Kwargs())
    {
        return new SoapySDR::Java::RxStream(self, format, channels, args);
    }

    // TODO: any way to do a Java generic overload?
    SoapySDR::Java::TxStream *setupTxStream(
        const std::string &format,
        const std::vector<size_t> &channels = std::vector<size_t>(),
        const SoapySDR::Kwargs &args = SoapySDR::Kwargs())
    {
        return new SoapySDR::Java::TxStream(self, format, channels, args);
    }
};

// TODO: add Java chrono for times
