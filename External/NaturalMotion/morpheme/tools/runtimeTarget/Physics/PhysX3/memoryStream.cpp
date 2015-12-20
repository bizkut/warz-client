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
#include "NMPlatform/NMFile.h"

#include "physics/PhysX3/mrPhysX3Includes.h"
#include "memoryStream.h"
//----------------------------------------------------------------------------------------------------------------------

namespace phx
{

MemoryWriteBuffer::MemoryWriteBuffer() : currentSize(0), maxSize(0), data(NULL)
{
}

MemoryWriteBuffer::~MemoryWriteBuffer()
{
  delete []data;
}

physx::PxU32 MemoryWriteBuffer::write(const void* src, physx::PxU32 count)
{
  physx::PxU32 expectedSize = currentSize + count;
  if (expectedSize > maxSize)
  {
    maxSize = expectedSize + 4096;

    physx::PxU8* newData = new physx::PxU8[maxSize];
    NMP_ASSERT(newData != NULL);

    if (data)
    {
      memcpy(newData, data, currentSize);
      delete[] data;
    }
    data = newData;
  }

  memcpy(data + currentSize, src, count);
  currentSize += count;
  return count;
}

MemoryReadBuffer::MemoryReadBuffer(const physx::PxU8* data) : buffer(data)
{
}

MemoryReadBuffer::~MemoryReadBuffer()
{
  // We don't own the data => no delete
}

physx::PxU32 MemoryReadBuffer::read(void* dest, physx::PxU32 count)
{
  memcpy(dest, buffer, count);
  buffer += count;
  return count;
}

// UserStream: files
//----------------------------------------------------------------------------------------------------------------------
FileStream::FileStream(const char* filename, bool load) : fp(NULL)
{
  NM_FOPEN(fp, filename, load ? "rb" : "wb");
}

FileStream::~FileStream()
{
  if(fp)
    fclose(fp);
}

physx::PxU32 FileStream::read(void* dest, physx::PxU32 count)
{
  size_t w = fread(dest, count, 1, fp);
  NMP_ASSERT(w);
  return (physx::PxU32) w;
}

physx::PxU32 FileStream::write(const void* src, physx::PxU32 count)
{
  size_t w = fwrite(src, count, 1, fp);
  NMP_ASSERT(w);
  return (physx::PxU32) w;
}

} // namespace phx

//----------------------------------------------------------------------------------------------------------------------