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

#include "NxStream.h"

namespace phx
{
class MemoryWriteBuffer : public NxStream
{
public:
  MemoryWriteBuffer();
  virtual ~MemoryWriteBuffer();
  void  clear();

  virtual NxU8    readByte()                const { return 0; }
  virtual NxU16   readWord()                const { return 0; }
  virtual NxU32   readDword()               const { return 0; }
  virtual float   readFloat()               const { return 0.0f;  }
  virtual double  readDouble()              const { return 0.0; }
  virtual void    readBuffer(void*, NxU32)  const { }

  virtual NxStream& storeByte(NxU8 b);
  virtual NxStream& storeWord(NxU16 w);
  virtual NxStream& storeDword(NxU32 d);
  virtual NxStream& storeFloat(NxReal f);
  virtual NxStream& storeDouble(NxF64 f);
  virtual NxStream& storeBuffer(const void* buffer, NxU32 size);

  NxU32 currentSize;
  NxU32 maxSize;
  NxU8* data;
};

class MemoryReadBuffer : public NxStream
{
public:
  MemoryReadBuffer(const NxU8* data);
  virtual ~MemoryReadBuffer();

  virtual NxU8    readByte() const;
  virtual NxU16   readWord() const;
  virtual NxU32   readDword() const;
  virtual float   readFloat() const;
  virtual double  readDouble() const;
  virtual void    readBuffer(void* buffer, NxU32 size) const;

  virtual NxStream& storeByte(NxU8)                 { return *this; }
  virtual NxStream& storeWord(NxU16)                { return *this; }
  virtual NxStream& storeDword(NxU32)               { return *this; }
  virtual NxStream& storeFloat(NxReal)              { return *this; }
  virtual NxStream& storeDouble(NxF64)              { return *this; }
  virtual NxStream& storeBuffer(const void*, NxU32) { return *this; }

  mutable const NxU8* buffer;
};
} // namespace phx
#endif
