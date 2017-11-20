// Copyright (c) 2017-2017 Coburn Wightman
//
// derived from SoapyRemote/client/ClientStreamData.cpp
// Copyright (c) 2015-2017 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include <SoapySDR/ConverterPrimatives.hpp>
#include <SoapySDR/ConverterRegistry.hpp>
#include <SoapySDR/Formats.hpp>
#include <cstring> //memcpy


// Copy Converter.  Source and Target formats are the same.
template <typename T_fmt, int T_elemDepth>

static void copyConverter(const T_fmt *srcBuff, T_fmt *dstBuff, const size_t numElems, const double scaler)
{
  if (scaler == 1.0)
    {
      const size_t sampleSize = sizeof(T_fmt);
      std::memcpy(dstBuff, srcBuff, numElems * T_elemDepth * sampleSize);
    }
  else
    {
      auto *src = (T_fmt*)srcBuff;
      auto *dst = (T_fmt*)dstBuff;
      for (size_t i = 0; i < numElems * T_elemDepth; i++)
	{
	  dst[i] = SoapySDR::convertSample<T_fmt, T_fmt>(src[i]) * scaler;
	}
    }
}

// Format Converter.  Source and Target formats are different.
template <typename T_src, typename T_dst, int T_elemDepth>

static void formatConverter(const T_src *srcBuff, T_dst *dstBuff, const size_t numElems, const double scaler)
{
  auto *src = (T_src*)srcBuff;
  auto *dst = (T_dst*)dstBuff;
  for (size_t i = 0; i < numElems * T_elemDepth; i++)
    {
      dst[i] = SoapySDR::convertSample<T_src, T_dst>(src[i]) * scaler;
    }
}
  
/************************************************************************************/
//
// build default REAL copy converters

static SoapySDR::ConverterRegistry
registerGenericF32toF32(SOAPY_SDR_F32, SOAPY_SDR_F32, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&copyConverter<float, 1>);

static SoapySDR::ConverterRegistry
registerGenericS32toS32(SOAPY_SDR_S32, SOAPY_SDR_S32, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&copyConverter<int32_t, 1>);

static SoapySDR::ConverterRegistry
registerGenericS16toS16(SOAPY_SDR_S16, SOAPY_SDR_S16, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&copyConverter<int16_t, 1>);

static SoapySDR::ConverterRegistry
registerGenericS8toS8(SOAPY_SDR_S8, SOAPY_SDR_S8, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&copyConverter<int8_t, 1>);

// build default REAL format converters

// F32 to signed
static SoapySDR::ConverterRegistry
registerGenericF32toS32(SOAPY_SDR_F32, SOAPY_SDR_S32, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<float, int32_t, 1>);
static SoapySDR::ConverterRegistry
registerGenericS32toF32(SOAPY_SDR_S32, SOAPY_SDR_F32, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<int32_t, float, 1>);

static SoapySDR::ConverterRegistry
registerGenericF32toS16(SOAPY_SDR_F32, SOAPY_SDR_S16, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<float, int16_t, 1>);
static SoapySDR::ConverterRegistry
registerGenericS16toF32(SOAPY_SDR_S16, SOAPY_SDR_F32, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<int16_t, float, 1>);

static SoapySDR::ConverterRegistry
registerGenericF32toS8(SOAPY_SDR_F32, SOAPY_SDR_S8, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<float, int8_t, 1>);
static SoapySDR::ConverterRegistry
registerGenericS8toF32(SOAPY_SDR_S8, SOAPY_SDR_F32, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<int8_t, float, 1>);

// F32 to unsigned
static SoapySDR::ConverterRegistry
registerGenericF32toU32(SOAPY_SDR_F32, SOAPY_SDR_U32, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<float, uint32_t, 1>);
static SoapySDR::ConverterRegistry
registerGenericU32toF32(SOAPY_SDR_U32, SOAPY_SDR_F32, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<uint32_t, float, 1>);

static SoapySDR::ConverterRegistry
registerGenericF32toU16(SOAPY_SDR_F32, SOAPY_SDR_U16, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<float, uint16_t, 1>);
static SoapySDR::ConverterRegistry
registerGenericU16toF32(SOAPY_SDR_U16, SOAPY_SDR_F32, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<uint16_t, float, 1>);

static SoapySDR::ConverterRegistry
registerGenericF32toU8(SOAPY_SDR_F32, SOAPY_SDR_U8, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<float, uint8_t, 1>);
static SoapySDR::ConverterRegistry
registerGenericU8toF32(SOAPY_SDR_U8, SOAPY_SDR_F32, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<uint8_t, float, 1>);

// S32 to signed
static SoapySDR::ConverterRegistry
registerGenericS32toS16(SOAPY_SDR_S32, SOAPY_SDR_S16, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<int32_t, int16_t, 1>);
static SoapySDR::ConverterRegistry
registerGenericS16toS32(SOAPY_SDR_S16, SOAPY_SDR_S32, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<int16_t, int32_t, 1>);

static SoapySDR::ConverterRegistry
registerGenericS32toS8(SOAPY_SDR_S32, SOAPY_SDR_S8, SoapySDR::ConverterRegistry::GENERIC,
		       (SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<int32_t, int8_t, 1>);
static SoapySDR::ConverterRegistry
registerGenericS8toS32(SOAPY_SDR_S8, SOAPY_SDR_S32, SoapySDR::ConverterRegistry::GENERIC,
		       (SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<int8_t, int32_t, 1>);

// S16 to signed
static SoapySDR::ConverterRegistry
registerGenericS16toS8(SOAPY_SDR_S16, SOAPY_SDR_S8, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<int16_t, int8_t, 1>);
static SoapySDR::ConverterRegistry
registerGenericS8toS16(SOAPY_SDR_S8, SOAPY_SDR_S16, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<int8_t, int16_t, 1>);

// U32 to signed
static SoapySDR::ConverterRegistry
registerGenericU32toS32(SOAPY_SDR_U32, SOAPY_SDR_S32, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<uint32_t, int32_t, 1>);
static SoapySDR::ConverterRegistry
registerGenericS32toU32(SOAPY_SDR_S32, SOAPY_SDR_U32, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<int32_t, uint32_t, 1>);

static SoapySDR::ConverterRegistry
registerGenericU32toS16(SOAPY_SDR_U32, SOAPY_SDR_S16, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<uint32_t, int16_t, 1>);
static SoapySDR::ConverterRegistry
registerGenericS16toU32(SOAPY_SDR_S16, SOAPY_SDR_U32, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<int16_t, uint32_t, 1>);

static SoapySDR::ConverterRegistry
registerGenericU32toS8(SOAPY_SDR_U32, SOAPY_SDR_S8, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<uint32_t, int8_t, 1>);
static SoapySDR::ConverterRegistry
registerGenericS8toU32(SOAPY_SDR_S8, SOAPY_SDR_U32, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<int8_t, uint32_t, 1>);

// U16 to signed
static SoapySDR::ConverterRegistry
registerGenericU16toS32(SOAPY_SDR_U16, SOAPY_SDR_S32, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<uint16_t, int32_t, 1>);
static SoapySDR::ConverterRegistry
registerGenericS32toU16(SOAPY_SDR_S32, SOAPY_SDR_U16, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<int32_t, uint16_t, 1>);

static SoapySDR::ConverterRegistry
registerGenericU16toS16(SOAPY_SDR_U16, SOAPY_SDR_S16, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<uint16_t, int16_t, 1>);
static SoapySDR::ConverterRegistry
registerGenericS16toU16(SOAPY_SDR_S16, SOAPY_SDR_U16, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<int16_t, uint16_t, 1>);

static SoapySDR::ConverterRegistry
registerGenericU16toS8(SOAPY_SDR_U16, SOAPY_SDR_S8, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<uint16_t, int8_t, 1>);
static SoapySDR::ConverterRegistry
registerGenericS8toU16(SOAPY_SDR_S8, SOAPY_SDR_U16, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<int8_t, uint16_t, 1>);

// U8 to signed
static SoapySDR::ConverterRegistry
registerGenericU8toS32(SOAPY_SDR_U8, SOAPY_SDR_S32, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<uint8_t, int32_t, 1>);
static SoapySDR::ConverterRegistry
registerGenericS32toU8(SOAPY_SDR_S32, SOAPY_SDR_U8, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<int32_t, uint8_t, 1>);

static SoapySDR::ConverterRegistry
registerGenericU8toS16(SOAPY_SDR_U8, SOAPY_SDR_S16, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<uint8_t, int16_t, 1>);
static SoapySDR::ConverterRegistry
registerGenericS16toU8(SOAPY_SDR_S16, SOAPY_SDR_U8, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<int16_t, uint8_t, 1>);

static SoapySDR::ConverterRegistry
registerGenericU8toS8(SOAPY_SDR_U8, SOAPY_SDR_S8, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<uint8_t, int8_t, 1>);
static SoapySDR::ConverterRegistry
registerGenericS8toU8(SOAPY_SDR_S8, SOAPY_SDR_U8, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<int8_t, uint8_t, 1>);


/************************************************************************************/
//
// build default COMPLEX copy converters

static SoapySDR::ConverterRegistry
registerGenericCF32toCF32(SOAPY_SDR_CF32, SOAPY_SDR_CF32, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&copyConverter<float, 2>);

static SoapySDR::ConverterRegistry
registerGenericCS32toCS32(SOAPY_SDR_CS32, SOAPY_SDR_CS32, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&copyConverter<int32_t, 2>);

static SoapySDR::ConverterRegistry
registerGenericCS16toCS16(SOAPY_SDR_CS16, SOAPY_SDR_CS16, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&copyConverter<int16_t, 2>);

static SoapySDR::ConverterRegistry
registerGenericCS8toCS8(SOAPY_SDR_CS8, SOAPY_SDR_CS8, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&copyConverter<int8_t, 2>);


// build default COMPLEX format converters

// CF32 to signed
static SoapySDR::ConverterRegistry
registerGenericCF32toCS32(SOAPY_SDR_CF32, SOAPY_SDR_CS32, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<float, int32_t, 2>);
static SoapySDR::ConverterRegistry
registerGenericCS32toCF32(SOAPY_SDR_CS32, SOAPY_SDR_CF32, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<int32_t, float, 2>);

static SoapySDR::ConverterRegistry
registerGenericCF32toCS16(SOAPY_SDR_CF32, SOAPY_SDR_CS16, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<float, int16_t, 2>);
static SoapySDR::ConverterRegistry
registerGenericCS16toCF32(SOAPY_SDR_CS16, SOAPY_SDR_CF32, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<int16_t, float, 2>);

static SoapySDR::ConverterRegistry
registerGenericCF32toCS8(SOAPY_SDR_CF32, SOAPY_SDR_CS8, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<float, int8_t, 2>);
static SoapySDR::ConverterRegistry
registerGenericCS8toCF32(SOAPY_SDR_CS8, SOAPY_SDR_CF32, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<int8_t, float, 2>);

// CF32 to unsigned
static SoapySDR::ConverterRegistry
registerGenericCF32toCU32(SOAPY_SDR_CF32, SOAPY_SDR_CU32, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<float, uint32_t, 2>);
static SoapySDR::ConverterRegistry
registerGenericCU32toCF32(SOAPY_SDR_CU32, SOAPY_SDR_CF32, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<uint32_t, float, 2>);

static SoapySDR::ConverterRegistry
registerGenericCF32toCU16(SOAPY_SDR_CF32, SOAPY_SDR_CU16, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<float, uint16_t, 2>);
static SoapySDR::ConverterRegistry
registerGenericCU16toCF32(SOAPY_SDR_CU16, SOAPY_SDR_CF32, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<uint16_t, float, 2>);

static SoapySDR::ConverterRegistry
registerGenericCF32toCU8(SOAPY_SDR_CF32, SOAPY_SDR_CU8, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<float, uint8_t, 2>);
static SoapySDR::ConverterRegistry
registerGenericCU8toCF32(SOAPY_SDR_CU8, SOAPY_SDR_CF32, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<uint8_t, float, 2>);

// CS32 to signed
static SoapySDR::ConverterRegistry
registerGenericCS32toCS16(SOAPY_SDR_CS32, SOAPY_SDR_CS16, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<int32_t, int16_t, 2>);
static SoapySDR::ConverterRegistry
registerGenericCS16toCS32(SOAPY_SDR_CS16, SOAPY_SDR_CS32, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<int16_t, int32_t, 2>);

static SoapySDR::ConverterRegistry
registerGenericCS32toCS8(SOAPY_SDR_CS32, SOAPY_SDR_CS8, SoapySDR::ConverterRegistry::GENERIC,
		       (SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<int32_t, int8_t, 2>);
static SoapySDR::ConverterRegistry
registerGenericCS8toCS32(SOAPY_SDR_CS8, SOAPY_SDR_CS32, SoapySDR::ConverterRegistry::GENERIC,
		       (SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<int8_t, int32_t, 2>);

// CS16 to signed
static SoapySDR::ConverterRegistry
registerGenericCS16toCS8(SOAPY_SDR_CS16, SOAPY_SDR_CS8, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<int16_t, int8_t, 2>);
static SoapySDR::ConverterRegistry
registerGenericCS8toCS16(SOAPY_SDR_CS8, SOAPY_SDR_CS16, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<int8_t, int16_t, 2>);

// CU32 to signed
static SoapySDR::ConverterRegistry
registerGenericCU32toCS32(SOAPY_SDR_CU32, SOAPY_SDR_CS32, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<uint32_t, int32_t, 2>);
static SoapySDR::ConverterRegistry
registerGenericCS32toCU32(SOAPY_SDR_CS32, SOAPY_SDR_CU32, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<int32_t, uint32_t, 2>);

static SoapySDR::ConverterRegistry
registerGenericCU32toCS16(SOAPY_SDR_CU32, SOAPY_SDR_CS16, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<uint32_t, int16_t, 2>);
static SoapySDR::ConverterRegistry
registerGenericCS16toCU32(SOAPY_SDR_CS16, SOAPY_SDR_CU32, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<int16_t, uint32_t, 2>);

static SoapySDR::ConverterRegistry
registerGenericCU32toCS8(SOAPY_SDR_CU32, SOAPY_SDR_CS8, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<uint32_t, int8_t, 2>);
static SoapySDR::ConverterRegistry
registerGenericCS8toCU32(SOAPY_SDR_CS8, SOAPY_SDR_CU32, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<int8_t, uint32_t, 2>);

// CU16 to signed
static SoapySDR::ConverterRegistry
registerGenericCU16toCS32(SOAPY_SDR_CU16, SOAPY_SDR_CS32, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<uint16_t, int32_t, 2>);
static SoapySDR::ConverterRegistry
registerGenericCS32toCU16(SOAPY_SDR_CS32, SOAPY_SDR_CU16, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<int32_t, uint16_t, 2>);

static SoapySDR::ConverterRegistry
registerGenericCU16toCS16(SOAPY_SDR_CU16, SOAPY_SDR_CS16, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<uint16_t, int16_t, 2>);
static SoapySDR::ConverterRegistry
registerGenericCS16toCU16(SOAPY_SDR_CS16, SOAPY_SDR_CU16, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<int16_t, uint16_t, 2>);

static SoapySDR::ConverterRegistry
registerGenericCU16toCS8(SOAPY_SDR_CU16, SOAPY_SDR_CS8, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<uint16_t, int8_t, 2>);
static SoapySDR::ConverterRegistry
registerGenericCS8toCU16(SOAPY_SDR_CS8, SOAPY_SDR_CU16, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<int8_t, uint16_t, 2>);

// CU8 to signed
static SoapySDR::ConverterRegistry
registerGenericCU8toCS32(SOAPY_SDR_CU8, SOAPY_SDR_CS32, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<uint8_t, int32_t, 2>);
static SoapySDR::ConverterRegistry
registerGenericCS32toCU8(SOAPY_SDR_CS32, SOAPY_SDR_CU8, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<int32_t, uint8_t, 2>);

static SoapySDR::ConverterRegistry
registerGenericCU8toCS16(SOAPY_SDR_CU8, SOAPY_SDR_CS16, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<uint8_t, int16_t, 2>);
static SoapySDR::ConverterRegistry
registerGenericCS16toCU8(SOAPY_SDR_CS16, SOAPY_SDR_CU8, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<int16_t, uint8_t, 2>);

static SoapySDR::ConverterRegistry
registerGenericCU8toCS8(SOAPY_SDR_CU8, SOAPY_SDR_CS8, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<uint8_t, int8_t, 2>);
static SoapySDR::ConverterRegistry
registerGenericCS8toCU8(SOAPY_SDR_CS8, SOAPY_SDR_CU8, SoapySDR::ConverterRegistry::GENERIC,
			(SoapySDR::ConverterRegistry::ConverterFunction)&formatConverter<int8_t, uint8_t, 2>);

