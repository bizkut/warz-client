// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#ifndef MEMORY_STREAM_H
#define MEMORY_STREAM_H

#include "PxIO.h"
#include "NMPlatform/NMPlatform.h"

namespace phx
{

class MemoryWriteBuffer : public physx::PxOutputStream
{
public:
  MemoryWriteBuffer();
  virtual ~MemoryWriteBuffer();

  virtual physx::PxU32 write(const void* src, physx::PxU32 count) NM_OVERRIDE;

  physx::PxU32 currentSize;
  physx::PxU32 maxSize;
  physx::PxU8* data;
};

class MemoryReadBuffer : public physx::PxInputStream
{
public:
  MemoryReadBuffer(const physx::PxU8* data);
  virtual ~MemoryReadBuffer();

  virtual physx::PxU32 read(void* dest, physx::PxU32 count) NM_OVERRIDE;

  mutable const physx::PxU8* buffer;
};

class FileStream : public physx::PxInputStream, public physx::PxOutputStream
{
public:
  FileStream(const char* filename, bool load);
  virtual ~FileStream();

  virtual physx::PxU32 write(const void* src, physx::PxU32 count) NM_OVERRIDE;
  virtual physx::PxU32 read(void* dest, physx::PxU32 count) NM_OVERRIDE;

  FILE* fp;
};

} // namespace phx
#endif
