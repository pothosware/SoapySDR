///
/// \file SoapySDR/ConverterPrimitives.hpp
///
/// inline Soapy real Format Converter primitives.
///
/// \copyright
/// Copyright (c) 2017-2017 Coburn Wightman
/// SPDX-License-Identifier: BSL-1.0
///

#pragma once
#include <stdint.h>
#include <type_traits>

namespace SoapySDR
{

  typedef int32_t S32;
  typedef int16_t S16;
  typedef int8_t  S8;

  typedef uint32_t U32;
  typedef uint16_t U16;
  typedef uint8_t  U8;
  
  typedef float F32;

  const uint32_t U32_ZERO_OFFSET = uint32_t(1<<31);
  const uint16_t U16_ZERO_OFFSET = uint16_t(1<<15);
  const uint8_t   U8_ZERO_OFFSET =  uint8_t(1<<7);

  const uint32_t S32_FULL_SCALE = (U32_ZERO_OFFSET-1);
  const uint16_t S16_FULL_SCALE = (U16_ZERO_OFFSET-1);
  const uint8_t   S8_FULL_SCALE =  (U8_ZERO_OFFSET-1);
  
/*!
 * Conversion primitives for converting real values between Soapy Formats.
 * \param from the value to convert from
 * \return the converted value
 */

  template <typename T_from, typename T_to>
  inline static T_to convertSample(T_from from){
    static_assert(std::is_same<T_from, T_to>::value, "convertSample() must be specialized for this type");
    return from;
  }
  
  // type conversion: float <> signed integers

  template <>
  inline S32 convertSample<F32, S32>(F32 from){
    return S32(from * S32_FULL_SCALE);
  }
  template<>
  inline F32 convertSample<S32, F32>(S32 from){
    return F32(from) / S32_FULL_SCALE;
  }

  template<>
  inline S16 convertSample<F32, S16>(F32 from){
    return S16(from * S16_FULL_SCALE);
  }
  template<>
  inline F32 convertSample<S16, F32>(S16 from){
    return F32(from) / S16_FULL_SCALE;
  }

  template<>
  inline S8 convertSample<F32, S8>(F32 from){
    return S8(from * S8_FULL_SCALE);
  }
  template<>
  inline F32 convertSample<S8, F32>(S8 from){
    return F32(from) / S8_FULL_SCALE;
  }

  // type conversion: offset binary <> two's complement (signed) integers

  template<>
  inline S32 convertSample<U32, S32>(U32 from){
    return S32(from - U32_ZERO_OFFSET);
  }
  template<>
  inline U32 convertSample<S32, U32>(S32 from){
    return U32(from) + U32_ZERO_OFFSET;
  }

  template<>
  inline S16 convertSample<U16, S16>(U16 from){
    return S16(from - U16_ZERO_OFFSET);
  }
  template<>
  inline U16 convertSample<S16, U16>(S16 from){
    return U16(from) + U16_ZERO_OFFSET;
  }

  template<>
  inline S8 convertSample<U8, S8>(U8 from){
    return S8(from - U8_ZERO_OFFSET);
  }
  template<>
  inline U8 convertSample<S8, U8>(S8 from){
    return U8(from) + U8_ZERO_OFFSET;
  }

// size conversion: signed <> signed
  template<>
  inline S16 convertSample<S32, S16>(S32 from){
    return S16(from >> 16);
  }
  template<>
  inline S32 convertSample<S16, S32>(S16 from){
    return S32(from << 16);
  }

  template<>
  inline S8 convertSample<S32, S8>(S32 from){
    return S8(from >> 24);
  }
  template<>
  inline S32 convertSample<S8, S32>(S8 from){
    return S32(from << 24);
  }
  
  template<>
  inline S8 convertSample<S16, S8>(S16 from){
    return S8(from >> 8);
  }
  template<>
  inline S16 convertSample<S8, S16>(S8 from){
    return S16(from << 8);
  }

// compound conversions

// float <> unsigned (type and size)

  template<>
  inline U32 convertSample<F32, U32>(F32 from){
    return convertSample<S32, U32>(convertSample<F32, S32>(from));
  }
  template<>
  inline F32 convertSample<U32, F32>(U32 from){
    return convertSample<S32, F32>(convertSample<U32, S32>(from));
  }

  template<>
  inline U16 convertSample<F32, U16>(F32 from){
    return convertSample<S16, U16>(convertSample<F32, S16>(from));
  }
  template<>
  inline F32 convertSample<U16, F32>(U16 from){
    return convertSample<S16, F32>(convertSample<U16, S16>(from));
  }

  template<>
  inline U8 convertSample<F32, U8>(F32 from){
    return convertSample<S8, U8>(convertSample<F32, S8>(from));
  }
  template<>
  inline F32 convertSample<U8, F32>(U8 from){
    return convertSample<S8, F32>(convertSample<U8, S8>(from));
  }

// signed <> unsigned (type and size)

  template <>
  inline U16 convertSample<S32, U16>(S32 from){
    return convertSample<S16, U16>(convertSample<S32, S16>(from));
  }
  template <>
  inline S32 convertSample<U16, S32>(U16 from){
    return convertSample<S16, S32>(convertSample<U16, S16>(from));
  }
  
  template <>
  inline S16 convertSample<U32, S16>(U32 from){
    return convertSample<S32, S16>(convertSample<U32, S32>(from));
  }
  template <>
  inline U32 convertSample<S16, U32>(S16 from){
    return convertSample<S32, U32>(convertSample<S16, S32>(from));
  }
  
  template <>
  inline U8 convertSample<S32, U8>(S32 from){
    return convertSample<S8, U8>(convertSample<S32, S8>(from));
  }
  template <>
  inline S32 convertSample<U8, S32>(U8 from){
    return convertSample<S8, S32>(convertSample<U8, S8>(from));
  }
  
  template <>
  inline S8 convertSample<U32, S8>(U32 from){
    return convertSample<S32, S8>(convertSample<U32, S32>(from));
  }
  template <>
  inline U32 convertSample<S8, U32>(S8 from){
    return convertSample<S32, U32>(convertSample<S8, S32>(from));
  }
  
  template <>
  inline U8 convertSample<S16, U8>(S16 from){
    return convertSample<S8, U8>(convertSample<S16, S8>(from));
  }
  template <>
  inline S16 convertSample<U8, S16>(U8 from){
    return convertSample<S8, S16>(convertSample<U8, S8>(from));
  }
  
  template <>
  inline S8 convertSample<U16, S8>(U16 from){
    return convertSample<S16, S8>(convertSample<U16, S16>(from));
  }

  template <>
  inline U16 convertSample<S8, U16>(S8 from){
    return convertSample<S16, U16>(convertSample<S8, S16>(from));
  }

}
