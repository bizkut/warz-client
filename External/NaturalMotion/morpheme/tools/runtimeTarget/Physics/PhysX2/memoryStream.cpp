// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"

#include <stdio.h>

#include "mrPhysX2Includes.h"

#include "memoryStream.h"
//----------------------------------------------------------------------------------------------------------------------

namespace phx
{

MemoryWriteBuffer::MemoryWriteBuffer() : currentSize(0), maxSize(0), data(NULL)
{
}

MemoryWriteBuffer::~MemoryWriteBuffer()
{
  NX_DELETE_ARRAY(data);
}

void MemoryWriteBuffer::clear()
{
  currentSize = 0;
}

NxStream& MemoryWriteBuffer::storeByte(NxU8 b)
{
  storeBuffer(&b, sizeof(NxU8));
  return *this;
}

NxStream& MemoryWriteBuffer::storeWord(NxU16 w)
{
  storeBuffer(&w, sizeof(NxU16));
  return *this;
}

NxStream& MemoryWriteBuffer::storeDword(NxU32 d)
{
  storeBuffer(&d, sizeof(NxU32));
  return *this;
}

NxStream& MemoryWriteBuffer::storeFloat(NxReal f)
{
  storeBuffer(&f, sizeof(NxReal));
  return *this;
}

NxStream& MemoryWriteBuffer::storeDouble(NxF64 f)
{
  storeBuffer(&f, sizeof(NxF64));
  return *this;
}

NxStream& MemoryWriteBuffer::storeBuffer(const void* buffer, NxU32 size)
{
  NxU32 expectedSize = currentSize + size;
  if (expectedSize > maxSize)
  {
    maxSize = expectedSize + 4096;

    NxU8* newData = new NxU8[maxSize];
    NX_ASSERT(newData != NULL);

    if (data)
    {
      memcpy(newData, data, currentSize);
      delete[] data;
    }
    data = newData;
  }

  memcpy(data + currentSize, buffer, size);
  currentSize += size;
  return *this;
}

MemoryReadBuffer::MemoryReadBuffer(const NxU8* data) : buffer(data)
{
}

MemoryReadBuffer::~MemoryReadBuffer()
{
  // We don't own the data => no delete
}

NxU8 MemoryReadBuffer::readByte() const
{
  NxU8 b;
  memcpy(&b, buffer, sizeof(NxU8));
  buffer += sizeof(NxU8);
  return b;
}

NxU16 MemoryReadBuffer::readWord() const
{
  NxU16 w;
  memcpy(&w, buffer, sizeof(NxU16));
  buffer += sizeof(NxU16);
  return w;
}

NxU32 MemoryReadBuffer::readDword() const
{
  NxU32 d;
  memcpy(&d, buffer, sizeof(NxU32));
  buffer += sizeof(NxU32);
  return d;
}

float MemoryReadBuffer::readFloat() const
{
  float f;
  memcpy(&f, buffer, sizeof(float));
  buffer += sizeof(float);
  return f;
}

double MemoryReadBuffer::readDouble() const
{
  double f;
  memcpy(&f, buffer, sizeof(double));
  buffer += sizeof(double);
  return f;
}

void MemoryReadBuffer::readBuffer(void* dest, NxU32 size) const
{
  memcpy(dest, buffer, size);
  buffer += size;
}

} // namespace phx

//----------------------------------------------------------------------------------------------------------------------