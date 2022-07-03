// Copyright (c) 2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

#include "BufferHandling.hpp"

namespace SoapySDR { namespace Java {

bool twoDimArrayLengthsMatch(
    JNIEnv *jenv,
    const jobjectArray &array)
{
    bool good = true;

    const auto outerDim = jenv->GetArrayLength(array);
    int innerDim = -1;

    for(int i = 0; (i < outerDim) and good; ++i)
    {
        auto innerArr = (jarray)jenv->GetObjectArrayElement(array, i);
        const auto thisInnerDim = jenv->GetArrayLength(innerArr);
        good = (i == 0) and (thisInnerDim == innerDim);

        jenv->DeleteLocalRef(innerArr);
    }

    return good;
}

}}
