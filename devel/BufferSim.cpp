// Copyright (c) 2017 Coburn Wightman
// SPDX-License-Identifier: BSL-1.0

//#include "buffer.hpp"
#include "convertPrimatives.hpp"
#include <SoapySDR/Convert.hpp>
//#include <SoapySDR/Device.hpp>
#include <iostream>
#include <cstdlib>
#include <vector>

bool fillBuffer(int8_t*, int, std::string);
bool dumpBuffer(int8_t*, int, std::string);
int readStream(void*, void * const *, const size_t);
int acquireReadBuffer(void *, size_t &, const void **);
void releaseReadBuffer(void *,  const size_t);
std::string getNativeStreamFormat(const int, const size_t, double &);
std::vector<std::string> getStreamFormats(const int, const size_t);
struct Stream *setupStream(const int, const std::string &, const std::vector<size_t> &);

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

SoapySDR::ConvertFunction cvtCS16toCS16(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  size_t elemDepth = 2;

  //  std::cout << "converting CS16 to CS16" << std::endl;

  float sf = float(1.0/scaler);
  int16_t *dst = (int16_t*)dstBuff;
  int16_t *src = (int16_t*)srcBuff;
  for (size_t i = 0; i < numElems*elemDepth; i+=elemDepth)
    {
      CS16toCS16(&src[i], &dst[i], sf);
    }
  //  std::cout << " sample copy with scaler" << std::endl;
  
  return 0;
}

SoapySDR::ConvertFunction cvtCS16toCS32(const void *srcBuff, void *dstBuff, const size_t numElems, const double scaler)
{
  size_t elemDepth = 2;

  //  std::cout << "converting CS16 to CS32" << std::endl;

  float sf = float(1.0/scaler);
  int32_t *dst = (int32_t*)dstBuff;
  int16_t *src = (int16_t*)srcBuff;
  for (size_t i = 0; i < numElems*elemDepth; i+=elemDepth)
    {
      CS16toCS32(&src[i], &dst[i], sf);
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


struct Stream{
  size_t numChans;
  //  vector chanList;

  SoapySDR::ConvertFunction converter;
  std::string streamElemFormat;
  size_t streamElemWidth;
  size_t streamElemDepth;
  size_t streamElemSize;
 
  std::string nativeElemFormat;
  size_t nativeElemWidth;
  size_t nativeElemDepth;
  size_t nativeElemSize;

  size_t fragmentRemaining;
  size_t fragmentOffset;
  size_t fragmentHandle;
  const void *fragmentBuffs[8];
};

struct CS16{
  int16_t i;
  int16_t q;
};

struct CF32{
  float i;
  float q;
};


struct Stream _stream;

int main(void)
{

  std::cout << "registering converters..." << std::endl;
  SoapySDR::registerConverter(SOAPY_SDR_CS16, SOAPY_SDR_CS16, (SoapySDR::ConvertFunction) cvtCS16toCS16);
  SoapySDR::registerConverter(SOAPY_SDR_CS16, SOAPY_SDR_CS32, (SoapySDR::ConvertFunction) cvtCS16toCS32);
  SoapySDR::registerConverter(SOAPY_SDR_CS16, SOAPY_SDR_CF32, (SoapySDR::ConvertFunction) cvtCS16toCF32);

  double fs;
  std::string nativeFormat = getNativeStreamFormat(0,1,fs);
  std::cout << "native format: " << nativeFormat << std::endl;

  std::cout << std::endl << "supported stream formats for " << nativeFormat << std::endl;
  std::vector<std::string> formats = SoapySDR::convertTargetFormats(nativeFormat);
  for(std::vector<std::string>::iterator it = formats.begin() ; it != formats.end(); ++it)
    {
      std::cout << " " << *it << std::endl;
    }

  const size_t numElems = 100; //64
  std::vector<size_t>channels;
  
  struct Stream* myStream = setupStream(0, SOAPY_SDR_CF32, channels );
  
  struct CF32* buffs[8];
  struct CF32 buffer[myStream->numChans][numElems];

  for (size_t chan = 0; chan < myStream->numChans; chan++){
    buffs[chan] = buffer[chan];
  }
  
  size_t elemCount = readStream((void *)myStream, (void**)buffs, numElems);

  for (size_t chan = 0; chan < myStream->numChans; chan++)
    {
      std::cout << "readStream() result: chan" << chan << " " << myStream->streamElemFormat << std::endl; 
      dumpBuffer((int8_t*)buffs[chan], elemCount, myStream->streamElemFormat);
    }
  
  std::cout << "DONE!" << std::endl;
  return EXIT_SUCCESS;
}

/*!
 * Query a list of the available stream formats.
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \return a list of allowed format strings. See setupStream() for the format syntax.
 */
std::vector<std::string> getStreamFormats(const int direction, const size_t channel) //const
{
  double fs;
  
  const std::string sourceFormat = getNativeStreamFormat(direction, channel, fs);
  std::vector<std::string> formats = SoapySDR::convertTargetFormats(sourceFormat);

  return formats;
}

/*!
 * Get the hardware's native stream format for this channel.
 * This is the format used by the underlying transport layer,
 * and the direct buffer access API calls (when available).
 * \param direction the channel direction RX or TX
 * \param channel an available channel on the device
 * \param [out] fullScale the maximum possible value
 * \return the native stream buffer format string
 */
std::string getNativeStreamFormat(const int direction, const size_t channel, double &fullScale) //const
{
  fullScale = 3.3/2;
  return SOAPY_SDR_CS16;
}

struct Stream *setupStream(const int direction, const std::string &format, const std::vector<size_t> &channels)
{
  struct Stream* stream = &_stream;
  double fs;

  stream->numChans = 8;

  stream->nativeElemFormat = getNativeStreamFormat(direction, 0, fs);
  stream->nativeElemWidth = 2; //bytes
  stream->nativeElemDepth = 2; //samples
  stream->nativeElemSize = stream->nativeElemWidth * stream->nativeElemDepth; 
  
  stream->streamElemFormat = format;
  stream->streamElemWidth = 4; //bytes
  stream->streamElemDepth = 2; //samples
  stream->streamElemSize = stream->streamElemWidth * stream->streamElemDepth;
  
  stream->converter = SoapySDR::getConverter(stream->nativeElemFormat, stream->streamElemFormat);

  stream->fragmentRemaining = 0;
  stream->fragmentOffset = 0;
  //  size_t fragmentHandle;

  return stream;
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
  
  size_t streamOffset = 0;
  size_t bufferRemaining = numElems;
  while (streamOffset < numElems)
    {
      if (myStream->fragmentRemaining == 0)
	{
	myStream->fragmentRemaining = acquireReadBuffer(stream, myStream->fragmentHandle, myStream->fragmentBuffs);
	}
      size_t elemCount = (bufferRemaining < myStream->fragmentRemaining) ? bufferRemaining : myStream->fragmentRemaining;
      for (size_t chan = 0; chan < myStream->numChans; chan++)
	{
	  int8_t* streamBuffer = (int8_t*) buffs[chan];
	  int8_t* fragmentBuffer = (int8_t*) myStream->fragmentBuffs[chan];
	  myStream->converter(&fragmentBuffer[myStream->fragmentOffset * myStream->nativeElemSize],
			      &streamBuffer[streamOffset * myStream->streamElemSize], elemCount, 1);
	}
      streamOffset += elemCount;
      bufferRemaining -= elemCount;
      
      myStream->fragmentOffset += elemCount;
      myStream->fragmentRemaining -= elemCount;
      
      std::cout << myStream->fragmentRemaining << std::endl;
      if (myStream->fragmentRemaining == 0)
	{
	  myStream->fragmentOffset = 0;
	  releaseReadBuffer(stream, myStream->fragmentHandle);
	}
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
  const size_t numElems = 12; //16
  const size_t numChans = 8;
  static int cycleCount = 0;
  
  struct Stream* myStream = (struct Stream*)stream;  
  std::cout << "acquireReadBuffer(): " << myStream->nativeElemFormat << std::endl; 

  // fabricate a dummy device buffer.
  static struct CS16 devBuffer[numChans][numElems];
  for (size_t chan = 0; chan < numChans; chan++){
    for (size_t elem = 0; elem < numElems; elem++){
      devBuffer[chan][elem].i = cycleCount * 1000 + chan*100 + elem;
      devBuffer[chan][elem].q = -(cycleCount * 1000 + chan*100 + elem);
    }
    buffs[chan] = devBuffer[chan];
  }

  ++cycleCount;
  handle = 0;

  dumpBuffer((int8_t*)devBuffer, numChans*numElems, myStream->nativeElemFormat);
  
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
  std::cout << "releaseReadBuffer(): " << myStream->nativeElemFormat << std::endl; 
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
