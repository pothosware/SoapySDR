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

%native(Device_readStreamByte1D) SoapySDR::Java::StreamResult readStreamByte1D(SoapySDR::Device *, const SoapySDR::Java::StreamHandle &stream, int8_t *buffer, long timeoutUs);
%native(Device_readStreamShort1D) SoapySDR::Java::StreamResult readStreamShort1D(SoapySDR::Device *, const SoapySDR::Java::StreamHandle &stream, short *buffer, long timeoutUs);
%native(Device_readStreamInt1D) SoapySDR::Java::StreamResult readStreamInt1D(SoapySDR::Device *, const SoapySDR::Java::StreamHandle &stream, int *buffer, long timeoutUs);
%native(Device_readStreamFloat1D) SoapySDR::Java::StreamResult readStreamFloat1D(SoapySDR::Device *, const SoapySDR::Java::StreamHandle &stream, float *buffer, long timeoutUs);
%native(Device_readStreamDouble1D) SoapySDR::Java::StreamResult readStreamDouble1D(SoapySDR::Device *, const SoapySDR::Java::StreamHandle &stream, double *buffer, long timeoutUs);

%native(Device_writeStreamByte1D) SoapySDR::Java::StreamResult writeStreamByte1D(SoapySDR::Device *, const SoapySDR::Java::StreamHandle &stream, const int8_t *buffer, long long timeNs, long timeoutUs);
%native(Device_writeStreamShort1D) SoapySDR::Java::StreamResult writeStreamShort1D(SoapySDR::Device *, const SoapySDR::Java::StreamHandle &stream, const short *buffer, long long timeNs, long timeoutUs);
%native(Device_writeStreamInt1D) SoapySDR::Java::StreamResult writeStreamInt1D(SoapySDR::Device *, const SoapySDR::Java::StreamHandle &stream, const int *buffer, long long timeNs, long timeoutUs);
%native(Device_writeStreamFloat1D) SoapySDR::Java::StreamResult writeStreamFloat1D(SoapySDR::Device *, const SoapySDR::Java::StreamHandle &stream, const float *buffer, long long timeNs, long timeoutUs);
%native(Device_writeStreamDouble1D) SoapySDR::Java::StreamResult writeStreamDouble1D(SoapySDR::Device *, const SoapySDR::Java::StreamHandle &stream, const double *buffer, long long timeNs, long timeoutUs);

%{
    //
    // Common code
    //

    static jlong readStream1D(
        JNIEnv *jenv,
        jlong jdevice,
        jlong jstream,
        jarray jbuffer,
        jlong jtimeout,
        const std::string &expectedFormat)
    {
        SoapySDR::Java::StreamResult result;
        jlong jresult = 0;

        const auto jelems = jenv->GetArrayLength(jbuffer);
        void *rawBuffer = jenv->GetPrimitiveArrayCritical(jbuffer, nullptr);

        if(rawBuffer)
        {
            try
            {
                auto *stream = *(SoapySDR::Java::StreamHandle**)&jstream;

                if(stream->channels.size() != 1)
                    throw std::runtime_error("This function can only be used for one-channel streams.");
                if(stream->format != expectedFormat)
                    throw std::runtime_error("Invalid stream format "+stream->format+". Expected "+expectedFormat+".");

                auto *device = *(SoapySDR::Device**)&jdevice;
                void *buffs[1] = {rawBuffer};
                const size_t elems = jelems / 2; // Complex

                int intFlags = 0;

                result.errorCode = SoapySDR::Java::ErrorCode(device->readStream(
                    stream->stream,
                    &buffs[0],
                    elems,
                    intFlags,
                    result.timeNs,
                    jtimeout));
                result.flags = SoapySDR::Java::StreamFlags(intFlags);
            }
            SWIG_JAVA_CATCH_JLONG
        }
        else
        {
            SWIG_JavaThrowException(jenv, SWIG_JavaOutOfMemoryError, "GetPrimitiveArrayCritical failed");
            return 0;
        }

        if(rawBuffer) jenv->ReleasePrimitiveArrayCritical(jbuffer, rawBuffer, 0);

        *(SoapySDR::Java::StreamResult **)&jresult = new SoapySDR::Java::StreamResult(result);
        return jresult;
    }

    static jlong writeStream1D(
        JNIEnv *jenv,
        jlong jdevice,
        jlong jstream,
        jarray jbuffer,
        jlong jtimeNs,
        jlong jtimeout,
        const std::string &expectedFormat)
    {
        SoapySDR::Java::StreamResult result;
        jlong jresult = 0;

        const auto jelems = jenv->GetArrayLength(jbuffer);
        void *rawBuffer = jenv->GetPrimitiveArrayCritical(jbuffer, nullptr);

        if(rawBuffer)
        {
            try
            {
                auto *stream = *(SoapySDR::Java::StreamHandle**)&jstream;

                if(stream->channels.size() != 1)
                    throw std::runtime_error("This function can only be used for one-channel streams.");
                if(stream->format != expectedFormat)
                    throw std::runtime_error("Invalid stream format "+stream->format+". Expected "+expectedFormat+".");

                auto *device = *(SoapySDR::Device**)&jdevice;
                const void *buffs[1] = {rawBuffer};
                const size_t elems = jelems / 2; // Complex

                int intFlags = 0;

                result.errorCode = SoapySDR::Java::ErrorCode(device->writeStream(
                    stream->stream,
                    &buffs[0],
                    elems,
                    intFlags,
                    jtimeNs,
                    jtimeout));
                result.flags = SoapySDR::Java::StreamFlags(intFlags);
            }
            SWIG_JAVA_CATCH_JLONG
        }
        else
        {
            SWIG_JavaThrowException(jenv, SWIG_JavaOutOfMemoryError, "GetPrimitiveArrayCritical failed");
            return 0;
        }

        if(rawBuffer) jenv->ReleasePrimitiveArrayCritical(jbuffer, rawBuffer, 0);

        *(SoapySDR::Java::StreamResult **)&jresult = new SoapySDR::Java::StreamResult(result);
        return jresult;
    }

    //
    // JNI-facing functions
    //

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_Device_1readStreamByte1D(
        JNIEnv *jenv,
        jclass,
        jlong jdevice,
        jobject,
        jlong jstream,
        jobject,
        jbyteArray jbuffer,
        jlong jtimeout)
    {
        return readStream1D(jenv, jdevice, jstream, jbuffer, jtimeout, SOAPY_SDR_CS8);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_Device_1readStreamShort1D(
        JNIEnv *jenv,
        jclass,
        jlong jdevice,
        jobject,
        jlong jstream,
        jobject,
        jshortArray jbuffer,
        jlong jtimeout)
    {
        return readStream1D(jenv, jdevice, jstream, jbuffer, jtimeout, SOAPY_SDR_CS16);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_Device_1readStreamInt1D(
        JNIEnv *jenv,
        jclass,
        jlong jdevice,
        jobject,
        jlong jstream,
        jobject,
        jintArray jbuffer,
        jlong jtimeout)
    {
        return readStream1D(jenv, jdevice, jstream, jbuffer, jtimeout, SOAPY_SDR_CS32);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_Device_1readStreamFloat1D(
        JNIEnv *jenv,
        jclass,
        jlong jdevice,
        jobject,
        jlong jstream,
        jobject,
        jfloatArray jbuffer,
        jlong jtimeout)
    {
        return readStream1D(jenv, jdevice, jstream, jbuffer, jtimeout, SOAPY_SDR_CF32);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_Device_1readStreamDouble1D(
        JNIEnv *jenv,
        jclass,
        jlong jdevice,
        jobject,
        jlong jstream,
        jobject,
        jdoubleArray jbuffer,
        jlong jtimeout)
    {
        return readStream1D(jenv, jdevice, jstream, jbuffer, jtimeout, SOAPY_SDR_CF64);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_Device_1writeStreamByte1D(
        JNIEnv *jenv,
        jclass,
        jlong jdevice,
        jobject,
        jlong jstream,
        jobject,
        jbyteArray jbuffer,
        jlong jtimeNs,
        jlong jtimeout)
    {
        return writeStream1D(jenv, jdevice, jstream, jbuffer, jtimeNs, jtimeout, SOAPY_SDR_CS8);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_Device_1writeStreamShort1D(
        JNIEnv *jenv,
        jclass,
        jlong jdevice,
        jobject,
        jlong jstream,
        jobject,
        jshortArray jbuffer,
        jlong jtimeNs,
        jlong jtimeout)
    {
        return writeStream1D(jenv, jdevice, jstream, jbuffer, jtimeNs, jtimeout, SOAPY_SDR_CS16);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_Device_1writeStreamInt1D(
        JNIEnv *jenv,
        jclass,
        jlong jdevice,
        jobject,
        jlong jstream,
        jobject,
        jintArray jbuffer,
        jlong jtimeNs,
        jlong jtimeout)
    {
        return writeStream1D(jenv, jdevice, jstream, jbuffer, jtimeNs, jtimeout, SOAPY_SDR_CS32);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_Device_1writeStreamFloat1D(
        JNIEnv *jenv,
        jclass,
        jlong jdevice,
        jobject,
        jlong jstream,
        jobject,
        jfloatArray jbuffer,
        jlong jtimeNs,
        jlong jtimeout)
    {
        return writeStream1D(jenv, jdevice, jstream, jbuffer, jtimeNs, jtimeout, SOAPY_SDR_CF32);
    }

    SWIGEXPORT jlong JNICALL Java_PothosWare_SoapySDR_SoapySDRJavaJNI_Device_1writeStreamByte1D(
        JNIEnv *jenv,
        jclass,
        jlong jdevice,
        jobject,
        jlong jstream,
        jobject,
        jdoubleArray jbuffer,
        jlong jtimeNs,
        jlong jtimeout)
    {
        return writeStream1D(jenv, jdevice, jstream, jbuffer, jtimeNs, jtimeout, SOAPY_SDR_CF64);
    }
%}
