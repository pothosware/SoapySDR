// Copyright (c) 2021-2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

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
%ignore SoapySDR::Java::TxStream::write;

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
// Manually implement streaming functions in JNI to optimize array/buffer
// usage.
//
// Typemaps based on: https://stackoverflow.com/a/32474455
//

%define BUFFER_TYPEMAPS(ctype, arr_jtype_jstype, nio_jtype_jstype)
    %typemap(jtype) (ctype *buffer), (const ctype *buffer) #arr_jtype_jstype
    %typemap(jtype) (ctype *nioBuffer), (const ctype *nioBuffer) #nio_jtype_jstype

    %typemap(jstype) (ctype *buffer), (const ctype *buffer) #arr_jtype_jstype
    %typemap(jstype) (ctype *nioBuffer), (const ctype *nioBuffer) #nio_jtype_jstype

    %typemap(javain) (ctype *buffer), (const ctype *buffer) "$javainput"
    %typemap(javain) (ctype *nioBuffer), (const ctype *nioBuffer) "$javainput"
%enddef

BUFFER_TYPEMAPS(int8_t, byte[], java.nio.ByteBuffer)
BUFFER_TYPEMAPS(short, short[], java.nio.ShortBuffer)
BUFFER_TYPEMAPS(int, int[], java.nio.IntBuffer)
BUFFER_TYPEMAPS(float, float[], java.nio.FloatBuffer)
BUFFER_TYPEMAPS(double, double[], java.nio.DoubleBuffer)

%native(RxStream_readByteArray1D) SoapySDR::Java::StreamResult readByteArray1D(SoapySDR::Java::RxStream *, int8_t *buffer, jlong timeoutUs);
%native(RxStream_readShortArray1D) SoapySDR::Java::StreamResult readShortArray1D(SoapySDR::Java::RxStream *, short *buffer, jlong timeoutUs);
%native(RxStream_readIntArray1D) SoapySDR::Java::StreamResult readIntArray1D(SoapySDR::Java::RxStream *, int *buffer, jlong timeoutUs);
%native(RxStream_readFloatArray1D) SoapySDR::Java::StreamResult readFloatArray1D(SoapySDR::Java::RxStream *, float *buffer, jlong timeoutUs);
%native(RxStream_readDoubleArray1D) SoapySDR::Java::StreamResult readDoubleArray1D(SoapySDR::Java::RxStream *, double *buffer, jlong timeoutUs);

%native(RxStream_readByteBuffer1D) SoapySDR::Java::StreamResult readByteBuffer1D(SoapySDR::Java::RxStream *, int8_t *nioBuffer, jlong timeoutUs);
%native(RxStream_readShortBuffer1D) SoapySDR::Java::StreamResult readShortBuffer1D(SoapySDR::Java::RxStream *, short *nioBuffer, jlong timeoutUs);
%native(RxStream_readIntBuffer1D) SoapySDR::Java::StreamResult readIntBuffer1D(SoapySDR::Java::RxStream *, int *nioBuffer, jlong timeoutUs);
%native(RxStream_readFloatBuffer1D) SoapySDR::Java::StreamResult readFloatBuffer1D(SoapySDR::Java::RxStream *, float *nioBuffer, jlong timeoutUs);
%native(RxStream_readDoubleBuffer1D) SoapySDR::Java::StreamResult readDoubleBuffer1D(SoapySDR::Java::RxStream *, double *nioBuffer, jlong timeoutUs);

%native(TxStream_writeByteArray1D) SoapySDR::Java::StreamResult writeByteArray1D(SoapySDR::Java::TxStream *, const int8_t *buffer, jlong timeNs, jlong timeoutUs);
%native(TxStream_writeShortArray1D) SoapySDR::Java::StreamResult writeShortArray1D(SoapySDR::Java::TxStream *, const short *buffer, jlong timeNs, jlong timeoutUs);
%native(TxStream_writeIntArray1D) SoapySDR::Java::StreamResult writeIntArray1D(SoapySDR::Java::TxStream *, const int *buffer, jlong timeNs, jlong timeoutUs);
%native(TxStream_writeFloatArray1D) SoapySDR::Java::StreamResult writeFloatArray1D(SoapySDR::Java::TxStream *, const float *buffer, jlong timeNs, jlong timeoutUs);
%native(TxStream_writeDoubleArray1D) SoapySDR::Java::StreamResult writeDoubleArray1D(SoapySDR::Java::TxStream *, const double *buffer, jlong timeNs, jlong timeoutUs);

%native(TxStream_writeByteBuffer1D) SoapySDR::Java::StreamResult writeByteBuffer1D(SoapySDR::Java::TxStream *, const int8_t *nioBuffer, jlong timeNs, jlong timeoutUs);
%native(TxStream_writeShortBuffer1D) SoapySDR::Java::StreamResult writeShortBuffer1D(SoapySDR::Java::TxStream *, const short *nioBuffer, jlong timeNs, jlong timeoutUs);
%native(TxStream_writeIntBuffer1D) SoapySDR::Java::StreamResult writeIntBuffer1D(SoapySDR::Java::TxStream *, const int *nioBuffer, jlong timeNs, jlong timeoutUs);
%native(TxStream_writeFloatBuffer1D) SoapySDR::Java::StreamResult writeFloatBuffer1D(SoapySDR::Java::TxStream *, const float *nioBuffer, jlong timeNs, jlong timeoutUs);
%native(TxStream_writeDoubleBuffer1D) SoapySDR::Java::StreamResult writeDoubleBuffer1D(SoapySDR::Java::TxStream *, const double *nioBuffer, jlong timeNs, jlong timeoutUs);

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

%typemap(javacode) SoapySDR::Java::RxStream
%{
    public StreamResult read(
        byte[] outputBuffer,
        long timeoutUs)
    {
        return SoapySDRJava.RxStream_readByteArray1D(
            this,
            outputBuffer,
            timeoutUs);
    }

    public StreamResult read(
        short[] outputBuffer,
        long timeoutUs)
    {
        return SoapySDRJava.RxStream_readShortArray1D(
            this,
            outputBuffer,
            timeoutUs);
    }

    public StreamResult read(
        int[] outputBuffer,
        long timeoutUs)
    {
        return SoapySDRJava.RxStream_readIntArray1D(
            this,
            outputBuffer,
            timeoutUs);
    }

    public StreamResult read(
        float[] outputBuffer,
        long timeoutUs)
    {
        return SoapySDRJava.RxStream_readFloatArray1D(
            this,
            outputBuffer,
            timeoutUs);
    }

    public StreamResult read(
        double[] outputBuffer,
        long timeoutUs)
    {
        return SoapySDRJava.RxStream_readDoubleArray1D(
            this,
            outputBuffer,
            timeoutUs);
    }

    public StreamResult read(
        java.nio.ByteBuffer outputBuffer,
        long timeoutUs)
    {
        return SoapySDRJava.RxStream_readByteBuffer1D(
            this,
            outputBuffer,
            timeoutUs);
    }

    public StreamResult read(
        java.nio.ShortBuffer outputBuffer,
        long timeoutUs)
    {
        return SoapySDRJava.RxStream_readShortBuffer1D(
            this,
            outputBuffer,
            timeoutUs);
    }

    public StreamResult read(
        java.nio.IntBuffer outputBuffer,
        long timeoutUs)
    {
        return SoapySDRJava.RxStream_readIntBuffer1D(
            this,
            outputBuffer,
            timeoutUs);
    }

    public StreamResult read(
        java.nio.FloatBuffer outputBuffer,
        long timeoutUs)
    {
        return SoapySDRJava.RxStream_readFloatBuffer1D(
            this,
            outputBuffer,
            timeoutUs);
    }

    public StreamResult read(
        java.nio.DoubleBuffer outputBuffer,
        long timeoutUs)
    {
        return SoapySDRJava.RxStream_readDoubleBuffer1D(
            this,
            outputBuffer,
            timeoutUs);
    }
%}

%typemap(javacode) SoapySDR::Java::TxStream
%{
    public StreamResult write(
        byte[] buffer,
        long timeNs,
        long timeoutUs)
    {
        return SoapySDRJava.TxStream_writeByteArray1D(
            this,
            buffer,
            timeNs,
            timeoutUs);
    }

    public StreamResult write(
        short[] buffer,
        long timeNs,
        long timeoutUs)
    {
        return SoapySDRJava.TxStream_writeShortArray1D(
            this,
            buffer,
            timeNs,
            timeoutUs);
    }

    public StreamResult write(
        int[] buffer,
        long timeNs,
        long timeoutUs)
    {
        return SoapySDRJava.TxStream_writeIntArray1D(
            this,
            buffer,
            timeNs,
            timeoutUs);
    }

    public StreamResult write(
        float[] buffer,
        long timeNs,
        long timeoutUs)
    {
        return SoapySDRJava.TxStream_writeFloatArray1D(
            this,
            buffer,
            timeNs,
            timeoutUs);
    }

    public StreamResult write(
        double[] buffer,
        long timeNs,
        long timeoutUs)
    {
        return SoapySDRJava.TxStream_writeDoubleArray1D(
            this,
            buffer,
            timeNs,
            timeoutUs);
    }

    public StreamResult write(
        java.nio.ByteBuffer buffer,
        long timeNs,
        long timeoutUs)
    {
        return SoapySDRJava.TxStream_writeByteBuffer1D(
            this,
            buffer,
            timeNs,
            timeoutUs);
    }

    public StreamResult write(
        java.nio.ShortBuffer buffer,
        long timeNs,
        long timeoutUs)
    {
        return SoapySDRJava.TxStream_writeShortBuffer1D(
            this,
            buffer,
            timeNs,
            timeoutUs);
    }

    public StreamResult write(
        java.nio.IntBuffer buffer,
        long timeNs,
        long timeoutUs)
    {
        return SoapySDRJava.TxStream_writeIntBuffer1D(
            this,
            buffer,
            timeNs,
            timeoutUs);
    }

    public StreamResult write(
        java.nio.FloatBuffer buffer,
        long timeNs,
        long timeoutUs)
    {
        return SoapySDRJava.TxStream_writeFloatBuffer1D(
            this,
            buffer,
            timeNs,
            timeoutUs);
    }

    public StreamResult write(
        java.nio.DoubleBuffer buffer,
        long timeNs,
        long timeoutUs)
    {
        return SoapySDRJava.TxStream_writeDoubleBuffer1D(
            this,
            buffer,
            timeNs,
            timeoutUs);
    }
%}

%{
#include "Stream.hpp"
%}

%{
    //
    // Common read code
    //

    static jlong readStream1D(
        JNIEnv *jenv,
        SoapySDR::Java::RxStream *stream,
        void *rawBuffer,
        size_t elems,
        jlong jtimeoutUs,
        const std::string &expectedFormat)
    {
        assert(rawBuffer);

        SoapySDR::Java::StreamResult result;
        jlong jresult = 0;

        try
        {
            if(stream->getFormat() != expectedFormat)
            {
                std::string errorMsg = "Invalid stream format "+stream->getFormat()+". Expected "+expectedFormat+".";
                SWIG_JavaThrowException(jenv, SWIG_JavaIllegalArgumentException, errorMsg.c_str());
            }
            else if(stream->getChannels().size() != 1)
            {
                SWIG_JavaThrowException(jenv, SWIG_JavaIllegalArgumentException, "This function can only be used with single-channel streams.");
            }

            void *buffs[1] = {rawBuffer};
            result = stream->read(buffs, elems, jtimeoutUs);
        }
        SWIG_JAVA_CATCH_JLONG

        *(SoapySDR::Java::StreamResult **)&jresult = new SoapySDR::Java::StreamResult(result);
        return jresult;
    }

    static jlong readStreamArray1D(
        JNIEnv *jenv,
        jlong jstream,
        jarray jbuffer,
        jlong jtimeoutUs,
        const std::string &expectedFormat)
    {
        jlong jresult = 0;

        auto *stream = *(SoapySDR::Java::RxStream**)&jstream;
        const auto jelems = jenv->GetArrayLength(jbuffer);

        if((jelems % 2) == 0)
        {
            void *rawBuffer = jenv->GetPrimitiveArrayCritical(jbuffer, nullptr);
            if(rawBuffer)
            {
                const auto elems = jelems / 2;
                jresult = readStream1D(jenv, stream, rawBuffer, elems, jtimeoutUs, expectedFormat);

                jenv->ReleasePrimitiveArrayCritical(jbuffer, rawBuffer, 0);
            }
            else SWIG_JavaThrowException(jenv, SWIG_JavaOutOfMemoryError, "GetPrimitiveArrayCritical failed");
        }
        else SWIG_JavaThrowException(jenv, SWIG_JavaIllegalArgumentException, "Arrays for interleaved samples must be of even length.");

        return jresult;
    }

    static jlong readStreamBuffer1D(
        JNIEnv *jenv,
        jlong jstream,
        jobject jbuffer,
        jlong jtimeoutUs,
        const std::string &expectedFormat)
    {
        jlong jresult = 0;

        auto *stream = *(SoapySDR::Java::RxStream**)&jstream;
        const auto jbytes = jenv->GetDirectBufferCapacity(jbuffer);
        const size_t elemSize = SoapySDR::formatToSize(expectedFormat);

        if((jbytes % elemSize) == 0)
        {
            const auto elems = jbytes / elemSize;
            if((elems % 2) == 0)
            {
                void *rawBuffer = jenv->GetDirectBufferAddress(jbuffer);

                if(rawBuffer) jresult = readStream1D(jenv, stream, rawBuffer, elems, jtimeoutUs, expectedFormat);
                else SWIG_JavaThrowException(jenv, SWIG_JavaOutOfMemoryError, "GetDirectBufferAddress failed");

            }
            else SWIG_JavaThrowException(jenv, SWIG_JavaIllegalArgumentException, "Buffers for interleaved samples must be of even length.");
        }
        else
        {
            std::string errorMsg = "Buffer size must be divisble by element size "+std::to_string(elemSize)+".";
            SWIG_JavaThrowException(jenv, SWIG_JavaIllegalArgumentException, errorMsg.c_str());
        }

        return jresult;
    }

    //
    // Common write code
    //

    static jlong writeStream1D(
        JNIEnv *jenv,
        SoapySDR::Java::TxStream *stream,
        void *rawBuffer,
        size_t elems,
        jlong jtimeNs,
        jlong jtimeoutUs,
        const std::string &expectedFormat)
    {
        SoapySDR::Java::StreamResult result;
        jlong jresult = 0;

        try
        {
            if(stream->getFormat() != expectedFormat)
            {
                std::string errorMsg = "Invalid stream format "+stream->getFormat()+". Expected "+expectedFormat+".";
                SWIG_JavaThrowException(jenv, SWIG_JavaIllegalArgumentException, errorMsg.c_str());
            }
            else if(stream->getChannels().size() != 1)
            {
                SWIG_JavaThrowException(jenv, SWIG_JavaIllegalArgumentException, "This function can only be used with single-channel streams.");
            }

            void *buffs[1] = {rawBuffer};
            result = stream->write(buffs, elems, jtimeNs, jtimeoutUs);
        }
        SWIG_JAVA_CATCH_JLONG

        *(SoapySDR::Java::StreamResult **)&jresult = new SoapySDR::Java::StreamResult(result);
        return jresult;
    }

    static jlong writeStreamArray1D(
        JNIEnv *jenv,
        jlong jstream,
        jarray jbuffer,
        jlong jtimeNs,
        jlong jtimeoutUs,
        const std::string &expectedFormat)
    {
        jlong jresult = 0;

        auto *stream = *(SoapySDR::Java::TxStream**)&jstream;
        const auto jelems = jenv->GetArrayLength(jbuffer);

        if((jelems % 2) == 0)
        {
            void *rawBuffer = jenv->GetPrimitiveArrayCritical(jbuffer, nullptr);
            if(rawBuffer)
            {
                const auto elems = jelems / 2;
                jresult = writeStream1D(jenv, stream, rawBuffer, elems, jtimeNs, jtimeoutUs, expectedFormat);

                jenv->ReleasePrimitiveArrayCritical(jbuffer, rawBuffer, 0);
            }
            else SWIG_JavaThrowException(jenv, SWIG_JavaOutOfMemoryError, "GetPrimitiveArrayCritical failed");
        }
        else SWIG_JavaThrowException(jenv, SWIG_JavaIllegalArgumentException, "Arrays for interleaved samples must be of even length.");

        return jresult;
    }

    static jlong writeStreamBuffer1D(
        JNIEnv *jenv,
        jlong jstream,
        jobject jbuffer,
        jlong jtimeNs,
        jlong jtimeoutUs,
        const std::string &expectedFormat)
    {
        jlong jresult = 0;

        auto *stream = *(SoapySDR::Java::TxStream**)&jstream;
        const auto jbytes = jenv->GetDirectBufferCapacity(jbuffer);
        const size_t elemSize = SoapySDR::formatToSize(expectedFormat);

        if((jbytes % elemSize) == 0)
        {
            const auto elems = jbytes / elemSize;
            if((elems % 2) == 0)
            {
                void *rawBuffer = jenv->GetDirectBufferAddress(jbuffer);

                if(rawBuffer) jresult = writeStream1D(jenv, stream, rawBuffer, elems, jtimeNs, jtimeoutUs, expectedFormat);
                else SWIG_JavaThrowException(jenv, SWIG_JavaOutOfMemoryError, "GetDirectBufferAddress failed");

            }
            else SWIG_JavaThrowException(jenv, SWIG_JavaIllegalArgumentException, "Buffers for interleaved samples must be of even length.");
        }
        else
        {
            std::string errorMsg = "Buffer size must be divisble by element size "+std::to_string(elemSize)+".";
            SWIG_JavaThrowException(jenv, SWIG_JavaIllegalArgumentException, errorMsg.c_str());
        }

        return jresult;
    }

    //
    // JNI-facing functions
    //

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_RxStream_1readByteArray1D(
        JNIEnv *jenv,
        jclass,
        jlong jstream,
        jobject,
        jbyteArray jbuffer,
        jlong jtimeoutUs)
    {
        return readStreamArray1D(jenv, jstream, jbuffer, jtimeoutUs, SOAPY_SDR_CS8);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_RxStream_1readShortArray1D(
        JNIEnv *jenv,
        jclass,
        jlong jstream,
        jobject,
        jshortArray jbuffer,
        jlong jtimeoutUs)
    {
        return readStreamArray1D(jenv, jstream, jbuffer, jtimeoutUs, SOAPY_SDR_CS16);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_RxStream_1readIntArray1D(
        JNIEnv *jenv,
        jclass,
        jlong jstream,
        jobject,
        jintArray jbuffer,
        jlong jtimeoutUs)
    {
        return readStreamArray1D(jenv, jstream, jbuffer, jtimeoutUs, SOAPY_SDR_CS32);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_RxStream_1readFloatArray1D(
        JNIEnv *jenv,
        jclass,
        jlong jstream,
        jobject,
        jfloatArray jbuffer,
        jlong jtimeoutUs)
    {
        return readStreamArray1D(jenv, jstream, jbuffer, jtimeoutUs, SOAPY_SDR_CF32);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_RxStream_1readDoubleArray1D(
        JNIEnv *jenv,
        jclass,
        jlong jstream,
        jobject,
        jdoubleArray jbuffer,
        jlong jtimeoutUs)
    {
        return readStreamArray1D(jenv, jstream, jbuffer, jtimeoutUs, SOAPY_SDR_CF64);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_RxStream_1readByteBuffer1D(
        JNIEnv *jenv,
        jclass,
        jlong jstream,
        jobject,
        jobject jbuffer,
        jlong jtimeoutUs)
    {
        return readStreamBuffer1D(jenv, jstream, jbuffer, jtimeoutUs, SOAPY_SDR_CS8);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_RxStream_1readShortBuffer1D(
        JNIEnv *jenv,
        jclass,
        jlong jstream,
        jobject,
        jobject jbuffer,
        jlong jtimeoutUs)
    {
        return readStreamBuffer1D(jenv, jstream, jbuffer, jtimeoutUs, SOAPY_SDR_CS16);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_RxStream_1readIntBuffer1D(
        JNIEnv *jenv,
        jclass,
        jlong jstream,
        jobject,
        jobject jbuffer,
        jlong jtimeoutUs)
    {
        return readStreamBuffer1D(jenv, jstream, jbuffer, jtimeoutUs, SOAPY_SDR_CS32);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_RxStream_1readFloatBuffer1D(
        JNIEnv *jenv,
        jclass,
        jlong jstream,
        jobject,
        jobject jbuffer,
        jlong jtimeoutUs)
    {
        return readStreamBuffer1D(jenv, jstream, jbuffer, jtimeoutUs, SOAPY_SDR_CF32);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_RxStream_1readDoubleBuffer1D(
        JNIEnv *jenv,
        jclass,
        jlong jstream,
        jobject,
        jobject jbuffer,
        jlong jtimeoutUs)
    {
        return readStreamBuffer1D(jenv, jstream, jbuffer, jtimeoutUs, SOAPY_SDR_CF64);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_TxStream_1writeByteArray1D(
        JNIEnv *jenv,
        jclass,
        jlong jstream,
        jobject,
        jbyteArray jbuffer,
        jlong jtimeNs,
        jlong jtimeoutUs)
    {
        return writeStreamArray1D(jenv, jstream, jbuffer, jtimeNs, jtimeoutUs, SOAPY_SDR_CS8);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_TxStream_1writeShortArray1D(
        JNIEnv *jenv,
        jclass,
        jlong jstream,
        jobject,
        jshortArray jbuffer,
        jlong jtimeNs,
        jlong jtimeoutUs)
    {
        return writeStreamArray1D(jenv, jstream, jbuffer, jtimeNs, jtimeoutUs, SOAPY_SDR_CS16);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_TxStream_1writeIntArray1D(
        JNIEnv *jenv,
        jclass,
        jlong jstream,
        jobject,
        jintArray jbuffer,
        jlong jtimeNs,
        jlong jtimeoutUs)
    {
        return writeStreamArray1D(jenv, jstream, jbuffer, jtimeNs, jtimeoutUs, SOAPY_SDR_CS32);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_TxStream_1writeFloatArray1D(
        JNIEnv *jenv,
        jclass,
        jlong jstream,
        jobject,
        jfloatArray jbuffer,
        jlong jtimeNs,
        jlong jtimeoutUs)
    {
        return writeStreamArray1D(jenv, jstream, jbuffer, jtimeNs, jtimeoutUs, SOAPY_SDR_CF32);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_TxStream_1writeDoubleArray1D(
        JNIEnv *jenv,
        jclass,
        jlong jstream,
        jobject,
        jdoubleArray jbuffer,
        jlong jtimeNs,
        jlong jtimeoutUs)
    {
        return writeStreamArray1D(jenv, jstream, jbuffer, jtimeNs, jtimeoutUs, SOAPY_SDR_CF64);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_TxStream_1writeByteBuffer1D(
        JNIEnv *jenv,
        jclass,
        jlong jstream,
        jobject,
        jobject jbuffer,
        jlong jtimeNs,
        jlong jtimeoutUs)
    {
        return writeStreamBuffer1D(jenv, jstream, jbuffer, jtimeNs, jtimeoutUs, SOAPY_SDR_CS8);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_TxStream_1writeShortBuffer1D(
        JNIEnv *jenv,
        jclass,
        jlong jstream,
        jobject,
        jobject jbuffer,
        jlong jtimeNs,
        jlong jtimeoutUs)
    {
        return writeStreamBuffer1D(jenv, jstream, jbuffer, jtimeNs, jtimeoutUs, SOAPY_SDR_CS16);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_TxStream_1writeIntBuffer1D(
        JNIEnv *jenv,
        jclass,
        jlong jstream,
        jobject,
        jobject jbuffer,
        jlong jtimeNs,
        jlong jtimeoutUs)
    {
        return writeStreamBuffer1D(jenv, jstream, jbuffer, jtimeNs, jtimeoutUs, SOAPY_SDR_CS32);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_TxStream_1writeFloatBuffer1D(
        JNIEnv *jenv,
        jclass,
        jlong jstream,
        jobject,
        jobject jbuffer,
        jlong jtimeNs,
        jlong jtimeoutUs)
    {
        return writeStreamBuffer1D(jenv, jstream, jbuffer, jtimeNs, jtimeoutUs, SOAPY_SDR_CF32);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_TxStream_1writeDoubleBuffer1D(
        JNIEnv *jenv,
        jclass,
        jlong jstream,
        jobject,
        jobject jbuffer,
        jlong jtimeNs,
        jlong jtimeoutUs)
    {
        return writeStreamBuffer1D(jenv, jstream, jbuffer, jtimeNs, jtimeoutUs, SOAPY_SDR_CF64);
    }
%}

%include "Stream.hpp"
