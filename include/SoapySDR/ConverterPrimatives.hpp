///
/// \file SoapySDR/ConverterPrimatives.hpp
///
/// inline Soapy real Format Converter primatives.
///
/// \copyright
/// Copyright (c) 2017-2017 Coburn Wightman
/// SPDX-License-Identifier: BSL-1.0
///

#pragma once
#include <stdint.h>

namespace SoapySDR
{
  
/*!
 * Conversion primatives for converting real values between Soapy Formats.
 * \param from a real value to convert from
 * \return the converted value
 */

// type conversion: float <> signed ints

inline int32_t F32toS32(float from){
  return int32_t(from);
}
inline float S32toF32(int32_t from){
  return float(from);
}

inline int16_t F32toS16(float from){
  return int16_t(from);
}
inline float S16toF32(int16_t from){
  return float(from);
}

inline int8_t F32toS8(float from){
  return int8_t(from);
}
inline float S8toF32(int8_t from){
  return float(from);
}


// type conversion: unsigned <> signed ints

inline int32_t U32toS32(uint32_t from){
  return int32_t(from - 0x7fffffff);
}

inline uint32_t S32toU32(int32_t from){
  return uint32_t(from + 0x7fffffff);
}

inline int16_t U16toS16(uint16_t from){
  return int16_t(from - 0x7fff);
}

inline uint16_t S16toU16(int16_t from){
  return uint16_t(from + 0x7fff);
}

inline int8_t U8toS8(uint8_t from){
  return int8_t(from - 0x7f);
}

inline uint8_t S8toU8(int8_t from){
  return uint8_t(from + 0x7f);
}

// size conversion: signed <> signed

inline int16_t S32toS16(int32_t from){
  return int16_t(from >> 16);
}
inline int32_t S16toS32(int16_t from){
  return int32_t(from << 16);
}

inline int8_t S16toS8(int16_t from){
  return int8_t(from >> 8);
}
inline int16_t S8toS16(int8_t from){
  return int16_t(from << 8);
}

// compound conversions

// float <> unsigned (type and size)

inline uint32_t F32toU32(float from){
  return S32toU32(F32toS32(from));
}
inline float U32toF32(uint32_t from){
  return S32toF32(U32toS32(from));
}

inline uint16_t F32toU16(float from){
  return S16toU16(F32toS16(from));
}
inline float U16toF32(uint16_t from){
  return S16toF32(U16toS16(from));
}

inline uint8_t F32toU8(float from){
  return S8toU8(F32toS8(from));
}
inline float U8toF32(uint8_t from){
  return S8toF32(U8toS8(from));
}

// signed <> unsigned (type and size)

inline uint16_t S32toU16(int32_t from){
  return S16toU16(S32toS16(from));
}
inline int32_t U16toS32(uint16_t from){
  return S16toS32(U16toS16(from));
}

inline uint8_t S32toU8(int32_t from){
  return S8toU8(S16toS8(S32toS16(from)));
}
inline int32_t U8toS32(uint8_t from){
  return S16toS32(S8toS16(U8toS8(from)));
}

inline uint8_t S16toU8(int16_t from){
  return S8toU8(S16toS8(from));
}
inline int16_t U8toS16(uint8_t from){
  return S8toS16(U8toS8(from));
}

inline int8_t U16toS8(uint16_t from){
  return S16toS8(U16toS16(from));
}
inline uint16_t S8toU16(int8_t from){
  return S16toU16(S8toS16(from));
}


}
