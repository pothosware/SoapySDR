// Copyright (c) 2021-2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

%typemap(javaclassmodifiers) SoapySDR::Java::StreamHandle "class";
%nodefaultctor SoapySDR::Java::StreamHandle;
%ignore SoapySDR::Java::StreamHandle::stream;
%ignore SoapySDR::Java::StreamHandle::format;
%ignore SoapySDR::Java::StreamHandle::channels;

// TODO: ignore NativeStreamFormat setters
%nodefaultctor SoapySDR::Java::NativeStreamFormat;
%nodefaultctor SoapySDR::Java::StreamFormat;

// Java doesn't support bitwise operations on enums, so the
// conventional API for this is to take in a set. We'll deal
// with the ugliness to get the conversions ourselves.

%typemap(jstype) const int flags "java.util.EnumSet<StreamFlags>"
%typemap(javain,
    pre="
        StreamFlags[] flagArray;
        $javainput.ToArray(flagArray);

        int jniInput = 0;
        for(StreamFlags flag: flagArray) {
            jniInput = jniInput | flag.swigValue();
        }

        Kwargs temp$javainput = Utility.ToStringList($javainput);
    ") const int flags "jniInput"

//
// Now, include everything.
//

%{
#include "Stream.hpp"
%}

%include "Stream.hpp"

//
// Manually implement streaming functions in JNI to optimize array/buffer
// usage.
//

%typemap(jtype) int8_t *buffer "byte[]"
%typemap(jtype) short *buffer "short[]"
%typemap(jtype) int *buffer "int[]"
%typemap(jtype) float *buffer "float[]"
%typemap(jtype) double *buffer "double[]"

%typemap(jtype) const int8_t *buffer "byte[]"
%typemap(jtype) const short *buffer "short[]"
%typemap(jtype) const int *buffer "int[]"
%typemap(jtype) const float *buffer "float[]"
%typemap(jtype) const double *buffer "double[]"

%typemap(jtype) int8_t *nioBuffer "java.nio.ByteBuffer"
%typemap(jtype) short *nioBuffer "java.nio.ShortBuffer"
%typemap(jtype) int *nioBuffer "java.nio.IntBuffer"
%typemap(jtype) float *nioBuffer "java.nio.FloatBuffer"
%typemap(jtype) double *nioBuffer "java.nio.DoubleBuffer"

%typemap(jtype) const int8_t *nioBuffer "java.nio.ByteBuffer"
%typemap(jtype) const short *nioBuffer "java.nio.ShortBuffer"
%typemap(jtype) const int *nioBuffer "java.nio.IntBuffer"
%typemap(jtype) const float *nioBuffer "java.nio.FloatBuffer"
%typemap(jtype) const double *nioBuffer "java.nio.DoubleBuffer"

%native(Device_readStreamByteArray1D) SoapySDR::Java::StreamResult readStreamByteArray1D(SoapySDR::Device *, const SoapySDR::Java::StreamHandle &stream, int8_t *buffer, long timeoutUs);
%native(Device_readStreamShortArray1D) SoapySDR::Java::StreamResult readStreamShortArray1D(SoapySDR::Device *, const SoapySDR::Java::StreamHandle &stream, short *buffer, long timeoutUs);
%native(Device_readStreamIntArray1D) SoapySDR::Java::StreamResult readStreamIntArray1D(SoapySDR::Device *, const SoapySDR::Java::StreamHandle &stream, int *buffer, long timeoutUs);
%native(Device_readStreamFloatArray1D) SoapySDR::Java::StreamResult readStreamFloatArray1D(SoapySDR::Device *, const SoapySDR::Java::StreamHandle &stream, float *buffer, long timeoutUs);
%native(Device_readStreamDoubleArray1D) SoapySDR::Java::StreamResult readStreamDoubleArray1D(SoapySDR::Device *, const SoapySDR::Java::StreamHandle &stream, double *buffer, long timeoutUs);

%native(Device_readStreamByteBuffer1D) SoapySDR::Java::StreamResult readStreamByteBuffer1D(SoapySDR::Device *, const SoapySDR::Java::StreamHandle &stream, int8_t *nioBuffer, long timeoutUs);
%native(Device_readStreamShortBuffer1D) SoapySDR::Java::StreamResult readStreamShortBuffer1D(SoapySDR::Device *, const SoapySDR::Java::StreamHandle &stream, short *nioBuffer, long timeoutUs);
%native(Device_readStreamIntBuffer1D) SoapySDR::Java::StreamResult readStreamIntBuffer1D(SoapySDR::Device *, const SoapySDR::Java::StreamHandle &stream, int *nioBuffer, long timeoutUs);
%native(Device_readStreamFloatBuffer1D) SoapySDR::Java::StreamResult readStreamFloatBuffer1D(SoapySDR::Device *, const SoapySDR::Java::StreamHandle &stream, float *nioBuffer, long timeoutUs);
%native(Device_readStreamDoubleBuffer1D) SoapySDR::Java::StreamResult readStreamDoubleBuffer1D(SoapySDR::Device *, const SoapySDR::Java::StreamHandle &stream, double *nioBuffer, long timeoutUs);

%native(Device_writeStreamByteArray1D) SoapySDR::Java::StreamResult writeStreamByteArray1D(SoapySDR::Device *, const SoapySDR::Java::StreamHandle &stream, const int8_t *buffer, long long timeNs, long timeoutUs);
%native(Device_writeStreamShortArray1D) SoapySDR::Java::StreamResult writeStreamShortArray1D(SoapySDR::Device *, const SoapySDR::Java::StreamHandle &stream, const short *buffer, long long timeNs, long timeoutUs);
%native(Device_writeStreamIntArray1D) SoapySDR::Java::StreamResult writeStreamIntArray1D(SoapySDR::Device *, const SoapySDR::Java::StreamHandle &stream, const int *buffer, long long timeNs, long timeoutUs);
%native(Device_writeStreamFloatArray1D) SoapySDR::Java::StreamResult writeStreamFloatArray1D(SoapySDR::Device *, const SoapySDR::Java::StreamHandle &stream, const float *buffer, long long timeNs, long timeoutUs);
%native(Device_writeStreamDoubleArray1D) SoapySDR::Java::StreamResult writeStreamDoubleArray1D(SoapySDR::Device *, const SoapySDR::Java::StreamHandle &stream, const double *buffer, long long timeNs, long timeoutUs);

%native(Device_writeStreamByteBuffer1D) SoapySDR::Java::StreamResult writeStreamByteBuffer1D(SoapySDR::Device *, const SoapySDR::Java::StreamHandle &stream, const int8_t *nioBuffer, long long timeNs, long timeoutUs);
%native(Device_writeStreamShortBuffer1D) SoapySDR::Java::StreamResult writeStreamShortBuffer1D(SoapySDR::Device *, const SoapySDR::Java::StreamHandle &stream, const short *nioBuffer, long long timeNs, long timeoutUs);
%native(Device_writeStreamIntBuffer1D) SoapySDR::Java::StreamResult writeStreamIntBuffer1D(SoapySDR::Device *, const SoapySDR::Java::StreamHandle &stream, const int *nioBuffer, long long timeNs, long timeoutUs);
%native(Device_writeStreamFloatBuffer1D) SoapySDR::Java::StreamResult writeStreamFloatBuffer1D(SoapySDR::Device *, const SoapySDR::Java::StreamHandle &stream, const float *nioBuffer, long long timeNs, long timeoutUs);
%native(Device_writeStreamDoubleBuffer1D) SoapySDR::Java::StreamResult writeStreamDoubleBuffer1D(SoapySDR::Device *, const SoapySDR::Java::StreamHandle &stream, const double *nioBuffer, long long timeNs, long timeoutUs);

%{
    //
    // Common code
    //

    static jlong readStream1D(
        JNIEnv *jenv,
        SoapySDR::Device *device,
        SoapySDR::Java::StreamHandle *stream,
        void *rawBuffer,
        size_t elems,
        jlong timeoutUs)
    {
        SoapySDR::Java::StreamResult result;
        jlong jresult = 0;

        try
        {
            void *buffs[1] = {rawBuffer};
            int intFlags = 0;

            result.errorCode = SoapySDR::Java::ErrorCode(device->readStream(
                stream->stream,
                &buffs[0],
                elems,
                intFlags,
                result.timeNs,
                timeoutUs));
            result.flags = SoapySDR::Java::StreamFlags(intFlags);
        }
        SWIG_JAVA_CATCH_JLONG

        *(SoapySDR::Java::StreamResult **)&jresult = new SoapySDR::Java::StreamResult(result);
        return jresult;
    }

    static jlong writeStream1D(
        JNIEnv *jenv,
        SoapySDR::Device *device,
        SoapySDR::Java::StreamHandle *stream,
        const void *rawBuffer,
        size_t elems,
        jlong jtimeNs,
        jlong timeoutUs)
    {
        SoapySDR::Java::StreamResult result;
        jlong jresult = 0;

        try
        {
            const void *buffs[1] = {rawBuffer};
            int intFlags = 0;

            result.errorCode = SoapySDR::Java::ErrorCode(device->writeStream(
                stream->stream,
                &buffs[0],
                elems,
                intFlags,
                jtimeNs,
                timeoutUs));
            result.flags = SoapySDR::Java::StreamFlags(intFlags);
        }
        SWIG_JAVA_CATCH_JLONG

        *(SoapySDR::Java::StreamResult **)&jresult = new SoapySDR::Java::StreamResult(result);
        return jresult;
    }

    static jlong readStreamArray1D(
        JNIEnv *jenv,
        jlong jdevice,
        jlong jstream,
        jarray jbuffer,
        jlong jtimeoutUs,
        const std::string &expectedFormat)
    {
        jlong jresult = 0;

        const auto jelems = jenv->GetArrayLength(jbuffer);
        auto *stream = *(SoapySDR::Java::StreamHandle**)&jstream;

        if(stream->format != expectedFormat)
        {
            std::string errorMsg = "Invalid stream format "+stream->format+". Expected "+expectedFormat+".";
            SWIG_JavaThrowException(jenv, SWIG_JavaIllegalArgumentException, errorMsg.c_str());
        }
        else if(stream->channels.size() != 1)
        {
            SWIG_JavaThrowException(jenv, SWIG_JavaIllegalArgumentException, "This function can only be used with single-channel streams.");
        }
        else if((jelems % 2) != 0)
        {
            SWIG_JavaThrowException(jenv, SWIG_JavaIllegalArgumentException, "Arrays for interleaved buffers must be of even length.");
        }
        else
        {
            void *rawBuffer = jenv->GetPrimitiveArrayCritical(jbuffer, nullptr);
            if(rawBuffer)
            {
                auto *device = *(SoapySDR::Device**)&jdevice;
                const size_t elems = jelems / 2;

                jresult = readStream1D(jenv, device, stream, rawBuffer, elems, jtimeoutUs);

                jenv->ReleasePrimitiveArrayCritical(jbuffer, rawBuffer, 0);
            }
            else SWIG_JavaThrowException(jenv, SWIG_JavaOutOfMemoryError, "GetPrimitiveArrayCritical failed");
        }

        return jresult;
    }

    static jlong readStreamBuffer1D(
        JNIEnv *jenv,
        jlong jdevice,
        jlong jstream,
        jobject jbuffer,
        jlong jtimeoutUs,
        const std::string &expectedFormat)
    {
        jlong jresult = 0;

        const auto jbytes = jenv->GetDirectBufferCapacity(jbuffer);
        auto *stream = *(SoapySDR::Java::StreamHandle**)&jstream;

        const size_t elemSize = SoapySDR::formatToSize(expectedFormat);

        if(stream->format != expectedFormat)
        {
            std::string errorMsg = "Invalid stream format "+stream->format+". Expected "+expectedFormat+".";
            SWIG_JavaThrowException(jenv, SWIG_JavaIllegalArgumentException, errorMsg.c_str());
        }
        else if(stream->channels.size() != 1)
        {
            SWIG_JavaThrowException(jenv, SWIG_JavaIllegalArgumentException, "This function can only be used with single-channel streams.");
        }
        else if((jbytes % elemSize) != 0)
        {
            std::string errorMsg = "Buffer size must be divisble by element size "+std::to_string(elemSize)+".";
            SWIG_JavaThrowException(jenv, SWIG_JavaIllegalArgumentException, errorMsg.c_str());
        }
        else
        {
            void *rawBuffer = jenv->GetDirectBufferAddress(jbuffer);
            if(rawBuffer)
            {
                auto *device = *(SoapySDR::Device**)&jdevice;
                const size_t elems = jbytes / elemSize;

                jresult = readStream1D(jenv, device, stream, rawBuffer, elems, jtimeoutUs);
            }
            else SWIG_JavaThrowException(jenv, SWIG_JavaOutOfMemoryError, "GetDirectBufferAddress failed");
        }

        return jresult;
    }

    static jlong writeStreamArray1D(
        JNIEnv *jenv,
        jlong jdevice,
        jlong jstream,
        jarray jbuffer,
        jlong jtimeNs,
        jlong jtimeoutUs,
        const std::string &expectedFormat)
    {
        jlong jresult = 0;

        const auto jbytes = jenv->GetDirectBufferCapacity(jbuffer);
        auto *stream = *(SoapySDR::Java::StreamHandle**)&jstream;

        const size_t elemSize = SoapySDR::formatToSize(expectedFormat);

        if(stream->format != expectedFormat)
        {
            std::string errorMsg = "Invalid stream format "+stream->format+". Expected "+expectedFormat+".";
            SWIG_JavaThrowException(jenv, SWIG_JavaIllegalArgumentException, errorMsg.c_str());
        }
        else if(stream->channels.size() != 1)
        {
            SWIG_JavaThrowException(jenv, SWIG_JavaIllegalArgumentException, "This function can only be used with single-channel streams.");
        }
        else if((jbytes % elemSize) != 0)
        {
            std::string errorMsg = "Buffer size must be divisble by element size "+std::to_string(elemSize)+".";
            SWIG_JavaThrowException(jenv, SWIG_JavaIllegalArgumentException, errorMsg.c_str());
        }
        else
        {
            const void *rawBuffer = jenv->GetDirectBufferAddress(jbuffer);
            if(rawBuffer)
            {
                auto *device = *(SoapySDR::Device**)&jdevice;
                const size_t elems = jbytes / elemSize;

                jresult = writeStream1D(jenv, device, stream, rawBuffer, elems, jtimeNs, jtimeoutUs);
            }
            else SWIG_JavaThrowException(jenv, SWIG_JavaOutOfMemoryError, "GetDirectBufferAddress failed");
        }

        return jresult;
    }

    static jlong writeStreamBuffer1D(
        JNIEnv *jenv,
        jlong jdevice,
        jlong jstream,
        jobject jbuffer,
        jlong jtimeNs,
        jlong jtimeoutUs,
        const std::string &expectedFormat)
    {
        jlong jresult = 0;

        const auto jbytes = jenv->GetDirectBufferCapacity(jbuffer);
        auto *stream = *(SoapySDR::Java::StreamHandle**)&jstream;

        const size_t elemSize = SoapySDR::formatToSize(expectedFormat);

        if(stream->format != expectedFormat)
        {
            std::string errorMsg = "Invalid stream format "+stream->format+". Expected "+expectedFormat+".";
            SWIG_JavaThrowException(jenv, SWIG_JavaIllegalArgumentException, errorMsg.c_str());
        }
        else if(stream->channels.size() != 1)
        {
            SWIG_JavaThrowException(jenv, SWIG_JavaIllegalArgumentException, "This function can only be used with single-channel streams.");
        }
        else if((jbytes % elemSize) != 0)
        {
            std::string errorMsg = "Buffer size must be divisble by element size "+std::to_string(elemSize)+".";
            SWIG_JavaThrowException(jenv, SWIG_JavaIllegalArgumentException, errorMsg.c_str());
        }
        else
        {
            const void *rawBuffer = jenv->GetDirectBufferAddress(jbuffer);
            if(rawBuffer)
            {
                auto *device = *(SoapySDR::Device**)&jdevice;
                const size_t elems = jbytes / elemSize;

                jresult = writeStream1D(jenv, device, stream, rawBuffer, elems, jtimeNs, jtimeoutUs);
            }
            else SWIG_JavaThrowException(jenv, SWIG_JavaOutOfMemoryError, "GetDirectBufferAddress failed");
        }

        return jresult;
    }

    //
    // JNI-facing functions
    //

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_Device_1readStreamByteArray1D(
        JNIEnv *jenv,
        jclass,
        jlong jdevice,
        jobject,
        jlong jstream,
        jobject,
        jbyteArray jbuffer,
        jlong jtimeoutUs)
    {
        return readStreamArray1D(jenv, jdevice, jstream, jbuffer, jtimeoutUs, SOAPY_SDR_CS8);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_Device_1readStreamShortArray1D(
        JNIEnv *jenv,
        jclass,
        jlong jdevice,
        jobject,
        jlong jstream,
        jobject,
        jshortArray jbuffer,
        jlong jtimeoutUs)
    {
        return readStreamArray1D(jenv, jdevice, jstream, jbuffer, jtimeoutUs, SOAPY_SDR_CS16);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_Device_1readStreamIntArray1D(
        JNIEnv *jenv,
        jclass,
        jlong jdevice,
        jobject,
        jlong jstream,
        jobject,
        jintArray jbuffer,
        jlong jtimeoutUs)
    {
        return readStreamArray1D(jenv, jdevice, jstream, jbuffer, jtimeoutUs, SOAPY_SDR_CS32);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_Device_1readStreamFloatArray1D(
        JNIEnv *jenv,
        jclass,
        jlong jdevice,
        jobject,
        jlong jstream,
        jobject,
        jfloatArray jbuffer,
        jlong jtimeoutUs)
    {
        return readStreamArray1D(jenv, jdevice, jstream, jbuffer, jtimeoutUs, SOAPY_SDR_CF32);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_Device_1readStreamDoubleArray1D(
        JNIEnv *jenv,
        jclass,
        jlong jdevice,
        jobject,
        jlong jstream,
        jobject,
        jdoubleArray jbuffer,
        jlong jtimeoutUs)
    {
        return readStreamArray1D(jenv, jdevice, jstream, jbuffer, jtimeoutUs, SOAPY_SDR_CF64);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_Device_1readStreamByteBuffer1D(
        JNIEnv *jenv,
        jclass,
        jlong jdevice,
        jobject,
        jlong jstream,
        jobject,
        jobject jbuffer,
        jlong jtimeoutUs)
    {
        return readStreamBuffer1D(jenv, jdevice, jstream, jbuffer, jtimeoutUs, SOAPY_SDR_CS8);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_Device_1readStreamShortBuffer1D(
        JNIEnv *jenv,
        jclass,
        jlong jdevice,
        jobject,
        jlong jstream,
        jobject,
        jobject jbuffer,
        jlong jtimeoutUs)
    {
        return readStreamBuffer1D(jenv, jdevice, jstream, jbuffer, jtimeoutUs, SOAPY_SDR_CS16);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_Device_1readStreamIntBuffer1D(
        JNIEnv *jenv,
        jclass,
        jlong jdevice,
        jobject,
        jlong jstream,
        jobject,
        jobject jbuffer,
        jlong jtimeoutUs)
    {
        return readStreamBuffer1D(jenv, jdevice, jstream, jbuffer, jtimeoutUs, SOAPY_SDR_CS32);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_Device_1readStreamFloatBuffer1D(
        JNIEnv *jenv,
        jclass,
        jlong jdevice,
        jobject,
        jlong jstream,
        jobject,
        jobject jbuffer,
        jlong jtimeoutUs)
    {
        return readStreamBuffer1D(jenv, jdevice, jstream, jbuffer, jtimeoutUs, SOAPY_SDR_CF32);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_Device_1readStreamDoubleBuffer1D(
        JNIEnv *jenv,
        jclass,
        jlong jdevice,
        jobject,
        jlong jstream,
        jobject,
        jobject jbuffer,
        jlong jtimeoutUs)
    {
        return readStreamBuffer1D(jenv, jdevice, jstream, jbuffer, jtimeoutUs, SOAPY_SDR_CF64);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_Device_1writeStreamByteArray1D(
        JNIEnv *jenv,
        jclass,
        jlong jdevice,
        jobject,
        jlong jstream,
        jobject,
        jbyteArray jbuffer,
        jlong jtimeNs,
        jlong jtimeoutUs)
    {
        return writeStreamArray1D(jenv, jdevice, jstream, jbuffer, jtimeNs, jtimeoutUs, SOAPY_SDR_CS8);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_Device_1writeStreamShortArray1D(
        JNIEnv *jenv,
        jclass,
        jlong jdevice,
        jobject,
        jlong jstream,
        jobject,
        jshortArray jbuffer,
        jlong jtimeNs,
        jlong jtimeoutUs)
    {
        return writeStreamArray1D(jenv, jdevice, jstream, jbuffer, jtimeNs, jtimeoutUs, SOAPY_SDR_CS16);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_Device_1writeStreamIntArray1D(
        JNIEnv *jenv,
        jclass,
        jlong jdevice,
        jobject,
        jlong jstream,
        jobject,
        jintArray jbuffer,
        jlong jtimeNs,
        jlong jtimeoutUs)
    {
        return writeStreamArray1D(jenv, jdevice, jstream, jbuffer, jtimeNs, jtimeoutUs, SOAPY_SDR_CS32);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_Device_1writeStreamFloatArray1D(
        JNIEnv *jenv,
        jclass,
        jlong jdevice,
        jobject,
        jlong jstream,
        jobject,
        jfloatArray jbuffer,
        jlong jtimeNs,
        jlong jtimeoutUs)
    {
        return writeStreamArray1D(jenv, jdevice, jstream, jbuffer, jtimeNs, jtimeoutUs, SOAPY_SDR_CF32);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_Device_1writeStreamDoubleArray1D(
        JNIEnv *jenv,
        jclass,
        jlong jdevice,
        jobject,
        jlong jstream,
        jobject,
        jdoubleArray jbuffer,
        jlong jtimeNs,
        jlong jtimeoutUs)
    {
        return writeStreamArray1D(jenv, jdevice, jstream, jbuffer, jtimeNs, jtimeoutUs, SOAPY_SDR_CF64);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_Device_1writeStreamByteBuffer1D(
        JNIEnv *jenv,
        jclass,
        jlong jdevice,
        jobject,
        jlong jstream,
        jobject,
        jobject jbuffer,
        jlong jtimeNs,
        jlong jtimeoutUs)
    {
        return writeStreamBuffer1D(jenv, jdevice, jstream, jbuffer, jtimeNs, jtimeoutUs, SOAPY_SDR_CS8);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_Device_1writeStreamShortBuffer1D(
        JNIEnv *jenv,
        jclass,
        jlong jdevice,
        jobject,
        jlong jstream,
        jobject,
        jobject jbuffer,
        jlong jtimeNs,
        jlong jtimeoutUs)
    {
        return writeStreamBuffer1D(jenv, jdevice, jstream, jbuffer, jtimeNs, jtimeoutUs, SOAPY_SDR_CS16);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_Device_1writeStreamIntBuffer1D(
        JNIEnv *jenv,
        jclass,
        jlong jdevice,
        jobject,
        jlong jstream,
        jobject,
        jobject jbuffer,
        jlong jtimeNs,
        jlong jtimeoutUs)
    {
        return writeStreamBuffer1D(jenv, jdevice, jstream, jbuffer, jtimeNs, jtimeoutUs, SOAPY_SDR_CS32);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_Device_1writeStreamFloatBuffer1D(
        JNIEnv *jenv,
        jclass,
        jlong jdevice,
        jobject,
        jlong jstream,
        jobject,
        jobject jbuffer,
        jlong jtimeNs,
        jlong jtimeoutUs)
    {
        return writeStreamBuffer1D(jenv, jdevice, jstream, jbuffer, jtimeNs, jtimeoutUs, SOAPY_SDR_CF32);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_Device_1writeStreamDoubleBuffer1D(
        JNIEnv *jenv,
        jclass,
        jlong jdevice,
        jobject,
        jlong jstream,
        jobject,
        jobject jbuffer,
        jlong jtimeNs,
        jlong jtimeoutUs)
    {
        return writeStreamBuffer1D(jenv, jdevice, jstream, jbuffer, jtimeNs, jtimeoutUs, SOAPY_SDR_CF64);
    }
%}
