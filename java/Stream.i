// Copyright (c) 2021 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

%ignore SoapySDR::Java::StreamHandle::stream;
%ignore SoapySDR::Java::StreamHandle::channels;
%javamethodmodifiers SoapySDR::Java::StreamHandle::GetChannels "";
%ignore SoapySDR::Java::StreamHandle::format;
%javamethodmodifiers SoapySDR::Java::StreamHandle::GetFormat "";
%javamethodmodifiers SoapySDR::Java::StreamHandle::GetPointer "";
%nodefaultctor SoapySDR::Java::StreamFormats;

/*
%typemap(javacode) SoapySDR::Java::StreamHandle %{
    public override string ToString()
    {
        return string.Format("Opaque SoapySDR stream handle at {0}", GetPointer());
    }

    public override bool Equals(object other)
    {
        var otherAsStreamHandle = other as StreamHandle;
        if(otherAsStreamHandle != null) return (GetHashCode() == other.GetHashCode());
        else                            throw new ArgumentException("Not a StreamHandle");
    }

    public override int GetHashCode()
    {
        return (GetType().GetHashCode() ^ GetPointer().GetHashCode());
    }
%}
*/
