// Copyright (c) 2020-2021 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <type_traits>
#include <vector>

// SWIG seems to struggle with size_t/uintptr_t, even with custom typemap stuff.
#if defined(SIZE_T_IS_UNSIGNED_INT)
using SWIGSize = uint32_t;
#else
using SWIGSize = uint64_t;
#endif

using SWIGSizeVector = std::vector<SWIGSize>;

static_assert(sizeof(SWIGSize) == sizeof(void*), "Can't reinterpret_cast size type to void*");

namespace detail
{
    template <typename Out, typename In>
    static inline typename std::enable_if<std::is_same<In, Out>::value, std::vector<Out>>::type copyVector(const std::vector<In>& vec)
    {
        return vec;
    }

    template <typename Out, typename In>
    static inline typename std::enable_if<!std::is_same<In, Out>::value, std::vector<Out>>::type copyVector(const std::vector<In>& vec)
    {
        std::vector<Out> ret;
        std::transform(
            vec.begin(),
            vec.end(),
            std::back_inserter(ret),
            [](const In num) {return static_cast<Out>(num); });

        return ret;
    }
}

template <typename Out, typename In>
static inline std::vector<Out> copyVector(const std::vector<In>& vec)
{
    return detail::copyVector<Out, In>(vec);
}

template <typename Out, typename In>
static inline std::vector<Out*> reinterpretCastVector(const std::vector<In>& vec)
{
    static_assert(sizeof(In) == sizeof(Out*), "In must be pointer-sized");

    std::vector<Out*> ret;
    std::transform(
        vec.begin(),
        vec.end(),
        std::back_inserter(ret),
        [](In elem) {return reinterpret_cast<Out*>(elem); });

    return ret;
}