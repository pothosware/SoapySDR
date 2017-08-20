// Copyright (c) 2017 Coburn Wightman
// SPDX-License-Identifier: BSL-1.0

#include "convertPrimatives.hpp"
#include <SoapySDR/Convert.hpp>
//#include <SoapySDR/Device.hpp>
#include <iostream>
#include <cstdlib>

bool fillBuffer(int8_t*, int, std::string);
bool dumpBuffer(int8_t*, int, std::string);
int readStream(void*, void * const *, const size_t);
int acquireReadBuffer(void *, size_t &, const void **);
void releaseReadBuffer(void *,  const size_t);

SoapySDR::ConvertFunction cvtCF32toCF32(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  size_t elemDepth = 2;

  std::cout << "converting CF32 to CF32" << std::endl;

  if (scaler == 1.0)
    {
      size_t elemSize = 4;
      std::memcpy(dstBuff, srcBuff, numElems*elemSize*elemDepth);
      std::cout << " straight memcpy" << std::endl;
    }
  else
    {
      float sf = float(1.0/scaler);
      float *dst = (float*)dstBuff;
      float *src = (float*)srcBuff;
      for (size_t i = 0; i < numElems*elemDepth; i+=elemDepth)
	{
	  // dst[i] = src[i] * sf;
	  // dst[i+1] = src[i+1] * sf;
	  CF32toCF32(&src[i], &dst[i], sf);
	}
      std::cout << " sample copy with scaler" << std::endl;
    }
  return 0;
}

SoapySDR::ConvertFunction cvtCS16toCF32(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  size_t elemDepth = 2;

  //  std::cout << "converting CS16 to CF32" << std::endl;

  float sf = float(1.0/scaler);
  float *dst = (float*)dstBuff;
  int16_t *src = (int16_t*)srcBuff;
  for (size_t i = 0; i < numElems*elemDepth; i+=elemDepth)
    {
      CS16toCF32(&src[i], &dst[i], sf);
    }
  //  std::cout << " sample copy with scaler" << std::endl;
  
  return 0;
}

SoapySDR::ConvertFunction cvtCU16toCF32(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  size_t elemDepth = 2;

  std::cout << "converting CU16 to CF32" << std::endl;

  float sf = float(1.0/scaler);
  float *dst = (float*)dstBuff;
  uint16_t *src = (uint16_t*)srcBuff;
  for (size_t i = 0; i < numElems*elemDepth; i+=elemDepth)
    {
      //dst[i] = src[i] - 0x7fff) * scaler;
      CU16toCF32(src, dst, sf);
    }
  std::cout << " sample copy with scaler" << std::endl;
  
  return 0;
}

// ******************************

struct Stream{
  size_t numChans;
  size_t elemDepth;
  //  vector chanList;
};

struct CS16{
  int16_t i;
  int16_t q;
};

struct CF32{
  float i;
  float q;
};


int main(void)
{
  const size_t numElems = 64;

  struct Stream stream;
  stream.numChans = 8;
  stream.elemDepth = 2;
  
  std::cout << "registering converters..." << std::endl;
  SoapySDR::registerConverter(SOAPY_SDR_CF32, SOAPY_SDR_CF32, (SoapySDR::ConvertFunction) cvtCF32toCF32);
  SoapySDR::registerConverter(SOAPY_SDR_CS16, SOAPY_SDR_CF32, (SoapySDR::ConvertFunction) cvtCS16toCF32);
  SoapySDR::registerConverter(SOAPY_SDR_CU16, SOAPY_SDR_CF32, (SoapySDR::ConvertFunction) cvtCU16toCF32);

  struct CF32* buffs[8];
  struct CF32 buffer[stream.numChans][numElems];

  for (size_t i = 0; i < stream.numChans; i++){
    buffs[i] = buffer[i];
  }
  
  size_t elemCount = readStream((void *)&stream, (void**)buffs, numElems);
  
  //dumpBuffer((char*)soapyBuffer, numElems, outElemFormat);
  
  std::cout << "DONE!" << std::endl;
  return elemCount; //EXIT_SUCCESS;
}

/*!
 * Read elements from a stream for reception.
 * This is a multi-channel call, and buffs should be an array of void *,
 * where each pointer will be filled with data from a different channel.
 *
 * **Client code compatibility:**
 * The readStream() call should be well defined at all times,
 * including prior to activation and after deactivation.
 * When inactive, readStream() should implement the timeout
 * specified by the caller and return SOAPY_SDR_TIMEOUT.
 *
 * \param stream the opaque pointer to a stream handle
 * \param buffs an array of void* buffers num chans in size
 * \param numElems the number of elements in each buffer
 * \param flags optional flag indicators about the result
 * \param timeNs the buffer's timestamp in nanoseconds
 * \param timeoutUs the timeout in microseconds
 * \return the number of elements read per buffer or error code
 */
int readStream(void* stream, void * const *buffs, const size_t numElems)
{
  struct Stream* myStream = (struct Stream*)stream;  
  
  std::string inElemFormat = SOAPY_SDR_CS16;
  std::string outElemFormat = SOAPY_SDR_CF32;
  SoapySDR::ConvertFunction convert = SoapySDR::getConverter(inElemFormat, outElemFormat);
  
  size_t handle;
  const void *payload[8];

  size_t elemOffset = 0;
  while (elemOffset < numElems)
    {
      size_t elemCount = acquireReadBuffer(stream, handle, payload);
      for (size_t chan = 0; chan < myStream->numChans; chan++)
	{
	  struct CF32* toBuff = (struct CF32*) buffs[chan];
	  struct CS16* fromBuff = (struct CS16*) payload[chan];
	  convert(&fromBuff[0], &toBuff[elemOffset], elemCount, 1);
	}
      elemOffset += elemCount;
      releaseReadBuffer(stream, handle);
    }

  for (size_t chan = 0; chan < myStream->numChans; chan++)
    {
      std::cout << "readStream(): chan " << chan << " " << outElemFormat << std::endl; 
      dumpBuffer((int8_t*)buffs[chan], numElems, outElemFormat);
    }
  
  return numElems;
}

/*!
 * Acquire direct buffers from a receive stream.
 * This call is part of the direct buffer access API.
 *
 * The buffs array will be filled with a stream pointer for each channel.
 * Each pointer can be read up to the number of return value elements.
 *
 * The handle will be set by the implementation so that the caller
 * may later release access to the buffers with releaseReadBuffer().
 * Handle represents an index into the internal scatter/gather table
 * such that handle is between 0 and num direct buffers - 1.
 *
 * \param stream the opaque pointer to a stream handle
 * \param handle an index value used in the release() call
 * \param buffs an array of void* buffers num chans in size
 * \param flags optional flag indicators about the result
 * \param timeNs the buffer's timestamp in nanoseconds
 * \param timeoutUs the timeout in microseconds
 * \return the number of elements read per buffer or error code
 */

int acquireReadBuffer(void *stream, size_t &handle, const void **buffs)
{
  const size_t numElems = 16;
  const size_t numChans = 8;
  const size_t elemDepth = 2;
  static int cycleCount = 0;
  
  struct Stream* myStream = (struct Stream*)stream;  

  static struct CS16 devBuffer[numChans][numElems];

  // fabricate a dummy buffer.
  for (size_t chan = 0; chan < numChans; chan++){
    for (size_t elem = 0; elem < numElems; elem++){
      devBuffer[chan][elem].i = cycleCount * 1000 + chan*100 + elem;
      devBuffer[chan][elem].q = -(cycleCount * 1000 + chan*100 + elem);
    }
    buffs[chan] = devBuffer[chan];
  }

  ++cycleCount;
  handle = 0;

  std::cout << "acquireReadBuffer(): " << SOAPY_SDR_CS16 << std::endl; 
  dumpBuffer((int8_t*)devBuffer, numChans*numElems, SOAPY_SDR_CS16);
  
  return numElems;
}


/*!
 * Release an acquired buffer back to the receive stream.
 * This call is part of the direct buffer access API.
 *
 * \param stream the opaque pointer to a stream handle
 * \param handle the opaque handle from the acquire() call
 */

void releaseReadBuffer(void *stream,  const size_t handle)
{
  struct Stream* myStream = (struct Stream*)stream;  
}

bool fillBuffer(int8_t* buffer, int numElems, std::string elemFormat){
  size_t elemDepth = 2;

  if (elemFormat == SOAPY_SDR_CF32){
    float* buff = (float*)buffer;
    for (size_t i = 0; i < numElems*elemDepth; i+=elemDepth)
      {
	buff[i] = (float)i;
	buff[i+1] = (float)i+1;
      }
  }
  else if (elemFormat == SOAPY_SDR_CS32){
    int32_t* buff = (int32_t*)buffer;
    for (size_t i = 0; i < numElems*elemDepth; i+=elemDepth)
      {
	buff[i] = i;
	buff[i+1] = i+1;
      }
  }
  else if (elemFormat == SOAPY_SDR_CS16){
    int16_t* buff = (int16_t*)buffer;
    for (size_t i = 0; i < numElems*elemDepth; i+=elemDepth)
      {
	buff[i] = i;
	buff[i+1] = i+1;
      }
  }
  else
    std::cout << "unrecognized elem format" << std::endl;

  return true;
}

bool dumpBuffer(int8_t* buffer, int numElems, std::string elemFormat){
  size_t elemDepth = 2;

  if (elemFormat == SOAPY_SDR_CF32){
    float* buff = (float*)buffer;
    for (size_t i = 0; i < numElems*elemDepth; i+=elemDepth)
      {
	if (i%16 == 0)
	  std::cout << std::endl << i << " > ";
	
	std::cout << ' ' << buff[i] << ":" << buff[i+1] << ", ";   
      }
  }
  else if (elemFormat == SOAPY_SDR_CS32){
    int32_t* buff = (int32_t*)buffer;
    for (size_t i = 0; i < numElems*elemDepth; i+=elemDepth)
      {
	if (i%16 == 0)
	  std::cout << std::endl << i << " > ";

	std::cout << ' ' << buff[i] << ":" << buff[i+1] << ", ";   
      }
  }
  else if (elemFormat == SOAPY_SDR_CS16){
    int16_t* buff = (int16_t*)buffer;
    for (size_t i = 0; i < numElems*elemDepth; i+=elemDepth)
      {
	if (i%16 == 0)
	  std::cout << std::endl << i << " > ";

	std::cout << ' ' << buff[i] << ":" << buff[i+1] << ", ";   
      }
  }
  else
    std::cout << "unrecognized elem format" << std::endl;

  std::cout << std::endl;
  
  return true;
}
