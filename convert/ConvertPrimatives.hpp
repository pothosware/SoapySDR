// ConvertPrimatives.hpp
// Copyright (c) 2017-2017 Coburn Wightman"
//
// derived from SoapyRemote/client/ClientStreamData.cpp
// Copyright (c) 2015-2017 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#pragma once
#include <stdint.h>

// Simple Primatives

// integers

inline int32_t U32toS32(int32_t from){
  return (from - 0x7fffffff);
}

inline int32_t U24toS32(int32_t from){
  return (from - 0x007fffff);
}

inline int32_t U16toS32(int16_t from){
  return (from - 0x7fff);
}

inline int32_t U8toS32(int8_t from){
  return (from - 0x7f);
}

inline int32_t S16toS32(int16_t from){
  return int32_t(from);
}

inline int16_t S16toS16(int16_t from){
  return int16_t(from);
}


// floats
inline int32_t F32toS32(float from){
  return int32_t(from);
}

inline float S16toF32(int16_t from){
  return float(from);
}

inline float U16toF32(int16_t from){
  return float(U16toS32(from));
}

inline float F32toF32(float from){
  return float(from);
}

// Complex primatives

///////////////////////////
// case CONVERT_CF32_CS12:
///////////////////////////
inline float* CS12toCF32(uint8_t* in, float* out, float scale){
  //  float out[2];
  uint16_t part0 = uint16_t(*(in++));
  uint16_t part1 = uint16_t(*(in++));
  uint16_t part2 = uint16_t(*(in++));
  int16_t i = int16_t((part1 << 12) | (part0 << 4));
  int16_t q = int16_t((part2 << 8) | (part1 & 0xf0));
  out[0] = float(i)*scale;
  out[1] = float(q)*scale;
  return out;
 }

///////////////////////////
// case CONVERT_CF32_CS16:
///////////////////////////
inline float* CF32toCF32(float* in, float* out, float scale){
  //const float scale = float(1.0/scaleFactor);
  //  float out[2];

  out[0] = F32toF32(in[0])*scale;
  out[1] = F32toF32(in[1])*scale;
  
  return out;
}

///////////////////////////
// case CONVERT_CF32_CS16:
///////////////////////////
inline float* CS16toCF32(int16_t* in, float* out, float scale){
  //const float scale = float(1.0/scaleFactor);
  //  float out[2];

  out[0] = S16toF32(in[0])*scale;
  out[1] = S16toF32(in[1])*scale;
  
  return out;
}

///////////////////////////
// case CONVERT_CF32_CU16:
///////////////////////////
inline float* CU16toCF32(uint16_t* in, float* out, float scale){
  //const float scale = float(1.0/scaleFactor);
  //  float out[2];

  out[0] = U16toF32(in[0])*scale;
  out[1] = U16toF32(in[1])*scale;
  
  return out;
}

///////////////////////////
// case CONVERT_CS32_CS16:
///////////////////////////
inline int32_t* CS16toCS32(int16_t* in, int32_t* out, float scale){
  //const float scale = float(1.0/scaleFactor);
  //  float out[2];

  out[0] = S16toS32(in[0])*scale;
  out[1] = S16toS32(in[1])*scale;
  
  return out;
}

///////////////////////////
// case CONVERT_CS16_CS16:
///////////////////////////
inline int16_t* CS16toCS16(int16_t* in, int16_t* out, float scale){
  //const float scale = float(1.0/scaleFactor);
  //  float out[2];

  out[0] = S16toS16(in[0])*scale;
  out[1] = S16toS16(in[1])*scale;
  
  return out;
}
