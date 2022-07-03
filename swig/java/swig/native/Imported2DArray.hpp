// Copyright (c) 2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "BufferHandling.hpp"

#include <jni.h>

#include <algorithm>
#include <vector>

namespace SoapySDR { namespace Java {

template <typename ElemType, typename Array>
struct ArrayRef
{
    void *pointer{nullptr};
    Array arrayRef;
};

template <typename ElemType, typename Array>
class Imported2DArray: public std::vector<ArrayRef<ElemType, Array>>
{
public:
    Imported2DArray<ElemType, Array>(
        JNIEnv *env,
        const jobjectArray &array,
        const StreamExecutionPolicy executionPolicy
    ):
        _env(env),
        _array(array),
        _executionPolicy(executionPolicy)
    {
        for(int i = 0; i < env->GetArrayLength(array); ++i)
        {
            ArrayRef<ElemType, Array> arrayRef;
            arrayRef.arrayRef = (Array)env->GetObjectArrayElement(array, i);
            arrayRef.pointer = get1DArray<ElemType, Array>(env, arrayRef.arrayRef, executionPolicy, _innerDimension);

            this->emplace_back(std::move(arrayRef));
        }
    }

    inline static Imported2DArray<ElemType, Array> *import(
        JNIEnv *env,
        const jobjectArray &array,
        const StreamExecutionPolicy executionPolicy)
    {
        return new Imported2DArray<ElemType, Array>(env, array, executionPolicy);
    }

    // This isn't RAII, we need to manually release. We need this
    // because releaseAll is templated, and we can't do that in
    // a destructor.
    virtual ~Imported2DArray(void) = default;

    inline size_t innerDimension(void) const noexcept
    {
        return _innerDimension;
    }

    std::vector<void *> pointers(void) const
    {
        std::vector<void *> ret;
        std::transform(
            this->begin(),
            this->end(),
            std::back_inserter(ret),
            [](const ArrayRef<ElemType, Array> &arrayRef)
            {
                return arrayRef.pointer;
            });

        return ret;
    }

    inline bool anyNull(void) const
    {
        return std::any_of(
            this->begin(),
            this->end(),
            [](const ArrayRef<ElemType, Array> &arrayRef)
            {
                return !arrayRef.pointer;
            });
    }

    void releaseAll(void)
    {
        for(const auto &elem: (*this))
        {
            release1DArray<ElemType, Array>(
                _env,
                (Array)elem.arrayRef,
                _executionPolicy,
                (ElemType *)elem.pointer);

            _env->DeleteLocalRef(elem.arrayRef);
        }

        this->clear();
    }

private:
    JNIEnv *_env{nullptr};
    const jobjectArray &_array;
    StreamExecutionPolicy _executionPolicy{StreamExecutionPolicy::EFFICIENT};

    size_t _innerDimension{0};
};

}}
