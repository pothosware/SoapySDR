// Copyright (c) 2021-2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

%{
#include "BufferHandling.hpp"
%}

// TODO: ignore NativeStreamFormat setters
%nodefaultctor SoapySDR::Java::NativeStreamFormat;
%nodefaultctor SoapySDR::Java::StreamFormat;

%nodefaultctor SoapySDR::Java::Stream;
%nodefaultctor SoapySDR::Java::RxStream;
%nodefaultctor SoapySDR::Java::TxStream;
%ignore SoapySDR::Java::Stream::Stream(SoapySDR::Device *, const int, const std::string &, const std::vector<size_t> &, const SoapySDR::Kwargs &);
%ignore SoapySDR::Java::RxStream::RxStream(SoapySDR::Device *, const std::string &, const std::vector<size_t> &, const SoapySDR::Kwargs &);
%ignore SoapySDR::Java::RxStream::read;
%ignore SoapySDR::Java::TxStream::TxStream(SoapySDR::Device *, const std::string &, const std::vector<size_t> &, const SoapySDR::Kwargs &);

// Java doesn't support bitwise operations on enums, so the
// conventional API for this is to take in a set. We'll deal
// with the ugliness to get the conversions ourselves.

%typemap(jstype) const int flags "java.util.Set<StreamFlags>"
%typemap(javain,
    pre="
        var flagArray = (StreamFlags[])$javainput.toArray(new StreamFlags[0]);

        int jniInput = 0;
        if(flagArray != null) {
            for(StreamFlags flag: flagArray) {
                jniInput = jniInput | flag.swigValue();
            }
        }
    ") const int flags "jniInput"

//
// Typemaps for passing buffers into SoapySDR. SWIG already has typemaps
// for arrays, but we are replacing them with versions that are less likely
// to copy buffers.
//

%define ARRAY_TYPEMAPS(ctype, arr_jnitype, arr_jtype_jstype)
    %typemap(jni) (const ctype *buffer) #arr_jnitype
    %typemap(jtype) (const ctype *buffer) #arr_jtype_jstype
    %typemap(jstype) (const ctype *buffer) #arr_jtype_jstype
    %typemap(javain) (const ctype *buffer) "$javainput"

    %typemap(in) (const ctype *buffer, const size_t length) {
        $1 = SoapySDR::Java::get1DArray<ctype>(jenv, $input, arg1->getExecutionPolicy(), $2);
        if((not $1) or ($2 == 0))
        {
            SWIG_JavaThrowException(jenv, SWIG_JavaOutOfMemoryError, "Failed to get native array pointer.");
            return $null;
        }
        else if ($2 % 2)
        {
            SWIG_JavaThrowException(jenv, SWIG_JavaIllegalArgumentException, "Buffers for interleaved samples must be of even length.");
            SoapySDR::Java::release1DArray(jenv, $input, arg1->getExecutionPolicy(), $1);
            return $null;
        }
    }

    %typemap(freearg) (const ctype *buffer, const size_t length) {
        SoapySDR::Java::release1DArray(jenv, $input, arg1->getExecutionPolicy(), $1);
    }

    %apply const ctype * buffer { ctype * outputBuffer };
    %apply (const ctype *buffer, const size_t length) { (ctype *outputBuffer, const size_t length) };
%enddef

%define NIO_BUFFER_TYPEMAPS(ctype, nio_jtype_jstype)
    %typemap(jni) (const ctype *nioBuffer) "jobject"
    %typemap(jtype) (const ctype *nioBuffer) #nio_jtype_jstype
    %typemap(jstype) (const ctype *nioBuffer) #nio_jtype_jstype
    %typemap(javain,
             pre="if(!$javainput.isDirect()) throw new java.lang.IllegalArgumentException(\"The given NIO buffer must be direct.\");"
            ) (const ctype *nioBuffer) "$javainput"

    %typemap(jni) (ctype *outputNIOBuffer) "jobject"
    %typemap(jtype) (ctype *outputNIOBuffer) #nio_jtype_jstype
    %typemap(jstype) (ctype *outputNIOBuffer) #nio_jtype_jstype
    %typemap(javain,
             pre="
             if(!$javainput.isDirect()) throw new java.lang.IllegalArgumentException(\"The given NIO buffer must be direct.\");
             if($javainput.isReadOnly()) throw new java.lang.IllegalArgumentException(\"Cannot read into a read-only NIO buffer.\");
             "
            ) (ctype *outputNIOBuffer) "$javainput"
            
    %typemap(in) (const ctype *nioBuffer, const size_t length) {
        const auto $input_jbytes = jenv->GetDirectBufferCapacity($input);
        const size_t $input_elemSize = SoapySDR::formatToSize(arg1->getFormat()); // arg1 should always be the stream class

        void * $input_buffer = nullptr;
        size_t $input_numElems = 0;

        if($input_jbytes < 0)
        {
            SWIG_JavaThrowException(jenv, SWIG_JavaIllegalArgumentException, "Invalid NIO buffer.");
            return $null;
        }

        if(($input_jbytes % $input_elemSize) == 0)
        {
            $input_numElems = $input_jbytes / $input_elemSize;
            if(($input_numElems % 2) == 0)
            {
                $input_buffer = jenv->GetDirectBufferAddress($input);

                if(not $input_buffer)
                {
                    SWIG_JavaThrowException(jenv, SWIG_JavaOutOfMemoryError, "GetDirectBufferAddress failed.");
                    return $null;
                }

            }
            else
            {
                SWIG_JavaThrowException(jenv, SWIG_JavaIllegalArgumentException, "Buffers for interleaved samples must be of even length.");
                return $null;
            }
        }
        else
        {
            std::string errorMsg = "NIO buffer size must be divisible by element size "+std::to_string($input_elemSize)+".";
            SWIG_JavaThrowException(jenv, SWIG_JavaIllegalArgumentException, errorMsg.c_str());
        }

        $1 = (ctype *)$input_buffer;
        $2 = size_t($input_numElems);
    }

    %apply (const ctype *nioBuffer, const size_t length) { (ctype *outputNIOBuffer, const size_t length) };
%enddef

%define BUFFER_TYPEMAPS(ctype, arr_jnitype, arr_jtype_jstype, nio_jtype_jstype)
    ARRAY_TYPEMAPS(ctype, arr_jnitype, arr_jtype_jstype)
    NIO_BUFFER_TYPEMAPS(ctype, nio_jtype_jstype)
%enddef

BUFFER_TYPEMAPS(jbyte, jbyteArray, byte[], java.nio.ByteBuffer)
BUFFER_TYPEMAPS(jshort, jshortArray, short[], java.nio.ShortBuffer)
BUFFER_TYPEMAPS(jint, jintArray, int[], java.nio.IntBuffer)
BUFFER_TYPEMAPS(jfloat, jfloatArray, float[], java.nio.FloatBuffer)
BUFFER_TYPEMAPS(jdouble, jdoubleArray, double[], java.nio.DoubleBuffer)

// This should be fine for derived classes to use.
%typemap(javacode) SoapySDR::Java::Stream
%{
    @Override
    public String toString() {
        return getClass().getName();
    }

    @Override
    public boolean equals(Object o) {
        return (hashCode() == o.hashCode());
    }

    @Override
    public int hashCode() {
        return new org.apache.commons.lang3.builder.HashCodeBuilder(1351, 4063)
            .append(getClass().getName())
            .append(swigCPtr)
            .toHashCode();
    }
%}

%{
#include "RxStream.hpp"
#include "Stream.hpp"
#include "TxStream.hpp"
%}

// TODO: figure out naming everything "write", may just need to put it in javacode

%include "StreamExecutionPolicy.hpp"
%include "Stream.hpp"
%include "RxStream.hpp"
%include "TxStream.hpp"
