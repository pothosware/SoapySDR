// Copyright (c) 2017-2017 Coburn Wightman
//
// derived from SoapyRemote/client/ClientStreamData.cpp
// Copyright (c) 2015-2017 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/ConverterPrimatives.hpp>
#include <SoapySDR/ConverterRegistry.hpp>
#include <SoapySDR/Formats.hpp>
#include <cstring> //memcpy

#include <iostream>

// Copy Converters

// CF32 <> CF32
static void genericCF32toCF32(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 2;

  std::cout << "converting CF32 to CF32" << std::endl;

  if (scaler == 1.0)
    {
      const size_t elemSize = sizeof(float);
      std::memcpy(dstBuff, srcBuff, numElems*elemSize*elemDepth);
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

static SoapySDR::ConverterRegistry registerGenericCF32toCF32(SOAPY_SDR_CF32, SOAPY_SDR_CF32, SoapySDR::ConverterRegistry::GENERIC, &genericCF32toCF32);

// CS32 <> CS32
static void genericCS32toCS32(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 2;

  std::cout << "converting CS32 to CS32" << std::endl;

  if (scaler == 1.0)
    {
      const size_t elemSize = sizeof(int32_t);
      std::memcpy(dstBuff, srcBuff, numElems*elemSize*elemDepth);
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

static SoapySDR::ConverterRegistry registerGenericCS32toCS32(SOAPY_SDR_CS32, SOAPY_SDR_CS32, SoapySDR::ConverterRegistry::GENERIC, &genericCS32toCS32);

// CS16 <> CS16
static void genericCS16toCS16(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 2;

  std::cout << "converting CS16 to CS16" << std::endl;

  if (scaler == 1.0)
    {
      const size_t elemSize = sizeof(int16_t);
      std::memcpy(dstBuff, srcBuff, numElems*elemSize*elemDepth);
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

static SoapySDR::ConverterRegistry registerGenericCS16toCS16(SOAPY_SDR_CS16, SOAPY_SDR_CS16, SoapySDR::ConverterRegistry::GENERIC, &genericCS16toCS16);

// CS8 <> CS8
static void genericCS8toCS8(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 2;

  std::cout << "converting CS8 to CS8" << std::endl;

  if (scaler == 1.0)
    {
      const size_t elemSize = sizeof(int8_t);
      std::memcpy(dstBuff, srcBuff, numElems*elemSize*elemDepth);
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

static SoapySDR::ConverterRegistry registerGenericCS8toCS8(SOAPY_SDR_CS8, SOAPY_SDR_CS8, SoapySDR::ConverterRegistry::GENERIC, &genericCS8toCS8);


// Type Converters

// CF32 <> CS16
static void genericCF32toCS16(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 2;

  std::cout << "converting CF32 to CS16" << std::endl;

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

  std::cout << "converting CS16 to CF32" << std::endl;

  auto *src = (int16_t*)srcBuff;
  auto *dst = (float*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::S16toF32(src[i]) * scaler;
    }
}

static SoapySDR::ConverterRegistry registerGenericCF32toCS16(SOAPY_SDR_CF32, SOAPY_SDR_CS16, SoapySDR::ConverterRegistry::GENERIC, &genericCF32toCS16);
static SoapySDR::ConverterRegistry registerGenericCS16toCF32(SOAPY_SDR_CS16, SOAPY_SDR_CF32, SoapySDR::ConverterRegistry::GENERIC, &genericCS16toCF32);


// CF32 <> CU16
static void genericCF32toCU16(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 2;

  std::cout << "converting CF32 to CU16" << std::endl;

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

  std::cout << "converting CU16 to CF32" << std::endl;

  auto *src = (uint16_t*)srcBuff;
  auto *dst = (float*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::U16toF32(src[i]) * scaler;
    }
}

static SoapySDR::ConverterRegistry registerGenericCF32toCU16(SOAPY_SDR_CF32, SOAPY_SDR_CU16, SoapySDR::ConverterRegistry::GENERIC, &genericCF32toCU16);
static SoapySDR::ConverterRegistry registerGenericCU16toCF32(SOAPY_SDR_CU16, SOAPY_SDR_CF32, SoapySDR::ConverterRegistry::GENERIC, &genericCU16toCF32);


// CF32 <> CS8
static void genericCF32toCS8(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 2;

  std::cout << "converting CF32 to CS8" << std::endl;

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

  std::cout << "converting CS8 to CF32" << std::endl;

  auto *src = (int8_t*)srcBuff;
  auto *dst = (float*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::S8toF32(src[i]) * scaler;
    }
}

static SoapySDR::ConverterRegistry registerGenericCF32toCS8(SOAPY_SDR_CF32, SOAPY_SDR_CU8, SoapySDR::ConverterRegistry::GENERIC, &genericCF32toCS8);
static SoapySDR::ConverterRegistry registerGenericCS8toCF32(SOAPY_SDR_CU8, SOAPY_SDR_CF32, SoapySDR::ConverterRegistry::GENERIC, &genericCS8toCF32);


// CF32 <> CU8
static void genericCF32toCU8(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 2;

  std::cout << "converting CF32 to CU8" << std::endl;

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

  std::cout << "converting CU8 to CF32" << std::endl;

  auto *src = (uint8_t*)srcBuff;
  auto *dst = (float*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::U8toF32(src[i]) * scaler;
    }
}

static SoapySDR::ConverterRegistry registerGenericCF32toCU8(SOAPY_SDR_CF32, SOAPY_SDR_CS8, SoapySDR::ConverterRegistry::GENERIC, &genericCF32toCU8);
static SoapySDR::ConverterRegistry registerGenericCU8toCF32(SOAPY_SDR_CS8, SOAPY_SDR_CF32, SoapySDR::ConverterRegistry::GENERIC, &genericCU8toCF32);

// CS16 <> CU16
static void genericCS16toCU16(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 2;

  std::cout << "converting CS16 to CU16" << std::endl;

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

  std::cout << "converting CU16 to CS16" << std::endl;

  auto *src = (uint16_t*)srcBuff;
  auto *dst = (int16_t*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::U16toS16(src[i]) * scaler;
    }
}

static SoapySDR::ConverterRegistry registerGenericCS16toCU16(SOAPY_SDR_CS16, SOAPY_SDR_CU16, SoapySDR::ConverterRegistry::GENERIC, &genericCS16toCU16);
static SoapySDR::ConverterRegistry registerGenericCU16toCS16(SOAPY_SDR_CU16, SOAPY_SDR_CS16, SoapySDR::ConverterRegistry::GENERIC, &genericCU16toCS16);


// CS16 <> CS8
static void genericCS16toCS8(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 2;

  std::cout << "converting CS16 to CS8" << std::endl;

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

  std::cout << "converting CS8 to CS16" << std::endl;

  auto *src = (int8_t*)srcBuff;
  auto *dst = (int16_t*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::S8toS16(src[i]) * scaler;
    }
}

static SoapySDR::ConverterRegistry registerGenericCS16toCS8(SOAPY_SDR_CS16, SOAPY_SDR_CS8, SoapySDR::ConverterRegistry::GENERIC, &genericCS16toCS8);
static SoapySDR::ConverterRegistry registerGenericCS8toCS16(SOAPY_SDR_CS8, SOAPY_SDR_CS16, SoapySDR::ConverterRegistry::GENERIC, &genericCS8toCS16);

// CS16 <> CU8
static void genericCS16toCU8(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 2;

  std::cout << "converting CS16 to CU8" << std::endl;

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

  std::cout << "converting CU8 to CS16" << std::endl;

  auto *src = (uint8_t*)srcBuff;
  auto *dst = (int16_t*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::U8toS16(src[i]) * scaler;
    }
}

static SoapySDR::ConverterRegistry registerGenericCS16toCU8(SOAPY_SDR_CS16, SOAPY_SDR_CU8, SoapySDR::ConverterRegistry::GENERIC, &genericCS16toCU8);
static SoapySDR::ConverterRegistry registerGenericCU8toCS16(SOAPY_SDR_CU8, SOAPY_SDR_CS16, SoapySDR::ConverterRegistry::GENERIC, &genericCU8toCS16);

// CU16 <> CS8
static void genericCU16toCS8(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  const size_t elemDepth = 2;

  std::cout << "converting CU16 to CS8" << std::endl;

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

  std::cout << "converting CS8 to CU16" << std::endl;

  auto *src = (int8_t*)srcBuff;
  auto *dst = (uint16_t*)dstBuff;
  for (size_t i = 0; i < numElems*elemDepth; i++)
    {
      dst[i] = SoapySDR::S8toU16(src[i]) * scaler;
    }
}

static SoapySDR::ConverterRegistry registerGenericCU16toCS8(SOAPY_SDR_CU16, SOAPY_SDR_CS8, SoapySDR::ConverterRegistry::GENERIC, &genericCU16toCS8);
static SoapySDR::ConverterRegistry registerGenericCS8toCU16(SOAPY_SDR_CS8, SOAPY_SDR_CU16, SoapySDR::ConverterRegistry::GENERIC, &genericCS8toCU16);

