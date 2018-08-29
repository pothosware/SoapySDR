// Copyright (c) 2017-2018 Coburn Wightman
//
// derived from SoapyRemote/client/ClientStreamData.cpp
// Copyright (c) 2015-2018 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/ConverterPrimitives.hpp>
#include <SoapySDR/ConverterRegistry.hpp>
#include <SoapySDR/Formats.hpp>
#include <cstring> //memcpy

// ********************************
// Real Soapy Formats

// Copy Converters

// F32 <> F32
static void genericF32toF32(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 1;

  if (scaler == 1.0)
    {
      const size_t sampleSize = sizeof(float);
      std::memcpy(dstBuff, srcBuff, numElems*elemDepth*sampleSize);
    }
  else
    {
      auto *src = (float*)srcBuff;
      auto *dst = (float*)dstBuff;
      for (size_t i = 0; i < numElems*elemDepth; i++)
        {
          dst[i] = float(src[i]) * scaler;
        }
    }
}

// S32 <> S32
static void genericS32toS32(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 1;

  if (scaler == 1.0)
    {
      const size_t sampleSize = sizeof(int32_t);
      std::memcpy(dstBuff, srcBuff, numElems*elemDepth*sampleSize);
    }
  else
    {
      auto *src = (int32_t*)srcBuff;
      auto *dst = (int32_t*)dstBuff;
      for (size_t i = 0; i < numElems*elemDepth; i++)
        {
          dst[i] = int32_t(src[i]) * scaler;
        }
    }
}

// S16 <> S16
static void genericS16toS16(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 1;

  if (scaler == 1.0)
    {
      const size_t sampleSize = sizeof(int16_t);
      std::memcpy(dstBuff, srcBuff, numElems*elemDepth*sampleSize);
    }
  else
    {
      auto *src = (int16_t*)srcBuff;
      auto *dst = (int16_t*)dstBuff;
      for (size_t i = 0; i < numElems*elemDepth; i++)
        {
          dst[i] = int16_t(src[i]) * scaler;
        }
    }
}

// S8 <> S8
static void genericS8toS8(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 1;

  if (scaler == 1.0)
    {
      const size_t sampleSize = sizeof(int8_t);
      std::memcpy(dstBuff, srcBuff, numElems*elemDepth*sampleSize);
    }
  else
    {
      auto *src = (int8_t*)srcBuff;
      auto *dst = (int8_t*)dstBuff;
      for (size_t i = 0; i < numElems*elemDepth; i++)
        {
          dst[i] = int8_t(src[i]) * scaler;
        }
    }
}

// Type Converters

// F32 <> S16
static void genericF32toS16(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 1;

  auto *src = (float*)srcBuff;
  auto *dst = (int16_t*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::F32toS16(src[i] * scaler);
    }
}

static void genericS16toF32(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 1;

  auto *src = (int16_t*)srcBuff;
  auto *dst = (float*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::S16toF32(src[i]) * scaler;
    }
}

// F32 <> U16
static void genericF32toU16(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 1;

  auto *src = (float*)srcBuff;
  auto *dst = (uint16_t*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::F32toU16(src[i] * scaler);
    }
}

static void genericU16toF32(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 1;

  auto *src = (uint16_t*)srcBuff;
  auto *dst = (float*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::U16toF32(src[i]) * scaler;
    }
}

// F32 <> S8
static void genericF32toS8(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 1;

  auto *src = (float*)srcBuff;
  auto *dst = (int8_t*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::F32toS8(src[i] * scaler);
    }
}

static void genericS8toF32(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 1;

  auto *src = (int8_t*)srcBuff;
  auto *dst = (float*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::S8toF32(src[i]) * scaler;
    }
}

// F32 <> U8
static void genericF32toU8(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 1;

  auto *src = (float*)srcBuff;
  auto *dst = (uint8_t*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::F32toU8(src[i] * scaler);
    }
}

static void genericU8toF32(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 1;

  auto *src = (uint8_t*)srcBuff;
  auto *dst = (float*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::U8toF32(src[i]) * scaler;
    }
}

// S16 <> U16
static void genericS16toU16(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 1;

  auto *src = (int16_t*)srcBuff;
  auto *dst = (uint16_t*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::S16toU16(src[i] * scaler);
    }
}

static void genericU16toS16(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 1;

  auto *src = (uint16_t*)srcBuff;
  auto *dst = (int16_t*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::U16toS16(src[i]) * scaler;
    }
}

// S16 <> S8
static void genericS16toS8(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 1;

  auto *src = (int16_t*)srcBuff;
  auto *dst = (int8_t*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::S16toS8(src[i] * scaler);
    }
}

static void genericS8toS16(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 1;

  auto *src = (int8_t*)srcBuff;
  auto *dst = (int16_t*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::S8toS16(src[i]) * scaler;
    }
}

// S16 <> U8
static void genericS16toU8(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 1;

  auto *src = (int16_t*)srcBuff;
  auto *dst = (uint8_t*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::S16toU8(src[i] * scaler);
    }
}

static void genericU8toS16(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 1;

  auto *src = (uint8_t*)srcBuff;
  auto *dst = (int16_t*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::U8toS16(src[i]) * scaler;
    }
}

// U16 <> S8
static void genericU16toS8(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 1;

  auto *src = (uint16_t*)srcBuff;
  auto *dst = (int8_t*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::U16toS8(src[i] * scaler);
    }
}

static void genericS8toU16(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 1;

  auto *src = (int8_t*)srcBuff;
  auto *dst = (uint16_t*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::S8toU16(src[i]) * scaler;
    }
}

// S8 <> U8
static void genericS8toU8(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 1;

  auto *src = (int8_t*)srcBuff;
  auto *dst = (uint8_t*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::S8toU8(src[i] * scaler);
    }
}

static void genericU8toS8(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 1;

  auto *src = (uint8_t*)srcBuff;
  auto *dst = (int8_t*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::U8toS8(src[i]) * scaler;
    }
}

// ********************************
// Complex Data Types

// Copy Converters

// CF32 <> CF32
static void genericCF32toCF32(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 2;

  if (scaler == 1.0)
    {
      const size_t sampleSize = sizeof(float);
      std::memcpy(dstBuff, srcBuff, numElems*elemDepth*sampleSize);
    }
  else
    {
      auto *src = (float*)srcBuff;
      auto *dst = (float*)dstBuff;
      for (size_t i = 0; i < numElems*elemDepth; i++)
        {
          dst[i] = float(src[i]) * scaler;
        }
    }
}

// CS32 <> CS32
static void genericCS32toCS32(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 2;

  if (scaler == 1.0)
    {
      const size_t sampleSize = sizeof(int32_t);
      std::memcpy(dstBuff, srcBuff, numElems*elemDepth*sampleSize);
    }
  else
    {
      auto *src = (int32_t*)srcBuff;
      auto *dst = (int32_t*)dstBuff;
      for (size_t i = 0; i < numElems*elemDepth; i++)
        {
          dst[i] = int32_t(src[i]) * scaler;
        }
    }
}

// CS16 <> CS16
static void genericCS16toCS16(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 2;

  if (scaler == 1.0)
    {
      const size_t sampleSize = sizeof(int16_t);
      std::memcpy(dstBuff, srcBuff, numElems*elemDepth*sampleSize);
    }
  else
    {
      auto *src = (int16_t*)srcBuff;
      auto *dst = (int16_t*)dstBuff;
      for (size_t i = 0; i < numElems*elemDepth; i++)
        {
          dst[i] = int16_t(src[i]) * scaler;
        }
    }
}

// CS8 <> CS8
static void genericCS8toCS8(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 2;

  if (scaler == 1.0)
    {
      const size_t sampleSize = sizeof(int8_t);
      std::memcpy(dstBuff, srcBuff, numElems*elemDepth*sampleSize);
    }
  else
    {
      auto *src = (int8_t*)srcBuff;
      auto *dst = (int8_t*)dstBuff;
      for (size_t i = 0; i < numElems*elemDepth; i++)
        {
          dst[i] = int8_t(src[i]) * scaler;
        }
    }
}

// Type Converters

// CF32 <> CS16
static void genericCF32toCS16(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 2;

  auto *src = (float*)srcBuff;
  auto *dst = (int16_t*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::F32toS16(src[i] * scaler);
    }
}

static void genericCS16toCF32(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 2;

  auto *src = (int16_t*)srcBuff;
  auto *dst = (float*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::S16toF32(src[i]) * scaler;
    }
}

// CF32 <> CU16
static void genericCF32toCU16(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 2;

  auto *src = (float*)srcBuff;
  auto *dst = (uint16_t*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::F32toU16(src[i] * scaler);
    }
}

static void genericCU16toCF32(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 2;

  auto *src = (uint16_t*)srcBuff;
  auto *dst = (float*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::U16toF32(src[i]) * scaler;
    }
}

// CF32 <> CS8
static void genericCF32toCS8(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 2;

  auto *src = (float*)srcBuff;
  auto *dst = (int8_t*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::F32toS8(src[i] * scaler);
    }
}

static void genericCS8toCF32(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 2;

  auto *src = (int8_t*)srcBuff;
  auto *dst = (float*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::S8toF32(src[i]) * scaler;
    }
}

// CF32 <> CU8
static void genericCF32toCU8(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 2;

  auto *src = (float*)srcBuff;
  auto *dst = (uint8_t*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::F32toU8(src[i] * scaler);
    }
}

static void genericCU8toCF32(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 2;

  auto *src = (uint8_t*)srcBuff;
  auto *dst = (float*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::U8toF32(src[i]) * scaler;
    }
}

// CS16 <> CU16
static void genericCS16toCU16(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 2;

  auto *src = (int16_t*)srcBuff;
  auto *dst = (uint16_t*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::S16toU16(src[i] * scaler);
    }
}

static void genericCU16toCS16(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 2;

  auto *src = (uint16_t*)srcBuff;
  auto *dst = (int16_t*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::U16toS16(src[i]) * scaler;
    }
}

// CS16 <> CS8
static void genericCS16toCS8(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 2;

  auto *src = (int16_t*)srcBuff;
  auto *dst = (int8_t*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::S16toS8(src[i] * scaler);
    }
}

static void genericCS8toCS16(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 2;

  auto *src = (int8_t*)srcBuff;
  auto *dst = (int16_t*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::S8toS16(src[i]) * scaler;
    }
}

// CS16 <> CU8
static void genericCS16toCU8(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 2;

  auto *src = (int16_t*)srcBuff;
  auto *dst = (uint8_t*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::S16toU8(src[i] * scaler);
    }
}

static void genericCU8toCS16(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 2;

  auto *src = (uint8_t*)srcBuff;
  auto *dst = (int16_t*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::U8toS16(src[i]) * scaler;
    }
}

// CU16 <> CS8
static void genericCU16toCS8(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 2;

  auto *src = (uint16_t*)srcBuff;
  auto *dst = (int8_t*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::U16toS8(src[i] * scaler);
    }
}

static void genericCS8toCU16(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 2;

  auto *src = (int8_t*)srcBuff;
  auto *dst = (uint16_t*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::S8toU16(src[i]) * scaler;
    }
}

// CS8 <> CU8
static void genericCS8toCU8(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 2;

  auto *src = (int8_t*)srcBuff;
  auto *dst = (uint8_t*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::S8toU8(src[i] * scaler);
    }
}

static void genericCU8toCS8(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 2;

  auto *src = (uint8_t*)srcBuff;
  auto *dst = (int8_t*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::U8toS8(src[i]) * scaler;
    }
}

/*!
 * lateLoadDefaultConverters() is called by loadModules()
 * to load the converters on-demand/not statically.
 * This works around an issue when a loading module
 * is linked against an older copy of SoapySDR
 * which also tries to load its converters
 * into the running copy of the library.
 */
void lateLoadDefaultConverters(void)
{
    static SoapySDR::ConverterRegistry registerGenericF32toF32(SOAPY_SDR_F32, SOAPY_SDR_F32, SoapySDR::ConverterRegistry::GENERIC, &genericF32toF32);
    static SoapySDR::ConverterRegistry registerGenericS32toS32(SOAPY_SDR_S32, SOAPY_SDR_S32, SoapySDR::ConverterRegistry::GENERIC, &genericS32toS32);
    static SoapySDR::ConverterRegistry registerGenericS16toS16(SOAPY_SDR_S16, SOAPY_SDR_S16, SoapySDR::ConverterRegistry::GENERIC, &genericS16toS16);
    static SoapySDR::ConverterRegistry registerGenericS8toS8(SOAPY_SDR_S8, SOAPY_SDR_S8, SoapySDR::ConverterRegistry::GENERIC, &genericS8toS8);
    static SoapySDR::ConverterRegistry registerGenericF32toS16(SOAPY_SDR_F32, SOAPY_SDR_S16, SoapySDR::ConverterRegistry::GENERIC, &genericF32toS16);
    static SoapySDR::ConverterRegistry registerGenericS16toF32(SOAPY_SDR_S16, SOAPY_SDR_F32, SoapySDR::ConverterRegistry::GENERIC, &genericS16toF32);
    static SoapySDR::ConverterRegistry registerGenericF32toU16(SOAPY_SDR_F32, SOAPY_SDR_U16, SoapySDR::ConverterRegistry::GENERIC, &genericF32toU16);
    static SoapySDR::ConverterRegistry registerGenericU16toF32(SOAPY_SDR_U16, SOAPY_SDR_F32, SoapySDR::ConverterRegistry::GENERIC, &genericU16toF32);
    static SoapySDR::ConverterRegistry registerGenericF32toS8(SOAPY_SDR_F32, SOAPY_SDR_S8, SoapySDR::ConverterRegistry::GENERIC, &genericF32toS8);
    static SoapySDR::ConverterRegistry registerGenericS8toF32(SOAPY_SDR_S8, SOAPY_SDR_F32, SoapySDR::ConverterRegistry::GENERIC, &genericS8toF32);
    static SoapySDR::ConverterRegistry registerGenericF32toU8(SOAPY_SDR_F32, SOAPY_SDR_U8, SoapySDR::ConverterRegistry::GENERIC, &genericF32toU8);
    static SoapySDR::ConverterRegistry registerGenericU8toF32(SOAPY_SDR_U8, SOAPY_SDR_F32, SoapySDR::ConverterRegistry::GENERIC, &genericU8toF32);
    static SoapySDR::ConverterRegistry registerGenericS16toU16(SOAPY_SDR_S16, SOAPY_SDR_U16, SoapySDR::ConverterRegistry::GENERIC, &genericS16toU16);
    static SoapySDR::ConverterRegistry registerGenericU16toS16(SOAPY_SDR_U16, SOAPY_SDR_S16, SoapySDR::ConverterRegistry::GENERIC, &genericU16toS16);
    static SoapySDR::ConverterRegistry registerGenericS16toS8(SOAPY_SDR_S16, SOAPY_SDR_S8, SoapySDR::ConverterRegistry::GENERIC, &genericS16toS8);
    static SoapySDR::ConverterRegistry registerGenericS8toS16(SOAPY_SDR_S8, SOAPY_SDR_S16, SoapySDR::ConverterRegistry::GENERIC, &genericS8toS16);
    static SoapySDR::ConverterRegistry registerGenericS16toU8(SOAPY_SDR_S16, SOAPY_SDR_U8, SoapySDR::ConverterRegistry::GENERIC, &genericS16toU8);
    static SoapySDR::ConverterRegistry registerGenericU8toS16(SOAPY_SDR_U8, SOAPY_SDR_S16, SoapySDR::ConverterRegistry::GENERIC, &genericU8toS16);
    static SoapySDR::ConverterRegistry registerGenericU16toS8(SOAPY_SDR_U16, SOAPY_SDR_S8, SoapySDR::ConverterRegistry::GENERIC, &genericU16toS8);
    static SoapySDR::ConverterRegistry registerGenericS8toU16(SOAPY_SDR_S8, SOAPY_SDR_U16, SoapySDR::ConverterRegistry::GENERIC, &genericS8toU16);
    static SoapySDR::ConverterRegistry registerGenericS8toU8(SOAPY_SDR_S8, SOAPY_SDR_U8, SoapySDR::ConverterRegistry::GENERIC, &genericS8toU8);
    static SoapySDR::ConverterRegistry registerGenericU8toS8(SOAPY_SDR_U8, SOAPY_SDR_S8, SoapySDR::ConverterRegistry::GENERIC, &genericU8toS8);
    static SoapySDR::ConverterRegistry registerGenericCF32toCF32(SOAPY_SDR_CF32, SOAPY_SDR_CF32, SoapySDR::ConverterRegistry::GENERIC, &genericCF32toCF32);
    static SoapySDR::ConverterRegistry registerGenericCS32toCS32(SOAPY_SDR_CS32, SOAPY_SDR_CS32, SoapySDR::ConverterRegistry::GENERIC, &genericCS32toCS32);
    static SoapySDR::ConverterRegistry registerGenericCS16toCS16(SOAPY_SDR_CS16, SOAPY_SDR_CS16, SoapySDR::ConverterRegistry::GENERIC, &genericCS16toCS16);
    static SoapySDR::ConverterRegistry registerGenericCS8toCS8(SOAPY_SDR_CS8, SOAPY_SDR_CS8, SoapySDR::ConverterRegistry::GENERIC, &genericCS8toCS8);
    static SoapySDR::ConverterRegistry registerGenericCF32toCS16(SOAPY_SDR_CF32, SOAPY_SDR_CS16, SoapySDR::ConverterRegistry::GENERIC, &genericCF32toCS16);
    static SoapySDR::ConverterRegistry registerGenericCS16toCF32(SOAPY_SDR_CS16, SOAPY_SDR_CF32, SoapySDR::ConverterRegistry::GENERIC, &genericCS16toCF32);
    static SoapySDR::ConverterRegistry registerGenericCF32toCU16(SOAPY_SDR_CF32, SOAPY_SDR_CU16, SoapySDR::ConverterRegistry::GENERIC, &genericCF32toCU16);
    static SoapySDR::ConverterRegistry registerGenericCU16toCF32(SOAPY_SDR_CU16, SOAPY_SDR_CF32, SoapySDR::ConverterRegistry::GENERIC, &genericCU16toCF32);
    static SoapySDR::ConverterRegistry registerGenericCF32toCS8(SOAPY_SDR_CF32, SOAPY_SDR_CS8, SoapySDR::ConverterRegistry::GENERIC, &genericCF32toCS8);
    static SoapySDR::ConverterRegistry registerGenericCS8toCF32(SOAPY_SDR_CS8, SOAPY_SDR_CF32, SoapySDR::ConverterRegistry::GENERIC, &genericCS8toCF32);
    static SoapySDR::ConverterRegistry registerGenericCF32toCU8(SOAPY_SDR_CF32, SOAPY_SDR_CU8, SoapySDR::ConverterRegistry::GENERIC, &genericCF32toCU8);
    static SoapySDR::ConverterRegistry registerGenericCU8toCF32(SOAPY_SDR_CU8, SOAPY_SDR_CF32, SoapySDR::ConverterRegistry::GENERIC, &genericCU8toCF32);
    static SoapySDR::ConverterRegistry registerGenericCS16toCU16(SOAPY_SDR_CS16, SOAPY_SDR_CU16, SoapySDR::ConverterRegistry::GENERIC, &genericCS16toCU16);
    static SoapySDR::ConverterRegistry registerGenericCU16toCS16(SOAPY_SDR_CU16, SOAPY_SDR_CS16, SoapySDR::ConverterRegistry::GENERIC, &genericCU16toCS16);
    static SoapySDR::ConverterRegistry registerGenericCS16toCS8(SOAPY_SDR_CS16, SOAPY_SDR_CS8, SoapySDR::ConverterRegistry::GENERIC, &genericCS16toCS8);
    static SoapySDR::ConverterRegistry registerGenericCS8toCS16(SOAPY_SDR_CS8, SOAPY_SDR_CS16, SoapySDR::ConverterRegistry::GENERIC, &genericCS8toCS16);
    static SoapySDR::ConverterRegistry registerGenericCS16toCU8(SOAPY_SDR_CS16, SOAPY_SDR_CU8, SoapySDR::ConverterRegistry::GENERIC, &genericCS16toCU8);
    static SoapySDR::ConverterRegistry registerGenericCU8toCS16(SOAPY_SDR_CU8, SOAPY_SDR_CS16, SoapySDR::ConverterRegistry::GENERIC, &genericCU8toCS16);
    static SoapySDR::ConverterRegistry registerGenericCU16toCS8(SOAPY_SDR_CU16, SOAPY_SDR_CS8, SoapySDR::ConverterRegistry::GENERIC, &genericCU16toCS8);
    static SoapySDR::ConverterRegistry registerGenericCS8toCU16(SOAPY_SDR_CS8, SOAPY_SDR_CU16, SoapySDR::ConverterRegistry::GENERIC, &genericCS8toCU16);
    static SoapySDR::ConverterRegistry registerGenericCS8toCU8(SOAPY_SDR_CS8, SOAPY_SDR_CU8, SoapySDR::ConverterRegistry::GENERIC, &genericCS8toCU8);
    static SoapySDR::ConverterRegistry registerGenericCU8toCS8(SOAPY_SDR_CU8, SOAPY_SDR_CS8, SoapySDR::ConverterRegistry::GENERIC, &genericCU8toCS8);
}
