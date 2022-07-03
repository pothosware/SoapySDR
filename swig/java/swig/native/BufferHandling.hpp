// Copyright (c) 2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "StreamExecutionPolicy.hpp"

#include <jni.h>

namespace SoapySDR { namespace Java {

namespace detail {

template <typename ElemType, typename Array>
inline ElemType *get1DArrayPointer(JNIEnv *, const Array &)
{
    return nullptr;
}

template <typename ElemType, typename Array>
inline void release1DArrayPointer(JNIEnv *, const Array &, ElemType *)
{
}

#define _1DARRAYPOINTERFUNCS(jtype, jarraytype, getFunc, releaseFunc) \
    template <> \
    inline jtype *get1DArrayPointer<jtype, jarraytype>(JNIEnv *jenv, const jarraytype &array) \
    { \
        return jenv->getFunc(array, nullptr); \
    } \
    template <> \
    inline void release1DArrayPointer<jtype, jarraytype>(JNIEnv *jenv, const jarraytype &array, jtype *elems) \
    { \
        jenv->releaseFunc(array, elems, 0); \
    } \

_1DARRAYPOINTERFUNCS(jbyte, jbyteArray, GetByteArrayElements, ReleaseByteArrayElements)
_1DARRAYPOINTERFUNCS(jshort, jshortArray, GetShortArrayElements, ReleaseShortArrayElements)
_1DARRAYPOINTERFUNCS(jint, jintArray, GetIntArrayElements, ReleaseIntArrayElements)
_1DARRAYPOINTERFUNCS(jfloat, jfloatArray, GetFloatArrayElements, ReleaseFloatArrayElements)
_1DARRAYPOINTERFUNCS(jdouble, jdoubleArray, GetDoubleArrayElements, ReleaseDoubleArrayElements)

}

//
// 1D array
//

template <typename ElemType, typename Array>
ElemType *get1DArray(
    JNIEnv *jenv,
    const Array &array,
    StreamExecutionPolicy executionPolicy,
    size_t &lengthOut)
{
    ElemType *ptr = nullptr;
    size_t length = jenv->GetArrayLength(array);

    switch(executionPolicy)
    {
    case StreamExecutionPolicy::EFFICIENT:
        ptr = (ElemType *)jenv->GetPrimitiveArrayCritical(array, nullptr);
        break;

    case StreamExecutionPolicy::THREAD_SAFE:
    {
        ptr = detail::get1DArrayPointer<ElemType>(jenv, array);
        break;
    }

    default:
        break;
    }

    if(ptr) lengthOut = length;

    return ptr;
}

template <typename ElemType, typename Array>
void release1DArray(
    JNIEnv *jenv,
    const Array &array,
    StreamExecutionPolicy executionPolicy,
    ElemType *buffer)
{
    switch(executionPolicy)
    {
    case StreamExecutionPolicy::EFFICIENT:
        jenv->ReleasePrimitiveArrayCritical(array, buffer, 0);
        break;

    case StreamExecutionPolicy::THREAD_SAFE:
    {
        detail::release1DArrayPointer(jenv, array, buffer);
        break;
    }

    default:
        break;
    }
}

//
// 2D array
//

bool twoDimArrayLengthsMatch(
    JNIEnv *jenv,
    const jobjectArray &array);

}}
